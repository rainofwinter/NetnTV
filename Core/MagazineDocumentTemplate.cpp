#include "stdafx.h"
#include "InternalSubscene.h"
#include "MagazineDocumentTemplate.h"
#include "Mesh.h"
#include "Image.h"
#include "Texture.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Xml.h"
#include "Scene.h"
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
#include "PageFlip.h"
#include "PageFlipOrig.h"

#ifdef USE_FBO
#include "TextureRenderer.h"
#endif

#include "VideoPlayer.h"
#include "AudioPlayer.h"

#include "PageChangingEvent.h"

#include "SceneChanger.h"
#include "ImageChanger.h"

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
	float t = (abs(vPosition.x) - 0.15)/(1.0 - 0.15);\n\
	float alpha = clamp(1.0 - t, 0.0, 1.0);\n\
	t = (abs(vPosition.y) - 0.175)/(0.875 - 0.175);\n\
	alpha *= clamp(1.0 - t, 0.0, 1.0);\n\
	gl_FragColor.a = alpha;\n\
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



const char * bookmarksFile = "__bookmarks.dat";

///////////////////////////////////////////////////////////////////////////////


void MagazineDocumentTemplate::BookmarkData::write(Writer & writer) const
{
	writer.write(articleIndex);
	writer.write(pageIndex);
	writer.write(text);
}
void MagazineDocumentTemplate::BookmarkData::read(Reader & reader, unsigned char version)
{
	reader.read(articleIndex);
	reader.read(pageIndex);
	reader.read(text);
}
	
///////////////////////////////////////////////////////////////////////////////
JSClass jsMagazineTemplateClass = InitClass(
	"MagazineTemplate", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, 0, 0);

JSBool MagazineTemplate_changeScene(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	MagazineDocumentTemplate * thisObj = 
		(MagazineDocumentTemplate *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsScene;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsScene)) return JS_FALSE;
	
	if (JS_GetPrototype(cx, jsScene) == s->sceneProto())
	{
		Scene * scene = (Scene *)JS_GetPrivate(cx, jsScene);
		thisObj->sceneChangeTo(scene);
	}

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool MagazineTemplate_changeSceneIndex(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	MagazineDocumentTemplate * thisObj = 
		(MagazineDocumentTemplate *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	uint32 jsArticleIndex, jsPageIndex;
	if (!JS_ConvertArguments(cx, argc, argv, "uu", &jsArticleIndex, &jsPageIndex)) return JS_FALSE;
	thisObj->sceneChangeTo((int)jsArticleIndex, (int)jsPageIndex);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};



JSFunctionSpec MagazineTemplateFuncs[] = {
	JS_FS("changeScene", MagazineTemplate_changeScene, 1, 0),
	JS_FS("changeSceneIndex", MagazineTemplate_changeSceneIndex, 1, 0),
    JS_FS_END
};

///////////////////////////////////////////////////////////////////////////////
JSObject * MagazineDocumentTemplate::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseDocumentTemplateProto(), &jsMagazineTemplateClass,
		0, 0, 0, MagazineTemplateFuncs, 0, 0);

	return proto;
}

JSObject * MagazineDocumentTemplate::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsMagazineTemplateClass, s->documentTemplateProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);
	}

	return scriptObject_;
}


void MagazineDocumentTemplate::create()
{
	init_ = false;
	thumbnailScale_ = 0.225f;
	gapFactor_ = 0.15f;
	layoutX_ = 0;
	redrawTriggered_ = false;

	scrollPos_ = 0;

	width_ = 768;
	height_ = 1024;

	subscene_.reset(new InternalSubscene);
	subsceneCached_.reset(new InternalSubscene);
	subsceneTransition_.reset(new InternalSubscene);
	
	mode_ = PageMode;

	articleIndex_ = 0;
	pageIndex_ = 0;

	uiImgFiles_.push_back(&scrollBarBgFile_);
	uiImgFiles_.push_back(&scrollBarLeftSideFile_);
	uiImgFiles_.push_back(&scrollBarRightSideFile_);
	uiImgFiles_.push_back(&topMenuBgFile_);
	uiImgFiles_.push_back(&topMenuLayoutButtonFile_);
	uiImgFiles_.push_back(&topMenuHomeButtonFile_);
	uiImgFiles_.push_back(&scrollBarThumbFile_);
	uiImgFiles_.push_back(&bgFile_);
	uiImgFiles_.push_back(&bookmarkActiveFile_);
	uiImgFiles_.push_back(&bookmarkInactiveFile_);
	uiImgFiles_.push_back(&bookmarkListFile_);
	uiImgFiles_.push_back(&bookmarkTitleFile_);
	uiImgFiles_.push_back(&aspectActiveFile_);
	uiImgFiles_.push_back(&aspectInactiveFile_);
	uiImgFiles_.push_back(&toContentsFile_);
	

	for (int i = 0; i < 5; ++i)
		subSceneBackup_.push_back(new InternalSubscene);


	scrollBarBg_.reset(new Texture);
	scrollBarLeft_.reset(new Texture);
	scrollBarRight_.reset(new Texture);
	bg_.reset(new Texture);		
	bookmarkTitle_.reset(new Texture);
	scrollBarThumb_.reset(new Image);

	moveDirection_ = Undetermined;
	scrollThumbGrabbed_ = false;

	pageChangingMsg_ = "pageChanging";

	//make sure the first setCurPage call goes through.
	//(it first checks to see if article/pageIndex are different from the 
	//passed in values and -1 is different from everything) 
	pageIndex_ = -1;
	articleIndex_ = -1;	

	displayingAppObjects_ = false;

	thumbsLoadedConfirmed_ = false;
#ifdef USE_FBO
	texRenderer_ = new TextureRenderer;
	renderTex_ = new Texture;
#else
	ssBuffer_ = 0;
	renderTex_ = 0;
#endif
	transitionStage_ = TransitionInactive;

	layoutOffsetY_ = 0;


	//menu objects
	menuBg_.reset(new Image);
	menuLayoutButton_.reset(new Image);
	menuHomeButton_.reset(new Image);
	menuBookmarkActiveButton_.reset(new Image);
	menuBookmarkInactiveButton_.reset(new Image);
	menuBookmarkListButton_.reset(new Image);
	menuAspectActiveButton_.reset(new Image);
	menuAspectInactiveButton_.reset(new Image);
	menuToContentsButton_.reset(new Image);


	menuImgs_.clear();
	menuImgs_.push_back(menuBg_.get());
	menuImgs_.push_back(menuLayoutButton_.get());
	menuImgs_.push_back(menuHomeButton_.get());
	menuImgs_.push_back(menuBookmarkActiveButton_.get());
	menuImgs_.push_back(menuBookmarkInactiveButton_.get());
	menuImgs_.push_back(menuAspectActiveButton_.get());
	menuImgs_.push_back(menuAspectInactiveButton_.get());
	menuImgs_.push_back(menuBookmarkListButton_.get());
	menuImgs_.push_back(menuToContentsButton_.get());

	BOOST_FOREACH(Image * img, menuImgs_)
	{
		VisualAttrib * attr = img->visualAttrib();
		attr->setOpacity(0.0f);
	}

	menuOpacity_ = 0;
	menuStartOpacity_ = 0;
	targetMenuOpacity_ = 0;
	menuTransitionStartTime_ = 0;
	menuHideTime_ = FLT_MAX;
	menuDuration_ = 3;

	menuOpacity_ = 0;
	targetMenuOpacity_ = 0;

	//loading page
	loadingPage_.reset(new LoadingScreen);
	curPageStarted_ = false;

	curPageHasBookmark_ = false;		
	bookmarksLastTopIndex_ = -1;
	bookmarksTopIndex_ = -1;
	bookmarksLastScrollY_ = FLT_MAX;

	doBookmarks_ = false;
	doAspect_ = false;
	homeButtonFunction_ = HomeButtonExitToViewerLayer;

	layoutModeSnappedBackYet_ = false;
	requiredLayoutMoveDirection_ = Undetermined;
	startLayoutOffsetY_ = 0.0f;

	preserveAspect_ = true;	
	resetArticleFirstPage_ = false;
	pageMovingId_ = -1;

	toContentsPage_ = false;

	transitionMode_ = SlidingMode;
	pageFlip_ = NULL;

	doBookmarks_ = false;
	homeButtonFunction_ = HomeButtonFirstPage;
	sceneCheck_ = false;

	resetComplete_ = false;
	searchQ_.clear();
}

void MagazineDocumentTemplate::doCurPageStart()
{
	curPageStarted_ = false;
	//force refresh of isLoaded flag
	//this helps prevents glitches on next draw call
	if (subscene_->isInit()) subscene_->asyncLoadUpdate(); 	
}

bool MagazineDocumentTemplate::pageHasBookmark(int articleIndex, int pageIndex) const
{

	BOOST_FOREACH(const BookmarkData & bookmark, bookmarks_)
	{
		if (bookmark.articleIndex == articleIndex && bookmark.pageIndex == pageIndex) return true;
	}

	return false;
}


void MagazineDocumentTemplate::addBookmark(const BookmarkData & data)
{
	bookmarks_.push_back(data);
	writeBookmarks();
	populateBookmarksScene();
}

void MagazineDocumentTemplate::populateBookmarksScene()
{
	float thumbnailWidth = width_ * thumbnailScale_;
	float thumbnailHeight = height_ * thumbnailScale_;
	float gap = gapFactor_ * thumbnailWidth;

	bool wasInit = bookmarksContentsScene_->isInit();
	bookmarksContentsScene_->root()->deleteChildren();
	bookmarkThumbImgs_.clear();

	int numThumbsForPage = height_ / (thumbnailHeight + gap) + 2;
	int numTotalThumbs = (int)bookmarks_.size();
	float screenHeight = gap + numTotalThumbs * (thumbnailHeight + gap);
	bookmarksContentsScene_->setScreenHeight(screenHeight);	
	
	float curY = gap;
	float curX = gap;
	for (int i = 0; i < numThumbsForPage; ++i)
	{
		Image * img = new Image;
		bookmarkThumbImgs_.push_back(img);
		bookmarksContentsScene_->addElement(SceneObjectSPtr(img));
		Transform transform;
		transform.setTranslation(gap, curY, 0);
		img->setTransform(transform);
		img->setWidth(thumbnailWidth);
		img->setHeight(thumbnailHeight);
		curY += thumbnailHeight + gap;
	}
	bookmarksLastTopIndex_ = -1;
	bookmarksTopIndex_ = -1;
	bookmarksLastScrollY_ = FLT_MAX;

	tempBookmarkThumbImg_.reset(new Image);

	float scrollY = bookmarksScroller_->scrollPosition().y;
	float scrollerHeight = height_ - bookmarksY_;
	float scrollYForBottom = scrollerHeight - screenHeight;
	if (scrollY < scrollYForBottom && scrollYForBottom < 0)
		bookmarksScroller_->setScrollPosition(Vector2(0, height_ - bookmarksY_ - screenHeight));

	if (screenHeight < scrollerHeight)
		bookmarksScroller_->setScrollPosition(Vector2(0, 0));
	updateBookmarksScene();
	if (wasInit) bookmarksContentsScene_->init(document_->renderer());
}

void MagazineDocumentTemplate::updateBookmarksScene()
{
	float scrollY = bookmarksScroller_->scrollPos().y;
	if (scrollY == bookmarksLastScrollY_) return;

	float thumbnailWidth = width_ * thumbnailScale_;
	float thumbnailHeight = height_ * thumbnailScale_;
	float gap = gapFactor_ * thumbnailWidth;
	
	int numTotalThumbs = (int)bookmarks_.size();
	float screenHeight = gap + numTotalThumbs * (thumbnailHeight + gap);
	bookmarksContentsScene_->setScreenHeight(screenHeight);		

	float curX = gap;
	int c = int((-scrollY) / (thumbnailHeight + gap));
	if (c < 0) c = 0;
	bookmarksTopIndex_ = c;

	
	float curY = gap + (thumbnailHeight + gap)*bookmarksTopIndex_;
	for (int i = 0; i < (int)bookmarkThumbImgs_.size(); ++i)
	{
		Image * img = bookmarkThumbImgs_[i];
		Transform transform;
		transform.setTranslation(curX, curY, 0);
		img->setTransform(transform);
		img->setWidth(thumbnailWidth);
		img->setHeight(thumbnailHeight);
		
		bookmarkThumbImgs_[i]->visualAttrib()->setVisible(true);

		// float >= sometimes yields unexpected results
		if ((int)curY >= (int)screenHeight) 
			bookmarkThumbImgs_[i]->visualAttrib()->setVisible(false);

		curY += thumbnailHeight + gap;
	}	

	int imgIndex = 0;
	int maxImgIndex = (int)bookmarkThumbImgs_.size() - 1;

	bool thumbsLoadedLastTime  = bookmarksLastTopIndex_ >= 0;

	if (thumbsLoadedLastTime && bookmarksLastTopIndex_ != bookmarksTopIndex_)
	{		
		//move reusable thumbs without reloading them
		int bookmarkDelta = bookmarksTopIndex_ - bookmarksLastTopIndex_;
		Image tempImg;			

		if (bookmarkDelta > 0)
		{
			for (int i = 0; i < (int)bookmarkThumbImgs_.size() - bookmarkDelta; ++i)
			{
				bookmarkThumbImgs_[i + bookmarkDelta]->swap(tempBookmarkThumbImg_.get());
				bookmarkThumbImgs_[i]->swap(tempBookmarkThumbImg_.get());					
			}
			imgIndex = (int)bookmarkThumbImgs_.size() - bookmarkDelta;
		}
		else
		{
			for (int i = (int)bookmarkThumbImgs_.size() - 1; i >= -bookmarkDelta; --i)
			{
				bookmarkThumbImgs_[i + bookmarkDelta]->swap(tempBookmarkThumbImg_.get());
				bookmarkThumbImgs_[i]->swap(tempBookmarkThumbImg_.get());					
			}				
			maxImgIndex = -bookmarkDelta;
		}	
	}
	else if (thumbsLoadedLastTime && bookmarksLastTopIndex_ == bookmarksTopIndex_)
	{
		//skip new image file loading all together
		maxImgIndex = imgIndex - 1;
	}
			
	//indexes into bookmarks_
	int curIndex = bookmarksTopIndex_ + imgIndex;
	
	while (curIndex < (int)bookmarks_.size() && imgIndex <= maxImgIndex)
	{
		int articleIndex = bookmarks_[curIndex].articleIndex;
		int pageIndex = bookmarks_[curIndex].pageIndex;

		std::string newFileName = articles_[articleIndex]->page(pageIndex)->thumbnailFile();

		if (newFileName != bookmarkThumbImgs_[imgIndex]->fileName())
		{
			bookmarkThumbImgs_[imgIndex]->setFileName(newFileName, false);
			bookmarkThumbImgs_[imgIndex]->init(document_->renderer());			
		}

		curIndex++;
		imgIndex++;
	}



	bookmarksLastTopIndex_ = bookmarksTopIndex_;
	bookmarksLastScrollY_ = scrollY;
}

void MagazineDocumentTemplate::removeBookmark(int articleIndex, int pageIndex)
{
	vector<BookmarkData>::iterator iter;
	for (iter = bookmarks_.begin(); iter != bookmarks_.end(); ++iter)
	{
		const BookmarkData & bookmark = *iter;
		if (bookmark.articleIndex == articleIndex && bookmark.pageIndex == pageIndex)
		{
			bookmarks_.erase(iter);
			break;
		}
	}
	writeBookmarks();
	populateBookmarksScene();
}

void MagazineDocumentTemplate::setDocument(Document * document)
{
	DocumentTemplate::setDocument(document);
}

void MagazineDocumentTemplate::setSize(int width, int height, bool resizeGl)
{
	btnDim_ = int(38 * dpiScale_);	
	menuHeight_ = int(44 * dpiScale_);
	btnMargin_ = int(5 * dpiScale_);
	bookmarkTitleHeight_ = int(28 * dpiScale_);
	bookmarksY_ = menuHeight_ + bookmarkTitleHeight_;
	
	width_ = (float)width;
	height_ = (float)height;

	camera_.SetTo2DArea(width_/2, height_/2, width_, height_, 60.0f, width_/height_);
	
	setMenuTransform();

	//set subscene scaling and translation
	setSubsceneScaling();
	setCurPagePos(subscene_->transform().translation());

	setScrollPos(scrollPos_);

	setScrollBarThumbTransform();
	
	reInitCurScene(document_->renderer());

	if (pageFlip_) pageFlip_->setPageDims(width, height);	
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

	setBookmarkUiSize();
}

void MagazineDocumentTemplate::setBookmarkUiSize()
{
	if (bookmarksScroller_)
	{
		Transform transform;
		float menuHeight = 2*btnMargin_ + btnDim_;		
		transform.setTranslation(0, bookmarksY_, 0);
		bookmarksScroller_->setTransform(transform);		
		bookmarksScroller_->setWindowWidth(width_);
		bookmarksScroller_->setWindowHeight(height_ - bookmarksY_);
		bookmarksContentsScene_->setScreenWidth(width_);
		
		populateBookmarksScene();
	}
}

void MagazineDocumentTemplate::readBookmarks()
{
	std::string writeFileName = Global::instance().writeDirectory() + bookmarksFile;
	FILE * file = fopen(writeFileName.c_str(), "rb");

	if (file)
	{
		Reader(file).read(bookmarks_);
		fclose(file);
	}	
}

void MagazineDocumentTemplate::writeBookmarks()
{
	std::string writeFileName = Global::instance().writeDirectory() + bookmarksFile;
	Writer(writeFileName.c_str(), 0).write(bookmarks_);
}

MagazineDocumentTemplate::MagazineDocumentTemplate()
{
	create();
}


MagazineDocumentTemplate::~MagazineDocumentTemplate()
{
	uninit();
	BOOST_FOREACH(InternalSubscene * subscene, subSceneBackup_) delete subscene;

#ifdef USE_FBO
	delete texRenderer_;
	delete renderTex_;
#endif

}

Scene * MagazineDocumentTemplate::curScene() const
{
	return subscene_->scene();
}

Scene * MagazineDocumentTemplate::prevScene() const
{
	if (articleIndex_ == 0) return NULL;
	return articles_[articleIndex_ - 1]->curPagePtr();

}

Scene * MagazineDocumentTemplate::nextScene() const
{
	if (articleIndex_ >= (int)articles_.size() - 1) return NULL;
	return articles_[articleIndex_ + 1]->curPagePtr();
}

void MagazineDocumentTemplate::getScenes(std::vector<Scene *> & scenes) const
{
	scenes.clear();
	for (int i = 0; i < (int)articles_.size(); ++i)
	{
		for (int j = 0; j < (int)articles_[i]->pages().size(); ++j)
		{
			scenes.push_back(articles_[i]->pages()[j]->scene());
		}
	}
}

#ifdef USE_FBO

void MagazineDocumentTemplate::initTexRenderer(GfxRenderer * gl)
{
	uninitTexRenderer();
	renderTex_->init(gl, width_, height_, Texture::UncompressedRgb16, 0);
	texRenderer_->init(gl, width_, height_, true);
	texRenderer_->setTexture(renderTex_);
}

void MagazineDocumentTemplate::uninitTexRenderer()
{	
	texRenderer_->uninit();
	renderTex_->uninit();
}

#endif

void MagazineDocumentTemplate::init(GfxRenderer * gl)
{
	if (init_) uninit();

	scrollBarThumb_->setFileName(scrollBarThumbFile_);

	//Use ModeScrollable for clipping

	vector<InternalSubscene *> subscenes;
	subscenes.push_back(subscene_.get());
	subscenes.push_back(subsceneCached_.get());
	subscenes.push_back(subsceneTransition_.get());

	BOOST_FOREACH(InternalSubscene * subscene, subscenes)
	{
		subscene->setMode(InternalSubscene::ModeFixed);
		subscene->setLocked(true);
		subscene->init(gl);
	}	

	setSubsceneScaling();

	subscene_->setMode(InternalSubscene::ModeFixed);
	
	BOOST_FOREACH(InternalSubscene * subscene, subSceneBackup_) 
	{
		subscene->init(gl);		
	}

	scrollBarBg_->init(gl, scrollBarBgFile_, Texture::UncompressedRgba32);
	scrollBarLeft_->init(gl, scrollBarLeftSideFile_, Texture::UncompressedRgba32);
	scrollBarRight_->init(gl, scrollBarRightSideFile_, Texture::UncompressedRgba32);
	bg_->init(gl, bgFile_, Texture::UncompressedRgba32);
	bookmarkTitle_->init(gl, bookmarkTitleFile_, Texture::UncompressedRgba32);

	
	scrollBarThumb_->init(gl);

#ifdef USE_FBO
	initTexRenderer(gl);
#endif
	////layout thumb shader	
	programThumb_ = glCreateProgram();
	std::string error;
	GLuint fragmentShader =	
		loadShader(GL_FRAGMENT_SHADER, gShaderSrc, &error);
	GLuint vertShader = 
		loadShader(GL_VERTEX_SHADER, gVertShaderSrc, &error);
	glAttachShader(programThumb_, vertShader);   
	glAttachShader(programThumb_, fragmentShader);  
	glBindAttribLocation(programThumb_, AttribPosition, "position");
	glBindAttribLocation(programThumb_, AttribTexCoord, "texCoord");
	linkProgram(programThumb_);	
	glDeleteShader(fragmentShader);
	glDeleteShader(vertShader);
	glUseProgram(programThumb_);
	locThumbMVPMatrix_ = 
		glGetUniformLocation(programThumb_, "modelViewProjectionMatrix");


	//init menu objects
	menuBg_->setFileName(topMenuBgFile_);
	menuLayoutButton_->setFileName(topMenuLayoutButtonFile_);
	menuHomeButton_->setFileName(topMenuHomeButtonFile_);	
	menuBookmarkActiveButton_->setFileName(bookmarkActiveFile_);
	menuBookmarkInactiveButton_->setFileName(bookmarkInactiveFile_);
	menuAspectActiveButton_->setFileName(aspectActiveFile_);
	menuAspectInactiveButton_->setFileName(aspectInactiveFile_);
	menuBookmarkListButton_->setFileName(bookmarkListFile_);
	menuToContentsButton_->setFileName(toContentsFile_);

	menuBg_->init(gl);
	menuHomeButton_->init(gl);
	menuLayoutButton_->init(gl);
	menuBookmarkActiveButton_->init(gl);
	menuBookmarkInactiveButton_->init(gl);
	menuAspectActiveButton_->init(gl);
	menuAspectInactiveButton_->init(gl);
	menuBookmarkListButton_->init(gl);
	menuToContentsButton_->init(gl);

	setMenuTransform();	

	loadingPage_->init(gl, width_, height_);


	readBookmarks();
	curPageHasBookmark_ = pageHasBookmark(articleIndex_, pageIndex_);
	bookmarksScroller_.reset(new InternalSubscene);	
	bookmarksContentsScene_.reset(new Scene);
	bookmarksContentsScene_->setZoom(true);
	bookmarksContentsScene_->setZoomMag(1);

	//scenes need to actually have a parent document for the code below to work

	document_->addScene(bookmarksContentsScene_);
	
	bookmarksScroller_->setMode(InternalSubscene::ModeScrollable);	
	bookmarksScroller_->setTransparentBg(true);
	bookmarksScroller_->setScene(bookmarksContentsScene_.get());

	//delete scenes because we don't want extraneous scenes showing up in the editor
	document_->deleteScene(bookmarksContentsScene_.get());

	setBookmarkUiSize();

	delete pageFlip_;
	pageFlip_ = NULL;
	if (transitionMode_ == PageFlipMode) 
	{
		pageFlip_ = new PageFlip;
	}
	else if (transitionMode_ == PageFlipOrigMode)
	{
		pageFlip_ = new PageFlipOrig;
	}
	if (pageFlip_) pageFlip_->init(gl);

#ifdef WATERMARK
	sceneChangerCheck(articles_[0]->page(0)->scene());
	sceneCheck_ = true;
	sceneChangerCheck(articles_.back()->page(articles_.back()->numPages()-1)->scene());
#endif

	init_ = true;
}

void MagazineDocumentTemplate::sceneChangerCheck(Scene * scene)
{
	std::vector<SceneObject *> sceneObject;
	boost::uuids::uuid type = SceneChanger().type();
	scene->getSceneObjectsByID(type, & sceneObject);
	if(!sceneObject.empty())
	{
		BOOST_FOREACH(SceneObject * obj, sceneObject)
		{
			if(obj->parent() == scene->root())
			{
				SceneChanger * sceneChanger = (SceneChanger*)obj;
				if(!sceneChanger->scenes().empty())
					if(!sceneCheck_)
						sceneChangerCheck(sceneChanger->scenes()[0]);
					else
						sceneChangerCheck(sceneChanger->scenes().back());
				else 
					imageChangerCheck(scene);
			}
		}
	}else{
		scene->waterMark();
	}
}

void MagazineDocumentTemplate::imageChangerCheck(Scene * scene)
{
	std::vector<SceneObject *> sceneObject;
	static boost::uuids::uuid type = ImageChanger().type();
	scene->getSceneObjectsByID(type, &sceneObject);
	if(!sceneObject.empty())
	{
		BOOST_FOREACH(SceneObject * obj, sceneObject)
		{
			if(obj->parent() == scene->root())
			{
				ImageChanger* imageChanger = (ImageChanger *)obj;
				if(!imageChanger->fileNames().empty())
				{
					for (int j = 0 ; j <imageChanger->pageIndex() ; j++)
					{
						if(!sceneCheck_)
						{
							if (j == 0)
								scene->waterMark();
						} else if(sceneCheck_){
							if(j == imageChanger->pageIndex())
								scene->waterMark();
						}
					}
				}
				else
					scene->waterMark();
			}
		}
	}else{
		scene->waterMark();
	}
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

void MagazineDocumentTemplate::setMenuTransform()
{
	Transform trans;

	menuBg_->setWidth((int)width_);
	menuBg_->setHeight((int)menuHeight_);

	vector<ButtonImgs> leftButtons;
	vector<ButtonImgs> rightButtons;

	leftButtons.push_back(ButtonImgs(menuBookmarkListButton_.get(), NULL));
	leftButtons.push_back(ButtonImgs(menuBookmarkActiveButton_.get(), menuBookmarkInactiveButton_.get()));

	rightButtons.push_back(ButtonImgs(menuAspectActiveButton_.get(), menuAspectInactiveButton_.get()));
	if (toContentsPage_) rightButtons.push_back(ButtonImgs(menuToContentsButton_.get(), NULL));
 	rightButtons.push_back(ButtonImgs(menuHomeButton_.get(), NULL));
	rightButtons.push_back(ButtonImgs(menuLayoutButton_.get(), NULL));

	int buttonY = (menuHeight_ - btnDim_) / 2;
	for (int i = 0; i < (int)leftButtons.size(); ++i)
	{
		leftButtons[i].enabled->setWidth(btnDim_);
		leftButtons[i].enabled->setHeight(btnDim_);
		trans = leftButtons[i].enabled->transform();
		trans.setTranslation(btnMargin_ + i*(btnDim_ + btnMargin_), buttonY, 0.0f);
		leftButtons[i].enabled->setTransform(trans);

		if (leftButtons[i].disabled)
		{
			leftButtons[i].disabled->setWidth(btnDim_);
			leftButtons[i].disabled->setHeight(btnDim_);
			leftButtons[i].disabled->setTransform(trans);
		}
	}

	int numRightButtons = (int)rightButtons.size();
	for (int i = 0; i < numRightButtons; ++i)
	{
		rightButtons[i].enabled->setWidth(btnDim_);
		rightButtons[i].enabled->setHeight(btnDim_);
		trans = rightButtons[i].enabled->transform();
		trans.setTranslation(
			width_ - (numRightButtons - i)*(btnDim_ + btnMargin_),
			buttonY,
			0.0f);	
		rightButtons[i].enabled->setTransform(trans);

		if (rightButtons[i].disabled)
		{
			rightButtons[i].disabled->setWidth(btnDim_);
			rightButtons[i].disabled->setHeight(btnDim_);
			rightButtons[i].disabled->setTransform(trans);
		}
	}

/*
	menuAspectActiveButton_->setWidth((int)btnDim_);
	menuAspectActiveButton_->setHeight((int)btnDim_);
	trans = menuAspectActiveButton_->transform();
	trans.setTranslation(width_ - 3*(btnDim_ + btnMargin_), btnMargin_, 0);
	menuAspectActiveButton_->setTransform(trans);	

	menuAspectInactiveButton_->setWidth((int)btnDim_);
	menuAspectInactiveButton_->setHeight((int)btnDim_);
	trans = menuAspectInactiveButton_->transform();
	trans.setTranslation(width_ - 3*(btnDim_ + btnMargin_), btnMargin_, 0);
	menuAspectInactiveButton_->setTransform(trans);	

	menuHomeButton_->setWidth((int)btnDim_);
	menuHomeButton_->setHeight((int)btnDim_);
	trans = menuHomeButton_->transform();
	trans.setTranslation(width_ - 2*(btnDim_ + btnMargin_), btnMargin_, 0);
	menuHomeButton_->setTransform(trans);	
	
	menuLayoutButton_->setWidth(btnDim_);
	menuLayoutButton_->setHeight(btnDim_);
	trans = menuLayoutButton_->transform();
	trans.setTranslation(width_ - (btnDim_ + btnMargin_), btnMargin_, 0);
	menuLayoutButton_->setTransform(trans);

	menuBookmarkActiveButton_->setWidth(btnDim_);
	menuBookmarkActiveButton_->setHeight(btnDim_);
	trans = menuBookmarkActiveButton_->transform();
	trans.setTranslation(2*btnMargin_ + btnDim_, btnMargin_, 0);
	menuBookmarkActiveButton_->setTransform(trans);

	menuBookmarkInactiveButton_->setWidth(btnDim_);
	menuBookmarkInactiveButton_->setHeight(btnDim_);
	trans = menuBookmarkInactiveButton_->transform();
	trans.setTranslation(2*btnMargin_ + btnDim_, btnMargin_, 0);
	menuBookmarkInactiveButton_->setTransform(trans);

	menuBookmarkListButton_->setWidth(btnDim_);
	menuBookmarkListButton_->setHeight(btnDim_);
	trans = menuBookmarkInactiveButton_->transform();
	trans.setTranslation(btnMargin_, btnMargin_, 0);
	menuBookmarkListButton_->setTransform(trans);*/
}

void MagazineDocumentTemplate::uninit()
{
	if (!init_) return;
	subscene_->uninit();
	subsceneCached_->uninit();
	subsceneTransition_->uninit();

	BOOST_FOREACH(InternalSubscene * subscene, subSceneBackup_)
	{
		subscene->uninit();
	}

	scrollBarBg_->uninit();
	scrollBarLeft_->uninit();
	scrollBarRight_->uninit();
	bg_->uninit();
	bookmarkTitle_->uninit();
	scrollBarThumb_->uninit();

	map<MagazinePage *, ThumbData>::iterator iter;
	for (iter = thumbMap_.begin(); iter != thumbMap_.end(); ++iter)
	{
		(*iter).second.uninit();
	}
	thumbMap_.clear();


#ifdef USE_FBO
	uninitTexRenderer();
#else
	if (ssBuffer_) {free(ssBuffer_); ssBuffer_ = 0;}
	if (renderTex_) 
	{
		glDeleteTextures(1, &renderTex_);
	}
#endif

	glDeleteProgram(programThumb_);	

	//menu uninit
	BOOST_FOREACH(Image * menuImg, menuImgs_)
		menuImg->uninit();

	loadingPage_->uninit();
	
	document_->deleteScene(bookmarksContentsScene_.get());

	bookmarksScroller_.reset();
	bookmarksContentsScene_.reset();
	
	delete pageFlip_;
	pageFlip_ = NULL;

	init_ = false;
}

void MagazineDocumentTemplate::ThumbData::uninit()
{
	texture->uninit();
}

Scene * MagazineDocumentTemplate::pageLeft() const
{
	if (articles_.empty()) return 0;
	if (articleIndex_ == 0) return 0;	
	return articles_[articleIndex_ - 1]->curPagePtr();
}

Scene * MagazineDocumentTemplate::pageRight() const
{
	if (articles_.empty()) return 0;
	if (articleIndex_ == (int)articles_.size() - 1) return 0;
	return articles_[articleIndex_ + 1]->curPagePtr();
}

Scene * MagazineDocumentTemplate::pageTop() const
{
	if (articles_.empty()) return 0;
	int curPage = articles_[articleIndex_]->curPage();
	if (curPage == 0) return 0;
	return articles_[articleIndex_]->page(curPage - 1)->scene();
}

Scene * MagazineDocumentTemplate::pageBottom() const
{
	if (articles_.empty()) return 0;
	int curPage = articles_[articleIndex_]->curPage();
	if (curPage == articles_[articleIndex_]->numPages() - 1) return 0;
	return articles_[articleIndex_]->page(curPage + 1)->scene();
}

bool MagazineDocumentTemplate::isCurrentScene(Scene * scene) const
{
	Scene * curScene = 0;
	if (0 <= articleIndex_ && articleIndex_ < (int)articles_.size())
	{
		const MagazineArticle * article = articles_[articleIndex_].get();
		
		if (0 <= pageIndex_ && pageIndex_ < article->numPages())
			curScene = article->page(pageIndex_)->scene();
	}

	if (curScene)
	{
		if (curScene == scene) return true;		
		else if (curScene->isShowing(scene)) return true;
	}

	return false;
}

void MagazineDocumentTemplate::setCurPage(int articleIndex, int pageIndex)
{
	/*
	Example: SceneChange action -> audio stopped action -> scene set action ....
	scene set action must override the first SceneChange action...
	so have to reset the animated page transition process
	*/
	if (transitionMode_ == PageFlipMode || transitionMode_ == PageFlipOrigMode)
	{
		//TODO reset the page flip effect
	}
	else
	{
		pageTween_.reset();
	}

	Global::instance().showAppLayer(true);

	bool sceneChanged = false;
	
	if (articleIndex_ != articleIndex || pageIndex_ != pageIndex)
		sceneChanged = true;
	if (!sceneChanged) return;	

	if (articleIndex >= (int)articles_.size()) return;
	if (pageIndex >= (int)articles_[articleIndex]->pages().size()) return;

	
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
	int prevArticleIndex = articleIndex_;
	int prevPageIndex = pageIndex_;
	articleIndex_ = articleIndex;
	pageIndex_ = pageIndex;
	articles_[articleIndex_]->setCurPage(pageIndex);


	if (isPreDownload_)
		resetSaveLocalRequest();


	if (resetArticleFirstPage_ && prevArticleIndex >= 0 && prevArticleIndex != articleIndex_)
	{
		articles_[prevArticleIndex]->setCurPage(0);
	}

	
	//stop any videos or audio that were playing on the previous page
	Global::instance().videoPlayer()->deactivate();
	Global::instance().audioPlayer()->stopAll();
	Global::instance().cameraOff();


	//temporarily store previous scenes to prevent needless unloading of scenes	
	if (subscene_) subSceneBackup_[0]->setScene(subscene_->scene());
	if (subsceneCached_) subSceneBackup_[1]->setScene(subsceneCached_->scene());

	subscene_->setScene(articles_[articleIndex_]->page(pageIndex)->scene());
    subscene_->setHasLeft(pageLeft() ? true : false);
    subscene_->setHasRight(pageRight() ? true : false);
    subscene_->setHasTop(pageTop() ? true : false);
    subscene_->setHasBottom(pageBottom() ? true : false);
	
	Scene * cc[4];
	WhichCached ww[4];
	if (pageIndex > prevPageIndex)
	{ 
		ww[0] = CachedBottom; ww[1] = CachedTop; ww[2] = CachedRight; ww[3] = CachedLeft;
	}
	else if (pageIndex < prevPageIndex)
	{
		ww[0] = CachedTop; ww[1] = CachedBottom; ww[2] = CachedLeft; ww[3] = CachedRight;
	}
	else if (articleIndex > prevArticleIndex)
	{
		ww[0] = CachedRight; ww[1] = CachedLeft; ww[2] = CachedBottom; ww[3] = CachedTop;
	}
	else if (articleIndex < prevArticleIndex)
	{ 
		ww[0] = CachedLeft; ww[1] = CachedRight; ww[2] = CachedTop; ww[3] = CachedBottom;
	}

	for (int i = 0; i < 4; ++i)
	{
		Scene * sceneToCache = NULL;
		switch (ww[i])
		{
		case CachedBottom: sceneToCache = pageBottom(); break;
		case CachedTop: sceneToCache = pageTop(); break;
		case CachedRight: sceneToCache = pageRight(); break;
		case CachedLeft: sceneToCache = pageLeft(); break;
		}
		if (sceneToCache)
		{
			whichCached_ = ww[i];
			setCachedScene(subsceneCached_.get(), sceneToCache);
			break;
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

	
	curPageHasBookmark_ = pageHasBookmark(articleIndex_, pageIndex_);
	
	Global::instance().callbackPageChanged(articles_[articleIndex_]->page(pageIndex)->scene()->name());
}

void MagazineDocumentTemplate::setSubsceneScaling()
{
	const int numSubScenes = 2;
	InternalSubscene * subscenes[numSubScenes];

	subscenes[0] = subscene_.get();
	subscenes[1] = subsceneCached_.get();

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

void MagazineDocumentTemplate::curSceneAppObjectStart() const
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

void MagazineDocumentTemplate::curSceneAppObjectStop() const
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


void MagazineDocumentTemplate::setCurPagePos(const Vector3 & pos)
{	
	setBaseAppObjectOffset(
		pos.x, pos.y, 
		document_->curSceneScaleX(), document_->curSceneScaleY(),
		pos.x, pos.y, width_, height_);
	

	Vector3 pagePos = pos;
	Transform transform;
	
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
        switch(whichCached_)
        {
            case CachedLeft:
                pagePos = Vector3(-width_ , 0 , 0) + Vector3(m._14, 0, 0);
                break;
            case CachedRight:
                pagePos = Vector3(tempW , 0 , 0) + Vector3(m._14, 0, 0);
                break;
            case CachedTop:
                pagePos = Vector3(0 , -height_ , 0) + Vector3(0, m._24, 0);
                break;
            case CachedBottom:
                pagePos = Vector3(0 , tempH , 0) + Vector3(0, m._24, 0);
                break;
        }
    }
    else
    {
        switch(whichCached_)
        {
        case CachedLeft:
        pagePos = pos + Vector3(-width_, 0, 0);
            break;
        case CachedRight:
        pagePos = pos + Vector3(width_, 0, 0);
            break;
        case CachedTop:
        pagePos = pos + Vector3(0, -height_, 0);
            break;
        case CachedBottom:
        pagePos = pos + Vector3(0, height_, 0);
            break;
        }
    }
    transform = subsceneCached_->transform();
    transform.setTranslation(pagePos.x, pagePos.y, 0);
    subsceneCached_->setTransform(transform);
}

void MagazineDocumentTemplate::setScrollBarThumbTransform()
{
	Transform transform = scrollBarThumb_->transform();
	Vector3 translation = transform.translation();
	float scrollThumbWidth = scrollBarThumb_->texture()->width() * dpiScale_;
	float scrollThumbHeight = scrollBarThumb_->texture()->height() * dpiScale_;
	scrollBarThumb_->setWidth(scrollThumbWidth);
	scrollBarThumb_->setHeight(scrollThumbHeight);

	float minX = (float)scrollBarLeft_->width();
	float maxX = width_ - 
		(scrollBarRight_->width() + scrollThumbWidth);

	float useScrollPos = scrollPos_;
	if (useScrollPos < 0) useScrollPos = 0;
	if (useScrollPos > 1) useScrollPos = 1;

	translation.x = minX + useScrollPos * (maxX - minX);
	translation.y = height_ - (scrollBarBg_->height()*dpiScale_ + scrollBarThumb_->height())/2;
	transform.setTranslation(translation.x, translation.y, translation.z);
	scrollBarThumb_->setTransform(transform);
}

void MagazineDocumentTemplate::setScrollPos(float factor)
{	
	/*
	if (factor < 0) factor = 0;
	if (factor > 1) factor = 1;
	*/
	float minX, maxX;

	//set layout position
	float thumbnailWidth = width_ * thumbnailScale_;
	float thumbnailHeight = height_ * thumbnailScale_;
	float gap = gapFactor_ * thumbnailWidth;
	int numArticles = (int)articles_.size();
	maxX = width_/2 - thumbnailWidth/2;
	minX = maxX - (numArticles - 1) * (thumbnailWidth + gap);

	layoutX_ =  roundFloat(maxX + factor * (minX - maxX));
	scrollPos_ = factor;

	setScrollBarThumbTransform();
	
}

bool MagazineDocumentTemplate::shouldAcceptInput() const
{
	bool pageTransition;

	if (transitionMode_ == PageFlipMode || transitionMode_ == PageFlipOrigMode)
	{
		pageTransition = pageFlip_->curTransitionDirection() != TransitionDirectionNone;
	}
	else
	{
		pageTransition = pageTween_.isHappening();
	}

	return 
		articles_.size() > 0 &&	
		!pageTransition &&		
		transitionStage_ == TransitionInactive;
}

bool MagazineDocumentTemplate::keyPressEvent(int keyCode)
{
	return subscene_->keyPressEvent(keyCode);
}

bool MagazineDocumentTemplate::keyReleaseEvent(int keyCode)
{
	return subscene_->keyReleaseEvent(keyCode);	
}

bool MagazineDocumentTemplate::pressEvent(const Vector2 & pressPt, int pressId, bool propagate)
{
	PressEventData & pressEvent = pressEvents_[pressId];
	pressEvent.propagate = propagate;

	if (!shouldAcceptInput()) 
	{
		pressEvent.pressed = PressEventData::StrayPress;
		return true;
	}

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

	bool insideContentRegion = mode_ == PageMode;

	bool insideScrollBar = mode_ == LayoutMode && 
		pressPt.y >= height_ - scrollBarBg_->height()*dpiScale_;


	if (insideContentRegion)	
	{
		pressEventMode = Contents;
	}
	else if (insideScrollBar)
	{
		pressEventMode = ScrollBar;		
		Vector3 intPt;
		if (scrollBarThumb_->intersect(&intPt, mouseRay))
		{
			scrollThumbGrabbed_ = true;
			redrawTriggered_ = true;
		}
	}

	bool handled = false;
	if (pressEventMode == Contents)
	{	
		if (propagate)
			handled = subscene_->pressEvent(pressPt, pressId);

		if (handled) pressEvent.pressed = PressEventData::PassedToChild;
	}	

	if (mode_ == BookmarkListMode)
	{
		bookmarksScroller_->pressEvent(pressPt, pressId);
	}

	if (pageMovingId_ == -1 && !handled) pageMovingId_ = pressId;
	return true;
}

Vector2 MagazineDocumentTemplate::toUnitCoords(const Vector2 & coords) const
{
	float sx = (float)1.0f / document_->width();
	float sy = (float)1.0f / document_->height();
	return Vector2(sx * coords.x, sy * coords.y);
}

void MagazineDocumentTemplate::setCachedScene(InternalSubscene * subsceneCached, Scene * scene)
{
	subsceneCached->setScene(scene);
	subsceneCached->preStart(Global::currentTime());
}

bool MagazineDocumentTemplate::moveEvent(const Vector2 & curPt, int pressId)
{
	PressEventData & pressEvent = pressEvents_[pressId];
	if (pressEvent.pressed == PressEventData::PassedToChild)
	{
		bool handled = subscene_->moveEvent(curPt, pressId);
		if (handled) return true;
	}
	else
	{
		bool handled = subscene_->moveEvent(curPt, pressId);	
		if (handled)
		{
			pressEvent.pressed = PressEventData::PassedToChild;
			return true;
		}	
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

	bool moveDirectionNewlyDetermined = false;
	if (moveDirection_ == Undetermined)
	{
		moveDirectionNewlyDetermined = true;
		moveDirection_ = Horz;

		if (fabs(curPt.x - pressPt.x) < fabs(curPt.y - pressPt.y))
		{			
			moveDirection_ = Vert;			
		}

	}
	
	if (mode_ == LayoutMode)
	{
		if (scrollThumbGrabbed_)
		{
			if (!pressEvent.startedMoving)
			{
				
				thumbGrabbedDelta_ = 
					Vector3(curPt.x, curPt.y, 0) - scrollBarThumb_->transform().translation();
			}
			/*
			Transform transform = scrollBarThumb_->transform();
			Vector2 t = transform.translation();
			t.x = curPt.x;*/
			
			float minX, maxX;
			minX = (float)scrollBarLeft_->width();
			maxX = float(width_ - 
				(scrollBarRight_->width() + scrollBarThumb_->width()));


			float thumbnailWidth = width_ * thumbnailScale_;
			float gap = gapFactor_ * thumbnailWidth;			
			int numArticles = (int)articles_.size();
			float factor = (curPt.x - thumbGrabbedDelta_.x - minX) / (maxX - minX);
			
			if (factor < 0) factor = 0;
			if (factor > 1) factor = 1;

			setScrollPos(factor);

			redrawTriggered_ = true;
		}
		else if (moveDirection_ == Horz && requiredLayoutMoveDirection_ != Vert)
		{
			requiredLayoutMoveDirection_ = Horz;
			float newLayoutX = startScrollX_ + curPt.x - startPt.x;

			float thumbnailWidth = width_ * thumbnailScale_;
			float thumbnailHeight = height_ * thumbnailScale_;
			float gap = gapFactor_ * thumbnailWidth;
			
			int numArticles = (int)articles_.size();

			float maxX = width_/2 - thumbnailWidth/2;
			float minX = maxX - (numArticles - 1) * (thumbnailWidth + gap);
			
			float factor = 0;
			if (maxX > minX)				
				factor = (maxX - newLayoutX) / (maxX - minX);
			else 
				factor = 0.5f;

			layoutTween_.reset();
			setScrollPos(factor);

			dragVelHandler_.addMoveHistory(
				Vector2(layoutX_, layoutOffsetY_), Global::currentTime());
		}
		else if (moveDirection_ == Vert && requiredLayoutMoveDirection_ != Horz)
		{
			dragVelHandler_.addMoveHistory(
				Vector2(layoutX_, layoutOffsetY_), Global::currentTime());
			requiredLayoutMoveDirection_ = Vert;
			layoutOffsetY_ = startLayoutOffsetY_ + curPt.y - startPt.y;
		}
		
	}
	else if (mode_ == BookmarkListMode)
	{		
		bookmarksScroller_->moveEvent(curPt, pressId);
	}
	else if (mode_ == PageMode)
	{	
		//Global::instance().showAppLayer(false);

		if (!pressEvent.startedMoving) notifyPageChanging();
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
		
        if (subscene_->transform().scaling().x > normalScaleX &&
            subscene_->transform().scaling().y > normalScaleY &&
			subscene_->isZoomable())
        {
            float deltaX = curPt.x - pressPt.x;
            float deltaY = curPt.y - pressPt.y;
            float x = fabs(deltaX);
            float y = fabs(deltaY);
            
            setCurPagePos(subscene_->operateTranslation(pressId));
            Transform t = subscene_->transform();
            checkOutside(t, normalScaleX, normalScaleY, false);
            subscene_->setTransform(t);
            if (deltaX < 0 && x > y && whichCached_ != CachedRight)
			{
				setCachedScene(subsceneCached_.get(), pageRight());
				whichCached_ = CachedRight;
			}
			else if (deltaX > 0 && x > y && whichCached_ != CachedLeft)
			{
				setCachedScene(subsceneCached_.get(), pageLeft());
				whichCached_ = CachedLeft;
			}else if (deltaY < 0 && y > x && whichCached_ != CachedBottom)
			{
				setCachedScene(subsceneCached_.get(), pageBottom());
				whichCached_ = CachedBottom;
			}
			else if (deltaY > 0 && y > x && whichCached_ != CachedTop)
			{
				setCachedScene(subsceneCached_.get(), pageTop());
				whichCached_ = CachedTop;
			}
        }
		else if (transitionMode_ == PageFlipMode || transitionMode_ == PageFlipOrigMode)
		{
			if (moveDirectionNewlyDetermined)
			{
				Vector2 unitCoords = toUnitCoords(curPt);
				
				TransitionDirection pageFlipDir = TransitionDirectionNone;
				if (moveDirection_ == Horz)
				{
					if (unitCoords.x > 0.5f && articleIndex_ < articles_.size() - 1) pageFlipDir = TransitionDirectionRight;
					else if (unitCoords.x <= 0.5f && articleIndex_ > 0) pageFlipDir = TransitionDirectionLeft;
					else if (unitCoords.y > 0.5f && pageIndex_ < articles_[articleIndex_]->numPages() - 1) pageFlipDir = TransitionDirectionDown;
					else if (unitCoords.y <= 0.5f && pageIndex_ > 0) pageFlipDir = TransitionDirectionUp;
				}
				else
				{
					if (unitCoords.y > 0.5f && pageIndex_ < articles_[articleIndex_]->numPages() - 1) pageFlipDir = TransitionDirectionDown;
					else if (unitCoords.y <= 0.5f && pageIndex_ > 0) pageFlipDir = TransitionDirectionUp;
					else if (unitCoords.x > 0.5f && articleIndex_ < articles_.size() - 1) pageFlipDir = TransitionDirectionRight;
					else if (unitCoords.x <= 0.5f && articleIndex_ > 0) pageFlipDir = TransitionDirectionLeft;					
				}

				if (pageFlipDir != TransitionDirectionNone)
				{					
					pageFlip_->pressEvent(unitCoords, pageFlipDir);

					Transform transform = subsceneCached_->transform();
					transform.setTranslation(0.0f, 0.0f, 0.0f);
					subsceneCached_->setTransform(transform);
					Global::instance().showAppLayer(false);
				}
				else
					Global::instance().showAppLayer(true);
			}

			pageFlip_->moveEvent(toUnitCoords(curPt));
			TransitionDirection tDir = pageFlip_->curTransitionDirection();				

			if (tDir == TransitionDirectionRight && whichCached_ != CachedRight)
			{
				setCachedScene(subsceneCached_.get(), pageRight());
				whichCached_ = CachedRight;
			}
			else if (tDir == TransitionDirectionLeft && whichCached_ != CachedLeft)
			{
				setCachedScene(subsceneCached_.get(), pageLeft());
				whichCached_ = CachedLeft;
			}
			else if (tDir == TransitionDirectionUp && whichCached_ != CachedTop)
			{
				setCachedScene(subsceneCached_.get(), pageTop());
				whichCached_ = CachedTop;
			}
			else if (tDir == TransitionDirectionDown && whichCached_ != CachedBottom)
			{
				setCachedScene(subsceneCached_.get(), pageBottom());
				whichCached_ = CachedBottom;
			}
		}
		else
		{
			if (moveDirection_ == Horz)
			{			
				//constrain scrolling if it will end up revealing empty space
				float delta = curPt.x - startPt.x;
				if (!pageLeft() && delta > 0) delta = 0;
				if (!pageRight() && delta < 0) delta = 0;

				setCurPagePos(Vector3(delta, 0, 0));// + subscene_->originTranslation()); //빈공간 막기 origniTranslagtion수
				if (delta < 0 && whichCached_ != CachedRight)
				{
					setCachedScene(subsceneCached_.get(), pageRight());
					whichCached_ = CachedRight;
				}
				else if (delta > 0 && whichCached_ != CachedLeft)
				{
					setCachedScene(subsceneCached_.get(), pageLeft());
					whichCached_ = CachedLeft;
				}			
			}
			else if (moveDirection_ == Vert && articles_[articleIndex_]->numPages() > 1)
			{
				//constrain scrolling if it will end up revealing empty space
				float delta = curPt.y - startPt.y;
				if (!pageTop() && delta > 0) delta = 0;
				if (!pageBottom() && delta < 0) delta = 0;				
				
				if (delta < 0 && whichCached_ != CachedBottom)
				{
					setCachedScene(subsceneCached_.get(), pageBottom());
					whichCached_ = CachedBottom;
				}
				else if (delta > 0 && whichCached_ != CachedTop)
				{
					setCachedScene(subsceneCached_.get(), pageTop());
					whichCached_ = CachedTop;
				}

				setCurPagePos(Vector3(0, delta, 0));
			}
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

bool MagazineDocumentTemplate::hasPageBeenMoved() const
{
	if (!subscene_) return false;
	const Vector3 & trans = subscene_->transform().translation();
	if (trans.x == 0.0f && trans.y == 0.0f) return false;
	return true;
}

Vector3 MagazineDocumentTemplate::zoomedPageTweenEndPos(bool & ql, bool & qr, bool & qp, bool & qn, bool & qf)
{
    Vector3 pageTweenEndPos = Vector3(0.0f, 0.0f, 0.0f);
    
    float temp = 0;
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
    float dy = (subscene_->windowHeight()/t.scaling().x*normalScaleY)/2; 
    
    bool sl = !qf && 
    mat._14 < -dx && pageLeft();
    
    bool sr = !qf &&
    mat._14 > subscene_->windowWidth() - dx && pageRight();
    
    bool sp = !qf && 
    mat._24 < -dy && pageTop();
    
    bool sn = !qf &&
    mat._24 > subscene_->windowHeight() - dy && pageBottom();
    if (ql || sl)
    {
        temp = ((width_ / t.scaling().x * normalScaleX) + mat._14 * normalScaleX ) / normalScaleX;
        mat._14 -= temp;
        mat = mat.inverse();
        t.setMatrix(mat, true);
        pageTweenEndPos = t.translation();
        targetArticleIndex_ = articleIndex_ - 1;
        targetPageIndex_ = articles_[targetArticleIndex_]->curPage();
    }
    else if (qr || sr)
    {
        temp = ((width_ / t.scaling().x * normalScaleX) - (mat._14 * normalScaleX  - (width_ - (width_ / t.scaling().x * normalScaleX )))) / normalScaleX;
        mat._14 += temp;
        mat = mat.inverse();
        t.setMatrix(mat, true);
        pageTweenEndPos = t.translation();		
        targetArticleIndex_ = articleIndex_ + 1;
        targetPageIndex_ = articles_[targetArticleIndex_]->curPage();
    }
    else if (qp || sp)
    {
        temp = ((height_ / t.scaling().x * normalScaleY) + mat._24 * normalScaleY ) / normalScaleY;
        mat._24 -= temp;
        mat = mat.inverse();
        t.setMatrix(mat, true);
        pageTweenEndPos = t.translation();
        targetArticleIndex_ = articleIndex_;
        targetPageIndex_ = pageIndex_ - 1;
    }
    else if (qn || sn)
    {
        temp = ((height_ / t.scaling().x * normalScaleY) - (mat._24 * normalScaleY  - (height_ - (height_ / t.scaling().x * normalScaleY )))) / normalScaleY;
        mat._24 += temp;
        mat = mat.inverse();
        t.setMatrix(mat, true);
        pageTweenEndPos = t.translation();
        targetArticleIndex_ = articleIndex_;
        targetPageIndex_ = pageIndex_ + 1;
    }
    else
    {
        checkOutside(t, normalScaleX, normalScaleY, true);
        pageTweenEndPos = t.translation();
    }
    
    return pageTweenEndPos;
}

void MagazineDocumentTemplate::doPageSnapBack(const PressEventData & pressEvent, const Vector2 & pos)
{
	pageTween_.reset();
	Vector2 startPt = pressEvent.startPt;
    Transform t = subscene_->transform();
    t.setPivot(0,0,0);
    subscene_->setTransform(t);
	Vector3 pageTweenStartPos = subscene_->transform().translation();
	Vector3 pageTweenEndPos = Vector3(0.0f, 0.0f, 0.0f);
	targetPageIndex_ = pageIndex_;
	targetArticleIndex_ = articleIndex_;
    
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

	bool isQuickFlick = 
		Global::currentTime() - pressEvent.startMovingTime < 
		Global::flickTime();
    
    bool quickFlickLeft = isQuickFlick && 
    pos.x > startPt.x && pageLeft() && subscene_->isMultiTouchEnded();
    bool slowLeft = !isQuickFlick && 
    pageTweenStartPos.x > width_/2 && pageLeft();
    
    bool quickFlickRight = isQuickFlick &&
    pos.x < startPt.x && pageRight() && subscene_->isMultiTouchEnded();
    bool slowRight = !isQuickFlick &&
    pageTweenStartPos.x < -width_/2 && pageRight();
    
    bool quickFlickPrev = isQuickFlick && 
    pos.y > startPt.y && pageTop() && subscene_->isMultiTouchEnded();
    bool slowPrev = !isQuickFlick && 
    pageTweenStartPos.y > height_/2 && pageTop();
    
    bool quickFlickNext = isQuickFlick &&
    pos.y < startPt.y && pageBottom() && subscene_->isMultiTouchEnded();
    bool slowNext = !isQuickFlick &&
    pageTweenStartPos.y < -height_/2 && pageBottom();

    if (subscene_->transform().scaling().x > normalScaleX && 
        subscene_->transform().scaling().y > normalScaleY && 
		subscene_->isZoomable())
    {
        pageTweenEndPos = zoomedPageTweenEndPos(quickFlickLeft,
                                                quickFlickRight,
                                                quickFlickPrev,
                                                quickFlickNext, isQuickFlick);
    }
	else if (moveDirection_ == Horz)
	{			
		if (quickFlickLeft || slowLeft)
		{
			pageTweenEndPos = Vector3(width_, 0, 0);
			targetArticleIndex_ = articleIndex_ - 1;
			targetPageIndex_ = articles_[targetArticleIndex_]->curPage();
		}
		else if (quickFlickRight || slowRight)
		{
			pageTweenEndPos = Vector3(-width_, 0, 0);		
			targetArticleIndex_ = articleIndex_ + 1;
			targetPageIndex_ = articles_[targetArticleIndex_]->curPage();
		}	

	}
	else if (moveDirection_ == Vert)
	{
		if (quickFlickPrev || slowPrev)
		{
			pageTweenEndPos = Vector3(0, height_, 0);
			targetArticleIndex_ = articleIndex_;
			targetPageIndex_ = pageIndex_ - 1;
		}
		else if (quickFlickNext || slowNext)
		{
			pageTweenEndPos = Vector3(0, -height_, 0);
			targetArticleIndex_ = articleIndex_;
			targetPageIndex_ = pageIndex_ + 1;
		}				
	}

	pageTween_.start(
		Global::currentTime(), pageTweenStartPos, pageTweenEndPos);
}

bool MagazineDocumentTemplate::releaseEvent(const Vector2 & pos, int pressId)
{
	PressEventData & pressEvent = pressEvents_[pressId];
	
	if (pressEvent.pressed == PressEventData::StrayPress) return true;

	if (mode_ == PageMode)
	{
		if (pressId == pageMovingId_) 
		{
			pageMovingId_ = -1;

			if (transitionMode_ == PageFlipMode || transitionMode_ == PageFlipOrigMode)
			{				
				pageFlip_->releaseEvent(toUnitCoords(pos));
				
				switch (pageFlip_->curTransitionDirection())
				{
				case TransitionDirectionLeft:					
					targetArticleIndex_ = articleIndex_ - 1;
					targetPageIndex_ = articles_[targetArticleIndex_]->curPage();
					break;
				case TransitionDirectionRight:					
					targetArticleIndex_ = articleIndex_ + 1;
					targetPageIndex_ = articles_[targetArticleIndex_]->curPage();
					break;
				case TransitionDirectionUp:
					targetArticleIndex_ = articleIndex_;
					targetPageIndex_ = articles_[articleIndex_]->curPage() - 1;
					break;
				case TransitionDirectionDown:
					targetArticleIndex_ = articleIndex_;
					targetPageIndex_ = articles_[articleIndex_]->curPage() + 1;
					break;
				}			
			}
			else
			{
				if (hasPageBeenMoved() && !subscene_->isTweenWorking()) doPageSnapBack(pressEvent, pos);
			}
		}

		if (pressEvent.pressed == PressEventData::PassedToChild)
			subscene_->releaseEvent(pos, pressId);		
	}	
	else if (mode_ == LayoutMode)
	{
		if (pressId == pageMovingId_) pageMovingId_ = -1;
		
		if (scrollPos_ < 0 || scrollPos_ > 1)
			dragVelHandler_.clearMoveHistory();
		else
			dragVelHandler_.calcVelocityFromHistory(Vector2(layoutX_, layoutOffsetY_), Global::currentTime());

		layoutModeSnappedBackYet_ = false;
		
	}
	else if (mode_ == BookmarkListMode)
	{
		bookmarksScroller_->releaseEvent(pos, pressId);
	}

	scrollThumbGrabbed_ = false;
	redrawTriggered_ = true;
	pressEvent.pressed = PressEventData::NotPressed;
	return true;
}

int MagazineDocumentTemplate::thumbWidth() const
{
	return (int)(width_ * thumbnailScale_);		
}

int MagazineDocumentTemplate::thumbHeight() const
{
	return height_ * thumbnailScale_;
}

Ray MagazineDocumentTemplate::processCoords(const Vector2 & pos)
{
	return Ray(Vector3(pos.x, pos.y, -1000.0f), Vector3(0.0f, 0.0f, 1.0f));
}

void MagazineDocumentTemplate::layoutModeClickEvent(const Vector2 & pos)
{
	bool handled = false;
	
	Vector3 intPt;	
	
	float thumbnailWidth = width_ * thumbnailScale_;
	float thumbnailHeight = height_ * thumbnailScale_;
	float gap = gapFactor_ * thumbnailWidth;
	float curX = layoutX_;	

	float startY = height_/2 - thumbnailHeight/2;

	for (int i = 0; i < (int)articles_.size(); ++i)
	{
		float curY = startY;
		curY -= articles_[i]->curPage() * (thumbnailHeight + gap);
		for (int j = 0; j < (int)articles_[i]->numPages(); ++j)
		{			
			if (curX < pos.x && pos.x < curX  + thumbnailWidth && 
				curY < pos.y && pos.y < curY + thumbnailHeight)
			{
				handled = true;

				startModeTransitionAnim(curX + thumbnailWidth/2,
					curY + thumbnailHeight/2, thumbnailScale_, 
					width_/2, height_/2, 1, PageMode, i, j);

				
			}			
			
			curY += thumbnailHeight + gap;
		}
		curX += thumbnailWidth + gap;
		
	}


}

bool MagazineDocumentTemplate::doMenuButtonInput(const Ray & localMouseRay)
{
	if (isMenuShowing())
	{
		Vector3 intPt;

		if (menuHomeButton_->intersect(&intPt, localMouseRay))
		{
			if (homeButtonFunction_ == HomeButtonExitToViewerLayer)
			Global::broadcastExternalMessage("__close");
			else
			{			
				Scene * scene = 0;			
				if (!articles_.empty())
				{
					bool articleNotEmpty = articles_[0]->numPages() > 0;
					MagazinePage * page = 0;
					if (articleNotEmpty) page = articles_[0]->page(0);
					if (page) scene = page->scene();
				}

				if (scene) sceneChangeTo(scene);	
			}
			return true;
		}
		else if (menuLayoutButton_->intersect(&intPt, localMouseRay))
		{
			if (mode_ == PageMode)
			{
				setMode(LayoutMode);
				startModeTransitionAnim(width_/2, height_/2, 1, 
					width_/2, height_/2, thumbnailScale_, LayoutMode, 
					articleIndex_, pageIndex_);	
			}
			else if (mode_ == LayoutMode)
			{
				float thumbnailWidth = width_ * thumbnailScale_;
				float thumbnailHeight = height_ * thumbnailScale_;
				float gap = gapFactor_ * thumbnailWidth;
				float baseY = height_/2 - thumbnailHeight/2;

				int middleI = (width_/2 - layoutX_) / (thumbnailWidth + gap);
				
				startModeTransitionAnim(width_/2,
					height_/2, thumbnailScale_, 
					width_/2, height_/2, 1, PageMode, middleI, articles_[middleI]->curPage());

			}
			else if (mode_ == BookmarkListMode)
			{
				setMode(LayoutMode);
			}
			return true;
		}
		else if (doBookmarks_ && menuBookmarkActiveButton_->intersect(&intPt, localMouseRay))
		{
			if (curPageHasBookmark_)
			{
				removeBookmark(articleIndex_, pageIndex_);
			}
			else
			{
				addBookmark(BookmarkData(articleIndex_, pageIndex_, "New book mark"));
			}
			menuHideTime_ += menuDuration_;
			curPageHasBookmark_ = !curPageHasBookmark_;
			return true;
		}
		else if (doBookmarks_ && menuBookmarkListButton_->intersect(&intPt, localMouseRay))
		{
			if (mode_ == BookmarkListMode)
				setMode(PageMode);
			else
				setMode(BookmarkListMode);	
			menuHideTime_ += menuDuration_;
			return true;
		}
		else if (doAspect_ && menuAspectActiveButton_->intersect(&intPt, localMouseRay))
		{
			preserveAspect_ = !preserveAspect_;
			Global::setPreserveAspectRatio(preserveAspect_);
			menuHideTime_ += menuDuration_;
			return true;
		}
		else if (toContentsPage_ && menuToContentsButton_->intersect(&intPt, localMouseRay))
		{	
			if (contentsArticleIndex_ >= 0 && contentsArticleIndex_ < (int)articles_.size())
			{
				MagazinePage * page = articles_[contentsArticleIndex_]->page(0);
				if (page) sceneChangeTo(page->scene());
			}
			return true;
		}
	}

	return false;
}


bool MagazineDocumentTemplate::clickEvent(const Vector2 & pos, int pressId)
{
	if (!shouldAcceptInput())
	{
		return false;
	}

	Ray mouseRay = processCoords(pos);

	bool menuInput = false;
	if (!doMenuButtonInput(mouseRay))
	{
		if (mode_ == PageMode) 
		{
			PressEventData & pressEvent = pressEvents_[pressId];
			if (pressEvent.propagate)
			{
				bool handled = subscene_->clickEvent(pos, pressId);

				if (!handled) 
				{
					if (isMenuHidden())
					{
						showMenu();
						menuHideTime_ = Global::currentTime() + menuDuration_;
					}
					else if (isMenuShowing())
					{
						hideMenu();
						menuHideTime_ = FLT_MAX;
					}
				}
			}
		}
		else if (mode_ == BookmarkListMode)
		{
			
				
			float yConvertDelta = 
				-bookmarksScroller_->transform().translation().y -
				bookmarksScroller_->scrollPosition().y;
			Vector2 localCoords(pos.x, pos.y + yConvertDelta);
				
			float thumbnailWidth = width_ * thumbnailScale_;
			float thumbnailHeight = height_ * thumbnailScale_;
			float gap = gapFactor_ * thumbnailWidth;
			
			
			for (int i = 0; i < (int)bookmarkThumbImgs_.size(); ++i)
			{
				if (!bookmarkThumbImgs_[i]->visualAttrib()->isVisible()) 
					continue;
				float minY =
					bookmarkThumbImgs_[i]->transform().translation().y;
				float maxY = minY + thumbnailHeight;
				float minX = gap;
				float maxX = gap + thumbnailWidth;

				if (minX < localCoords.x && localCoords.x < maxX && 
					minY < localCoords.y && localCoords.y < maxY)
				{
					int clickedArticle = 
						bookmarks_[bookmarksTopIndex_ + i].articleIndex;
					int clickedPage = 
						bookmarks_[bookmarksTopIndex_ + i].pageIndex;

					startModeTransitionAnim(minX + thumbnailWidth/2,
						minY - yConvertDelta + thumbnailHeight/2, thumbnailScale_, 
						width_/2, height_/2, 1, PageMode, clickedArticle, clickedPage);					
				}	
			}

			bookmarksScroller_->clickEvent(pos, pressId);
		}
		else if (mode_ == LayoutMode)
		{
			layoutModeClickEvent(pos);
		}	
	}
	
	redrawTriggered_ = true;
	return true;
}

bool MagazineDocumentTemplate::doubleClickEvent(const Vector2 & pos, int pressId)
{
	if (!shouldAcceptInput())
	{
		return false;
	}

	bool handled = false;
	bool inPage = false;

	if (mode_ == PageMode)
	{
		PressEventData & pressEvent = pressEvents_[pressId];
		if (pressEvent.propagate)
			subscene_->doubleClickEvent(pos, pressId);
	}

	return true;
}


void MagazineDocumentTemplate::startModeTransitionAnim(
	float startX, float startY, float startScale, 
	float endX, float endY, float endScale, MagazineDocumentTemplate::Mode endMode,
	int i, int j)
{
	transitionStage_ = TransitionNeedToLoadTargetScene;
	transitionDuration_ = 0.3f;	
	transitionStartPt_ = Vector2(startX, startY);
	transitionStartScale_ = startScale;
	transitionEndPt_ = Vector2(endX, endY);
	transitionEndScale_ = endScale;
	transitionScreenShotTaken_ = false;
	transitionEndMode_ = endMode;
	transitionT_ = 0;
	transitionEndArticle_ = i;
	transitionEndPage_ = j;

	InternalSubscene * subscene = subsceneTransition_.get();
	Scene * scene = articles_[i]->page(j)->scene();
	subscene->setScene(scene);		
	if (scene)
	{
		Transform transform = subscene->transform();
		float sx = width_ / scene->screenWidth();
		float sy = height_ / scene->screenHeight();
		transform.setScaling(sx, sy, (sx + sy)/2);
		subscene->setTransform(transform);
		subscene->setWindowWidth(scene->screenWidth());
		subscene->setWindowHeight(scene->screenHeight());
	}

	//reset page before taking screen shot

	if (endMode == PageMode && (articleIndex_ != i || pageIndex_ != j)) 
		subsceneTransition_->preStart(Global::currentTime());
}

bool MagazineDocumentTemplate::isModeTransitionHappening() const
{
	return transitionDuration_ > 0;
}

void MagazineDocumentTemplate::endResetTransition()
{
	transitionDuration_ = 0;
}

void MagazineDocumentTemplate::notifyPageChanging()
{		
	Scene * curPageScene = subscene_->scene();
	if (!curPageScene) return;
	document_->broadcastMessage(
		curPageScene, pageChangingMsg_);

	static PageChangingEvent event;
	curPageScene->root()->handleEvent(&event, Global::currentTime());

}

void MagazineDocumentTemplate::sceneSet(Scene * scene)
{
	bool found = false;
	int articleIndex = 0; int pageIndex = 0; 
	BOOST_FOREACH(MagazineArticleSPtr article, articles_)
	{
		pageIndex = 0;
		BOOST_FOREACH(MagazinePageSPtr page, article->pages())
		{
			if (page->scene() == scene)
			{
				found = true; break;
			}
			++pageIndex;
		}
		if (found) break;
		++articleIndex;
	}

	if (!found) return;
	setCurPage(articleIndex, pageIndex);
}

void MagazineDocumentTemplate::sceneChangeTo(int articleIndex, int pageIndex)
{
	if (articleIndex_ == articleIndex && pageIndex_ == pageIndex) return;
	if (mode_ != LayoutMode) notifyPageChanging(); 
	setCurPage(articleIndex, pageIndex);
	document_->triggerRedraw();
}

void MagazineDocumentTemplate::sceneChangeTo(Scene * scene)
{
	bool found = false;
	int articleIndex = 0; int pageIndex = 0; 
	BOOST_FOREACH(MagazineArticleSPtr article, articles_)
	{
		pageIndex = 0;
		BOOST_FOREACH(MagazinePageSPtr page, article->pages())
		{
			if (page->scene() == scene)
			{
				found = true; break;
			}
			++pageIndex;
		}
		if (found) break;
		++articleIndex;
	}

	if (found) sceneChangeTo(articleIndex, pageIndex);
	if (mode_ != PageMode) setMode(PageMode);
}

void MagazineDocumentTemplate::doLayoutTween(float startPos, float endPos)
{
	if (endPos < 0) endPos = 0;
	if (endPos > 1) endPos = 1;

	float thumbnailWidth = width_ * thumbnailScale_;
	float thumbnailHeight = height_ * thumbnailScale_;
	float gap = gapFactor_ * thumbnailWidth;
	float baseY = height_/2 - thumbnailHeight/2;

	int middleI = int((width_/2 - layoutX_) / (thumbnailWidth + gap));
	
	if (middleI < 0) middleI = 0;
	if (middleI > (int)articles_.size() - 1) middleI = (int)articles_.size() - 1;
	
	int deltaPageIndex = (int)roundFloat(-layoutOffsetY_ / (thumbnailHeight + gap));

	int newPageIndex = articles_[middleI]->curPage() + deltaPageIndex;
	if (newPageIndex < 0) newPageIndex = 0;
	if (newPageIndex > articles_[middleI]->numPages() - 1)
		newPageIndex = articles_[middleI]->numPages() - 1;

	layoutTweenEndArticleIndex_ = middleI;
	layoutTweenEndPageIndex_ = newPageIndex;

	deltaPageIndex = newPageIndex - articles_[middleI]->curPage();

	layoutTween_.start(Global::currentTime(), 
		Vector2(startPos, layoutOffsetY_), 
		Vector2(endPos, -deltaPageIndex * (thumbnailHeight + gap)));
}

void MagazineDocumentTemplate::setMode(Mode mode)
{
	bool curSceneHasToBeStopped = mode_ == PageMode;
	if (curSceneHasToBeStopped)
	{
		//stop any videos or audio that were playing on the previous page
		Global::instance().videoPlayer()->deactivate();
		Global::instance().audioPlayer()->stopAll();

		curSceneAppObjectStop();
	}

	if (mode_ == BookmarkListMode)
	{
		bookmarksScroller_->uninit();
	}

	menuBookmarkActiveButton_->visualAttrib()->setVisible(false);
	menuBookmarkInactiveButton_->visualAttrib()->setVisible(false);


	if (mode == LayoutMode)
	{
		int numArticles = (int)articles_.size();
		float factor;
		if (numArticles == 1)
			factor = 0;
		else
			factor = (float)articleIndex_ / (numArticles - 1);
		setScrollPos(factor);

		showMenu();
		menuHideTime_ = FLT_MAX;
		
	}
	else if (mode == BookmarkListMode)
	{
		bookmarksScroller_->init(document_->renderer());
		showMenu();
		menuHideTime_ = FLT_MAX;
	}
	else if (mode == PageMode)
	{
		if (mode_ == LayoutMode || mode_ == BookmarkListMode)
		{
			curSceneAppObjectStart();
			menuHideTime_ = Global::currentTime() + menuDuration_;
		}

		menuBookmarkActiveButton_->visualAttrib()->setVisible(true);
		menuBookmarkInactiveButton_->visualAttrib()->setVisible(true);
	}

	mode_ = mode;

	dragVelHandler_.clearMoveHistory();

}

void MagazineDocumentTemplate::showMenu()
{
	targetMenuOpacity_ = 1;
	menuTransitionStartTime_ = Global::currentTime();
	menuStartOpacity_ = menuOpacity_;
}

void MagazineDocumentTemplate::hideMenu()
{
	targetMenuOpacity_ = 0;
	menuTransitionStartTime_ = Global::currentTime();
	menuStartOpacity_ = menuOpacity_;
}

bool MagazineDocumentTemplate::isMenuShowing() const
{
	return menuOpacity_ > 0;
}

bool MagazineDocumentTemplate::isMenuHidden() const
{
	return menuOpacity_ == 0;
}

bool MagazineDocumentTemplate::update(float time)
{
	bool needRedraw = false;

	if (!subscene_->isLoaded())
		needRedraw = true;
	else
		needRedraw |= subscene_->update(time);
	
	if (redrawTriggered_)
	{
		redrawTriggered_ = false;
		needRedraw = true;
	}

	//menu opacity
	if (targetMenuOpacity_ != menuOpacity_)
	{

		float diff = targetMenuOpacity_ - menuStartOpacity_;

		float dir = 1;
		if (diff < 0) dir = -1;
		dir *= 4;
		menuOpacity_ = menuStartOpacity_ + dir*(time - menuTransitionStartTime_);

		if ((targetMenuOpacity_ - menuOpacity_) * diff < 0)
		{
			menuOpacity_ = targetMenuOpacity_;
		}

		BOOST_FOREACH(Image * menuImg, menuImgs_)
		{
			VisualAttrib * attr;
			attr = menuImg->visualAttrib();
			attr->setOpacity(menuOpacity_);
			if (menuOpacity_ == 0) attr->setVisible(false);
			else attr->setVisible(true);
		}

		needRedraw = true;
	}

	if (time > menuHideTime_)
	{
		menuHideTime_ = FLT_MAX;
		hideMenu();
	}

	//layout mode thumbnail drag acceleration / snap back
	if (mode_ == LayoutMode)
	{		
		Vector2 dragVelocity = dragVelHandler_.dragVelocity();		
		if (dragVelocity.x != 0 || dragVelocity.y != 0)
		{
			dragVelHandler_.update(time);			

			float minX, maxX;

			//set layout position
			float thumbnailWidth = width_ * thumbnailScale_;
			float thumbnailHeight = height_ * thumbnailScale_;
			float gap = gapFactor_ * thumbnailWidth;
			int numArticles = (int)articles_.size();
			maxX = width_/2 - thumbnailWidth/2;
			minX = maxX - (numArticles - 1) * (thumbnailWidth + gap);

			int middleI = int((width_/2 - layoutX_) / (thumbnailWidth + gap));			
			if (middleI < 0) middleI = 0;
			if (middleI > (int)articles_.size() - 1) middleI = (int)articles_.size() - 1;

			float minLayoutOffsetY = 0;
			float maxLayoutOffsetY = 0;

			if (middleI >= 0)
			{
				minLayoutOffsetY = 
					-(articles_[middleI]->numPages() - 1 - articles_[middleI]->curPage()) * 
					(thumbnailHeight + gap);

				maxLayoutOffsetY = 
					articles_[middleI]->curPage() * (thumbnailHeight + gap);
			}


			bool hasHitBoundaryX = false, hasHitBoundaryY = false;

			layoutX_ += dragVelHandler_.moveDelta().x;
			float factor;
			
			if (minX == maxX)
			{
				factor = 0.5f;
				hasHitBoundaryX = true;
			}
			else
			{
				factor = (layoutX_ - maxX)/(minX - maxX);
				if (factor < 0 || factor > 1) hasHitBoundaryX = true;			
			}
			

			layoutOffsetY_ += dragVelHandler_.moveDelta().y;
			
			if (layoutOffsetY_ < minLayoutOffsetY || layoutOffsetY_ > maxLayoutOffsetY) 
			{
				hasHitBoundaryY = true;
			}

			dragVelHandler_.hasHitBoundary(hasHitBoundaryX, hasHitBoundaryY);

			setScrollPos(factor);
			needRedraw = true;	

		}
		else if (!layoutModeSnappedBackYet_)
		{			
			int numArticles = (int)articles_.size();		
			float scrollEndPos = 0.5f;
			if (numArticles > 1)
			{
				scrollEndPos = (int)(scrollPos_ * (numArticles - 1) + 0.5f);
				scrollEndPos /= (numArticles - 1);
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

			layoutOffsetY_ = tweenedData.y;

			if (!layoutTween_.isHappening())
			{				
				layoutOffsetY_ = 0;
				articles_[layoutTweenEndArticleIndex_]->setCurPage(
					layoutTweenEndPageIndex_);
			}

			needRedraw = true;	
		}
		else if (pageMovingId_ == -1) //notPressed
		{
			requiredLayoutMoveDirection_ = Undetermined;
		}

	}

	if (transitionMode_ == PageFlipMode || transitionMode_ == PageFlipOrigMode)
	{
		needRedraw |= pageFlip_->update();
		static TransitionDirection prevTransDir;
		TransitionDirection transDir = pageFlip_->curTransitionDirection();
		
		if (transDir == TransitionDirectionNone && prevTransDir != transDir) 
		{
			if (pageFlip_->doPageChange()) setCurPage(targetArticleIndex_, targetPageIndex_);
			Global::instance().showAppLayer(true);		
		}		

		prevTransDir = transDir;
	}
	else
	{
		if (pageTween_.isHappening())
		{			
			
			Vector3 pos = pageTween_.update(time);
			setCurPagePos(pos);

			float thumbnailWidth = width_ * thumbnailScale_;
			float gap = gapFactor_ * thumbnailWidth;
			int numArticles = (int)articles_.size();
			float factor = 0.5f;
			if (numArticles > 1)
			{
				factor = (float)articleIndex_ / (numArticles - 1) - 
					pos.x / (width_* (numArticles - 1));
			}

			setScrollPos(factor);

			if (!pageTween_.isHappening())
			{
				Scene * prevScene = subscene_->scene();
				setCurPage(targetArticleIndex_, targetPageIndex_);
			}

			needRedraw = true;	
		}
		else
		{
			float normalScaleX;
			float normalScaleY;
			bool checkchange = false;
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
			Transform t = subscene_->transform();
	        
			if(checkOutside(t, normalScaleX, normalScaleY, false)) {
				subscene_->setTransform(t);
				needRedraw = true;
			}
		}
	}

	if (transitionStage_ == TransitionNeedToLoadTargetScene)
	{
		subsceneTransition_->asyncLoadUpdate();
		if (subsceneTransition_->isLoaded())
		{
			transitionStartTime_ = time;
			transitionStage_ = TransitionScreenShotNotTaken;
		}
		needRedraw = true;
	}
	else if (transitionStage_ == TransitionAnimating)
	{
		transitionT_ = (time - transitionStartTime_)/transitionDuration_;

		if (transitionT_ > 1) 
		{
			transitionStage_ = TransitionInactive;		
			if (transitionEndMode_ != mode_)
				setMode(transitionEndMode_);

			if (transitionEndMode_ == PageMode)
			{
				setCurPage(transitionEndArticle_, transitionEndPage_);
				subsceneTransition_->setScene(0);
			}
		}
		else 
		{
			transitionT_ = 1 - (transitionT_ - 1) * (transitionT_ - 1);
		}
		
		needRedraw = true;
	}
	
	if (bookmarksScroller_) 
	{
		needRedraw |= bookmarksScroller_->update(time);
		updateBookmarksScene();
	}

	if (subscene_->isLoaded())
	{
		Matrix m = subscene_->transform().computeMatrix();
		setBaseAppObjectOffset(
			m._14, m._24, 
			m._11, m._22,
			m._14, m._24, width_ * m._11 / subscene_->normalScaleX() , height_ * m._22 / subscene_->normalScaleY());
	}
	
	return needRedraw;
}

bool MagazineDocumentTemplate::asyncLoadUpdate()
{
	bool everythingLoaded = true;

	everythingLoaded &= subscene_->asyncLoadUpdate();
	if (subsceneCached_)everythingLoaded &= subsceneCached_->asyncLoadUpdate();

	everythingLoaded &= scrollBarBg_->asyncLoadUpdate();
	everythingLoaded &= scrollBarLeft_->asyncLoadUpdate();
	everythingLoaded &= scrollBarRight_->asyncLoadUpdate();
	everythingLoaded &= scrollBarThumb_->asyncLoadUpdate();
	everythingLoaded &= bg_->asyncLoadUpdate();
	everythingLoaded &= bookmarkTitle_->asyncLoadUpdate();
	everythingLoaded &= menuBg_->asyncLoadUpdate();

	BOOST_FOREACH(Image * menuImg, menuImgs_)
	{
		everythingLoaded &= menuImg->asyncLoadUpdate();
	}

	/*
	everythingLoaded &= menuHomeButton_->asyncLoadUpdate();
	everythingLoaded &= menuLayoutButton_->asyncLoadUpdate();
	everythingLoaded &= menuBookmarkActiveButton_->asyncLoadUpdate();
	everythingLoaded &= menuBookmarkInactiveButton_->asyncLoadUpdate();
	everythingLoaded &= menuBookmarkListButton_->asyncLoadUpdate();	
	everythingLoaded &= menuAspectActiveButton_->asyncLoadUpdate();
	everythingLoaded &= menuAspectInactiveButton_->asyncLoadUpdate();
	*/

	if (bookmarksScroller_) everythingLoaded &= bookmarksScroller_->asyncLoadUpdate();


	map<MagazinePage *, ThumbData>::iterator titr = thumbMap_.begin();
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

	if(!searchQ_.empty() && resetComplete_)
	{
		Scene * scene = searchQ_.front();
		searchQ_.pop_front();
		scene->requestLocalSaveObject();
	}
	
	return everythingLoaded;
}

int MagazineDocumentTemplate::numPages() const
{
	int numPages = 0;
	BOOST_FOREACH(MagazineArticleSPtr article, articles_)
	{
		numPages += article->numPages();
	}
	return numPages;
}

void MagazineDocumentTemplate::drawBookmarkList(GfxRenderer * gl) const
{
	gl->useTextureProgram();
	gl->setTextureProgramOpacity(1.0f);
	gl->use(bg_.get());
	gl->drawRect(0, 0, width_, height_);

	gl->useColorProgram();
	gl->setColorProgramColor(0.1176f, 0.1176f, 0.1176f, 1.0f);
	gl->drawRect(0, menuHeight_, width_, bookmarkTitleHeight_);

	gl->useTextureProgram();
	gl->use(bookmarkTitle_.get());
	gl->drawRect(0, menuHeight_, 
		(float)bookmarkTitle_->width(), bookmarkTitleHeight_);

	bookmarksScroller_->draw(gl);	
}

void MagazineDocumentTemplate::drawLayout(GfxRenderer * gl) const
{
	static GLfloat vertices[] = {
		0, 0, 0, 
		static_cast <GLfloat>(1.0f), 0, 0,
		static_cast <GLfloat>(0.5f), static_cast <GLfloat>(1.0f), 0
	};

	float thumbnailWidth = width_ * thumbnailScale_;
	float thumbnailHeight = height_ * thumbnailScale_;
	float gap = gapFactor_ * thumbnailWidth;

	float startY = height_/2 - thumbnailHeight/2;

	gl->useTextureProgram();
	
	gl->setTextureProgramOpacity(1.0f);
	gl->use(bg_.get());
	gl->drawRect(0, 0, width_, height_);
	
	gl->useColorProgram();
	
	//draw pointers
	gl->setColorProgramColor(1, 1, 1, 0.25f);

	vertices[0] = width_/2 - gap*0.8f/2;
	vertices[1] = startY - gap*0.8f;
	vertices[3] = width_/2 + gap*0.8f/2;
	vertices[4] = vertices[1];
	vertices[6] = width_/2;
	vertices[7] = startY - gap*0.2f;

	gl->enableVertexAttribArrayPosition();
	gl->bindArrayBuffer(0);
	gl->vertexAttribPositionPointer(0, (char *)vertices);	
	gl->applyCurrentShaderMatrix();
	glDrawArrays(GL_TRIANGLES, 0, 3);

	vertices[0] = width_/2;
	vertices[1] = startY + thumbnailHeight + gap*0.2f;
	vertices[3] = width_/2 - gap*0.8f/2;
	vertices[4] =  startY + thumbnailHeight + gap*0.8f;
	vertices[6] = width_/2 + gap*0.8f/2;
	vertices[7] = vertices[4];

	glDrawArrays(GL_TRIANGLES, 0, 3);

	gl->useCustomProgram(programThumb_, locThumbMVPMatrix_);

	map<MagazinePage *, ThumbData>::iterator iter;
	
	//flag all thumb textures as unused initially
	for (iter = thumbMap_.begin(); iter != thumbMap_.end(); ++iter)	
		(*iter).second.used = false;

	int i = -(layoutX_ + thumbnailWidth) / (thumbnailWidth + gap) + 1;
	int lastI = -(layoutX_ - width_) / (thumbnailWidth + gap) + 1;
	
	if (i < 0) i = 0;
	if (i > (int)articles_.size() - 1) i = (int)articles_.size() - 1;

	if (lastI < 0) lastI = 0;
	if (lastI > (int)articles_.size()) lastI = (int)articles_.size();

	float curX = layoutX_ + i * (thumbnailWidth + gap);

	int middleI = (int)((width_/2 - layoutX_) / (thumbnailWidth + gap));

	for (;i < lastI; ++i)
	{			
		float curY = startY;
		curY -= articles_[i]->curPage() * (thumbnailHeight + gap);

		if (i == middleI) curY += layoutOffsetY_;
		
		for (int j = 0; j < (int)articles_[i]->numPages(); ++j)
		{

			if (curY > -thumbnailHeight && curY < height_)
			{
				iter = 
					thumbMap_.find(articles_[i]->page(j));
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
					texture->init(gl, articles_[i]->page(j)->thumbnailFile(), Texture::UncompressedRgba32);					
					thumbData.texture.reset(texture);
					thumbMap_[articles_[i]->page(j)] = thumbData;
					thumbTexture = texture;
					thumbsLoadedConfirmed_ = false;
				}				
				gl->use(thumbTexture);				
				gl->drawRect(curX, curY, 
					(float)thumbWidth(), (float)thumbHeight());
				
			}
			curY += thumbnailHeight + gap;
		}
		curX += thumbnailWidth + gap;			
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

	//draw scrollbar regions
	gl->useTextureProgram();
	gl->setTextureProgramOpacity(1);

	
	gl->use(scrollBarBg_.get());

	float scrollGrooveWidth = 
		width_ - 
		(scrollBarLeft_->width() + scrollBarRight_->width());

	float scrollBgHeight = scrollBarBg_->height() * dpiScale_;
	float scrollThumbHeight=  scrollBarThumb_->height() * dpiScale_;

	gl->drawRect(
		scrollBarLeft_->width(), 
		height_ - scrollBgHeight, 
		scrollGrooveWidth, 
		scrollBgHeight);		
	

	gl->use(scrollBarLeft_.get());
	gl->drawRect(
		0, height_ - scrollBgHeight, 
		scrollBarLeft_->width(), 
		scrollBgHeight);

	gl->use(scrollBarRight_.get());
	gl->drawRect(
		width_ - scrollBarRight_->width(), 
		height_ - scrollBgHeight, 
		scrollBarRight_->width(), 
		scrollBgHeight);

	scrollBarThumb_->draw(gl);		
	

}

void MagazineDocumentTemplate::drawSubscene(GfxRenderer * gl, InternalSubscene * subscene) const
{		
	if (!subscene->isLoaded())
	{
		const Vector3 & trans = subscene->transform().translation();
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | Global::extraGlClearOptions());
		loadingPage_->draw(gl, trans.x, trans.y);
		document_->triggerRedraw();
	}
	else
	{
		bool isCurPage = subscene == subscene_.get();
		if (isCurPage && !curPageStarted_)
		{
			subscene->start(Global::currentTime());
			curSceneAppObjectStart();
			curPageStarted_ = true;
		}
		subscene->draw(gl);
	}
}

void MagazineDocumentTemplate::takeScreenshot(GfxRenderer * gl, InternalSubscene * subscene) const
{
#ifdef USE_FBO
		texRenderer_->beginRenderToTexture(gl);
		drawSubscene(gl, subscene);
		texRenderer_->endRenderToTexture(gl);
#else
		
		if (width_ != ssWidth_ || height_ != ssHeight_)
		{
			if (ssBuffer_) free(ssBuffer_);
			ssWidth_ = width_;
			ssHeight_ = height_;
			ssBuffer_ = (unsigned char *)malloc((ssWidth_ + 1) * (ssHeight_ + 1) * 2);

			if (renderTex_) glDeleteTextures(1, &renderTex_);
			glGenTextures(1, &renderTex_);
			glBindTexture(GL_TEXTURE_2D, renderTex_);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);			
		}


		Matrix camMatrix = gl->cameraMatrix();
		camMatrix = Matrix::Scale(1.0f, -1.0f, 1.0f) * camMatrix;
		gl->setCameraMatrix(camMatrix);
		drawSubscene(gl, subscene);
		camMatrix = Matrix::Scale(1.0f, -1.0f, 1.0f) * camMatrix;
		gl->setCameraMatrix(camMatrix);
		
		
		/*
		glReadPixels(document_->glViewPortX(), document_->glViewPortY(), ssWidth_, ssHeight_, 
			GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ssBuffer_);
		*/
		glReadPixels(0, 0, ssWidth_, ssHeight_, 
			GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ssBuffer_);

		glBindTexture(GL_TEXTURE_2D, renderTex_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
			ssWidth_, ssHeight_, 
			0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ssBuffer_);
#endif
}

void MagazineDocumentTemplate::draw(GfxRenderer * gl) const
{
	glDepthFunc(GL_ALWAYS);

	if (transitionStage_ == TransitionScreenShotNotTaken)
	{
		takeScreenshot(gl, subsceneTransition_.get());
		transitionStage_ = TransitionAnimating;
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | Global::extraGlClearOptions());

	gl->setCameraMatrix(camera_.GetMatrix());
	gl->loadMatrix(Matrix::Identity());


	Matrix transform;	
		
	if (mode_ == PageMode)
	{	

		if (transitionMode_ == PageFlipMode || transitionMode_ == PageFlipOrigMode)
		{
			TransitionDirection tdir = pageFlip_->curTransitionDirection();
			if (tdir != TransitionDirectionNone)
			{
				if (tdir == TransitionDirectionLeft || tdir == TransitionDirectionUp)
				{
					if (pageFlip_->texture() == NULL && subsceneCached_->isLoaded()) 
					{										
						takeScreenshot(gl, subsceneCached_.get());
#ifdef USE_FBO
						pageFlip_->setTexture(renderTex_);
#else
						pageFlip_->setGlTexture(renderTex_);
#endif
					}

					subscene_->setMode(InternalSubscene::ModeFixed);
					drawSubscene(gl, subscene_.get());
					pageFlip_->draw(gl);					
				}
				else
				{
					if (pageFlip_->texture() == NULL) 
					{										
						takeScreenshot(gl, subscene_.get());
#ifdef USE_FBO 
						pageFlip_->setTexture(renderTex_);
#else
						pageFlip_->setGlTexture(renderTex_);
#endif
					}

					drawSubscene(gl, subsceneCached_.get());
					pageFlip_->draw(gl);					
				}
			}
			else
			{
				if (pageFlip_->texture()) pageFlip_->setTexture(NULL);
				subscene_->setMode(InternalSubscene::ModeFixed);
				drawSubscene(gl, subscene_.get());
			}
			
		}
		else
		{
			bool drawCached = false;
			switch(whichCached_)
			{
			case CachedLeft:
				drawCached = subsceneCached_->transform().translation().x > -width_;
				break;
			case CachedRight:
				drawCached = subsceneCached_->transform().translation().x < width_;
				break;
			case CachedTop:
				drawCached = subsceneCached_->transform().translation().y > -height_;
				break;
			case CachedBottom:
				drawCached = subsceneCached_->transform().translation().y < height_;
				break;
			}

			if (drawCached)
			{
				subscene_->setMode(InternalSubscene::ModeScrollable);
				drawSubscene(gl, subsceneCached_.get());
				drawSubscene(gl, subscene_.get());
			}
			else
			{
				subscene_->setMode(InternalSubscene::ModeFixed);
				drawSubscene(gl, subscene_.get());
			}
		}	
			
	}
	
	else if (mode_ == LayoutMode)
	{
		drawLayout(gl);
	}	
	else if (mode_ == BookmarkListMode)
	{		
		drawBookmarkList(gl);
	}	

	
	if (transitionStage_ == TransitionAnimating)
	{
		float scale = transitionStartScale_ + 
			transitionT_ * (transitionEndScale_ - transitionStartScale_);
		Vector2 center = transitionStartPt_ + 
			transitionT_ * (transitionEndPt_ - transitionStartPt_);

		gl->useTextureProgram();
		gl->setTextureProgramOpacity(1);

		gl->use(renderTex_);
		gl->drawRect(
			center.x - width_ * scale/2, center.y - height_ * scale/2,
			width_ * scale, height_ * scale
			);

	}

	if (menuOpacity_ > 0.0f)
	{
		menuBg_->draw(gl);
		menuHomeButton_->draw(gl);
		menuLayoutButton_->draw(gl);
		
		if (doBookmarks_)
		{
			if (curPageHasBookmark_)
				menuBookmarkActiveButton_->draw(gl);
			else	
				menuBookmarkInactiveButton_->draw(gl);

			menuBookmarkListButton_->draw(gl);
		}

		if (toContentsPage_)
		{
			menuToContentsButton_->draw(gl);
		}
		if (doAspect_)
		{
			if (preserveAspect_)
				menuAspectActiveButton_->draw(gl);
			else
				menuAspectInactiveButton_->draw(gl);
		}

	}
	

}

void MagazineDocumentTemplate::start(float docTime)
{
	setCurPage(0, 0);
	Global::setPreserveAspectRatio(preserveAspect_);
}

Scene * MagazineDocumentTemplate::dependsOn(Scene * scene) const
{
	BOOST_FOREACH(MagazineArticleSPtr article, articles_)
	{
		BOOST_FOREACH(MagazinePageSPtr page, article->pages())
		{
			if (page->scene() == scene || page->scene()->dependsOn(scene))
				return page->scene();
		}
	}

	return 0;
}

void MagazineDocumentTemplate::pageScenes(std::vector<Scene *> * scenes) const
{
	BOOST_FOREACH(MagazineArticleSPtr article, articles_)
	{
		BOOST_FOREACH(MagazinePageSPtr page, article->pages())
			scenes->push_back(page->scene());
	}
}

void MagazineDocumentTemplate::referencedFiles(
	vector<string> * refFiles) const
{
	refFiles->push_back(scrollBarBgFile_);
	refFiles->push_back(scrollBarLeftSideFile_);
	refFiles->push_back(scrollBarRightSideFile_);
	refFiles->push_back(topMenuBgFile_);
	refFiles->push_back(topMenuLayoutButtonFile_);
	refFiles->push_back(topMenuHomeButtonFile_);
	refFiles->push_back(scrollBarThumbFile_);	
	refFiles->push_back(bgFile_);
	refFiles->push_back(bookmarkActiveFile_);
	refFiles->push_back(bookmarkInactiveFile_);
	refFiles->push_back(bookmarkListFile_);
	refFiles->push_back(bookmarkTitleFile_);
	refFiles->push_back(aspectActiveFile_);
	refFiles->push_back(aspectInactiveFile_);	
	refFiles->push_back(toContentsFile_);
	

	BOOST_FOREACH(MagazineArticleSPtr article, articles_)
	{
		BOOST_FOREACH(MagazinePageSPtr page, article->pages())
			refFiles->push_back(page->thumbnailFile());
	}
}

int MagazineDocumentTemplate::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	scrollBarBgFile_ = refFiles[index++];
	scrollBarLeftSideFile_ = refFiles[index++];
	scrollBarRightSideFile_ = refFiles[index++];
	topMenuBgFile_ = refFiles[index++];
	topMenuLayoutButtonFile_ = refFiles[index++];	
	topMenuHomeButtonFile_ = refFiles[index++];
	scrollBarThumbFile_ = refFiles[index++];	
	bgFile_ = refFiles[index++];
	bookmarkActiveFile_ = refFiles[index++];
	bookmarkInactiveFile_ = refFiles[index++];
	bookmarkListFile_ = refFiles[index++];
	bookmarkTitleFile_ = refFiles[index++];
	aspectActiveFile_ = refFiles[index++];
	aspectInactiveFile_ = refFiles[index++];
	toContentsFile_ = refFiles[index++];

	BOOST_FOREACH(MagazineArticleSPtr article, articles_)
	{
		BOOST_FOREACH(MagazinePageSPtr page, article->pages())
			page->setThumbnailFile(refFiles[index++]);
	}
	return index;
}

void MagazineDocumentTemplate::write(Writer & writer) const
{
	writer.writeParent<DocumentTemplate>(this);
	writer.write(visualAttrib_, "visualAttrib");
	writer.write(articles_);

	BOOST_FOREACH(string * str, uiImgFiles_)	
	{
		writer.write(*str, "ui image");
	}

	writer.write(width_, "width");
	writer.write(height_, "height");

	writer.write(pageChangingMsg_, "pageChangingMsg");
	
	writer.write(doBookmarks_);	
	writer.write(homeButtonFunction_);
	writer.write(doAspect_);

	writer.write(resetArticleFirstPage_);

	writer.write(toContentsPage_);
	writer.write(contentsArticleIndex_);

	writer.write(transitionMode_);

	writer.write(isPreDownload_, "preDownload");
}

void MagazineDocumentTemplate::writeXml(XmlWriter & w) const
{
	w.startTag("Articles");
	for (int i = 0; i < (int)articles_.size(); ++i)
	{
		w.startTag("Article");
		articles_[i]->writeXml(w);
		w.endTag();
	}
	w.endTag();

	w.startTag("UiImgFiles");
	BOOST_FOREACH(string * str, uiImgFiles_)	
	{
		w.writeTag("FileName", convertToRelativePath(Global::instance().readDirectory(), *str));
	}
	w.endTag();

	w.writeTag("DoBookmarks", doBookmarks_);
	w.writeTag("HomeButtonFunction", (unsigned int)homeButtonFunction_);
	w.writeTag("DoAspect", doAspect_);
	w.writeTag("ResetArticleFirstPage", resetArticleFirstPage_);
	w.writeTag("ToContentsPage", toContentsPage_);
	w.writeTag("TransitionMode", (unsigned int)transitionMode_);

}

void MagazineDocumentTemplate::readXml(XmlReader & r, xmlNode * parent)
{	
	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.isNodeName(curNode, "Articles"))
		{
			for(xmlNode * articleNode = curNode->children; articleNode; articleNode = articleNode->next)
			{		
				if (articleNode->type != XML_ELEMENT_NODE) continue;

				if (r.isNodeName(articleNode, "Article"))
				{
					MagazineArticleSPtr article(new MagazineArticle);
					article->readXml(r, articleNode);
					articles_.push_back(article);
					articleNode = articleNode->next;
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
		else if (r.getNodeContentIfName(doBookmarks_, curNode, "DoBookmarks"));
		else if (r.getNodeContentIfName(val, curNode, "HomeButtonFunction"))
		{
			homeButtonFunction_ = (HomeButtonFunction)val;
		}
		else if (r.getNodeContentIfName(doAspect_, curNode, "DoAspect"));
		else if (r.getNodeContentIfName(resetArticleFirstPage_, curNode, "ResetArticleFirstPage"));
		else if (r.getNodeContentIfName(toContentsPage_, curNode, "ToContentsPage"));
		else if (r.getNodeContentIfName(val, curNode, "TransitionMode"))
		{
			transitionMode_ = (TransitionMode)val;
		}
	}
}

void MagazineDocumentTemplate::read(Reader & reader, unsigned char version)
{
	if (version < 4)
		throw Exception("Unsupported version");

	reader.readParent<DocumentTemplate>(this);
	reader.read(visualAttrib_);
	reader.read(articles_);	

	if (version >= 16)
	{
		for (int i = 0; i < 15; ++i) reader.read(*uiImgFiles_[i]);
	}
	else if (version >= 14)
	{
		for (int i = 0; i < 14; ++i) reader.read(*uiImgFiles_[i]);
	}
	else if (version == 13 || version == 12)
	{
		for (int i = 0; i < 12; ++i) reader.read(*uiImgFiles_[i]);
	}
	else if (version == 11)
		{
		for (int i = 0; i < 11; ++i) reader.read(*uiImgFiles_[i]);
		}
	else if (version == 10)
	{
		for (int i = 0; i < 8; ++i) reader.read(*uiImgFiles_[i]);
	}
	else if (version == 9)
	{
		std::vector<std::string *> uiImgFiles;
		std::string dummy;
		for (int i = 0; i < 9; ++i) uiImgFiles.push_back(&dummy);
		BOOST_FOREACH(string * str, uiImgFiles) reader.read(*str);
	}
	else if (version == 7 || version == 8)
	{
		std::vector<std::string *> uiImgFiles;
		std::string dummy;
		for (int i = 0; i < 8; ++i) uiImgFiles.push_back(&dummy);
		BOOST_FOREACH(string * str, uiImgFiles) reader.read(*str);	
	}
	else
	{
		for (int i = 0; i < 7; ++i)
		{
			reader.read(*uiImgFiles_[i]);
		}
	}

	if (version >= 4)
	{
		reader.read(width_);
		reader.read(height_);
	}

	if (version >= 6)
	{
		reader.read(pageChangingMsg_);
	}

	if (version >= 13)
	{
		reader.read(doBookmarks_);
		reader.read(homeButtonFunction_);
	}

	if (version >= 14)
	{
		reader.read(doAspect_);
	}

	if (version >= 15)
	{
		reader.read(resetArticleFirstPage_);
	}

	if (version >= 16)
	{
		reader.read(toContentsPage_);
		reader.read(contentsArticleIndex_);
	}

	if (version >= 17)
	{
		reader.read(transitionMode_);
	}

	if (version >= 19)
		reader.read(isPreDownload_);
}

void MagazineDocumentTemplate::setUiImgFiles(const std::vector<std::string> & uiImgFiles)
{
	int index = 0;
	scrollBarBgFile_ = uiImgFiles[index++];
	scrollBarLeftSideFile_ = uiImgFiles[index++];
	scrollBarRightSideFile_ = uiImgFiles[index++];
	topMenuBgFile_ = uiImgFiles[index++];
	topMenuLayoutButtonFile_ = uiImgFiles[index++];
	topMenuHomeButtonFile_ = uiImgFiles[index++];
	scrollBarThumbFile_ = uiImgFiles[index++];	
	bgFile_ = uiImgFiles[index++];
	bookmarkActiveFile_ = uiImgFiles[index++];
	bookmarkInactiveFile_ = uiImgFiles[index++];
	bookmarkListFile_ = uiImgFiles[index++];
	bookmarkTitleFile_ = uiImgFiles[index++];
	aspectActiveFile_ = uiImgFiles[index++];
	aspectInactiveFile_ = uiImgFiles[index++];
	toContentsFile_ = uiImgFiles[index++];
}

void MagazineDocumentTemplate::uiImgFiles(std::vector<std::string> * uiImgFiles)
{
	uiImgFiles->clear();
	uiImgFiles->push_back(scrollBarBgFile_);
	uiImgFiles->push_back(scrollBarLeftSideFile_);
	uiImgFiles->push_back(scrollBarRightSideFile_);
	uiImgFiles->push_back(topMenuBgFile_);
	uiImgFiles->push_back(topMenuLayoutButtonFile_);
	uiImgFiles->push_back(topMenuHomeButtonFile_);
	uiImgFiles->push_back(scrollBarThumbFile_);	
	uiImgFiles->push_back(bgFile_);
	uiImgFiles->push_back(bookmarkActiveFile_);
	uiImgFiles->push_back(bookmarkInactiveFile_);
	uiImgFiles->push_back(bookmarkListFile_);
	uiImgFiles->push_back(bookmarkTitleFile_);
	uiImgFiles->push_back(aspectActiveFile_);
	uiImgFiles->push_back(aspectInactiveFile_);
	uiImgFiles->push_back(toContentsFile_);
}

void MagazineDocumentTemplate::reInitCurScene(GfxRenderer * gl)
{
	if (!subscene_->isInit()) return;
	subscene_->resizeText(gl);
	if (!subsceneCached_->isInit()) return;
	subsceneCached_->resizeText(gl);
	if (!subsceneTransition_->isInit()) return;
	subsceneTransition_->resizeText(gl);
}

bool MagazineDocumentTemplate::checkOutside(Transform & t, float normalScaleX, float normalScaleY, bool hasDirectionScene)
{
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
	else if(mat._14 * normalScaleX > subscene_->sceneWidth() * normalScaleX - (subscene_->sceneWidth() * normalScaleX / t.scaling().x * normalScaleX) && (!(pageRight()) || hasDirectionScene))
	{
		Matrix m = mat;
		m._14 = (subscene_->sceneWidth() * normalScaleX - (subscene_->sceneWidth() * normalScaleX / t.scaling().x * normalScaleX)) / normalScaleX;
		mat = m;
		m = m.inverse();
		t.setMatrix(m, true);
		checkchange = true;
	}
	if(mat._24 * normalScaleY < 0 && (!(pageTop()) || hasDirectionScene))
	{
		Matrix m = t.computeMatrix();
		m._24 = 0;
		t.setMatrix(m, true);
		checkchange = true;
	}
	else if(mat._24 * normalScaleY > subscene_->sceneHeight() * normalScaleY - (subscene_->sceneHeight() * normalScaleY / t.scaling().y * normalScaleY) && (!(pageBottom()) || hasDirectionScene))
	{
		Matrix m = mat;
		m._24 = (subscene_->sceneHeight() * normalScaleY - (subscene_->sceneHeight() * normalScaleY / t.scaling().y * normalScaleY)) / normalScaleY;
		m = m.inverse();
		t.setMatrix(m, true);
		checkchange = true;
	}
	return checkchange;
}

void MagazineDocumentTemplate::resetSaveLocalRequest()
{
    Global & g = Global::instance();
    g.resetSaveLocalRequest();
	resetComplete_ = false;
	searchQ_.clear();
	int searchArticleDepth, seartchPageDepth;
	if(articles_.size()/2 > articleIndex_)
		searchArticleDepth = articles_.size() - articleIndex_;
	else
		searchArticleDepth = articleIndex_;

	//if (articles_[articleIndex_]->numPages()/2 > pageIndex_)
	//	seartchPageDepth = articles_[articleIndex_]->numPages() - pageIndex_;
	//else
	//	seartchPageDepth = pageIndex_;

	for(int i = 0 ; i < searchArticleDepth ; i++)
	{
		MagazineArticle* article = NULL;
		if (articleIndex_+i < articles_.size() && i!=0)
		{
			//searchQ_.push_back(scenes_[pageIndex_+i]);
			article = articles_[articleIndex_+i].get();
			for (int j = 0; j < article->numPages() ; j++)
			{
				searchQ_.push_back(article->page(j)->scene());
			}
		}
		if (articleIndex_-i >= 0 && i!=0)
		{
			//searchQ_.push_back(scenes_[pageIndex_-i]);
			article = articles_[articleIndex_-i].get();
			for (int j = 0; j < article->numPages() ; j++)
			{
				searchQ_.push_back(article->page(j)->scene());
			}
		}

		//if (article)
		//{
			//if (i != 0)
			//{
				
			//}
			//else
			//{
			//	for (int j = 0; j < seartchPageDepth ; j++)
			//	{
			//		if (pageIndex_+j < article->numPages() && j!=0)
			//		{
			//			//searchQ_.push_back(scenes_[pageIndex_+i]);
			//			searchQ_.push_back(article->page(pageIndex_+j)->scene());
			//		}
			//		if (pageIndex_-j >= 0 && j!=0)
			//		{
			//			//searchQ_.push_back(scenes_[pageIndex_-i]);
			//			searchQ_.push_back(article->page(pageIndex_-j)->scene());
			//		}
			//	}
			//}
		//}
	}
	resetComplete_ = true;
}