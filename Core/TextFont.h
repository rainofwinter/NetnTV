#pragma once

class Writer;
class Reader;
class XmlWriter;
class XmlReader;
class ScriptProcessor;

class TextFont
{
public:

	static TextFont fromScriptObject(ScriptProcessor * s, JSObject * obj);

	TextFont()
	{
		//font file is initially empty
		pointSize = 16.0;
		faceIndex = 0;
		italic = false;
		bold = false;
		shadow = false;
		textureScaleSize_ = 1.0;

	}

	std::string fontFile;
	float pointSize;
	int faceIndex;
	bool italic;
	bool bold;
	bool shadow;



	bool operator < (const TextFont & rhs) const
	{
		if (fontFile != rhs.fontFile) return fontFile < rhs.fontFile;
		if (pointSize != rhs.pointSize) return pointSize < rhs.pointSize;
		if (faceIndex != rhs.faceIndex) return faceIndex < rhs.faceIndex;
		if (italic != rhs.italic) return italic < rhs.italic;
		if (shadow != rhs.shadow) return shadow < rhs.shadow;
		if(textureScaleSize_ != rhs.textureScaleSize_) return textureScaleSize_ < rhs.textureScaleSize_;
		return bold < rhs.bold;
	}

	bool operator == (const TextFont & rhs) const
	{
		return 
			fontFile == rhs.fontFile &&
			pointSize == rhs.pointSize &&
			faceIndex == rhs.faceIndex && 
			italic == rhs.italic && shadow == rhs.shadow && 
			bold == rhs.bold && textureScaleSize_ == rhs.textureScaleSize_;
	}

	unsigned char version() const {return 3;}
	void write(Writer & writer) const;
	const char * xmlTag() const {return "TextFont";}
	void writeXml(XmlWriter & w) const;
	void read(Reader & reader, unsigned char);
	void readXml(XmlReader & r, xmlNode * node);

	void setTextureScaleSize(float v){ textureScaleSize_ = v;};
	float getTextureScaleSize(){ return textureScaleSize_;};

private:

	float textureScaleSize_;
};