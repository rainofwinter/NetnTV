#pragma once
#include "Color.h"

class Image;

class PixelBuffer
{
public:
	enum FillMode
	{
		NORMAL,
		XOR,
		ALPHABLEND,
		ALPHACHANNEL,
		//MULTIPLY
	};

public:
	static JSObject * createScriptObjectProto(ScriptProcessor * s, JSObject * global);
	JSObject * getScriptObject(ScriptProcessor * s);

	PixelBuffer(int width, int height);
	PixelBuffer();
	~PixelBuffer();

	void setFillColor(const Color & color);

	void setPenColor(const Color & color);

	bool getPixel(int x, int y, Color * coor);
	bool setPixel(int x, int y, const Color & color);
	
	bool add(const PixelBuffer & rhs);
	bool copyFrom(int lx, int ly, const PixelBuffer & rhs, int rx, int ry, int rwidth, int rheight, FillMode fillMode);	
	void invert();
	int countPixels(const Color & color, int x, int y, int w, int h) const;
	int countPixelsWithAlpha(float a) const;
	void replacePixels(const Color & oldColor, const Color & newColor);

	unsigned char * bits() const {return bits_;}

	int width() const {return width_;}
	int height() const {return height_;}

	void setFillColor();
	void fillRect(int x, int y, int width, int height);
	void fillCircle(int x, int y, int radius);
	void fillCircleCrayon(int x, int y, int radius);
	void fillCircleBrush(int x, int y, int radius);
	void fill();

	void deleteBits();
	void allocateBits(int width, int height);

	bool getBitsFromImage(Image * image);
	void setBits(unsigned char * bits);

private:
	PixelBuffer(const PixelBuffer & rhs);
	PixelBuffer & operator = (const PixelBuffer & rhs);

	bool setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	inline void setPixelPtr(unsigned char *& ptr, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	void fitWithinBuffer(int & rx, int & ry, int & rw, int & rh) const;
private:
	Color fillColor_;
	Color penColor_;


	unsigned char * bits_;
	int width_, height_;

	JSObject * scriptObject_;

};





void PixelBuffer::setPixelPtr(unsigned char *& ptr, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{	
	*(ptr++) = r;
	*(ptr++) = g;
	*(ptr++) = b;
	*(ptr++) = a;	
}