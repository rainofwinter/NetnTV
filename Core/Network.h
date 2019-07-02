#pragma once
#include <pthread.h>

class NetworkRequest
{
public:
	enum Status
	{
		StatusFailed,
		StatusSuccessful,
		StatusStarted,
		StatusInProgress,
	};

	struct Progress
	{
		Progress()
		{
			loaded = 0;
			total = 0;
			lengthComputable = false;
		}
		double loaded, total;
		bool lengthComputable;
	};


public:
	NetworkRequest();
	~NetworkRequest();

	void init();
	void uninit();

	
	/*
	void send();
	void open(const std::string & url);
	*/
	void start(const std::string & url);
	

	static void * run(void * arg);

	Progress progress();

	///if download has started or is in progress
	bool isRunning() const {return statusStarted_;}

	const std::string & file() const {return fileName_;}
	void setFile(const std::string & file) {fileName_ = file;}

	bool resume() const {return resumeDownload_;}
	void setResume(bool val) {resumeDownload_ = val;}

	void abort();
	void setTimeout(int var) {timeout_ = var;}
	int timeout() {return timeout_;}

	/**
	Caution: check isRunning() == false before calling this function,
	otherwise its return value is unreliable. Also make sure that start
	was indeed called sometime before.
	@return whether last download was successful
	*/
	bool wasSuccessful() const
	{
		return statusSuccessful_;
	}

	bool wasFileExist() const
	{
		return fileExists_;
	}

	//std::string responseText() const;

private:
	

	struct WriteFuncData
	{
		NetworkRequest * x;
		std::vector<unsigned char> * buffer;
	};
private:
	static int progressCallback(void *clientp, 
		double dltotal, double dlnow, 
		double ultotal, double ulnow);

	static size_t writeDataCallback(
		void *buffer, size_t size, size_t nmemb, void *userp);


private:
	bool init_;

	std::string url_;
	
	std::string fileName_;
	bool resumeDownload_;

	FILE * file_;
	long resumePos_;

	std::vector<unsigned char> buffer_;
	
	bool cancel_;
	

	Progress progress_;

	pthread_t thread_;
	pthread_mutex_t mutex_;


	/**
	true only when download failed 
	(== false does not necessarily indicated download succeeded)
	*/
	bool statusSuccessful_;
	bool statusStarted_;
	bool fileExists_;
	int timeout_;
};