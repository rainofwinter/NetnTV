#pragma once
#include "DocumentTemplate.h"
#include "Color.h"
#include "Attrib.h"
#include "SceneContainer.h"
#include "PressEventData.h"
#include "MagazineArticle.h"
#include "DragVelocityHandler.h"
#include "Tween.h"
#include "Camera.h"

///////////////////////////////////////////////////////////////////////////////

class InternalSubscene;
class TextureRenderer;
class AppImage;
class LoadingScreen;
class PageFlipInterface;

#if defined(IOS)
#define USE_FBO 
#endif

#if defined(ANDROID)
#define USE_FBO
#endif



class MagazineDocumentTemplate : public DocumentTemplate
{
private:
	enum Mode
	{
		LayoutMode,		
		Transitioning,
		PageMode,
		BookmarkListMode		
	};


	struct BookmarkData
	{
		BookmarkData() {}
		BookmarkData(int articleIndex, int pageIndex, const std::string & text)
		{
			this->articleIndex = articleIndex;
			this->pageIndex = pageIndex;
			this->text = text;
		}
		int articleIndex;
		int pageIndex;
		std::string text;

		virtual unsigned char version() const {return 0;}
		virtual void write(Writer & writer) const;
		virtual void read(Reader & reader, unsigned char version);
	};

public:
	enum TransitionMode
	{
		SlidingMode,
		PageFlipMode,
		PageFlipOrigMode
	};

	enum HomeButtonFunction 
	{
		HomeButtonFirstPage,
		HomeButtonExitToViewerLayer
	};

public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);


	virtual DocumentTemplate * newInstance() const 
	{
		return new MagazineDocumentTemplate;
	}

	virtual const char * typeStr() const 
	{
		return "MagazineDocumentTemplate";
	}
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("ABCCADF2-ABCD-EFB9-BEEE-51D5377EE7AF");
	}	

	void setUiImgFiles(const std::vector<std::string> & uiImgFiles);
	void uiImgFiles(std::vector<std::string> * uiImgFiles);

	MagazineDocumentTemplate();

	
	~MagazineDocumentTemplate();

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

	virtual void start(float docTime);
	virtual bool update(float sceneTime);
	bool dbg_update(float sceneTime);
	virtual bool asyncLoadUpdate();
	virtual void stop() {}

	void setArticles(const std::vector<MagazineArticleSPtr> & articles)
	{
		articles_ = articles;
	}

	int numArticles() const 
	{
		return (int)articles_.size();
	}

	int numPages() const;

	const std::vector<MagazineArticleSPtr> & articles() const {return articles_;}

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
	void sceneChangeTo(int articleIndex, int pageIndex);

	Scene * pageLeft() const;
	Scene * pageRight() const;
	Scene * pageTop() const;
	Scene * pageBottom() const;

	void setTransitionMode(const TransitionMode & transitionMode) {transitionMode_ = transitionMode;}
	const TransitionMode & transitionMode() const {return transitionMode_;}

	const bool & doBookmarks() const {return doBookmarks_;}
	void setDoBookmarks(const bool & doBookmarks) {doBookmarks_ = doBookmarks;}

	const bool & doAspect() const {return doAspect_;}
	void setDoAspect(const bool & doAspect) {doAspect_ = doAspect;}

	const HomeButtonFunction & homeButtonFunction() const {return homeButtonFunction_;}
	void setHomeButtonFunction(const HomeButtonFunction & homeButtonFunction)
	{
		homeButtonFunction_ = homeButtonFunction;
	}

	const bool & resetArticleFirstPage() const {return resetArticleFirstPage_;}
	void setResetArticleFirstPage(const bool & resetArticle) {resetArticleFirstPage_ = resetArticle;}

	const bool & doToContentsPage() const {return toContentsPage_;}
	void setDoToContentsPage(const bool & val) {toContentsPage_ = val;}

	const int & contentsArticleIndex() const 
	{
		return contentsArticleIndex_;
	}	
	void setContentsArticleIndex(const int & contentsArticleIndex) 
	{
		contentsArticleIndex_ = contentsArticleIndex;
	}

	//version 18: added PageFlipOrigMode to transitionMode_
	virtual unsigned char version() const {return 19;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
	virtual const char * xmlTag() const {return "MagazineTemplate";}

	// request요청 scene의 우선순위를 정하기 위한 함수들
	void resetSaveLocalRequest();
private:	
	void create();
	void drawSubscene(GfxRenderer * gl, InternalSubscene * subscene) const;

	MagazineDocumentTemplate(const MagazineDocumentTemplate & rhs);
	MagazineDocumentTemplate & operator = (const MagazineDocumentTemplate & rhs);

	Ray processCoords(const Vector2 & pos);

	bool scrollModeIsInPage(const Vector2 & localPt);

	void layoutModeClickEvent(const Vector2 & pt);

	Vector3 getLocalCoords(
		const Vector2 & pos, const Matrix & invTransform) const;

	int articleIndexFromScrollPos() const
	{
		return (int)roundFloat(scrollPos_ * (articles_.size() - 1));
	}

	void setCurPage(int articleIndex, int pageIndex);
	void setCurPagePos(const Vector3 & pos);

	void setPageAction(int articleIndex, int pageIndex);

	void setScrollPos(float factor);
	float scrollPos() const;

	void initTexRenderer(GfxRenderer * gl);
	void uninitTexRenderer();

	void setDocument(Document * document);

	void doLayoutTween(float startPos, float endPos);

	void notifyPageChanging();
	void setMode(Mode mode);

	void curSceneAppObjectStart() const;
	void curSceneAppObjectStop() const;

	bool shouldAcceptInput() const;
	
	void setSubsceneScaling();
	void setScrollBarThumbTransform();

	void setMenuTransform();
	void showMenu();
	void hideMenu();
	bool isMenuShowing() const;
	bool isMenuHidden() const;

	bool doMenuButtonInput(const Ray & localMouseRay);

	void doCurPageStart();

	bool pageHasBookmark(int articleIndex, int pageIndex) const;
	void addBookmark(const BookmarkData & data);
	void removeBookmark(int articleIndex, int pageIndex);
	void drawBookmarkList(GfxRenderer * gl) const;
	void setBookmarkUiSize();
	void readBookmarks();
	void writeBookmarks();
	void populateBookmarksScene();
	void updateBookmarksScene();

	bool hasPageBeenMoved() const;
	void doPageSnapBack(const PressEventData & pressEvent, const Vector2 & pos);
    Vector3 zoomedPageTweenEndPos(bool & ql, bool & qr, bool & qp, bool & qn, bool & qf);
	bool checkOutside(Transform & t, float normalScaleX, float normalScaleY, bool hasDirectionScene = false);

	Vector2 toUnitCoords(const Vector2 & coords) const;
	void takeScreenshot(GfxRenderer * gl, InternalSubscene * subscene) const;

	void setCachedScene(InternalSubscene * subsceneCached, Scene * scene);

	void sceneChangerCheck(Scene * scene);	
	void imageChangerCheck(Scene * scene);	
private:

	Mode mode_;


	float width_, height_;
	float thumbnailScale_;
	float topMenuHeight_;
	float bottomMenuHeight_;
	float scrollThumbWidth_;
	float scrollButtonWidth_;
	float layoutButtonWidth_;
	float gapFactor_;

	std::string scrollBarBgFile_;
	std::string scrollBarLeftSideFile_;
	std::string scrollBarRightSideFile_;
	std::string topMenuBgFile_;
	std::string topMenuLayoutButtonFile_;
	std::string topMenuHomeButtonFile_;
	std::string scrollBarThumbFile_;	
	std::string bgFile_;
	std::string bookmarkActiveFile_;
	std::string bookmarkInactiveFile_;
	std::string aspectActiveFile_;
	std::string aspectInactiveFile_;
	std::string bookmarkListFile_;
	std::string bookmarkTitleFile_;
	std::string toContentsFile_;
	std::vector<std::string *> uiImgFiles_;

	TextureSPtr scrollBarBg_;
	TextureSPtr scrollBarLeft_;
	TextureSPtr scrollBarRight_;
	TextureSPtr bg_;
	TextureSPtr bookmarkTitle_;
	boost::scoped_ptr<Image> scrollBarThumb_;

	float layoutX_;


	boost::scoped_ptr<InternalSubscene> subscene_;
	boost::scoped_ptr<InternalSubscene> subsceneCached_;
	enum WhichCached
	{
		CachedTop,
		CachedBottom,
		CachedLeft,
		CachedRight,
		CachedNone
	} whichCached_;
	boost::scoped_ptr<InternalSubscene> subsceneTransition_;

	std::vector<InternalSubscene *> subSceneBackup_;

	std::vector<MagazineArticleSPtr> articles_;
	
	VisualAttrib visualAttrib_;

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
	int layoutTweenEndArticleIndex_;


	Tween<Vector3> pageTween_;
	int targetArticleIndex_;
	int targetPageIndex_;

	//page zoom in/out transition when switching to and from layout mode
	
	void startModeTransitionAnim(
		float startX, float startY, float startScale,
		float endX, float endY, float endScale, 
		Mode endMode, int article, int index);

	bool isModeTransitionHappening() const;

	void endResetTransition();

	mutable enum TransitionStage
	{
		TransitionInactive,
		TransitionNeedToLoadTargetScene,
		TransitionScreenShotNotTaken,
		TransitionAnimating
	} transitionStage_;
	float transitionStartTime_;
	float transitionDuration_;
	bool transitionScreenShotTaken_;
	float transitionT_;
	Vector2 transitionStartPt_;
	Vector2 transitionEndPt_;
	float transitionStartScale_;
	float transitionEndScale_;
	Mode transitionEndMode_;
	int transitionEndArticle_, transitionEndPage_;



	enum MoveDirection{
		Horz,
		Vert,
		Undetermined
	} moveDirection_, requiredLayoutMoveDirection_;

	int articleIndex_;
	int pageIndex_;

	struct ThumbData 
	{

		void uninit();
		TextureSPtr texture;
		bool used;
	};
	mutable std::map<MagazinePage *, ThumbData> thumbMap_;
	mutable bool thumbsLoadedConfirmed_;

	bool scrollThumbGrabbed_;
	Vector3 thumbGrabbedDelta_;


	std::string pageChangingMsg_;

	bool init_;

	mutable bool displayingAppObjects_;
#ifdef USE_FBO

	TextureRenderer * texRenderer_;
	mutable Texture * renderTex_;

#else

	mutable GLuint renderTex_;
	mutable unsigned char * ssBuffer_;
	mutable int ssWidth_, ssHeight_;
#endif

	GLuint programThumb_;
	GLint locThumbMVPMatrix_;	

	float layoutOffsetY_;

	float menuStartOpacity_;
	float menuOpacity_;
	float targetMenuOpacity_;
	float menuTransitionStartTime_;
	float menuHideTime_;
	float menuDuration_;

	bool buttonPosHandled_;
	boost::scoped_ptr<Image> menuBg_;
	boost::scoped_ptr<Image> menuLayoutButton_;
	boost::scoped_ptr<Image> menuHomeButton_;
	boost::scoped_ptr<Image> menuBookmarkActiveButton_;
	boost::scoped_ptr<Image> menuBookmarkInactiveButton_;
	boost::scoped_ptr<Image> menuAspectActiveButton_;
	boost::scoped_ptr<Image> menuAspectInactiveButton_;
	boost::scoped_ptr<Image> menuBookmarkListButton_;
	boost::scoped_ptr<Image> menuToContentsButton_;
	std::vector<Image *> menuImgs_;
	
	boost::scoped_ptr<LoadingScreen> loadingPage_;
	mutable bool curPageStarted_;


	bool curPageHasBookmark_;
	std::vector<BookmarkData> bookmarks_;

	boost::shared_ptr<InternalSubscene> bookmarksScroller_;
	SceneSPtr bookmarksContentsScene_;
	int bookmarksLastTopIndex_;
	int bookmarksTopIndex_;
	float bookmarksLastScrollY_;
	std::vector<Image *> bookmarkThumbImgs_;
	boost::scoped_ptr<Image> tempBookmarkThumbImg_;


	DragVelocityHandler dragVelHandler_;
	bool layoutModeSnappedBackYet_;
	float startLayoutOffsetY_;

	int btnDim_;
	int menuHeight_;
	int btnMargin_;
	int bookmarkTitleHeight_;
	float bookmarksY_;

	boost::scoped_ptr<AppImage> aspectMargin1_, aspectMargin2_;
	bool preserveAspect_;

	TransitionMode transitionMode_;
	PageFlipInterface * pageFlip_;

	Camera camera_;

	bool doBookmarks_;
	bool doAspect_;
	bool resetArticleFirstPage_;
	bool sceneCheck_;
	
	bool toContentsPage_;
	///assume page 1 of article
	int contentsArticleIndex_;

	HomeButtonFunction homeButtonFunction_;

	//use for preDownload
	bool resetComplete_;
	std::deque<Scene *> searchQ_;
};

