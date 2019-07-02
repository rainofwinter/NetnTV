#pragma once

#include "AudioPlayer.h"

class AudioMedia;
class Document;


class PcAudioPlayerMutexLock
{
public:
	PcAudioPlayerMutexLock(boost::mutex & mutex);
	~PcAudioPlayerMutexLock();
	void lock_mutex();

private:
	boost::lock_guard<boost::mutex>  * lock_guard_;
	static bool sCurLocked_;
	static boost::thread::id sCurThreadId_;

};

class PcAudioPlayer : public AudioPlayer
{
	friend class PcPlayerSupport;
public:
	PcAudioPlayer(Document * document);
	virtual ~PcAudioPlayer();

	virtual void play(const std::string & source, float startTime);
	virtual void stopAll();

	virtual void pauseAll();
	virtual void resumeAll();

	virtual void play(int id, const std::string & source, bool repeat, float startTime);
	virtual void stop(int id);
	virtual void pause(int id);
	virtual void resume(int id);
	virtual float currentTime(int id);

	virtual void setVolume(int id, float val);
	virtual void setAllVolume(float val) {}

	virtual void startRecord();
	virtual void playRecorded(int id, bool repeat);
	virtual void endRecord();
	virtual float curRecordVolume();

	void update();

	bool isRecording() const {return isRecording_;}
	void recProcessHeader(WAVEHDR * pHdr);

private:
	void recGetDevicesData();	
	void recPrepareBuffers();
	void recUnprepareBuffers();
	std::string recGetFileName() const;

	WAVEFORMATEX getWaveFormat(int samples, int channels, int bits) const;

private:
	std::vector<AudioMedia *> playingAudio_;

	std::map<int, AudioMedia *> audioObjectAudio_;
	Document * document_;


	struct ScheduledEvent
	{
		enum Type {
			Stopped,
			Paused,
			Played,
			Resumed
		} type;

		int id;

		ScheduledEvent(Type type, int id)
		{
			this->type = type;
			this->id = id;
		}
	};

	bool isRecording_;	
	HMMIO hRecFile_;
	HWAVEIN hWaveIn_;
	WAVEFORMATEX waveFormat_;
	MMCKINFO stckOut_, stckOutRIFF_;	
	MMIOINFO mmioInfo_;

	/*
	HMIXER hMixer_;
	std::vector<MIXERCONTROL> mxcs_;
	MIXERLINE mxl_;
	MIXERLINECONTROLS mxlc_;
	MIXERCONTROLDETAILS mxcd_;
    MIXERCONTROLDETAILS_SIGNED volStruct_;
	*/
	float recCurVolume_;

	/*
	struct RecDeviceInfo
	{
		std::wstring name;
		std::vector<WAVEFORMATEX> modes;

		void addRecMode(int samples, int channels, int bits);

	};
	std::vector<RecDeviceInfo> recDevices_;
	*/

	std::vector<WAVEHDR> waveHeaders_;

	bool doingCallback_;
};
