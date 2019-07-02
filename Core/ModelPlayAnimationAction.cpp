#include "stdafx.h"
#include "ModelPlayAnimationAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ModelFile.h"
#include "Model.h"
#include "ElementMapping.h"

ModelPlayAnimationAction::ModelPlayAnimationAction()
{
	modelFile_ = NULL;
}

ModelPlayAnimationAction::~ModelPlayAnimationAction()
{	
}

bool ModelPlayAnimationAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

Action & ModelPlayAnimationAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (ModelPlayAnimationAction &)rhs;
	return *this;
}


bool ModelPlayAnimationAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == ModelFile().type();
}

bool ModelPlayAnimationAction::start(SceneObject * object, float time)
{		
	modelFile_->playAnimation(modelAnimation_);	
	return true;
}

bool ModelPlayAnimationAction::hasDuration() const
{
	return true;
}

bool ModelPlayAnimationAction::isExecuting() const
{
	return false;
}

bool ModelPlayAnimationAction::update(SceneObject * object, float time)
{
	return false;
}

void ModelPlayAnimationAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(modelFile_);
	writer.write(modelAnimation_);
}

void ModelPlayAnimationAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	reader.read(modelFile_);
	reader.read(modelAnimation_);
}

void ModelPlayAnimationAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.startTag(modelFile_->xmlTag());
	modelFile_->writeXml(w);
	w.endTag();
	w.startTag(modelAnimation_.xmlTag());
	modelAnimation_.writeXml(w);
	w.endTag();
}

void ModelPlayAnimationAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.isNodeName(curNode, modelFile_->xmlTag()))
		{
			modelFile_->readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.isNodeName(curNode, modelAnimation_.xmlTag()))
		{
			modelAnimation_.readXml(r, curNode);
			curNode = curNode->next;
		}
	}
}