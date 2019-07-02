#include "stdafx.h"

#include "AudioMedia.h"
#include "Exception.h"
#include "GfxRenderer.h"

using namespace std;
using namespace boost;

extern LPDIRECTSOUND8 gDirectSound8;


AudioMedia::AudioMedia(const char * fileName, bool repeat) 
: audioStream(-1), aCodecCtx(0), aFormatCtx_(0), aCodec(0), 
isPlaying_(0), terminate_(0), pts_(-1),
dSoundBuffer_(0), repeat_(repeat)
{

	curTime_ = 0;
	timeOffset_ = 0;

	dSoundUpdateSecs_ = 1;
	dSoundMargins_ = 1;
	dSoundBufferPos_ = 0;
	tempAudioBuffer_ = (::uint8_t*)av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);
	memset(tempAudioBuffer_, 9, AVCODEC_MAX_AUDIO_FRAME_SIZE);
	tempAudioBufferSize_ = 0;
	tempAudioBufferPos_ = 0;

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

	lastSoundUpdateTime_ = 0;
	numDecodeAfterSeek_ = 0;

	//start decoding
	decodeThread_ = thread(boost::ref(*this));	
}


AudioMedia::~AudioMedia()
{
	terminate_ = true;
	decodeThread_.join();	
	
	if (aFormatCtx_) av_close_input_file(aFormatCtx_);
	if (dSoundBuffer_) 
	{
		dSoundBuffer_->Stop();
		dSoundBuffer_->Release();
	}
	if (tempAudioBuffer_) av_free(tempAudioBuffer_);
}


void AudioMedia::LoadFile(const char * fileNameA)
{
	int err;
	
	err = av_open_input_file(&aFormatCtx_, fileNameA, NULL, 0, NULL);
	if (err != 0)
		throw std::runtime_error("Couldn't open video stream");
	//trackID = 15;

	err = av_find_stream_info(aFormatCtx_);	
	if(err < 0 )
		throw std::runtime_error("Couldn't find stream info");

	int i;

	// Find the first audio stream
	for(i=0; i<(int)aFormatCtx_->nb_streams; i++)
		if(aFormatCtx_->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO) {
			audioStream=i;
			break;
		}
	
		
	if (audioStream < 0) throw std::runtime_error("Couldn't find audio stream");		

	aCodecCtx = aFormatCtx_->streams[audioStream]->codec;

	//aCodecCtx->sample_rate is sometimes 0 for some reason so calculate the 
	//sample rate like this:
	sampleRate_ = aCodecCtx->sample_rate;
	if (sampleRate_ == 0)
	{
		sampleRate_ = 
			(double)aFormatCtx_->streams[audioStream]->time_base.den
			/ aFormatCtx_->streams[audioStream]->time_base.num;
	}
	timeBase_ = 1.0 / av_q2d(aFormatCtx_->streams[audioStream]->time_base);
	duration_ = 
		(double)aFormatCtx_->streams[audioStream]->duration / 
		timeBase_;		

	// Find the decoder for the video stream
	aCodec=avcodec_find_decoder(aCodecCtx->codec_id);
	if(aCodec==NULL) {
		throw std::runtime_error("Unsupported audio codec");
	}
	// Open codec
	if(avcodec_open(aCodecCtx, aCodec)<0)
		throw std::runtime_error("Couldn't open audio codec");
	
}

void AudioMedia::Play(double time)
{	
	lock_guard<mutex> lk(soundMutex_);
	UINT64 ticks;
	QueryPerformanceCounter((LARGE_INTEGER *)&ticksAtPlayStart_);
	timeOffset_ = time;

	CalcCurTime();	
	Seek();
	Play();
}

void AudioMedia::Resume()
{
	lock_guard<mutex> lk(soundMutex_);
	UINT64 ticks;
	QueryPerformanceCounter((LARGE_INTEGER *)&ticksAtPlayStart_);
	timeOffset_ = curTime_;

	CalcCurTime();	
	Seek();
	Play();
}

void AudioMedia::Play()
{
	isPlaying_ = true;	
	
	if (dSoundBuffer_)
	{
		dSoundBufferPos_ = 0;
		lastSoundUpdateTime_ = curTime_;
		tempAudioBufferPos_ = 0;
		tempAudioBufferSize_ = 0;
	
	
		GetNextAudioData(dSoundUpdateSecs_ + dSoundMargins_);
	
		dSoundBuffer_->SetCurrentPosition(0);
		dSoundBuffer_->Play(0, 0, DSBPLAY_LOOPING);
	}
}

void AudioMedia::zeroSoundBuffer()
{
	unsigned char * lockedBuffer1 = 0;
	DWORD lockedBufferSize1;
	HRESULT hr;
	
	hr = dSoundBuffer_->Lock(
		0, dSoundBufferSize_, 
		(void **)&lockedBuffer1, &lockedBufferSize1, 
		0, 0, 0);

	memset(lockedBuffer1, 0, lockedBufferSize1);

	hr = dSoundBuffer_->Unlock(
		lockedBuffer1, lockedBufferSize1, 0, 0);
}
void AudioMedia::Seek()
{
	numDecodeAfterSeek_ = 0;
	::int64_t seek_target = curTime_ * timeBase_;
	int seekFlags = AVSEEK_FLAG_ANY;
	lastSoundUpdateTime_ = curTime_;
	av_seek_frame(aFormatCtx_, audioStream, seek_target, 0);
	
}

void AudioMedia::Stop()
{
	lock_guard<mutex> lk(soundMutex_);
	stopInternal();	
}

void AudioMedia::stopInternal()
{
	isPlaying_ = false;
	if (dSoundBuffer_)
	{		
		HRESULT hr = dSoundBuffer_->Stop();
	}
}

bool AudioMedia::isEnded() const
{
	lock_guard<mutex> lk(soundMutex_);
	return curTime_ >= duration_;
}

double AudioMedia::GetDuration() const
{
	lock_guard<mutex> lk(soundMutex_);
	return duration_;
}

double AudioMedia::curTime() const
{
	lock_guard<mutex> lk(soundMutex_);
	if (isPlaying_)
		return curTime_;
	else return 0.0;
}

bool AudioMedia::isPlaying() const
{
	lock_guard<mutex> lk(soundMutex_);
	return isPlaying_;
}

void AudioMedia::SetVolume(double volume)
{	
	lock_guard<mutex> lk(soundMutex_);
	if(dSoundBuffer_)
	{		
		dSoundBuffer_->SetVolume(DSBVOLUME_MIN + volume*(DSBVOLUME_MAX - DSBVOLUME_MIN));
	}
		
}


DWORD AudioMedia::CopyToSoundBuffer(
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

void AudioMedia::GetNextAudioData(double secs)
{
	unsigned char * lockedBuffer1 = 0, * lockedBuffer2 = 0;
	DWORD lockedBufferSize1, lockedBufferSize2;
	HRESULT hr;
	
	int len = secs * dSoundBytesPerSec_;

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

			/**
			mute undesirable popping noise right after a seek
			*/
			if (numDecodeAfterSeek_ < 2)
			{
				memset(tempAudioBuffer_, 0, tempAudioBufferSize_);		
				numDecodeAfterSeek_++;
			}

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


void AudioMedia::CalcCurTime()
{
	UINT64 ticks;
	QueryPerformanceCounter((LARGE_INTEGER *)&ticks);
	curTime_ = 
		double(ticks - ticksAtPlayStart_) / double(ticksPerSecond_) + 
		timeOffset_;
	if (curTime_ > duration_) curTime_ = duration_;
}

void AudioMedia::operator()()
{	
	SetPriorityClass(GetCurrentThread(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	while(1) //main loop
	{		
		this_thread::sleep(posix_time::milliseconds(10));		
	
		lock_guard<mutex> lk(soundMutex_);
		if (terminate_) break;
		if (!isPlaying_) continue;		
		CalcCurTime();
		if (curTime_ >= duration_) 
		{			
			if (repeat_)
			{
				UINT64 ticks;
				QueryPerformanceCounter((LARGE_INTEGER *)&ticksAtPlayStart_);
				timeOffset_ = 0.0;

				CalcCurTime();	
				Seek();
				Play();
			}
			else
			{
				stopInternal();
			}
		}
		
		if (curTime_ - lastSoundUpdateTime_ > dSoundUpdateSecs_)
		{
			if (dSoundBuffer_)
				GetNextAudioData(dSoundUpdateSecs_);
			lastSoundUpdateTime_ += dSoundUpdateSecs_;
		}		
		
			
	}//end main loop

}

