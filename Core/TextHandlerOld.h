#ifndef TextHandlerOld_h__
#define TextHandlerOld_h__
#include "MathStuff.h"
#include "TextFont.h"
#include <ft2build.h>
#include "TextHandler.h"

#include FT_FREETYPE_H

class Color;
class GfxRenderer;

struct TextProperties;
struct LineProperties;
struct ImageProperties;
//enum TextAlign;

class Texture;

struct CharData_old
{
	short left;
	short top;
	short width;
	short height;
	short advanceX;
	FT_UInt glyphIndex;
	int index;

	short texWidth;
	short texHeight;
};

class StringData_oldElement_old
{	
public:
	StringData_oldElement_old();
	GLuint texture;
	GLuint vertexBuffer;
	GLuint indexBuffer;
	int numIndices;
};

class TextSpan_old
{
public:
	int x;
	int y;
	int width;
};

class StringData_old
{
public:
	~StringData_old();
public:
	std::vector<TextSpan_old> spans;
	Vector2 size;
	std::vector<StringData_oldElement_old> renderData;
};

struct ImageData_old
{
	int x;	//left
	int y;	//top
	int width;	
	int height;
};

///////////////////////////////////////////////////////////////////////////////

class TextHandlerOld
{
public:
	TextHandlerOld(const TextFont & font);
	~TextHandlerOld();

	void init();
	void uninit();

	void addChars(const std::string & str);	

	void drawString(
		GfxRenderer * gl, const StringData_old * str,
		const Color & color, float opacity, const std::vector<ImageProperties> & imgProps);

	const std::string & fontFile() const {return font_.fontFile;}
	int pointSize() const {return font_.pointSize;}
	int pixelSize() const {return charDim_;}

	const bool & isinit() const {return init_;}

	StringData_old * genString(
		GfxRenderer * gl,
		const std::wstring & str, 
		const std::vector<TextProperties> & properties,
		const TextProperties & thisProps,
		const std::vector<LineProperties> & lines,
		std::vector<ImageProperties> & imgProps,
		float width, float height, float letterSpacing, int align);

	bool operator < (const TextHandlerOld & rhs) const
	{
		return font_ < rhs.font_;
	}
	
	const TextFont & font() const {return font_;}

	void genAnyNewChars(const std::vector<TextProperties> & properties,
		const std::wstring & str);

	const CharData_old * charData_old(const wchar_t & c) const
	{
		std::map<wchar_t, CharData_old>::const_iterator iter = charData_old_.find(c);
		if (iter != charData_old_.end())
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
	void resizeText();

	bool asyncLoadUpdate();

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
	int padding_;
	int texPadding_;

	int curCharIndex_;
	unsigned char * imgBuffer_;

	bool boldBaked_;
	bool italicBaked_;

	float underlinePosition_;
	float underlineThickness_;
	float lineHeight_;
	float descender_;	

	std::map<wchar_t, CharData_old> charData_old_;
	
	std::vector<Texture*> imgTextures_;

};

#endif // TextHandlerOld_h__