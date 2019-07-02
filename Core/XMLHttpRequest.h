#pragma once
#include <pthread.h>
class ScriptProcessor;

struct ProgressEvent
{
	double loaded, total;
	bool lengthComputable;
};

class XMLHttpRequest
{
	friend class ScriptProcessor;
	friend JSBool XMLHttpRequest_ctor(JSContext *cx, uintN argc, jsval *vp);
public:


	static JSObject * createScriptObjectProto(ScriptProcessor * s, JSObject * global);
	static void gcMark(JSTracer * trc, JSObject * obj);
	
	~XMLHttpRequest();
	JSObject * onReadyStateChange() const {return onReadyStateChange_;}
	void setOnReadyStateChange(JSObject * obj);

	void send(const std::string & body);
	void open(const std::string & method, const std::string & url);

	void addEventListener(const std::string & event, JSObject * handler);
	void fireEvent(const std::string & event, JSObject * evtObject);

	void update(float time);

	static void * run(void * arg);

	static int progressCallback(void *clientp, 
		double dltotal, double dlnow, 
		double ultotal, double ulnow);

	static size_t writeDataCallback(
		void *buffer, size_t size, size_t nmemb, void *userp);

	const std::string & file() const {return fileName_;}
	void setFile(const std::string & file) {fileName_ = file;}

	bool resume() const {return resumeDownload_;}
	void setResume(bool val) {resumeDownload_ = val;}

	int status() const {return status_;}

	void abort();

	std::vector<jschar> responseText() const;
	int readyState() const {return readyState_;}

private:
	XMLHttpRequest(ScriptProcessor * s, JSObject * jsObjForThis);
	ScriptProcessor * s_;
	JSObject * jsObjForThis_;

	JSObject * onReadyStateChange_;

	enum Method
	{
		GET, POST
	} method_;
	std::string url_;
	std::string body_;
	
	std::string fileName_;
	bool resumeDownload_;

	FILE * file_;
	long resumePos_;

	std::vector<unsigned char> buffer_;

	typedef std::multimap<std::string, JSObject *> EventListeners;
	EventListeners eventListeners_;
	
	bool isQueuedProgressEvent_;
	ProgressEvent queuedProgressEvent_;
	std::vector<int> queuedReadyStateChanges_;


	int readyState_;
	bool cancel_;
	bool threadRunning_;

	/**
	0 - download failed
	1 - download succeeded
	*/
	int status_;

	pthread_t thread_;
	pthread_cond_t cond_;
	pthread_mutex_t mutex_;

	float lastUpdateTime_;
};
