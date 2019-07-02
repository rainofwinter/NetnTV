#pragma once
#include "DocumentTemplate.h"
#include "Color.h"
#include "Attrib.h"
#include "SceneContainer.h"
#include "PressEventData.h"
#include "DragVelocityHandler.h"
#include "Tween.h"
#include "Camera.h"


#define SUBSCENECOUNT 4

class InternalSubscene;
class TextureRenderer;
class AppImage;
class LoadingScreen;


#if defined(IOS)
#define USE_FBO 
#endif

#if defined(ANDROID)
#define USE_FBO
#endif



class CatalogDocumentTemplate : public DocumentTemplate
{
private:
	struct BookmarkData
	{
		BookmarkData() {}
		BookmarkData(int pageIndex, const std::string & filename, const std::string & time)
		{
			this->pageIndex = pageIndex;
			this->filename = filename;
			this->time = time;
		}
		int pageIndex;
		std::string filename;

		//version >= 1
		std::string time; // 1234년 56월 78일 91시 23분 45초 -> 12345678912345(14자리)

		unsigned char version() const {return 1;}
		void write(Writer & writer) const;
		void read(Reader & reader, unsigned char version);

		const char * xmlTag() const {return "BookmarkData";}
		void writeXml(XmlWriter & w) const;
		void readXml(XmlReader & r, xmlNode * parent);
	};

	struct LandscapeSubscenes
	{
		LandscapeSubscenes();
		boost::scoped_ptr<InternalSubscene> leftpage_;
		boost::scoped_ptr<InternalSubscene> rightpage_;

		void create();
		void setLeftpage(Scene* scene);
		void setRightpage(Scene* scene);
		bool correctRightPageTranslation();
	};
public:
	virtual DocumentTemplate * newInstance() const 
	{
		return new CatalogDocumentTemplate;
	}

	virtual const char * typeStr() const 
	{
		return "CatalogDocumentTemplate";
	}
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("02bd0758-c868-4b8c-b9b3-27d3ddb3af0f");
	}

	void setUiImgFiles(const std::vector<std::string> & uiImgFiles);
	void uiImgFiles(std::vector<std::string> * uiImgFiles);

	void setThumbFileMap(const std::map<int, std::string> & thumbFileMap);
	void thumbFileMap( std::map<int, std::string> * thumbFileMap);

	CatalogDocumentTemplate();

	~CatalogDocumentTemplate();

	const std::vector<Scene *> & scenes() const {return scenes_;}
	void setScenes(const std::vector<Scene *> & scenes) {scenes_ = scenes;}

	void drawLayout(GfxRenderer * gl) const;
	virtual void draw(GfxRenderer * gl) const;
	virtual void reInitCurScene(GfxRenderer * gl);

	virtual void init(GfxRenderer * gl);
	virtual void uninit();

	virtual Scene * prevScene() const;
	virtual Scene * nextScene() const;
	virtual Scene * curScene() const;
	virtual void getScenes(std::vector<Scene *> & scenes) const;

	virtual bool clickEvent(const Vector2 & mousePos, int pressId);
	virtual bool doubleClickEvent(const Vector2 & pos, int pressId);
	virtual bool pressEvent(const Vector2 & startPos, int pressId, bool propagate);
	virtual bool moveEvent(const Vector2 & pos, int pressId);
	virtual bool releaseEvent(const Vector2 & pos, int pressId);

	virtual bool keyPressEvent(int keyCode);
	virtual bool keyReleaseEvent(int keyCode);

	int thumbWidth() const;
	int thumbHeight() const;

	int thumbsdWidth() const;
	int thumbsdHeight() const;

	virtual void start(float docTime);
	virtual bool update(float sceneTime);
	bool dbg_update(float sceneTime);
	virtual bool asyncLoadUpdate();
	virtual void stop() {}

	const float & width() const {return width_;}
	const float & height() const {return height_;}

	virtual void setSize(int width, int height, bool resizeGl);
	virtual void sceneSet(Scene * scene);	

	virtual Scene * dependsOn(Scene * scene) const;
	virtual void pageScenes(std::vector<Scene *> * scenes) const;

	const std::string & pageChangingMsg() const {return pageChangingMsg_;}
	void setPageChangingMsg(const std::string & msg) {pageChangingMsg_ = msg;}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;	
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex);

	virtual bool isCurrentScene(Scene * scene) const;

	virtual void sceneChangeTo(Scene * scene);
	void sceneChangeTo(int pageIndex);

	Scene * pageLeft() const;
	Scene * pageRight() const;

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "CatalogDocument";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);

	virtual void changeLandscape(const bool & var);

	virtual void goToPage(const int & index);

	void clickThumbNailLayout();
	void setThumbnailFile(int key, const std::string file);

	virtual bool setCurPageBookmark();
	virtual void getBookmarkPages(std::vector<int> * pages);

	virtual int pageIndex() {return pageIndex_;}

private:	
	void create();
	void drawSubscene(GfxRenderer * gl, InternalSubscene * subscene) const;

	CatalogDocumentTemplate(const CatalogDocumentTemplate & rhs);
	CatalogDocumentTemplate & operator = (const CatalogDocumentTemplate & rhs);

	Ray processCoords(const Vector2 & pos);

	Vector3 getLocalCoords(
		const Vector2 & pos, const Matrix & invTransform) const;

	void setCurPage(int pageIndex);
	void setCurPagePos(const Vector3 & pos);

	void setPageAction(int pageIndex);

	void setScrollPos(float factor);
	float scrollPos() const;

	void initTexRenderer(GfxRenderer * gl);
	void uninitTexRenderer();

	void setDocument(Document * document);

	void doLayoutTween(float startPos, float endPos);

	void notifyPageChanging();

	void curSceneAppObjectStart() const;
	void curSceneAppObjectStop() const;

	bool shouldAcceptInput() const;

	void setSubsceneScaling();

	void doCurPageStart();

	bool pageHasBookmark(int pageIndex) const;
	void addBookmark(const BookmarkData & data);
	void removeBookmark(int pageIndex);
	void readBookmarks();
	void writeBookmarks();

	bool hasPageBeenMoved() const;
	void doPageSnapBack(const PressEventData & pressEvent, const Vector2 & pos);
	Vector3 zoomedPageTweenEndPos(bool & ql, bool & qr, bool & qf);
	bool checkOutside(Transform & t, bool hasDirectionScene = false);

	// request요청 scene의 우선순위를 정하기 위한 함수들
	void resetSaveLocalRequest();
    void loadZoomImageRequest();

	void setButtonTransform();

	bool thumbnailClickEvent(const Vector2 & pos);

	void changeLandscape();
	void correctLandscapeSceneImage();

	void fadeIn();
	void fadeOut();

	void drawBookmark(GfxRenderer * gl) const;
	std::string currentTimeString();

private:

	float width_, height_;
	float thumbnailScale_;
	float scrollThumbWidth_;
	float scrollButtonWidth_;
	float layoutButtonWidth_;
	float gapFactor_;

	std::string topMenuLayoutButtonFile_;
	//std::string thumbnailNormalOpenedFile_;
	//std::string thumbnailNormalClosedFile_;
	//std::string thumbnailWidthOpenedFile_;
	//std::string thumbnailWidthClosedFile_;
	std::string thumbnailNormalBarFile_;
	std::string thumbnailWidthBarFile_;
	std::string thumbnailNormalClosedFile_;
	std::string thumbnailNormalOpenedFile_;
	std::string bookmarkCheckFile_;
	std::string thumbnailShadowFile_;
	std::string thumbnailWidthClosedFile_;
	std::string thumbnailWidthOpenedFile_;
	//std::string lineShadowFile_;
	//std::string toContentsFile_;
	std::vector<std::string *> uiImgFiles_;

	boost::scoped_ptr<Image> menuLayoutButton_;

	//TextureSPtr thumbnailNormalClosed_;
	//TextureSPtr thumbnailNormalOpened_;
	//TextureSPtr thumbnailWidthClosed_;
	//TextureSPtr thumbnailWidthOpened_;
	TextureSPtr thumbnailNormal_;
	TextureSPtr thumbnailWidth_;
	TextureSPtr thumbnailOpened_;
	TextureSPtr thumbnailClosed_;
	TextureSPtr bookmarkCheck_;
	TextureSPtr thumbnailShadow_;
	TextureSPtr thumbnailWidthClosed_;
	TextureSPtr thumbnailWidthOpened_;
	//TextureSPtr lineShadow_;

	//TextureSPtr scrollBarBg_;
	//TextureSPtr scrollBarLeft_;
	//TextureSPtr scrollBarRight_;
	//boost::scoped_ptr<Image> scrollBarThumb_;

	float layoutX_;
	float layoutOffsetY_;
	bool isOpenedThumb_;

	boost::scoped_ptr<InternalSubscene> subscene_;
	boost::scoped_ptr<InternalSubscene> subsceneCached_[SUBSCENECOUNT];
	
	LandscapeSubscenes* landSubscene_;
	LandscapeSubscenes* landSubsceneCached_[2];

	std::vector<InternalSubscene *> subSceneBackup_;
	//std::vector<LandscapeSubscenes *> landSubSceneBackup_;

	std::vector<Scene *> scenes_;

	VisualAttrib visualAttrib_;

	bool isLandscape_;

	float startScrollX_;
	bool redrawTriggered_;	

	/**
	id of the press event that initiated the current page drag.
	-1 if there is no page drag.
	*/
	int pageMovingId_;
	std::map<int, PressEventData> pressEvents_;

	float scrollPos_;

	
	/**
	x - scrollPos
	y - layoutOffsetY
	*/
	Tween<Vector2> layoutTween_;
	int layoutTweenEndPageIndex_;

	Tween<float> layoutYMoveTween_;


	Tween<Vector3> pageTween_;
	int targetPageIndex_;

	int pageIndex_;

	enum MoveDirection{
		Horz,
		Vert,
		Undetermined
	} moveDirection_, requiredLayoutMoveDirection_;


	std::map<int, std::string> thumbFiles_;
	struct ThumbData 
	{
		void uninit();
		TextureSPtr texture;
		bool used;
	};
	mutable std::map<int, ThumbData> thumbMap_;

	mutable bool thumbsLoadedConfirmed_;

	bool scrollThumbGrabbed_;
	Vector3 thumbGrabbedDelta_;

	
	std::string pageChangingMsg_;

	bool init_;

	mutable bool displayingAppObjects_;
#ifdef USE_FBO

	TextureRenderer * texRenderer_;
	Texture * renderTex_;

#else

	mutable GLuint ssTexture_;
	mutable unsigned char * ssBuffer_;
	mutable int ssWidth_, ssHeight_;
#endif
	GLuint programThumb_;
	GLint locThumbMVPMatrix_;	

	boost::scoped_ptr<LoadingScreen> loadingPage_;
	mutable bool curPageStarted_;

	std::vector<BookmarkData> bookmarks_;

	DragVelocityHandler dragVelHandler_;
	bool layoutModeSnappedBackYet_;
	float startLayoutOffsetY_;

	int btnDim_;
	int btnMargin_;

	boost::scoped_ptr<AppImage> aspectMargin1_, aspectMargin2_;
	bool preserveAspect_;

	Camera camera_;

	bool resetComplete_;
	std::deque<Scene *> searchQ_;

	bool doBookmarks_;
	bool doAspect_;

	float landscapeMovingTranslation_;

	float opacity_;
	bool fadeIn_;
	bool fadeOut_;
};