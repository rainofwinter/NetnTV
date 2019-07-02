#include "stdafx.h"
#include "InternalSubscene.h"
#include "CatalogDocumentTemplate.h"
#include "Image.h"
#include "Texture.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Scene.h"
#include "Xml.h"
#include "Reader.h"
#include "Camera.h"
#include "GLUtils.h"
#include "ElementMapping.h"
#include "FileUtils.h"
#include "Exception.h"
#include "Document.h"
#include "Global.h"
#include "Root.h"
#include "AppImage.h"
#include "LoadingPage.h"
#include "ScriptProcessor.h"

#include "SceneChanger.h"

#ifdef USE_FBO
#include "TextureRenderer.h"
#endif

#include "VideoPlayer.h"
#include "AudioPlayer.h"

#include "PageChangingEvent.h"

#ifdef MSVC
#include <boost/thread.hpp>
#else
#include <unistd.h>
#endif


using namespace std;

#if defined(MSVC) && !defined(ES_EMULATOR)

const char gShaderSrc[] =
"uniform vec4 Color;\n\
varying vec2 vTexCoord;\n\
varying vec2 vPosition;\n\
uniform sampler2D tex0;\n\
\n\
void main()\n\
{\
gl_FragColor = texture2D(tex0, vTexCoord.st);\n\
gl_FragColor.a = 1.0;\n\
}\n\
";

#else

const char gShaderSrc[] =
"precision mediump float;\n\
uniform vec4 Color;\n\
varying vec2 vTexCoord;\n\
varying vec2 vPosition;\n\
uniform sampler2D tex0;\n\
\n\
void main()\n\
{\
gl_FragColor = texture2D(tex0, vTexCoord.st);\n\
float t = (abs(vPosition.x) - 0.15)/(1.0 - 0.15);\n\
float alpha = clamp(1.0 - t, 0.0, 1.0);\n\
t = (abs(vPosition.y) - 0.175)/(0.875 - 0.175);\n\
alpha *= clamp(1.0 - t, 0.0, 1.0);\n\
gl_FragColor.a = alpha;\n\
}\n\
";

#endif


const char gVertShaderSrc[] =
"\
attribute vec4 position;\
attribute vec2 texCoord;\
uniform mat4 modelViewProjectionMatrix;\
\
varying vec2 vTexCoord;\
varying vec2 vPosition;\
\
void main()\
{\
\
gl_Position = modelViewProjectionMatrix*position;\
vTexCoord = texCoord;\
vPosition.x = gl_Position.x/gl_Position.w;\
vPosition.y = gl_Position.y/gl_Position.w;\
}\
";

const char * ctbookmarksFile = "__ctbookmarks.dat";

///////////////////////////////////////////////////////////////////////////////

void CatalogDocumentTemplate::BookmarkData::write(Writer & writer) const
{
	writer.write(pageIndex);
	writer.write(filename);
	writer.write(time);
}
void CatalogDocumentTemplate::BookmarkData::read(Reader & reader, unsigned char version)
{
	reader.read(pageIndex);
	reader.read(filename);
	if (version >= 1)
		reader.read(time);
	else
		time = "00000000000000";
}

void CatalogDocumentTemplate::BookmarkData::writeXml(XmlWriter & w) const
{
	w.writeTag("PageIndex", pageIndex);
	w.writeTag("Filename", filename);
	w.writeTag("Time", time);
}

void CatalogDocumentTemplate::BookmarkData::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(pageIndex, curNode, "PageIndex"));
		else if (r.getNodeContentIfName(filename, curNode, "Filename"));
		else if (r.getNodeContentIfName(time, curNode, "Time"));
	}
}
///////////////////////////////////////////////////////////////////////////////


CatalogDocumentTemplate::LandscapeSubscenes::LandscapeSubscenes()
{
	leftpage_.reset(new InternalSubscene);
	leftpage_->setHasLeft(true);
	leftpage_->setHasRight(true);
	leftpage_->setViewState(InternalSubscene::LandscapeLeft);
	//leftpage_->setHasBottom(true);
	//leftpage_->setHasTop(true);
	rightpage_.reset(new InternalSubscene);
	rightpage_->setHasLeft(true);
	rightpage_->setHasRight(true);
	rightpage_->setViewState(InternalSubscene::LandscapeRight);
	//rightpage_->setHasBottom(true);
	//rightpage_->setHasTop(true);
}

void CatalogDocumentTemplate::LandscapeSubscenes::setLeftpage(Scene* scene)
{
	leftpage_->setScene(scene);
}
void CatalogDocumentTemplate::LandscapeSubscenes::setRightpage(Scene* scene)
{
	rightpage_->setScene(scene);
}
bool CatalogDocumentTemplate::LandscapeSubscenes::correctRightPageTranslation()
{
	Transform lt = leftpage_->transform();
	Transform rt = rightpage_->transform();
	Vector3 prevpos = rt.translation();
	Matrix lm = lt.computeMatrix();
	Matrix rm = rt.computeMatrix();

	float wid = leftpage_->windowWidth();
	float scax = lt.scaling().x;
	float norsx = leftpage_->normalScaleX();
	float temp = leftpage_->windowWidth() * lt.scaling().x / leftpage_->normalScaleX();
	rm._14 = lm._14 + temp;
	rt.setMatrix(rm, true);
	rightpage_->setTransform(rt);

	return rt.translation() == prevpos;
}


///////////////////////////////////////////////////////////////////////////////
void CatalogDocumentTemplate::create()
{
	init_ = false;
	thumbnailScale_ = 0.125f;
	gapFactor_ = 0.24f;
	layoutX_ = 0;
	redrawTriggered_ = false;

	isOpenedThumb_ = false;

	resetComplete_ = false;
	searchQ_.clear();

	scrollPos_ = 0;

	width_ = 768;
	height_ = 1024;

	subscene_.reset(new InternalSubscene);
	subscene_->setHasLeft(true);
	subscene_->setHasRight(true);
	//subscene_->setHasBottom(true);
	//subscene_->setHasTop(true);
	for(int i = 0 ; i < SUBSCENECOUNT ; i++)
		subsceneCached_[i].reset(new InternalSubscene);

	landSubscene_ = new LandscapeSubscenes;
	for(int i = 0 ; i < 2 ; i++)
		landSubsceneCached_[i] = new LandscapeSubscenes;

	pageIndex_ = 0;

	uiImgFiles_.push_back(&topMenuLayoutButtonFile_);
	//uiImgFiles_.push_back(&thumbnailNormalOpenedFile_);
	//uiImgFiles_.push_back(&thumbnailNormalClosedFile_);
	//uiImgFiles_.push_back(&thumbnailWidthOpenedFile_);
	//uiImgFiles_.push_back(&thumbnailWidthClosedFile_);
	uiImgFiles_.push_back(&thumbnailNormalBarFile_);
	uiImgFiles_.push_back(&thumbnailWidthBarFile_);
	uiImgFiles_.push_back(&thumbnailNormalClosedFile_);
	uiImgFiles_.push_back(&thumbnailNormalOpenedFile_);
	uiImgFiles_.push_back(&bookmarkCheckFile_);
	uiImgFiles_.push_back(&thumbnailShadowFile_);
	uiImgFiles_.push_back(&thumbnailWidthClosedFile_);
	uiImgFiles_.push_back(&thumbnailWidthOpenedFile_);
	//uiImgFiles_.push_back(&toContentsFile_);

	menuLayoutButton_.reset(new Image);
	VisualAttrib * attr = menuLayoutButton_->visualAttrib();
	attr->setOpacity(1.0f);

	for (int i = 0; i < SUBSCENECOUNT+7; ++i)
		subSceneBackup_.push_back(new InternalSubscene);


	//thumbnailNormalClosed_.reset(new Texture);
	//thumbnailNormalOpened_.reset(new Texture);
	//thumbnailWidthClosed_.reset(new Texture);
	//thumbnailWidthOpened_.reset(new Texture);
	thumbnailNormal_.reset(new Texture);
	thumbnailWidth_.reset(new Texture);
	thumbnailOpened_.reset(new Texture);
	thumbnailClosed_.reset(new Texture);
	bookmarkCheck_.reset(new Texture);
	thumbnailShadow_.reset(new Texture);
	thumbnailWidthClosed_.reset(new Texture);
	thumbnailWidthOpened_.reset(new Texture);
	//scrollBarBg_.reset(new Texture);
	//scrollBarLeft_.reset(new Texture);
	//scrollBarRight_.reset(new Texture);
	//scrollBarThumb_.reset(new Image);	

	moveDirection_ = Undetermined;
	scrollThumbGrabbed_ = false;

	pageChangingMsg_ = "pageChanging";

	pageIndex_ = -1;

	displayingAppObjects_ = false;

	thumbsLoadedConfirmed_ = false;
#ifdef USE_FBO
	texRenderer_ = new TextureRenderer;
	renderTex_ = new Texture;
#else
	ssBuffer_ = 0;
	ssTexture_ = 0;
#endif

	layoutOffsetY_ = height_ - 64 * dpiScale_;

	//loading page
	loadingPage_.reset(new LoadingScreen);
	curPageStarted_ = false;


	layoutModeSnappedBackYet_ = false;
	requiredLayoutMoveDirection_ = Undetermined;
	startLayoutOffsetY_ = 0.0f;

	preserveAspect_ = true;	
	pageMovingId_ = -1;

	isLandscape_ = false;
	landscapeMovingTranslation_ = 0;

	opacity_ = 0.0f;
	fadeIn_ = false;
	fadeOut_ = false;
}

void CatalogDocumentTemplate::doCurPageStart()
{
	curPageStarted_ = false;

	if (isLandscape_)
	{
		if (landSubscene_->leftpage_->isInit()) landSubscene_->leftpage_->asyncLoadUpdate();
		if (landSubscene_->rightpage_->isInit()) landSubscene_->rightpage_->asyncLoadUpdate();
	}
	else
	{
		if (subscene_->isInit()) subscene_->asyncLoadUpdate();
	}
}

bool CatalogDocumentTemplate::pageHasBookmark(int pageIndex) const
{

	BOOST_FOREACH(const BookmarkData & bookmark, bookmarks_)
	{
		if (bookmark.pageIndex == pageIndex) return true;
	}

	return false;
}

void CatalogDocumentTemplate::getBookmarkPages(std::vector<int> * pages)
{
	BOOST_FOREACH(const BookmarkData & bookmark, bookmarks_)
	{
		pages->push_back(bookmark.pageIndex);
	}
}

void CatalogDocumentTemplate::addBookmark(const BookmarkData & data)
{
	bookmarks_.push_back(data);
	writeBookmarks();
}

std::string CatalogDocumentTemplate::currentTimeString()
{
	time_t t;
	time(&t);
	tm * tm = localtime(&t);
	char ybuf[5];
	int year = tm->tm_year + 1900;
	sprintf(ybuf, "%d", year);
	char monbuf[3];
	int mon = tm->tm_mon + 1;
	if (mon < 10)
		sprintf(monbuf, "0%d", mon);
	else
		sprintf(monbuf, "%d", mon);
	char dbuf[3];
	int day = tm->tm_mday;
	if (day < 10)
		sprintf(dbuf, "0%d", day);
	else
		sprintf(dbuf, "%d", day);
	char hbuf[3];
	int hour = tm->tm_hour;
	if (hour < 10)
		sprintf(hbuf, "0%d", hour);
	else
		sprintf(hbuf, "%d", hour);
	char mbuf[3];
	int min = tm->tm_min;
	if (min < 10)
		sprintf(mbuf, "0%d", min);
	else
		sprintf(mbuf, "%d", min);
	char sbuf[3];
	int sec = tm->tm_sec;
	if (sec < 10)
		sprintf(sbuf, "0%d", sec);
	else
		sprintf(sbuf, "%d", sec);
	char tbuf[15];
	sprintf(tbuf, "%s%s%s%s%s%s", ybuf, monbuf, dbuf, hbuf, mbuf, sbuf);
	std::string time = tbuf;


	return time;
}

bool CatalogDocumentTemplate::setCurPageBookmark()
{
	redrawTriggered_ = true;

	std::string time = currentTimeString();

	std::string filename;
	int sceneSize = scenes_.size() - 1;
	if (!isLandscape_ || (pageIndex_ == 0 || (pageIndex_%2 == 1 && pageIndex_ == sceneSize)) )
	{
		if (pageHasBookmark(pageIndex_))
		{
			removeBookmark(pageIndex_);
			return false;
		}
		else
		{
			filename = thumbFiles_.find(pageIndex_)->second;
			addBookmark(BookmarkData(pageIndex_, filename, time));
			return true;
		}
	}
	else
	//if (isLandscape_ && ((pageIndex_ != 0) && (pageIndex_%2 == 1 && pageIndex_ != sceneSize)))
	{
		if (pageIndex_ % 2 == 0)
		{
			if (pageHasBookmark(pageIndex_) && pageHasBookmark(pageIndex_-1))
			{
				removeBookmark(pageIndex_);
				removeBookmark(pageIndex_-1);
				return false;
			}
			else if (pageHasBookmark(pageIndex_))
			{
				filename = thumbFiles_.find(pageIndex_-1)->second;
				addBookmark(BookmarkData(pageIndex_-1, filename, time));
				return true;
			}
			else if (pageHasBookmark(pageIndex_-1))
			{
				filename = thumbFiles_.find(pageIndex_)->second;
				addBookmark(BookmarkData(pageIndex_, filename, time));
				return true;
			}	
			else
			{
				filename = thumbFiles_.find(pageIndex_)->second;
				addBookmark(BookmarkData(pageIndex_, filename, time));
				filename = thumbFiles_.find(pageIndex_-1)->second;
				addBookmark(BookmarkData(pageIndex_-1, filename, time));
				return true;
			}
		}
		else if (pageIndex_ % 2 == 1)
		{
			if (pageHasBookmark(pageIndex_) && pageHasBookmark(pageIndex_+1))
			{
				removeBookmark(pageIndex_);
				removeBookmark(pageIndex_+1);
				return false;
			}
			else if (pageHasBookmark(pageIndex_))
			{
				filename = thumbFiles_.find(pageIndex_+1)->second;
				addBookmark(BookmarkData(pageIndex_+1, filename, time));
				return true;
			}
			else if (pageHasBookmark(pageIndex_+1))
			{
				filename = thumbFiles_.find(pageIndex_)->second;
				addBookmark(BookmarkData(pageIndex_, filename, time));
				return true;
			}
			else
			{
				filename = thumbFiles_.find(pageIndex_)->second;
				addBookmark(BookmarkData(pageIndex_, filename, time));
				filename = thumbFiles_.find(pageIndex_+1)->second;
				addBookmark(BookmarkData(pageIndex_+1, filename, time));
				return true;
			}
		}
	}
	return true;
}

void CatalogDocumentTemplate::removeBookmark(int pageIndex)
{
	vector<BookmarkData>::iterator iter;
	for (iter = bookmarks_.begin(); iter != bookmarks_.end(); ++iter)
	{
		const BookmarkData & bookmark = *iter;
		if (bookmark.pageIndex == pageIndex)
		{
			bookmarks_.erase(iter);
			break;
		}
	}
	writeBookmarks();
}


void CatalogDocumentTemplate::setDocument(Document * document)
{
	DocumentTemplate::setDocument(document);
}

void CatalogDocumentTemplate::setSize(int width, int height, bool resizeGl)
{
	width_ = (float)width;
	height_ = (float)height;

	camera_.SetTo2DArea(width_/2, height_/2, width_, height_, 60.0f, width_/height_);

	float thumbscale;
	if (isLandscape_)
	{
		thumbscale = width_ / 2048;
	}
	else
	{
		thumbscale = width_ / 1536;
	}
    if(isOpenedThumb_)
        layoutOffsetY_ = height_ - thumbnailOpened_->height() * dpiScale_ * thumbscale - thumbnailNormal_->height() * thumbscale * dpiScale_;
    else
    {
        if (thumbnailOpened_->isLoaded() && !isLandscape_)
            layoutOffsetY_ = height_ - thumbnailOpened_->height() * dpiScale_ * thumbscale;
		else if (thumbnailWidthOpened_->isLoaded() && isLandscape_)
            layoutOffsetY_ = height_ - thumbnailWidthOpened_->height() * dpiScale_ * thumbscale;
        else
            layoutOffsetY_ = height_ - 64 * dpiScale_ * thumbscale;
    }
	//layoutOffsetY_ = height_ - 64 * thumbscale * dpiScale_;// - thumbnailNormal_->height() * thumbscale;

	setButtonTransform();

	//set subscene scaling and translation
	setSubsceneScaling();
	if (isLandscape_)
		setCurPagePos(landSubscene_->leftpage_->transform().translation());
	else
		setCurPagePos(subscene_->transform().translation());

	setScrollPos(scrollPos_);

	//setScrollBarThumbTransform();

	reInitCurScene(document_->renderer());

	if (resizeGl)
	{
		loadingPage_->init(document_->renderer(), width, height);
		static boost::uuids::uuid type = SceneChanger().type();
		for(int i = 0 ; i < document_->scenes().size() ; i++)
		{
			std::vector<SceneObject *> scenechanger;
			document_->scenes()[i]->getSceneObjectsByID(type, &scenechanger);
			BOOST_FOREACH(SceneObject * obj, scenechanger)
			{
				SceneChanger* sc = (SceneChanger*)obj;
				sc->reSize();
			}
		}
	}

#ifdef USE_FBO
	initTexRenderer(document_->renderer());
#endif
}

void CatalogDocumentTemplate::readBookmarks()
{
	std::string FileName = Global::instance().writeDirectory() + ctbookmarksFile;
	FILE * file = fopen(FileName.c_str(), "rb");

	if (file)
	{
		Reader(file).read(bookmarks_);
		fclose(file);
	}
}

void CatalogDocumentTemplate::writeBookmarks()
{
	std::string writeFileName = Global::instance().writeDirectory() + ctbookmarksFile;
	Writer(writeFileName.c_str(), 0).write(bookmarks_);
}


CatalogDocumentTemplate::CatalogDocumentTemplate()
{
	create();
}

CatalogDocumentTemplate::~CatalogDocumentTemplate()
{
	uninit();
	delete landSubscene_;
	delete landSubsceneCached_[0];
	delete landSubsceneCached_[1];
	BOOST_FOREACH(InternalSubscene * subscene, subSceneBackup_) delete subscene;

#ifdef USE_FBO
	delete texRenderer_;
	delete renderTex_;
#endif

}
Scene * CatalogDocumentTemplate::curScene() const
{
	if (isLandscape_)
		return landSubscene_->leftpage_->scene();
	else
		return subscene_->scene();
}
Scene * CatalogDocumentTemplate::nextScene() const
{
	if (pageIndex_ >= scenes_.size() - 1) return NULL;
	return scenes_[pageIndex_ + 1];
}
Scene * CatalogDocumentTemplate::prevScene() const
{
	if (pageIndex_ <= 1) return NULL;
	return scenes_[pageIndex_ - 1];
}

void CatalogDocumentTemplate::getScenes(std::vector<Scene *> & scenes) const
{
	scenes.clear();
	scenes.insert(scenes.end(), scenes_.begin(), scenes_.end());	
}
#ifdef USE_FBO

void CatalogDocumentTemplate::initTexRenderer(GfxRenderer * gl)
{
	uninitTexRenderer();
	renderTex_->init(gl, width_, height_, Texture::UncompressedRgb16, 0);
	texRenderer_->init(gl, width_, height_, true);
	texRenderer_->setTexture(renderTex_);
}

void CatalogDocumentTemplate::uninitTexRenderer()
{	
	texRenderer_->uninit();
	renderTex_->uninit();
}

#endif


void CatalogDocumentTemplate::init(GfxRenderer * gl)
{
	if (init_) uninit();

	if (!thumbFiles_.empty())
		Global::instance().requestLocalSaveObjectFirst(thumbFiles_.find(0)->second);

	//scrollBarThumb_->setFileName(thumbnailWidthClosedFile_);

	//Use ModeScrollable for clipping

	vector<InternalSubscene *> subscenes;
	subscenes.push_back(subscene_.get());
	for(int i = 0 ; i < SUBSCENECOUNT ; i++)
		subscenes.push_back(subsceneCached_[i].get());

	BOOST_FOREACH(InternalSubscene * subscene, subscenes)
	{
		subscene->setMode(InternalSubscene::ModeFixed);
		subscene->setLocked(false);

		subscene->setWindowWidth(width_);
		subscene->setWindowHeight(height_);
		subscene->init(gl);
	}	

	subscenes.clear();
	subscenes.push_back(landSubscene_->leftpage_.get());
	subscenes.push_back(landSubscene_->rightpage_.get());
	for(int i = 0 ; i < 2 ; i++)
	{
		subscenes.push_back(landSubsceneCached_[i]->leftpage_.get());
		subscenes.push_back(landSubsceneCached_[i]->rightpage_.get());
	}

	BOOST_FOREACH(InternalSubscene * subscene, subscenes)
	{
		subscene->setMode(InternalSubscene::ModeFixed);
		subscene->setLocked(false);

		//subscene->setWindowWidth(height_/2);
		//subscene->setWindowHeight((height_/2*height_)/width_);
		subscene->setWindowWidth(width_);
		subscene->setWindowHeight(height_);
		subscene->init(gl);
	}	


	subscene_->setMode(InternalSubscene::ModeFixed);

	BOOST_FOREACH(InternalSubscene * subscene, subSceneBackup_) 
	{
		subscene->init(gl);		
	}

	//thumbnailNormalClosed_->init(gl, thumbnailNormalClosedFile_, Texture::UncompressedRgba32);
	//thumbnailNormalOpened_->init(gl, thumbnailNormalOpenedFile_, Texture::UncompressedRgba32);
	//thumbnailWidthClosed_->init(gl, thumbnailWidthClosedFile_, Texture::UncompressedRgba32);
	//thumbnailWidthOpened_->init(gl, thumbnailWidthOpenedFile_, Texture::UncompressedRgba32);
	thumbnailNormal_->init(gl, thumbnailNormalBarFile_, Texture::UncompressedRgba32);
	thumbnailWidth_->init(gl, thumbnailWidthBarFile_, Texture::UncompressedRgba32);
	thumbnailOpened_->init(gl, thumbnailNormalOpenedFile_, Texture::UncompressedRgba32);
	thumbnailClosed_->init(gl, thumbnailNormalClosedFile_, Texture::UncompressedRgba32);
	bookmarkCheck_->init(gl, bookmarkCheckFile_, Texture::UncompressedRgba32);
	thumbnailShadow_->init(gl, thumbnailShadowFile_, Texture::UncompressedRgba32);
	thumbnailWidthClosed_->init(gl, thumbnailWidthClosedFile_, Texture::UncompressedRgba32);
	thumbnailWidthOpened_->init(gl, thumbnailWidthOpenedFile_, Texture::UncompressedRgba32);
	//scrollBarBg_->init(gl, thumbnailNormalOpenedFile_, Texture::UncompressedRgba32);
	//scrollBarLeft_->init(gl, thumbnailNormalClosedFile_, Texture::UncompressedRgba32);
	//scrollBarRight_->init(gl, thumbnailWidthOpenedFile_, Texture::UncompressedRgba32);


	//scrollBarThumb_->init(gl);

#ifdef USE_FBO
	initTexRenderer(gl);
#endif
	////layout thumb shader	
	//programThumb_ = glCreateProgram();
	//std::string error;
	//GLuint fragmentShader =	
	//	loadShader(GL_FRAGMENT_SHADER, gShaderSrc, &error);
	//GLuint vertShader = 
	//	loadShader(GL_VERTEX_SHADER, gVertShaderSrc, &error);
	//glAttachShader(programThumb_, vertShader);   
	//glAttachShader(programThumb_, fragmentShader);  
	//glBindAttribLocation(programThumb_, AttribPosition, "position");
	//glBindAttribLocation(programThumb_, AttribTexCoord, "texCoord");
	//linkProgram(programThumb_);	
	//glDeleteShader(fragmentShader);
	//glDeleteShader(vertShader);
	//glUseProgram(programThumb_);
	//locThumbMVPMatrix_ = 
	//	glGetUniformLocation(programThumb_, "modelViewProjectionMatrix");

	menuLayoutButton_->setFileName(topMenuLayoutButtonFile_);
	menuLayoutButton_->init(gl);


	float thumbscale;
	if (isLandscape_)
	{
		thumbscale = width_ / 2048;
	}
	else
	{
		thumbscale = width_ / 1536;
	}

	layoutOffsetY_ = height_ - 64 * thumbscale * dpiScale_;// - thumbnailNormal_->height() * thumbscale;
	setButtonTransform();


	loadingPage_->init(gl, width_, height_);

	readBookmarks();

	BOOST_FOREACH(Scene * scene, scenes_)
	{
		scene->setBgColor(Color(43.0 / 255.0, 43.0 / 255.0, 43.0 / 255.0, 1.0f));
	}

	init_ = true;
}

struct ButtonImgs
{
	ButtonImgs(Image * enabled, Image * disabled)
	{
		this->enabled = enabled;
		this->disabled = disabled;
	}
	Image * enabled;
	Image * disabled;
};

void CatalogDocumentTemplate::uninit()
{
	if (!init_) return;
	subscene_->uninit();
	for(int i = 0 ; i < SUBSCENECOUNT ; i++)
		subsceneCached_[i]->uninit();

	landSubscene_->leftpage_->uninit();
	landSubscene_->rightpage_->uninit();
	for(int i = 0 ; i < 2 ; i++)
	{
		landSubsceneCached_[i]->leftpage_->uninit();
		landSubsceneCached_[i]->rightpage_->uninit();
	}

	BOOST_FOREACH(InternalSubscene * subscene, subSceneBackup_)
	{
		subscene->uninit();
	}

	//thumbnailNormalClosed_->uninit();
	//thumbnailNormalOpened_->uninit();
	//thumbnailWidthClosed_->uninit();
	//thumbnailWidthOpened_->uninit();
	thumbnailNormal_->uninit();
	thumbnailWidth_->uninit();
	thumbnailOpened_->uninit();
	thumbnailClosed_->uninit();
	bookmarkCheck_->uninit();
	thumbnailShadow_->uninit();
	thumbnailWidthClosed_->uninit();
	thumbnailWidthOpened_->uninit();
	//scrollBarBg_->uninit();
	//scrollBarLeft_->uninit();
	//scrollBarRight_->uninit();
	//scrollBarThumb_->uninit();

	map<int , ThumbData>::iterator iter;
	for (iter = thumbMap_.begin(); iter != thumbMap_.end(); ++iter)
	{
		(*iter).second.uninit();
	}
	thumbMap_.clear();


#ifdef USE_FBO
	uninitTexRenderer();
#else
	if (ssBuffer_) {free(ssBuffer_); ssBuffer_ = 0;}
	if (ssTexture_) 
	{
		glDeleteTextures(1, &ssTexture_);
	}
#endif

	glDeleteProgram(programThumb_);	

	menuLayoutButton_->uninit();

	loadingPage_->uninit();

	Global::instance().resetSaveThread();
	Global::instance().resetLoadThread();

	init_ = false;
}


void CatalogDocumentTemplate::ThumbData::uninit()
{
	texture->uninit();
}

Scene * CatalogDocumentTemplate::pageLeft() const
{
	if (scenes_.empty()) return 0;
	if (pageIndex_ == 0) return 0;

	return scenes_[pageIndex_ - 1];
}

Scene * CatalogDocumentTemplate::pageRight() const
{
	if (scenes_.empty()) return 0;
	if (pageIndex_ == (int)scenes_.size() - 1) return 0;
	if (isLandscape_ && pageIndex_ == (int)scenes_.size() - 2 && (scenes_.size() - 1)%2 == 0) return 0;

	return scenes_[pageIndex_ + 1];	
}


bool CatalogDocumentTemplate::isCurrentScene(Scene * scene) const
{
    Scene * curScene = 0;
    Scene * leftScene = 0;
    Scene * rightScene = 0;
    if (0 <= pageIndex_ && pageIndex_ < (int)scenes_.size())
    {
        if (pageIndex_ == 0 || (pageIndex_%2 == 1 && pageIndex_ == scenes_.size() - 1))
        {
            leftScene = scenes_[pageIndex_];
            rightScene= scenes_[pageIndex_];
        }
        else if (pageIndex_ % 2 == 0)
        {
			leftScene = scenes_[pageIndex_-1];
            rightScene = scenes_[pageIndex_];
        }
        else if  (pageIndex_ % 2 == 1)
        {
            leftScene = scenes_[pageIndex_];
            rightScene = scenes_[pageIndex_+1];
        }
    }
    if (0 <= pageIndex_ && pageIndex_ < (int)scenes_.size())
        curScene = scenes_[pageIndex_]; 
    return (curScene || (isLandscape_ && leftScene)) && ((scene == curScene || curScene->isShowing(scene)) || ((scene == leftScene || leftScene->isShowing(scene)) || (scene == rightScene || rightScene->isShowing(scene))));
}

void CatalogDocumentTemplate::setCurPage(int pageIndex)
{
	bool sceneChanged = false;

	if (isLandscape_)
	{
		if (pageIndex % 2 == 0)
		{
			if (pageIndex_ != pageIndex && pageIndex_+1 != pageIndex)
				sceneChanged = true;
		}
		else
		{
			if (pageIndex_ != pageIndex && pageIndex_-1 != pageIndex)
				sceneChanged = true;
		}
	}
	else
	{
		if (pageIndex_ != pageIndex)
			sceneChanged = true;
	}
	if (!sceneChanged) return;	

	if (pageIndex >= (int)scenes_.size()) return;


	curSceneAppObjectStop();
	subscene_->stop();

	/*
	set articleIndex before deactivate() because, for example: there may be a 
	VideoStoppedEvent on the previous page. There may be a SceneChangeAction
	connected to that event. In this case the SceneChangeAction should not be
	executed. SceneChangeAction checks to see if the current scene is equal 
	to the scene that the action is associated with. We need for that check 
	to fail in order for the SceneChangeAction to be property not executed. 
	Therefore set the current scene to the new scene before calling 
	deactivate() which can trigger a VideoStoppedEvent.

	Caution: curSceneAppObjectStop still must come before, because it operates
	on the current scene.
	*/
	/*
	set articleIndex before subscene_->stop() for a similar reason as above.
	For example, SceneStopEvent -> SceneChangeAction.		

	Actually, maybe the above reason is not that compelling.
	Also, there is a reason to do subscene_->stop() before setting the 
	articleIndex. Suppose there is a script event handler attached to 
	SceneStopEvent. That event handler won't properly be called if the event 
	handler's scene is not recognized as the current scene.
	*/
	int prevPageIndex = pageIndex_;
	pageIndex_ = pageIndex;

	resetSaveLocalRequest();

	//stop any videos or audio that were playing on the previous page
	Global::instance().videoPlayer()->deactivate();
	Global::instance().audioPlayer()->stopAll();
	Global::instance().cameraOff();

	Transform t;
	Global & g = Global::instance();
	

	if (isLandscape_)
	{
		if (landSubscene_->leftpage_) 
			subSceneBackup_[SUBSCENECOUNT+1]->setScene(landSubscene_->leftpage_->scene());
		if (landSubscene_->rightpage_) 
			subSceneBackup_[SUBSCENECOUNT+2]->setScene(landSubscene_->rightpage_->scene());
		for(int i = 0 ; i < 2 ; i++)
		{
			if (landSubsceneCached_[i]->leftpage_) 
				subSceneBackup_[i+SUBSCENECOUNT+3]->setScene(landSubsceneCached_[i]->leftpage_->scene());
			if (landSubsceneCached_[i]->rightpage_) 
				subSceneBackup_[i+SUBSCENECOUNT+5]->setScene(landSubsceneCached_[i]->rightpage_->scene());
		}
		landSubscene_->leftpage_->setTransform(t);
		landSubscene_->rightpage_->setTransform(t);

		if (pageIndex % 2 == 0) //pageIndex == 짝수
		{
			//g.requestLocalSaveObjectFirst(thumbFiles_.find(pageIndex)->second);
			if (pageIndex == 0)
			{
				landSubscene_->leftpage_->setScene(scenes_[pageIndex]);
				landSubscene_->leftpage_->setViewState(InternalSubscene::LandscapeCenter);
				landSubscene_->rightpage_->setScene(0);
			}
			else
			{
				//g.requestLocalSaveObjectFirst(thumbFiles_.find(pageIndex-1)->second);
				landSubscene_->rightpage_->setScene(scenes_[pageIndex]);
				landSubscene_->rightpage_->setViewState(InternalSubscene::LandscapeRight);
				landSubscene_->leftpage_->setScene(scenes_[pageIndex-1]);
				landSubscene_->leftpage_->setViewState(InternalSubscene::LandscapeLeft);
			}
			if (pageIndex-2 < 1)
			{
				if (pageIndex == 0)
					landSubsceneCached_[0]->leftpage_->setScene(0);
				else
					landSubsceneCached_[0]->leftpage_->setScene(scenes_[0]);
				landSubsceneCached_[0]->rightpage_->setScene(0);
			}
			else
			{
				landSubsceneCached_[0]->leftpage_->setScene(scenes_[pageIndex-3]);
				landSubsceneCached_[0]->rightpage_->setScene(scenes_[pageIndex-2]);
			}
			if (scenes_.size() > pageIndex+1)
				landSubsceneCached_[1]->leftpage_->setScene(scenes_[pageIndex+1]);
			else
				landSubsceneCached_[1]->leftpage_->setScene(0);
			if (scenes_.size() > pageIndex+2)
				landSubsceneCached_[1]->rightpage_->setScene(scenes_[pageIndex+2]);
			else
				landSubsceneCached_[1]->rightpage_->setScene(0);
		}
		else	//pageIndex == 홀수
		{
			//g.requestLocalSaveObjectFirst(thumbFiles_.find(pageIndex)->second);
			landSubscene_->leftpage_->setScene(scenes_[pageIndex]);
			if (scenes_.size() > pageIndex+1)
			{
				//g.requestLocalSaveObjectFirst(thumbFiles_.find(pageIndex+1)->second);
				landSubscene_->rightpage_->setScene(scenes_[pageIndex+1]);
                landSubscene_->leftpage_->setViewState(InternalSubscene::LandscapeLeft);
                landSubscene_->rightpage_->setViewState(InternalSubscene::LandscapeRight);
			}
			else
            {
				landSubscene_->rightpage_->setScene(0);
                landSubscene_->leftpage_->setViewState(InternalSubscene::LandscapeCenter);
            }

			if (1 > pageIndex - 1)
			{
				landSubsceneCached_[0]->leftpage_->setScene(scenes_[0]);
				landSubsceneCached_[0]->rightpage_->setScene(0);
			}
			else
			{
				landSubsceneCached_[0]->leftpage_->setScene(scenes_[pageIndex-2]);
				landSubsceneCached_[0]->rightpage_->setScene(scenes_[pageIndex-1]);
			}
			if (scenes_.size() > pageIndex+2)
				landSubsceneCached_[1]->leftpage_->setScene(scenes_[pageIndex+2]);
			else
				landSubsceneCached_[1]->leftpage_->setScene(0);
			if (scenes_.size() > pageIndex+3)
				landSubsceneCached_[1]->rightpage_->setScene(scenes_[pageIndex+3]);
			else
				landSubsceneCached_[1]->rightpage_->setScene(0);
		}
	}
	else
	{
		//g.requestLocalSaveObjectFirst(thumbFiles_.find(pageIndex)->second);
		//temporarily store previous scenes to prevent needless unloading of scenes	
		if (subscene_) subSceneBackup_[0]->setScene(subscene_->scene());
		for(int i = 0 ; i < SUBSCENECOUNT ; i++)
			if (subsceneCached_[i]) subSceneBackup_[i+1]->setScene(subsceneCached_[i]->scene());

		subscene_->setScene(scenes_[pageIndex]);
		subscene_->setTransform(t);

		// catalogtemplete 서브신 설정
		subsceneCached_[0]->setScene(pageRight());
		subsceneCached_[1]->setScene(pageLeft());
		for (int i = 2 ; i < SUBSCENECOUNT ; i++)
		{
			if (i%2 == 0)
			{
				if (pageIndex < (int)scenes_.size() - i)
					subsceneCached_[i]->setScene(scenes_[pageIndex + i]);
				else
					subsceneCached_[i]->setScene(0);
			}
			else
			{
				if (pageIndex > i-pageIndex)
					subsceneCached_[i]->setScene(scenes_[pageIndex - i + 1]);
				else
					subsceneCached_[i]->setScene(0);
			}

		}
	}
	setSubsceneScaling();

	//use this function to make sure the current scene starts only after the 
	//loading screen finishes
	doCurPageStart();	
	float time = Global::currentTime();
	//subsceneCached_->start(time);

	for (int i = 0; i < (int)subSceneBackup_.size(); ++i)
		subSceneBackup_[i]->setScene(0);


	setCurPagePos(Vector3(0, 0, 0));

	//g.requestLocalSaveObjectFirst(thumbFiles_.find(pageIndex)->second);
	//if (isLandscape_)
	//{
	//	if (pageIndex % 2 == 0) //pageIndex == 짝수
	//	{
	//		if (pageIndex != 0)
	//			g.requestLocalSaveObjectFirst(thumbFiles_.find(pageIndex-1)->second);
	//	}
	//	else	//pageIndex == 홀수
	//	{
	//		if (scenes_.size() > pageIndex+1)
	//			g.requestLocalSaveObjectFirst(thumbFiles_.find(pageIndex+1)->second);
	//	}
	//}		
	Global::instance().callbackPageChanged(scenes_[pageIndex]->name());
}

void CatalogDocumentTemplate::setSubsceneScaling()
{
	if (isLandscape_)
	{
		InternalSubscene * subscenes[6];
		subscenes[0] = landSubscene_->leftpage_.get();
		subscenes[1] = landSubscene_->rightpage_.get();
		int i;
		for(i = 2 ; i < 6 ; i+=2)
		{
			subscenes[i] = landSubsceneCached_[i/2-1]->leftpage_.get();
			subscenes[i+1] = landSubsceneCached_[i/2-1]->rightpage_.get();
		}
		for (i = 0; i < 6; ++i)
		{
			InternalSubscene * subscene = subscenes[i];
			Scene * scene = subscene->scene();		
			if (scene)
			{
				Transform transform = subscene->transform();
				//float sx = (height_/2) / scene->screenWidth();
				//float sy = (height_ * (height_/2)/width_) / scene->screenHeight();
				float s = (width_/2) / scene->screenWidth();
				transform.setScaling(s, s, s);
				subscene->setTransform(transform);
				subscene->setWindowWidth(scene->screenWidth() * s);
				subscene->setWindowHeight(scene->screenHeight() * s);
				subscene->setNormalScaleX(s);
				subscene->setNormalScaleY(s);
			}
		}
		Vector3 curSceneScale = subscenes[0]->transform().scaling();
		document_->setCurSceneScale(curSceneScale.x, curSceneScale.y);
	}
	else
	{
		const int numSubScenes = SUBSCENECOUNT+1;
		InternalSubscene * subscenes[numSubScenes];

		subscenes[0] = subscene_.get();
		// catalogtemplete
		for(int i = 0 ; i < SUBSCENECOUNT ; i++)
			subscenes[i+1] = subsceneCached_[i].get();

		for (int i = 0; i < numSubScenes; ++i)
		{
			InternalSubscene * subscene = subscenes[i];
			Scene * scene = subscene->scene();		
			if (scene)
			{
				Transform transform = subscene->transform();
				float sx = width_ / scene->screenWidth();
				float sy = height_ / scene->screenHeight();
				transform.setScaling(sx, sy, (sx + sy)/2);
				subscene->setTransform(transform);
				subscene->setWindowWidth(scene->screenWidth());
				subscene->setWindowHeight(scene->screenHeight());
				subscene->setNormalScaleX(sx);
				subscene->setNormalScaleY(sy);
			}
		}
		Vector3 curSceneScale = subscene_->transform().scaling();
		document_->setCurSceneScale(curSceneScale.x, curSceneScale.y);
	}

	redrawTriggered_ = true;
}


void CatalogDocumentTemplate::curSceneAppObjectStart() const
{
	if (isLandscape_)
	{
		Scene * curScene1 = landSubscene_->leftpage_->scene();
		Scene * curScene2 = landSubscene_->rightpage_->scene();
		if (curScene1)
		{
			if (!displayingAppObjects_)
			{
				curScene1->initAppObjects();
				if (curScene2)
					curScene2->initAppObjects();
				displayingAppObjects_ = true;
			}			
		}
	}
	else
	{
		Scene * curScene = subscene_->scene();
		//init app objects
		if (curScene)
		{
			if (!displayingAppObjects_)
			{
				curScene->initAppObjects();
				displayingAppObjects_ = true;
			}			
		}
	}
}


void CatalogDocumentTemplate::curSceneAppObjectStop() const
{
	if (isLandscape_)
	{
		Scene * curScene1 = landSubscene_->leftpage_->scene();
		Scene * curScene2 = landSubscene_->rightpage_->scene();
		if (curScene1)
		{
			if (displayingAppObjects_)
			{
				curScene1->uninitAppObjects();
				if (curScene2)
					curScene2->uninitAppObjects();
				displayingAppObjects_ = false;
			}			
		}
	}
	else
	{
		Scene * curScene = subscene_->scene();
		//uninit app objects
		if (curScene)
		{
			if (displayingAppObjects_)
			{
				curScene->uninitAppObjects();
				displayingAppObjects_ = false;
			}		
		}
	}
}

void CatalogDocumentTemplate::setCurPagePos(const Vector3 & pos)
{
	setBaseAppObjectOffset(
		pos.x, pos.y, 
		document_->curSceneScaleX(), document_->curSceneScaleY(),
		pos.x, pos.y, width_, height_);

	Vector3 pagePos = pos;
	Transform transform;
	if (isLandscape_)
	{
		transform = landSubscene_->leftpage_->transform();
		float landY = (height_ - landSubscene_->leftpage_->windowHeight())/2;
		float normalScaleX;
		float normalScaleY;
		if (landSubscene_->leftpage_->scene())
		{
			normalScaleX = landSubscene_->leftpage_->normalScaleX();
			normalScaleY = landSubscene_->leftpage_->normalScaleY();
		}
		else
		{
			normalScaleX = Global::instance().magnification();;
			normalScaleY = Global::instance().magnification();;        
		}
		if ((pageIndex_ == 0) || (pageIndex_%2 == 1 && pageIndex_ == scenes_.size() - 1))
		{
			if (transform.scaling().x > normalScaleX && 
				transform.scaling().y > normalScaleY &&
				(landSubscene_->leftpage_->isZoomable() || landSubscene_->rightpage_->isZoomable()))
			{
			}
			else
				pagePos.y = landY;
			pagePos += Vector3((width_ - landSubscene_->leftpage_->windowWidth())/2,
				0,
				0);
			transform = landSubscene_->leftpage_->transform();
			transform.setTranslation(pagePos.x, pagePos.y, 0);
			landSubscene_->leftpage_->setTransform(transform);
			landSubscene_->rightpage_->setTransform(transform);

			if (transform.scaling().x > normalScaleX && 
				transform.scaling().y > normalScaleY &&
				(landSubscene_->leftpage_->isZoomable() || landSubscene_->rightpage_->isZoomable()))
			{
				float tempW = landSubscene_->leftpage_->windowWidth() * transform.scaling().x / normalScaleX;
				float tempH = landSubscene_->leftpage_->windowHeight() * transform.scaling().y / normalScaleY;
				Matrix m = landSubscene_->leftpage_->transform().computeMatrix();
				m.inverse();
				if (pageIndex_ == 0)
				{
					pagePos = Vector3(tempW, 0 , 0) + Vector3(m._14 + (width_ - landSubscene_->leftpage_->windowWidth())/2, 0, 0);
					pagePos.y = landY;
					transform = landSubsceneCached_[1]->leftpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[1]->leftpage_->setTransform(transform);

					pagePos = pagePos + Vector3(width_/2, 0, 0);
					pagePos.y = landY;
					transform = landSubsceneCached_[1]->rightpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[1]->rightpage_->setTransform(transform);
				}
				else if (pageIndex_%2 == 1 && pageIndex_ == scenes_.size() - 1)
				{
					pagePos = Vector3(m._14, 0, 0) + Vector3(-width_*3/4, 0, 0);
					pagePos.y = landY;
					transform = landSubsceneCached_[0]->rightpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[0]->rightpage_->setTransform(transform);

					pagePos = pagePos + Vector3(-width_/2, 0, 0);
					pagePos.y = landY;
					transform = landSubsceneCached_[0]->leftpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[0]->leftpage_->setTransform(transform);
				}
			}
			else
			{
				if (pageIndex_ == 0)
				{
					pagePos = pos + Vector3(width_, 0, 0);
					pagePos.y = landY;
					transform = landSubsceneCached_[1]->leftpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[1]->leftpage_->setTransform(transform);

					pagePos = pagePos + Vector3(width_/2, 0, 0);
					transform = landSubsceneCached_[1]->rightpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[1]->rightpage_->setTransform(transform);
				}
				else if (pageIndex_%2 == 1 && pageIndex_ == scenes_.size() - 1)
				{
					pagePos = pos + Vector3(-width_/2, 0, 0);
					pagePos.y = landY;
					transform = landSubsceneCached_[0]->rightpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[0]->rightpage_->setTransform(transform);

					pagePos = pagePos + Vector3(-width_/2, 0, 0);
					transform = landSubsceneCached_[0]->leftpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[0]->leftpage_->setTransform(transform);
				}
			}
		}
		else
		{
			if (transform.scaling().x > normalScaleX && 
				transform.scaling().y > normalScaleY &&
				(landSubscene_->leftpage_->isZoomable() || landSubscene_->rightpage_->isZoomable()))
			{
				transform = landSubscene_->leftpage_->transform();
				transform.setTranslation(pagePos.x, pagePos.y, 0);
				landSubscene_->leftpage_->setTransform(transform);

				float tempW =  (width_/2) * transform.scaling().x / normalScaleX;
				transform = landSubscene_->rightpage_->transform();
				transform.setTranslation(pagePos.x + tempW, pagePos.y, 0);
				landSubscene_->rightpage_->setTransform(transform);

				Matrix m = landSubscene_->leftpage_->transform().computeMatrix();
				m.inverse();

				if (pageIndex_ < 3)
				{
					pagePos = Vector3(m._14, 0, 0) + Vector3(-width_ + (width_ - landSubscene_->leftpage_->windowWidth())/2, 0, 0);
					pagePos.y = landY;
					transform = landSubsceneCached_[0]->leftpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[0]->leftpage_->setTransform(transform);
					landSubsceneCached_[0]->rightpage_->setTransform(transform);
				}
				else
				{
					pagePos = Vector3(m._14, 0, 0) + Vector3(-width_/2, 0, 0);
					pagePos.y = landY;
					transform = landSubsceneCached_[0]->rightpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[0]->rightpage_->setTransform(transform);

					pagePos = pagePos + Vector3(-width_/2, 0, 0);
					transform = landSubsceneCached_[0]->leftpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[0]->leftpage_->setTransform(transform);
				}

				if (pageIndex_  > scenes_.size() - 4 && (scenes_.size()-1) % 2 == 1)
				{
					pagePos = Vector3(tempW * 2, 0 , 0) + Vector3(m._14, 0, 0) + Vector3((width_ - landSubscene_->leftpage_->windowWidth())/2, 0, 0);
					pagePos.y = landY;
					transform = landSubsceneCached_[1]->leftpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[1]->leftpage_->setTransform(transform);
					landSubsceneCached_[1]->rightpage_->setTransform(transform);
				}
				else
				{
					pagePos = Vector3(tempW * 2 , 0 , 0) + Vector3(m._14, 0, 0);
					pagePos.y = landY;
					transform = landSubsceneCached_[1]->leftpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[1]->leftpage_->setTransform(transform);

					pagePos = pagePos + Vector3(width_/2, 0, 0);
					transform = landSubsceneCached_[1]->rightpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[1]->rightpage_->setTransform(transform);
				}
			}
			else
			{
				pagePos.y = landY;
				transform = landSubscene_->leftpage_->transform();
				transform.setTranslation(pagePos.x, pagePos.y, 0);
				landSubscene_->leftpage_->setTransform(transform);

				transform = landSubscene_->rightpage_->transform();
				transform.setTranslation(pagePos.x + width_/2, pagePos.y, 0);
				landSubscene_->rightpage_->setTransform(transform);


				pagePos += Vector3(-width_,
					0,
					0);

				if (pageIndex_ - 1 < 2)
				{
					pagePos += Vector3((width_ - landSubscene_->leftpage_->windowWidth())/2,
						0,
						0);
					transform = landSubsceneCached_[0]->leftpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[0]->leftpage_->setTransform(transform);
				}
				else
				{
					transform = landSubsceneCached_[0]->leftpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[0]->leftpage_->setTransform(transform);

					pagePos = pagePos + Vector3(width_/2, 0, 0);
					transform = landSubsceneCached_[0]->rightpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[0]->rightpage_->setTransform(transform);					
				}

				pagePos = pos + Vector3(width_,
					0,
					0);
				pagePos.y = landY;

				if (pageIndex_ + 1 > scenes_.size() - 3 && (scenes_.size()-1) % 2 == 1)
				{
					pagePos += Vector3((width_ - landSubscene_->leftpage_->windowWidth())/2,
						0,
						0);
					transform = landSubsceneCached_[1]->leftpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[1]->leftpage_->setTransform(transform);
				}
				else
				{
					transform = landSubsceneCached_[1]->leftpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[1]->leftpage_->setTransform(transform);

					pagePos = pagePos + Vector3(width_/2, 0, 0);
					transform = landSubsceneCached_[1]->rightpage_->transform();
					transform.setTranslation(pagePos.x, pagePos.y, 0);
					landSubsceneCached_[1]->rightpage_->setTransform(transform);
				}
			}
		}
	}
	else
	{
		transform = subscene_->transform();
		transform.setTranslation(pagePos.x, pagePos.y, 0);
		subscene_->setTransform(transform);	
		float normalScaleX;
		float normalScaleY;
		if (subscene_->scene())
		{
			normalScaleX = width_/subscene_->scene()->screenWidth();
			normalScaleY = height_/subscene_->scene()->screenHeight();
		}
		else
		{
			normalScaleX = Global::instance().magnification();
			normalScaleY = Global::instance().magnification();        
		}

		if (transform.scaling().x > normalScaleX && 
			transform.scaling().y > normalScaleY &&
			subscene_->isZoomable())
		{
			float tempW = width_ * transform.scaling().x / normalScaleX;
			float tempH = height_ * transform.scaling().y / normalScaleY;
			Matrix m = transform.computeMatrix();
			for(int i = 0 ; i < SUBSCENECOUNT ; i++)
			{
				int j = i/2 + 1;
				if (i%2 == 0)	
					pagePos = Vector3(tempW , 0 , 0) + Vector3(m._14, 0, 0) + ((j-1) * Vector3(width_, 0, 0));
				else
					pagePos = Vector3(m._14, 0, 0) + (j * Vector3(-width_, 0, 0));
				transform = subsceneCached_[i]->transform();
				transform.setTranslation(pagePos.x, pagePos.y, 0);
				subsceneCached_[i]->setTransform(transform);	
			}
		}
		else
		{
			for(int i = 0 ; i < SUBSCENECOUNT ; i++)
			{
				int j = i/2 + 1;
				if (i%2 == 0)	
					pagePos = pos + (j * Vector3(width_, 0, 0));
				else
					pagePos = pos + (j * Vector3(-width_, 0, 0));
				transform = subsceneCached_[i]->transform();
				transform.setTranslation(pagePos.x, pagePos.y, 0);
				subsceneCached_[i]->setTransform(transform);
			}
		}
	}
}

void CatalogDocumentTemplate::setScrollPos(float factor)
{	
	/*
	if (factor < 0) factor = 0;
	if (factor > 1) factor = 1;
	*/
	float minX, maxX;
	int drawThumbCount = 1;

	//set layout position
	float thumbnailWidth = thumbWidth();
	float thumbnailHeight = thumbHeight();
	float gap;
	if (isLandscape_)
		drawThumbCount = 11 / dpiScale_;
	else
		drawThumbCount = 8 / dpiScale_;
	gap = gapFactor_ * thumbnailWidth;
	int scenes = (int)scenes_.size();
	if (scenes < drawThumbCount)
		drawThumbCount = scenes;
	maxX = 0;//width_/2 - thumbnailWidth/2;
	minX = maxX - (scenes - drawThumbCount) * (thumbnailWidth + gap) * dpiScale_;

	layoutX_ =  roundFloat(maxX + factor * (minX - maxX));
	scrollPos_ = factor;

	//setScrollBarThumbTransform();	
}

bool CatalogDocumentTemplate::shouldAcceptInput() const
{
	return 
		scenes_.size() > 0 &&	
		!pageTween_.isHappening();
}

bool CatalogDocumentTemplate::keyPressEvent(int keyCode)
{
	return subscene_->keyPressEvent(keyCode);
}

bool CatalogDocumentTemplate::keyReleaseEvent(int keyCode)
{
	return subscene_->keyReleaseEvent(keyCode);
}

bool CatalogDocumentTemplate::pressEvent(const Vector2 & pressPt, int pressId, bool propagate)
{
	PressEventData & pressEvent = pressEvents_[pressId];
	pressEvent.propagate = propagate;

	if (!shouldAcceptInput()) 
	{
		pressEvent.pressed = PressEventData::StrayPress;
		return true;
	}

	if (fadeIn_ || fadeOut_)
		return false;

	Ray mouseRay = processCoords(pressPt);	

	startScrollX_ = layoutX_;

	pressEvent.pressed = PressEventData::Pressed;
	pressEvent.startedMoving = false;
	pressEvent.pressPt = Vector2(pressPt);

	moveDirection_ = Undetermined;
	enum
	{
		None,
		ScrollBar,
		Contents		
	} pressEventMode = None;

	dragVelHandler_.clearMoveHistory();
	layoutTween_.reset();

	bool insideScrollBar;
	if (isLandscape_)
		insideScrollBar= 
		pressPt.y >= height_ - thumbnailWidth_->height() * dpiScale_ * (width_ / 2048);
		//pressPt.y >= height_ - thumbnailWidthClosed_->height()/**dpiScale_*/ * (width_ / thumbnailWidthClosed_->width());
	else
		insideScrollBar= 
		pressPt.y >= height_ - thumbnailNormal_->height() * dpiScale_ * (width_ / 1536);
		//pressPt.y >= height_ - thumbnailNormalClosed_->height()/**dpiScale_*/ * (width_ / thumbnailNormalClosed_->width());


	if (insideScrollBar && isOpenedThumb_)
	{
		pressEventMode = ScrollBar;		
		Vector3 intPt;
		//if (scrollBarThumb_->intersect(&intPt, mouseRay))
		//{
		//	scrollThumbGrabbed_ = true;
		//	redrawTriggered_ = true;
		//}
	}
	else
		pressEventMode = Contents;

	bool handled = false;
	if (pressEventMode == Contents)
	{	
		if (isLandscape_)
		{
			if (propagate)
			{
				handled |= landSubscene_->leftpage_->pressEvent(pressPt, pressId);
				handled |= landSubscene_->rightpage_->pressEvent(pressPt, pressId);
			}
		}
		else
		{
			if (propagate)
				handled = subscene_->pressEvent(pressPt, pressId);
		}

		if (handled) pressEvent.pressed = PressEventData::PassedToChild;
	}	

	if (pageMovingId_ == -1 && !handled) pageMovingId_ = pressId;
	return true;
}


bool CatalogDocumentTemplate::moveEvent(const Vector2 & curPt, int pressId)
{
	if (fadeIn_ || fadeOut_)
		return false;
	PressEventData & pressEvent = pressEvents_[pressId];
	if (pressEvent.pressed == PressEventData::PassedToChild)
	{
		bool handled = false;
		if (isLandscape_)
		{
			handled |= landSubscene_->leftpage_->moveEvent(curPt, pressId);
			handled |= landSubscene_->rightpage_->moveEvent(curPt, pressId);
		}
		else
			handled = subscene_->moveEvent(curPt, pressId);
		if (handled) return true;
	}

	if (pressEvent.pressed == PressEventData::StrayPress) return true;
	if (pageMovingId_ == -1)
		pageMovingId_ = pressId;
	else if (pageMovingId_ != pressId) return false;

	if (!pressEvent.startedMoving)
	{
		pressEvent.startPt = curPt;
		startLayoutOffsetY_ = layoutOffsetY_;
		pressEvent.startMovingTime = Global::currentTime();
	}

	const Vector2 & startPt = pressEvent.startPt;
	const Vector2 & pressPt = pressEvent.pressPt;

	if (moveDirection_ == Undetermined)
	{
		moveDirection_ = Horz;
	}

	// thumnail move
	//if (scrollThumbGrabbed_)
	//{
	//	if (!pressEvent.startedMoving)
	//	{

	//		thumbGrabbedDelta_ = 
	//			Vector3(curPt.x, curPt.y, 0) - scrollBarThumb_->transform().translation();
	//	}
	//	/*
	//	Transform transform = scrollBarThumb_->transform();
	//	Vector2 t = transform.translation();
	//	t.x = curPt.x;*/

	//	float minX, maxX;
	//	minX = 0;
	//	if (isLandscape_)
	//		maxX = height_;
	//	else
	//		maxX = width_;
	//	//minX = (float)scrollBarLeft_->width();
	//	//maxX = float(width_ - 
	//	//	(scrollBarRight_->width() + scrollBarThumb_->width()));


	//	float thumbnailWidth = width_ * thumbnailScale_;
	//	float gap = gapFactor_ * thumbnailWidth;			
	//	int numArticles = (int)scenes_.size();
	//	float factor = (curPt.x - thumbGrabbedDelta_.x - minX) / (maxX - minX);

	//	if (factor < 0) factor = 0;
	//	if (factor > 1) factor = 1;

	//	setScrollPos(factor);

	//	redrawTriggered_ = true;

	//	if (!pressEvent.startedMoving)
	//	{
	//		dragVelHandler_.clearMoveHistory();
	//		pressEvent.startedMoving = true;
	//	}
	//	return true;
	//}
	//else 
	if (moveDirection_ == Horz && requiredLayoutMoveDirection_ != Vert &&
		isOpenedThumb_ && pressEvent.pressPt.y >= layoutOffsetY_)
	{
		requiredLayoutMoveDirection_ = Horz;
		float newLayoutX = startScrollX_ + curPt.x - startPt.x;

		float thumbnailWidth = thumbWidth();
		float thumbnailHeight = thumbHeight();
		float gap = gapFactor_ * thumbnailWidth;

		int numArticles = (int)scenes_.size();

		int drawThumbCount = 1;

		if (isLandscape_)
			drawThumbCount = 11 / dpiScale_;
		else
			drawThumbCount = 8 / dpiScale_;

		if (numArticles < drawThumbCount)
			drawThumbCount = numArticles;
		float maxX = 0;//width_/2 - thumbnailWidth/2;
		float minX = maxX - (numArticles - drawThumbCount) * (thumbnailWidth + gap) * dpiScale_;

		float factor = 0;
		if (maxX > minX)				
			factor = (maxX - newLayoutX) / (maxX - minX);

		layoutTween_.reset();
		setScrollPos(factor);

		dragVelHandler_.addMoveHistory(
			Vector2(layoutX_, layoutOffsetY_), Global::currentTime());

		redrawTriggered_ = true;

		if (!pressEvent.startedMoving)
		{
			dragVelHandler_.clearMoveHistory();
			pressEvent.startedMoving = true;
		}
		return true;
	}

	//dragging a page
	if (!pressEvent.startedMoving) notifyPageChanging();

	float normalScaleX;
	float normalScaleY;
	if (isLandscape_)
	{
		if (landSubscene_->leftpage_->scene())
		{
			normalScaleX = landSubscene_->leftpage_->normalScaleX();
			normalScaleY = landSubscene_->leftpage_->normalScaleY();
		}
		else
		{
			normalScaleX = Global::instance().magnification();;
			normalScaleY = Global::instance().magnification();;        
		}
		if (fabs(landSubscene_->leftpage_->transform().scaling().x - normalScaleX) > 0.00001 &&
			fabs(landSubscene_->leftpage_->transform().scaling().y - normalScaleY) > 0.00001 &&
			(landSubscene_->leftpage_->isZoomable() || landSubscene_->rightpage_->isZoomable()))
		{
//			float deltaX = curPt.x - pressPt.x;
//			float deltaY = curPt.y - pressPt.y;
//			float x = fabs(deltaX);
//			float y = fabs(deltaY);


			//*****************************************************************
			//*                           임시                                *
			//*****************************************************************
			Vector3 temp = landSubscene_->leftpage_->operateTranslation(pressId);
			Transform t = landSubscene_->leftpage_->transform();
			Matrix mat = t.computeMatrix().inverse();
			if (pageIndex_ == 0 ||(pageIndex_ % 2 == 1 && pageIndex_ == scenes_.size() - 1))
			{
				temp -= Vector3((width_ - landSubscene_->leftpage_->windowWidth())/2, 0,0);
			}
			setCurPagePos(temp);
			t = landSubscene_->leftpage_->transform();
			mat = t.computeMatrix().inverse();
			//Transform t = landSubscene_->leftpage_->transform();
			//checkOutside(t, normalScaleX, normalScaleY, false);
			//landSubscene_->leftpage_->setTransform(t);
			//cout << "landsub: " << landSubscene_->leftpage_->transform().translation().x << "\t"
			//	<< landSubscene_->leftpage_->transform().translation().y << "\n";
			//cout << "cached[1]: " << landSubsceneCached_[1]->leftpage_->transform().translation().x << "\t"
			//	<< landSubsceneCached_[1]->leftpage_->transform().translation().y << "\n";
		}
		else if (moveDirection_ == Horz)
		{
			//constrain scrolling if it will end up revealing empty space
			float delta = curPt.x - startPt.x;
			if (!pageLeft() && delta > 0) delta = 0;
			if (!pageRight() && delta < 0) delta = 0;

			setCurPagePos(Vector3(delta, 0, 0));// + subscene_->originTranslation()); //빈공간 막기 origniTranslagtion수
			//cout << "landsub: " << landSubscene_->leftpage_->transform().translation().x << "\t"
			//	<< landSubscene_->leftpage_->transform().translation().y << "\n";
			//cout << "cached[0]: " << landSubsceneCached_[0]->leftpage_->transform().translation().x << "\t"
			//	<< landSubsceneCached_[0]->leftpage_->transform().translation().y << "\n";
			//cout << "cached[1]: " << landSubsceneCached_[1]->leftpage_->transform().translation().x << "\t"
			//	<< landSubsceneCached_[1]->leftpage_->transform().translation().y << "\n";
		}
	}
	else
	{
		if (subscene_->scene())
		{
			normalScaleX = width_/subscene_->scene()->screenWidth();
			normalScaleY = height_/subscene_->scene()->screenHeight();
		}
		else
		{
			normalScaleX = Global::instance().magnification();
			normalScaleY = Global::instance().magnification();        
		}
		if (fabs(subscene_->transform().scaling().x - normalScaleX) > 0.00001 &&
			fabs(subscene_->transform().scaling().y - normalScaleY) > 0.00001 &&
			subscene_->isZoomable())
		{
//			float deltaX = curPt.x - pressPt.x;
//			float deltaY = curPt.y - pressPt.y;
//			float x = fabs(deltaX);
//			float y = fabs(deltaY);

			setCurPagePos(subscene_->operateTranslation(pressId));
			//Transform t = subscene_->transform();
			//checkOutside(t, normalScaleX, normalScaleY, false);
			//subscene_->setTransform(t);
		}
		else if (moveDirection_ == Horz)
		{
			//constrain scrolling if it will end up revealing empty space
			float delta = curPt.x - startPt.x;
			if (!pageLeft() && delta > 0) delta = 0;
			if (!pageRight() && delta < 0) delta = 0;

			setCurPagePos(Vector3(delta, 0, 0));// + subscene_->originTranslation()); //빈공간 막기 origniTranslagtion수
		}
	}

	redrawTriggered_ = true;

	if (!pressEvent.startedMoving)
	{
		dragVelHandler_.clearMoveHistory();
		pressEvent.startedMoving = true;
	}
	return true;
}


bool CatalogDocumentTemplate::hasPageBeenMoved() const
{
	if (isLandscape_)
	{
		if (!landSubscene_->leftpage_) return false;
		const Vector3 & trans = landSubscene_->leftpage_->transform().translation();
		if (trans.x == 0.0f && trans.y == 0.0f) return false;
	}
	else
	{
		if (!subscene_) return false;
		const Vector3 & trans = subscene_->transform().translation();
		if (trans.x == 0.0f && trans.y == 0.0f) return false;
	}

	return true;
}

Vector3 CatalogDocumentTemplate::zoomedPageTweenEndPos(bool & ql, bool & qr, bool & qf)
{
	Vector3 pageTweenEndPos = Vector3(0.0f, 0.0f, 0.0f);

	float temp = 0;
	if (isLandscape_)
	{
		Transform t = landSubscene_->leftpage_->transform();
		float normalScaleX;
		float normalScaleY;
		if (landSubscene_->leftpage_->scene())
		{
			normalScaleX = landSubscene_->leftpage_->normalScaleX();
			normalScaleY = landSubscene_->leftpage_->normalScaleY();
		}
		else
		{
			normalScaleX = Global::instance().magnification();;
			normalScaleY = Global::instance().magnification();;        
		}
		Matrix mat = t.computeMatrix();
		mat = mat.inverse();

		float dx = (landSubscene_->leftpage_->windowWidth()/t.scaling().x*normalScaleX)/2;
		float windowWidth = landSubscene_->leftpage_->windowWidth()/t.scaling().x*normalScaleX*2;


		//if (pageIndex_ != 0 && pageIndex_ != scenes_.size() - 1)
		//{
			//dx = landSubscene_->leftpage_->scene()->screenWidth()/t.scaling().x/2;
			//windowWidth /= 2;
		//}

		bool sl = !qf && 
			mat._14 < -windowWidth + dx && pageLeft();

		Matrix mr = landSubscene_->rightpage_->transform().computeMatrix().inverse();
		bool sr = !qf &&
			mr._14 > windowWidth - dx && pageRight();

		if (ql || sl)
		{
			temp = ((width_ / t.scaling().x * normalScaleX) + mat._14 * normalScaleX ) / normalScaleX;
			mat._14 -= temp;
			if (pageIndex_ == scenes_.size() - 1)
				mat._14 = -landSubscene_->leftpage_->windowWidth() * 2 / t.scaling().x * 5 / 4 ;
			mat = mat.inverse();
			t.setMatrix(mat, true);
			pageTweenEndPos = t.translation();
			if (pageIndex_ == scenes_.size() - 1)
				targetPageIndex_ = pageIndex_ - 1;
			else
				targetPageIndex_ = pageIndex_ - 2;

			if (targetPageIndex_ < 0)
				targetPageIndex_ = 0;
		}
		else if (qr || sr)
		{
			temp = ((width_ / t.scaling().x * normalScaleX) - (mat._14 * normalScaleX  - (width_ - (width_ / t.scaling().x * normalScaleX )))) / normalScaleX;
			mat._14 += temp;
			if (pageIndex_ == 0)
				mat._14 = landSubscene_->leftpage_->windowWidth() * 2 / t.scaling().x * 5 / 4 ;

			mat = mat.inverse();
			t.setMatrix(mat, true);
			pageTweenEndPos = t.translation();
			if (pageIndex_ == 0)
				targetPageIndex_ = pageIndex_ + 1;
			else
				targetPageIndex_ = pageIndex_ + 2;

			if (targetPageIndex_ > scenes_.size() - 1)
				targetPageIndex_ = scenes_.size() - 1;
		}
		else
		{
			checkOutside(t, true);
			pageTweenEndPos = t.translation();
		}
	}
	else
	{
		Transform t = subscene_->transform();
		float normalScaleX;
		float normalScaleY;
		if (subscene_->scene())
		{
			normalScaleX = width_/subscene_->scene()->screenWidth();
			normalScaleY = height_/subscene_->scene()->screenHeight();
		}
		else
		{
			normalScaleX = Global::instance().magnification();
			normalScaleY = Global::instance().magnification();        
		}
		Matrix mat = t.computeMatrix();
		mat = mat.inverse();

		float dx = (subscene_->windowWidth()/t.scaling().x*normalScaleX)/2;

		bool sl = !qf && 
			mat._14 < -dx && pageLeft();

		bool sr = !qf &&
			mat._14 > subscene_->windowWidth() - dx && pageRight();

		if (ql || sl)
		{
			temp = ((width_ / t.scaling().x * normalScaleX) + mat._14 * normalScaleX ) / normalScaleX;
			mat._14 -= temp;
			mat = mat.inverse();
			t.setMatrix(mat, true);
			pageTweenEndPos = t.translation();
			targetPageIndex_ = pageIndex_ - 1;
		}
		else if (qr || sr)
		{
			temp = ((width_ / t.scaling().x * normalScaleX) - (mat._14 * normalScaleX  - (width_ - (width_ / t.scaling().x * normalScaleX )))) / normalScaleX;
			mat._14 += temp;
			mat = mat.inverse();
			t.setMatrix(mat, true);
			pageTweenEndPos = t.translation();		
			targetPageIndex_ = pageIndex_ + 1;
		}
		else
		{
			checkOutside(t, true);
			pageTweenEndPos = t.translation();
		}
	}

	return pageTweenEndPos;
}

void CatalogDocumentTemplate::doPageSnapBack(const PressEventData & pressEvent, const Vector2 & pos)
{
	pageTween_.reset();
	Vector2 startPt = pressEvent.startPt;
	Transform t;
	Vector3 pageTweenStartPos;
	float normalScaleX, normalScaleY, dx;
	bool isMultiTouched;
	dx = width_/2;
	if (isLandscape_)
	{
		t = landSubscene_->leftpage_->transform();
		t.setPivot(0,0,0);
		landSubscene_->leftpage_->setTransform(t);
		t = landSubscene_->rightpage_->transform();
		t.setPivot(0,0,0);
		landSubscene_->rightpage_->setTransform(t);
		pageTweenStartPos = landSubscene_->leftpage_->transform().translation();
		isMultiTouched = landSubscene_->leftpage_->isMultiTouchEnded();
		if (landSubscene_->leftpage_->scene())
		{
			normalScaleX = landSubscene_->leftpage_->normalScaleX();
			normalScaleY = landSubscene_->leftpage_->normalScaleY();
		}
		else
		{
			normalScaleX = Global::instance().magnification();;
			normalScaleY = Global::instance().magnification();; 
		}
	}
	else
	{
		t = subscene_->transform();
		t.setPivot(0,0,0);
		subscene_->setTransform(t);
		pageTweenStartPos = subscene_->transform().translation();
		isMultiTouched = subscene_->isMultiTouchEnded();
		if (subscene_->scene())
		{
			normalScaleX = width_/subscene_->scene()->screenWidth();
			normalScaleY = height_/subscene_->scene()->screenHeight();
		}
		else
		{
			normalScaleX = Global::instance().magnification();
			normalScaleY = Global::instance().magnification();        
		}
	}
	Vector3 pageTweenEndPos = Vector3(0.0f, 0.0f, 0.0f);
	targetPageIndex_ = pageIndex_;




	bool isQuickFlick = 
		Global::currentTime() - pressEvent.startMovingTime < 
		Global::flickTime();

	if (isLandscape_ && (pageIndex_ == 0 || pageIndex_ == scenes_.size() - 1))
	{
		pageTweenStartPos -= Vector3((width_ - landSubscene_->leftpage_->windowWidth())/2, 0.0f, 0.0f);
	}

	bool quickFlickLeft = isQuickFlick && 
		pos.x > startPt.x && pageLeft() && isMultiTouched;
	bool slowLeft = !isQuickFlick && 
		pageTweenStartPos.x > dx && pageLeft();

	bool quickFlickRight = isQuickFlick &&
		pos.x < startPt.x && pageRight() && isMultiTouched;
	bool slowRight = !isQuickFlick &&
		pageTweenStartPos.x < -dx && pageRight();



	if ((subscene_->transform().scaling().x > normalScaleX && 
		subscene_->transform().scaling().y > normalScaleY && 
		subscene_->isZoomable() && !isLandscape_) ||
		(landSubscene_->leftpage_->transform().scaling().x > normalScaleX && 
		landSubscene_->leftpage_->transform().scaling().y > normalScaleY && 
		landSubscene_->leftpage_->isZoomable() && isLandscape_))
	{
		pageTweenEndPos = zoomedPageTweenEndPos(quickFlickLeft,
			quickFlickRight,
			isQuickFlick);
		if (isLandscape_ && (pageIndex_ == 0 || pageIndex_ == scenes_.size() - 1))
		{
			pageTweenEndPos -= Vector3((width_ - landSubscene_->leftpage_->windowWidth())/2, 0.0f, 0.0f);
		}
	}
	else if (moveDirection_ == Horz)
	{			
		if (quickFlickLeft || slowLeft)
		{
			if (isLandscape_)
			{
				pageTweenEndPos = Vector3(width_, 0, 0);
				targetPageIndex_ = pageIndex_ - 2;
				if (targetPageIndex_ < 0)
					targetPageIndex_ = 0;
			}
			else
			{
				pageTweenEndPos = Vector3(width_, 0, 0);
				targetPageIndex_ = pageIndex_ - 1;
			}
		}
		else if (quickFlickRight || slowRight)
		{
			if (isLandscape_)
			{
				pageTweenEndPos = Vector3(-width_, 0, 0);
				targetPageIndex_ = pageIndex_ + 2;
				if (targetPageIndex_ > scenes_.size() - 1)
					targetPageIndex_ = scenes_.size() - 1;
			}
			else
			{
				pageTweenEndPos = Vector3(-width_, 0, 0);
				targetPageIndex_ = pageIndex_ + 1;
			}
		}
	}
	else
		return;
	pageTween_.start(
		Global::currentTime(), pageTweenStartPos, pageTweenEndPos);
}


bool CatalogDocumentTemplate::releaseEvent(const Vector2 & pos, int pressId)
{
	PressEventData & pressEvent = pressEvents_[pressId];
	if (fadeIn_ || fadeOut_)
	{
		pressEvent.pressed = PressEventData::NotPressed;
		return false;
	}

	if (pressEvent.pressed == PressEventData::StrayPress) return true;

	if (isOpenedThumb_ && pressEvent.pressPt.y >= layoutOffsetY_)
	{
		if (pressId == pageMovingId_) pageMovingId_ = -1;

		if (scrollPos_ < 0 || scrollPos_ > 1)
			dragVelHandler_.clearMoveHistory();
		else
			dragVelHandler_.calcVelocityFromHistory(Vector2(layoutX_, layoutOffsetY_), Global::currentTime());

		layoutModeSnappedBackYet_ = false;
	}
	else
	{
		if (pressId == pageMovingId_) 
		{
			pageMovingId_ = -1;
			if (hasPageBeenMoved() && (!subscene_->isTweenWorking() || 
				!landSubscene_->leftpage_->isTweenWorking())) doPageSnapBack(pressEvent, pos);
		}

		if (pressEvent.pressed == PressEventData::PassedToChild)
		{
			if (isLandscape_)
			{
				landSubscene_->leftpage_->releaseEvent(pos, pressId);
				landSubscene_->rightpage_->releaseEvent(pos, pressId);
			}
			else
				subscene_->releaseEvent(pos, pressId);
		}
	}
	//}

	scrollThumbGrabbed_ = false;
	redrawTriggered_ = true;
	pressEvent.pressed = PressEventData::NotPressed;
	return true;
}


int CatalogDocumentTemplate::thumbWidth() const
{
	float thumbscale;
	if (isLandscape_)
	{
		thumbscale = width_ / 2048;
	}
	else
	{
		thumbscale = width_ / 1536;
	}	
	return 152 * thumbscale;// * dpiScale_;
}

int CatalogDocumentTemplate::thumbHeight() const
{
	float thumbscale;
	if (isLandscape_)
	{
		thumbscale = width_ / 2048;
	}
	else
	{
		thumbscale = width_ / 1536;
	}
	//return height_ * thumbnailScale_;
	return 201 * thumbscale;// * dpiScale_;
}

int CatalogDocumentTemplate::thumbsdWidth() const
{
	float thumbscale;
	if (isLandscape_)
	{
		thumbscale = width_ / 2048;
	}
	else
	{
		thumbscale = width_ / 1536;
	}
	return 160 * thumbscale;
}
int CatalogDocumentTemplate::thumbsdHeight() const
{
	float thumbscale;
	if (isLandscape_)
	{
		thumbscale = width_ / 2048;
	}
	else
	{
		thumbscale = width_ / 1536;
	}
	return 209 * thumbscale;
}

Ray CatalogDocumentTemplate::processCoords(const Vector2 & pos)
{
	return Ray(Vector3(pos.x, pos.y, -1000.0f), Vector3(0.0f, 0.0f, 1.0f));
}


bool CatalogDocumentTemplate::clickEvent(const Vector2 & pos, int pressId)
{
	if (fadeIn_ || fadeOut_)
		return false;
	if (!shouldAcceptInput())
	{
		return false;
	}

	bool handled = false;

	Ray mouseRay = processCoords(pos);

	Vector3 intPt;
	if (menuLayoutButton_->intersect(&intPt, mouseRay))
	{
		clickThumbNailLayout();
	}
	else if (thumbnailClickEvent(pos))
	{
	}
	else
	{
		PressEventData & pressEvent = pressEvents_[pressId];
		if (pressEvent.propagate)
		{
			if (isLandscape_)
			{
				handled |= landSubscene_->leftpage_->clickEvent(pos, pressId);
				handled |= landSubscene_->rightpage_->clickEvent(pos, pressId);
			}
			else
				handled |= subscene_->clickEvent(pos, pressId);
		}
	}

	if (!handled)
		document_->changeNaviBarDisplay();

	redrawTriggered_ = true;
	return true;
}

bool CatalogDocumentTemplate::thumbnailClickEvent(const Vector2 & pos)
{
	if (fadeIn_ || fadeOut_)
		return false;
	bool handled = false;

	if (isOpenedThumb_ && pos.y >= layoutOffsetY_)
	{

		Vector3 intPt;	

		float thumbnailWidth = thumbWidth();
		float gap = gapFactor_ * thumbnailWidth;
		float curX = layoutX_;	

		float startY = layoutOffsetY_;

		for (int i = 0; i < (int)scenes_.size(); ++i)
		{
			if (curX < pos.x && pos.x < curX  + thumbnailWidth * dpiScale_)
			{
				handled = true;

				sceneChangeTo(i);
				break;
			}			
			curX += (thumbnailWidth + gap) * dpiScale_;

		}
	}

	return handled;
}

bool CatalogDocumentTemplate::doubleClickEvent(const Vector2 & pos, int pressId)
{
	if (fadeIn_ || fadeOut_)
		return false;
	if (!shouldAcceptInput())
	{
		return false;
	}

	PressEventData & pressEvent = pressEvents_[pressId];
	if (pressEvent.propagate)
	{
        if (isOpenedThumb_ && pos.y >= layoutOffsetY_)
        {
        }
        else
        {
            if (isLandscape_)
            {
                landSubscene_->rightpage_->doubleClickEvent(pos, pressId);
                landSubscene_->leftpage_->doubleClickEvent(pos, pressId);
            }
            else
                subscene_->doubleClickEvent(pos, pressId);
        }
	}
	return true;
}


void CatalogDocumentTemplate::notifyPageChanging()
{
	Scene * curPageScene;
	Scene * curPageScene1 = NULL;
	if (isLandscape_)
	{
		curPageScene = landSubscene_->leftpage_->scene();
		curPageScene1 = landSubscene_->rightpage_->scene();

		if (!curPageScene) return;
		document_->broadcastMessage(
			curPageScene, pageChangingMsg_);
		if (curPageScene1)
			document_->broadcastMessage(
			curPageScene1, pageChangingMsg_);
	}
	else
	{
		curPageScene = subscene_->scene();
		if (!curPageScene) return;
		document_->broadcastMessage(
			curPageScene, pageChangingMsg_);
	}

	static PageChangingEvent event;
	if (isLandscape_)
	{
		curPageScene->root()->handleEvent(&event, Global::currentTime());
		if (curPageScene1)
			curPageScene1->root()->handleEvent(&event, Global::currentTime());
	}
	else
		curPageScene->root()->handleEvent(&event, Global::currentTime());

}

void CatalogDocumentTemplate::sceneSet(Scene * scene)
{
	bool found = false;
	int pageIndex = 0; 

	for ( ; pageIndex < scenes_.size() ; pageIndex++)
	{
		if (scenes_[pageIndex] == scene)
		{
			found = true; break;
		}
	}

	if (!found) return;
	setCurPage(pageIndex);
}

void CatalogDocumentTemplate::sceneChangeTo(int pageIndex)
{
	if (pageIndex_ == pageIndex) return;
	notifyPageChanging(); 
	setCurPage(pageIndex);
	document_->triggerRedraw();
}


void CatalogDocumentTemplate::sceneChangeTo(Scene * scene)
{
	bool found = false;
	int pageIndex = 0; 
	for ( ; pageIndex < scenes_.size() ; pageIndex++)
	{
		if (scenes_[pageIndex] == scene)
		{
			found = true; break;
		}
	}

	if (found) sceneChangeTo(pageIndex);
}


void CatalogDocumentTemplate::doLayoutTween(float startPos, float endPos)
{
	if (endPos < 0) endPos = 0;
	if (endPos > 1) endPos = 1;

	float thumbnailWidth = thumbWidth();
	float thumbnailHeight = thumbHeight();
	float gap = gapFactor_ * thumbnailWidth;

	int middleI = int((width_/2 - layoutX_) / (thumbnailWidth + gap) * dpiScale_);

	if (middleI < 0) middleI = 0;
	if (middleI > (int)scenes_.size() - 1) middleI = (int)scenes_.size() - 1;

	int deltaPageIndex = (int)roundFloat(-layoutOffsetY_ / (thumbnailHeight + gap) * dpiScale_);

	int newPageIndex = pageIndex_ + deltaPageIndex;
	if (newPageIndex < 0) newPageIndex = 0;
	if (newPageIndex > scenes_.size() - 1)
		newPageIndex = scenes_.size() - 1;

	layoutTweenEndPageIndex_ = newPageIndex;

	deltaPageIndex = newPageIndex - pageIndex_;

	layoutTween_.start(Global::currentTime(), 
		Vector2(startPos, layoutOffsetY_), 
		Vector2(endPos, -deltaPageIndex * (thumbnailHeight + gap)));
}

bool CatalogDocumentTemplate::update(float time)
{
	bool needRedraw = false;

	if (isLandscape_)
	{
		if (!landSubscene_->leftpage_->isLoaded())
			needRedraw = true;
		else
			needRedraw |= landSubscene_->leftpage_->update(time);
		if (!landSubscene_->rightpage_->isLoaded())
			needRedraw = true;
		else
		{
			needRedraw |= landSubscene_->rightpage_->update(time);

			if (!landSubscene_->correctRightPageTranslation())
				needRedraw = true;
		}
	}
	else
	{
		if (!subscene_->isLoaded())
			needRedraw = true;
		else
			needRedraw |= subscene_->update(time);
	}

	if (redrawTriggered_)
	{
		redrawTriggered_ = false;
		needRedraw = true;
	}
	//layout mode thumbnail drag acceleration / snap back

	Vector2 dragVelocity = dragVelHandler_.dragVelocity();		
	if (dragVelocity.x != 0 || dragVelocity.y != 0)
	{
		dragVelHandler_.update(time);

		float minX, maxX;

		//set layout position
		float thumbnailWidth = thumbWidth();
		float thumbnailHeight = thumbHeight();
		float gap = gapFactor_ * thumbnailWidth;
		int numArticles = (int)scenes_.size();
		int drawThumbCount = 1;

		if (isLandscape_)
			drawThumbCount = 11 / dpiScale_;
		else
			drawThumbCount = 8 / dpiScale_;

		if (numArticles < drawThumbCount)
			drawThumbCount = numArticles;
		maxX = 0;//width_/2 - thumbnailWidth/2;
		minX = maxX - (numArticles - drawThumbCount) * (thumbnailWidth + gap) * dpiScale_;

		int middleI = int((width_/2 - layoutX_) / ((thumbnailWidth + gap) * dpiScale_));
		if (middleI < 0) middleI = 0;
		if (middleI > (int)scenes_.size() - 1) middleI = (int)scenes_.size() - 1;

		bool hasHitBoundaryX = false, hasHitBoundaryY = false;

		layoutX_ += dragVelHandler_.moveDelta().x;
		float factor = (layoutX_ - maxX)/(minX - maxX);
		if (factor < 0 || factor > 1) hasHitBoundaryX = true;			

		dragVelHandler_.hasHitBoundary(hasHitBoundaryX, hasHitBoundaryY);

		setScrollPos(factor);
		needRedraw = true;	

	}
	else if (!layoutModeSnappedBackYet_)
	{			
		int numScenes = (int)scenes_.size();		
		float scrollEndPos = 0.5f;
		if (numScenes > 1)
		{
			scrollEndPos = (int)(scrollPos_ * (numScenes - 1) + 0.5f);
			scrollEndPos /= (numScenes - 1);
		}

		if (pageMovingId_ == -1) //notPressed
			doLayoutTween(scrollPos_, scrollEndPos);

		layoutModeSnappedBackYet_ = true;
	}
	else if (layoutTween_.isHappening())
	{
		Vector2 tweenedData = layoutTween_.update(time);

		float tweenedPos = tweenedData.x;
		setScrollPos(tweenedPos);

		//layoutOffsetY_ = tweenedData.y;

//		if (!layoutTween_.isHappening())
//		{
//			float thumbscale;
//			if (isLandscape_)
//			{
//				thumbscale = width_ / 2048;
//			}
//			else
//			{
//				thumbscale = width_ / 1536;
//			}
			//*******************************************************************
			//								임시								*
			//*******************************************************************
//			if(isOpenedThumb_)
//				layoutOffsetY_ = height_ - thumbnailOpened_->height() * dpiScale_ * thumbscale - thumbnailNormal_->height() * thumbscale * dpiScale_;
//			else
//			{
//				if (thumbnailOpened_->isLoaded())
//					layoutOffsetY_ = height_ - thumbnailOpened_->height() * dpiScale_ * thumbscale;
//				else
//					layoutOffsetY_ = height_ - 64 * dpiScale_ * thumbscale;
//			}
			//if (isLandscape_)
			//{
			//	if(isOpenedThumb_)
			//		layoutOffsetY_ = height_/* - (height_ * thumbnailScale_)*/ - (thumbnailWidth_->height() * thumbscale);
			//		//layoutOffsetY_ = height_/* - (height_ * thumbnailScale_)*/ - (thumbnailWidthClosed_->height() * thumbscale);
			//	else
			//		layoutOffsetY_ = height_ - 62 * thumbscale;// * dpiScale_;
			//}
			//else
			//{
			//	if(isOpenedThumb_)
			//		layoutOffsetY_ = height_/* - (height_ * thumbnailScale_)*/ - (thumbnailNormal_->height() * thumbscale);
			//		//layoutOffsetY_ = height_/* - (height_ * thumbnailScale_)*/ - (thumbnailNormalClosed_->height() * thumbscale);
			//	else
			//		layoutOffsetY_ = height_ - 62 * thumbscale;// * dpiScale_;
			//}
			//scenes_[layoutTweenEndArticleIndex_]->setCurPage(
			//	layoutTweenEndPageIndex_);
//		}

		needRedraw = true;	
	}
	else if (pageMovingId_ == -1) //notPressed
	{
		requiredLayoutMoveDirection_ = Undetermined;
	}


	if (layoutYMoveTween_.isHappening())
	{
		layoutOffsetY_ = layoutYMoveTween_.update(time);

		setButtonTransform();

		if (!layoutYMoveTween_.isHappening())
		{
			isOpenedThumb_ = !isOpenedThumb_;
		}

		needRedraw = true;
	}


	if (pageTween_.isHappening())
	{
		Vector3 pos = pageTween_.update(time);
		setCurPagePos(pos);


		int numArticles = (int)scenes_.size();
		float factor = 0.5f;
		if (numArticles > 1)
		{
			factor = (float)pageIndex_ / (numArticles - 1) - 
				pos.x / (width_* (numArticles - 1));
		}

		setScrollPos(factor);

		if (!pageTween_.isHappening())
		{
//			if (isLandscape_)
//				Scene * prevScene = landSubscene_->leftpage_->scene();
//			else
//				Scene * prevScene = subscene_->scene();
			setCurPage(targetPageIndex_);
		}

		needRedraw = true;	
	}
	//  else
	//  {
	//      float normalScaleX;
	//      float normalScaleY;
	//      bool checkchange = false;
	//if (isLandscape_)
	//{
	//	if (landSubscene_->leftpage_->scene())
	//	{
	//		normalScaleX = (height_/2)/landSubscene_->leftpage_->scene()->screenWidth();
	//		normalScaleY = width_/landSubscene_->leftpage_->scene()->screenHeight();
	//	}
	//	else
	//	{
	//		normalScaleX = 0.75;
	//		normalScaleY = 0.75;        
	//	}
	//	Transform t = landSubscene_->leftpage_->transform();

	//}
	//else
	//{
	//	if (subscene_->scene())
	//	{
	//		normalScaleX = width_/subscene_->scene()->screenWidth();
	//		normalScaleY = height_/subscene_->scene()->screenHeight();
	//	}
	//	else
	//	{
	//		normalScaleX = Global::instance().magnification();
	//		normalScaleY = Global::instance().magnification();        
	//	}
	//	Transform t = subscene_->transform();

	//	//if(checkOutside(t, normalScaleX, normalScaleY, false)) {
	//	//	subscene_->setTransform(t);
	//	//	needRedraw = true;
	//	//}
	//}
	//  }

	if (fadeIn_)
	{
		fadeIn();
		needRedraw = true;
	}
	else if(fadeOut_)
	{
		fadeOut();
		needRedraw = true;
	}

	return needRedraw;
}


bool CatalogDocumentTemplate::asyncLoadUpdate()
{
	bool everythingLoaded = true;
	everythingLoaded &= thumbnailOpened_->asyncLoadUpdate();
	everythingLoaded &= thumbnailClosed_->asyncLoadUpdate();
	everythingLoaded &= thumbnailWidthClosed_->asyncLoadUpdate();
	everythingLoaded &= thumbnailWidthOpened_->asyncLoadUpdate();

	Global & g = Global::instance();
	bool checkCurrentSceneLoaded = false;

	if (isLandscape_)
	{
		everythingLoaded &= landSubscene_->leftpage_->asyncLoadUpdate();
		everythingLoaded &= landSubscene_->rightpage_->asyncLoadUpdate();
		
		checkCurrentSceneLoaded |= landSubscene_->leftpage_->asyncLoadUpdate();
		checkCurrentSceneLoaded |= landSubscene_->rightpage_->asyncLoadUpdate();
		for(int i = 0 ; i < 2 ; i++)
		{
			if (landSubsceneCached_[i]->leftpage_)
				everythingLoaded &= landSubsceneCached_[i]->leftpage_->asyncLoadUpdate();
			if (landSubsceneCached_[i]->rightpage_)
				everythingLoaded &= landSubsceneCached_[i]->rightpage_->asyncLoadUpdate();
		}
	}
	else
	{
		everythingLoaded &= subscene_->asyncLoadUpdate();
		checkCurrentSceneLoaded |= subscene_->asyncLoadUpdate();
		for(int i = 0 ; i < SUBSCENECOUNT ; i++)
			if (subsceneCached_[i])everythingLoaded &= subsceneCached_[i]->asyncLoadUpdate();
	}
	g.setIsCurrentSceneLoaded(checkCurrentSceneLoaded);

	//everythingLoaded &= thumbnailNormalClosed_->asyncLoadUpdate();
	//everythingLoaded &= thumbnailNormalOpened_->asyncLoadUpdate();
	//everythingLoaded &= thumbnailWidthClosed_->asyncLoadUpdate();
	//everythingLoaded &= thumbnailWidthOpened_->asyncLoadUpdate();
	everythingLoaded &= thumbnailNormal_->asyncLoadUpdate();
	everythingLoaded &= thumbnailWidth_->asyncLoadUpdate();
	//everythingLoaded &= thumbnailOpened_->asyncLoadUpdate();
	//everythingLoaded &= thumbnailClosed_->asyncLoadUpdate();
	everythingLoaded &= bookmarkCheck_->asyncLoadUpdate();
	everythingLoaded &= thumbnailShadow_->asyncLoadUpdate();
	//everythingLoaded &= thumbnailWidthClosed_->asyncLoadUpdate();
	//everythingLoaded &= thumbnailWidthOpened_->asyncLoadUpdate();

	//everythingLoaded &= scrollBarBg_->asyncLoadUpdate();
	//everythingLoaded &= scrollBarLeft_->asyncLoadUpdate();
	//everythingLoaded &= scrollBarRight_->asyncLoadUpdate();
	//everythingLoaded &= scrollBarThumb_->asyncLoadUpdate();

	everythingLoaded &= menuLayoutButton_->asyncLoadUpdate();



	map<int, ThumbData>::iterator titr = thumbMap_.begin();
	bool thumbsLoaded = true;
	for (; titr != thumbMap_.end(); ++titr)
	{
		bool thumbLoaded = (*titr).second.texture->asyncLoadUpdate();
		everythingLoaded &=  thumbLoaded;
		thumbsLoaded &= thumbLoaded;
	}

	if (!everythingLoaded)
		redrawTriggered_ = true;

	if (thumbsLoaded && !thumbsLoadedConfirmed_)
	{
		//have to ensure that there is a redraw the very first time all thumbs
		//load, or else some thumbs may not show up
		redrawTriggered_ = true;
		thumbsLoadedConfirmed_ = true;
	}

	//Global & g = Global::instance();

	if(!searchQ_.empty() && resetComplete_)
	{
		Scene * scene = searchQ_.front();
		searchQ_.pop_front();
		scene->requestLocalSaveObject();
	}

	return everythingLoaded;
}


void CatalogDocumentTemplate::drawSubscene(GfxRenderer * gl, InternalSubscene * subscene) const
{		
	if (!subscene->isLoaded())
	{
		const Vector3 & trans = subscene->transform().translation();		
		loadingPage_->draw(gl, trans.x, trans.y);
		document_->triggerRedraw();
	}
	else
	{
		bool isCurPage;
		if (isLandscape_)
		{
			isCurPage = (subscene == landSubscene_->leftpage_.get() || subscene == landSubscene_->rightpage_.get());
		}
		else
			isCurPage = subscene == subscene_.get();
		if (isCurPage && !curPageStarted_)
		{
			subscene->start(Global::currentTime());
			curSceneAppObjectStart();
			curPageStarted_ = true;
		}
		subscene->draw(gl);
		if (fadeIn_ || fadeOut_)
		{
			float color = 43.0 / 255.0;
			gl->useColorProgram();
			gl->setColorProgramColor(color, color, color, opacity_);
			gl->drawRect(subscene->transform().translation().x, subscene->transform().translation().y, 
				subscene->sceneWidth() * subscene->transform().scaling().x, subscene->sceneHeight() * subscene->transform().scaling().y);
		}
	}
}

void CatalogDocumentTemplate::fadeIn()
{
	//float opacity;
	//if (isLandscape_)
	//	opacity = landSubscene_->leftpage_->scene()->root()->visualAttrib()->opacity() - 0.2;
	//else
	//	opacity = subscene_->scene()->root()->visualAttrib()->opacity() - 0.2;
	//opacity_ = 0;
	//if (opacity_ <= 0)
	//{
	//	opacity_ = 0;
	//	fadeIn_ = false;
	//	fadeOut_ = true;
	//}
	opacity_ = 1;
	fadeIn_ = false;
	fadeOut_ = true;
	//if (isLandscape_)
	//{
	//	landSubscene_->leftpage_->scene()->root()->visualAttrib()->setOpacity(opacity);
	//	if (pageIndex_ != 0 && pageIndex_ != scenes_.size()-1)
	//		landSubscene_->rightpage_->scene()->root()->visualAttrib()->setOpacity(opacity);
	//}
	//else
	//	subscene_->scene()->root()->visualAttrib()->setOpacity(opacity);

	//if (fadeOut_)
}

void CatalogDocumentTemplate::fadeOut()
{
	if (opacity_ >= 1)
		changeLandscape();
	//float opacity;
	//if (isLandscape_)
	//	opacity = landSubscene_->leftpage_->scene()->root()->visualAttrib()->opacity() + 0.05;
	//else
	//	opacity = subscene_->scene()->root()->visualAttrib()->opacity() + 0.05;
	opacity_ -= 0.05;
	//if (opacity_ >= 1)
	//{
	//	opacity_ = 1;
	//	fadeOut_ = false;
	//}
	if (opacity_ <= 0)
	{
		opacity_ = 0;
		fadeOut_ = false;
	}
	//if (isLandscape_)
	//{
	//	landSubscene_->leftpage_->scene()->root()->visualAttrib()->setOpacity(opacity);
	//	if (pageIndex_ != 0 && pageIndex_ != scenes_.size()-1)
	//		landSubscene_->rightpage_->scene()->root()->visualAttrib()->setOpacity(opacity);
	//	for (int i = 0 ; i < 2 ; i++)
	//	{
	//		if (landSubsceneCached_[i]->leftpage_->scene())
	//			landSubsceneCached_[i]->leftpage_->scene()->root()->visualAttrib()->setOpacity(opacity);
	//		if (landSubsceneCached_[i]->rightpage_->scene())
	//			landSubsceneCached_[i]->rightpage_->scene()->root()->visualAttrib()->setOpacity(opacity);
	//	}
	//}
	//else
	//{
	//	subscene_->scene()->root()->visualAttrib()->setOpacity(opacity);
	//	for(int i = 0 ; i < SUBSCENECOUNT ; i++)
	//	{
	//		if (subsceneCached_[i]->scene())
	//			subsceneCached_[i]->scene()->root()->visualAttrib()->setOpacity(opacity);
	//	}
	//}
}

void CatalogDocumentTemplate::draw(GfxRenderer * gl) const
{
	glDepthFunc(GL_ALWAYS);
	float color = 43.0 / 255.0;

	glClearColor(color, color, color, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | Global::extraGlClearOptions());

	gl->setCameraMatrix(camera_.GetMatrix());
	gl->loadMatrix(Matrix::Identity());

	// catalogtemplete
	if (isLandscape_)
	{
		for(int i = 0 ; i < 2 ; i++)
		{
			if (landSubsceneCached_[i]->leftpage_)
			{
				bool drawCached = false;
				switch(i)
				{
				case 0:
					drawCached = landSubsceneCached_[0]->leftpage_->transform().translation().x > -width_;
					break;
				case 1:
					drawCached = landSubsceneCached_[1]->leftpage_->transform().translation().x < width_;
					break;
				}
				if (drawCached)
				{
					drawSubscene(gl, landSubsceneCached_[i]->leftpage_.get());
					drawSubscene(gl, landSubsceneCached_[i]->rightpage_.get());
				}
				else
				{
					landSubscene_->leftpage_.get()->setMode(InternalSubscene::ModeFixed);
					landSubscene_->rightpage_.get()->setMode(InternalSubscene::ModeFixed);
				}
			}
		}
		drawSubscene(gl, landSubscene_->leftpage_.get());
		drawSubscene(gl, landSubscene_->rightpage_.get());
	}
	else
	{
		for(int i = 0 ; i < 2 ; i++)
		{
			if (subsceneCached_[i])
			{
				bool drawCached = false;
				switch(i)
				{
				case 0:
					drawCached = subsceneCached_[0]->transform().translation().x < width_;
					break;
				case 1:
					drawCached = subsceneCached_[1]->transform().translation().x > -width_;
					break;
				}
				if (drawCached)
				{
					subscene_->setMode(InternalSubscene::ModeScrollable);
					drawSubscene(gl, subsceneCached_[i].get());
				}
				else
				{
					subscene_->setMode(InternalSubscene::ModeFixed);
				}
			}
		}
		drawSubscene(gl, subscene_.get());
	}

	//draw scrollbar regions
	gl->useTextureProgram();
	gl->setTextureProgramOpacity(1);
	
	float startX = 0;

	if (width_ * dpiScale_ > width_)
		startX = (width_ - width_ * dpiScale_)/2;

	if (isLandscape_)
	{
		//if(isOpenedThumb_)
		//{
		//	gl->use(thumbnailWidthOpened_.get());
		//	gl->drawRect(
		//		0, layoutOffsetY_, 
		//		width_, 
		//		thumbnailWidthOpened_->height() * /*dpiScale_ **/ (width_ / thumbnailWidthOpened_->width()));
		//}
		//else
		//{
		//	gl->use(thumbnailWidthClosed_.get());
		//	gl->drawRect(
		//		0, layoutOffsetY_, 
		//		width_, 
		//		thumbnailWidthClosed_->height() */* dpiScale_ **/ (width_ / thumbnailWidthClosed_->width()));
		//}
		gl->use(thumbnailWidth_.get());
		gl->drawRect(
			0, layoutOffsetY_ + thumbnailWidthOpened_->height() * dpiScale_ * (width_ / thumbnailWidthOpened_->width()), 
			width_, 
			thumbnailWidth_->height() * dpiScale_ * (width_ / thumbnailWidth_->width()));
		//gl->use(lineShadow_.get());
		//gl->drawRect(
		//		(width_ - lineShadow_->width())/2, -landSubscene_->leftpage_->transform().computeMatrix().inverse()._24* landSubscene_->leftpage_->transform().scaling().y,
		//		lineShadow_->width(),
		//		landSubscene_->leftpage_->sceneHeight() * landSubscene_->leftpage_->transform().scaling().y);
		if(isOpenedThumb_)
		{
			gl->use(thumbnailWidthClosed_.get());
			gl->drawRect(
				startX, layoutOffsetY_, 
				width_ * dpiScale_, 
				thumbnailWidthClosed_->height() * dpiScale_ * (width_ / thumbnailWidthClosed_->width()));
		}
		else
		{
			gl->use(thumbnailWidthOpened_.get());
			gl->drawRect(
				startX, layoutOffsetY_, 
				width_ * dpiScale_, 
				thumbnailWidthOpened_->height() * dpiScale_ * (width_ / thumbnailWidthOpened_->width()));
		}
	}
	else
	{
		gl->use(thumbnailNormal_.get());
		gl->drawRect(
			0, layoutOffsetY_ + thumbnailOpened_->height() * dpiScale_ * (width_ / thumbnailOpened_->width()), 
			width_, 
			thumbnailNormal_->height() * dpiScale_ * (width_ / thumbnailNormal_->width()));
		if(isOpenedThumb_)
		{
			gl->use(thumbnailClosed_.get());
			gl->drawRect(
				startX, layoutOffsetY_, 
				width_ * dpiScale_, 
				thumbnailClosed_->height() * dpiScale_ * (width_ / thumbnailClosed_->width()));
		}
		else
		{
			
			gl->use(thumbnailOpened_.get());
			gl->drawRect(
				startX, layoutOffsetY_, 
				width_ * dpiScale_, 
				thumbnailOpened_->height() * dpiScale_ * (width_ / thumbnailOpened_->width()));
		}
		//if(isOpenedThumb_)
		//{
		//	gl->use(thumbnailNormalOpened_.get());
		//	gl->drawRect(
		//		0, layoutOffsetY_, 
		//		width_, 
		//		thumbnailNormalOpened_->height() * /*dpiScale_ * */(width_ / thumbnailNormalOpened_->width()));
		//}
		//else
		//{
		//	gl->use(thumbnailNormalClosed_.get());
		//	gl->drawRect(
		//		0, layoutOffsetY_, 
		//		width_, 
		//		thumbnailNormalClosed_->height() * /*dpiScale_ **/ (width_ / thumbnailNormalClosed_->width()));
		//}
	}

	drawBookmark(gl);

	if(isOpenedThumb_)
		drawLayout(gl);

	menuLayoutButton_->draw(gl);

}

void CatalogDocumentTemplate::drawBookmark(GfxRenderer * gl) const
{
	gl->use(bookmarkCheck_.get());

	Transform t;
	if (isLandscape_)
	{
//		normalscalex = landSubscene_->leftpage_->normalScaleX();
//		normalscaley = landSubscene_->leftpage_->normalScaleY();
		float windowWidth = landSubscene_->leftpage_->windowWidth()/landSubscene_->leftpage_->normalScaleX();
		t = landSubscene_->leftpage_->transform();
		if (((pageIndex_ == 0) || (pageIndex_%2 == 1 && pageIndex_ == scenes_.size() - 1)) && pageHasBookmark(pageIndex_))
		{
			gl->drawRect(
                         (-t.computeMatrix().inverse()._14 + windowWidth)* t.scaling().x - bookmarkCheck_->width() * dpiScale_ * 1.2 * (width_ / thumbnailWidth_->width()),  -t.computeMatrix().inverse()._24 * t.scaling().y, 
                         bookmarkCheck_->width() /** dpiScale_*/ * dpiScale_ * (width_ / thumbnailWidth_->width()), 
                         bookmarkCheck_->height() /** dpiScale_*/ * dpiScale_ * (width_ / thumbnailWidth_->width()));
		}
		else if (pageIndex_ % 2 == 0)
		{
			if (pageHasBookmark(pageIndex_-1))
			{
				gl->drawRect(
                             (-t.computeMatrix().inverse()._14)* t.scaling().x + (bookmarkCheck_->width() * dpiScale_ * 0.2 * (width_ / thumbnailWidth_->width())),  -t.computeMatrix().inverse()._24 * t.scaling().y, 
                             bookmarkCheck_->width() /** dpiScale_*/ * dpiScale_ * (width_ / thumbnailWidth_->width()), 
                             bookmarkCheck_->height() /** dpiScale_*/ * dpiScale_ * (width_ / thumbnailWidth_->width()));
			}
			if (pageHasBookmark(pageIndex_))
			{
				t = landSubscene_->rightpage_->transform();
				gl->drawRect(
					(-t.computeMatrix().inverse()._14 + windowWidth)* t.scaling().x - bookmarkCheck_->width() * dpiScale_ * 1.2 * (width_ / thumbnailWidth_->width()),  -t.computeMatrix().inverse()._24 * t.scaling().y, 
                             bookmarkCheck_->width() /** dpiScale_*/ * dpiScale_ * (width_ / thumbnailWidth_->width()), 
                             bookmarkCheck_->height() /** dpiScale_*/ * dpiScale_ * (width_ / thumbnailWidth_->width()));
			}
		}
		else if (pageIndex_ % 2 == 1)
		{
			if (pageHasBookmark(pageIndex_))
			{
				gl->drawRect(
					(-t.computeMatrix().inverse()._14)* t.scaling().x + (bookmarkCheck_->width() * dpiScale_ * 0.2 * (width_ / thumbnailWidth_->width())),  -t.computeMatrix().inverse()._24 * t.scaling().y, 
                             bookmarkCheck_->width() /** dpiScale_*/ * dpiScale_ * (width_ / thumbnailWidth_->width()), 
                             bookmarkCheck_->height() /** dpiScale_*/ * dpiScale_ * (width_ / thumbnailWidth_->width()));
			}
			if (pageHasBookmark(pageIndex_+1))
			{
				t = landSubscene_->rightpage_->transform();
				gl->drawRect(
                             (-t.computeMatrix().inverse()._14 + windowWidth)* t.scaling().x - bookmarkCheck_->width() * dpiScale_ * 1.2 * (width_ / thumbnailWidth_->width()),  -t.computeMatrix().inverse()._24 * t.scaling().y, 
                             bookmarkCheck_->width() /** dpiScale_*/ * dpiScale_ * (width_ / thumbnailWidth_->width()), 
                             bookmarkCheck_->height() /** dpiScale_*/ * dpiScale_ * (width_ / thumbnailWidth_->width()));
			}
		}
	}
	else
	{
		float width = subscene_->windowWidth();
		if (pageHasBookmark(pageIndex_))
		{
			t = subscene_->transform();
			gl->drawRect(
				(-t.computeMatrix().inverse()._14 + width )* t.scaling().x - (bookmarkCheck_->width() * dpiScale_ * 1.2 * (width_ / thumbnailNormal_->width())), (-t.computeMatrix().inverse()._24-2 * dpiScale_) * t.scaling().y, 
                         bookmarkCheck_->width() * dpiScale_ * (width_ / thumbnailNormal_->width()), 
                         bookmarkCheck_->height() * dpiScale_ * (width_ / thumbnailNormal_->width()));
		}
		if (pageHasBookmark(pageIndex_-1))
		{
			t = subsceneCached_[1]->transform();
			gl->drawRect(
                         (-t.computeMatrix().inverse()._14 + width )* t.scaling().x - (bookmarkCheck_->width() * dpiScale_ * 1.2 * (width_ / thumbnailNormal_->width())), (-t.computeMatrix().inverse()._24-2 * dpiScale_) * t.scaling().y, 
                         bookmarkCheck_->width() * dpiScale_ * (width_ / thumbnailNormal_->width()), 
                         bookmarkCheck_->height() * dpiScale_ * (width_ / thumbnailNormal_->width()));
		}
		if (pageHasBookmark(pageIndex_+1))
		{
			t = subsceneCached_[0]->transform();
			gl->drawRect(
                         (-t.computeMatrix().inverse()._14 + width )* t.scaling().x - (bookmarkCheck_->width() * dpiScale_ * 1.2 * (width_ / thumbnailNormal_->width())), (-t.computeMatrix().inverse()._24-2 * dpiScale_) * t.scaling().y, 
                         bookmarkCheck_->width() * dpiScale_ * (width_ / thumbnailNormal_->width()), 
                         bookmarkCheck_->height() * dpiScale_ * (width_ / thumbnailNormal_->width()));
		}
	}
}

void CatalogDocumentTemplate::drawLayout(GfxRenderer * gl) const
{
	//static GLfloat vertices[] = {
	//	0, 0, 0, 
	//	1.0f, 0, 0,
	//	0.5f, 1.0f, 0
	//};
	float thumbnailWidth = thumbWidth();
	float thumbnailHeight = thumbHeight();
	float thumbnailsdWidth = thumbsdWidth();
	float thumbnailsdHeight = thumbsdHeight();
	float gap = gapFactor_ * thumbnailWidth;

	float startY;
    startY = layoutOffsetY_ + (96 * thumbnailHeight / 200 * dpiScale_);// * dpiScale_;

	gl->useTextureProgram();

	gl->setTextureProgramOpacity(1);

	//gl->useColorProgram();

	//draw pointers
	//gl->setColorProgramColor(1, 1, 1, 0.25f);

	//vertices[0] = width_/2 - gap*0.8f/2;
	//vertices[1] = startY - gap*0.8f;
	//vertices[3] = width_/2 + gap*0.8f/2;
	//vertices[4] = vertices[1];
	//vertices[6] = width_/2;
	//vertices[7] = startY - gap*0.2f;

	//gl->enableVertexAttribArrayPosition();
	//gl->bindArrayBuffer(0);
	//gl->vertexAttribPositionPointer(0, (char *)vertices);	
	//gl->applyCurrentShaderMatrix();
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	//vertices[0] = width_/2;
	//vertices[1] = startY + thumbnailHeight + gap*0.2f;
	//vertices[3] = width_/2 - gap*0.8f/2;
	//vertices[4] =  startY + thumbnailHeight + gap*0.8f;
	//vertices[6] = width_/2 + gap*0.8f/2;
	//vertices[7] = vertices[4];

	//glDrawArrays(GL_TRIANGLES, 0, 3);


	map<int, ThumbData>::iterator iter;

	//flag all thumb textures as unused initially
	for (iter = thumbMap_.begin(); iter != thumbMap_.end(); ++iter)	
		(*iter).second.used = false;

	int i = -(layoutX_ + thumbnailWidth * dpiScale_) / ((thumbnailWidth + gap) * dpiScale_) + 1;
	int lastI = -(layoutX_ - width_) / ((thumbnailWidth + gap) * dpiScale_) + 1;

	if (i < 0) i = 0;
	if (i > (int)scenes_.size() - 1) i = (int)scenes_.size() - 1;

	if (lastI < 0) lastI = 0;
	if (lastI > (int)scenes_.size()) lastI = (int)scenes_.size();

	float curX = layoutX_ + i * (thumbnailWidth + gap) * dpiScale_;

	int middleI = (int)((width_/2 - layoutX_) / ((thumbnailWidth + gap) * dpiScale_));

	for (;i < lastI; ++i)
	{			
		float curY = startY;

		iter = 
			thumbMap_.find(i);
		Texture * thumbTexture;
		if (iter != thumbMap_.end())
		{
			thumbTexture = (*iter).second.texture.get();
			(*iter).second.used = true;
		}
		else
		{
			ThumbData thumbData;
			thumbData.used = true;
			Texture * texture = new Texture;
			texture->init(gl, thumbFiles_.find(i)->second, Texture::UncompressedRgba32);					
			thumbData.texture.reset(texture);
			thumbMap_[i] = thumbData;
			thumbTexture = texture;
			thumbsLoadedConfirmed_ = false;
		}
		if (thumbTexture->isLoaded() && 
		   (i == pageIndex_ || (isLandscape_ && 
		   ((pageIndex_ % 2 == 0 && i == pageIndex_-1) || (pageIndex_ % 2 == 1 && i == pageIndex_+1)))))
		{
			//gl->setTextureProgramOpacity(0.5);
			gl->useColorProgram();
			gl->setColorProgramColor(1, 0, 0, 1.0f);
			gl->drawRect(curX - (thumbnailsdWidth - thumbnailWidth)/2 * dpiScale_, curY - (thumbnailsdHeight - thumbnailHeight)/2 * dpiScale_, 
				thumbnailsdWidth * dpiScale_, thumbnailsdHeight * dpiScale_);



			gl->useTextureProgram();
			gl->setTextureProgramOpacity(1);
		}
		else
		{
			// 그림자그리기
			gl->use(thumbnailShadow_.get());				
			gl->drawRect(curX - (thumbnailsdWidth - thumbnailWidth)/2 * dpiScale_, curY - (thumbnailsdHeight - thumbnailHeight)/2 * dpiScale_, 
				thumbnailsdWidth * dpiScale_, thumbnailsdHeight * dpiScale_);
		}
		//gl->drawRect(curX - (3/* * dpiScale_*/), curY - (3/* * dpiScale_*/), 
		//	thumbnailWidth + (6 /** dpiScale_*/), thumbnailHeight+ (6 /** dpiScale_*/));


		if (thumbTexture->isLoaded())
		{
			//gl->useCustomProgram(programThumb_, locThumbMVPMatrix_);
			gl->use(thumbTexture);				
			gl->drawRect(curX, curY, 
				thumbnailWidth * dpiScale_, thumbnailHeight * dpiScale_);
		}

		curX += (thumbnailWidth + gap) * dpiScale_;
	}

	//delete any unused thumbnail textures
	bool erased;
	do 
	{
		erased = false;
		for (iter = thumbMap_.begin(); iter != thumbMap_.end(); ++iter)
		{
			ThumbData & data = (*iter).second;
			if (!data.used)
			{
				data.uninit();
				thumbMap_.erase(iter);	
				erased = true;
				break;
			}
		}
	} while (erased);

	//gl->use(scrollBarBg_.get());
	//float scrollGrooveWidth = 
	//	width_ - 
	//	(scrollBarLeft_->width() + scrollBarRight_->width());

	//float scrollBgHeight = scrollBarBg_->height() * dpiScale_;
	//float scrollThumbHeight=  scrollBarThumb_->height() * dpiScale_;

	//gl->drawRect(
	//	scrollBarLeft_->width(), 
	//	height_ - scrollBgHeight, 
	//	scrollGrooveWidth, 
	//	scrollBgHeight);		


	//gl->use(scrollBarLeft_.get());
	//gl->drawRect(
	//	0, height_ - scrollBgHeight, 
	//	scrollBarLeft_->width(), 
	//	scrollBgHeight);

	//gl->use(scrollBarRight_.get());
	//gl->drawRect(
	//	width_ - scrollBarRight_->width(), 
	//	height_ - scrollBgHeight, 
	//	scrollBarRight_->width(), 
	//	scrollBgHeight);

	//scrollBarThumb_->draw(gl);	

}

void CatalogDocumentTemplate::start(float docTime)
{
	int i = 0;
	std::vector<SceneObject *> images;
	static boost::uuids::uuid type = Image().type();
	do
	{
		if (i == scenes_.size())
			break;
		scenes_[i]->getSceneObjectsByID(type, &images);
		i++;
	}while(images.empty());
	if (!images.empty())
	{
		Image* image = (Image*)images[0];

		landscapeMovingTranslation_ = (scenes_[i]->screenWidth() - (image->width() * image->transform().scaling().x))/2;
	}

	//   BOOST_FOREACH(SceneObject * obj, images)
	//   {
	//       Image* img = (Image*)obj;
	//       img->requestZoom();
	//   }
	setCurPage(0);
	Global::setPreserveAspectRatio(preserveAspect_);
}

Scene * CatalogDocumentTemplate::dependsOn(Scene * scene) const
{
	BOOST_FOREACH(Scene * childScene, scenes_)
	{
		if (childScene == scene || childScene->dependsOn(scene))
			return childScene;
	}
	return 0;
}

void CatalogDocumentTemplate::pageScenes(std::vector<Scene *> * scenes) const
{
	BOOST_FOREACH(Scene * childScene, scenes_) scenes->push_back(childScene);
}



void CatalogDocumentTemplate::referencedFiles(
	vector<string> * refFiles) const
{
	refFiles->push_back(topMenuLayoutButtonFile_);
	//refFiles->push_back(thumbnailNormalOpenedFile_);
	//refFiles->push_back(thumbnailNormalClosedFile_);
	//refFiles->push_back(thumbnailWidthOpenedFile_);
	//refFiles->push_back(thumbnailWidthClosedFile_);
	refFiles->push_back(thumbnailNormalBarFile_);
	refFiles->push_back(thumbnailWidthBarFile_);
	refFiles->push_back(thumbnailNormalClosedFile_);
	refFiles->push_back(thumbnailNormalOpenedFile_);
	refFiles->push_back(bookmarkCheckFile_);
	refFiles->push_back(thumbnailShadowFile_);	
	refFiles->push_back(thumbnailWidthClosedFile_);
	refFiles->push_back(thumbnailWidthOpenedFile_);
	//refFiles->push_back(toContentsFile_);

	map<int, std::string>::iterator itr;
	for (int i = 0 ; i < scenes_.size() ; i++)
	{
		refFiles->push_back(thumbFiles_.find(i)->second);
	}
}

int CatalogDocumentTemplate::setReferencedFiles(const std::string & baseDirectory,
												const vector<string> & refFiles, int index)
{
	topMenuLayoutButtonFile_ = refFiles[index++];
	//thumbnailNormalOpenedFile_ = refFiles[index++];
	//thumbnailNormalClosedFile_ = refFiles[index++];
	//thumbnailWidthOpenedFile_ = refFiles[index++];
	//thumbnailWidthClosedFile_ = refFiles[index++];
	thumbnailNormalBarFile_ = refFiles[index++];
	thumbnailWidthBarFile_ = refFiles[index++];
	thumbnailNormalClosedFile_ = refFiles[index++];
	thumbnailNormalOpenedFile_ = refFiles[index++];	
	bookmarkCheckFile_ = refFiles[index++];
	thumbnailShadowFile_ = refFiles[index++];
	thumbnailWidthClosedFile_ = refFiles[index++];
	thumbnailWidthOpenedFile_ = refFiles[index++];
	//toContentsFile_ = refFiles[index++];

	for (int i = 0 ; i < scenes_.size() ; i++)
	{
		thumbFiles_[i] = refFiles[index++];
	}
	return index;
}


void CatalogDocumentTemplate::write(Writer & writer) const
{
	writer.writeParent<DocumentTemplate>(this);
	writer.write(visualAttrib_, "visualAttrib");
	writer.write(scenes_, "scenes");

	string curDir = getCurDir();


	BOOST_FOREACH(string * str, uiImgFiles_)	
	{
		string relFileName = convertToRelativePath(curDir, *str);
		writer.write(relFileName, "ui image");
	}

	writer.write(width_, "width");
	writer.write(height_, "height");

	writer.write(pageChangingMsg_, "pageChangingMsg");

	writer.write(doBookmarks_);	
	writer.write(doAspect_);

	for (int i = 0 ; i < thumbFiles_.size() ; i++)
	{
		string relFileName = convertToRelativePath(curDir, thumbFiles_.find(i)->second);
		writer.write(relFileName, "thumbnail");
	}
}


void CatalogDocumentTemplate::read(Reader & reader, unsigned char version)
{
	reader.readParent<DocumentTemplate>(this);
	reader.read(visualAttrib_);
	reader.read(scenes_);

	if (version >= 1)
	{
		for (int i = 0; i < 9; ++i) reader.read(*uiImgFiles_[i]);
	}
	else
	{
		for (int i = 0; i < 8; ++i) reader.read(*uiImgFiles_[i]);
	}

	reader.read(width_);
	reader.read(height_);


	reader.read(pageChangingMsg_);

	reader.read(doBookmarks_);
	reader.read(doAspect_);

	for (int i = 0 ; i < scenes_.size() ; i++)
	{
		string temp;
		reader.read(temp);
		thumbFiles_[i] = temp;
	}
}

void CatalogDocumentTemplate::writeXml(XmlWriter & w) const
{
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	w.startTag("Scenes");
	for (int i = 0; i < (int)scenes_.size(); ++i)
	{
		w.writeTag("Scene", w.idMapping().getId(scenes_[i]));
	}
	w.endTag();
	string curDir = getCurDir();
	w.startTag("UiImgFiles");
	BOOST_FOREACH(string * str, uiImgFiles_)	
	{
		string relFileName = convertToRelativePath(curDir, *str);
		w.writeTag("FileName", relFileName);
	}
	w.endTag();
	w.writeTag("Width", width_);
	w.writeTag("Height", height_);
	w.writeTag("PageChangingMsg", pageChangingMsg_);
	w.writeTag("Bookmarks", doBookmarks_);
	w.writeTag("Aspect", doAspect_);
	char* c;
	w.startTag("Thumbnails");
	for (int i = 0 ; i < thumbFiles_.size() ; i++)
	{
		string relFileName = convertToRelativePath(curDir, thumbFiles_.find(i)->second);
		sprintf(c, "%d", i);
		string attrStr(c);
		attrStr = "Page = \"" + attrStr + "\"";
		w.writeTag("FileName", attrStr, relFileName);
	}
	w.endTag();
}

void CatalogDocumentTemplate::readXml(XmlReader & r, xmlNode * parent)
{
	unsigned int val;
	thumbFiles_.clear();
	uiImgFiles_.clear();
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.isNodeName(curNode, "Scenes"))
		{
			for(xmlNode * sceneNode = curNode->children; sceneNode; sceneNode = sceneNode->next)
			{		
				if (sceneNode->type != XML_ELEMENT_NODE) continue;
				if (r.getNodeContentIfName(val, curNode, "Scene"))
				{
					Scene* scene = r.idMapping().getScene(val);
					scenes_.push_back(scene);
				}
			}
		}
		else if (r.isNodeName(curNode, "UiImgFiles"))
		{
			int index = 0;
			string fileName;
			for(xmlNode * fileNode = curNode->children; fileNode; fileNode = fileNode->next)
			{		
				if (fileNode->type != XML_ELEMENT_NODE) continue;				
				if (r.getNodeContentIfName(fileName, fileNode, "FileName"))
				{
					if (index < uiImgFiles_.size())
					{
						*uiImgFiles_[index] = fileName;
						++index;
					}					
				}
			}	
		}
		else if (r.getNodeContentIfName(width_, curNode, "Width"));
		else if (r.getNodeContentIfName(height_, curNode, "Height"));
		else if (r.getNodeContentIfName(pageChangingMsg_, curNode, "PageChangingMsg"));
		else if (r.getNodeContentIfName(doBookmarks_, curNode, "Bookmarks"));
		else if (r.getNodeContentIfName(doAspect_, curNode, "Aspect"));
		else if (r.isNodeName(curNode, "Thumbnails"))
		{
			int page = 0;
			string fileName;
			for(xmlNode * fileNode = curNode->children; fileNode; fileNode = fileNode->next)
			{		
				if (fileNode->type != XML_ELEMENT_NODE) continue;				
				if (r.getNodeContentIfName(fileName, fileNode, "FileName"))
				{
					r.getNodeAttribute(page, curNode, "Page");
					thumbFiles_[page] = fileName;
				}
			}
		}
	}
}

void CatalogDocumentTemplate::setUiImgFiles(const std::vector<std::string> & uiImgFiles)
{
	int index = 0;
	topMenuLayoutButtonFile_ = uiImgFiles[index++];
	//thumbnailNormalOpenedFile_ = uiImgFiles[index++];
	//thumbnailNormalClosedFile_ = uiImgFiles[index++];
	//thumbnailWidthOpenedFile_ = uiImgFiles[index++];
	//thumbnailWidthClosedFile_ = uiImgFiles[index++];
	thumbnailNormalBarFile_ = uiImgFiles[index++];
	thumbnailWidthBarFile_ = uiImgFiles[index++];
	thumbnailNormalClosedFile_ = uiImgFiles[index++];
	thumbnailNormalOpenedFile_ = uiImgFiles[index++];
	bookmarkCheckFile_ = uiImgFiles[index++];
	thumbnailShadowFile_ = uiImgFiles[index++];
	thumbnailWidthClosedFile_ = uiImgFiles[index++];
	thumbnailWidthOpenedFile_ = uiImgFiles[index++];
	//toContentsFile_ = uiImgFiles[index++];
}

void CatalogDocumentTemplate::uiImgFiles(std::vector<std::string> * uiImgFiles)
{
	uiImgFiles->clear();
	uiImgFiles->push_back(topMenuLayoutButtonFile_);
	//uiImgFiles->push_back(thumbnailNormalOpenedFile_);
	//uiImgFiles->push_back(thumbnailNormalClosedFile_);
	//uiImgFiles->push_back(thumbnailWidthOpenedFile_);
	//uiImgFiles->push_back(thumbnailWidthClosedFile_);
	uiImgFiles->push_back(thumbnailNormalBarFile_);
	uiImgFiles->push_back(thumbnailWidthBarFile_);
	uiImgFiles->push_back(thumbnailNormalClosedFile_);
	uiImgFiles->push_back(thumbnailNormalOpenedFile_);
	uiImgFiles->push_back(bookmarkCheckFile_);
	uiImgFiles->push_back(thumbnailShadowFile_);
	uiImgFiles->push_back(thumbnailWidthClosedFile_);
	uiImgFiles->push_back(thumbnailWidthOpenedFile_);
	//uiImgFiles->push_back(toContentsFile_);
}

void CatalogDocumentTemplate::setThumbnailFile(int key, const std::string file)
{
	thumbFiles_.erase(key);
	thumbFiles_.insert(pair<int, std::string>(key, file));
}

void CatalogDocumentTemplate::setThumbFileMap(const std::map<int, std::string> & thumbFileMap)
{
	thumbFiles_.clear();
	for (int i = 0 ; i < thumbFileMap.size() ; i++)
	{
		thumbFiles_[i] = thumbFileMap.find(i)->second;
	}
}

void CatalogDocumentTemplate::thumbFileMap( std::map<int, std::string> * thumbFileMap)
{
	//std::string temp;
	for (int i = 0 ; i < thumbFiles_.size() ; i++)
	{
		thumbFileMap->erase(i);
		thumbFileMap->insert(pair<int, std::string>(i, thumbFiles_.find(i)->second));
	}
}

void CatalogDocumentTemplate::reInitCurScene(GfxRenderer * gl)
{
	if (isLandscape_)
	{
		if (!landSubscene_->leftpage_->isInit()) return;
		landSubscene_->leftpage_->resizeText(gl);
		if (landSubscene_->rightpage_->isInit()) 
			landSubscene_->rightpage_->resizeText(gl);
		for(int i = 0 ; i < 2 ; i++)
		{
			if (landSubsceneCached_[i]->leftpage_->isInit())
				landSubsceneCached_[i]->leftpage_->resizeText(gl);
			if (landSubsceneCached_[i]->rightpage_->isInit()) 
				landSubsceneCached_[i]->rightpage_->resizeText(gl);
		}
	}
	else
	{
		if (!subscene_->isInit()) return;
		subscene_->resizeText(gl);
		for(int i = 0 ; i < SUBSCENECOUNT ; i++)
		{
			if (!subsceneCached_[i]->isInit()) return;
			subsceneCached_[i]->resizeText(gl);
		}
	}
}

bool CatalogDocumentTemplate::checkOutside(Transform & t, bool hasDirectionScene)
{
	float normalScaleX;
	float normalScaleY;
	if (subscene_->scene() && !isLandscape_)
	{
		normalScaleX = subscene_->normalScaleX();
		normalScaleY = subscene_->normalScaleY();
	}
	else if (landSubscene_->leftpage_->scene() && isLandscape_)
	{
		normalScaleX = landSubscene_->leftpage_->normalScaleX();
		normalScaleY = landSubscene_->leftpage_->normalScaleY();
	}
	else
	{
		normalScaleX = Global::instance().magnification();
		normalScaleY = Global::instance().magnification();        
	}
	bool checkchange = false;
	Matrix mat = t.computeMatrix();
	mat = mat.inverse();
	if (mat._14 * normalScaleX < 0 && (!(pageLeft()) || hasDirectionScene))
	{
		Matrix m = t.computeMatrix();
		m._14 = 0;
		mat = m.inverse();
		t.setMatrix(m, true);
		checkchange = true;
	}
	if (!isLandscape_)
	{
		if(mat._14  > subscene_->sceneWidth()  - (subscene_->sceneWidth() * normalScaleX / t.scaling().x) && (!(pageRight()) || hasDirectionScene))
		{
			Matrix m = mat;
			m._14 = (subscene_->sceneWidth() * normalScaleX - (subscene_->sceneWidth() * normalScaleX / t.scaling().x * normalScaleX)) / normalScaleX;
			mat = m;
			m = m.inverse();
			t.setMatrix(m, true);
			checkchange = true;
		}
	}
	else
	{
		if (pageIndex_ == 0 || pageIndex_ == scenes_.size() - 1)
		{
//			if(mat._14 > (landSubscene_->leftpage_->sceneWidth()) - (landSubscene_->leftpage_->sceneWidth() * normalScaleX / t.scaling().x) && (!(pageRight()) || hasDirectionScene))
//			{
//				Matrix m = mat;
//				m._14 = ((landSubscene_->leftpage_->sceneWidth() * normalScaleX) - (landSubscene_->leftpage_->sceneWidth() * normalScaleX / t.scaling().x * normalScaleX)) / normalScaleX;
//				mat = m;
//				m = m.inverse();
//				t.setMatrix(m, true);
//				checkchange = true;
//			}
            float windwowwidth = landSubscene_->leftpage_->windowWidth() * t.scaling().x / normalScaleX;
            float landx = (width_ - windwowwidth)/2;
            Matrix m = mat;
            m = m.inverse();
            m._14 = landx;
            t.setMatrix(m, true);
            checkchange = true;
            mat = m.inverse();
		}
		else
		{
			if(mat._14 > (landSubscene_->leftpage_->sceneWidth() *2) - (landSubscene_->leftpage_->windowWidth() / t.scaling().x * 2) && (!(pageRight()) || hasDirectionScene))
			{
				Matrix m = mat;
				m._14 = ((landSubscene_->leftpage_->sceneWidth() *2) - (landSubscene_->leftpage_->windowWidth() / t.scaling().x * 2));
				mat = m;
				m = m.inverse();
				t.setMatrix(m, true);
				checkchange = true;
			}
		}
	}
	if(mat._24 * normalScaleY < 0)
	{
//		Matrix m = t.computeMatrix();
//        m = m.inverse();
        Matrix m = mat;
		m._24 = 0;
        m = m.inverse();
		t.setMatrix(m, true);
		checkchange = true;
	}
	if (!isLandscape_)
	{
		if(mat._24 * normalScaleY > subscene_->sceneHeight() * normalScaleY - (subscene_->sceneHeight() * normalScaleY / t.scaling().y * normalScaleY) )
		{
			Matrix m = mat;
			m._24 = (subscene_->sceneHeight() * normalScaleY - (subscene_->sceneHeight() * normalScaleY / t.scaling().y * normalScaleY)) / normalScaleY;
			m = m.inverse();
			t.setMatrix(m, true);
			checkchange = true;
		}
	}
	else
	{
		if(mat._24 * normalScaleY > landSubscene_->leftpage_->sceneHeight() * normalScaleY - (landSubscene_->leftpage_->sceneHeight() * normalScaleY / t.scaling().y * normalScaleY) )
		{
			Matrix m = mat;
			m._24 = (landSubscene_->leftpage_->sceneHeight() * normalScaleY - (landSubscene_->leftpage_->sceneHeight() * normalScaleY / t.scaling().y * normalScaleY)) / normalScaleY;
			m = m.inverse();
			t.setMatrix(m, true);
			checkchange = true;
		}
	}
	return checkchange;
}


void CatalogDocumentTemplate::resetSaveLocalRequest()
{
    Global & g = Global::instance();
    g.resetSaveLocalRequest();
	resetComplete_ = false;
	searchQ_.clear();
	int searchDepth;
	if(scenes_.size()/2 > pageIndex_)
		searchDepth = scenes_.size() - pageIndex_;
	else
		searchDepth = pageIndex_;

	for(int i = 0 ; i < searchDepth ; i++)
	{
		if (pageIndex_+i < scenes_.size() && i!=0)
			searchQ_.push_back(scenes_[pageIndex_+i]);
		if (pageIndex_-i >= 0 && i!=0)
			searchQ_.push_back(scenes_[pageIndex_-i]);
	}
	resetComplete_ = true;
}

void CatalogDocumentTemplate::loadZoomImageRequest()
{
	std::vector<SceneObject *> images;
	static boost::uuids::uuid type = Image().type();
	subscene_->scene()->getSceneObjectsByID(type, &images);
	BOOST_FOREACH(SceneObject * obj, images)
	{
		Image* img = (Image*)obj;
		img->requestZoom();
	}
}


void CatalogDocumentTemplate::clickThumbNailLayout()
{
	layoutYMoveTween_.reset();

	//changeLandscape();
	float startpos, endpos;

	float thumbscale;
	if (isLandscape_)
	{
		thumbscale = width_ / 2048;
	}
	else
	{
		thumbscale = width_ / 1536;
	}

	if (!isOpenedThumb_)
	{
		//if (isLandscape_)
		//{
		//	startpos = height_ - 62 * thumbscale;// * dpiScale_;
		//	//endpos = height_ - (thumbnailWidthClosed_->height() /** dpiScale_*/ * (width_ / thumbnailWidthClosed_->width()));
		//	endpos = height_ - (thumbnailWidth_->height() /** dpiScale_*/ * (width_ / thumbnailWidth_->width()));
		//}
		//else
		//{
		//	startpos = height_ - 62 * thumbscale;//* dpiScale_;
		//	//endpos = height_ /*- (height_ * thumbnailScale_)*/ - (thumbnailNormalClosed_->height() /** dpiScale_*/ * (width_ / thumbnailNormalClosed_->width()));
		//	endpos = height_ /*- (height_ * thumbnailScale_)*/ - (thumbnailNormal_->height() /** dpiScale_*/ * (width_ / thumbnailNormal_->width()));
		//}
		endpos = height_ - thumbnailOpened_->height() * dpiScale_ * thumbscale - thumbnailNormal_->height() * thumbscale * dpiScale_;

		startpos = layoutOffsetY_; //= height_ - thumbnailOpened_->height() * dpiScale_ * thumbscale;
	}
	else
	{
		startpos = layoutOffsetY_;
		endpos = height_ - thumbnailOpened_->height() * dpiScale_ * thumbscale;
		//if (isLandscape_)
		//	endpos = height_ - 62 * thumbscale;//* dpiScale_;
		//else
		//	endpos = height_ - 62 * thumbscale;//* dpiScale_;
	}
	layoutYMoveTween_.start(Global::currentTime(), startpos, endpos);
}

void CatalogDocumentTemplate::setButtonTransform()
{
	float thumbscale;
	if (isLandscape_)
	{
		thumbscale = width_ / 2048;
	}
	else
	{
		thumbscale = width_ / 1536;
	}
	float btnWidth, btnHight;
	btnWidth = 130 * thumbscale * dpiScale_;
	btnHight = 72 * thumbscale * dpiScale_;
	menuLayoutButton_->setWidth(btnWidth);
	menuLayoutButton_->setHeight(btnHight);
	Transform t = menuLayoutButton_->transform();

	t.setTranslation(
		width_/2 - btnWidth/2,
		layoutOffsetY_,
		0.0f);
	menuLayoutButton_->setTransform(t);
}

void CatalogDocumentTemplate::changeLandscape()
{
	//Matrix m = Matrix::Identity();
	//Transform t = landSubscene_->leftpage_->transform();
	//t.setPivot(0,0,0);
	//t.setMatrix(m, true);
	Transform t;
	Global & g = Global::instance();
    float thumbscale;
    float width = width_;
    
	if (isLandscape_)
	{
        if (width_ > height_)
            width = height_;
		thumbscale = width / 1536;
	}
	else
	{
        if (width_ < height_)
            width = height_;
		thumbscale = width / 2048;
	}
	if (isLandscape_)
	{
		g.requestLocalSaveObjectFirst(thumbFiles_.find(pageIndex_)->second);
		if (landSubscene_->leftpage_) 
			subSceneBackup_[SUBSCENECOUNT+1]->setScene(landSubscene_->leftpage_->scene());
		if (landSubscene_->rightpage_) 
			subSceneBackup_[SUBSCENECOUNT+2]->setScene(landSubscene_->rightpage_->scene());
		for(int i = 0 ; i < 2 ; i++)
		{
			if (landSubsceneCached_[i]->leftpage_) 
				subSceneBackup_[i+SUBSCENECOUNT+3]->setScene(landSubsceneCached_[i]->leftpage_->scene());
			if (landSubsceneCached_[i]->rightpage_) 
				subSceneBackup_[i+SUBSCENECOUNT+5]->setScene(landSubsceneCached_[i]->rightpage_->scene());
		}
		subscene_->setTransform(t);
		for(int i = 0 ; i < SUBSCENECOUNT ; i++)
			subsceneCached_[i]->setTransform(t);
		subscene_->setScene(scenes_[pageIndex_]);

		subsceneCached_[0]->setScene(pageRight());
		subsceneCached_[1]->setScene(pageLeft());
		for (int i = 2 ; i < SUBSCENECOUNT ; i++)
		{
			if (i%2 == 0)
			{
				if (pageIndex_ < (int)scenes_.size() - i)
					subsceneCached_[i]->setScene(scenes_[pageIndex_ + i]);
				else
					subsceneCached_[i]->setScene(0);
			}
			else
			{
				if (pageIndex_ > i-pageIndex_)
					subsceneCached_[i]->setScene(scenes_[pageIndex_ - i + 1]);
				else
					subsceneCached_[i]->setScene(0);
			}
		}
		landSubscene_->rightpage_->setScene(0);
		landSubscene_->leftpage_->setScene(0);
		for (int i = 0 ; i < 2 ; i++)
		{
			landSubsceneCached_[i]->leftpage_->setScene(0);
			landSubsceneCached_[i]->rightpage_->setScene(0);
		}
		isLandscape_ = false;
        float height = height_;
        if (width_ > height_)
            height = width_;
		if (!thumbnailOpened_->isLoaded())
			layoutOffsetY_ = height_ - 64 * dpiScale_ * thumbscale;
        else if (isOpenedThumb_)
            layoutOffsetY_ = height - thumbnailOpened_->height() * dpiScale_ * thumbscale - thumbnailNormal_->height() * thumbscale * dpiScale_;
        else
            layoutOffsetY_ = height - thumbnailOpened_->height() * dpiScale_ * thumbscale;
		//if (isOpenedThumb_)
		//	//layoutOffsetY_ = height_ - thumbnailNormalClosed_->height() /** dpiScale_*/ *  (width_ / thumbnailNormalClosed_->width());
		//	layoutOffsetY_ = height_ - thumbnailNormal_->height() /** dpiScale_*/ *  (width_ / thumbnailNormal_->width());
		//else
		//	//layoutOffsetY_ = height_ - 62 *  (width_ / thumbnailNormalOpened_->width());// * dpiScale_;
		//	layoutOffsetY_ = height_ - 62 *  (width_ / thumbnailNormal_->width());// * dpiScale_;
	}
	else
	{
		if (subscene_) subSceneBackup_[0]->setScene(subscene_->scene());
		for(int i = 0 ; i < SUBSCENECOUNT ; i++)
			if (subsceneCached_[i]) subSceneBackup_[i+1]->setScene(subsceneCached_[i]->scene());

		landSubscene_->leftpage_->setTransform(t);
		landSubscene_->rightpage_->setTransform(t);
		for (int i = 0 ; i < 2 ; i++)
		{
			landSubsceneCached_[i]->leftpage_->setTransform(t);
			landSubsceneCached_[i]->rightpage_->setTransform(t);
		}
		if (pageIndex_ % 2 == 0)
		{
			g.requestLocalSaveObjectFirst(thumbFiles_.find(pageIndex_)->second);
			if (pageIndex_ == 0)
			{
				landSubscene_->leftpage_->setScene(scenes_[pageIndex_]);
				landSubscene_->leftpage_->setViewState(InternalSubscene::LandscapeCenter);
				landSubscene_->rightpage_->setScene(0);
			}
			else
			{
				g.requestLocalSaveObjectFirst(thumbFiles_.find(pageIndex_-1)->second);
				landSubscene_->rightpage_->setScene(scenes_[pageIndex_]);
				landSubscene_->leftpage_->setScene(scenes_[pageIndex_-1]);
				landSubscene_->leftpage_->setViewState(InternalSubscene::LandscapeLeft);
				landSubscene_->rightpage_->setViewState(InternalSubscene::LandscapeRight);
			}
			if (1 > pageIndex_-2)
			{
				if (pageIndex_ == 0)
					landSubsceneCached_[0]->leftpage_->setScene(0);
				else
					landSubsceneCached_[0]->leftpage_->setScene(scenes_[0]);
				landSubsceneCached_[0]->rightpage_->setScene(0);
			}
			else
			{
				landSubsceneCached_[0]->leftpage_->setScene(scenes_[pageIndex_-3]);
				landSubsceneCached_[0]->rightpage_->setScene(scenes_[pageIndex_-2]);
			}
			if (scenes_.size() > pageIndex_+1)
				landSubsceneCached_[1]->leftpage_->setScene(scenes_[pageIndex_+1]);
			else
				landSubsceneCached_[1]->leftpage_->setScene(0);
			if (scenes_.size() > pageIndex_+2)
				landSubsceneCached_[1]->rightpage_->setScene(scenes_[pageIndex_+2]);
			else
				landSubsceneCached_[1]->rightpage_->setScene(0);
		}
		else
		{
			g.requestLocalSaveObjectFirst(thumbFiles_.find(pageIndex_)->second);
			landSubscene_->leftpage_->setScene(scenes_[pageIndex_]);
			if (scenes_.size() > pageIndex_+1)
			{
				g.requestLocalSaveObjectFirst(thumbFiles_.find(pageIndex_+1)->second);
				landSubscene_->rightpage_->setScene(scenes_[pageIndex_+1]);
				landSubscene_->leftpage_->setViewState(InternalSubscene::LandscapeLeft);
				landSubscene_->rightpage_->setViewState(InternalSubscene::LandscapeRight);
			}
			else
			{
				landSubscene_->rightpage_->setScene(0);
				landSubscene_->leftpage_->setViewState(InternalSubscene::LandscapeCenter);
			}

			if (1 > pageIndex_ - 1)
			{
				landSubsceneCached_[0]->leftpage_->setScene(scenes_[0]);
				landSubsceneCached_[0]->rightpage_->setScene(0);
			}
			else
			{
				landSubsceneCached_[0]->leftpage_->setScene(scenes_[pageIndex_-2]);
				landSubsceneCached_[0]->rightpage_->setScene(scenes_[pageIndex_-1]);
			}
			if (scenes_.size() > pageIndex_+2)
				landSubsceneCached_[1]->leftpage_->setScene(scenes_[pageIndex_+2]);
			else
				landSubsceneCached_[1]->leftpage_->setScene(0);
			if (scenes_.size() > pageIndex_+3)
				landSubsceneCached_[1]->rightpage_->setScene(scenes_[pageIndex_+3]);
			else
				landSubsceneCached_[1]->rightpage_->setScene(0);
		}
		subscene_->setScene(0);
		for(int i = 0 ; i < SUBSCENECOUNT ; i++)
			subsceneCached_[i]->setScene(0);
		isLandscape_ = true;
        
        float height = height_;
        if (width_ < height_)
            height = width_;
		if (!thumbnailWidthOpened_->isLoaded())
			layoutOffsetY_ = height_ - 64 * dpiScale_ * thumbscale;
        else if (isOpenedThumb_)
            layoutOffsetY_ = height - thumbnailWidthOpened_->height() * dpiScale_ * thumbscale - thumbnailWidth_->height() * thumbscale * dpiScale_;
        else
            layoutOffsetY_ = height - thumbnailWidthOpened_->height() * dpiScale_ * thumbscale;
		//if (isOpenedThumb_)
		//	//layoutOffsetY_ = height_ - thumbnailWidthClosed_->height() /** dpiScale_*/ *  (width_ / thumbnailWidthClosed_->width());
		//	layoutOffsetY_ = height_ - thumbnailWidth_->height() /** dpiScale_*/ *  (width_ / thumbnailWidth_->width());
		//else
		//	//layoutOffsetY_ = height_ - 62 * (width_ / thumbnailWidthOpened_->width());// * dpiScale_;
		//	layoutOffsetY_ = height_ - 62 * (width_ / thumbnailWidth_->width());// * dpiScale_;
	}
    //cout << "change landscape layoutOffsetY_: " << layoutOffsetY_ << endl;
	setSubsceneScaling();

	doCurPageStart();

	for (int i = 0; i < (int)subSceneBackup_.size(); ++i)
		subSceneBackup_[i]->setScene(0);

	setCurPagePos(Vector3(0, 0, 0));
    
    float temp = pageIndex_;
    layoutX_ = -((thumbWidth() + (thumbWidth() * gapFactor_)) * dpiScale_) * pageIndex_;
    scrollPos_ = temp / (scenes_.size()-1);

	correctLandscapeSceneImage();

	redrawTriggered_ = true;

	setButtonTransform();
}

void CatalogDocumentTemplate::correctLandscapeSceneImage()
{
	Transform t;
	for (int i = 0 ; i < scenes_.size() ; i++)
	{
		if (!(i == 0 || (i == scenes_.size()-1 && i%2 == 1)))
		{
			t = scenes_[i]->root()->transform();
			if (isLandscape_)
			{
				switch(i%2)
				{
				case 0:
					t.setTranslation(t.translation().x - landscapeMovingTranslation_,  t.translation().y, t.translation().z);
					break;
				case 1:
					t.setTranslation(t.translation().x + landscapeMovingTranslation_,  t.translation().y, t.translation().z);
					break;
				}
			}
			else
			{
				Matrix m = Matrix::Identity();
				t.setMatrix(m, true);
			}
			scenes_[i]->root()->setTransform(t);
		}
	}
}

void CatalogDocumentTemplate::changeLandscape(const bool & var)
{
	if (var != isLandscape_)
		fadeIn_ = true;
}

void CatalogDocumentTemplate::goToPage(const int & index) 
{
	float temp = index;
    layoutX_ = -((thumbWidth() + (thumbWidth() * gapFactor_)) * dpiScale_) * index;
    scrollPos_ = temp / (scenes_.size()-1);
	sceneChangeTo(index);
}