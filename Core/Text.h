#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Color.h"
#include "TextFont.h"

///////////////////////////////////////////////////////////////////////////////
class Texture;
class Mesh;
class Material;

class TextHandlerOld;
class StringData_old;

typedef struct _xmlNode xmlNode;
typedef xmlNode *xmlNodePtr;
typedef struct _xmlAttr xmlAttr;
typedef xmlAttr *xmlAttrPtr;
typedef xmlNodePtr htmlNodePtr;


struct Highlight
{
	unsigned startIndex;
	unsigned endIndex;
	Color color;
};

struct OldTextProperties
{
	enum LineSpacingMode
	{
		LineSpacingAuto = 0,
		LineSpacingCustom
	} lineSpacingMode;
	int lineSpacing;

	int index;
	TextFont font;	
	Color color;
	bool underline;
	std::string link;
	
	unsigned char version() const {return 2;}
	void write(Writer & writer) const;
	void read(Reader & reader, unsigned char);
};

struct TextProperties
{
	enum Type
	{
		PropertyColor = 1 << 0,
		PropertyFont = 1 << 1,
		PropertyFontSize = 1 << 2,
		PropertyFontFileName = 1 << 3
	};

	enum LineSpacingMode
	{
		LineSpacingAuto = 0,
		LineSpacingCustom
	}lineSpacingMode;
	int lineSpacing;
	TextProperties()
	{
		index = 0;
		textHandler = 0;
		underline = false;
	}

	TextProperties(const OldTextProperties & rhs);

	int index;
	TextFont font;	
	Color color;
	bool underline;
	std::string link;

	void assign(const TextProperties & rhs, Type types);

	//This is assigned to after-the-fact and are separate from the
	//above. TODO consider moving this to another data structure
	TextHandlerOld * textHandler;

	bool operator < (const TextProperties & rhs) const
	{
		return index < rhs.index;
	}

	bool operator == (const TextProperties & rhs) const
	{
		return 
			font == rhs.font && 
			color == rhs.color && 
			underline == rhs.underline && 
			link == rhs.link;
	}

	unsigned char version() const {return 3;}
	void write(Writer & writer) const;
	const char * xmlTag() const {return "TextProperties";}
	void writeXml(XmlWriter & w) const;
	void read(Reader & reader, unsigned char);
	void readXml(XmlReader & r, xmlNode * node);
};

struct LineProperties
{
	int index;
	int toNextLine;
	int ascenderHeight;
	int descender;
};

struct ImageProperties
{
	int index;
	int width;
	int height;
	std::string fileName;
	int x;
	int y;
};

//keep track of where to insert newlines to create the proper word wrapping 
//effect
struct WordWrapPoint
{
	int wrapAtIndex;
};

struct textMemoIndex
{	
	int startIndex;
	int endIndex;
};

////////////////////////////////////////////////////////////////////////////////

class Text : public SceneObject
{
public:
	enum LineSpacingMode
	{
		LineSpacingAuto = 0,
		LineSpacingCustom
	};

	enum TextAlign
	{
		Left = 0,
		Center = 1,
		Right = 2
	};
	
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "Text";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = 
			sUuidGen_("90210DF2-4D37-2799-1A12-56D53733872F");
		return uuid;
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new Text(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	Text();
	Text(const Text & rhs, ElementMapping * elementMapping);

	~Text();


	virtual void setTextString(const std::wstring & text);
	virtual const std::wstring & textString() const {return text_;}

	void insertText(const std::wstring & text, const std::vector<TextProperties> & props, unsigned index);
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
	

	virtual Text * text() {return this;}

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);

	int toNextLine(		
		TextHandlerOld * handler, const TextProperties & props);

	static int lineHeight(		
		TextHandlerOld * handler, const TextProperties & props);

	///cheat function to allow Init of this object even when its not parented
	///under a parentScene
	void init(GfxRenderer * gl, Scene * parentScene, bool firstTime);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;

	virtual bool pressEvent(const Vector2 & startPos, int pressId);
	virtual bool releaseEvent(const Vector2 & pos, int pressId);
	virtual bool clickEvent(const Vector2 & mousePos, int pressId);
	int indexAtPos(const Vector2 & mousePos, int * propertyIndex) const;
	bool posAtIndex(int index, Vector2 * posTop, int * posHeight, int * posWidth) const;

	virtual unsigned char version() const {return 8;}
	virtual void write(Writer & writer) const;
	virtual const char * xmlTag() const {return "Text";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void read(Reader & reader, unsigned char);
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual bool asyncLoadUpdate();

	float width() const;
	float height() const;

	//const TextFont & font() const {return properties_.front().font;}
	void setFont(const TextFont & font);

	const TextFont & font() const {return properties_.front().font;}
	
	const std::vector<TextProperties> & properties() const {return properties_;}
	void setProperties(const std::vector<TextProperties> & properties)
		{properties_ = properties;}

	void applyProperties(const std::vector<TextProperties> & localIndexProperties,
		unsigned startIndex, unsigned endIndex);

	void applyProperties(const TextProperties & properties, 
		TextProperties::Type types, unsigned charLen = UINT_MAX);

	void mergeProperties();

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	/**
	@param index character index
	*/
	bool lineDataForChar(		
		int index, int * lineY, int * lineIndex, 
		int * lineStartIndex, int * lineEndIndex) const;
	/**
	@param pos this has to be in local text object coords
	*/
	bool lineDataForPos(
		const Vector2 & pos, int * lineY, 
		int * lineIndex, int * lineStartIndex, int * lineEndIndex) const;

	bool lineDataForLine(int lineIndex, 
		int * lineY, int * lineWidth, int * lineHeight, 
		int * lineStartIndex, int * lineEndIndex) const;
	
	int startingPropertyIndexForLine(int lineIndex) const;

	int numLines() const {return (int)lines_.size();}

	const int & letterSpacing() const;
	void setLetterSpacing(const int & letterSpacing);

	const int & lineSpacing() const {return lineSpacing_;}
	void setLineSpacing(const int & lineSpacing) {lineSpacing_ = lineSpacing;}

	const LineSpacingMode & lineSpacingMode() const {return lineSpacingMode_;}

	void setLineSpacingMode(const LineSpacingMode & lineSpacingMode) 
	{
		lineSpacingMode_ = lineSpacingMode;
	}

	const TextAlign & textAlign() const {return textAlign_;}
	void setTextAlign(const TextAlign & textAlign) {textAlign_ = textAlign;}
	bool lineLength(int lineIndex, float * lineLength) const;

	const bool & canSelectPart() const {return canSelectPart_;}
	void setCanSelectPart(const bool & var) {canSelectPart_ = var;}

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

	void getProperties(std::vector<TextProperties> & properties, 
		unsigned startIndex, unsigned endIndex, bool localIndices) const;

	void getProperties(unsigned & propStartIndex, unsigned & propEndIndex, 
		unsigned startIndex, unsigned endIndex) const;

	void addImageProperties(ImageProperties prop);

	void setMemos(std::vector<textMemoIndex> memos, Scene* curScene, unsigned char * bits, GfxRenderer *gl, int width, int height);
	void addMemo(int startIndex, int endIndex, Scene* curScene);
	void deleteMemo(int startIndex, int endIndex);
	void setMemoIconSize(float w, float h) { memoW_ = w; memoH_ = h; }
	void setDocSize(float w, float h) { dw_ = w, dh_ = h; }
	void drawMemoIcon(GfxRenderer * gl) const;

private:
	Text(const Text & rhs);
	Text & operator = (const Text & rhs);
	void create();

	void computeWordWrap(
		std::vector<WordWrapPoint> * wordWrapPoints,
		const std::wstring & str,
		const std::vector<TextProperties> & properties,
		int width, int height);

	void computeLines(
		std::vector<LineProperties> * lines,
		const std::wstring & str,
		const std::vector<TextProperties> & properties,
		const std::vector<WordWrapPoint> & wordWrapPoints);

	///ensure there is a TextProperties at the startIndex, endIndex borders	
	void makeBorderProperties(unsigned startIndex, unsigned endindex);
/*
	void processLink(htmlNodePtr node, xmlAttrPtr attr,
		std::string * str, 
		std::vector<TextProperties>::iterator & iter);

	void processMarkup(
		std::string * str, 
		std::vector<TextProperties>::iterator & iter, 
		htmlNodePtr element);
*/
	Vector2 processCoords(const Vector2 & globalDevicePos) const;

private:
	std::wstring text_;	
	std::vector<TextProperties> properties_;

	///post markup
	/*
	std::wstring noMarkupText_;
	std::vector<TextProperties> postMarkupProperties_;
	*/
	std::vector<LineProperties> lines_;

	struct TextData
	{
		TextHandlerOld * textHandler;
		Color color;
		bool underline;

		bool operator < (const TextData & rhs) const
		{
			if (textHandler != rhs.textHandler)
				return textHandler < rhs.textHandler;
			
			if (underline != rhs.underline)
				return underline < rhs.underline;

			return color < rhs.color;
		}
	};
	std::map<TextData, StringData_old *> textMap_;

	VisualAttrib visualAttrib_;	

	float boundaryWidth_;
	float boundaryHeight_;

	TextAlign textAlign_;
	int letterSpacing_;
	LineSpacingMode lineSpacingMode_;
	int lineSpacing_;

	bool canSelectPart_;
	bool isSelectedPart_;

	int charStartIndex_, charEndIndex_;

	std::vector<Highlight> highlights_;

	std::vector<ImageProperties> imgProps_;

	//memo
	std::vector<textMemoIndex> memos_;
	std::vector<Vector2> drawMemoCoords_;
	TextureSPtr memoTex_;
	float memoW_, memoH_;
	float dw_, dh_;
};