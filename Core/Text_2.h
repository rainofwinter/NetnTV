#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Color.h"
#include "TextFont.h"
#include "Text.h"

///////////////////////////////////////////////////////////////////////////////
class Texture;
class Mesh;
class Material;
class TextSpan;

class TextHandler;
class StringData;

#define IMAGE_OBJECT_STRING 0xfffc
#define MAX_INCREASEMENT_SIZE 60
#define TAB_SIZE 96
#define TAB_LEVEL_SIZE 18
#define ENTER_SIZE 5

typedef enum Object_Properties_Type{
	UNKNOWN_OBJECT = 0,
	IMAGE_OBJECT = 1
}Object_Properties_Type;

#define BASE_Object_Properties    \
	Object_Properties_Type property_type;\
	float width;\
	float height;\
	float x;\
	float y;\
	Texture * texture;\
	bool bMakeTexture;
	

struct ObjectProperties{
	BASE_Object_Properties;
};

struct ImageObjectProperties
{
	BASE_Object_Properties;

	std::string fileName;
	
};


struct LineDrawRectInfo{
	float x;
	float y;

	float width;
	Color color;
};
struct UnderlineInfo{
	
	float avglineY;
	float avglineboder;
	
	std::vector<LineDrawRectInfo> rectInfos;
	int spancount;
};

struct CanclelineInfo{
	float avglineY;
	float avglineboder;

	std::vector<LineDrawRectInfo> rectInfos;
	int spancount;
};


class TextProperties_2
{
public:
	enum Type
	{
		PropertyColor = 1 << 0,
		PropertyFont = 1 << 1,
		PropertyFontSize = 1 << 2,
		PropertyFontFileName = 1 << 3,
		PropertyUnderLine = 1 << 4,
		PropertyCancleLine = 1 << 5,
		PropertyLetterWidthRatio = 1 << 6,
		PropertyFontShandow = 1 << 7
	};

	enum LineSpacingMode
	{
		LineSpacingAuto = 0,
		LineSpacingCustom
	}lineSpacingMode;


	int lineSpacing;

	TextProperties_2()
	{
		index = 0;
		textHandler = 0;
		underline_ = false;
		cancleline_ = false;
		bObject = false;
		letterwidthratio_ = 100.0f;
		object_type = UNKNOWN_OBJECT;
	}

	TextProperties_2(const TextProperties_2 & rhs, bool bCopyObjectProp);
	
	~TextProperties_2();

	int index;
	TextFont font;
	
	float letterwidthratio_;

	Color color;
	bool underline_;
	bool cancleline_;

	std::string link;

	void assign(const TextProperties_2 & rhs, Type types);

	//This is assigned to after-the-fact and are separate from the
	//above. TODO consider moving this to another data structure
	TextHandler * textHandler;

	bool operator < (const TextProperties_2 & rhs) const
	{
		return index < rhs.index;
	}

	bool operator == (const TextProperties_2 & rhs) const
	{
		if(bObject == true || ((TextProperties_2 *)&rhs)->isObjectProperty() == true)
			return false;

		return font == rhs.font && color == rhs.color && underline_ == rhs.underline_ && cancleline_ == rhs.cancleline_ &&  link == rhs.link && letterwidthratio_ == rhs.letterwidthratio_;
	}

	unsigned char version() const {return 0;}
	void write(Writer & writer) const;
	const char * xmlTag() const {return "TextProperties_2";}
	void writeXml(XmlWriter & w) const;
	void read(Reader & reader, unsigned char);
	void readXml(XmlReader & r, xmlNode * node);

	void SetObjectProperties(ObjectProperties *pSrcobject);

	bool isObjectProperty(){
		return bObject;
	}

	const ObjectProperties *GetObjectProperty(){
		ObjectProperties *rv = NULL;

		if(!bObject)
			return NULL;

		if(object_type == IMAGE_OBJECT)
			rv = (ObjectProperties *)&imageObject_;
		return rv;
	}

	void ResetObjectProperties(){
		bObject = false;
		object_type = UNKNOWN_OBJECT;
	}

private:
	ImageObjectProperties imageObject_;

	Object_Properties_Type object_type;
	bool bObject;
};

struct ObjectPadding{
	int left;
	int top;
	int right;
	int bottom;

	bool operator == (const ObjectPadding & rhs) const
	{
		return left == rhs.left &&
			top == rhs.top &&
			right == rhs.right &&
			bottom == rhs.bottom;
	}
};

enum LineIncreasementType{
	NONE, 
	NUMBER = L'1',
	LOWERER_CASE_ALPHABET = L'a',
	UPPER_CASE_ALPHABET = L'A',
	CIRCLE_NUMBER = 9312,
	CIRCLE_ALPHABET = 9424, 
	BRACKET_NUMBER = 9332,
	BRACKET_ALPHABET = 9372,
	EXCLAMATION_POINT = 8251,
	RECTANGLE = 9632,
	TRIAGLE_1 = 9650,
	TRIAGLE_2 = 9654,
	STAR = 9733,
};

struct LineIncreasement{
	int index;

	int depth;

	LineIncreasementType type;
	TextProperties_2 prop;
	std::wstring wstrIndex;

	bool operator == (const LineIncreasement & rhs) const
	{
		return index == rhs.index &&
			depth == rhs.depth &&
			type == rhs.type &&
			wstrIndex == rhs.wstrIndex &&
			prop == rhs.prop;
	}
};


class LineStyle
{
public:
		
	static int getLineIncreasementIndex(LineIncreasementType &type);

	enum Type
	{
		LineStyleIncleasement = 0,
		LineStyleAlign,
		LineStyleTap,
		LineStyleLetterSpacing
	};

	enum TextAlign
	{
		Left = 0,
		Center = 1,
		Right = 2,
		Justify_Left = 3
	};

	LineStyle(){
		index_ = 0;
		tabIndex_ = 0;

		padding_.top = 0;
		padding_.bottom = 0;
		padding_.left = 0;
		padding_.right = 0;

		lineIncreasement_.type = NONE;
		lineIncreasement_.depth = 0;
		lineIncreasement_.index = 0;
		textAlign_ = LineStyle::Left;
		lineIncreasement_.wstrIndex = L"";
		charIndex = 0;
		startPosX = 0;
		letterSpacing_ = 0;
	};

	LineStyle(LineStyle *sibling){

		index_ = sibling->index_ + 1;
		tabIndex_ = sibling->tabIndex_;
		lineIncreasement_.type = sibling->lineIncreasement_.type;
		padding_ = sibling->padding_;
		textAlign_ = sibling->textAlign_;

		if(lineIncreasement_.type != NONE){

			lineIncreasement_.index = sibling->lineIncreasement_.index + 1;
			lineIncreasement_.depth = sibling->lineIncreasement_.depth;

		}else{
			lineIncreasement_.index = 0;
			lineIncreasement_.depth = 0;
		}

		letterSpacing_ = sibling->letterSpacing_;
	};

	~LineStyle(){};

	bool operator == (const LineStyle & rhs) const
	{
		return index_ == rhs.index_ &&
			tabIndex_ == rhs.tabIndex_ &&
			charIndex == rhs.charIndex &&
			startPosX == rhs.startPosX &&
			textAlign_ == rhs.textAlign_ &&
			letterSpacing_ == rhs.letterSpacing_ &&
			padding_ == rhs.padding_ &&
			lineIncreasement_ == rhs.lineIncreasement_;
	}

	int index_;
	int tabIndex_;
	int charIndex;
	float startPosX;
	ObjectPadding padding_;

	LineIncreasement lineIncreasement_;

	TextAlign textAlign_;
	float letterSpacing_;
};

struct WordWrapPoint_2
{
	int wrapAtIndex;
	float wrapWidth;
	float toNextLine;
	float ascenderHeight;
	float descender;
	float nStartx;
};

////////////////////////////////////////////////////////////////////////////////

struct WordInfo{
	std::wstring text;
	float left;
	float right;
	float top;
	float bottom;

	bool hasWord;
};

class Text_2 : public SceneObject
{
public:
	enum LineSpacingMode
	{
		LineSpacingAuto = 0,
		LineSpacingCustom
	};

	enum TextDirection{
		DRight = 0,DLeft,DDown,DUp
	};	
	
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "TextObject";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = 
			sUuidGen_("7fc97e80-b102-11e4-ab27-0800200c9a66");
		return uuid;
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new Text_2(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	Text_2();
	Text_2(const Text_2 & rhs, ElementMapping * elementMapping);

	~Text_2();


	void setTextString(const std::string & text);
	
	virtual void setTextString(const std::wstring & text);
	virtual const std::wstring & textString() const {return text_;}

	void insertText(const std::wstring & text, const std::vector<TextProperties_2> & props, unsigned index);
	void removeText(unsigned startIndex, unsigned endIndex);

	const Color & color() const {return properties_.front().color;}
	void setColor(const Color & color)
	{
		properties_.front().color = color;
	}

	const float & boundaryWidth() const {return boundaryWidth_;}
	void setBoundaryWidth(const float & width) {boundaryWidth_ = width;}

	const float & boundaryHeight() const {return boundaryHeight_;}
	void setBoundaryHeight(const float & height) {boundaryHeight_ = height;}
	

	virtual Text_2 * text_2() {return this;}

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);

	float toNextLine(		
		TextHandler * handler, const TextProperties_2 & props);

	static int lineHeight(		
		TextHandler * handler, const TextProperties_2 & props);

	///cheat function to allow Init of this object even when its not parented
	///under a parentScene
	void init(GfxRenderer * gl, Scene * parentScene, bool firstTime);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;

	virtual bool pressEvent(const Vector2 & startPos, int pressId);
	//virtual bool releaseEvent(const Vector2 & pos, int pressId);
	virtual bool clickEvent(const Vector2 & mousePos, int pressId);
	int indexAtPos(const Vector2 & mousePos, int * propertyIndex) const;
	bool posAtIndex(int index, Vector2 * posTop, float * posHeight, float * posWidth) const;

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual const char * xmlTag() const {return "Text_2";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void read(Reader & reader, unsigned char);
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual bool asyncLoadUpdate();

	void setFont(const TextFont & font);

	const TextFont & font() const {return properties_.front().font;}
	
	const std::vector<TextProperties_2> & properties() const {return properties_;}
	void setProperties(const std::vector<TextProperties_2> & properties)
		{properties_ = properties;}

	void applyProperties(const std::vector<TextProperties_2> & localIndexProperties,
		unsigned startIndex, unsigned endIndex);

	void applyProperties(const TextProperties_2 & properties, 
		TextProperties_2::Type types, unsigned charLen = UINT_MAX);

	void mergeProperties();

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	/**
	@param index character index
	*/
	bool lineDataForChar(		
		int index, float * lineY, int * lineIndex, 
		int * lineStartIndex, int * lineEndIndex) const;
	/**
	@param pos this has to be in local text object coords
	*/
	bool lineDataForPos(
		const Vector2 & pos, int * lineY, 
		int * lineIndex, int * lineStartIndex, int * lineEndIndex) const;

	bool lineDataForLine(int lineIndex, 
		float * lineY, float * lineWidth, float * lineHeight, 
		int * lineStartIndex, int * lineEndIndex) const;
	
	int startingPropertyIndexForLine(int lineIndex) const;

	int numLines() const {return (int)wordWrapPoints_.size();}

	const float & letterSpacing() const;
	void setLetterSpacing(const float & letterSpacing);

	const float & lineSpacing() const {return lineSpacing_;}
	void setLineSpacing(const float & lineSpacing) {lineSpacing_ = lineSpacing;}

	const LineSpacingMode & lineSpacingMode() const {return lineSpacingMode_;}

	void setLineSpacingMode(const LineSpacingMode & lineSpacingMode) 
	{
		lineSpacingMode_ = lineSpacingMode;
	}/*

	const TextAlign & textAlign() const {return textAlign_;}
	void setTextAlign(const TextAlign & textAlign) {textAlign_ = textAlign;}*/
	bool lineLength(int lineIndex, float * lineLength) const;

	const bool & canSelectPart() const {return canSelectPart_;}	
	void setCanSelectPart(const bool & var) {canSelectPart_ = var;}

	const int & textDirection() const {return direction_;}
	void setTextDirection(const int & var) {direction_ = (TextDirection)var;}

	void clearTextSelect() {isSelectedPart_ = false;}
	bool isSelectedPart() {return isSelectedPart_;}
	void addHighlight(const Color & color);
	void addHighlight(const Highlight & highlight);
	void removeHighlight(unsigned int startIndex, unsigned int endIndex);

	const std::vector<Highlight> & highlights() const {return highlights_;}
	void drawHighlight(GfxRenderer * gl, const Highlight & highlight) const;
	Vector2 selectedPartPos();
	std::wstring  textPartString(unsigned startIndex, unsigned endIndex);

	Vector2 toLocalPos(const Vector2 & pos);
	void selectPart(int charIndex, int charEndIndex);
	void selectPart(int & startIndex, int & endIndex, int charIndex);

	void search(std::vector<unsigned> & foundIndices, const std::wstring & searchStr) const;

	void getProperties(std::vector<TextProperties_2> & properties, 
		unsigned startIndex, unsigned endIndex, bool localIndices) const;

	void getProperties(unsigned & propStartIndex, unsigned & propEndIndex, 
		unsigned startIndex, unsigned endIndex) const;

	Texture *addImageTexture(std::string imageName);

	void applyLineStyle(const LineStyle &linestyle, LineStyle::Type types, unsigned int lineLen = UINT_MAX);

	void addLineStyle(int position);
	
	void addLineStyle(LineStyle &lineStyle, int position);

	void addLineStyle(const std::vector<LineStyle> &linestyles, int position);

	void delLineStyle(int nStart , int nEnd);
	
	const std::vector<LineStyle> & getLinesStyle() const{ return linesStyle_;};

	void setLinesStyle(const std::vector<LineStyle> &linesStyle){
		linesStyle_ = linesStyle;
	};

	void getLinesStyle(std::vector<LineStyle> & properties, unsigned startIndex, unsigned endIndex, bool localIndices) const;

	int getFirstCharIndexByLine(int lineNum);
	const LineStyle &getLineStyleByCharIndex(int index) const;
	const WordWrapPoint_2 &getLineByIndex(int index) const;

	float getDisplayTextWidth(){ return nDisplayW_;};


	const float width() const {return nDisplayW_;}
	void setWidth(const float & width) {boundaryWidth_ = width;}

	const float height() const {return nDisplayH_;}
	void setHeight(const float & height) {boundaryHeight_ = height;}
	static const std::string &getBaseFontFilePath(){return strBaseFontFilePath;};
	const float &getRenderStringHeight();

	WordInfo getPosWordInfo(float x, float y);

	Vector2 selectWidth(int start, int end);

	void setMemos(std::vector<textMemoIndex> memos, Scene* curScene, unsigned char * bits, GfxRenderer *gl, int width, int height);
	void addMemo(int startIndex, int endIndex, Scene* curScene);
	void deleteMemo(int startIndex, int endIndex);
	void setMemoIconSize(float w, float h) { memoW_ = w; memoH_ = h; }
	void setDocSize(float w, float h) { dw_ = w, dh_ = h; }
	void drawMemoIcon(GfxRenderer * gl) const;

	void convertText_2(Text* orgText);
private:
	Text_2(const Text_2 & rhs);
	Text_2 & operator = (const Text_2 & rhs);
	void create();

	void computeWordWrap(
		std::vector<WordWrapPoint_2> * wordWrapPoints,
		const std::wstring & str,
		const std::vector<TextProperties_2> & properties, const std::vector<LineStyle> & linesStyle,
		int width, int height);

	void makeBorderProperties(unsigned startIndex, unsigned endindex);

	Vector2 processCoords(const Vector2 & globalDevicePos) const;

private:
	std::wstring text_;	
	std::vector<TextProperties_2> properties_;

	TextDirection direction_;

	std::vector<WordWrapPoint_2> wordWrapPoints_;

	struct TextData
	{
		TextHandler * textHandler;
		Color color;
		bool underline;
		bool cancleline;
		bool shadow;
		float letterwidthratio;

		bool operator < (const TextData & rhs) const
		{
			if (textHandler != rhs.textHandler)
				return textHandler < rhs.textHandler;
			
			if (underline != rhs.underline)
				return underline < rhs.underline;

			if (cancleline != rhs.cancleline)
				return cancleline < rhs.cancleline;

			if (shadow != rhs.shadow)
				return shadow < rhs.shadow;

			if(letterwidthratio != rhs.letterwidthratio)
				return letterwidthratio < rhs.letterwidthratio;

			return color < rhs.color;
		}
	};
	std::map<TextData, StringData *> textMap_;
	std::map<std::string, Texture *> imageTextureMap_;
		
	VisualAttrib visualAttrib_;	

	float boundaryWidth_;
	float boundaryHeight_;

	float letterSpacing_;
	LineSpacingMode lineSpacingMode_;
	float lineSpacing_;

	bool canSelectPart_;
	bool isSelectedPart_;

	int charStartIndex_, charEndIndex_;

	std::vector<Highlight> highlights_;

	std::vector<LineStyle> linesStyle_;

	std::vector<UnderlineInfo> underlineInfos_;
	std::vector<CanclelineInfo> canclelineInfo_;

	

	void ReconstructLinStyle(GfxRenderer * gl);

	static std::string const strBaseFontFilePath;
	
	float nDisplayW_;
	float nDisplayH_;

	//memo
	std::vector<textMemoIndex> memos_;
	std::vector<Vector2> drawMemoCoords_;
	TextureSPtr memoTex_;
	float memoW_, memoH_;
	float dw_, dh_;
};