#pragma once
#include "Types.h"
#include "MathStuff.h"
#include "BoundingBox.h"
#include <boost/uuid/uuid.hpp>
#include "AppObject.h"
class GfxRenderer;

///////////////////////////////////////////////////////////////////////////////
class VisualAttrib;
class EventListener;
class Event;
class Writer;
class Reader;
class TempRenderObject;
class ElementMapping;
class PressEventData;
class Document;
class Scene;
class XmlWriter;
class XmlReader;

class ContainerObjectsComparer
{
public:
	bool operator() (const ContainerObjects & lhs, const ContainerObjects & rhs) const;
};


class DocumentTemplate
{
	friend class Scene;
	friend class Document;
public:
	static JSObject * createBaseScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);


	DocumentTemplate();
	virtual DocumentTemplate * newInstance() const = 0;
	
	virtual const char * typeStr() const = 0;
	virtual boost::uuids::uuid type() const = 0;

	virtual void setDocument(Document * document) {document_ = document;}

	virtual ~DocumentTemplate() {}

	/**
	@return whether it is allowable for the specified scene to influence the
	app/viewer layer. For example, in the Magazine template, a scene just
	to the right of the current scene may have start(...) called on it. However,
	if there is a PlayVideoAction connected to the SceneStart event, that action
	should not happen, since that scene is offscreen.

	*/
	virtual bool isCurrentScene(Scene * scene) const;

	virtual void init(GfxRenderer * gl) = 0;
	virtual void uninit() = 0;
	
	virtual void draw(GfxRenderer * gl) const = 0;

	virtual void reInitCurScene(GfxRenderer * gl) = 0;

	virtual Scene * curScene() const = 0;
	virtual Scene * prevScene() const = 0;
	virtual Scene * nextScene() const = 0;	
	virtual void getScenes(std::vector<Scene *> & scenes) const = 0;
	virtual void sceneChangeTo(Scene * scene) = 0;
	virtual void sceneSet(Scene * scene) = 0;
	
	const AppObjectOffset & baseAppObjectOffset() const {return baseAppObjectOffset_;}
	AppObjectOffset totalAppObjectOffset(const ContainerObjects & containerObjects) const;

	AppObjectOffset totalTextFeaturesOffset(const ContainerObjects & containerObjects) const;

	void setBaseAppObjectOffset(float x, float y, float sx, float sy,
		float clipX, float clipY, float clipWidth, float clipHeight)
	{
		baseAppObjectOffset_.dx = x;
		baseAppObjectOffset_.dy = y;
		baseAppObjectOffset_.sx = sx;
		baseAppObjectOffset_.sy = sy;
		baseAppObjectOffset_.clip = true;
		baseAppObjectOffset_.clipX = clipX;
		baseAppObjectOffset_.clipY = clipY;
		baseAppObjectOffset_.clipWidth = clipWidth;
		baseAppObjectOffset_.clipHeight = clipHeight;
	}

	/**
	@return whether the event was handled or not.
	*/
	virtual bool clickEvent(const Vector2 & mousePos, int pressId) = 0;
	virtual bool doubleClickEvent(const Vector2 & mousePos, int pressId) = 0;
	virtual bool pressEvent(const Vector2 & startPos, int pressId, bool propagate) = 0;
	virtual bool releaseEvent(const Vector2 & pos, int pressId) = 0;
	virtual bool moveEvent(const Vector2 & pos, int pressId) = 0;	

	virtual bool keyPressEvent(int keyCode) = 0;
	virtual bool keyReleaseEvent(int keyCode) = 0;

	virtual void appObjectTapEvent(AppObject * appObject) {}

	virtual void setSize(int width, int height, bool resizeGl = true) = 0;	
	void setDpiScale(float dpiScale)
	{
		dpiScale_ = dpiScale;
	}
	float dpiScale() {return dpiScale_;}
	
	virtual void start(float docTime) = 0;
	virtual bool update(float sceneTime) = 0;
	virtual bool asyncLoadUpdate() = 0;
	virtual void stop() = 0;

	/**
	@return the immediate scene, contained in the template, that depends on
	the scene passed as the parameter
	*/
	virtual Scene * dependsOn(Scene * scene) const = 0;
	
	virtual void pageScenes(std::vector<Scene *> * scenes) const = 0;
	

	/**
	refFiles is added onto, and is not cleared.
	*/
	virtual void referencedFiles(std::vector<std::string> * refFiles) const = 0;
	/**
	@return end index (= to next object's start index)
	*/
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex) = 0;

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const {}
	virtual void read(Reader & reader, unsigned char) {}

	virtual const char * xmlTag() const {return "";}
	virtual void writeXml(XmlWriter & w) const {}
	virtual void readXml(XmlReader & r, xmlNode * node) {}

	
	// 단말기에서 북마크를 제어할 경우
	virtual bool setCurPageBookmark() {return false;}
	virtual void getBookmarkPages(std::vector<int> * pages) {}

	virtual void goToPage(const int & index) {}
	virtual int pageIndex() {return 0;}

	// Landscape
	virtual void changeLandscape(const bool & var) {}

	void setPreDownload(const bool & var) {isPreDownload_ = var;}
	bool isPreDownload() {return isPreDownload_;}

private:

	DocumentTemplate(const DocumentTemplate & rhs);
	DocumentTemplate & operator = (const DocumentTemplate & rhs);

protected:

	AppObjectOffset baseAppObjectOffset_;	


	float dpiScale_;

	Document * document_;
	JSObject * scriptObject_;

	// predownload All Scenes
	bool isPreDownload_;
};
