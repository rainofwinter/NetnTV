#include "stdafx.h"
#include "PcAudioPlayer.h"
#include "AudioMedia.h"
#include "AudioObject.h"
#include "Document.h"

#include <boost/scope_exit.hpp>

#define MAX_BUFFERS 4

using namespace boost;
using namespace std;

PcAudioPlayer::PcAudioPlayer(Document * document)
{
	document_ = document;
	isRecording_ = false;	
	hWaveIn_ = NULL;
	hRecFile_ = NULL;
	waveHeaders_.resize(MAX_BUFFERS);

	//delete recorded audio file
	string fileName = recGetFileName();
	wstring wFileName(fileName.begin(), fileName.end());
	DeleteFile(wFileName.c_str());	
}

PcAudioPlayer::~PcAudioPlayer()
{
	if (isRecording_) endRecord();
	stopAll();	

	//delete recorded audio file
	string fileName = recGetFileName();
	wstring wFileName(fileName.begin(), fileName.end());
	DeleteFile(wFileName.c_str());	
}

void PcAudioPlayer::update()
{
	vector<AudioMedia *>::iterator iter = playingAudio_.begin();
	for(; iter != playingAudio_.end();)
	{
		AudioMedia * audio = *iter;
		if (audio->isEnded())
		{
			delete audio;
			iter = playingAudio_.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	map<int, AudioMedia *> audioObjectAudio = audioObjectAudio_;
	map<int, AudioMedia *>::iterator itr = audioObjectAudio.begin();

	for (; itr != audioObjectAudio.end(); ++itr)
	{
		AudioMedia * audio = (*itr).second;
		int id = (*itr).first;
		if (audio->isEnded())
		{
			delete audio;			
			audioObjectAudio_.erase(id);
			document_->audioStoppedEvent((*itr).first);
		}
	}
}

void PcAudioPlayer::play(const std::string & source, float startTime)
{
	try {
	
	AudioMedia * audio = new AudioMedia(source.c_str());
	audio->Play(startTime);
	playingAudio_.push_back(audio);

	} catch(...) 
	{
	}
}

void PcAudioPlayer::setVolume(int id, float val)
{
	map<int, AudioMedia *>::iterator iter = audioObjectAudio_.find(id);
	if (iter != audioObjectAudio_.end())
	{
		AudioMedia * audio = (*iter).second;
		audio->SetVolume(val);		
	}
}


void PcAudioPlayer::play(int id, const std::string & source, bool repeat, float startTime)
{
	
	try {
	
	map<int, AudioMedia *>::iterator iter = audioObjectAudio_.find(id);
	if (iter != audioObjectAudio_.end())
	{
		AudioMedia * oldAudio = (*iter).second;
		audioObjectAudio_.erase(iter);
		delete oldAudio;		
	}
	
	AudioMedia * audio = new AudioMedia(source.c_str(), repeat);	
	AudioObject * obj = (AudioObject *)Global::instance().idToPtr(id);
	audio->SetVolume(obj->volume());
	audio->Play(startTime);	
	audioObjectAudio_[id] = audio;
	document_->audioPlayedEvent(id);

	} catch(const std::exception & e) {
		QMessageBox::information(0, "Error", e.what());
	}
}

void PcAudioPlayer::stop(int id)
{	
	try {
	
	map<int, AudioMedia *>::iterator iter = audioObjectAudio_.find(id);
	if (iter != audioObjectAudio_.end())
	{
		AudioMedia * audio = (*iter).second;
		audioObjectAudio_.erase(iter);
		delete audio;
		document_->audioStoppedEvent(id);
	}

	} catch(const std::exception & e) {
		QMessageBox::information(0, "Error", e.what());
	}
}

void PcAudioPlayer::pause(int id)
{	
	try {
	
	map<int, AudioMedia *>::iterator iter = audioObjectAudio_.find(id);
	if (iter != audioObjectAudio_.end())
	{
		AudioMedia * audio = (*iter).second;
		audio->Stop();
		document_->audioPausedEvent(id);
	}

	} catch(...) 
	{
	}
}

void PcAudioPlayer::resume(int id)
{	
	try {
	
	map<int, AudioMedia *>::iterator iter = audioObjectAudio_.find(id);
	if (iter != audioObjectAudio_.end())
	{
		AudioMedia * audio = (*iter).second;
		audio->Resume();
		document_->audioResumedEvent(id);
	}

	} catch(...) 
	{
	}
}

float PcAudioPlayer::currentTime(int id)
{
	try {
	
	map<int, AudioMedia *>::iterator iter = audioObjectAudio_.find(id);
	if (iter != audioObjectAudio_.end())
	{
		AudioMedia * audio = (*iter).second;
		return (float)audio->curTime();
	}	

	} catch(...) 
	{
	}
	return 0.0f;
}

void PcAudioPlayer::stopAll()
{	
	for (int i = 0; i < (int)playingAudio_.size(); ++i)
	{
		delete playingAudio_[i];
	}
	playingAudio_.clear();

	map<int, AudioMedia *>::iterator iter;
	map<int, AudioMedia *> audioObjectAudio = audioObjectAudio_;
	audioObjectAudio_.clear();

	for (iter = audioObjectAudio.begin(); iter != audioObjectAudio.end(); ++iter)
	{
		AudioMedia * audio = (*iter).second;
		delete audio;
		document_->audioStoppedEvent((*iter).first);
	}
	
}

void PcAudioPlayer::pauseAll()
{	
	for (int i = 0; i < (int)playingAudio_.size(); ++i)
	{
		playingAudio_[i]->Stop();		
	}

	map<int, AudioMedia *>::iterator iter;
	map<int, AudioMedia *> audioObjectAudio = audioObjectAudio_;
	audioObjectAudio_.clear();

	for (iter = audioObjectAudio.begin(); iter != audioObjectAudio.end(); ++iter)
	{
		AudioMedia * audio = (*iter).second;
		audio->Stop();
		document_->audioPausedEvent((*iter).first);
	}
}

void PcAudioPlayer::resumeAll()
{	
	for (int i = 0; i < (int)playingAudio_.size(); ++i)
	{
		playingAudio_[i]->Resume();		
	}

	map<int, AudioMedia *>::iterator iter;
	map<int, AudioMedia *> audioObjectAudio = audioObjectAudio_;

	for (iter = audioObjectAudio.begin(); iter != audioObjectAudio.end(); ++iter)
	{
		AudioMedia * audio = (*iter).second;
		audio->Resume();
		document_->audioResumedEvent((*iter).first);
	}	
}


/*
void PcAudioPlayer::RecDeviceInfo::addRecMode(int samples, int channels, int bits)
{
	WAVEFORMATEX waveFormatEx;	
	int blockAlign = channels * bits / 8;

	waveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
	waveFormatEx.nChannels = channels;	
	waveFormatEx.nSamplesPerSec = samples;		
	waveFormatEx.nAvgBytesPerSec = samples * blockAlign;	
	waveFormatEx.nBlockAlign = channels * bits/8;
	waveFormatEx.wBitsPerSample = bits;
	waveFormatEx.cbSize = sizeof(WAVEFORMATEX);

	modes.push_back(waveFormatEx);
}
*/
void CALLBACK waveInProc(HWAVEIN hwi,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{

	PcAudioPlayer *pObj=(PcAudioPlayer *)dwInstance;
	switch(uMsg)
	{
		case WIM_DATA:			
				pObj->recProcessHeader((WAVEHDR *)dwParam1);
			break;
	}
}

WAVEFORMATEX PcAudioPlayer::getWaveFormat(int samples, int channels, int bits) const
{
	WAVEFORMATEX waveFormatEx;	
	int blockAlign = channels * bits / 8;

	waveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
	waveFormatEx.nChannels = channels;	
	waveFormatEx.nSamplesPerSec = samples;		
	waveFormatEx.nAvgBytesPerSec = samples * blockAlign;	
	waveFormatEx.nBlockAlign = channels * bits/8;
	waveFormatEx.wBitsPerSample = bits;
	waveFormatEx.cbSize = sizeof(WAVEFORMATEX);

	return waveFormatEx;
}

VOID PcAudioPlayer::recProcessHeader(WAVEHDR * pHdr)
{
	//TODO if the wav file is too big or too long do an endRecord()
	MMRESULT mRes=0;
	if(pHdr->dwFlags & WHDR_DONE)
	{
		short * sampleBuffer = (short *)pHdr->lpData;
		unsigned int numBits = pHdr->dwBytesRecorded / 2;

		double rmsAmplitude = 0;
		for (unsigned int i = 0; i < numBits; ++i) 
		{
			rmsAmplitude += (sampleBuffer[i] * sampleBuffer[i])/numBits;
		}

		rmsAmplitude = sqrt(rmsAmplitude);
		double maxAmplitude = SHRT_MAX;
		recCurVolume_ = (float)(rmsAmplitude / maxAmplitude);

		mmioWrite(hRecFile_,pHdr->lpData,pHdr->dwBytesRecorded);
		mRes=waveInAddBuffer(hWaveIn_,pHdr,sizeof(WAVEHDR));			
	}
}

std::string PcAudioPlayer::recGetFileName() const
{
	return Global::instance().writeDirectory() + "__recordFile.wav";
}

float PcAudioPlayer::curRecordVolume()
{
	/*
	if (!isRecording_ || !hMixer_) return 0.0f;
	MMRESULT rc;
	rc = mixerGetControlDetails((HMIXEROBJ)hMixer_, &mxcd_,
                                 MIXER_GETCONTROLDETAILSF_VALUE);
    if (rc != MMSYSERR_NOERROR) {
		return 0.0f;
    }
    long volume = volStruct_.lValue;
    // Get the absolute value of the volume.
    if (volume < 0) volume = -volume;
	//cout << "min: " << mxcs_[1].Bounds.lMinimum << " max: " << mxcs_[1].Bounds.lMaximum << " vol: " << volume << "\n";
	return (float)volume;
	*/

	if (!isRecording_) return 0.0f;
	return recCurVolume_;

}

void PcAudioPlayer::startRecord()
{
	if (isRecording_) endRecord();

	MMRESULT mRes;
	/*
	UINT numMixerDevs = mixerGetNumDevs();
	for (UINT id = 0; id < numMixerDevs; ++id)
	{
		mRes = mixerOpen(&hMixer_, id,0,0,0);
		if (MMSYSERR_NOERROR != mRes) {
			// Couldn't open the mixer.
		}

		MIXERCAPS mixerCaps;
		mixerGetDevCaps(id, &mixerCaps, sizeof(MIXERCAPS));
		bool foundSuitable = false;
		for (UINT destination = 0; destination < mixerCaps.cDestinations; destination++)
		{
			ZeroMemory(&mxl_,sizeof(mxl_));
			mxl_.cbStruct = sizeof(mxl_);
			mxl_.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
			mxl_.dwDestination = destination;
			mRes = mixerGetLineInfo((HMIXEROBJ)hMixer_, &mxl_,
								   MIXER_GETLINEINFOF_COMPONENTTYPE);


			if (mRes == MMSYSERR_NOERROR && mxl_.cControls) 
			{
				foundSuitable = true;
				break;
			}
	
		}
		if (foundSuitable) break;

		mixerClose(hMixer_);
		hMixer_ = NULL;
	}

	if (hMixer_)
	{
		// Get the control.
		ZeroMemory(&mxlc_, sizeof(mxlc_));
		mxlc_.cbStruct = sizeof(mxlc_);
		mxlc_.dwLineID = mxl_.dwLineID;
		mxlc_.dwControlType = MIXERCONTROL_CONTROLTYPE_PEAKMETER;
		mxlc_.cControls = mxl_.cControls;

		mxcs_.resize(mxl_.cControls);
		for (int i = 0; i < mxl_.cControls; ++i)
		{
			ZeroMemory(&mxcs_[i], sizeof(MIXERCONTROL));
			mxcs_[i].cbStruct = sizeof(MIXERCONTROL);
		}

		mxlc_.cbmxctrl = sizeof(MIXERCONTROL);
		mxlc_.pamxctrl = &mxcs_[0];

		mRes = mixerGetLineControls((HMIXEROBJ)hMixer_,&mxlc_,
								   MIXER_GETLINECONTROLSF_ALL);
		if (MMSYSERR_NOERROR != mRes) {
			// Couldn't get the control.
		}



		// Initialize the MIXERCONTROLDETAILS structure
		ZeroMemory(&mxcd_, sizeof(mxcd_));
		mxcd_.cbStruct = sizeof(mxcd_);
		mxcd_.cbDetails = sizeof(volStruct_);
		mxcd_.dwControlID = mxcs_[1].dwControlID;
		mxcd_.paDetails = &volStruct_;
		mxcd_.cChannels = 1; 
	}
*/

	/*
	recGetDevicesData();
	if (recDevices_.empty()) return;

	isRecording_ = true;
	int deviceIndex = 0;
	int modeIndex = 0;
	int numModes = (int)recDevices_[deviceIndex].modes.size();
	for (int i = 0; i < numModes; ++i)
	{
		if (recDevices_[deviceIndex].modes[i].nChannels == 1 &&
			recDevices_[deviceIndex].modes[i].nSamplesPerSec == 44100 &&
			recDevices_[deviceIndex].modes[i].wBitsPerSample == 16
			)
			modeIndex = i;
	}

	waveFormat_ = recDevices_[deviceIndex].modes[modeIndex];
	*/
	recCurVolume_ = 0.0f;
	waveFormat_ = getWaveFormat(16000, 1, 16);

	//waveInOpen(&hWaveIn_, deviceIndex, &waveFormat_, (DWORD_PTR)waveInProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
	mRes = waveInOpen(&hWaveIn_, WAVE_MAPPER, &waveFormat_, (DWORD_PTR)waveInProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
	if (mRes != MMSYSERR_NOERROR ) return;
	

	
	string fileName = recGetFileName();
	wstring wFileName(fileName.begin(), fileName.end());
	DeleteFile(wFileName.c_str());	

	memset(&mmioInfo_, 0, sizeof(MMIOINFO));
	hRecFile_ = mmioOpen((LPWSTR)wFileName.c_str(), &mmioInfo_, MMIO_WRITE | MMIO_CREATE);

	memset(&stckOutRIFF_, 0, sizeof(MMCKINFO));
	stckOutRIFF_.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmioCreateChunk(hRecFile_, &stckOutRIFF_, MMIO_CREATERIFF);

	memset(&stckOut_, 0, sizeof(MMCKINFO));
	stckOut_.ckid = mmioFOURCC('f', 'm', 't', ' ');
	stckOut_.cksize = sizeof(waveFormat_);
	
	mRes = mmioCreateChunk(hRecFile_, &stckOut_, 0);
	mRes = mmioWrite(hRecFile_, (HPSTR) &waveFormat_, sizeof(waveFormat_));
	mRes = mmioAscend(hRecFile_, &stckOut_, 0);
	stckOut_.ckid = mmioFOURCC('d', 'a', 't', 'a');	
	mRes = mmioCreateChunk(hRecFile_, &stckOut_, 0);
		
	recPrepareBuffers();
	mRes = waveInStart(hWaveIn_);

	isRecording_ = true;
}

void PcAudioPlayer::recPrepareBuffers()
{
	int samplesPerSec = 5;
	recUnprepareBuffers();
	for (int i = 0; i < MAX_BUFFERS; ++i)
	{
		WAVEHDR waveHdr;
		memset(&waveHdr, 0, sizeof(WAVEHDR));
		waveHdr.lpData = 
			//(LPSTR)HeapAlloc(GetProcessHeap(),8,waveFormat_.nAvgBytesPerSec);
			(LPSTR)malloc(waveFormat_.nAvgBytesPerSec/samplesPerSec);
		waveHdr.dwBufferLength = waveFormat_.nAvgBytesPerSec/samplesPerSec;
		waveHdr.dwUser = i;
		waveHeaders_.push_back(waveHdr);

		waveInPrepareHeader(hWaveIn_, &waveHeaders_[i], sizeof(WAVEHDR));
		waveInAddBuffer(hWaveIn_, &waveHeaders_[i], sizeof(WAVEHDR));		
	}
}

void PcAudioPlayer::recUnprepareBuffers()
{
	for (int i = 0; i < (int)waveHeaders_.size(); ++i)
	{
		waveInUnprepareHeader(hWaveIn_, &waveHeaders_[i], sizeof(WAVEHDR));
		//HeapFree(GetProcessHeap(), 0, waveHeaders_[i].lpData);
		free(waveHeaders_[i].lpData);		

	}
	waveHeaders_.clear();
}
/*
void PcAudioPlayer::recGetDevicesData()
{
	MMRESULT mRes;
	WAVEINCAPS waveInCaps = {0};

	UINT numDevices;
	numDevices = waveInGetNumDevs();

	recDevices_.clear();
	for (UINT i = 0; i < numDevices; ++i)
	{
		RecDeviceInfo info;

		memset(&waveInCaps, 0, sizeof(WAVEINCAPS));
		mRes = waveInGetDevCaps(i, &waveInCaps, sizeof(WAVEINCAPS));
		//info for each wave in device

		//device name
		info.name = waveInCaps.szPname;
		//supported formats
		if (waveInCaps.dwFormats & WAVE_FORMAT_1M08) info.addRecMode(11025, 1, 8);
		if (waveInCaps.dwFormats & WAVE_FORMAT_1S08) info.addRecMode(11025, 2, 8);
		if (waveInCaps.dwFormats & WAVE_FORMAT_1M16) info.addRecMode(11025, 1, 16);
		if (waveInCaps.dwFormats & WAVE_FORMAT_1S16) info.addRecMode(11025, 2, 16);

		if (waveInCaps.dwFormats & WAVE_FORMAT_2M08) info.addRecMode(22050, 1, 8);
		if (waveInCaps.dwFormats & WAVE_FORMAT_2S08) info.addRecMode(22050, 2, 8);
		if (waveInCaps.dwFormats & WAVE_FORMAT_2M16) info.addRecMode(22050, 1, 16);
		if (waveInCaps.dwFormats & WAVE_FORMAT_2S16) info.addRecMode(22050, 2, 16);
		
		if (waveInCaps.dwFormats & WAVE_FORMAT_4M08) info.addRecMode(44100, 1, 8);
		if (waveInCaps.dwFormats & WAVE_FORMAT_4S08) info.addRecMode(44100, 2, 8);
		if (waveInCaps.dwFormats & WAVE_FORMAT_4M16) info.addRecMode(44100, 1, 16);
		if (waveInCaps.dwFormats & WAVE_FORMAT_4S16) info.addRecMode(44100, 2, 16);

		if (waveInCaps.dwFormats & WAVE_FORMAT_96M08) info.addRecMode(96000, 1, 8);
		if (waveInCaps.dwFormats & WAVE_FORMAT_96S08) info.addRecMode(96000, 2, 8);
		if (waveInCaps.dwFormats & WAVE_FORMAT_96M16) info.addRecMode(96000, 1, 16);
		if (waveInCaps.dwFormats & WAVE_FORMAT_96S16) info.addRecMode(96000, 2, 16);
		recDevices_.push_back(info);
	}	

}
*/

void PcAudioPlayer::playRecorded(int id, bool repeat)
{
	if (isRecording_) endRecord();
	play(id, recGetFileName(), repeat, 0.0f);
}

void PcAudioPlayer::endRecord()
{
	isRecording_ = false;
	if (hWaveIn_)
	{
		waveInStop(hWaveIn_);
		recUnprepareBuffers();
		waveInClose(hWaveIn_);		
	}

	if (hRecFile_)
	{
		mmioAscend(hRecFile_, &stckOut_, 0);
		mmioAscend(hRecFile_, &stckOutRIFF_, 0);
		mmioClose(hRecFile_, 0);
		hRecFile_ = NULL;
	}

	hWaveIn_ = NULL;
/*
	if (hMixer_)
	{
		mixerClose(hMixer_);
		hMixer_ = NULL;
	}
	*/
}
