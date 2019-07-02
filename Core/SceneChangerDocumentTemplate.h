#pragma once
#include "DocumentTemplate.h"
#include "Color.h"
#include "Attrib.h"
#include "SceneContainer.h"

#include "PressEventData.h"
#include "Camera.h"
///////////////////////////////////////////////////////////////////////////////
class InternalSubscene;
class LoadingScreen;

class SceneChangerDocumentTemplate : public DocumentTemplate
{
public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual DocumentTemplate * newInstance() const 
	{
		return new SceneChangerDocumentTemplate;
	}

	virtual Scene * curScene() const;
	virtual Scene * prevScene() const;
	virtual Scene * nextScene() const;
	virtual void getScenes(std::vector<Scene *> & scenes) const;

	virtual const char * typeStr() const 
	{
		return "SceneChanger";
	}
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("BADDL01B-ABCD-EFB9-BADE-51D5377987FF");
	}

	virtual void setSize(int width, int height, bool resizeGl);
	
	virtual void setDocument(Document * document);

	SceneChangerDocumentTemplate();	
	~SceneChangerDocumentTemplate();

	virtual void init(GfxRenderer * gl);
	virtual void uninit();

	virtual void draw(GfxRenderer * gl) const;	

	virtual void reInitCurScene(GfxRenderer * gl);
	
	virtual bool clickEvent(const Vector2 & mousePos, int pressId);
	virtual bool doubleClickEvent(const Vector2 & mousePos, int pressId);
	virtual bool pressEvent(const Vector2 & startPos, int pressId, bool propagate);
	virtual bool moveEvent(const Vector2 & pos, int pressId);	
	virtual bool releaseEvent(const Vector2 & pos, int pressId);

	virtual bool keyPressEvent(int keyCode);
	virtual bool keyReleaseEvent(int keyCode);

	virtual bool isCurrentScene(Scene * scene) const;

	virtual void start(float docTime);
	virtual bool update(float sceneTime);
	virtual bool asyncLoadUpdate();
	virtual void stop() {}

	virtual Scene * dependsOn(Scene * scene) const;
	virtual void pageScenes(std::vector<Scene *> * scenes) const;

	int numScenes() const;

	void sceneChangeTo(int index);
	virtual void sceneChangeTo(Scene * scene);
	virtual void sceneSet(Scene * scene);

	const std::vector<Scene *> & scenes() const {return scenes_;}
	void setScenes(const std::vector<Scene *> & scenes) {scenes_ = scenes;}

	const std::string & activeMarker() const {return activeMarkerFileName_;}
	void setActiveMarker(const std::string & file) {activeMarkerFileName_ = file;}

	const std::string & inactiveMarker() const {return inactiveMarkerFileName_;}
	void setInactiveMarker(const std::string & file) {inactiveMarkerFileName_ = file;}

	const bool & allowDrag() const {return allowDrag_;}
	void setAllowDrag(const bool & allowDrag) {allowDrag_ = allowDrag;}

	void notifyPageChanging();
	const std::string & pageChangingMsg() const {return pageChangingMsg_;}
	void setPageChangingMsg(const std::string & msg) {pageChangingMsg_ = msg;}

	/**
	@return handled
	*/
	bool doPageChangedEvent(int index);

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;	
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex);

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual const char * xmlTag() const {return "SceneChangerTemplate";}
	virtual void writeXml(XmlWriter & w) const;

	virtual void read(Reader & reader, unsigned char);
	virtual void readXml(XmlReader & r, xmlNode * node);
	bool addEventListener(const std::string & event, JSObject * handler);

	Scene * pageLeft() const;
	Scene * pageRight() const;

	// request요청 scene의 우선순위를 정하기 위한 함수들
	void resetSaveLocalRequest();
private:	
	void create();
	SceneChangerDocumentTemplate(const SceneChangerDocumentTemplate & rhs);
	SceneChangerDocumentTemplate & operator = (const SceneChangerDocumentTemplate & rhs);

	void setCurPage(int pageIndex);
	void setCurPagePos(const Vector3 & pos);
	

	void curSceneAppObjectStart() const;
	void curSceneAppObjectStop() const;

	void drawSubscene(GfxRenderer * gl, InternalSubscene * subscene) const;

	void resetPageTween()
	{
		pageTweenDuration_ = 0;
	}

	bool isPageTweenHappening() const 
	{
		return pageTweenDuration_ > 0;
	}

	void doScrollTween(float startPos, float endPos);
	void setSubsceneScaling();
	void doCurPageStart();
	
	enum Event
	{
		EventSceneChanged
	};

	bool fireEvent(Event events, JSObject * evtObject);

	bool hasPageBeenMoved() const;
	void doPageSnapBack(const PressEventData & pressEvent, const Vector2 & pos);

	void setCachedScene(InternalSubscene * subsceneCached, Scene * scene);

	void sceneChangerCheck(Scene * scene);
	void imageChangerCheck(Scene * scene);

private:
	void drawMarkers(GfxRenderer * gl) const;

	VisualAttrib visualAttrib_;
	

	float width_, height_;
	std::string pageChangingMsg_;
		
	boost::scoped_ptr<InternalSubscene> subscene_;
	boost::scoped_ptr<InternalSubscene> subsceneLeft_;
	boost::scoped_ptr<InternalSubscene> subsceneRight_;
	std::vector<InternalSubscene *> subSceneBackup_;

	std::vector<Scene *> scenes_;

	Matrix invTransMatrix_;
	Matrix transMatrix_;
	bool redrawTriggered_;
	
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

	bool init_;
	bool sceneCheck_;

	Transform origSubsceneTransform_;
	
	boost::scoped_ptr<Texture> activePageImg_, inactivePageImg_;
	std::string activeMarkerFileName_, inactiveMarkerFileName_;
	bool showMarkers_;

	mutable bool displayingAppObjects_;


	boost::scoped_ptr<LoadingScreen> loadingPage_;
	mutable bool curPageStarted_;


	typedef std::multimap<Event, JSObject *> EventListeners;
	EventListeners eventListeners_;
	
	Camera camera_;

	Texture * netntvTexture_;

	//use for preDownload
	bool resetComplete_;
	std::deque<Scene *> searchQ_;
};

