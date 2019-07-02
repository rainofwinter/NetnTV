#include "stdafx.h"
#include "Xml.h"
#include "Document.h"
#include "Color.h"
#include "MathStuff.h"
#include "Transform.h"
#include "Attrib.h"
#include "SceneObject.h"
#include "ElementMapping.h"
#include "Global.h"
#include "Event.h"
#include "Action.h"
#include "AnimationChannel.h"
#include "AppObject.h"
#include "Scene.h"
#include "Animation.h"
#include "DocumentTemplate.h"
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;



XmlException::XmlException(xmlNode * node, const std::string & whatStr)
{
	what_ = whatStr;
	line_ = 0;
	if (node) line_ = node->line;
}

void XmlIdMapping::init(Document * document)
{
	mapping_.reset(new ElementMapping);
	mapping_->setDestinationData(document, NULL);
	document_ = document;
}


unsigned int XmlIdMapping::getId(Scene * obj)
{
	if (!obj) return NULL;
	map<Scene *, unsigned int>::iterator itr = sceneIds_.find(obj);
	if (itr != sceneIds_.end()) return (*itr).second;
	else
	{
		unsigned int id = sceneIds_.size() + 1;
		setId(obj, id);
		return id;
	}
}

unsigned int XmlIdMapping::getId(SceneObject * obj)
{
	if (!obj) return NULL;
	map<SceneObject *, unsigned int>::iterator itr = sceneObjIds_.find(obj);
	if (itr != sceneObjIds_.end()) return (*itr).second;
	else
	{
		unsigned int id = sceneObjIds_.size() + 1;
		setId(obj, id);
		return id;
	}
}

unsigned int XmlIdMapping::getId(AppObject * obj)
{
	if (!obj) return NULL;
	map<AppObject *, unsigned int>::iterator itr = appObjIds_.find(obj);
	if (itr != appObjIds_.end()) return (*itr).second;
	else
	{
		unsigned int id = appObjIds_.size() + 1;
		setId(obj, id);
		return id;
	}
}

unsigned int XmlIdMapping::getId(Animation * obj)
{
	if (!obj) return NULL;
	map<Animation *, unsigned int>::iterator itr = animIds_.find(obj);
	if (itr != animIds_.end()) return (*itr).second;
	else
	{
		unsigned int id = animIds_.size() + 1;
		setId(obj, id);
		return id;
	}
}

void XmlIdMapping::setId(SceneObject * obj, unsigned int id)
{
	sceneObjIds_[obj] = id;
	mapping_->addObjectMapping((SceneObject *)id, obj);
}

void XmlIdMapping::setId(AppObject * obj, unsigned int id)
{
	appObjIds_[obj] = id;
	mapping_->addAppObjectMapping((AppObject *)id, obj);
}

void XmlIdMapping::setId(Animation * obj, unsigned int id)
{
	animIds_[obj] = id;
	mapping_->addAnimationMapping((Animation *)id, obj);
}

void XmlIdMapping::setId(Scene * obj, unsigned int id)
{
	sceneIds_[obj] = id;
	mapping_->addSceneMapping((Scene *)id, obj);
}

Scene * XmlIdMapping::getScene(unsigned int id)
{
	if (id == 0) return NULL;
	return mapping_->mapScene((Scene *)id);
}

SceneObject * XmlIdMapping::getSceneObject(unsigned int id)
{
	if (id == 0) return NULL;
	return mapping_->mapObject((SceneObject *)id);
}

AppObject * XmlIdMapping::getAppObject(unsigned int id)
{
	if (id == 0) return NULL;
	return mapping_->mapAppObject((AppObject *)id);
}

Animation * XmlIdMapping::getAnimation(unsigned int id)
{
	if (id == 0) return NULL;
	return mapping_->mapAnimation((Animation *)id);
}
///////////////////////////////////////////////////////////////////////////////

XmlWriter::XmlWriter()
{
}


XmlWriter::~XmlWriter()
{
}

void XmlWriter::startTag(const std::string & tagName, const std::string & attrStr)
{
	fprintf(file_, indentStr_.c_str());
	if (attrStr.empty())
		fprintf(file_, "<%s>\n", tagName.c_str());
	else
		fprintf(file_, "<%s %s>\n", tagName.c_str(), attrStr.c_str());
	tags_.push(tagName);
	indentStr_ += "   ";
}

void XmlWriter::endTag()
{
	indentStr_ = indentStr_.substr(0, indentStr_.size() - 3);
	std::string tag = tags_.top();
	tags_.pop();
	fprintf(file_, indentStr_.c_str());
	fprintf(file_, "</%s>\n", tag.c_str());
}

void XmlWriter::writeTag(const std::string & tagName, const std::string & attrStr, const std::string & value)
{
	fprintf(file_, indentStr_.c_str());
	std::string str = value;
	std::size_t pos = 0;
	do
	{
		pos = str.find("<", pos, 1);
		if (pos != std::string::npos)
		{
			std::string temp = "";
			for (int i = 0 ; i < str.size() ; i++)
			{
				if (i == pos)
					temp.append("&lt;");
				else
					temp.push_back(str.at(i));
			}
			str = temp;
		}
	}while(pos != std::string::npos);
	pos = 0;
	do
	{
		pos = str.find(">", pos, 1);
		if (pos != std::string::npos)
		{
			std::string temp = "";
			for (int i = 0 ; i < str.size() ; i++)
			{
				if (i == pos)
					temp.append("&gt;");
				else
					temp.push_back(str.at(i));
			}
			str = temp;
		}
	}while(pos != std::string::npos);
	fprintf(file_, "<%s %s>%s</%s>\n", tagName.c_str(), attrStr.c_str(), str.c_str(), tagName.c_str());
}

void XmlWriter::writeTag(const std::string & tagName, const std::string & value)
{
	fprintf(file_, indentStr_.c_str());
	std::string str = value;
	std::size_t pos = 0;
	do
	{
		pos = str.find("<", pos, 1);
		if (pos != std::string::npos)
		{
			std::string temp = "";
			for (int i = 0 ; i < str.size() ; i++)
			{
				if (i == pos)
					temp.append("&lt;");
				else
					temp.push_back(str.at(i));
			}
			str = temp;
		}
	}while(pos != std::string::npos);
	pos = 0;
	do
	{
		pos = str.find(">", pos, 1);
		if (pos != std::string::npos)
		{
			std::string temp = "";
			for (int i = 0 ; i < str.size() ; i++)
			{
				if (i == pos)
					temp.append("&gt;");
				else
					temp.push_back(str.at(i));
			}
			str = temp;
		}
	}while(pos != std::string::npos);
	fprintf(file_, "<%s>%s</%s>\n", tagName.c_str(), str.c_str(), tagName.c_str());
}

void XmlWriter::writeTag(const std::string & tagName, const std::wstring & value)
{
	fprintf(file_, indentStr_.c_str());
	std::string temp;
	temp.resize(value.size());
	for (unsigned int i = 0 ; i < value.size() ; i++)
		temp[i] = value[i] < 256 ? value[i] : ' ';
	fprintf(file_, "<%s>%s</%s>\n", tagName.c_str(), temp.c_str(), tagName.c_str());
}

void XmlWriter::writeTag(const std::string & tagName, const Color & value)
{
	stringstream ss;
	ss << value.r << ", " << value.g << ", " << value.b << ", " << value.a;	
	fprintf(file_, indentStr_.c_str());
	fprintf(file_, "<%s>%s</%s>\n", tagName.c_str(), ss.str().c_str(), tagName.c_str());
}

void XmlWriter::writeTag(const std::string & tagName, const unsigned int & value)
{
	fprintf(file_, indentStr_.c_str());
	fprintf(file_, "<%s>%s</%s>\n", tagName.c_str(), lexical_cast<string>(value).c_str(), tagName.c_str());
}

void XmlWriter::writeTag(const std::string & tagName, const int & value)
{
	fprintf(file_, indentStr_.c_str());
	fprintf(file_, "<%s>%s</%s>\n", tagName.c_str(), lexical_cast<string>(value).c_str(), tagName.c_str());
}

void XmlWriter::writeTag(const std::string & tagName, const float & value)
{
	fprintf(file_, indentStr_.c_str());
	fprintf(file_, "<%s>%s</%s>\n", tagName.c_str(), lexical_cast<string>(value).c_str(), tagName.c_str());
}

void XmlWriter::writeTag(const std::string & tagName, SceneObject * obj)
{
	string str;
	unsigned int objId = idMapping_.getId(obj);	
	fprintf(file_, indentStr_.c_str());
	fprintf(file_, "<%s>%s</%s>\n", tagName.c_str(), lexical_cast<string>(objId).c_str(), tagName.c_str());
}

void XmlWriter::writeTag(const std::string & tagName, Animation * anim)
{
	string str;
	unsigned int objId = idMapping_.getId(anim);	
	fprintf(file_, indentStr_.c_str());
	fprintf(file_, "<%s>%s</%s>\n", tagName.c_str(), lexical_cast<string>(objId).c_str(), tagName.c_str());
}

void XmlWriter::writeTag(const std::string & tagName, AppObject * appObject)
{
	string str;
	unsigned int objId = idMapping_.getId(appObject);	
	fprintf(file_, indentStr_.c_str());
	fprintf(file_, "<%s>%s</%s>\n", tagName.c_str(), lexical_cast<string>(objId).c_str(), tagName.c_str());
}

void XmlWriter::writeTag(const std::string & tagName, Scene * scene)
{
	string str;
	unsigned int objId = idMapping_.getId(scene);	
	fprintf(file_, indentStr_.c_str());
	fprintf(file_, "<%s>%s</%s>\n", tagName.c_str(), lexical_cast<string>(objId).c_str(), tagName.c_str());
}

void XmlWriter::writeTag(const std::string & tagName, const Vector3 & val)
{
	stringstream ss;
	ss << val.x << ", " << val.y << ", " << val.z;	
	fprintf(file_, indentStr_.c_str());
	fprintf(file_, "<%s>%s</%s>\n", tagName.c_str(), ss.str().c_str(), tagName.c_str());
}

void XmlWriter::writeTag(const std::string & tagName, const Transform & val)
{
	startTag(tagName);
	val.writeXml(*this);
	endTag();
}

void XmlWriter::writeObject(SceneObject * obj)
{
	string attrStr = "Id = \"" + boost::lexical_cast<string>(idMapping_.getId(obj)) + "\"";
	startTag(obj->xmlTag(), attrStr);
	obj->writeXml(*this);
	endTag();
}

void XmlWriter::writeAppObject(AppObject * appObj)
{
	string idStr = boost::lexical_cast<string>(idMapping_.getId(appObj));
	string attrStr = "Id = \"" + idStr + "\"";
	startTag(appObj->xmlTag(), attrStr);
	appObj->writeXml(*this);
	endTag();
}

void XmlWriter::writeEvent(Event * event)
{
	startTag(event->xmlTag());
	event->writeXml(*this);
	endTag();
}

void XmlWriter::writeAction(Action * action)
{
	startTag(action->xmlTag());
	action->writeXml(*this);
	endTag();
}


void XmlWriter::writeText(const std::string & value)
{
	fprintf(file_, "%s\n", value.c_str());
}

void XmlWriter::write(Document * document, const char * fileName)
{
	file_ = NULL;

	BOOST_SCOPE_EXIT( (&file_)) {
		if (file_) fclose(file_);
	} BOOST_SCOPE_EXIT_END

	file_ = fopen(fileName, "wb");
	idMapping_.init(document);
	if (!file_) throw XmlException(NULL, "Couldn't open file to write");
	document_ = document;
	document_->writeXml(*this);		
}

///////////////////////////////////////////////////////////////////////////////

XmlReader::XmlReader()
{
}

XmlReader::~XmlReader()
{
}


Document * XmlReader::read(const char * fileName)
{
	doc_ = NULL;
	document_ = new Document;

	bool read = false;
	BOOST_SCOPE_EXIT( (&read) (&document_) (&doc_)) {
		if (!read) 
		{
			delete document_;
			document_ = NULL;
		}
		if (doc_) xmlFreeDoc(doc_);
	} BOOST_SCOPE_EXIT_END
	
	doc_ = xmlReadFile(fileName, NULL, 0);
	if (!doc_) throw XmlException(NULL, "Couldn't open xml file");
	idMapping_.init(document_);
	xmlNode * node = xmlDocGetRootElement(doc_);
	if (!xmlStrEqual(node->name, (xmlChar *)"Document")) 
		throw XmlException(node, std::string("Was expecting node Document"));
	
	document_->readXml(*this, node);
		
	read = true;
	return document_;
}

void XmlReader::setScene(Scene * scene)
{	
	scene_ = scene;
}


void XmlReader::getNodeContent(Color & val, xmlNode * node)
{
	if (!node || !node->children) return;

	val.r = 0.0f;
	val.g = 0.0f;
	val.b = 0.0f;
	val.a = 1.0f;

	string str = (char *)node->children->content;
	char_separator<char> sep(", ");
	tokenizer < char_separator<char> > tokens(str, sep);
	tokenizer < char_separator<char> >::iterator itr = tokens.begin();
	
	try {
		if (itr != tokens.end()) val.r = lexical_cast<float>(*(itr++));
		if (itr != tokens.end()) val.g = lexical_cast<float>(*(itr++));
		if (itr != tokens.end()) val.b = lexical_cast<float>(*(itr++));
		if (itr != tokens.end()) val.a = lexical_cast<float>(*(itr++));
	} catch (...) {}
}

void XmlReader::getNodeContent(Vector3 & val, xmlNode * node)
{
	using namespace boost;
	if (!node || !node->children) return;

	val.x = 0.0f;
	val.y = 0.0f;
	val.z = 0.0f;

	string str = (char *)node->children->content;
	char_separator<char> sep(", ");
	tokenizer < char_separator<char> > tokens(str, sep);
	tokenizer < char_separator<char> >::iterator itr = tokens.begin();

	try {
		if (itr != tokens.end()) val.x = lexical_cast<float>(*(itr++));
		if (itr != tokens.end()) val.y = lexical_cast<float>(*(itr++));
		if (itr != tokens.end()) val.z = lexical_cast<float>(*(itr++));
	} catch (...) {}
}

void XmlReader::getNodeContent(Transform & val, xmlNode * node)
{
	if (!node || !node->children) return;
	val.readXml(*this, node);	
}


bool XmlReader::getSceneObject(SceneObjectSPtr & val, xmlNode * node)
{
	const vector<SceneObject *> & sceneObjTypes = Global::instance().sceneObjectTypes();

	bool nameMatched = false;
	BOOST_FOREACH(SceneObject * obj, sceneObjTypes)
	{
		if (isNodeName(node, obj->xmlTag()))
		{			
			nameMatched = true;
			val.reset(obj->clone(NULL));
			
			unsigned int objId = 0;
			getNodeAttribute(objId, node, "Id"); 	
			if (!objId) throw XmlException(node, "No object id");

			val->readXml(*this, node);			
			idMapping_.setId(val.get(), objId);
			break;
		}
	}

	return nameMatched;
}


bool XmlReader::getTemplate(DocumentTemplateSPtr & val, xmlNode * node)
{
	vector<DocumentTemplate *> templates;
	Global::instance().documentTemplates(&templates);

	bool nameMatched = false;
	BOOST_FOREACH(DocumentTemplate * tmp, templates)
	{
		if (isNodeName(node, tmp->xmlTag()))
		{
			nameMatched = true;
			val.reset(tmp->newInstance());
			val->readXml(*this, node);			
			break;
		}
	}

	return nameMatched;
}

bool XmlReader::getAppObject(AppObjectSPtr & val, xmlNode * node)
{
	const vector<AppObject *> & types = Global::instance().appObjectTypes();

	bool nameMatched = false;
	BOOST_FOREACH(AppObject * type, types)
	{
		if (isNodeName(node, type->xmlTag()))
		{			
			nameMatched = true;
			val.reset(type->clone());		
			val->readXml(*this, node);				
			break;
		}
	}

	return nameMatched;
}

bool  XmlReader::getEvent(EventSPtr & val, xmlNode * node)
{
	const vector<Event *> & eventTypes = Global::instance().eventTypes();

	bool nameMatched = false;
	BOOST_FOREACH(Event * event, eventTypes)
	{
		if (isNodeName(node, event->xmlTag()))
		{			
			nameMatched = true;
			val.reset(event->clone());
			val->readXml(*this, node);			
			break;
		}
	}

	return nameMatched;
}


bool XmlReader::getAction(ActionSPtr & val, xmlNode * node)
{
	const vector<Action *> & actionTypes = Global::instance().actionTypes();

	bool nameMatched = false;
	BOOST_FOREACH(Action * action, actionTypes)
	{
		if (isNodeName(node, action->xmlTag()))
		{			
			nameMatched = true;
			val.reset(action->clone());
			val->readXml(*this, node);

			break;
		}
	}

	return nameMatched;
}


bool XmlReader::getAnimChannel(AnimationChannelSPtr & val, xmlNode * node)
{
	const vector<AnimationChannel *> & types = Global::instance().channelTypes();

	bool nameMatched = false;
	BOOST_FOREACH(AnimationChannel * type, types)
	{
		if (isNodeName(node, type->xmlTag()))
		{			
			nameMatched = true;
			val.reset(type->clone());
			val->readXml(*this, node);
			break;
		}
	}

	return nameMatched;
}