#include "stdafx.h"
#include "XMLHttpRequest.h"
#include "ScriptProcessor.h"
#include "Exception.h"
#include "Global.h"
#include <curl/curl.h>
#include <utf8.h>

using namespace std;

void XMLHttpRequest_finalize(JSContext * cx, JSObject * obj)
{		
	XMLHttpRequest * privateData = (XMLHttpRequest *)JS_GetPrivate(cx, obj);
	delete privateData;
}

JSClass jsXmlHttpRequestClass = InitClass(
	"XMLHttpRequest", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, XMLHttpRequest_finalize, XMLHttpRequest::gcMark);

JSBool XMLHttpRequest_ctor(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_NewObject(cx, &jsXmlHttpRequestClass, 0, 0);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	XMLHttpRequest * x = new XMLHttpRequest(s, obj);
	JS_SetPrivate(cx, obj, x);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));

	return JS_TRUE;
}


//------------------------------------------------------------------------------

JSBool XMLHttpRequest_open(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	XMLHttpRequest * x = (XMLHttpRequest *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	
	const jschar * jsMethod, * jsUrl;
	if (!JS_ConvertArguments(cx, argc, argv, "WW", &jsMethod, &jsUrl)) 
		return JS_FALSE;

	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	string method = jsCharStrToStdString(jsMethod);
	string url = jsCharStrToStdString(jsUrl);

	x->open(method, url);

	return JS_TRUE;
};

JSBool XMLHttpRequest_send(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * jsThis = JS_THIS_OBJECT(cx, vp);
	XMLHttpRequest * x = (XMLHttpRequest *)JS_GetPrivate(cx, jsThis);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	std::string body;
	if (argc >= 1)
	{
		
		if (JSVAL_IS_STRING(argv[0]))
		{
			JSString * jstr = JSVAL_TO_STRING(argv[0]);
			const jschar * jsChars = JS_GetStringCharsZ(cx, jstr);
			body = jsCharStrToStdString(jsChars);			
		}
	}

	try {
	x->send(body);
	} catch (const Exception & e)
	{
		JS_ReportError(cx, e.what());
		return JS_FALSE;
	}


	return JS_TRUE;
};

JSBool XMLHttpRequest_addEventListener(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	XMLHttpRequest * x = (XMLHttpRequest *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsHandler;
	jschar * jsEvent;
	if (!JS_ConvertArguments(cx, argc, argv, "Wo", &jsEvent, &jsHandler)) return JS_FALSE;

	if (!JS_ObjectIsFunction(cx, jsHandler))
	{
		JS_ReportError(cx, "XMLHttpRequest::addEventListener - Parameter must be a function");
		return JS_FALSE;
	}

	std::string event = jsCharStrToStdString(jsEvent);

	x->addEventListener(event, jsHandler);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool XMLHttpRequest_abort(JSContext *cx, uintN argc, jsval *vp)
{
	XMLHttpRequest * x = (XMLHttpRequest *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	x->abort();
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;

};
JSFunctionSpec XMLHttpRequestFuncs[] = {
	JS_FS("open", XMLHttpRequest_open, 5, 0),
	JS_FS("send", XMLHttpRequest_send, 1, 0),
	JS_FS("abort", XMLHttpRequest_abort, 0, 0),

	JS_FS("addEventListener", XMLHttpRequest_addEventListener, 2, 0),

    JS_FS_END
};

//------------------------------------------------------------------------------

enum
{
	XMLHttpRequest_onreadystatechange,
	XMLHttpRequest_responseText,
	XMLHttpRequest_readyState,
	XMLHttpRequest_file,
	XMLHttpRequest_resume,
	XMLHttpRequest_status,
};


JSBool XMLHttpRequest_getProperty(
	JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{	
	XMLHttpRequest * x = (XMLHttpRequest *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	
	int nID = JSID_TO_INT(id);

	switch (nID) {
	case XMLHttpRequest_onreadystatechange:
		if (x->onReadyStateChange())
			JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(x->onReadyStateChange()));		
		else
			JS_SET_RVAL(cx, vp, JSVAL_NULL);
		break;			
	case XMLHttpRequest_responseText:
		{
			std::vector<jschar> jsChars = x->responseText();
			JS_SET_RVAL(cx, vp, 
				STRING_TO_JSVAL(JS_NewUCStringCopyN(cx, &jsChars[0], jsChars.size()))
				);
		}
		break;
	case XMLHttpRequest_readyState:
		JS_SET_RVAL(cx, vp, INT_TO_JSVAL(x->readyState()));
		break;
	case XMLHttpRequest_file:
		{
			JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(
				JS_NewStringCopyZ(cx, x->file().c_str())));
		}
		break;
	case XMLHttpRequest_resume:
		{
			JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(x->resume()));
		}
		break;
	case XMLHttpRequest_status:
		{
			JS_SET_RVAL(cx, vp, INT_TO_JSVAL(x->status()));
		}
		break;
	}

	return JS_TRUE;
}

JSBool XMLHttpRequest_setProperty(
	JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp)
{	
	XMLHttpRequest * x = (XMLHttpRequest *)JS_GetPrivate(cx, obj);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	int nID = JSID_TO_INT(id);

	switch (nID) {
	case XMLHttpRequest_onreadystatechange:
		
		if (JSVAL_IS_OBJECT(*vp))
		{
			JSObject * handler = JSVAL_TO_OBJECT(*vp);
			if (JS_ObjectIsFunction(cx, handler))
			{
				x->setOnReadyStateChange(handler);								
			}
		}

		break;	
	case XMLHttpRequest_file:
		if (JSVAL_IS_STRING(*vp))
		{
			JSString * jstr = JSVAL_TO_STRING(*vp);
			const jschar * jschars = JS_GetStringCharsZ(cx, jstr);
			x->setFile(jsCharStrToStdString(jschars));
		}
		break;
	case XMLHttpRequest_resume:
		if (JSVAL_IS_BOOLEAN(*vp))
		{		
			x->setResume((bool)JSVAL_TO_BOOLEAN(*vp));			
		}
		break;
	}
	return JS_TRUE;
}

JSPropertySpec XMLHttpRequestProps[] = {
	
	{"onreadystatechange",	XMLHttpRequest_onreadystatechange, 
	JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	XMLHttpRequest_getProperty, XMLHttpRequest_setProperty},

	{"responseText",	XMLHttpRequest_responseText, 
	JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_READONLY, 
	XMLHttpRequest_getProperty, JS_StrictPropertyStub},

	{"readyState",	XMLHttpRequest_readyState, 
	JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_READONLY, 
	XMLHttpRequest_getProperty, JS_StrictPropertyStub},

	{"file",	XMLHttpRequest_file, 
	JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	XMLHttpRequest_getProperty, XMLHttpRequest_setProperty},

	{"resume",	XMLHttpRequest_resume, 
	JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	XMLHttpRequest_getProperty, XMLHttpRequest_setProperty},

	{"status",	XMLHttpRequest_status, 
	JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_READONLY, 
	XMLHttpRequest_getProperty, JS_StrictPropertyStub},

	{0}
};

////////////////////////////////////////////////////////////////////////////////
struct WriteFuncData
{
	XMLHttpRequest * x;
	std::vector<unsigned char> * buffer;
};

JSObject * XMLHttpRequest::createScriptObjectProto(ScriptProcessor * s, JSObject * global)
{
	JSObject * proto = JS_InitClass(s->jsContext(), global, 
		0, &jsXmlHttpRequestClass, XMLHttpRequest_ctor, 0, 
		XMLHttpRequestProps, XMLHttpRequestFuncs,
		0, 0);	
	return proto;
}



void XMLHttpRequest::gcMark(JSTracer *trc, JSObject *obj)
{
	JSContext * cx = trc->context;
	XMLHttpRequest * x = (XMLHttpRequest *)JS_GetPrivate(cx, obj);
	if (x)
	{
		EventListeners::iterator iter;		
		for (iter = x->eventListeners_.begin(); iter != x->eventListeners_.end(); ++iter)
		{				
			trc->debugPrinter = JSTracerDummyDebugPrinter;
			JS_CallTracer(trc, (*iter).second, JSTRACE_OBJECT);			
		}
		if (x->onReadyStateChange_)
		{			
			trc->debugPrinter = JSTracerDummyDebugPrinter;
			JS_CallTracer(trc, x->onReadyStateChange_, JSTRACE_OBJECT);	
		}
	}
}

void XMLHttpRequest::addEventListener(const std::string & event, JSObject * handler)
{
	eventListeners_.insert(make_pair(event, handler));
}

void XMLHttpRequest::fireEvent(const std::string & event, JSObject * evtObject)
{
	typedef multimap<string, JSObject *>::iterator iterator;
	pair<iterator, iterator> range = eventListeners_.equal_range(event);

	jsval evtVal = OBJECT_TO_JSVAL(evtObject);
	for (iterator iter = range.first; iter != range.second; ++iter)
	{
		JSObject * handler = (*iter).second;
		jsval rval;
		jsval funcVal = OBJECT_TO_JSVAL(handler);
		
		JS_CallFunctionValue(
			s_->jsContext(), s_->jsGlobal(), funcVal, 1, &evtVal, &rval);
	}
	
}

XMLHttpRequest::XMLHttpRequest(ScriptProcessor * s, JSObject * jsObjForThis)
{
	s_ = s;
	onReadyStateChange_ = 0;
	pthread_mutex_init(&mutex_, 0);
	pthread_cond_init (&cond_, 0);
	cancel_ = false;
	file_ = 0;
	threadRunning_ = false;
	s_->addXmlHttpRequest(this);
	resumeDownload_ = false;
	resumePos_ = 0;
	status_ = 0;
	lastUpdateTime_ = -FLT_MAX;
	isQueuedProgressEvent_ = false;
	jsObjForThis_ = jsObjForThis;
}

XMLHttpRequest::~XMLHttpRequest()
{
	pthread_mutex_lock(&mutex_);
	cancel_ = true;
	while(threadRunning_)
	{
		pthread_cond_wait(&cond_, &mutex_);		
	}
	pthread_mutex_unlock(&mutex_);
	pthread_cond_destroy(&cond_);
	pthread_mutex_destroy(&mutex_);


	multimap<string, JSObject *>::iterator iter;
	for (iter = eventListeners_.begin(); iter != eventListeners_.end(); ++iter)
	{
		s_->unrootJSObject((*iter).second);
	}
	eventListeners_.clear();

	s_->unrootJSObject(onReadyStateChange_);
	s_->removeXmlHttpRequest(this);
}


std::vector<jschar> XMLHttpRequest::responseText() const
{
	std::vector<jschar> jschars;
	utf8::utf8to16(buffer_.begin(), buffer_.end(), back_inserter(jschars));	
	return jschars;
}

size_t XMLHttpRequest::writeDataCallback(void *buffer, size_t size, size_t nmemb, void *userp)
{
	WriteFuncData * writeFuncData = (WriteFuncData *)userp;
	vector<unsigned char> & recvBuffer = *writeFuncData->buffer;
	XMLHttpRequest * x = writeFuncData->x;
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

void XMLHttpRequest::update(float time)
{
	if (time - lastUpdateTime_ < 0.5f) return;
	lastUpdateTime_ = time;
	
	
	pthread_mutex_lock(&mutex_);	
	
	if (isQueuedProgressEvent_)
	{
		ProgressEvent ev = queuedProgressEvent_;
		JSContext * cx = s_->jsContext();
		JSObject * evtObject = JS_NewObject(cx, 0, 0, 0);
		jsval val;
		val = BOOLEAN_TO_JSVAL(ev.lengthComputable);
		JS_SetProperty(cx, evtObject, "lengthComputable", &val);
		val = DOUBLE_TO_JSVAL(ev.loaded);
		JS_SetProperty(cx, evtObject, "loaded", &val);
		val = DOUBLE_TO_JSVAL(ev.total);
		JS_SetProperty(cx, evtObject, "total", &val);
		
		fireEvent("progress", evtObject);
		isQueuedProgressEvent_ = false;
	}


	BOOST_FOREACH(int readyState, queuedReadyStateChanges_)
	{
		readyState_ = readyState;
		if (onReadyStateChange_)s_->execute(onReadyStateChange_);

		if (readyState_ == 4)
		{
			s_->unrootJSObject(jsObjForThis_);
			s_->unrootJSObject(onReadyStateChange_);
			multimap<string, JSObject *>::iterator iter;
			for (iter = eventListeners_.begin(); iter != eventListeners_.end(); ++iter)
			{
				s_->unrootJSObject((*iter).second);
			}
			eventListeners_.clear();			
		}
		
	}
	queuedReadyStateChanges_.clear();

	pthread_mutex_unlock(&mutex_);
}

int XMLHttpRequest::progressCallback(
	void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	int ret = 0;
	XMLHttpRequest * x = (XMLHttpRequest *)clientp;	

	pthread_mutex_lock(&x->mutex_);	
	ProgressEvent event;
	event.lengthComputable = dltotal > 0;
	event.loaded = dlnow + x->resumePos_;
	event.total = dltotal + x->resumePos_;

	x->isQueuedProgressEvent_ = true;
	x->queuedProgressEvent_ = event;
	if (x->cancel_) ret = -1;
	pthread_mutex_unlock(&x->mutex_);	
	return ret;
}

void * XMLHttpRequest::run(void * arg)
{
	long statLong ;
	XMLHttpRequest * x = (XMLHttpRequest *)arg;
	pthread_mutex_lock(&x->mutex_);
	x->threadRunning_ = true;
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
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, XMLHttpRequest::writeDataCallback);	
	WriteFuncData d;
	d.x = x;
	d.buffer = &x->buffer_;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &d); 	
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, XMLHttpRequest::progressCallback);	
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, x);	
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
	curl_easy_setopt(curl, CURLOPT_BUFFERSIZE,64000);
	if (x->method_ == POST)
	{
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, x->body_.c_str());
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);	
	}
	
	
	curl_easy_setopt(curl, CURLOPT_RESUME_FROM, x->resumePos_);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, ~0);
	code = curl_easy_perform(curl);
	curl_easy_getinfo( curl , CURLINFO_RESPONSE_CODE , &statLong );
	curl_easy_cleanup(curl);

	if (code != 0)
	{
		x->status_ = code;
		const char * err = curl_easy_strerror(code);
		Global::logMessage(err);

	}	
	else 
	{
		x->status_ = statLong;
	}

	//call done callback function
	pthread_mutex_lock(&x->mutex_);	
	x->queuedReadyStateChanges_.push_back(4);	
	x->threadRunning_ = false;
	if (x->file_) 
	{
		fclose(x->file_);
		x->file_ = 0;
	}
	pthread_cond_signal(&x->cond_);	
	pthread_mutex_unlock(&x->mutex_);
	return 0;
}

void XMLHttpRequest::setOnReadyStateChange(JSObject * obj) 
{
	if (onReadyStateChange_) s_->unrootJSObject(onReadyStateChange_);
	onReadyStateChange_ = obj;	
	
}

void XMLHttpRequest::abort()
{
	pthread_mutex_lock(&mutex_);
	cancel_ = true;
	while(threadRunning_)
	{
		pthread_cond_wait(&cond_, &mutex_);		
	}
	pthread_mutex_unlock(&mutex_);
}

void XMLHttpRequest::send(const std::string & body)
{
	pthread_mutex_lock(&mutex_);
	cancel_ = true;
	while(threadRunning_)
	{
		pthread_cond_wait(&cond_, &mutex_);		
	}
	pthread_mutex_unlock(&mutex_);

	body_ = body;	
	

	if (!fileName_.empty())
	{
		if (resumeDownload_)		
			file_ = fopen(fileName_.c_str(), "ab");
		else
			file_ = fopen(fileName_.c_str(), "wb");

		if (!file_)
		{
			throw Exception("XMLHttpRequest::send - Could not open file.");
		}
		long size = ftell(file_);
	}

	isQueuedProgressEvent_ = false;
	queuedReadyStateChanges_.clear();
	cancel_ = false;
	lastUpdateTime_ = -FLT_MAX;

	s_->rootJSObject(jsObjForThis_);
	pthread_create(&thread_, 0, XMLHttpRequest::run, this);	
	/*
	struct sched_param param;
	param.sched_priority = SCHED_MIN;
	pthread_setschedparam(thread_, SCHED_RR, &param);*/
}

void XMLHttpRequest::open(const std::string & method, const std::string & url)
{
	if (method == "POST") method_ = POST;
	else method_ = GET;
	url_ = url;
}