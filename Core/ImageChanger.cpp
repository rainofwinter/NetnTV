#include "stdafx.h"
#include "Subscene.h"
#include "ImageChanger.h"
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
#include "Global.h"
#include "Document.h"
#include "LoadingPage.h"
#include "PageChangedEvent.h"
#include "ScriptProcessor.h"

using namespace std;

JSClass jsImageChangerClass = InitClass(
	"ImageChanger", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);


JSBool ImageChanger_changeImage(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ImageChanger * thisObj = (ImageChanger *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	uint32 imageIndex;
	if (!JS_ConvertArguments(cx, argc, argv, "u", &imageIndex)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	thisObj->imageChangeTo(imageIndex);
	return JS_TRUE;
};



JSFunctionSpec ImageChangerFuncs[] = {
	JS_FS("changeImage", ImageChanger_changeImage, 1, 0),

    JS_FS_END
};

///////////////////////////////////////////////////////////////////////////////

JSObject * ImageChanger::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsImageChangerClass,
		0, 0, 0, ImageChangerFuncs, 0, 0);

	return proto;
}

JSObject * ImageChanger::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsImageChangerClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void ImageChanger::create()
{
	width_ = 768;
	height_ = 1024;	
	redrawTriggered_ = false;

	subscene_.reset(new Image);
	subscenePrev_.reset(new Image);
	subsceneNext_.reset(new Image);
	
	pageTweenEndPos_ = Vector3(0, 0, 0);
	pageTweenStartTime_ = pageTweenDuration_ = 0;
	pageTweenLastT_ = 0;

	startPagePos_ = Vector3(0, 0, 0);
	allowDrag_ = true;

	showMarkers_ = false;
	resetPageTween();
	pageIndex_ = -1;
	draggingOnMarkers_ = false;
	loadingIcon_ = new LoadingScreen;
	format_ = Texture::UncompressedRgba32;

	scrollDirection_ = Horizontal;
	markerLocation_ = Bottom;

	markerOffsetX_ = 0;
	markerOffsetY_ = 0;
	pageMovingId_ = -1;

	prevAutoTransitionTime_ = -FLT_MAX; 
	autoTransitionTime_ = 0.0f;
}

ImageChanger::ImageChanger()
{
	create();
	setId("ImageChanger");
}

ImageChanger::ImageChanger(
	const ImageChanger & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	visualAttrib_ = rhs.visualAttrib_;
	width_ = rhs.width_;
	height_ = rhs.height_;

	BOOST_FOREACH(string fileName, rhs.fileNames_)
		fileNames_.push_back(fileName);		

	activeMarkerFileName_ = rhs.activeMarkerFileName_;
	inactiveMarkerFileName_ = rhs.inactiveMarkerFileName_;
	allowDrag_ = rhs.allowDrag_;

	showMarkers_ = rhs.showMarkers_;
	scrollDirection_ = rhs.scrollDirection_;
	markerLocation_ = rhs.markerLocation_;
	markerOffsetX_ = rhs.markerOffsetX_;
	markerOffsetY_ = rhs.markerOffsetY_;
	

}

ImageChanger::~ImageChanger()
{
	uninit();
	delete loadingIcon_;
}

void ImageChanger::setFileNames(const std::vector<std::string> & fileNames)
{	
	fileNames_ = fileNames;
	int newPageIndex = pageIndex_;
	if (pageIndex_ >= fileNames.size()) newPageIndex = 0;
	if (fileNames.size() == 0) newPageIndex = -1;	
	setCurPage(-1);
	setCurPage(newPageIndex);
}


void ImageChanger::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
	float startTime = Global::currentTime();

	Transform trans;

	subscene_->setFormat(format_);
	subscene_->init(gl);
	setAsSubObject(subscene_.get());
	subscenePrev_->setFormat(format_);
	subscenePrev_->init(gl);
	setAsSubObject(subscenePrev_.get());
	subsceneNext_->setFormat(format_);
	subsceneNext_->init(gl);
	setAsSubObject(subsceneNext_.get());	

	activePageImg_.reset(new Texture);	
	inactivePageImg_.reset(new Texture);


	//activePageImg_->init("c:\\active.png");
	//inactivePageImg_->init("c:\\inactive.png");

	if (showMarkers_)
	{
	activePageImg_->init(gl, activeMarkerFileName_, format_);
	inactivePageImg_->init(gl, inactiveMarkerFileName_, format_);
	}
	asyncLoadUpdate();
	loadingIcon_->init(gl, width_, height_);
	if (pageIndex_ == -1) setCurPage(0);
	redrawTriggered_ = true;
}

void ImageChanger::uninit()
{		
	if (!gl_) return;
	
	subscene_->uninit();
	subscenePrev_->uninit();
	subsceneNext_->uninit();

	activePageImg_->uninit();
	inactivePageImg_->uninit();

	inactivePageImg_.reset();
	activePageImg_.reset();

	loadingIcon_->uninit();
	
	SceneObject::uninit();
	
}

const string * ImageChanger::pagePrev() const
{
	if (pageIndex_ <= 0) return 0;
	return &fileNames_[pageIndex_ - 1];
}

const string * ImageChanger::pageNext() const
{
	if (pageIndex_ >= (int)fileNames_.size() - 1) return 0;
	return &fileNames_[pageIndex_ + 1];	
}

void ImageChanger::setInitPage()
{	
	if (pageIndex_ >= 0) return; 
	if (fileNames_.empty()) return;

	subscene_->setFileName(fileNames_[0]);
}

void ImageChanger::setCurPage(int pageIndex)
{
	bool sceneChanged = false;	
	if (pageIndex_ != pageIndex) sceneChanged = true;
	if (!sceneChanged) return;

	if (pageIndex >= (int)fileNames_.size()) return;

	Document * doc = parentScene_->parentDocument();

	bool oneLeft = false;
	bool oneRight = false;
	if (pageIndex_ >= 0)
	{
		if (pageIndex == pageIndex_ + 1) oneRight = true;
		if (pageIndex == pageIndex_ - 1) oneLeft = true;
	}

	pageIndex_ = pageIndex;

	const string * pstr;

	if (oneRight)
	{
		subscene_->swap(subscenePrev_.get());
		subscene_->swap(subsceneNext_.get());
		pstr = pageNext();		
		if (pstr) subsceneNext_->setFileName(*pstr);
		else if (!fileNames_.empty()) subsceneNext_->setFileName(fileNames_.front());
		else subsceneNext_->setFileName("");
		subsceneNext_->init(doc->renderer());

	}
	else if (oneLeft)
	{
		subscene_->swap(subsceneNext_.get());
		subscene_->swap(subscenePrev_.get());
		pstr = pagePrev();
		if (pstr) subscenePrev_->setFileName(*pstr);
		else if (!fileNames_.empty()) subscenePrev_->setFileName(fileNames_.back());
		else subscenePrev_->setFileName("");
		subscenePrev_->init(doc->renderer());
	}
	else
	{
		if (subscenePrev_->fileName() == fileNames_[pageIndex])
		{
			subscene_->swap(subscenePrev_.get());
		}
		else if (subsceneNext_->fileName() == fileNames_[pageIndex])
		{
			subscene_->swap(subsceneNext_.get());
		}
		else
		{
			subscene_->setFileName(fileNames_[pageIndex]);
			subscene_->init(doc->renderer());
		}

		pstr = pagePrev();
		if (pstr) subscenePrev_->setFileName(*pstr);
		else if (!fileNames_.empty()) subsceneNext_->setFileName(fileNames_.back());
		else subscenePrev_->setFileName("");
		subscenePrev_->init(doc->renderer());

		pstr = pageNext();
		if (pstr) subsceneNext_->setFileName(*pstr);
		else if (!fileNames_.empty()) subsceneNext_->setFileName(fileNames_.front());
		else subsceneNext_->setFileName("");
		subsceneNext_->init(doc->renderer());
	}

	setCurPagePos(Vector3(0, 0, 0));	

	doPageChangedEvent(pageIndex);
}

bool ImageChanger::hasPageBeenMoved() const
{
	if (!subscene_) return false;
	const Vector3 & trans = subscene_->transform().translation();
	if (trans.x == 0.0f && trans.y == 0.0f) return false;
	return true;
}

void ImageChanger::doPageSnapBack(const PressEventData & pressEvent, const Vector2 & pt)
{
	Vector2 startPt = pressEvent.startPt;
	resetPageTween();
	pageTweenStartTime_ = Global::currentTime();
	pageTweenDuration_ = 0.3f;
	pageTweenStartPos_ = subscene_->transform().translation();
	pageTweenEndPos_ = Vector3(0, 0, 0);
	targetPageIndex_ = pageIndex_;

	bool isQuickFlick = 
		Global::currentTime() - pressEvent.startMovingTime < 
		Global::flickTime();
	
	float maxX = 0;
	float maxY = 0;
	float halfPos;
	float startPos;
	float curPos;
	float scenePos;

	if (scrollDirection_ == Horizontal)
	{
		maxX = width_;
		startPos = startPt.x;
		curPos = pt.x;	
		scenePos = pageTweenStartPos_.x;
		halfPos = width_/2;
	}
	else
	{
		maxY = height_;
		startPos = startPt.y;
		curPos = pt.y;		
		scenePos = pageTweenStartPos_.y;
		halfPos = height_/2;
	}
	
	if (isQuickFlick)
	{
		if (curPos > startPos && pagePrev())
		{
			pageTweenEndPos_ = Vector3(maxX, maxY, 0);
			targetPageIndex_ = pageIndex_ - 1;
		}
		else if (curPos < startPos && pageNext())
		{
			pageTweenEndPos_ = Vector3(-maxX, -maxY, 0);		
			targetPageIndex_ = pageIndex_ + 1;
		}
	}
	else
	{
		if (scenePos > halfPos && pagePrev())
		{
			pageTweenEndPos_ = Vector3(maxX, maxY, 0);
			targetPageIndex_ = pageIndex_ - 1;
		}	
		else if (scenePos < -halfPos && pageNext())		
		{
			pageTweenEndPos_ = Vector3(-maxX, -maxY, 0);		
			targetPageIndex_ = pageIndex_ + 1;
		}
	}
}

void ImageChanger::setCurPagePos(const Vector3 & pos)
{
	Vector3 pagePos = pos;
	Transform transform;
	
	float maxX = 0, maxY = 0;
	if (scrollDirection_ == Horizontal)
		maxX = width_;
	else
		maxY = height_;	
	
	transform = subscene_->transform();
	transform.setTranslation(pagePos.x, pagePos.y, 0);
	subscene_->setTransform(transform);	

	pagePos = pos + Vector3(-maxX, -maxY, 0);
	transform = subscenePrev_->transform();
	transform.setTranslation(pagePos.x, pagePos.y, 0);
	subscenePrev_->setTransform(transform);	

	pagePos = pos + Vector3(maxX, maxY, 0);
	transform = subsceneNext_->transform();
	transform.setTranslation(pagePos.x, pagePos.y, 0);
	subsceneNext_->setTransform(transform);		
}

bool ImageChanger::pressEvent(const Vector2 & screenPos, int pressId)
{
	PressEventData & pressEvent = pressEvents_[pressId];
	if (isPageTweenHappening()) 
	{
		pressEvent.pressed = PressEventData::StrayPress;
		return true;
	}

	const Camera * cam = parentScene_->camera();
	Ray mouseRay = cam->unproject(parentScene_->screenToDeviceCoords(screenPos));
	Vector3 startPt;
	intersectRect(&startPt, mouseRay);
	transMatrix_ = parentTransform() * visualAttrib_.transformMatrix();
	invTransMatrix_ = transMatrix_.inverse();
	startPt = invTransMatrix_ * startPt;

	startPagePos_ = subscene_->transform().translation();	
		
	pressEvent.startedMoving = false;
	pressEvent.pressed = PressEventData::Pressed;
	pressEvent.pressPt = Vector2(startPt);

	if (!allowDrag_) 
	{
		pressEvent.pressed = PressEventData::NotPressed;
		return false;
	}

	draggingOnMarkers_ = false;
	if (showMarkers_ && cursorOnWhichMarker(startPt) >= 0) draggingOnMarkers_ = true;

	if (pageMovingId_ == -1) pageMovingId_ = pressId;
	return true;
}

bool ImageChanger::doPageChangedEvent(int index)
{
	static SceneChangerPageChangedEvent event;
	event.setIndex(index);
	return handleEvent(&event, Global::currentTime());
}

void ImageChanger::setRangePos(float pos)
{
	if (pos < 0) pos = 0;
	if (pos > 1) pos = 1;
	int numPages = (int)fileNames_.size();

	int pageIndex;
	float pixelPos;
	float pagePos;
	float maxPos;
	float pageX = 0, pageY = 0;

	if (scrollDirection_ == Horizontal)
		maxPos = width_;
	else
		maxPos = height_;

	pixelPos = maxPos * (numPages - 1) * pos;			
	pageIndex = (int)(pixelPos / maxPos);
	pagePos = pixelPos - pageIndex*maxPos;
		
	if (pagePos > maxPos / 2)
	{
		pageIndex++;
		pagePos -= maxPos;
	}	

	setCurPage(pageIndex);

	if (scrollDirection_ == Horizontal)
	{
		setCurPagePos(Vector3(-pagePos, 0, 0));	
	}
	else
	{
		setCurPagePos(Vector3(0, -pagePos, 0));
	}

}

float ImageChanger::getCursorRangePos(const Vector3 & pt)
{
	int numScenes = (int)fileNames_.size();	

	if (markerLocation_ == Top || markerLocation_ == Bottom)
	{
	float maxWidth = (float)activePageImg_->width();
		if (inactivePageImg_->width() > maxWidth)
			maxWidth = (float)inactivePageImg_->width();

	float minX = 0.5f*width_ - 0.5f*(numScenes * maxWidth) + maxWidth/2;	
	float maxX = minX + (numScenes - 1)*maxWidth;

	float pos = (pt.x - minX)/(maxX - minX);
	if (pos < 0) pos = 0;
	if (pos > 1) pos = 1;
	return pos;	
}
	else
	{
		float maxHeight = (float)activePageImg_->height();
		if (inactivePageImg_->height() > maxHeight)
			maxHeight = (float)inactivePageImg_->height();

		float minY = 0.5f*height_ - 0.5f*(numScenes * maxHeight) + maxHeight/2;	
		float maxY = minY + (numScenes - 1)*maxHeight;

		float pos = (pt.y - minY)/(maxY - minY);
		if (pos < 0) pos = 0;
		if (pos > 1) pos = 1;
		return pos;	
	}
}

int ImageChanger::cursorOnWhichMarker(const Vector3 & pt)
{
	int numScenes = (int)fileNames_.size();	

	float maxWidth = (float)activePageImg_->width();
	if (inactivePageImg_->width() > maxWidth)
		maxWidth = (float)inactivePageImg_->width();
	float maxHeight = (float)activePageImg_->height();
	if (inactivePageImg_->height() > maxHeight) 
		maxHeight = (float)inactivePageImg_->height();

	float dX = 0;
	float dY = 0;
	float curX, curY;
	
	if (markerLocation_ == Top || markerLocation_ == Bottom)		
		dX = maxWidth;	
	else	
		dY = maxHeight;	
	
	switch (markerLocation_)
		{
	case Top: 
		curX = 0.5f*width_ - 0.5f*(numScenes * maxWidth);	
		curY = 0;	
		break;
	case Bottom: 
		curX = 0.5f*width_ - 0.5f*(numScenes * maxWidth);	
			curY = height_ - maxHeight;		
		break;
	case Left: 
		curX = 0;	
		curY = 0.5f*height_ - 0.5f*(numScenes * maxHeight);	
		break;
	case Right: 
		curX = width_ - maxWidth;
		curY = 0.5f*height_ - 0.5f*(numScenes * maxHeight);
		break;
			}	

	int pageIndex = pageIndex_;
	if (pageIndex < 0) pageIndex = 0;
	for (int i = 0; i < numScenes; ++i)
		{
		float x = curX;
			float w = maxWidth;
			float y = curY;
			float h = maxHeight;

			if (x < pt.x && pt.x < x + w && y < pt.y && pt.y < y + h)
			{
				return i;
			}	

		curX += dX;
		curY += dY;
	}

	return -1;
}

bool ImageChanger::clickEvent(const Vector2 & pos, int pressId)
{
	const Camera * cam = parentScene_->camera();
	Ray mouseRay = cam->unproject(parentScene_->screenToDeviceCoords(pos));
	Vector3 pt;
	intersectRect(&pt, mouseRay);
	transMatrix_ = parentTransform() * visualAttrib_.transformMatrix();
	invTransMatrix_ = transMatrix_.inverse();
	pt = invTransMatrix_ * pt;

	if (showMarkers_ && allowDrag_)
	{
		int index = cursorOnWhichMarker(pt);
		if (index >= 0)
		{
			imageChangeTo(index);					
			return true;
		}
	}
	
	return SceneObject::clickEvent(pos, pressId);
}

bool ImageChanger::moveEvent(const Vector2 & screenPos, int pressId)
{	
	PressEventData & pressEvent = pressEvents_[pressId];	
	if (pressEvent.pressed == PressEventData::StrayPress) return true;
	if (pressEvent.pressed == PressEventData::NotPressed) return false;
	if (pressEvent.pressed == PressEventData::PassedToParent) return false;
	if (!allowDrag_) return false;
	if (pageMovingId_ == -1)
		pageMovingId_ = pressId;
	else if (pageMovingId_ != pressId) return false;

	const Camera * cam = parentScene_->camera();
	Ray mouseRay = cam->unproject(parentScene_->screenToDeviceCoords(screenPos));
	Vector3 curPt;
	intersectRect(&curPt, mouseRay);
	curPt = invTransMatrix_ * curPt; 

	float dragXDelta = curPt.x - pressEvent.pressPt.x;
	float dragYDelta = curPt.y - pressEvent.pressPt.y;
	bool verticalDragged = fabs(dragXDelta) < fabs(dragYDelta);		
	bool horzDragged = !verticalDragged;

	bool wrongDirection;
	if (draggingOnMarkers_)
	{
		wrongDirection =
			(verticalDragged && (markerLocation_ == Top || markerLocation_ == Bottom)) 
			||
			(horzDragged && (markerLocation_ == Left || markerLocation_ == Right));
	}
	else
	{
		wrongDirection = 
			(verticalDragged && scrollDirection_ == Horizontal) 
			||
			(horzDragged && scrollDirection_ == Vertical);
	}		

	if (!pressEvent.startedMoving) 
	{
		if (wrongDirection)
		{
			//must pass on event data to parents upstream
			pressEvent.pressed = PressEventData::PassedToParent;
			return false;			
		}

		pressEvent.startMovingTime = Global::currentTime();
		pressEvent.startPt = curPt;
		pressEvent.startedMoving = true;
	}
	
	if (!draggingOnMarkers_)
	{
		//if at the end of scrollable region, pass move event to parent
		//to process potential page change
		float curPagePos, dragDelta;
		float dx = 0, dy = 0;

		if (scrollDirection_ == Horizontal)
		{
			curPagePos = subscene_->transform().translation().x;
			dragDelta = dragXDelta;
			dx = curPt.x - pressEvent.startPt.x;
		}
		else
		{
			curPagePos = subscene_->transform().translation().y;
			dragDelta = dragYDelta;
			dy = curPt.y - pressEvent.startPt.y;
		}


		bool passOn = false;
		
		if (pageIndex_ == 0 && curPagePos == 0 && dragDelta > 0 && !wrongDirection)	
			passOn = true;

		if (pageIndex_ == (int)fileNames_.size() - 1 && dragDelta < 0 && !wrongDirection)		
			passOn = true;		

		if (passOn)
		{
			pressEvent.pressed = PressEventData::PassedToParent;
			return false;
		}	

		//change page position according to drag
		
		setCurPagePos(startPagePos_ + Vector3(dx, dy, 0));
		prevAutoTransitionTime_ = FLT_MAX;
	}
	else
	{
		float pos = getCursorRangePos(curPt);
		setRangePos(pos);
	}
	
	resetPageTween();		
	redrawTriggered_ = true;
	return true;
}

bool ImageChanger::releaseEvent(const Vector2 & screenPos, int pressId)
{
	PressEventData & pressEvent = pressEvents_[pressId];

	if (pressEvent.pressed == PressEventData::PassedToParent) return false;	
	if (pressEvent.pressed == PressEventData::NotPressed) return false;
	if (pressEvent.pressed == PressEventData::StrayPress) return true;

	if (pressId == pageMovingId_)
	{
		const Camera * cam = parentScene_->camera();
		Ray mouseRay = cam->unproject(parentScene_->screenToDeviceCoords(screenPos));
		Vector3 pt;
		intersectRect(&pt, mouseRay);
		invTransMatrix_ = 
			(parentTransform() * visualAttrib_.transformMatrix()).inverse();
		pt = invTransMatrix_ * pt;	

		pageMovingId_ = -1;
		if (hasPageBeenMoved()) doPageSnapBack(pressEvent, pt);
	}

	redrawTriggered_ = true;	
	return true;
}

bool ImageChanger::update(float sceneTime)
{
	bool needRedraw = false;
	needRedraw |= SceneObject::update(sceneTime);	
	needRedraw |= subscene_->update(sceneTime);	

	if (redrawTriggered_)
	{
		redrawTriggered_ = false;
		needRedraw = true;
	}
	
	if (isPageTweenHappening())
	{			
		float t = (sceneTime - pageTweenStartTime_) / pageTweenDuration_;		
		if (t > 1) t = 1;		
		t = 1 - (t - 1)*(t - 1);

		Vector3 pos = 
			pageTweenStartPos_ + t*(pageTweenEndPos_ - pageTweenStartPos_);

		setCurPagePos(pos);		

		prevAutoTransitionTime_ = FLT_MAX;
		if (pageTweenLastT_ == 1)
		{
			std::string prevFileName = subscene_->fileName();
			resetPageTween();
			setCurPage(targetPageIndex_);
			prevAutoTransitionTime_ = sceneTime;
		}

		pageTweenLastT_ = t;
		needRedraw = true;	
	}

	if (autoTransitionTime_ > 0.0f && sceneTime > prevAutoTransitionTime_ + autoTransitionTime_)
	{
		imageChangeTo((pageIndex_ + 1)%(int)fileNames_.size(), TransitionDirectionPlus);
	}

	return needRedraw;
}

bool ImageChanger::asyncLoadUpdate() 
{
	bool everythingLoaded = SceneObject::asyncLoadUpdate();
	bool imgLoaded;
	Transform trans;

	//Commenting out. Want to report as loaded, even if ImageChanger child 
	//images aren't all loaded. This is because, the ImageChanger object itself
	//will display a loading page, so the parent document template doesn't 
	//have to.

	if (!subscene_->fileName().empty())
	{
		imgLoaded = subscene_->asyncLoadUpdate();	
		if (imgLoaded)
		{
			trans = subscene_->transform();
			trans.setScaling(width_/subscene_->width(), height_/subscene_->height(), 1);
			subscene_->setTransform(trans);
		}
		//everythingLoaded &= imgLoaded;
	}

	if (!subscenePrev_->fileName().empty())
	{
		imgLoaded = subscenePrev_->asyncLoadUpdate();	
		if (imgLoaded)
		{
			trans = subscenePrev_->transform();
			trans.setScaling(width_/subscenePrev_->width(), height_/subscenePrev_->height(), 1);
			subscenePrev_->setTransform(trans);
		}
		//everythingLoaded &= imgLoaded;
	}

	if (!subsceneNext_->fileName().empty())
	{
		imgLoaded = subsceneNext_->asyncLoadUpdate();	
		if (imgLoaded)
		{
			trans = subsceneNext_->transform();
			trans.setScaling(width_/subsceneNext_->width(), height_/subsceneNext_->height(), 1);
			subsceneNext_->setTransform(trans);
		}
		//everythingLoaded &= imgLoaded;
	}

	if (activePageImg_)
	{
	everythingLoaded &= activePageImg_->asyncLoadUpdate();
	everythingLoaded &= inactivePageImg_->asyncLoadUpdate();
	}

	if (!everythingLoaded) redrawTriggered_ = true;
	return everythingLoaded;
}

bool ImageChanger::isLoaded() const
{
	//bug fix, want to allow running the scene (including querying loaded
	//status) without having called init
	if (!gl_) return false;

	return 
		subscene_->isLoaded() && 
		subscenePrev_->isLoaded() && 
		subsceneNext_->isLoaded() &&
		activePageImg_->isLoaded() &&
		inactivePageImg_->isLoaded();
}

void ImageChanger::drawMarkers(GfxRenderer * gl) const
{
	gl->useTextureProgram();
	gl->setTextureProgramOpacity(1);

	int numScenes = (int)fileNames_.size();	
	
	float maxWidth = (float)activePageImg_->width();
	if (inactivePageImg_->width() > maxWidth)
		maxWidth = (float)inactivePageImg_->width();
	float maxHeight = (float)activePageImg_->height();
	if (inactivePageImg_->height() > maxHeight) 
		maxHeight = (float)inactivePageImg_->height();

	float dX = 0;
	float dY = 0;
	float curX, curY;
	
	if (markerLocation_ == Top || markerLocation_ == Bottom)		
		dX = maxWidth;	
	else	
		dY = maxHeight;	
	
	switch (markerLocation_)
	{
	case Top: 
		curX = 0.5f*width_ - 0.5f*(numScenes * maxWidth);	
		curY = 0;	
		break;
	case Bottom: 
		curX = 0.5f*width_ - 0.5f*(numScenes * maxWidth);	
		curY = height_ - maxHeight;	
		break;
	case Left: 
		curX = 0;	
		curY = 0.5f*height_ - 0.5f*(numScenes * maxHeight);	
		break;
	case Right: 
		curX = width_ - maxWidth;
		curY = 0.5f*height_ - 0.5f*(numScenes * maxHeight);
		break;
	}
	
	int pageIndex = pageIndex_;
	if (pageIndex < 0) pageIndex = 0;
	for (int i = 0; i < numScenes; ++i)
	{
		if (i == pageIndex) 
		{
			gl->use(activePageImg_.get());
			gl->drawRect(curX, curY,
				activePageImg_->width(), activePageImg_->height());
		}
		else 
		{
			gl->use(inactivePageImg_.get());
			gl->drawRect(curX, curY,
				(float)inactivePageImg_->width(), (float)inactivePageImg_->height());
		}
		curX += dX;
		curY += dY;		
	}
		}
		
void ImageChanger::setImage(int index)
{
	if (index < 0 || fileNames_.size() <= index) return;
	targetPageIndex_ = index;
	setCurPage(targetPageIndex_);
}

int ImageChanger::prevImageIndex() const
{
	if (pageIndex_ <= 0) return -1;
	return pageIndex_ - 1;
}

int ImageChanger::nextImageIndex() const
{
	if (pageIndex_ >= fileNames_.size() - 1) return -1;
	return pageIndex_ + 1;
}

void ImageChanger::imageChangeTo(int index, TransitionDirection dir)
{
	if (index < 0 || fileNames_.size() <= index) return;

	pageTweenStartTime_ = Global::currentTime();
	pageTweenDuration_ = 0.3f;
	pageTweenLastT_ = 0;
	pageTweenStartPos_ = subscene_->transform().translation();
	pageTweenEndPos_ = Vector3(0, 0, 0);

	targetPageIndex_ = index;
	if (targetPageIndex_ == pageIndex_) return;

	Document * doc = parentScene_->parentDocument();
	

	float maxX = 0, maxY = 0;
	if (scrollDirection_ == Horizontal) maxX = width_;
	else maxY = height_;

	if (dir == TransitionDirectionAuto)
	{
		if (targetPageIndex_ < pageIndex_)		
			dir = TransitionDirectionMinus;
		else
			dir = TransitionDirectionPlus;
	}

	if (dir == TransitionDirectionMinus)
	{
		pageTweenEndPos_ = Vector3(maxX, maxY, 0);
		if (fileNames_[targetPageIndex_] != subscenePrev_->fileName())
		{
			subscenePrev_->setFileName(fileNames_[targetPageIndex_]);
			subscenePrev_->init(doc->renderer());
		}
	}
	else if (dir == TransitionDirectionPlus)
	{
		pageTweenEndPos_ = Vector3(-maxX, -maxY, 0);		
		if (fileNames_[targetPageIndex_] != subsceneNext_->fileName())
		{
			subsceneNext_->setFileName(fileNames_[targetPageIndex_]);
			subsceneNext_->init(doc->renderer());
		}
	}
	redrawTriggered_ = true;

}

void ImageChanger::drawObject(GfxRenderer * gl) const
{		
	gl->beginIncStencilDrawing();
	gl->useColorProgram();
	gl->setColorProgramColor(0,0,0,0);
	gl->drawRect(0, 0, width_, height_);
	gl->endStencilDrawing();
	
	Vector3 pos = subscene_->transform().translation();

	if (subscene_->isLoaded()) subscene_->draw(gl);
	else loadingIcon_->draw(gl, pos.x, pos.y);

	float posCoord;
	float maxPosCoord;
	float maxPosX = 0;
	float maxPosY = 0;
	if (scrollDirection_ == Horizontal)
	{
		posCoord = subscenePrev_->transform().translation().x;
		maxPosCoord = width_;
		maxPosX = width_;
	}
	else
	{
		posCoord = subscenePrev_->transform().translation().y;
		maxPosCoord = height_;
		maxPosY = height_;
	}

	if (posCoord > -maxPosCoord)
	{
		if (subscenePrev_->isLoaded()) subscenePrev_->draw(gl);
		else loadingIcon_->draw(gl, pos.x - maxPosX, pos.y - maxPosY);
	}
	else if (posCoord < maxPosCoord)
	{
		if (subsceneNext_->isLoaded()) 
			subsceneNext_->draw(gl);
		else 
			loadingIcon_->draw(gl, pos.x + maxPosX, pos.y + maxPosY);
	}

	bool showMarkers = showMarkers_ &&
		activePageImg_->isLoaded() && inactivePageImg_->isLoaded();

	if (showMarkers) drawMarkers(gl);

	gl->beginDecStencilDrawing();
	gl->useColorProgram();
	gl->setColorProgramColor(0,0,0,0);
	gl->drawRect(0, 0, width_, height_);
	gl->endStencilDrawing();
}

void ImageChanger::start(float docTime)
{
	SceneObject::start(docTime);	
	subscene_->start(docTime);	
}

void ImageChanger::preStart(float docTime)
{
	SceneObject::preStart(docTime);
	subscene_->preStart(docTime);
}

BoundingBox ImageChanger::extents() const
{
	return BoundingBox(Vector3(0, 0, 0), Vector3(width_, height_, 0));
}


SceneObject * ImageChanger::intersect(Vector3 * intPt, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;
	if (intersectRect(intPt, ray)) return this;
	else return 0;
}

bool ImageChanger::intersectRect(Vector3 * out, const Ray & ray)
{
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();
	
	Vector3 a = trans * Vector3(0, 0, 0);
	Vector3 b = trans * Vector3(0, height(), 0);
	Vector3 c = trans * Vector3(width(), height(), 0);
	Vector3 d = trans * Vector3(width(), 0, 0);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return true;
	}
	else return false;
}

void ImageChanger::remapReferences(const ElementMapping & elementMapping)
{
	SceneObject::remapReferences(elementMapping);
}


void ImageChanger::referencedFiles(std::vector<std::string> * refFiles) const
{	
	SceneObject::referencedFiles(refFiles);
	if (showMarkers_)
	{
		refFiles->push_back(activeMarkerFileName_);
		refFiles->push_back(inactiveMarkerFileName_);
	}

	BOOST_FOREACH(std::string fileName, fileNames_)
		refFiles->push_back(fileName);
}

int ImageChanger::setReferencedFiles(
	const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);
	if (showMarkers_)
	{
		activeMarkerFileName_ = refFiles[index++];
		inactiveMarkerFileName_ = refFiles[index++];
	}

	BOOST_FOREACH(std::string & fileName, fileNames_)
		fileName = refFiles[index++];

	return index;
}

bool ImageChanger::dependsOn(Scene * scene) const
{
	return false;
}


void ImageChanger::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	writer.write(width_, "width");
	writer.write(height_, "height");

	writer.write(fileNames_, "fileNames");


	writer.write(showMarkers_, "showMarkers");
	writer.write(activeMarkerFileName_, "activeMarker");
	writer.write(inactiveMarkerFileName_, "inactiveMarker");
	writer.write(allowDrag_, "allowDrag");

	writer.write(format_);
	
	writer.write(scrollDirection_);
	writer.write(markerLocation_);	
	writer.write(markerOffsetX_);
	writer.write(markerOffsetY_);

	writer.write(autoTransitionTime_);
}

void ImageChanger::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	reader.read(width_);
	reader.read(height_);
	reader.read(fileNames_);

	if (version >= 1) reader.read(showMarkers_);	
	reader.read(activeMarkerFileName_);
	reader.read(inactiveMarkerFileName_);
	if (version < 1) showMarkers_ = !activeMarkerFileName_.empty();
	
	reader.read(allowDrag_);

	if (version >= 2)
	{
		reader.read(format_);
	}
	
	if (version >= 3)
	{
		reader.read(scrollDirection_);
		reader.read(markerLocation_);
		reader.read(markerOffsetX_);
		reader.read(markerOffsetY_);
	}

	if (version >= 4)
	{
		reader.read(autoTransitionTime_);
	}

	setInitPage();
}

void ImageChanger::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	w.writeTag("Width", width_);
	w.writeTag("Height", height_);
	for (unsigned int i = 0; i < (unsigned int)fileNames_.size(); ++i)
		w.writeTag("FileName", fileNames_[i]);
	
	w.writeTag("ShowMarkers", showMarkers_);
	w.writeTag("ActiveMarker", activeMarkerFileName_);
	w.writeTag("InactiveMarker", inactiveMarkerFileName_);
	w.writeTag("AllowDrag", allowDrag_);

	int temp = format_;
	w.writeTag("Format", temp);

	temp = scrollDirection_;
	w.writeTag("ScrollDirection", temp);

	temp = markerLocation_;
	w.writeTag("MarkerLocation", temp);
	w.writeTag("MarkerOffsetX", markerOffsetX_);
	w.writeTag("MarkerOffsetY", markerOffsetY_);
	w.writeTag("AutoTransitionTime", autoTransitionTime_);
}

void ImageChanger::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);
	int temp = 0;
	std::string filename;

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;		
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(width_, curNode, "Width"));
		else if (r.getNodeContentIfName(height_, curNode, "Height"));
		else if (r.getNodeContentIfName(filename, curNode, "FileName")) fileNames_.push_back(filename);
		else if (r.getNodeContentIfName(showMarkers_, curNode, "ShowMarkers"));
		else if (r.getNodeContentIfName(activeMarkerFileName_, curNode, "ActiveMarker"));
		else if (r.getNodeContentIfName(inactiveMarkerFileName_, curNode, "InactiveMarker"));
		else if (r.getNodeContentIfName(allowDrag_, curNode, "AllowDrag"));
		else if (r.getNodeContentIfName(temp, curNode, "Format")) format_ = (Texture::Format)temp;
		else if (r.getNodeContentIfName(temp, curNode, "ScrollDirection")) scrollDirection_ = (ScrollDirection)temp;
		else if (r.getNodeContentIfName(temp, curNode, "MarkerLocation")) markerLocation_ = (MarkerLocation)temp;
		else if (r.getNodeContentIfName(markerOffsetX_, curNode, "MarkerOffsetX"));
		else if (r.getNodeContentIfName(markerOffsetY_, curNode, "MarkerOffsetY"));
	}
}

void ImageChanger::requestLocalSaveObject()
{
	BOOST_FOREACH(std::string & str, fileNames_)
	{
		Image img;
		img.setFileName(str);
		img.requestLocalSaveObject();
	}
	Image img;
	img.setFileName(activeMarkerFileName_);
	img.requestLocalSaveObject();
	img.setFileName(inactiveMarkerFileName_);
	img.requestLocalSaveObject();
}