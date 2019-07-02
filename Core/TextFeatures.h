#pragma once
#include "Types.h"
#include "MathStuff.h"
#include "Color.h"

class Document;
class Text;

class GfxRenderer;
typedef struct _xmlNode xmlNode;

struct Memo
{
	Scene * getScene() const;
	unsigned int memoIndex;
	unsigned int charIndex;
	unsigned int endIndex;
	SceneObject * textObj;
	///UTF-8
	std::string memo; 
};

typedef boost::shared_ptr<Memo> MemoSPtr;
typedef std::multimap<SceneObject *, MemoSPtr> MemoMap;


struct Highlight;
typedef std::multimap<SceneObject *, Highlight> HighlightMap;

struct TFSearchResult
{
	SceneObject * text;
	std::vector<unsigned> indices;
};

struct TextFeatures
{
public:
	TextFeatures(Document * document);
	~TextFeatures();

	bool isCurrentScene(Scene *) const;

	void show(bool val);
	void uninit();

	void registerDrawTextObj(SceneObject * text);

	void startTextSel(SceneObject * selTextObj, int textSelStartIndex, int textSelEndIndex);	
	SceneObject * selTextObj(int * startIndex, int * endIndex) const;
	Vector2 selectedTextPos() const;
	bool pressEvent(const Vector2 & pos, int pressId);
	bool moveEvent(const Vector2 & pos, int pressId);	
	bool releaseEvent(const Vector2 & pos, int pressId, bool isClick);

	void selectPart(SceneObject * text, unsigned startIndex, unsigned endIndex);
	void clearTextSelect();


	void setMemo(Memo * memo, const std::string & utf8Str);
	Memo * curMemo() const;	
	void getMemos(std::vector<Memo *> & memos) const;
	void deleteMemo(Memo * memo);
	void showText(SceneObject * text, unsigned charIndex);


	void search(std::vector<TFSearchResult> & searchResults, const std::string & utf8Str) const;

	void addHighlight(const Color & color);
	void removeHighlight();

	void beginDraw();
	void draw();

	bool saveXml(const std::string & fileName);
	bool loadXml(const std::string & fileName);
	bool loadXmlMemo(
		std::map<unsigned, std::vector<SceneObject *> > & texts, 
		xmlNode * memoNode);
	bool loadXmlHighlight(
		std::map<unsigned, std::vector<SceneObject *> > & texts, 
		xmlNode * hiliteNode);

	bool update(float curTime);

	std::string selectedText() const;

	void setSelPos(Vector2 pos) {selPos_ = pos;}

private:
	void computeMemoCoords();
	
	Vector2 toDocPos(SceneObject * obj, const Vector2 & coords) const;
	Vector2 toLocalScenePos(const SceneObject * obj, const Vector2 & docPos) const;

	void textSelHandleCoords();
	bool doTextSelHandleDrag(int selHandleIndex, const Vector2 & mousePos);
	
	bool isTouchingTextSelHandle(int index, const Vector2 & coords);
	void defaultPressHandleDelta();

	bool memoClick(const Vector2 & docPos, Ray mouseRay, Scene * scene);
private:
	
	//selection
	enum TextSelMode
	{
		Unselected,
		Pressed,
		Selecting

	}selMode;	
	
	float startSelTime_;
	Vector2 startSelPos_, selPos_;

	SceneObject * textObj_;
	TextureSPtr handleTex_;
	int startIndex_, endIndex_;
	int pressedHandle_;
	float handleW_, handleH_;

	Vector2 handle0Pos_, handle1Pos_;
	bool handle0ReverseX_, handle1ReverseX_;
	Vector2 pressHandleDelta_;

	//memos
	std::set<SceneObject *> drawTexts_;
	std::vector<Memo *> drawMemos_;
	std::vector<Vector2> drawMemoCoords_;	
	MemoMap memos_;
	TextureSPtr memoTex_;
	Memo * pressedMemo_;
	float memoW_, memoH_;
	unsigned curMemoIndex_;	

	

	//highlights
	HighlightMap highlights_;

	static const unsigned int cMagicNumber_ = 23982;

	Document * document_;

	bool visible_;

	bool newlySelected_;
};