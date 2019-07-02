#pragma once
#include "Types.h"
class Document;
class Color;
class Transform;
class SceneObject;
class Scene;
class Vector3;
class VisualAttrib;
class ElementMapping;
class AnimationChannel;
class AppObject;

typedef struct _xmlNode xmlNode;
typedef struct _xmlDoc xmlDoc;
typedef xmlDoc *xmlDocPtr;
typedef xmlNode *xmlNodePtr;
typedef struct _xmlAttr xmlAttr;
typedef xmlAttr *xmlAttrPtr;
typedef xmlNodePtr htmlNodePtr;


class XmlException : public std::exception
{
public:
	XmlException(xmlNode * node, const std::string & whatStr);
	unsigned int line() const {return line_;}
	virtual const char * what() const throw() {return what_.c_str();}
	virtual ~XmlException() throw() {}
private:
	unsigned int line_;
	std::string what_;
};

///////////////////////////////////////////////////////////////////////////////


class XmlIdMapping
{
public:
	void init(Document * document);

	unsigned int getId(Scene *);
	Scene * getScene(unsigned int id);

	unsigned int getId(SceneObject *);
	SceneObject * getSceneObject(unsigned int id);

	unsigned int getId(AppObject *);
	AppObject * getAppObject(unsigned int id);

	unsigned int getId(Animation *);
	Animation * getAnimation(unsigned int id);

	void setId(Scene *, unsigned int);
	void setId(SceneObject *, unsigned int);
	void setId(AppObject *, unsigned int);
	void setId(Animation *, unsigned int);

	ElementMapping * elemMapping() {return mapping_.get();}
	
private:	
	std::map<SceneObject *, unsigned int> sceneObjIds_;	
	std::map<AppObject *, unsigned int> appObjIds_;	
	std::map<Animation *, unsigned int> animIds_;
	std::map<Scene *, unsigned int> sceneIds_;

	boost::scoped_ptr<ElementMapping> mapping_;

	Document * document_;
};

///////////////////////////////////////////////////////////////////////////////


class XmlWriter
{
public:	
	XmlWriter();
	~XmlWriter();

	void write(Document * document, const char * fileName);
	Document * document() const {return document_;}
	
	void setScene(const Scene * scene) {scene_ = scene;}
	const Scene * scene() const {return scene_;}
	
	void startTag(const std::string & tag, const std::string & attrStr = "");

	

	void endTag();
	void writeTag(const std::string & tag, const std::string & value);
	void writeTag(const std::string & tag, const std::string & attrStr, const std::string & value);
	void writeTag(const std::string & tag, const std::wstring & value);
	void writeTag(const std::string & tag, const Color & value);
	void writeTag(const std::string & tag, const unsigned int & value);
	void writeTag(const std::string & tag, const int & value);
	void writeTag(const std::string & tag, const float & value);	
	void writeTag(const std::string & tag, const Vector3 & val);	
	void writeTag(const std::string & tag, const Transform & val);
	void writeTag(const std::string & tag, SceneObject * obj);
	void writeTag(const std::string & tag, Scene * scene);
	void writeTag(const std::string & tag, Animation * animation);
	void writeTag(const std::string & tag, AppObject * appObject);
	void writeAppObject(AppObject * appObj);
	void writeObject(SceneObject * obj);
	void writeEvent(Event * event);
	void writeAction(Action * action);

	void writeText(const std::string & value);

	XmlIdMapping & idMapping() {return idMapping_;}

private:
	std::stack<std::string> tags_;
	FILE * file_;
	std::string indentStr_;
	Document * document_;
	const Scene * scene_;

	XmlIdMapping idMapping_;
};

///////////////////////////////////////////////////////////////////////////////

class XmlReader
{
public:
	XmlReader();
	~XmlReader();

	Document * read(const char * fileName);
	Document * document() const {return document_;}

	void setScene(Scene * scene);
	Scene * scene() const {return scene_;}

	void setVersion(unsigned ver) {version_ = ver;}
	unsigned int version() const {return version_;}

	XmlIdMapping & idMapping() {return idMapping_;}

	template <typename T>
	bool getNodeAttribute(T & val, xmlNode * node, const char * name)
	{
		char * ret = NULL;
	
		ret = (char *)xmlGetProp(node, (xmlChar *)name);
		if (!ret) return false;
		std::string str = ret;
		try
		{
			val = boost::lexical_cast<T>(str);
		}
		catch (...)
		{
			xmlFree(ret);
			throw XmlException(node, "Wrong attribute type");
		}
		xmlFree(ret);
		return true;
	}


	bool isNodeName(xmlNode * node, const char * name) 
	{
		if (!node) return false;
		return xmlStrEqual(node->name, (xmlChar *)name);		
	}

	template <typename T>
	bool getNodeContentIfName(T & val, xmlNode * node, const char * name)
	{
		if (!isNodeName(node, name)) return false;
		getNodeContent(val, node);
		return true;
	}

	template <typename T>
	void getNodeContent(T & val, xmlNode * node)
	{		
		if (!node || !node->children) return;		
		try 
		{
			val = boost::lexical_cast<T>(node->children->content);
		} catch(...) 
		{
			throw XmlException(node, "Wrong node type");	
		}
	}

	void getNodeContent(Color & val, xmlNode * node);
	void getNodeContent(Vector3 & val, xmlNode * node);
	void getNodeContent(Transform & val, xmlNode * node);
	
	bool getTemplate(DocumentTemplateSPtr & val, xmlNode * node);
	bool getSceneObject(SceneObjectSPtr & val, xmlNode * node);
	bool getEvent(EventSPtr & val, xmlNode * node);
	bool getAction(ActionSPtr & val, xmlNode * node);
	bool getAnimChannel(AnimationChannelSPtr & val, xmlNode * node);
	bool getAppObject(AppObjectSPtr & val, xmlNode * node);

private:

	xmlDocPtr doc_;
	Document * document_;
	Scene * scene_;
	typedef boost::scoped_ptr<ElementMapping> ElementMappingSPtr;
	
	unsigned int version_;

	XmlIdMapping idMapping_;
};


