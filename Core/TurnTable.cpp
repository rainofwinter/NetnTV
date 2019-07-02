#include "stdafx.h"
#include "TurnTable.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "PageFlip.h"
#include "FileUtils.h"
#include "Scene.h"
#include "Camera.h"
#include "Document.h"
#include "ScriptProcessor.h"
#include "Global.h"
using namespace std;


JSClass jsTurnTableClass = InitClass(
	"TurnTable", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);


JSBool TurnTable_getFrameIndex(JSContext *cx, uintN argc, jsval *vp)
{
	TurnTable * thisObj = (TurnTable *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));	
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(thisObj->frameIndex()));
	return JS_TRUE;
};

JSBool TurnTable_setFrameIndex(JSContext *cx, uintN argc, jsval *vp)
{
	
	jsval * argv = JS_ARGV(cx, vp);
	TurnTable * thisObj = (TurnTable *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	int32 jsInt;
	if (!JS_ConvertArguments(cx, argc, argv, "i", &jsInt)) return JS_FALSE;	
	thisObj->setFrameIndex(jsInt);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;	
};

JSFunctionSpec TurnTableFuncs[] = {
	JS_FS("getFrameIndex", TurnTable_getFrameIndex, 0, 0),
	JS_FS("setFrameIndex", TurnTable_setFrameIndex, 1, 0),	
	JS_FS_END
};

///////////////////////////////////////////////////////////////////////////////
JSObject * TurnTable::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsTurnTableClass,
		0, 0, 0, TurnTableFuncs, 0, 0);

	return proto;
}

JSObject * TurnTable::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsTurnTableClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void TurnTable::create()
{
	texture_ = new Texture;
	textureBack_ = new Texture;
	redrawTriggered_ = false;

	curImgIndex_ = 0;
	speed_ = 5;
}

TurnTable::TurnTable()
{
	create();
	setId("TurnTable");
}

TurnTable::TurnTable(const TurnTable & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	imgFileNames_ = rhs.imgFileNames_;	
	visualAttrib_ = rhs.visualAttrib_;
	speed_ = rhs.speed_;
}




TurnTable::~TurnTable()
{		
	uninit();
	delete texture_;
	delete textureBack_;
}

void TurnTable::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);

	BOOST_FOREACH(string imgFileName, imgFileNames_)
		refFiles->push_back(imgFileName);	
}

int TurnTable::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(
		baseDirectory, refFiles, index);
	BOOST_FOREACH(string & imgFileName, imgFileNames_)
	{
		imgFileName = refFiles[index++];
	}
	
	return index;
}


void TurnTable::setImgFileNames(const vector<string> & fileNames)
{
	imgFileNames_ = fileNames;	
}

void TurnTable::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
	Global::instance().getImageDims(imgFileNames_[curImgIndex_], &width_, &height_);
	textureBack_->init(gl, imgFileNames_[curImgIndex_], Texture::UncompressedRgba32);	

	pressEvent_.pressed = PressEventData::NotPressed;
}

void TurnTable::uninit()
{	
	texture_->uninit();
	textureBack_->uninit();
	SceneObject::uninit();
}
void TurnTable::drawObject(GfxRenderer * gl) const
{

	float fOpacity = totalOpacity();
	if (fOpacity == 0) return;

	if (!textureBack_->fileName().empty() && textureBack_->isLoaded())
	{
		swap(texture_, textureBack_);
		textureBack_->setFileName("");
		textureBack_->uninit();
	}

	if (texture_->needSeparateAlpha())
	{
		gl->useTextureAlphaProgram();
		gl->setTextureAlphaProgramOpacity(fOpacity);		
	}
	else
	{
		gl->useTextureProgram();
		gl->setTextureProgramOpacity(fOpacity);				
	}	

	gl->use(texture_);
	gl->drawRect(0, 0, (float)texture_->width(), (float)texture_->height());
}


BoundingBox TurnTable::extents() const
{
	return BoundingBox(
		Vector3(0, 0, 0), 
		Vector3((float)width_, (float)height_, 0));
}

void TurnTable::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	writer.write(imgFileNames_, "imgFileNames");
	writer.write(speed_);
	
}

void TurnTable::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	reader.read(imgFileNames_);
	if (version >= 1)
	{
		reader.read(speed_);
	}
}

void TurnTable::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	for (unsigned int i = 0; i < (unsigned int)imgFileNames_.size(); ++i)
		w.writeTag("ImgFileName", imgFileNames_[i]);
	w.writeTag("Speed", speed_);
}

void TurnTable::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);
	std::string filename;

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;		
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(filename, curNode, "ImgFileName")) imgFileNames_.push_back(filename);
		else if (r.getNodeContentIfName(speed_, curNode, "Speed"));
	}
}


int TurnTable::width() const
{
	return width_;
}

int TurnTable::height() const
{
	return height_;
}

SceneObject * TurnTable::intersect(Vector3 * out, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();


	Vector3 a = trans * Vector3(0, 0, 0);
	Vector3 b = trans * Vector3(0, (float)height_, 0);
	Vector3 c = trans * Vector3((float)width_, (float)height_, 0);
	Vector3 d = trans * Vector3((float)width_, 0, 0);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return this;
	}
	else return 0;
}

Vector2 TurnTable::localCoords(
	const Vector2 & pos, const Matrix & invTransMatrix)
{
	Ray mouseRay = parentScene_->camera()->unproject(
		parentScene_->screenToDeviceCoords(pos));
	Vector3 pt;
	intersect(&pt, mouseRay);	
	pt = invTransMatrix * pt;
	return Vector2(pt.x, pt.y);
}

bool TurnTable::pressEvent(const Vector2 & startPos, int pressId)
{
	if (!texture_->isLoaded()) return false;
	pressEvent_.pressed = PressEventData::Pressed;
	pressId_ = pressId;
	pressEvent_.startedMoving = false;
	
	invTransMatrix_ = 
		(parentTransform() * visualAttrib_.transformMatrix()).inverse();

	pressEvent_.pressPt = localCoords(startPos, invTransMatrix_);
	pressImgIndex_ = curImgIndex_;
	return true;
	
}

bool TurnTable::moveEvent(const Vector2 & pos, int pressId)
{
	if (pressId != pressId_) return false;
	if (pressEvent_.pressed == PressEventData::PassedToParent) return false;

	Vector2 movePos = localCoords(pos, invTransMatrix_);

	if (!pressEvent_.startedMoving) 
	{
		if (fabs(movePos.x - pressEvent_.pressPt.x) <
			fabs(movePos.y - pressEvent_.pressPt.y))
		{
			//vertical scrolling
			//must pass on event data to parents upstream
			pressEvent_.pressed = PressEventData::PassedToParent;
			return false;			
		}
		pressEvent_.startPt = movePos;
		pressEvent_.startedMoving = true;
	}

	
	float xDelta = movePos.x - pressEvent_.startPt.x;

	int newImgIndex = (int)(pressImgIndex_ + speed_ / 100 * xDelta);
	int numImgs = (int)imgFileNames_.size();
	while (newImgIndex >= numImgs)	newImgIndex -= numImgs;
	while(newImgIndex < 0) newImgIndex += numImgs;

	if (newImgIndex != curImgIndex_)
	{
		curImgIndex_ = newImgIndex;		
		redrawTriggered_ = true;
	}
	return true;
}

void TurnTable::setFrameIndex(int frameIndex)
{
	int oldImgIndex = curImgIndex_;

	int delta = frameIndex - curImgIndex_;
	curImgIndex_ += delta;
	pressImgIndex_ += delta;
	
	int numImgs = (int)imgFileNames_.size();

	if (curImgIndex_ < 0)
	{
		curImgIndex_ -= ((curImgIndex_ + 1 - numImgs) / numImgs)*numImgs;
	}
	curImgIndex_ = curImgIndex_ % numImgs;

	if (pressImgIndex_ < 0)
	{
		pressImgIndex_ -= ((pressImgIndex_ + 1 - numImgs) / numImgs)*numImgs;
	}
	pressImgIndex_ = pressImgIndex_ % numImgs;
	redrawTriggered_ = true;
}

bool TurnTable::releaseEvent(const Vector2 & pos, int pressId)
{
	if (pressId != pressId_) return false;
	if (pressEvent_.pressed == PressEventData::PassedToParent) return false;
	pressEvent_.pressed = PressEventData::NotPressed;
	return true;
}

bool TurnTable::update(float sceneTime)
{
	bool needRedraw = false;
	needRedraw |= SceneObject::update(sceneTime);
	if (redrawTriggered_)
	{
		redrawTriggered_ = false;
		needRedraw = true;
	}

	if (texture_->fileName() != imgFileNames_[curImgIndex_])
	{
		Document * doc = parentScene_->parentDocument();
		if (textureBack_->fileName().empty())
			textureBack_->init(doc->renderer(), imgFileNames_[curImgIndex_], Texture::UncompressedRgba32);				
	}

	needRedraw |= !texture_->isFullyLoaded();
	return needRedraw;
}

bool TurnTable::asyncLoadUpdate()
{	
	bool loaded = texture_->asyncLoadUpdate();
	loaded &= textureBack_->asyncLoadUpdate();
	return loaded;
}
