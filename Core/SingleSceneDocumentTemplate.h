#pragma once
#include "DocumentTemplate.h"
#include "Camera.h"
class Document;
class Scene;
class LoadingScreen;
class InternalSubscene;

class SingleSceneDocumentTemplate : public DocumentTemplate
{
public:
	virtual DocumentTemplate * newInstance() const
	{
		return new SingleSceneDocumentTemplate;
	}

	virtual Scene * curScene() const {return scene();}
	virtual Scene * prevScene() const {return NULL;}
	virtual Scene * nextScene() const {return NULL;}
	virtual void getScenes(std::vector<Scene *> & scenes) const
	{
		scenes.clear(); 
		scenes.push_back(scene());
	}

	virtual const char * typeStr() const {return "SingleScene";}
	
	boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("ABCDDDDD-ABCD-ABCD-BADE-51D5377987AF");
	}

	virtual void setSize(int width, int height, bool resizeGl);
	
	SingleSceneDocumentTemplate();
	virtual ~SingleSceneDocumentTemplate();

	virtual bool isCurrentScene(Scene * scene) const;

	virtual void sceneChangeTo(Scene * scene);
	virtual void sceneSet(Scene * scene) {sceneChangeTo(scene);}

	virtual void init(GfxRenderer * gl);
	virtual void uninit();

	void setScene(Scene * scene);
	Scene * scene() const;

	virtual void draw(GfxRenderer * gl) const;

	virtual void reInitCurScene(GfxRenderer * gl);

	virtual bool clickEvent(const Vector2 & mousePos, int pressId);
	virtual bool doubleClickEvent(const Vector2 & mousePos, int pressId);
	virtual bool pressEvent(const Vector2 & startPos, int pressId, bool propagate);
	virtual bool releaseEvent(const Vector2 & pos, int pressId);
	virtual bool moveEvent(const Vector2 & pos, int pressId);

	virtual bool keyPressEvent(int keyCode);
	virtual bool keyReleaseEvent(int keyCode);

	virtual void start(float docTime);
	virtual bool update(float sceneTime);
	virtual bool asyncLoadUpdate();
	virtual void stop();

	virtual Scene * dependsOn(Scene * scene) const;
	virtual void pageScenes(std::vector<Scene *> * scenes) const;

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;	
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex);

	virtual unsigned char version() const {return 2;}
	virtual void write(Writer & writer) const;
	virtual const char * xmlTag() const {return "SingleSceneTemplate";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void read(Reader & reader, unsigned char);
	virtual void readXml(XmlReader & r, xmlNode * node);

	// request요청 scene의 우선순위를 정하기 위한 함수들
	void resetSaveLocalRequest();
private:

	void curSceneAppObjectStart() const;
	void curSceneAppObjectStop() const;

	Vector2 toLocalCoords(const Vector2 & coords) const;

	void sceneChangerCheck(Scene * scene);
	void imageChangerCheck(Scene * scene);

	void setSubsceneScaling();
private:
	boost::scoped_ptr<InternalSubscene> subscene_;
	bool isLoaded_;
	mutable bool displayingAppObjects_;

	Camera loadingCamera_;
	boost::scoped_ptr<LoadingScreen> loadingPage_;

	bool redrawTriggered_;
	mutable bool curPageStarted_;

	float width_, height_;

	Camera camera_;	

	std::map<int, bool> propagate_;

	bool isSceneCreated_;
	bool sceneCheck_;

	//use for preDownload
	bool resetComplete_;
	std::deque<Scene *> searchQ_;
};