#ifndef AudioPlayer_h__
#define AudioPlayer_h__

class Audio;

class AudioPlayer
{
public:
	AudioPlayer() {}
	virtual ~AudioPlayer() {}

	virtual void play(const std::string & source, float startTime) = 0;
	virtual void stopAll() = 0;

	virtual void resumeAll() = 0;
	virtual void pauseAll() = 0;

	virtual void play(int id, const std::string & source, bool repeat, float startTime) = 0;	
	virtual void stop(int id) = 0;
	virtual void pause(int id) = 0;
	virtual void resume(int id) = 0;
	virtual float currentTime(int id) = 0;
	virtual void setVolume(int id, float val) = 0;
	virtual void setAllVolume(float val) = 0;


	virtual void startRecord() = 0;
	virtual void endRecord() = 0;
	virtual void playRecorded(int id, bool repeat) = 0;
	virtual float curRecordVolume() = 0;


private:

};


#endif // AudioPlayer_h__