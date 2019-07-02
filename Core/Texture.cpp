#include "stdafx.h"
#include "Texture.h"
#include "Global.h"
#include "ImageReader.h"
#include "Exception.h"
#include "GfxRenderer.h"
#include "Reader.h"
#include "Writer.h"
#include "FileUtils.h"
#include "PixelBuffer.h"
#include "Document.h"
#include "DocumentTemplate.h"

#define GL_ETC1_RGB8_OES 0x8d64

#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03

using namespace std;
unsigned int nearestPow2(unsigned int num);

unsigned int Texture::getTextureDataSize(Texture::Format type, int width, int height)
{
	switch(type)
	{
	case Texture::UncompressedRgb16: return width*height*2;
	case Texture::UncompressedRgba16: return width*height*2;
	case Texture::UncompressedRgba32: return width*height*4;
	case Texture::UncompressedA8: return width*height;
	case Texture::UncompressedRgb24: return width*height*3;
	case Texture::CompressionEtc1: return ceil(width/4.0f) * ceil(height/4.0f) * 8;
	case Texture::CompressionEtc1Alpha: return 2 * ceil(width/4.0f) * ceil(height/4.0f) * 8;
	case Texture::CompressionPvrtc: 
		{
			unsigned int uw = nearestPow2(width);
			unsigned int uh = nearestPow2(height);
			if (uw > uh) uh = uw; else uw = uh;
			return max((unsigned int)32, uw * uh * 4 / 8);
		}
	case Texture::CompressionPvrtcAlpha2Bpp:
	{
		unsigned int uw = nearestPow2(width);
		unsigned int uh = nearestPow2(height);
		if (uw > uh) uh = uw; else uw = uh;
		return 
			max((unsigned int)32, uw * uh * 4 / 8) + 
			max((unsigned int)32, uw * uh * 2 / 8);
	}
	case Texture::CompressionPvrtcAlpha4Bpp:
		{
			unsigned int uw = nearestPow2(width);
			unsigned int uh = nearestPow2(height);
			if (uw > uh) uh = uw; else uw = uh;
			return 
				2* max((unsigned int)32, uw * uh * 4 / 8);
		}
	
	default:
		return 0;
	}
}

Texture::Texture(bool forModel)
{	
	texture_ = textureAlpha_ = 0;	
	activeWidth_ = width_ = 0;
	activeHeight_ = height_ = 0;
	loadRequested_ = false;
	requesteZoom_ = false;
    needZoomImage_ = false;
	forModel_ = forModel;
	compression_ = UncompressedRgba32;
}

Texture::~Texture()
{
	uninit();
}

void Texture::init(GfxRenderer * gl)
{
	init(gl, fileName_, compression_);
}

void Texture::init(GfxRenderer * gl, Texture::Format format)
{
	init(gl, fileName_, format);
}

bool Texture::getBitsFromPixelBuffer(PixelBuffer * pixelBuffer)
{
	if (compression_ != UncompressedRgba32)
	{
		compression_ = UncompressedRgba32;
		if (textureAlpha_)
		{
			glDeleteTextures(1, &textureAlpha_);
			textureAlpha_ = 0;
		}
	}
	if (texture_)
	{
		glBindTexture(GL_TEXTURE_2D, texture_);
		activeWidth_ = width_ = pixelBuffer->width();
		activeHeight_ = height_ = pixelBuffer->height();
		genGlTexture(GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer->bits());

		return true;
	}
	return false;
}

bool checkPow2(unsigned val)
{
	return val == 1 || !(val & (val - 1));
}

bool Texture::needSeparateAlpha() const
{
	return 
		compression_ == CompressionEtc1Alpha || 
		compression_ == CompressionPvrtcAlpha2Bpp || 
		compression_ == CompressionPvrtcAlpha4Bpp;
}

/**
texture_ must be 0
width_, height_ must be valid
compression_ must be set properly
*/
void Texture::setupGlTexture(bool mipMapsIfCompressed)
{

	GLuint * textures[] = {&texture_, &textureAlpha_};

	int numTex = 1;
	if (needSeparateAlpha()) numTex = 2;

	bool for3d = forModel_ && width_ > 0 && height_ > 0 &&
		checkPow2((unsigned int)width_) && checkPow2((unsigned int)height_);


	bool isCompressed = !(compression_ >= UncompressedRgb16 && compression_ <= UncompressedRgb24);

	for (int i = 0; i< numTex; ++i)
	{
		glGenTextures(1, textures[i]);	

		//determine if texture is power of 2
		//if so, use GL_REPEAT instead of GL_CLAMP_TO_EDGE
			
		glBindTexture(GL_TEXTURE_2D, *textures[i]);
		if (for3d)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			if (isCompressed && !mipMapsIfCompressed)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			}
			
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		}	
	}	
}

void Texture::genGlTexture(GLint format, GLenum type, GLvoid * pixels)
{
	glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format, type, pixels);
}

bool Texture::isInit() const
{
	return loadRequested_ || texture_;
}

void Texture::init(GfxRenderer * gl, int width, int height, Format format, unsigned char * bits)
{
	fileName_.clear();
	uninit();
	
	activeWidth_ = width_ = width;
	activeHeight_ = height_ = height;
	
	
	compression_ = format;
	setupGlTexture(false);
	if (format == UncompressedRgb16)
	{
		genGlTexture(GL_RGB, GL_UNSIGNED_BYTE, bits);
	}
	else if (format == UncompressedRgba16)
	{
		genGlTexture(GL_RGB, GL_UNSIGNED_SHORT_4_4_4_4, bits);
	}
	else if (format == UncompressedRgba32)
	{
		genGlTexture(GL_RGBA, GL_UNSIGNED_BYTE, bits);
	}
	else if (format == UncompressedA8)
	{
		genGlTexture(GL_ALPHA, GL_UNSIGNED_BYTE, bits);
	}
	else
	{
		throw Exception("Unsupported format");
	}
}

bool Texture::isFullyLoaded() const
{
	bool alphaLoaded = !needSeparateAlpha() || textureAlpha_;
	return ((texture_ != 0 && alphaLoaded) || fileName_.empty()) && !loadRequested_;
}

bool Texture::isLoaded() const
{
	bool alphaLoaded = !needSeparateAlpha() || textureAlpha_; 
	return (texture_ != 0 && alphaLoaded) || fileName_.empty();
}


bool Texture::isEquivalent(const Texture & texture) const
{
	//TODO: maybe take into consideration the texture format
	if (fileName_ == texture.fileName_) return true;
	else return false;
}
void getEtc1MipMap(unsigned int & mipMapChainEtc1Size, unsigned int & numMipMaps, int width, int height, bool doMipMaps);
void getExpandedDims(int & expandedwidth, int & expandedheight, int width, int height);
void gfxLog(const std::string & str);
void Texture::createGlTextureFrom(unsigned char * argBits)
{
	if (compression_ == CompressionEtc1)
	{
		bool mipMaps = (bool)argBits[0];
		unsigned char * bits = argBits + 1;
		
		int expWidth, expHeight;
		getExpandedDims(expWidth, expHeight, width_, height_);	
		unsigned int level0Size = (expWidth/4 * expHeight/4) * 8;

		unsigned int mipMapChainSize;
		unsigned int numMipMaps;
		
		getEtc1MipMap(mipMapChainSize, numMipMaps, width_, height_, mipMaps);
		
		glBindTexture(GL_TEXTURE_2D, texture_);		
		
		int w = width_;
		int h = height_;
		for (unsigned i = 0; i < numMipMaps; ++i)
		{				
			int ew;
			int eh;
			getExpandedDims(ew, eh, w, h);
			unsigned s = ew/4*eh/4*8;
			glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_ETC1_RGB8_OES, w, h, 0, s, bits);
			if (w > 1) w /= 2;
			if (h > 1) h /= 2;
			bits += s;
		}	
		//glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, width_, height_, 0, dataSize, bits);
	}
	else if (compression_ == CompressionEtc1Alpha)
	{
		bool mipMaps = (bool)argBits[0];
		unsigned char * bits = argBits + 1;
		
		int expWidth, expHeight;
		getExpandedDims(expWidth, expHeight, width_, height_);		
		unsigned int level0Size = (expWidth/4 * expHeight/4) * 8;

		unsigned int mipMapChainSize;
		unsigned int numMipMaps;
		getEtc1MipMap(mipMapChainSize, numMipMaps, width_, height_, mipMaps);
		
		glBindTexture(GL_TEXTURE_2D, textureAlpha_);
		int w = width_;
		int h = height_;
		unsigned char * b = bits + mipMapChainSize;
		for (unsigned i = 0; i < numMipMaps; ++i)
		{		
			int ew;
			int eh;
			getExpandedDims(ew, eh, w, h);
			unsigned s = ew/4*eh/4*8;
			glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_ETC1_RGB8_OES, w, h, 0, s, b);
			if (w > 1) w /= 2;
			if (h > 1) h /= 2;
			b += s;
		}
		//glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, width_, height_, 0, dataSize, bits + dataSize);


		glBindTexture(GL_TEXTURE_2D, texture_);	
		w = width_;
		h = height_;
		b = bits;
		for (unsigned i = 0; i < numMipMaps; ++i)
		{		
			int ew;
			int eh;
			getExpandedDims(ew, eh, w, h);
			unsigned s = ew/4*eh/4*8;
			glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_ETC1_RGB8_OES, w, h, 0, s, b);
			if (w > 1) w /= 2;
			if (h > 1) h /= 2;
			b += s;
		}
		//glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, width_, height_, 0, dataSize, bits);		
	}
	else if (compression_ == CompressionPvrtc)
	{
		bool mipMaps = (bool)argBits[0];
		unsigned char * bits = argBits + 1;

		width_ = nearestPow2(activeWidth_);
		height_ = nearestPow2(activeHeight_);
		if (width_ > height_) height_ = width_;
		else width_ = height_;

		GLenum internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;	
		unsigned int dataSize = max(32, width_*height_*4/8);
		glBindTexture(GL_TEXTURE_2D, texture_);		
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0, dataSize, bits);

		//CHECK
		//if (for3d) glGenerateMipmap(GL_TEXTURE_2D);	
	}
	else if (compression_ == CompressionPvrtcAlpha2Bpp || compression_ == CompressionPvrtcAlpha4Bpp)
	{
		bool mipMaps = (bool)argBits[0];
		unsigned char * bits = argBits + 1;

		width_ = nearestPow2(activeWidth_);
		height_ = nearestPow2(activeHeight_);
		if (width_ > height_) height_ = width_;
		else width_ = height_;

		int alphaBpp = 2;
		GLenum alphaInternalFormat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
		if (compression_ == CompressionPvrtcAlpha4Bpp) 
		{
			alphaBpp = 4;
			alphaInternalFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
		}

		GLenum internalFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
		unsigned int dataSize = max(32, width_*height_*4/8);
		unsigned int alphaDataSize = max(32, width_*height_*alphaBpp/8);

		glBindTexture(GL_TEXTURE_2D, textureAlpha_);
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, alphaInternalFormat, width_, height_, 0, alphaDataSize, bits + dataSize);
		//CHECK
		//if (for3d) glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, texture_);		
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0, dataSize, bits);
		//CHECK
		//if (for3d) glGenerateMipmap(GL_TEXTURE_2D);
	}
	else if (compression_ >= UncompressedRgb16 && compression_ <= UncompressedRgb24)
	{		
		glBindTexture(GL_TEXTURE_2D, texture_);
		if (compression_ == UncompressedRgba32)
			genGlTexture(GL_RGBA, GL_UNSIGNED_BYTE, argBits);
		else if (compression_ == UncompressedRgb16)
			genGlTexture(GL_RGB, GL_UNSIGNED_BYTE, argBits);
		else if (compression_ == UncompressedRgba16)
			genGlTexture(GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, argBits);
		else if (compression_ == UncompressedRgb24)
			genGlTexture(GL_RGB, GL_UNSIGNED_BYTE, argBits);
		else if (compression_ == UncompressedA8)
			genGlTexture(GL_ALPHA, GL_UNSIGNED_BYTE, argBits);
	
		bool for3d = forModel_ && width_ > 0 && height_ > 0 &&
			checkPow2((unsigned int)width_) && checkPow2((unsigned int)height_);

		if (for3d) glGenerateMipmap(GL_TEXTURE_2D);	
	}
}

bool Texture::asyncLoadUpdate()
{
	//if (texture_ == 0 && !fileName_.empty())
	if (loadRequested_)
	{
		unsigned char * bits;
		LoadedImage::Type progType;
		int inWidth, inHeight;
		bool isEmpty = false;
		bool loaded = Global::instance().getLoadedImage(this, fileName_, 
			&compression_, &bits, &inWidth, &inHeight, &progType, &isEmpty);
				
		if (loaded)
		{	
			activeWidth_ = width_ = (unsigned short)inWidth;
			activeHeight_ = height_ = (unsigned short)inHeight;
			bool mipmapsIfCompressed;
			if (compression_ >= UncompressedRgb16 && compression_ <= UncompressedRgb24)
				mipmapsIfCompressed = false;
			else
				mipmapsIfCompressed = (bool)bits[0];

			if (!texture_)
			{
				setupGlTexture(mipmapsIfCompressed);
			}

			if (progType == LoadedImage::Full)
			{			
				createGlTextureFrom(bits);
				loadRequested_ = false;
                if (requesteZoom_ && needZoomImage_)
                {
                    Global::instance().requestObjectLoad(this, fileName_, LoadRequest::LoadRequestZoomImage);
                    needZoomImage_ = false;
                    loadRequested_ = true;
                }
			}
			else if (progType == LoadedImage::Progressive1)
			{
				createGlTextureFrom(bits);

				//request load of second part of progressive image
				Global & g = Global::instance();	
				g.requestObjectLoad(this, fileName_, LoadRequest::LoadIRequestmage2);
			}
			else if (progType == LoadedImage::Progressive2)
			{
				createGlTextureFrom(bits);
				loadRequested_ = false;
			}
			else if (progType == LoadedImage::Failed)
			{
				//TODO: handle failure properly
				if (!requesteZoom_)
				{
					//if (texture_) 
					//{
					//	glDeleteTextures(1, &texture_);
					//	texture_ = 0;
					//}
					Global & g = Global::instance();
					//g.requestDeleteFile(fileName_);
					//if (requesteZoom_)
					//{
					//	g.requestObjectLoad(this, fileName_, LoadRequest::LoadRequestZoomImage);
					//}
					//else
					g.requestObjectLoad(this, fileName_, LoadRequest::LoadIRequestmage);
				}
				else //if (requesteZoom_)
				{
					//requesteZoom_ = false;
					Global & g = Global::instance();
					//g.requestDeleteFile(fileName_);
					g.requestObjectLoad(this, fileName_, LoadRequest::LoadRequestZoomImage);
					//g.requestObjectLoad(this, fileName_, LoadRequest::LoadIRequestmage);
				}
			}
			else if (progType == LoadedImage::HasNot)
			{
				//TODO: handle failure properly
				if (!requesteZoom_)
				{
					createGlTextureFrom(bits);
					//g.requestDeleteFile(fileName_);
				}
				loadRequested_ = false;
			}

			free(bits);

		}
		else if (isEmpty)
		{
			Global::instance().requestObjectLoad(this, fileName_, LoadRequest::LoadIRequestmage);
		}
	}

	return texture_ != 0 || fileName_.empty();
}

void Texture::init(GfxRenderer * gl, const std::string & fileName, Texture::Format format)
{
	uninit();	
	fileName_ = fileName;
	compression_ = format;
	Global & g = Global::instance();

	if (fileName.empty()) return;
	
	loadRequested_ = true;
	if (!g.curDocument()->documentTemplate()->isPreDownload())
		g.requestObjectLoad(this, fileName_, LoadRequest::LoadIRequestmage);
	else
		g.requestObjectLoad(this, fileName_, LoadRequest::LoadRequestFullImage);


}

void Texture::setFileName(const std::string & fileName)
{
	Global & g = Global::instance();
	if (loadRequested_)
		g.cancelObjectLoad(this, fileName_);
	fileName_ = fileName;	
	if (loadRequested_)
	{
		if (!g.curDocument()->documentTemplate()->isPreDownload())
			g.requestObjectLoad(this, fileName_, LoadRequest::LoadIRequestmage);
		else
			g.requestObjectLoad(this, fileName_, LoadRequest::LoadRequestFullImage);
	}
}

void Texture::uninit()
{
    needZoomImage_ = false;
	if (loadRequested_)
	{		
		loadRequested_ = false;		
		Global::instance().cancelObjectLoad(this, fileName_);		
	}

	if (texture_) 
	{
		glDeleteTextures(1, &texture_);
		texture_ = 0;
	}

	if (textureAlpha_)
	{
		glDeleteTextures(1, &textureAlpha_);
		textureAlpha_ = 0;
	}
}



void Texture::write(Writer & writer) const
{
	writer.write(fileName_, "filename");
	writer.write(compression_, "format");
}

void Texture::read(Reader & reader, unsigned char version)
{
	reader.read(fileName_);
	reader.read(compression_);
}


void Texture::requestLocalSaveObject(const std::string & fileName)
{
	if (!loadRequested_)
	{
		Global & g = Global::instance();
		if (!g.curDocument()->documentTemplate()->isPreDownload())
			g.requestLocalSaveObject(this, fileName, LoadRequest::LoadIRequestmage);
		else
			g.requestLocalSaveObject(this, fileName, LoadRequest::LoadRequestFullImage);
	}
}

void Texture::requestZoom()
{
	if (!requesteZoom_)
	{
        if (loadRequested_)
            needZoomImage_ = true;
		loadRequested_ = true;
		requesteZoom_ = true;
		Global::instance().requestObjectLoad(this, fileName_, LoadRequest::LoadRequestZoomImage);
	}
}