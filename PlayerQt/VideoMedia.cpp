#include "stdafx.h"

#include "VideoMedia.h"
#include "Exception.h"
#include "GfxRenderer.h"

using namespace std;
using namespace boost;

extern LPDIRECTSOUND8 gDirectSound8;

VideoMedia::VideoMedia(const char * fileName) 
: videoStream(-1), audioStream(-1),
pFrameRGB(0), pFrame(0), pCodecCtx(0), aCodecCtx(0), 
vFormatCtx_(0), aFormatCtx_(0), pCodec(0), aCodec(0), img_convert_ctx(0), 
isPlaying_(0), terminate_(0), pts_(-1),
dSoundBuffer_(0)
{
	curTime_ = 0;
	timeOffset_ = 0;

	dSoundUpdateSecs_ = 1;
	dSoundMargins_ = 1;
	dSoundBufferPos_ = 0;
	tempAudioBuffer_ = (::uint8_t*)av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);
	tempAudioBufferSize_ = 0;
	tempAudioBufferPos_ = 0;

	image_ = 0;

	LoadFile(fileName);
	QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond_);
	QueryPerformanceCounter((LARGE_INTEGER *)&ticksAtPlayStart_);

	if (aCodecCtx && aCodecCtx->sample_fmt == SAMPLE_FMT_S16)
	{
		HRESULT hr;

		WAVEFORMATEX waveFormat;
		waveFormat.cbSize = sizeof(waveFormat);
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nChannels = aCodecCtx->channels;
		waveFormat.nSamplesPerSec = aCodecCtx->sample_rate;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
				
		dSoundBuffer_ = NULL;
		if (gDirectSound8)
		{
			DSBUFFERDESC desc = {0};
			desc.dwSize = sizeof(desc);
			desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME;
			dSoundBytesPerSec_ = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
			//3 secs
			desc.dwBufferBytes = dSoundBytesPerSec_ * (dSoundUpdateSecs_+2*dSoundMargins_);
			desc.dwReserved = 0;
			desc.lpwfxFormat = &waveFormat;
			hr = gDirectSound8->CreateSoundBuffer(&desc, &dSoundBuffer_, 0);		
			dSoundBufferSize_ = desc.dwBufferBytes;
		}		
	}

	if (dSoundBuffer_)
		GetNextAudioData(dSoundUpdateSecs_ + dSoundMargins_);
	lastSoundUpdateTime_ = 0;


	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 
		0, GL_RGBA, GL_UNSIGNED_BYTE, 0);


	//start decoding
	decodeThread_ = thread(boost::ref(*this));	
}


VideoMedia::~VideoMedia()
{
	terminate_ = true;
	decodeThread_.join();	
	if (pFrameRGB) av_free(pFrameRGB);
	if (pFrame) av_free(pFrame);
	if (pCodecCtx) avcodec_close(pCodecCtx);
	if (aCodecCtx) avcodec_close(aCodecCtx);
	if (vFormatCtx_) av_close_input_file(vFormatCtx_);
	if (aFormatCtx_) av_close_input_file(aFormatCtx_);
	if (img_convert_ctx) sws_freeContext(img_convert_ctx);	
	if (dSoundBuffer_) 
	{
		dSoundBuffer_->Stop();
		dSoundBuffer_->Release();
	}
	if (tempAudioBuffer_) av_free(tempAudioBuffer_);
	if (image_)av_free(image_);
	glDeleteTextures(1, &texture_);
}


void VideoMedia::LoadFile(const char * fileNameA)
{
	int err;

	err = av_open_input_file(&vFormatCtx_, fileNameA, NULL, 0, NULL);
	if (err != 0)
		throw std::runtime_error("Couldn't open video stream");
	//trackID = 15;

	if (!strcmp(vFormatCtx_->iformat->name, "rtsp"))	
		throw std::runtime_error("Unsupported format");

	err = av_find_stream_info(vFormatCtx_);	
	if(err < 0 )
		throw std::runtime_error("Couldn't find stream info");
		
	int i;
	
	// Find the first video stream
	for(i=0; i<vFormatCtx_->nb_streams; i++)
		if(vFormatCtx_->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
			videoStream=i;
			break;
		}
	
	// Find the first audio stream
	for(i=0; i<vFormatCtx_->nb_streams; i++)
		if(vFormatCtx_->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO) {
			audioStream=i;
			break;
		}

	
	
	if(videoStream==-1)
		throw std::runtime_error("Couldn't find video stream");
		
	if(audioStream>=0)
	{
		
		if (av_open_input_file(&aFormatCtx_, fileNameA, NULL, 0, NULL)!=0)
			throw std::runtime_error("Couldn't open audio stream");
		aCodecCtx = aFormatCtx_->streams[audioStream]->codec;
		audioTimeBase_ = 1.0 / av_q2d(aFormatCtx_->streams[audioStream]->time_base);
	}		

	pCodecCtx = vFormatCtx_->streams[videoStream]->codec;
	timeBase_ = 1.0 / av_q2d(vFormatCtx_->streams[videoStream]->time_base);
	
	fps_ = av_q2d(vFormatCtx_->streams[videoStream]->r_frame_rate);
	duration_ = vFormatCtx_->duration / AV_TIME_BASE;	

	// Find the decoder for the video stream
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL) {
		throw std::runtime_error("Unsupported video codec");
	}
	// Open codec
	if(avcodec_open(pCodecCtx, pCodec)<0)
		throw std::runtime_error("Couldn't open video codec");

	if (audioStream >= 0)
	{
		aCodec = avcodec_find_decoder(aCodecCtx->codec_id);
		if(!aCodec) {
			throw std::runtime_error("Unsupported audio codec");
		}
		if(avcodec_open(aCodecCtx, aCodec)<0)
			throw std::runtime_error("Couldn't open audio codec");

	}

	width_ = pCodecCtx->width;
	height_ = pCodecCtx->height;	

	pFrameRGB = avcodec_alloc_frame();
	if (!pFrameRGB)	
		throw std::runtime_error("Couldn't allocate RGB frame");
	
	pFrame=avcodec_alloc_frame();
	if (!pFrame)	
		throw std::runtime_error("Couldn't allocate frame");


	if (image_)av_free(image_);
	padding_ = 0;
	image_ = (unsigned char *)av_malloc((width_ + padding_)*height_*4);
	imageWritten_ = false;

	::PixelFormat dstFmt = PIX_FMT_RGBA;
	::PixelFormat srcFmt = pCodecCtx->pix_fmt;

	img_convert_ctx = sws_getContext(width_, height_, srcFmt, 
		width_, height_, dstFmt, SWS_FAST_BILINEAR, 0, 0, 0);
	
}


void VideoMedia::DrawCurFrame(GfxRenderer * gl, int x, int y, int width, int height)
{
	unique_lock<mutex> lk(imageMutex_);
	if (!imageWritten_) return;
	glBindTexture(GL_TEXTURE_2D, texture_);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
		width_, height_, 
		0, GL_RGBA, GL_UNSIGNED_BYTE, image_);
	lk.unlock();

	int prevDepthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);
	glDepthFunc(GL_ALWAYS);

	gl->useTextureProgram();
	gl->setTextureProgramOpacity(1);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture_);
	
	gl->drawRect(x, y, width, height);

	glDepthFunc(prevDepthFunc);
}

void VideoMedia::Play(double time)
{	
	double origCurTime = curTime_;
	UINT64 ticks;
	QueryPerformanceCounter((LARGE_INTEGER *)&ticksAtPlayStart_);
	timeOffset_ = time;

	CalcCurTime();
	if (curTime_ < 0.001f) curTime_ = 0;
	SeekFrom(origCurTime);
	Play();
}

void VideoMedia::Play()
{
	isPlaying_ = true;	

	if (audioStream >= 0)
	{
		
		if (dSoundBuffer_)
		{				
			dSoundBufferPos_ = 0;
			lastSoundUpdateTime_ = curTime_;
			tempAudioBufferPos_ = 0;
			tempAudioBufferSize_ = 0;
			GetNextAudioData(dSoundUpdateSecs_ + dSoundMargins_);
			
			lock_guard<mutex> lk(soundMutex_);
			dSoundBuffer_->SetCurrentPosition(0);
			dSoundBuffer_->Play(0, 0, DSBPLAY_LOOPING);
		}
	}
}

void VideoMedia::SeekFrom(double origCurTime)
{
	unique_lock<mutex> lk(codecMutex_);
	::int64_t seek_target = curTime_ * timeBase_;
	::int64_t audio_seek_target = curTime_ * audioTimeBase_;

	lastSoundUpdateTime_ = curTime_;
	if (audioStream >= 0)	
		av_seek_frame(aFormatCtx_, audioStream, audio_seek_target, AVSEEK_FLAG_ANY);					

	pts_ = -1;	
	if (fabs(curTime_ - origCurTime) > 1)
	{
		av_seek_frame(vFormatCtx_, videoStream, seek_target, AVSEEK_FLAG_BACKWARD);	
	
		bool failed = false;
		UINT64 startTicks, curTicks;
		QueryPerformanceCounter((LARGE_INTEGER *)&startTicks);
		bool reached = false;
		while(reached)
		{
			reached = true;
			if (pts_ < curTime_)
			{
				reached = false;
				DecodeNextFrame();
			}
			
			QueryPerformanceCounter((LARGE_INTEGER *)&curTicks);
			double elapsed = 
				double(curTicks - startTicks) / double(ticksPerSecond_);

			if (elapsed > 2.0) 
			{
				failed = true;
				break;				
			}
		}

		if (failed)
		{
			av_seek_frame(vFormatCtx_, videoStream, seek_target, AVSEEK_FLAG_ANY);			
		}

		QueryPerformanceCounter((LARGE_INTEGER *)&ticksAtPlayStart_);
	}
}

void VideoMedia::Stop()
{
	isPlaying_ = false;
	if (dSoundBuffer_)
	{
		lock_guard<mutex> lk(soundMutex_);
		HRESULT hr = dSoundBuffer_->Stop();
	}
	lock_guard<mutex> lk(imageMutex_);
	imageWritten_ = false;
}

void VideoMedia::SetVolume(double volume)
{	
	if(dSoundBuffer_)
		dSoundBuffer_->SetVolume(DSBVOLUME_MIN + volume*(DSBVOLUME_MAX - DSBVOLUME_MIN));
		
}

void VideoMedia::DecodeNextFrame()
{
	int frameFinished = 0;
	AVPacket packet;
	int ret = 0;
	while((ret = av_read_frame(vFormatCtx_, &packet))>=0) {
		// Is this a packet from the video stream?
		if(packet.stream_index==videoStream) {

			if(packet.dts != AV_NOPTS_VALUE)
			{				
				pts_ = packet.dts;
				pts_ /= timeBase_;				
			}

			// Decode video frame
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
	    
			if(frameFinished) 
			{
				lock_guard<mutex> lk(imageMutex_);

				//frame dimensions differ from what was indicated in header?
				if (pFrame->width != width_ || pFrame->height != height_ ||
					pFrame->linesize[0] - pFrame->width != padding_)
				{
					padding_ = pFrame->linesize[0] - pFrame->width;
					int nw, nh;
					avcodec_align_dimensions(pCodecCtx, &nw, &nh);
					width_ = pFrame->width;
					height_ = pFrame->height;
					sws_freeContext(img_convert_ctx);	
					::PixelFormat dstFmt = PIX_FMT_RGBA;
					::PixelFormat srcFmt = pCodecCtx->pix_fmt;
					image_ = (unsigned char *)av_realloc(image_, (width_ + padding_)*height_*4);
					img_convert_ctx = 
						sws_getContext(width_, height_, srcFmt, 
						width_, height_, dstFmt, SWS_FAST_BILINEAR, 0, 0, 0);
				}
			
				::uint8_t * bufferStart = 
					(::uint8_t *)image_;

				int lineSize = (width_ + padding_)*4;	


				int ret = sws_scale(
					img_convert_ctx, pFrame->data, pFrame->linesize, 0, 
					height_, &bufferStart, &lineSize);

				if (padding_ > 0)
				{
					for (int i = 0; i < height_; ++i)
					{
						memcpy(bufferStart + i * width_ * 4,
							bufferStart + i * (width_ + padding_) * 4, width_*4);					
					}
					imageWritten_ = true;
				}
								

			} //end if(frameFinished)
		}

		// Free the packet that was allocated by av_read_frame
		av_free_packet(&packet);

		//read just one frame
		if (frameFinished) break;
	
	}
}

DWORD VideoMedia::CopyToSoundBuffer(
	unsigned char *& buf1, DWORD &buf1Size, unsigned char *& buf2, DWORD &buf2Size, 
	unsigned char * data, DWORD size)
{
	if (size < buf1Size)
	{
		memcpy(buf1, data, size);
		buf1 += size;
		buf1Size -= size;
		return size;
	}
	else
	{
		DWORD totalSize = size;
		if (size > buf1Size + buf2Size) 
		{
			size = buf1Size + buf2Size;
			totalSize = buf1Size + buf2Size;
		}
		memcpy (buf1, data, buf1Size);
		buf1 += buf1Size;		
		data += buf1Size;
		size -= buf1Size;
		buf1Size = 0;
		memcpy(buf2, data, size);
		buf2 += size;
		buf2Size -= size;
		return totalSize;
	}
}

void VideoMedia::GetNextAudioData(double secs)
{
	unsigned char * lockedBuffer1 = 0, * lockedBuffer2 = 0;
	DWORD lockedBufferSize1, lockedBufferSize2;
	HRESULT hr;
	
	int len = secs * dSoundBytesPerSec_;
	lock_guard<mutex> lk(soundMutex_);

	hr = dSoundBuffer_->Lock(
		dSoundBufferPos_, len, 
		(void **)&lockedBuffer1, &lockedBufferSize1, 
		(void **)&lockedBuffer2, &lockedBufferSize2, 0);

	unsigned char * lockedBuf1Pos = lockedBuffer1, * lockedBuf2Pos = lockedBuffer2;
	DWORD lockedBuf1Size = lockedBufferSize1, lockedBuf2Size = lockedBufferSize2;

	AVPacket packet;		

	//first finish copying left over data from last time
	bool needMoreData = true;
	if (tempAudioBufferSize_ > 0)
	{
		DWORD leftOverBytes = tempAudioBufferSize_ - tempAudioBufferPos_;
		DWORD copied = CopyToSoundBuffer(
			lockedBuf1Pos, lockedBuf1Size, lockedBuf2Pos, lockedBuf2Size, 
			tempAudioBuffer_ + tempAudioBufferPos_, leftOverBytes);

		if (copied < leftOverBytes)
		{
			tempAudioBufferPos_ += copied;			
			needMoreData = false;
		}
		else
		{			
			tempAudioBufferPos_ = 0;
			tempAudioBufferSize_ = 0;
		}

		len -= copied;
	}

	while(needMoreData && av_read_frame(aFormatCtx_, &packet)>=0) {
		if(packet.stream_index==audioStream) {	
			
			tempAudioBufferSize_ = AVCODEC_MAX_AUDIO_FRAME_SIZE;
			
			int usedLen = avcodec_decode_audio3(aCodecCtx, (::int16_t *)tempAudioBuffer_, 
				&tempAudioBufferSize_, &packet);

			if (usedLen < 0) continue;

			int bytesToCopy = tempAudioBufferSize_;

			DWORD bytesCopied = CopyToSoundBuffer(
					lockedBuf1Pos, lockedBuf1Size, lockedBuf2Pos, lockedBuf2Size, 
					tempAudioBuffer_, bytesToCopy);				

			len -= bytesCopied;
			tempAudioBufferPos_ += bytesCopied;

			if (bytesCopied == tempAudioBufferSize_)
			{
				tempAudioBufferPos_ = 0;
				tempAudioBufferSize_ = 0;
			}		
		}

		// Free the packet that was allocated by av_read_frame
		av_free_packet(&packet);
		if (len == 0) break;
	
	}
	
	//zero out rest
	memset(lockedBuf1Pos, 0, lockedBuf1Size);
	memset(lockedBuf2Pos, 0, lockedBuf2Size);

	hr = dSoundBuffer_->Unlock(
		lockedBuffer1, lockedBufferSize1, lockedBuffer2, lockedBufferSize2);

	dSoundBufferPos_ += lockedBufferSize1 + lockedBufferSize2;
	if (dSoundBufferPos_ >= dSoundBufferSize_) dSoundBufferPos_ -= dSoundBufferSize_;
	
}

void VideoMedia::CalcCurTime()
{
	UINT64 ticks;
	QueryPerformanceCounter((LARGE_INTEGER *)&ticks);
	lock_guard<mutex> lk(timeMutex_);
	curTime_ = 
		double(ticks - ticksAtPlayStart_) / double(ticksPerSecond_) + 
		timeOffset_;	
	if (curTime_ > duration_) curTime_ = duration_;
}

double VideoMedia::currentTime() const
{
	lock_guard<mutex> lk(timeMutex_);
	return curTime_;
}

double VideoMedia::duration() const
{
	lock_guard<mutex> lk(timeMutex_);
	return duration_;
}

void VideoMedia::operator()()
{	
	SetPriorityClass(GetCurrentThread(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	while(1) //main loop
	{
		if (terminate_) break;
		this_thread::sleep(posix_time::milliseconds(10));
		if (!isPlaying_) continue;		
	
		CalcCurTime();
		if (curTime_ >= duration_) Stop();
		
		if (curTime_ - lastSoundUpdateTime_ > dSoundUpdateSecs_)
		{
			if (dSoundBuffer_)
				GetNextAudioData(dSoundUpdateSecs_);
			lastSoundUpdateTime_ += dSoundUpdateSecs_;
		}		
		
		if (pts_ < curTime_)
		{
			lock_guard<mutex> lk(codecMutex_);
			DecodeNextFrame();
		}	
		
	}//end main loop

}

