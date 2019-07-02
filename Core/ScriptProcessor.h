#pragma once

#ifdef _MSC_VER
#if _MSC_VER >= 1600
#define JS_HAVE_STDINT_H 1
#endif
#endif

#include "Types.h"
#include "ScriptEventListener.h"

JSClass InitClass(const char * name, uint32 flags, JSPropertyOp getProperty, 
	JSStrictPropertyOp setProperty, JSFinalizeOp finalize, JSTraceOp mark);

size_t js_strlen(const jschar * s);

std::string jsCharStrToStdString(const jschar * msg);
std::wstring jsCharStrToStdWString(const jschar * msg);

std::wstring multiByteStringToWideCharString(std::string const& str, std::locale loc);
std::string wideCharStringToMultiByteString(std::wstring const& str, std::locale loc);

bool getPropertyString(
	JSContext * cx, JSObject * obj, const char * propName, std::string * outStr);

bool getPropertyFloat(
	JSContext * cx, JSObject * obj, const char * propName, float * outVal);

bool getPropertyInt(
	JSContext * cx, JSObject * obj, const char * propName, int * outVal);

bool getPropertyUint(
	JSContext * cx, JSObject * obj, const char * propName, unsigned int * outVal);

bool getPropertyBool(
	JSContext * cx, JSObject * obj, const char * propName, bool * outVal);

bool getFloat(JSContext * cx, jsval val, float * outVal);
bool getInt(JSContext * cx, jsval val, int * outVal);
bool getString(JSContext * cx, jsval val, std::string * outStr);

inline void JSTracerDummyDebugPrinter(JSTracer * trc, char * buf, size_t bufsize) {}

#ifdef _DEBUG
#define JS_TRACER_SET_DEBUG_VARS(trc) trc->debugPrinter = JSTracerDummyDebugPrinter;
#else
#define JS_TRACER_SET_DEBUG_VARS(trc)
#endif


/**
Finalizer that makes sure associated private data (that is owned by the js 
object) is deleted upon object garbage collection.

Also removes any keep alive data associated with the deleted script object
*/
void SceneObject_finalize(JSContext * cx, JSObject * obj);



////////////////////////////////////////////////////////////////////////////////

///keep track of rooted JSObjects
class JSObjectRoots
{
public:
	JSObjectRoots();
	void rootJSObject(JSContext * cx, JSObject * origObject);
	void unrootAllJSObjects(JSContext * cx);
	void unrootJSObject(JSContext * cx, JSObject * rootedObject);
private:
	struct JSObjectRootData
	{
		JSObjectRootData() {this->stored = false; this->rootedObject = 0;}
		JSObject * rootedObject;
		bool stored;
	};
	std::vector<JSObjectRootData> jsObjects_;
	std::set<JSObject *> jsObjectsSet_;
	int firstFreeIndex_;
};

////////////////////////////////////////////////////////////////////////////////
//keep track of allocated private data

class JSPrivateDataAllocBase
{
public:
	virtual ~JSPrivateDataAllocBase() 
	{
	}

	virtual void deleteData() = 0;
};

template <typename T>
class JSPrivateDataAlloc : public JSPrivateDataAllocBase
{
public:
	JSPrivateDataAlloc(T * data)
	{
		privateData_ = data;
	}

	virtual ~JSPrivateDataAlloc()
	{
		delete privateData_;
	}

	virtual void deleteData()
	{
		delete privateData_;
	}
private:
	T * privateData_;
};

class JSPrivateDataAllocs
{
public:

	~JSPrivateDataAllocs()
	{
		deleteAllPrivateData();
	}

	/**
	Make sure not to add the same object twice using this function
	*/
	template<typename T>
	void addPrivateData(T * privateData)
	{
		privateDataAllocs_[privateData] = new JSPrivateDataAlloc<T>(privateData);
	}

	void deletePrivateData(void * privateData)
	{
		if (privateDataAllocs_.find(privateData) == privateDataAllocs_.end()) return;
		
		delete privateDataAllocs_[privateData];		
		privateDataAllocs_.erase(privateData);		
	}

	void deleteAllPrivateData()
	{
		std::map<void *, JSPrivateDataAllocBase *>::iterator iter;

		for (iter = privateDataAllocs_.begin(); iter != privateDataAllocs_.end(); ++iter)
		{
			JSPrivateDataAllocBase * d = (*iter).second;
			delete d;
		}

		privateDataAllocs_.clear();
	}
private:
	std::map<void *, JSPrivateDataAllocBase *> privateDataAllocs_;	
};

////////////////////////////////////////////////////////////////////////////////
class SceneObject;


/**
When a scene object is removed from the scene, its pointer must be kept alive,
so it can still be accessed as private data of its script object.

TODO maybe need to generalize it to accomodate more than just SceneObject types
*/
class KeepAliveData
{
public:
	void addKeepAliveData(const boost::shared_ptr<SceneObject> & keepAliveData)
	{
		keepAliveData_.insert(keepAliveData);
	}

	SceneObjectSPtr removeKeepAliveData(void * data)
	{
		std::set<SceneObjectSPtr>::iterator iter;
		for (iter = keepAliveData_.begin(); iter != keepAliveData_.end(); ++iter)
		{
			if ((*iter).get() == data)
			{
				SceneObjectSPtr ret = *iter;
				keepAliveData_.erase(iter);
				return ret;
			}
		}
		return SceneObjectSPtr();
	}
private:
	std::set<SceneObjectSPtr> keepAliveData_;
};


////////////////////////////////////////////////////////////////////////////////
class Document;

class ExecScriptData 
{
public:		
	int id;
	float repeatInterval;
	float execTime;		
	JSObject * funcObj;	
	bool executed;
};


////////////////////////////////////////////////////////////////////////////////
class SceneObject;
class DocumentTemplate;
class XMLHttpRequest;

class AppObject;

class ScriptProcessor
{
public:
	ScriptProcessor(Document * document);
	~ScriptProcessor();

	/**
	The behavior of init is a little bit unusual. If the object is already
	initialized, then the call is a no op. Usually it would uninit first,
	then init in that situation.
	*/
	void init();
	void uninit();
	bool isInit() const {return init_;}

	bool executeAndUnroot(JSObject * functionObj);
	bool execute(JSObject * functionObj);
	bool execute(const std::string & scriptFile, JSObject * global = NULL);
	bool execute(
		const std::string & scriptFile, 
		const std::string & script, 
		JSObject * global = NULL);
	bool executeScriptEvent(Event * event, const ScriptEventListener & listener);

	unsigned int setFuncTimeout(JSObject * functionObj, unsigned int interval);
	unsigned int setFuncInterval(JSObject * functionObj, unsigned int interval);
	void clearInterval(int id);
	void clearTimeout(int id);

	void update(float docTime);

	void rootJSObject(JSObject * origObject);
	void unrootAllJSObjects();
	void unrootJSObject(JSObject * rootedObject);

	JSObject * sceneProto() const {return sceneProto_;}
	JSObject * baseDocumentTemplateProto() const {return baseDocumentTemplateProto_;}
	JSObject * documentTemplateProto(DocumentTemplate * templ);
	JSObject * sceneObjectProto(SceneObject * sceneObject);
	JSObject * appObjectProto(AppObject * appObject);	
	JSObject * baseSceneObjectProto() const {return baseSceneObjectProto_;}
	JSObject * baseAppObjectProto() const {return baseAppObjectProto_;}
	JSObject * matrixProto() const {return matrixProto_;}
	JSObject * vector3Proto() const {return vector3Proto_;}
	JSObject * pathProto() const {return pathProto_;}
	JSObject * vector2Proto() const {return vector2Proto_;}
	JSObject * transformProto() const {return transformProto_;}
	JSObject * eventProto(Event * event);
	JSObject * baseEventProto() const {return baseEventProto_;}
	JSObject * animationProto() const {return animationProto_;}
	JSObject * pixelBufferProto() const {return pixelBufferProto_;}
	JSObject * kdTreeProto() const {return kdTreeProto_;}

	void addXmlHttpRequest(XMLHttpRequest * x);
	void removeXmlHttpRequest(XMLHttpRequest * x);

	/**
	Register event listeners attached to objects in order to properly clean them
	up when the script engine is destroyed
	*/
	void registerEventListener(SceneObject * object, JSObject * funcObj);

	template <typename T>
	void addPrivateData(T * privateData) 
	{
		privateDataAllocs_.addPrivateData(privateData);
	}

	void deletePrivateData(void * privateData);

	template <typename T>
	void addKeepAliveData(boost::shared_ptr<T> & keepAliveSPtr)
	{	
		keepAliveData_.addKeepAliveData(keepAliveSPtr);
	}

	template <typename T>
	boost::shared_ptr<T> removeKeepAliveData(T * data)
	{
		return keepAliveData_.removeKeepAliveData(data);
	}

	template <typename T>
	void discardKeepAliveData(T * data)
	{		
		keepAliveData_.removeKeepAliveData(data);
	}

	JSContext * jsContext() const {return cx_;}
	JSObject * jsGlobal() const {return global_;}

	JSObject * newGlobal();

	Document * document() const {return document_;}
	std::string getActualFileName(const std::string & scriptSideFileName) const;
	
	JSObject * curExecGlobal() const {return execGlobals_.top();}

public:
	//type related data
	JSClass globalClass;
	JSClass documentClass;
	JSClass xmlHttpRequeustClass;

	JSObject * documentProto;
	JSObject * consoleProto;
	JSObject * xmlHttpRequestProto;
private:
	ScriptProcessor(const ScriptProcessor & rhs);
	ScriptProcessor & operator = (const ScriptProcessor & rhs);
private:
	Document * document_;
	bool init_;

	JSRuntime * rt_;
	JSContext * cx_;
	JSObject * global_;

	
	JSObject * documentInstance_;

	JSObject * console_;
	JSObject * consoleInstance_;
	
	float lastUpdateTime_;

	JSObjectRoots rootedObjects_;
	JSPrivateDataAllocs privateDataAllocs_;

	int curExecScriptId_;
	std::vector<ExecScriptData> execScriptData_;

	std::map<boost::uuids::uuid, JSObject *> sceneObjectProtos_;
	std::map<boost::uuids::uuid, JSObject *> eventObjectProtos_;
	std::map<boost::uuids::uuid, JSObject *> documentTemplateProtos_;
	JSObject * matrixProto_;
	JSObject * vector3Proto_;
	JSObject * pathProto_;
	JSObject * vector2Proto_;
	JSObject * transformProto_;
	JSObject * sceneProto_;
	JSObject * baseSceneObjectProto_;
	JSObject * baseAppObjectProto_;
	JSObject * baseDocumentTemplateProto_;
	JSObject * baseEventProto_;
	JSObject * animationProto_;
	JSObject * pixelBufferProto_;
	JSObject * kdTreeProto_;

	KeepAliveData keepAliveData_;

	JSObject * xmlHttpRequestProto_;

	std::set<XMLHttpRequest *> xmlHttpRequests_;

	std::stack<JSObject *> execGlobals_;

	
};

////////////////////////////////////////////////////////////////////////////////

extern std::map<JSContext *, ScriptProcessor *> gScriptProcessors;
