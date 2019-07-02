#pragma once
#include "SceneObject.h"
#include "Color.h"
#include "Attrib.h"

#include "PressEventData.h"
#include "Texture.h"
///////////////////////////////////////////////////////////////////////////////
class Mesh;
class Material;
class Subscene;
class LoadingScreen;
class ScriptProcessor;

class ImageChanger : public SceneObject
{
public:
	enum ScrollDirection
	{
		Horizontal = 0,
		Vertical
	};

	enum MarkerLocation
	{
		Left = 0,
		Top,
		Right,
		Bottom
	};
public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "ImageChanger";
	}
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("DADDAD0D-ABCD-DDCC-EDAB-51D5377987AF");
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new ImageChanger(*this, elementMapping);}

	
	virtual void remapReferences(const ElementMapping & elementMapping);
	
	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(
		const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex);

	ImageChanger();
	ImageChanger(const ImageChanger & rhs, ElementMapping * elementMapping);
	
	~ImageChanger();

	void drawLayout(GfxRenderer * gl) const;
	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;	

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	virtual bool clickEvent(const Vector2 & mousePos, int pressId);
	virtual bool pressEvent(const Vector2 & startPos, int pressId);
	virtual bool moveEvent(const Vector2 & pos, int pressId);
	virtual bool releaseEvent(const Vector2 & pos, int pressId);

	virtual void start(float docTime);
	virtual void preStart(float docTime);
	virtual bool update(float sceneTime);
	virtual bool asyncLoadUpdate();
	virtual bool isLoaded() const;

	int numScenes() const;
	int prevImageIndex() const;
	int nextImageIndex() const;

	const float & width() const {return width_;}
	void setWidth(const float & width) {width_ = width;}
	const float & height() const {return height_;}
	void setHeight(const float & height) {height_ = height;}

	void setImage(int index);

	enum TransitionDirection
	{
		TransitionDirectionAuto,
		TransitionDirectionMinus,
		TransitionDirectionPlus
	};
	void imageChangeTo(int index, TransitionDirection dir = TransitionDirectionAuto);

	const std::vector<std::string> & fileNames() const 
	{
		return fileNames_;
	}
	
	void setFileNames(const std::vector<std::string> & fileNames);

	virtual bool dependsOn(Scene * scene) const;

	const std::string & activeMarker() const {return activeMarkerFileName_;}
	void setActiveMarker(const std::string & file) {activeMarkerFileName_ = file;}

	const std::string & inactiveMarker() const {return inactiveMarkerFileName_;}
	void setInactiveMarker(const std::string & file) {inactiveMarkerFileName_ = file;}

	const bool & allowDrag() const {return allowDrag_;}
	void setAllowDrag(const bool & allowDrag) {allowDrag_ = allowDrag;}

	const bool & showMarkers() const {return showMarkers_;}
	void setShowMarkers(const bool & val) {showMarkers_ = val;}

	const ScrollDirection & scrollDirection() const {return scrollDirection_;}
	void setScrollDirection(const ScrollDirection & val) {scrollDirection_ = val;}

	const MarkerLocation & markerLocation() const {return markerLocation_;}
	void setMarkerLocation(const MarkerLocation & val) {markerLocation_ = val;}

	const float & autoTransitionTime() const {return autoTransitionTime_;}
	void setAutoTransitionTime(const float & autoTransitionTime) 
	{
		autoTransitionTime_ = autoTransitionTime;
	}

	virtual unsigned char version() const {return 4;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "ImageChanger";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	void setFormat(Texture::Format format) {format_ = format;}
	Texture::Format format() const {return format_;}

	/**
	@return handled
	*/
	bool doPageChangedEvent(int index);

	const int & pageIndex() {return pageIndex_;}

	virtual void requestLocalSaveObject();

private:	
	void create();
	ImageChanger(const ImageChanger & rhs);
	ImageChanger & operator = (const ImageChanger & rhs);
	bool intersectRect(Vector3 * out, const Ray & ray);

	void scrollModeClickEvent(const Vector3 & pt, const Ray & mouseRay);
	void layoutModeClickEvent(const Vector3 & pt, const Ray & mouseRay);
	void pageModeClickEvent(const Vector3 & pt, const Ray & mouseRay);

	Vector3 getLocalCoords(
		const Vector2 & pos, const Matrix & invTransform) const;


	const std::string * pagePrev() const;
	const std::string * pageNext() const;

	/**
	This is for the benefit of the editor
	In the editor the SceneChanger is displayed without start being called
	so this function sets subscene_ to just display the first scene without
	triggering the full start process
	*/
	void setInitPage();

	void setCurPage(int pageIndex);
	void setCurPagePos(const Vector3 & pos);

	void resetPageTween()
	{
		pageTweenDuration_ = 0;
		pageTweenLastT_ = 0;
	}

	bool isPageTweenHappening() const 
	{
		return pageTweenDuration_ > 0;
	}


	void doScrollTween(float startPos, float endPos);


	int cursorOnWhichMarker(const Vector3 & pt);
	/**
	@param pos from 0 to 1
	*/
	void setRangePos(float pos);
	float getCursorRangePos(const Vector3 & pt);

	bool hasPageBeenMoved() const;
	void doPageSnapBack(const PressEventData & pressEvent, const Vector2 & pos);

private:
	void drawMarkers(GfxRenderer * gl) const;

	VisualAttrib visualAttrib_;

	float width_, height_;
		
	boost::scoped_ptr<Image> subscene_;
	boost::scoped_ptr<Image> subscenePrev_;
	boost::scoped_ptr<Image> subsceneNext_;

	std::vector<std::string> fileNames_;

	Matrix invTransMatrix_;
	Matrix transMatrix_;
	bool redrawTriggered_;
	
	/**
	id of the press event that initiated the current page drag.
	-1 if there is no page drag.
	*/
	int pageMovingId_;
	std::map<int, PressEventData> pressEvents_;
	//PressEventData pressEvent_;

	float pageTweenStartTime_;
	float pageTweenDuration_;
	float pageTweenLastT_;
	int targetArticleIndex_;
	int targetPageIndex_;
	Vector3 pageTweenStartPos_;
	Vector3 pageTweenEndPos_;

	Vector3 startPagePos_;
	int pageIndex_;

	bool allowDrag_;


	Transform origSubsceneTransform_;
	
	boost::scoped_ptr<Texture> activePageImg_, inactivePageImg_;	
	std::string activeMarkerFileName_, inactiveMarkerFileName_;
	bool showMarkers_;
	bool draggingOnMarkers_;


	LoadingScreen * loadingIcon_;
	Texture::Format format_;

	ScrollDirection scrollDirection_;	
	MarkerLocation markerLocation_;
	int markerOffsetX_, markerOffsetY_;

	float autoTransitionTime_;
	float prevAutoTransitionTime_;
};

