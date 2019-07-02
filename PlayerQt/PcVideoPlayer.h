#pragma once
#include <string>
#include "VideoPlayer.h"


class VideoMedia;
class GfxRenderer;


class PcVideoPlayer : public VideoPlayer
{
public:
	PcVideoPlayer(Document * document);
	virtual ~PcVideoPlayer();

	virtual void activate(int id,
		const std::string & fileName, bool isUrl, bool showControls, float startTime);

	virtual void activateRegion(int id, const std::string & fileName,
		int x, int y, int width, int height, 
		bool isUrl, bool showControls, float startTime);

	virtual void deactivate();

	virtual float currentTime() const;

	void play();
	void stop();
	void toggleFullscreen();

	float duration() const;
	void seek(float time);


	void resize(int width, int height);
	bool isActivated() const {return activated_;}
	bool isShowControls() const {return showControls_;}
	bool isBlockingCursor(int mouseX, int mouseY) const;
	bool isActivatedFullScreen() const {return activated_ && fullScreen_;}
	bool wasFullScreenInitially() const {return wasFullScreenInitially_;}

	virtual void setRegion(int x, int y, int w, int h);
	//virtual void setRegionOffset(int offsetX, int offsetY);


	void drawRegion(int * rx, int * ry, int * rw, int * rh)
	{
		*rx = rx_;
		*ry = ry_;
		*rw = rwidth_;
		*rh = rheight_;
	}
	void draw(GfxRenderer * gl);
private:

	//static LRESULT CALLBACK wndProcCaller(HWND, UINT, WPARAM, LPARAM);
	//LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);	

private:
	bool activated_;
	bool showControls_;
	bool fullScreen_;
	bool wasFullScreenInitially_;
	int rx_, ry_, rwidth_, rheight_;

	enum
	{
		ID_PLAY_BUTTON,
		ID_STOP_BUTTON,
		ID_EXIT_BUTTON
	};


	int width_, height_;
	VideoMedia * media_;

	int id_;

	
	Document * document_;

};