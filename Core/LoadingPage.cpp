#include "stdafx.h"
#include "LoadingPage.h"
#include "GLUtils.h"
#include "GfxRenderer.h"
#include "Global.h"

#include "TextureRenderer.h"
#include "Texture.h"





LoadingScreen::LoadingScreen()
{
	texture_ = new Texture;
	texRenderer_ = new TextureRenderer;
	init_ = false;
	dim_ = 0;
	width_ = height_ = 0;
}

LoadingScreen::~LoadingScreen()
{
	delete texture_;
	delete texRenderer_;
}

void LoadingScreen::init(GfxRenderer * gl, int width, int height)
{
	uninit();
	dim_ = std::max(std::min(std::min(width, height)*0.3f, 48.0f), 24.0f);	
	width_ = width;
	height_ = height;
	texture_->init(gl, dim_, dim_, Texture::UncompressedRgba32, 0);
	texRenderer_->init(gl, dim_, dim_, false);
	texRenderer_->setTexture(texture_);

	
	////layout thumb shader		
	startTime_ = Global::currentTime();
	init_ = true;
}

void LoadingScreen::uninit()
{
	if (!init_) return;
	texRenderer_->uninit();
	texture_->uninit();
	
	
	init_ = false;
}

void LoadingScreen::draw(GfxRenderer * gl, float x, float y)
{
	texRenderer_->beginRenderToTexture(gl);

	gl->useLoadingProgram();
	gl->setLoadingProgramDim(dim_);
	float t = 6*(Global::currentTime() - startTime_);
	int numPeriods = (int)(t / (2*M_PI));
	t -= (2*M_PI)*numPeriods;
	gl->setLoadingProgramAngle(t);
	

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	gl->drawRect(0, 0, (float)dim_, (float)dim_);
	texRenderer_->endRenderToTexture(gl);
	/*
	gl->useColorProgram();
	gl->setColorProgramColor(0, 0, 0, 1);
	gl->drawRect(x, y, width_, height_);*/
	gl->useTextureProgram();
	gl->setTextureProgramOpacity(0.75f);
	gl->use(texture_);
	gl->drawRect(float(x + width_/2 - dim_/2), float(y + height_/2 - dim_/2), (float)dim_, (float)dim_);
}