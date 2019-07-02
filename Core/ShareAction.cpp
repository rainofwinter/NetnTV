#include "stdafx.h"
#include "ShareAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Document.h"
#include "Global.h"


ShareAction::ShareAction()
{		
	targetType_ = TextTarget;
	imageNum_ = -1;
	filename_ = NULL;	
}

ShareAction::~ShareAction()
{		
}


Action & ShareAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (ShareAction &)rhs;
	return *this;
}


bool ShareAction::hasDuration() const
{
	return false;
}

bool ShareAction::isExecuting() const
{
	return false;
}

bool ShareAction::update(SceneObject * object, float time)
{
	//if (targetType_ == TextTarget)
	//	Global::instance().shareMsg(0, message_);
	//else
	//	Global::instance().shareMsg(1, message_);
	return false;
}

void ShareAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	
	writer.write(message_, "message");	
	writer.write(targetType_, "targetType");
	writer.write(imageNum_, "imageNumber");
	//writer.write(filename_, "fileName");

}

void ShareAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	reader.read(message_);
	reader.read(targetType_);
	
	if (version >= 1)
	{
		reader.read(imageNum_);
	}	
	/*if (version >= 2)
	{		
		reader.read(filename_);
	}	*/
}

void ShareAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Message", message_);	
	w.writeTag("TargetType", (unsigned int)targetType_);	
}

void ShareAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		
		if (r.getNodeContentIfName(message_, curNode, "Message"));		
		else if (r.getNodeContentIfName(val, curNode, "Message")) targetType_ = (TargetType)val;
	}
}

bool ShareAction::start(SceneObject * object, float time)
{	
	Scene * scene = object->parentScene();

	Document * document = scene->parentDocument();	
	
	if (targetType_ == TextTarget)
		Global::instance().shareMsg(TextTarget, message_);	
	else if (targetType_ == UrlTarget)
		Global::instance().shareMsg(UrlTarget, message_);
	/*
	else if (targetType_ == ImageTarget){		
		Global::instance().shareMsg(ImageTarget, message_);
		//Global::instance().shareMsg(ImageTarget, *filename_);		
	}*/
	return true;
}

void ShareAction::referencedFiles(std::vector<std::string> * refFiles) const
{
	Action::referencedFiles(refFiles);	
}

int ShareAction::setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index)
{
	index = Action::setReferencedFiles(baseDirectory, refFiles, index);
	if(filename_)
		message_ = *filename_;
	return index;
}
