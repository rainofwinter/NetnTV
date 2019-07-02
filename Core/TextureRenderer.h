#ifndef RenderableTexture_h__
#define RenderableTexture_h__
#include "MathStuff.h"

class Texture;
class GfxRenderer;
class Camera;
class TextureRenderer
{
public:
	TextureRenderer();
	~TextureRenderer();

	/**
	width and height must be non zero
	*/
	void init(GfxRenderer * gl, int width, int height, bool stencilBuffer);
	void uninit();

	void setTexture(Texture * texture);
	unsigned char * textureBits() const;

	int width() const {return width_;}
	int height() const {return height_;}

	bool isInit() const {return width_ > 0;}

	void beginRenderToTexture(GfxRenderer * gl, bool newState = true);
	void endRenderToTexture(GfxRenderer * gl);
	
private:
	int width_, height_;
	GLuint frameBuffer_, depthBuffer_, stencilBuffer_;
	GLuint prevFrameBuffer_;
	GLint viewport_[4];

	bool frameBufferSet_;	

	Matrix camMatrix_;
	Matrix prevCamMatrix_;

	bool newState_;
};

#endif // RenderableTexture_h__