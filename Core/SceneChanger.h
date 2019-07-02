#pragma once
#include "SubScene.h"
#include "SceneObject.h"
#include "Color.h"
#include "Attrib.h"
#include "SceneContainer.h"
#include "PageFlipInterface.h"
#include "PressEventData.h"
///////////////////////////////////////////////////////////////////////////////
class Mesh;
class Material;
class Subscene;
class TextureRenderer;
class Texture;
class LoadingScreen;
class ScriptProcessor;
class PageFlipInterface;

class SceneChanger : public SceneObject, public SceneContainer
{
public:
	enum ScrollDirection
	{
		Horizontal,
		Vertical
	};

	enum MarkerLocation
	{
		Left,
		Top,
		Right,
		Bottom
	};

	enum TransitionMode
	{
		SlidingMode,
		PageFlipMode,
		PageFlipOrigMode
	};

public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "SceneChanger";
	}
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("BADBL00D-ABCD-EFB9-BADE-51D5377987AF");
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new SceneChanger(*this, elementMapping);}

	
	virtual void remapReferences(const ElementMapping & elementMapping);
	
	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex);

	SceneChanger();
	SceneChanger(const SceneChanger & rhs, ElementMapping * elementMapping);
	
	~SceneChanger();

	void drawLayout(GfxRenderer * gl) const;
	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;	

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	virtual bool clickEvent(const Vector2 & mousePos, int pressId);
	virtual bool doubleClickEvent(const Vector2 & mousePos, int pressId);
	virtual bool pressEvent(const Vector2 & startPos, int pressId);
	virtual bool moveEvent(const Vector2 & pos, int pressId);	
	virtual bool releaseEvent(const Vector2 & pos, int pressId);


	virtual void start(float docTime);
	virtual void preStart(float docTime);
	virtual void stop();
	virtual bool update(float sceneTime);
	virtual bool asyncLoadUpdate();
	virtual bool isLoaded() const;

	int numScenes() const;

	const float & width() const {return width_;}
	void setWidth(const float & width) {width_ = width;}
	const float & height() const {return height_;}
	void setHeight(const float & height) {height_ = height;}

	Scene * prevScene() const;
	Scene * nextScene() const;

	int pageIndex() const {return pageIndex_;}

	virtual void sceneChangeTo(Scene * scene);	
	void sceneChangeTo(int index);
	virtual void sceneSet(Scene * scene);
	void sceneSet(int index);

	const std::vector<Scene *> & scenes() const {return scenes_;}
	void setScenes(const std::vector<Scene *> & scenes);

	virtual bool dependsOn(Scene * scene) const;
	virtual void dependsOnScenes(std::vector<Scene *> * dependsOnScenes, bool recursive = true) const;
	virtual bool isShowing(Scene * scene) const;
	virtual void showingScenes(std::set<Scene *> * showingScenes) const;

	const std::string & activeMarker() const {return activeMarkerFileName_;}
	void setActiveMarker(const std::string & file) {activeMarkerFileName_ = file;}

	const std::string & inactiveMarker() const {return inactiveMarkerFileName_;}
	void setInactiveMarker(const std::string & file) {inactiveMarkerFileName_ = file;}

	const bool & allowDrag() const {return allowDrag_;}
	void setAllowDrag(const bool & allowDrag) {allowDrag_ = allowDrag;}

	const bool & transparentBg() const;
	void setTransparentBg(const bool & transparentBg);

	void notifyPageChanging();
	const std::string & pageChangingMsg() const {return pageChangingMsg_;}
	void setPageChangingMsg(const std::string & msg) {pageChangingMsg_ = msg;}

	const ScrollDirection & scrollDirection() const {return scrollDirection_;}
	void setScrollDirection(const ScrollDirection & val) {scrollDirection_ = val;}

	const MarkerLocation & markerLocation() const {return markerLocation_;}
	void setMarkerLocation(const MarkerLocation & val) {markerLocation_ = val;}

	const bool & linkWithViewerLevelObjects() const {return linkWithViewerLevelObjects_;}
	void setLinkWithViewerLevelObjects(const bool & linkWithViewerLevelObject)
	{
		linkWithViewerLevelObjects_ = linkWithViewerLevelObject;
	}

	const bool & preloadAdjacent() const {return preloadAdjacent_;}
	void setPreloadAdjacent(const bool & val) {preloadAdjacent_ = val;}

	/**
	@return handled
	*/
	bool doPageChangedEvent(int index);

	const bool & showMarkers() const {return showMarkers_;}
	void setShowMarkers(const bool & val) {showMarkers_ = val;}

	virtual unsigned char version() const {return 10;}
	virtual void write(Writer & writer) const;

	virtual const char * xmlTag() const {return "SceneChanger";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual void read(Reader & reader, unsigned char);

	virtual AppObjectOffset appObjectOffset() const;

	void setTransitionMode(const TransitionMode & transitionMode);
	const TransitionMode & transitionMode() const {return transitionMode_;}

	bool shouldAcceptInput() const;

	void resizeText(GfxRenderer * gl);

	const Subscene::CameraMode & cameraMode() const
	{
		return cameraMode_;
	}

	void setCameraMode(const Subscene::CameraMode & cameraMode);

	virtual bool showPoint(Vector2 & outCoords, Scene * scene, const Vector2 & coords);

	virtual Vector2 convertToChildSceneCoords(const Vector2 & coords) const;
	virtual Vector2 convertFromChildSceneCoords(const Vector2 & coords) const;

	virtual void getSceneObjectsByID(const boost::uuids::uuid & type, std::vector<SceneObject *> * objs) const;

	void reSize();

	virtual void requestLocalSaveObject();
	Scene * curScene();
	virtual AppObjectOffset textFeaturesOffset() const;
private:	
	void create();
	SceneChanger(const SceneChanger & rhs);
	SceneChanger & operator = (const SceneChanger & rhs);
	bool intersectRect(Vector3 * out, const Ray & ray);

	void scrollModeClickEvent(const Vector3 & pt, const Ray & mouseRay);
	void layoutModeClickEvent(const Vector3 & pt, const Ray & mouseRay);
	void pageModeClickEvent(const Vector3 & pt, const Ray & mouseRay);

	Vector3 getLocalCoords(
		const Vector2 & pos, const Matrix & invTransform) const;


	Scene * pagePrev() const;
	Scene * pageNext() const;

	virtual void initAppObjects();
	virtual void uninitAppObjects();


	/**
	This is for the benefit of the editor
	In the editor the SceneChanger is displayed without start being called
	so this function sets subscene_ to just display the first scene without
	triggering the full start process
	*/
	void setInitPage();

	void setCurPage(int pageIndex);
	void setCurPagePos(const Vector3 & pos);

	void drawSubscene(GfxRenderer * gl, Subscene * subscene) const;

	void resetPageTween()
	{
		pageTweenDuration_ = 0;
	}

	bool isPageTweenHappening() const 
	{
		return pageTweenDuration_ > 0;
	}


	void doScrollTween(float startPos, float endPos);

	void initTextureRendering(GfxRenderer * gl);
	void uninitTextureRendering();

	int cursorOnWhichMarker(const Vector3 & pt);
	/**
	@param pos from 0 to 1
	*/
	void setRangePos(float pos);
	float getCursorRangePos(const Vector3 & pt);

	void doAppObjectOffset();

	bool hasPageBeenMoved() const;
	void doPageSnapBack(const PressEventData & pressEvent, const Vector2 & pos);

	void takeScreenshot(GfxRenderer * gl, Subscene * subscene) const;
	Vector2 toUnitCoords(const Vector2 & coords) const;

	void drawPageFlip(GfxRenderer * gl, Subscene * subscene) const;

	void setSubsceneScaling();
private:
	void drawMarkers(GfxRenderer * gl) const;

	void doCurPageStart();

	VisualAttrib visualAttrib_;

	float width_, height_;
	std::string pageChangingMsg_;
		
	boost::scoped_ptr<Subscene> subscene_;
	boost::scoped_ptr<Subscene> subscenePrev_;
	boost::scoped_ptr<Subscene> subsceneNext_;
	std::vector<Subscene *> subSceneBackup_;

	std::vector<Scene *> scenes_;

	Matrix invTransMatrix_;
	Matrix transMatrix_;
	mutable bool redrawTriggered_;
	
	/**
	id of the press event that initiated the current page drag.
	-1 if there is no page drag.
	*/
	int pageMovingId_;
	Vector2 pageMovingPos_;
	std::map<int, PressEventData> pressEvents_;
	//PressEventData pressEvent_;

	float pageTweenStartTime_;
	float pageTweenDuration_;
	int targetArticleIndex_;
	int targetPageIndex_;
	Vector3 pageTweenStartPos_;
	Vector3 pageTweenEndPos_;

	Vector3 startPagePos_;
	int pageIndex_;

	bool allowDrag_;
	
	boost::scoped_ptr<Texture> activePageImg_, inactivePageImg_;
	std::string activeMarkerFileName_, inactiveMarkerFileName_;
	bool showMarkers_;
	bool draggingOnMarkers_;

	bool linkWithViewerLevelObjects_;
	bool displayingAppObjects_;


	
	bool useStencilBuffer_;
	TextureRenderer * texRenderer_, *texRenderer2_;
	Texture * renderTex_, *renderTex2_;		

	boost::scoped_ptr<LoadingScreen> loadingPage_;
	mutable bool curPageStarted_;

	ScrollDirection scrollDirection_;
	MarkerLocation markerLocation_;
	int markerOffsetX_, markerOffsetY_;

	bool preloadAdjacent_;


	TransitionMode transitionMode_;
	TransitionDirection transDir_, prevTransDir_;
	PageFlipInterface * pageFlip_;	

	Subscene::CameraMode cameraMode_;

	float normalScaleX, normalScaleY;
};

