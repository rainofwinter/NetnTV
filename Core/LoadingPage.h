#pragma once

class TextureRenderer;
class Texture;
class GfxRenderer;

class LoadingScreen
{
public:
	LoadingScreen();
	~LoadingScreen();
	void init(GfxRenderer * gl, int width, int height);
	void uninit();
	void draw(GfxRenderer * gl, float x, float y);
private:
	Texture * texture_;
	TextureRenderer * texRenderer_;
	bool init_;
	int width_, height_;
	int dim_;
	float startTime_;
	
};