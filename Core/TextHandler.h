#ifndef TextHandler_h__
#define TextHandler_h__
#include "MathStuff.h"
#include "TextFont.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Color.h"

class GfxRenderer;
class TextHandler;
class TextProperties_2;
class LineStyle;
struct WordWrapPoint_2;

struct WordWrapPoint;
struct TextProperties;
struct LineProperties;
struct ImageProperties;
//enum TextAlign;

class Texture;

struct CharData
{
	float left;
	float top;
	float width;
	float height;
	float advanceX;
	FT_UInt glyphIndex;
	int index;

	float texWidth;
	float texHeight;
};

class StringDataElement
{	
public:
	StringDataElement();
	GLuint texture;
	GLuint vertexBuffer;
	GLuint indexBuffer;
	int numIndices;
};

struct TextSpan
{
	int index;
	bool underline;
	bool cancleline;
	int lineIndex;
	TextHandler *texture;
	float x;
	float y;
	float width;
	Color color;

	bool operator<  ( const TextSpan& rhs ) const
	{
		return index < rhs.index;
	}
};

class StringData
{
public:
	~StringData();
public:
	std::vector<TextSpan> spans;
	Vector2 size;
	std::vector<StringDataElement> renderData;
};

struct ImageData
{
	int x;	//left
	int y;	//top
	int width;	
	int height;
};

int pointsToPixels(int pointSize);
void ApplyBold(FT_GlyphSlot  slot);

struct VertexStruct
{
	VertexStruct(const Vector3 & pos, const Vector2 & texCoord)
	{
		this->position = pos;
		this->texCoord = texCoord;
	}
	Vector3 position;
	Vector2 texCoord;
};

///////////////////////////////////////////////////////////////////////////////

class TextHandler
{
public:
	TextHandler(const TextFont & font);
	~TextHandler();

	void init();
	void uninit();

	void addChars(const std::string & str);	

	void drawString(
		GfxRenderer * gl, const StringData * str,
		const Color & color, float opacity);

	const std::string & fontFile() const {return font_.fontFile;}
	int pointSize() const {return font_.pointSize;}
	int pixelSize() const {return charDim_;}

	const bool & isinit() const {return init_;}

	StringData * genString(
		GfxRenderer * gl,
		const std::wstring & str, 
		int direction,
		const std::vector<TextProperties_2> & properties,
		const TextProperties_2 & thisProps,
		const std::vector<WordWrapPoint_2> & lines,
		const std::vector<LineStyle> & linesStyle,
		float width, float height, float letterSpacing);//new

	bool operator < (const TextHandler & rhs) const
	{
		return font_ < rhs.font_;
	}
	
	const TextFont & font() const {return font_;}

	void genAnyNewChars(const std::vector<TextProperties_2> & properties,
		const std::wstring & str);//new

	void genAnyNewChars(const std::wstring & str);

	const CharData * charData(const wchar_t & c) const
	{
		std::map<wchar_t, CharData>::const_iterator iter = charData_.find(c);
		if (iter != charData_.end())
		{
			return &(*iter).second;
		}
		else
		{
			return 0;
		}
	}

	FT_Face ftFace() const {return ftFace_;}

	const float & underlinePosition() const {return underlinePosition_;}
	const float & underlineThickness() const {return underlineThickness_;}
	const float & toNextLine() const {return lineHeight_;}
	const float & descender() const {return descender_;}

	void setMagnification(float mag);
	float getMagnification();
	void resizeText();

	bool asyncLoadUpdate();

	const unsigned char * imgBuffer(){return imgBuffer_;};
	int textureDim() {return textureDim_;};
	int charDim() {return charDim_;};
	int texDim() {return texDim_;};
	int numCharsPerTextureRow(){ return numCharsPerTextureRow_;};
	int numCharsPerTexture() {return numCharsPerTexture_;};
	float padding(){return padding_; };
	float texPadding(){return texPadding_;};

private:
	

	void create();
	
	void genChars(const std::set<wchar_t> & chars);
	void genChar(wchar_t c, unsigned int index);

	bool init_;
	FT_Face ftFace_;


	TextFont font_;	
	std::vector<GLuint> textures_;
	int textureDim_;
	int charDim_;
	int texDim_;
	int numCharsPerTextureRow_;
	int numCharsPerTexture_;
	std::set<wchar_t> chars_;
	float padding_;
	float texPadding_;

	int curCharIndex_;
	unsigned char * imgBuffer_;

	bool boldBaked_;
	bool italicBaked_;

	float underlinePosition_;
	float underlineThickness_;
	float lineHeight_;
	float descender_;	

	std::map<wchar_t, CharData> charData_;
	
	std::vector<Texture*> imgTextures_;

};

#endif // TextHandler_h__