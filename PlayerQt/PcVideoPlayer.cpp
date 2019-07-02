#include "stdafx.h"
#include "PCVideoPlayer.h"
#include "VideoMedia.h"
#include "GfxRenderer.h"
#include "MathStuff.h"
#include "Camera.h"
#include "Document.h"

PcVideoPlayer::PcVideoPlayer(Document * document)
{
	document_ = document;

	showControls_ = false;
	activated_ = false;
	wasFullScreenInitially_ = false;
	
	media_ = 0;
}

PcVideoPlayer::~PcVideoPlayer()
{
	if (isActivated())deactivate();
	if (media_) delete media_;
}

float PcVideoPlayer::currentTime() const
{
	if (!media_) return 0.0f;
	return (float)media_->currentTime();
}


bool PcVideoPlayer::isBlockingCursor(int mouseX, int mouseY) const
{
	if (activated_)
	{
		if (fullScreen_) return true;		

		float cx = rx_;
		float cy = ry_;
		if (cx <= mouseX && mouseX <= cx + rwidth_ &&
			cy <= mouseY && mouseY <= cy + rheight_)
			return true;
	}
	
	return false;
}

void PcVideoPlayer::activate(int id,
	const std::string & fileName, bool isUrl, bool showControls, float startTime)
{		
	if (isActivated()) deactivate();	
		
	activated_ = true;

	if (media_)delete media_;
	try{
		media_ = new VideoMedia(fileName.c_str());
	} catch (const std::exception & e) {
		QMessageBox::information(0, "Error", e.what());
		media_ = 0;
		deactivate();		
		return;
	}

	fullScreen_ = true;
	wasFullScreenInitially_ = true;
	media_->Play(0);	
	document_->videoPlayedEvent(id);
	id_ = id;
}



void PcVideoPlayer::activateRegion(int id, 
	const std::string & fileName, 
	int x, int y, int width, int height, bool isUrl, bool showControl, float startTime)
{
	if (isActivated()) deactivate();
	activated_ = true;

	if (media_)delete media_;
	try{
		media_ = new VideoMedia(fileName.c_str());
	} catch (const std::exception & e) {
		QMessageBox::information(0, "Error", e.what());

		media_ = 0;
		deactivate();		
		return;
	}
	showControls_ = showControl;
	rx_ = x;
	ry_ = y;
	rwidth_ = width;
	rheight_ = height;

	fullScreen_ = false;
	wasFullScreenInitially_ = false;
	media_->Play(startTime);
	document_->videoPlayedEvent(id);
	id_ = id;
}

void PcVideoPlayer::deactivate()
{
	if (!isActivated()) return;
	if (media_) 
	{
		media_->Stop();
		delete media_;
		media_ = 0;
	}
	
	activated_ = false;
	showControls_ = false;
	wasFullScreenInitially_ = false;
	document_->videoStoppedEvent();
	document_->videoStoppedEvent(id_);
}

void PcVideoPlayer::play()
{
	media_->Play(0);
	document_->videoPlayedEvent(id_);
}

void PcVideoPlayer::stop()
{
	deactivate();
}

void PcVideoPlayer::toggleFullscreen()
{
	if (wasFullScreenInitially_) return;
	fullScreen_ = !fullScreen_;
}

void PcVideoPlayer::draw(GfxRenderer * gl)
{	
	if (!isActivated()) return;
	if (!media_) return;
	float aspect = (float)media_->GetWidth()/media_->GetHeight();
	float vidWidth = width_;
	float vidHeight = vidWidth / aspect;
	
	if (vidHeight > height_ + 0.001f)
	{
		vidHeight = height_;
		vidWidth = vidHeight * aspect;
	}
	Camera camera;

	camera.SetTo2DArea(width_/2, height_/2, width_, height_, 60, (float)width_ / height_);

	if (fullScreen_)
	{		
		gl->setCameraMatrix(Matrix::Identity());
		gl->useColorProgram();
		gl->setColorProgramColor(0, 0, 0, 1);
		gl->drawRect(-1, -1, 2, 2);

		gl->setCameraMatrix(camera.GetMatrix());

		media_->DrawCurFrame(
			gl, (int)(width_ - vidWidth)/2, (int)(height_ - vidHeight)/2, (int)vidWidth, (int)vidHeight);
			
	}
	else
	{	

		gl->setCameraMatrix(camera.GetMatrix());
		media_->DrawCurFrame(gl, rx_, ry_, rwidth_, rheight_);
	}

	if (media_->isEnded()) 
	{

		deactivate();
	}
	
}


/*
void PcVideoPlayer::setRegionOffset(int offsetX, int offsetY)
{
	offsetX_ = offsetX;
	offsetY_ = offsetY;

}
*/

void PcVideoPlayer::setRegion(int x, int y, int w, int h)
{
	rx_ = x;
	ry_ = y;
	rwidth_ = w;
	rheight_ = h;
}




void PcVideoPlayer::resize(int width, int height)
{
	/*
	if (frameHwnd_)
	{
		MoveWindow(frameHwnd_, 0, height - 24, width, height, true);		
	}*/

	width_ = width;
	height_ = height;
}

/*
LRESULT CALLBACK PCVideoPlayer::
	wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static RECT rect;
	switch (message)
	{	
	case WM_SIZE:
		{		
			GetClientRect(hwnd, &rect);	
			MoveWindow(playBtnHwnd_, 0, 0, rect.right/3, 24, true); 
			MoveWindow(stopBtnHwnd_, rect.right/3, 0, rect.right/3, 24, true);
			MoveWindow(exitBtnHwnd_, 2*rect.right/3, 0, rect.right - 2*rect.right/3, 24, true);
		
			
			return 0;
		}
	case WM_COMMAND:
		{			
			switch (LOWORD(wParam))
			{
			case ID_PLAY_BUTTON:	
				media_->Play(0);
				
				break;
			case ID_STOP_BUTTON:
				media_->Stop();
				break;
			case ID_EXIT_BUTTON:
				deactivate();
				break;
			}
			return 0;
		}
	
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
*/

float PcVideoPlayer::duration() const
{
	if (!media_) return 0.0f;
	return (float)media_->duration();
}

void PcVideoPlayer::seek(float time)
{
	media_->Play((double)time);
	document_->videoSeekedEvent(id_, time);
}