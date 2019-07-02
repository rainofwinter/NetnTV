#pragma once
#include "SceneObject.h"
#include "Color.h"
#include "Attrib.h"
#include "PressEventData.h"
#include "Tween.h"
#include "DragVelocityHandler.h"
#include "SceneReferencer.h"
///////////////////////////////////////////////////////////////////////////////
class Mesh;
class Material;
class ElementMapping;
class TextureRenderer;
class Texture;
class ScriptProcessor;
struct JSObject;

class Subscene : public SceneObject, public SceneReferencer
{
public:
	enum Mode
	{
		ModeFixed,
		ModeScrollable,
		ModeDraggable
	};

	enum DragSnapState
	{
		SnapInitial,
		SnapUp,
		SnapLeft,
		SnapDown,
		SnapRight
	};

	enum CameraMode
	{
		ChildScene, 
		ParentScene
	};

public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "Subscene";
	}
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("911CADF2-4CD7-2799-BADE-51D5377987AF");
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new Subscene(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & elementMapping);

	Subscene();
	Subscene(const Subscene & rhs, ElementMapping * elementMapping);
	~Subscene();

	virtual void drawObject(GfxRenderer * gl) const;
	void drawBg(GfxRenderer * gl) const;

	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();
	virtual bool isInit() {return SceneObject::isInit();}

	/**
	In scroll or drag modes, set whether the scroll position should be locked
	(whether the contents should be kept stationary)
	*/
	void setLocked(bool locked) {locked_ = locked;}

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;	

	Scene * const & scene() const {return scene_;}
	virtual void setScene(Scene * const & scene);
	void setScene(SceneObject * containerObject, Scene * const & scene);
	void changeScene(Scene * const & scene);

	const float & windowWidth() const {return windowWidth_;}
	const float & windowHeight() const {return windowHeight_;}

	void setWindowWidth(const float & width);
	void setWindowHeight(const float & height);

	const float & maxPlusDrag() const {return maxPlusDrag_;}
	void setMaxPlusDrag(const float & maxUpDrag) {maxPlusDrag_ = maxUpDrag;}

	const float & maxMinusDrag() const {return maxMinusDrag_;}
	void setMaxMinusDrag(const float & maxDownDrag) {maxMinusDrag_ = maxDownDrag;}

	const float & maxTopDrag() const {return maxTopDrag_;}
	void setMaxTopDrag(const float & maxTopdrag) {maxTopDrag_ = maxTopdrag;}

	const float & maxBotDrag() const {return maxBotDrag_;}
	void setMaxBotDrag(const float & maxBotdrag) {maxBotDrag_ = maxBotdrag;}

	const bool & transparentBg() const {return transparentBg_;}
	void setTransparentBg(const bool & transparentBg) 
	{transparentBg_ = transparentBg;}

	void setScrollPosition(const Vector2 & pos);
	void setDragPosition(const Vector2 & pos);

	const Vector2 & scrollPosition() const {return scenePos_;}
	const Vector2 & dragPosition() const {return dragScenePos_;}

	float sceneWidth() const;
	float sceneHeight() const;

	const Mode & mode() const
	{
		return mode_;
	}

	void setMode(const Mode & mode) 
	{
		mode_ = mode;
	}

	const CameraMode & cameraMode() const
	{
		return cameraMode_;
	}

	void setCameraMode(const CameraMode & cameraMode)
	{
		cameraMode_ = cameraMode;
	}

	void resetScrollTween()
	{
		scrollTweenDuration_ = 0;		
	}

	void setDragSnap(const bool & dragSnap) {dragSnap_ = dragSnap;}
	const bool & dragSnap() const {return dragSnap_;}

	void setDragVertical(const int & dragVertical) {dragVertical_ = dragVertical;}
	const int & dragVertical() const {return dragVertical_;}

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	virtual AppObjectOffset appObjectOffset() const;

	virtual bool clickEvent(const Vector2 & mousePos, int pressId);
	virtual bool doubleClickEvent(const Vector2 & mousePos, int pressId);
	virtual bool pressEvent(const Vector2 & startPos, int pressId);
	virtual bool releaseEvent(const Vector2 & pos, int pressId);
	virtual bool moveEvent(const Vector2 & pos, int pressId);

	virtual void start(float docTime);	
	virtual void preStart(float docTime);
	void start(float docTime, bool triggerEvent);

	virtual void initAppObjects();
	virtual void uninitAppObjects();

	virtual void stop();
	virtual bool update(float sceneTime);
	virtual bool asyncLoadUpdate();

	/**
	This function only returns true if the subscene's child scene is truly 
	fully loaded. This is different from checking the return value of 
	asyncLoadUpdate because asyncLoadUpdate returns true if the subscene object
	is loaded even if the child scene is not yet fully loaded.
	*/
	virtual bool isLoaded() const;

	virtual bool dependsOn(Scene * scene) const;
	virtual void dependsOnScenes(std::vector<Scene *> * dependsOnScenes, bool recursive = true) const;
	virtual bool isShowing(Scene * scene) const;
	virtual void showingScenes(std::set<Scene *> * showingScenes) const;

	const Vector2 & scrollPos() const {return scenePos_;}

	const bool & linkWithViewerLevelObjects() const {return linkWithViewerLevelObjects_;}
	void setLinkWithViewerLevelObjects(const bool & linkWithViewerLevelObjects)
	{
		linkWithViewerLevelObjects_ = linkWithViewerLevelObjects;
	}

	virtual unsigned char version() const {return 9;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "SubScene";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual void getSceneObjectsByID(const boost::uuids::uuid & type, std::vector<SceneObject *> * objs) const;
	
	//SceneChanger zoom
	void dragRelease(const Vector2 & localPos);

	void setHasLeft(const bool & val) {hasLeft_ = val;}
    void setHasRight(const bool & val) {hasRight_ = val;}
    void setHasTop(const bool & val) {hasTop_ = val;}
    void setHasBottom(const bool & val) {hasBottom_ = val;}

	const Vector3 operateTranslation(int id);

	bool isZoomable() const {return zoomFlag_;}
	bool isMultiTouchEnded() const;
    bool isTweenWorking() const;

	void resizeText(GfxRenderer * gl);

	void setNormalScaleX(float normalx) {normalScaleX_ = normalx;}
	const float normalScaleX() {return normalScaleX_;}
	void setNormalScaleY(float normaly) {normalScaleY_ = normaly;}
	const float normalScaleY() {return normalScaleY_;}

	const float curSceneMag() {return curMag_;}
	void setCurSceneMag(float val) {curMag_ = val;}

	virtual Vector2 convertToChildSceneCoords(const Vector2 & coords) const;
	virtual Vector2 convertFromChildSceneCoords(const Vector2 & coords) const;

	virtual bool showPoint(Vector2 & outCoords, Scene * scene, const Vector2 & coords);
	
	void reSize();

	virtual void requestLocalSaveObject();

	virtual AppObjectOffset textFeaturesOffset() const;
private:

	bool isValid() const;

	float subsceneWidth() const;
	float subsceneHeight() const;
	/**
	Convert to local coords
	*/
	Vector2 processCoords(const Vector2 & screenCoords) const;
	/**
	Convert local coords to local scene coords
	*/
	Vector2 toLocalScenePos(const Vector2 & localPos) const;
	Vector2 fromLocalScenePos(const Vector2 & localScenePos) const;

	float modeWidth() const;
	float modeHeight() const;

	Vector2 boundToWindow(const Vector2 & scenePos) const;
	Vector2 boundDragPos(const Vector2 & dragPos) const;
	bool isScrollable() const;
	bool isDraggable() const;
	bool intersectRect(Vector3 * out, const Ray & ray);
	void create();

	void endScrollTween();
	bool isScrollTweenHappening() const;
	void startScrollTween(
		Vector2 * target, const Vector2 & startPos, const Vector2 & endPos);


	void startDragTween(const DragSnapState & dragSnapState);

	Subscene(const Subscene & rhs);
	Subscene & operator = (const Subscene & rhs);
	/*
	void initTextureRendering(GfxRenderer * gl);
	void uninitTextureRendering();
	bool isInitTextureRendering() const;
	*/
	//void doAppObjectOffset();
	void dragRelease(const PressEventData & pressEvent, const Vector2 & localPos);

	
	//SceneChanger zoom
	bool checkParentHandled(Transform & t);
	void checkOutside(Transform & t);
	float pageEndLine(Transform & t);
	
private:
	Scene * scene_;		
	VisualAttrib visualAttrib_;

	Mode mode_;


	float windowWidth_, windowHeight_;

	float maxPlusDrag_;
	float maxMinusDrag_;
	float maxTopDrag_;
	float maxBotDrag_;
	int dragVertical_;


	//handle dragging
	Vector2 dragScenePos_;
	Vector2 pressDragScenePos_;
	bool dragSnap_;

	//whether user has just released pointer
	bool justReleased_;

	//scroll pos	
	Vector2 scenePos_;
	Vector2 pressScenePos_;

	enum MoveDirection{
		Horz,
		Vert,
		Undetermined
	} moveDirection_;

	int pageMovingId_;
	std::map<int, PressEventData> pressEvents_;
	//PressEventData pressEvent_;
	
	DragVelocityHandler dragVelHandler_;

	float scrollTweenStartTime_;
	float scrollTweenDuration_;
	Vector2 scrollTweenStartPos_;
	Vector2 scrollTweenEndPos_;
	Vector2 * scrollTweenTargetPos_;
	
	Tween<Vector2> dragTween_;

	bool transparentBg_;
	bool redrawTriggered_;

	bool locked_;

	bool linkWithViewerLevelObjects_;

	bool displayingAppObjects_;

	bool isLoaded_;

	CameraMode cameraMode_;

	
	//SceneChanger zoom
	// Zoom in, Zoom out
	bool zoomFlag_;
	bool isMultiTouched_;
    bool isTouched_;
	float defaultTouchLength_;
	Vector3 pivot_;
	Vector3 scale_;
    Vector3 trans_;
	int touchedID1_, touchedID2_;
	float normalScaleX_;
    float normalScaleY_;
    float multiTouchEndTime_;
	float maxZoomMagnification_;
    Tween<Vector3> zoomScaleTween_;
    Tween<Vector3> zoomTransTween_;
	Tween<float> yTransTween_;

	bool hasLeft_;
    bool hasRight_;
    bool hasTop_;
    bool hasBottom_;

	float curMag_;
	float pressMag_;

	bool dragZooming_;
};

