#include "stdafx.h"
#include "TextureRenderer.h"
#include "Texture.h"
#include "GfxRenderer.h"
#include "Camera.h"
#include "Global.h"

#if defined(IOS)
#define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
//#define GL_FRAMEBUFFER_BINDING  GL_FRAMEBUFFER_BINDING_OES
#define GL_FRAMEBUFFER_BINDING  0x8CA6
#endif 

#ifndef GL_DEPTH24_STENCIL8
#define GL_DEPTH24_STENCIL8 0x88F0
#endif

#ifndef GL_DEPTH_STENCIL
#define GL_DEPTH_STENCIL 0x84F9
#endif

#ifdef MSVC
//for intel gpus using EXT versions can fix crashing problems
#define glBindFramebuffer glBindFramebufferEXT
#define glBindRenderbuffer glBindRenderbufferEXT
#define glCheckFramebufferStatus glCheckFramebufferStatusEXT
#define glDeleteFramebuffers glDeleteFramebuffersEXT
#define glDeleteRenderbuffers glDeleteRenderbuffersEXT
#define glFramebufferRenderbuffer glFramebufferRenderbufferEXT
#define glFramebufferTexture1D glFramebufferTexture1DEXT
#define glFramebufferTexture2D glFramebufferTexture2DEXT
#define glFramebufferTexture3D glFramebufferTexture3DEXT
#define glGenFramebuffers glGenFramebuffersEXT
#define glGenRenderbuffers glGenRenderbuffersEXT
#define glGenerateMipmap glGenerateMipmapEXT
#define glGetFramebufferAttachmentParameteriv glGetFramebufferAttachmentParameterivEXT
#define glGetRenderbufferParameteriv glGetRenderbufferParameterivEXT
#define glIsFramebuffer glIsFramebufferEXT
#define glIsRenderbuffer glIsRenderbufferEXT
#define glRenderbufferStorage glRenderbufferStorageEXT

#endif

TextureRenderer::TextureRenderer()
{
	frameBuffer_ = depthBuffer_ = stencilBuffer_ = 0;
	width_ = height_ = 0;
	frameBufferSet_ = false;
}

TextureRenderer::~TextureRenderer()
{
	uninit();
}

void TextureRenderer::setTexture(Texture * texture)
{
	if (frameBufferSet_)
	{		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->texture_, 0);				
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);	
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->texture_, 0);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(status == GL_FRAMEBUFFER_COMPLETE) {
			//std::cout << "Framebuffer completed" << std::endl;
			LOGE("Framebuffer completed");
		} else {
			//std::cout << "Framebuffer error: " << status << std::endl;
			LOGE("Framebuffer error: %d", status);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}	
}

unsigned char * TextureRenderer::textureBits() const
{
	unsigned char * buffer = (unsigned char *)malloc(4*width_*height_);

	if (frameBufferSet_)
	{		
		glReadPixels(0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *)buffer); 		
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);		
		glReadPixels(0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *)buffer); 		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}	

	return buffer;
}



void TextureRenderer::init(GfxRenderer * gl, int width, int height, bool stencilBuffer)
{
	uninit();

	width_ = width;
	height_ = height;

	Camera camera;
	camera.SetTo2DArea(width/2.0f, height/2.0f, width, height, 60, (float)width/height);
	camMatrix_ = Matrix::Scale(1, -1, 1) * camera.GetMatrix();

	glGenFramebuffers(1, &frameBuffer_);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);


	if (Global::useStencilBuffer() && stencilBuffer)
	{
		//#ifdef ANDROID
		//
		//
		//glGenRenderbuffers(1, &depthBuffer_);
		//glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width_, height_);
		//
		//glGenRenderbuffers(1, &stencilBuffer_);
		//glBindRenderbuffer(GL_RENDERBUFFER, stencilBuffer_);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width_, height_);
		//
		//
		//glFramebufferRenderbuffer(
		//	GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);
		//glFramebufferRenderbuffer(
		//	GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer_);
		//#else
		glGenRenderbuffers(1, &depthBuffer_);
		glCheckFramebufferStatus(GL_FRAMEBUFFER);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
		//glFramebufferRenderbuffer(
		//	GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);
		//#endif
	}
	else
	{
		glGenRenderbuffers(1, &depthBuffer_);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width_, height_);	
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);
	}

	//GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void TextureRenderer::uninit()
{
	if (frameBuffer_)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);	
		glBindFramebuffer(GL_FRAMEBUFFER, 0);	

		glDeleteFramebuffers(1, &frameBuffer_);
		frameBuffer_ = 0;
	}

	if (depthBuffer_)
	{
		glDeleteRenderbuffers(1, &depthBuffer_);
		depthBuffer_ = 0;
	}

	if (stencilBuffer_)
	{
		glDeleteRenderbuffers(1, &stencilBuffer_);
		stencilBuffer_ = 0;
	}	

	width_ = -1;
	height_ = -1;
}

void TextureRenderer::beginRenderToTexture(GfxRenderer * gl, bool newState)
{
	newState_ = newState;
	GLint prevFrameBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFrameBuffer);
	gl->setCurTexRenderer(this);
	prevFrameBuffer_ = prevFrameBuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
	glGetIntegerv(GL_VIEWPORT, viewport_);

	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);

	if (newState_)
	{
		prevCamMatrix_ = gl->cameraMatrix();
		gl->setCameraMatrix(camMatrix_);
		//GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		gl->pushMatrix();
		gl->loadMatrix(Matrix::Identity());
	}

	glViewport(0, 0, width_, height_);

	frameBufferSet_ = true;

	gl->pushStencilState();

}

void TextureRenderer::endRenderToTexture(GfxRenderer * gl)
{
	glViewport(viewport_[0], viewport_[1], viewport_[2], viewport_[3]);
	glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer_);
	frameBufferSet_ = false;

	if (newState_)
	{
		gl->setCameraMatrix(prevCamMatrix_);
		gl->popMatrix();
	}

	gl->popStencilState();
}
