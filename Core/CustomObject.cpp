#include "stdafx.h"
#include "CustomObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "Group.h"
#include "ScriptProcessor.h"
#include "GfxRenderer.h"
#include "Scene.h"
#include "Document.h"
#include <utf8.h>
using namespace std;

///////////////////////////////////////////////////////////////////////////////

void CustomObjectData::write(Writer & writer) const
{
	writer.write(script_);
}

void CustomObjectData::read(Reader & reader, unsigned char version)
{
	reader.read(script_);
}

void CustomObjectData::writeXml(XmlWriter & w) const
{
	w.writeTag("Script", script_);
}

void CustomObjectData::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;		
		else if (r.getNodeContentIfName(script_, curNode, "Script"));
	}
}


///////////////////////////////////////////////////////////////////////////////

void CustomObject::create()
{
	global_ = NULL;
	isLoadedSinceInit_ = true;
	rootObject_.reset(new Group);
	rootObject_->setId("CustomObject root");
	setAsSubObject(rootObject_.get());

}

CustomObject::CustomObject()
{
	create();
	setId("CustomObject");
}

CustomObject::CustomObject(const CustomObject & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	visualAttrib_ = rhs.visualAttrib_;
	customObjectId_ = rhs.customObjectId_;
	data_ = rhs.data_;
}

CustomObject::~CustomObject()
{
	uninit();
}

void CustomObject::init(GfxRenderer * gl, bool firstTime)
{		
	SceneObject::init(gl, firstTime); //does uninit first if necessary

	isLoadedSinceInit_ = false;
	ScriptProcessor * s = getScriptProcessor();
	if (s)
	{
		Document * document = s->document();
		/*
		{
			//for testing/debugging

			boost::uuids::string_generator gen;
			boost::uuids::uuid id = gen("99ED2222-3AB7-2799-1717-82D5364987AF");
			
			string scriptFile = "c:/git_working/GlContentEditor/Editor/Resources/multiChoice.js";
			FILE * file = 0;	
			std::vector<char> buffer;
			BOOST_SCOPE_EXIT( (&file)) {
				if (file) fclose(file);
			} BOOST_SCOPE_EXIT_END

			file = fopen(scriptFile.c_str(), "rb");
			if (!file) return;
			fseek(file, 0, SEEK_END);	
			long fileSize = ftell(file);
			fseek(file, 0, SEEK_SET);
			buffer.resize(fileSize);
			fread(&buffer[0], 1, fileSize, file);
			std::string bufferStr(buffer.begin(), buffer.end());
			document->registerCustomObject(id, "Test", bufferStr);	

			data_ = 
"<data>\
<width>450</width>\
<height>320</height>\
<text>This is the question text</text>\
<font>C:/Windows/Fonts/malgun.ttf</font>\
<fontSize>18</fontSize>\
<uncheckedImage>C:/ST/scriptTest/unchecked.png</uncheckedImage>\
<checkedImage>C:/ST/scriptTest/checked.png</checkedImage>\
<checkAnswerImage>C:/ST/scriptTest/f4.png</checkAnswerImage>\
<choice>choice 1</choice>\
<choice>choice 2</choice>\
<choice>choice 3</choice>\
<choice>choice 4</choice>\
<correctChoice>0</correctChoice>\
<columns>3</columns>\
<correctImage>C:/ST/scriptTest/correct.png</correctImage>\
<incorrectImage>C:/ST/scriptTest/incorrect.png</incorrectImage>\
</data>";
		}
*/

		CustomObjectData * data = document->customObjectData(customObjectId_);		
		if (data && !global_)
		{			
			ScriptProcessor * s = document->scriptProcessor();
			global_ = s->newGlobal();
			//root this global object to prevent crashes
			s->rootJSObject(global_);		
			JSObject * jsObj = getScriptObject(s);			
			JSObject * rootObj = rootObject_->getScriptObject(s);			
			jsval val = OBJECT_TO_JSVAL(jsObj);		
			JS_SetProperty(s->jsContext(), global_, "customObject", &val);			
			val = OBJECT_TO_JSVAL(rootObj);			
			JS_SetProperty(s->jsContext(), jsObj, "root", &val);			
			
			//JS_GC(s->jsContext());			
			document->scriptProcessor()->execute(
				stringId_, data->script_, global_);
				
			jsval rval;
			if (!data_.empty())
			{
				jsval jsdata = STRING_TO_JSVAL(
					JS_NewStringCopyN(s->jsContext(), &data_[0], data_.size()));
				JS_CallFunctionName(s->jsContext(), jsObj, "_init", 1, &jsdata, &rval);
			}
			else
			{
				JS_CallFunctionName(s->jsContext(), jsObj, "_init", 0, 0, &rval);
			}
			
		}
	}
	//do init after script _init so that script writer doesn't have to init manually		
	rootObject_->init(gl);
}

void CustomObject::uninit()
{
	SceneObject::uninit();
	ScriptProcessor * s = getScriptProcessor();
	if (s)
	{
		if (global_)
		{
			JSObject * jsObject = getScriptObject(s);
			jsval rval;
			JS_CallFunctionName(s->jsContext(), jsObject, "_uninit", 0, 0, &rval);
			s->unrootJSObject(global_);
			global_ = NULL;
			//JS_GC(s->jsContext());
		}
	}

	//do uninit after script _uninit so that script writer doesn't have to uninit manually
	rootObject_->uninit();
}

ScriptProcessor * CustomObject::getScriptProcessor() const
{
	Scene * scene = parentScene();	
	if (!scene) return NULL;
	Document * document = scene->parentDocument();
	if (!document) return NULL;
	return document->scriptProcessor();
}


void CustomObject::setData(const std::string & data)
{	
	data_ = data;
	ScriptProcessor * s = getScriptProcessor();
	if (s)
	{
		Document * document = s->document();
		JSObject * jsObj = getScriptObject(s);

		jsval rval;
		jsval jsdata = STRING_TO_JSVAL(
			JS_NewStringCopyN(s->jsContext(), &data_[0], data_.size()));
		JS_CallFunctionName(s->jsContext(), jsObj, "_setData", 1, &jsdata, &rval);

		//JS_GC(s->jsContext());
	}	
}

void CustomObject::remapReferences(const ElementMapping & elementMapping)
{
	rootObject_->remapReferences(elementMapping);
	SceneObject::remapReferences(elementMapping);
}
void CustomObject::referencedFiles(std::vector<std::string> * refFiles) const
{
	ScriptProcessor * s = getScriptProcessor();
	if (s)
	{
		JSObject * jsObj = const_cast<CustomObject *>(this)->getScriptObject(s);
		JSContext * cx = s->jsContext();
		jsval rval;
		JS_CallFunctionName(s->jsContext(), jsObj, "_getReferencedFiles", 0, 0, &rval);

		JSObject * arrayObj = JSVAL_TO_OBJECT(rval);
		//TODO check JS_IsArrayObject
		jsuint jsLen;
		JS_GetArrayLength(cx, arrayObj, &jsLen);

		for (int i = 0; i < (int)jsLen; ++i)
		{
			jsval jsCurVal;
			JS_GetElement(cx, arrayObj, i, &jsCurVal);
			JSString * jsStr = JSVAL_TO_STRING(jsCurVal);
			char * str = JS_EncodeString(cx, jsStr);
			refFiles->push_back(str);
			JS_free(cx, str);
		}
	}


	SceneObject::referencedFiles(refFiles);
}


int CustomObject::setReferencedFiles(
	const std::string & baseDirectory,
	const std::vector<std::string> & refFiles, int startIndex)
{
	vector<string> curRefFiles;
	referencedFiles(&curRefFiles);

	vector<string> newRefFiles;
	for (int i = 0; i < (int)curRefFiles.size(); ++i)
		newRefFiles.push_back(refFiles[startIndex++]);		

	ScriptProcessor * s = getScriptProcessor();
	if (s)
	{
		JSObject * jsObj = const_cast<CustomObject *>(this)->getScriptObject(s);
		JSContext * cx = s->jsContext();
		
		std::vector<jsval> initVals;
		initVals.resize(newRefFiles.size());

		for (int i = 0; i < (int)newRefFiles.size(); ++i)
		{
			initVals[i] = STRING_TO_JSVAL(JS_NewStringCopyN(
				cx, newRefFiles[i].c_str(), newRefFiles[i].size()));
		}

		JSObject * arrayObj = 
			JS_NewArrayObject(cx, (jsint)newRefFiles.size(), &initVals[0]);

		jsval argv = OBJECT_TO_JSVAL(arrayObj);
		jsval rval;

		JS_CallFunctionName(cx, jsObj, "_setReferencedFiles", 1, &argv, &rval);	

		JS_CallFunctionName(cx, jsObj, "_getData", 0, 0, &rval);	
		JSString * jsStr = JSVAL_TO_STRING(rval);
		char * encStr = JS_EncodeString(cx, jsStr);
		data_ = encStr;		
		JS_free(cx, encStr);
	}

	return SceneObject::setReferencedFiles(baseDirectory, refFiles, startIndex);
}

void CustomObject::setParentScene(Scene * parentScene)
{
	SceneObject::setParentScene(parentScene);
	rootObject_->setParentScene(parentScene);
}


//TODO
//TODO
//TODO make this class handle child objects better... in the below virtual funcs
//.... or enforce the convention that only Group objects can have children once and for all.

bool CustomObject::clickEvent(const Vector2 & mousePos, int pressId)
{
	return rootObject_->clickEvent(mousePos, pressId);
}

bool CustomObject::doubleClickEvent(const Vector2 & mousePos, int pressId)
{
	return rootObject_->clickEvent(mousePos, pressId);
}

bool CustomObject::pressEvent(const Vector2 & startPos, int pressId)
{
	return rootObject_->pressEvent(startPos, pressId);
}

bool CustomObject::releaseEvent(const Vector2 & pos, int pressId)
{
	return rootObject_->releaseEvent(pos, pressId);
}

bool CustomObject::moveEvent(const Vector2 & pos, int pressId)
{
	return rootObject_->moveEvent(pos, pressId);
}

bool CustomObject::receiveMessageEvent(const std::string & message)
{
	return rootObject_->receiveMessageEvent(message);
}

void CustomObject::start(float docTime)
{
	return rootObject_->start(docTime);
}

void CustomObject::preStart(float docTime)
{
	return rootObject_->preStart(docTime);
}

void CustomObject::initAppObjects()
{
	rootObject_->initAppObjects();
}

void CustomObject::uninitAppObjects()
{
	rootObject_->uninitAppObjects();
}



bool CustomObject::update(float sceneTime)
{	
	return rootObject_->update(sceneTime) || SceneObject::update(sceneTime);
}

bool CustomObject::asyncLoadUpdate()
{
	bool everythingLoaded = rootObject_->asyncLoadUpdate();

	if (!everythingLoaded)
	{
		isLoadedSinceInit_ = false;
	}
	else if (!isLoadedSinceInit_)
	{
		isLoadedSinceInit_ = true;

		Scene * scene = parentScene();
		Document * document = NULL;
		if (scene) document = scene->parentDocument();
		if (document)
		{
			ScriptProcessor * s = (ScriptProcessor *)document->scriptProcessor();
			jsval rval;
			JS_CallFunctionName(s->jsContext(), scriptObject_, "_onLoaded", 0, 0, &rval);
		}

		
	}

	return everythingLoaded;
}

bool CustomObject::handleEvent(Event * event, float sceneTime)
{
	return rootObject_->handleEvent(event, sceneTime);
}

bool CustomObject::dependsOn(Scene * scene) const
{
	return rootObject_->dependsOn(scene);
}

void CustomObject::dependsOnScenes(std::vector<Scene *> * dependsOnScenes, bool recursive) const
{
	rootObject_->dependsOnScenes(dependsOnScenes, recursive);
}

AppObjectOffset CustomObject::appObjectOffset() const
{
	return rootObject_->appObjectOffset();
}

bool CustomObject::isShowing(Scene * scene) const
{
	return rootObject_->isShowing(scene);
}

void CustomObject::showingScenes(std::set<Scene *> * showingScenes) const
{
	rootObject_->showingScenes(showingScenes);
}


void CustomObject::drawObject(GfxRenderer * gl) const
{
	rootObject_->draw(gl);
}

BoundingBox CustomObject::extents() const
{	
	return SceneObject::extents().unite(rootObject_->extents());
}

SceneObject * CustomObject::intersect(Vector3 * out, const Ray & ray)
{
	//TODO also intersect with children of this object and return object w nearest intersection point
	return rootObject_->intersect(out, ray);	
}

void CustomObject::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	writer.write(customObjectId_);
	writer.write(data_);
	
}

void CustomObject::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	reader.read(customObjectId_);	
	reader.read(data_);
}


void CustomObject::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	//std::stringstream ss;
	//ss << customObjectId_;
	std::string str = boost::lexical_cast<std::string>(customObjectId_);

	w.writeTag("CustomObjectId", str);
	w.writeTag("Data", data_);
}

void CustomObject::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);
	std::string str;

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;		
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(str, curNode, "CustomObjectId"))
		{
			boost::uuids::string_generator gen;
			customObjectId_ = gen(str.c_str());
		}
		else if (r.getNodeContentIfName(data_, curNode, "Data"));
	}
}
