#pragma once
#include "SceneObject.h"
#include "SceneReferencer.h"
#include "Color.h"
#include "Attrib.h"
#include "PressEventData.h"
#include "Tween.h"
#include "DragVelocityHandler.h"
///////////////////////////////////////////////////////////////////////////////
//#define MAXZOOMMAGNIFICATION 2.0

#define DRAW_SCENE_BACKGROUND	//for tvstorm

class Mesh;
class Material;
class ElementMapping;
class TextureRenderer;
class Texture;
class ScriptProcessor;
struct JSObject;
class NetworkRequest;

/**
Based on Subscene class
*/
class InternalSubscene : public SceneReferencer
{
public:
	enum Mode
	{
		ModeFixed,
		ModeScrollable
	};
	enum ViewState
	{
		Normal,
		LandscapeLeft,
		LandscapeRight,
		LandscapeCenter
	};
public:
	InternalSubscene();
	~InternalSubscene();

	const Transform & transform() const;
	void setTransform(const Transform & transform);

	void draw(GfxRenderer * gl) const;
	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();
	virtual bool isInit() {return gl_ != NULL;}

	/**
	In scroll or drag modes, set whether the scroll position should be locked
	(whether the contents should be kept stationary)
	*/
	void setLocked(bool locked) {locked_ = locked;}

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	Scene * const & scene() const {return scene_;}
	void setScene(Scene * const & scene);
	void changeScene(Scene * const & scene);

	const float & windowWidth() const {return windowWidth_;}
	const float & windowHeight() const {return windowHeight_;}

	void setWindowWidth(const float & width);
	void setWindowHeight(const float & height);

	const bool & transparentBg() const {return transparentBg_;}
	void setTransparentBg(const bool & transparentBg) 
	{transparentBg_ = transparentBg;}

	void setScrollPosition(const Vector2 & pos);
	void setDragPosition(const Vector2 & pos);

	const Vector2 & scrollPosition() const {return scenePos_;}

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

	void setViewState(const ViewState & viewState) { viewState_ = viewState; }

	void resetScrollTween()
	{
		scrollTweenDuration_ = 0;		
	}

	virtual AppObjectOffset appObjectOffset() const;

	virtual bool clickEvent(const Vector2 & mousePos, int pressId);
	virtual bool doubleClickEvent(const Vector2 & mousePos, int pressId);
	virtual bool pressEvent(const Vector2 & startPos, int pressId);
	virtual bool releaseEvent(const Vector2 & pos, int pressId);
	virtual bool moveEvent(const Vector2 & pos, int pressId);

	virtual bool keyPressEvent(int keyCode);
	virtual bool keyReleaseEvent(int keyCode);

	virtual void start(float docTime);	
	void start(float docTime, bool triggerEvent);
	void preStart(float docTime);

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

	virtual AppObjectOffset textFeaturesOffset() const;
private:
	float subsceneWidth() const;
	float subsceneHeight() const;
	/**
	Convert to local coords
	*/
	Vector2 processCoords(const Vector2 & screenCoords);
	/**
	Convert local coords to local scene coords
	*/
	Vector2 toLocalScenePos(const Vector2 & localPos) const;

	float modeWidth() const;
	float modeHeight() const;

	Vector2 boundToWindow(const Vector2 & scenePos) const;
	bool isScrollable() const;
	
	void create();

	void endScrollTween();
	bool isScrollTweenHappening() const;
	void startScrollTween(
		Vector2 * target, const Vector2 & startPos, const Vector2 & endPos);

	InternalSubscene(const InternalSubscene & rhs);
	InternalSubscene & operator = (const InternalSubscene & rhs);
	/*
	void initTextureRendering(GfxRenderer * gl);
	void uninitTextureRendering();
	bool isInitTextureRendering() const;
	*/
	//void doAppObjectOffset();
	void dragRelease(const Vector2 & localPos);

	bool checkParentHandled(Transform & t);
	void checkOutside(Transform & t);
	float pageEndLine(Transform & t);

	void loadZoomImageRequest();

private:
	Scene * scene_;		
	VisualAttrib visualAttrib_;

	Mode mode_;
	ViewState viewState_;

	float windowWidth_, windowHeight_;

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
	
	DragVelocityHandler dragVelHandler_;

	float scrollTweenStartTime_;
	float scrollTweenDuration_;
	Vector2 scrollTweenStartPos_;
	Vector2 scrollTweenEndPos_;
	Vector2 * scrollTweenTargetPos_;
	
	bool transparentBg_;
	bool redrawTriggered_;

	bool locked_;

	bool linkWithViewerLevelObjects_;

	bool displayingAppObjects_;

	bool isLoaded_;

	GfxRenderer * gl_;

	// Zoom in, Zoom out
	bool zoomFlag_;
	bool isMultiTouched_;
    bool isTouched_;
	float defaultTouchLength_;
	Vector3 pivot_;
	Vector3 scale_;
    Vector3 trans_;
	//int touchedID1_, touchedID2_;
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

	NetworkRequest * networkRequest_;
	float startTime_;
};

