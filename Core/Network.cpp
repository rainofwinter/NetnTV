#include "stdafx.h"
#include "Network.h"
#include "Exception.h"
#include <curl/curl.h>

using namespace std;


////////////////////////////////////////////////////////////////////////////////

NetworkRequest::NetworkRequest()
{
	pthread_mutex_init(&mutex_, 0);
	cancel_ = false;
	file_ = 0;
	resumeDownload_ = false;
	resumePos_ = 0;	
	statusSuccessful_ = false;
	statusStarted_ = false;
	timeout_ = 0;
}

NetworkRequest::~NetworkRequest()
{
	abort();
	pthread_mutex_destroy(&mutex_);
}

/*
std::string NetworkRequest::responseText() const
{
	std::vector<jschar> jschars;
	utf8::utf8to16(buffer_.begin(), buffer_.end(), back_inserter(jschars));	
	return jschars;
}*/

size_t NetworkRequest::writeDataCallback(void *buffer, size_t size, size_t nmemb, void *userp)
{
	WriteFuncData * writeFuncData = (WriteFuncData *)userp;
	vector<unsigned char> & recvBuffer = *writeFuncData->buffer;
	NetworkRequest * x = writeFuncData->x;
	size_t written = 0;
	pthread_mutex_lock(&x->mutex_);
	if (!x->file_)
	{
		size_t startIndex = recvBuffer.size();	
		recvBuffer.resize(recvBuffer.size() + size*nmemb);
		memcpy(&recvBuffer[startIndex], buffer, size*nmemb);	
		written = size * nmemb;
	}
	else
	{
		written = fwrite(buffer, size, nmemb, x->file_);
	}
	pthread_mutex_unlock(&x->mutex_);

	return written;
}

int NetworkRequest::progressCallback(
	void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	int ret = 0;
	NetworkRequest * x = (NetworkRequest *)clientp;	

	pthread_mutex_lock(&x->mutex_);	
	Progress event;
	event.lengthComputable = dltotal > 0;
	event.loaded = dlnow + x->resumePos_;
	event.total = dltotal + x->resumePos_;
	x->progress_ = event;

	if (x->cancel_)
    {
        unlink(x->fileName_.c_str());
        ret = -1;
    }
	pthread_mutex_unlock(&x->mutex_);	
	return ret;
}

#ifdef MSVC
#include <boost/thread.hpp>
#else
#include <unistd.h>
#endif

void * NetworkRequest::run(void * arg)
{
	int flag;
	NetworkRequest * x = (NetworkRequest *)arg;

	pthread_mutex_lock(&x->mutex_);
	x->buffer_.clear();
	x->resumePos_ = 0;
	if (x->file_ && x->resumeDownload_)
	{		
		fseek(x->file_, 0, SEEK_END);
		x->resumePos_ = ftell(x->file_);
	}
	pthread_mutex_unlock(&x->mutex_);

	CURL * curl = curl_easy_init();
	CURLcode code;
	curl_easy_setopt(curl, CURLOPT_URL, x->url_.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NetworkRequest::writeDataCallback);	
	WriteFuncData d;
	d.x = x;
	d.buffer = &x->buffer_;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &d); 	
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, NetworkRequest::progressCallback);	
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, x);	
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
	curl_easy_setopt(curl, CURLOPT_BUFFERSIZE,64000);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);	
	curl_easy_setopt(curl, CURLOPT_RESUME_FROM, x->resumePos_);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, ~0);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, x->timeout());
	code = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &flag);

	curl_easy_cleanup(curl);
	/*
	if (code != 0) x->status_ = 0;
	else x->status_ = 1;
	*/

	pthread_mutex_lock(&x->mutex_);	
	if (x->file_) 
	{
		fclose(x->file_);
		x->file_ = 0;
	}

	if (code != 0)
	{
		int ret = unlink(x->fileName_.c_str());
		LOGI("unlinking: %s, result: %d", x->fileName_.c_str(), ret);
		x->statusSuccessful_ = false;		
	}
	else
	{
		x->statusSuccessful_ = true;		
	}

	if (flag == 404 && flag == 204)
	{
        unlink(x->fileName_.c_str());
		x->statusSuccessful_ = false;
		x->fileExists_ = false;
	}
	else if (flag / 100 >= 4 || flag == 0)
	{
		unlink(x->fileName_.c_str());
		x->statusSuccessful_ = false;
		x->fileExists_ = true;
	}
	else
		x->fileExists_ = true;

	pthread_mutex_unlock(&x->mutex_);

	x->statusStarted_ = false;
	return 0;
}


void NetworkRequest::abort()
{	
	if (statusStarted_)
	{

		cancel_ = true;
		pthread_join(thread_, 0);
		cancel_ = false;
	}
	
}

void NetworkRequest::start(const std::string & url)
{
	abort();
	statusStarted_ = true;

	url_ = url;
	if (!fileName_.empty())
	{
		if (resumeDownload_)		
			file_ = fopen(fileName_.c_str(), "ab");
		else
			file_ = fopen(fileName_.c_str(), "wb");

		if (!file_)
		{
			throw Exception("NetworkRequest::send - Could not open file.");
		}
		long size = ftell(file_);
	}
	
	pthread_create(&thread_, 0, NetworkRequest::run, this);
}


NetworkRequest::Progress NetworkRequest::progress()
{
	Progress ret;

	pthread_mutex_lock(&mutex_);
	ret = progress_;
	pthread_mutex_unlock(&mutex_);

	return ret;
}