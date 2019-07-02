#pragma once
class GfxRenderer;

////////////////////////////////////////////////////////////////////////////////

class VideoMedia
{
public:
	VideoMedia(const char * fileName);
	~VideoMedia();

	void DrawCurFrame(GfxRenderer * gl, int, int, int, int);

	void Play(double time);	
	
	void Stop();

	void SetVolume(double volume);

	int GetWidth() const {return width_;}
	int GetHeight() const {return height_;}
	double duration() const;
	double currentTime() const;

	bool isPlaying() const {return isPlaying_;}
	bool isEnded() const {return !isPlaying_ && curTime_ >= duration_;}

	//run thread
	void operator()();

private:

	DWORD CopyToSoundBuffer(
		unsigned char *& buf1, DWORD &buf1Size, unsigned char *& buf2, DWORD &buf2Size, 
		unsigned char * data, DWORD size);
	void GetNextAudioData(double secs);

	void Play();
	void SeekFrom(double origCurTime);
	void CalcCurTime();

	//codecMutex_ should be locked when this is called
	void DecodeNextFrame();

	bool isPlaying_;
	bool terminate_;
	boost::thread decodeThread_;
	
	void LoadFile(const char * fileName);

	AVFormatContext * vFormatCtx_;
	AVFormatContext * aFormatCtx_;
	int videoStream;
	int audioStream;

	int width_;
	int padding_;
	int height_;

	mutable boost::mutex timeMutex_;
	double curTime_;
	double duration_;
	double fps_;
	double timeBase_, audioTimeBase_;

	boost::mutex codecMutex_;
	AVFormatContext * formatCtx_;
	AVCodecContext * pCodecCtx;
	AVCodecContext * aCodecCtx;
	AVCodec * pCodec;
	AVCodec * aCodec;
	
	boost::mutex soundMutex_;
	int tempAudioBufferSize_;
	::uint8_t * tempAudioBuffer_;	
	unsigned int tempAudioBufferPos_;
	
	double pts_;

	AVFrame * pFrameRGB;
	AVFrame * pFrame;

	SwsContext * img_convert_ctx;

	//----------------------------------------------------------------------------
	UINT64 ticksPerSecond_;
	double timeOffset_;

	//----------------------------------------------------------------------------	
	unsigned int dSoundBufferSize_;
	unsigned int dSoundBytesPerSec_;
	LPDIRECTSOUNDBUFFER dSoundBuffer_;
	unsigned int dSoundBufferPos_; ///< in # bytes
	double dSoundUpdateSecs_;
	double dSoundMargins_;
	double lastSoundUpdateTime_;

	UINT64 ticksAtPlayStart_;
	

	boost::mutex imageMutex_;
	bool imageWritten_;
	unsigned char * image_;

	GLuint texture_;

};


void initVideoEnv(HWND hwnd);

void uninitVideoEnv();