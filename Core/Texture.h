#pragma once
class ImageReader;
class GfxRenderer;
class Reader;
class Writer;
class PixelBuffer;

class Texture
{
	friend class GfxRenderer;
	friend class Material;
	friend class TextureRenderer;
public:
	enum Format
	{		
		UncompressedRgb16 = 0,
		UncompressedRgba16,
		UncompressedRgba32,		
		UncompressedA8,
		UncompressedRgb24,		
		CompressionEtc1,
		CompressionEtc1Alpha,
		CompressionPvrtc,
		CompressionPvrtcAlpha2Bpp,
		CompressionPvrtcAlpha4Bpp
	};

	static unsigned int getTextureDataSize(Texture::Format type, int width, int height);
public:
	/**
	@param forModel true if texture is intended for use on a 3d model.
	False if it's intended for use on 2d elements
	*/
	Texture(bool forModel = false);
	~Texture();

	const std::string & fileName() const {return fileName_;}
	void setFileName(const std::string & fileName);

	unsigned short width() const {return width_;}
	unsigned short height() const {return height_;}

	unsigned short activeWidth() const {return activeWidth_;}
	unsigned short activeHeight() const {return activeHeight_;}

	unsigned char * bits() const;

	void init(GfxRenderer * gl);
	void init(GfxRenderer * gl, Format format);
	void init(GfxRenderer * gl, int width, int height, Format format, unsigned char * bits);
	void init(GfxRenderer * gl, const std::string & fileName, Format format);
	
	bool getBitsFromPixelBuffer(PixelBuffer * pixelBuffer);
	/**
	@return true if fully loaded
	*/
	bool asyncLoadUpdate();

	/**
	@return whether the texture has been fully loaded. Note, also return true if the
	texture file name is empty.
	*/
	bool isFullyLoaded() const;

	/**
	@return whether the texture has been at least partially loaded (ex: stage 1 of 
	a progressive image). Note, also return true if the texture file name is 
	empty.
	*/
	bool isLoaded() const;

	bool isEquivalent(const Texture & texture) const;
	
	void setForModel(bool val) {forModel_ = val;}

	void uninit();

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	void requestLocalSaveObject(const std::string & fileName);

	void requestZoom();
	GLuint glTexture() const {return texture_;}
	bool needSeparateAlpha() const;
	bool isInit() const;

private:
	void createGlTextureFrom(unsigned char * bits);
	
	void setupGlTexture(bool mipMapsIfCompressed);
	void genGlTexture(GLint format, GLenum type, GLvoid * pixels);
private:
	std::string fileName_;

	unsigned short width_, height_;
	unsigned short activeWidth_, activeHeight_;
	GLuint texture_, textureAlpha_;

	Format compression_;
	bool forModel_;

	bool loadRequested_;
	bool requesteZoom_;
    bool needZoomImage_;
};