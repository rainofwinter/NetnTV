#pragma once
class GfxRenderer;

////////////////////////////////////////////////////////////////////////////////

class AudioMedia
{
public:
	AudioMedia(const char * fileName, bool repeat = false);
	~AudioMedia();


	void Play(double time);	
	
	void Stop();
	void Resume();
	void SetVolume(double volume);

	double GetDuration() const;
	double curTime() const;
	
	bool isPlaying() const;
	bool isEnded() const;
	

	//run thread
	void operator()();

private:
	void zeroSoundBuffer();
	DWORD CopyToSoundBuffer(
		unsigned char *& buf1, DWORD &buf1Size, unsigned char *& buf2, DWORD &buf2Size, 
		unsigned char * data, DWORD size);
	void GetNextAudioData(double secs);

	void Play();
	void stopInternal();
	void Seek();
	void CalcCurTime();

	bool isPlaying_;
	bool terminate_;
	boost::thread decodeThread_;
	
	void LoadFile(const char * fileName);

	AVFormatContext * aFormatCtx_;
	int audioStream;

	double sampleRate_;
	double timeBase_;
	double duration_;

	AVFormatContext * formatCtx_;
	AVCodecContext * aCodecCtx;
	AVCodec * aCodec;
	
	mutable boost::mutex soundMutex_;
	int tempAudioBufferSize_;
	::uint8_t * tempAudioBuffer_;	
	unsigned int tempAudioBufferPos_;
	
	double pts_;


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
	double curTime_;

	unsigned int numDecodeAfterSeek_;

	bool repeat_;
};


void initVideoEnv(HWND hwnd);

void uninitVideoEnv();