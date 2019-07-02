#include "stdafx.h"
#include "SetCameraAction.h"
#include "CameraObject.h"
#include "ElementMapping.h"
#include "Scene.h"
#include "Reader.h"
#include "Writer.h"
#include "Xml.h"

SetCameraAction::SetCameraAction()
{
	cameraObj_ = NULL;
}

SetCameraAction::~SetCameraAction()
{	
}

bool SetCameraAction::dependsOn(SceneObject * obj) const 
{	
	SceneObject * curObj = cameraObj_;
	while (curObj)
	{
		if (curObj == obj) return true;
		curObj = curObj->parent();
	}

	return false;	
}

bool SetCameraAction::dependsOn(Scene * scene) const
{
	return false;
}

Action & SetCameraAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (SetCameraAction &)rhs;
	return *this;
}

bool SetCameraAction::remapReferences(const ElementMapping & mapping)
{
	cameraObj_ = (CameraObject *)mapping.mapObject(cameraObj_);
	return Action::remapReferences(mapping);
}

bool SetCameraAction::supportsObject(SceneObject * obj) const
{
	return !obj;
}

bool SetCameraAction::start(SceneObject * object, float time)
{
	object->parentScene()->setUserCamera(cameraObj_);	
	return true;
}

bool SetCameraAction::hasDuration() const
{
	return false;
}

bool SetCameraAction::isExecuting() const
{
	return false;
}

bool SetCameraAction::update(SceneObject * object, float time)
{
	return false;
}

void SetCameraAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(cameraObj_, "camera object");
	
}

void SetCameraAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	reader.read(cameraObj_);	
}
void SetCameraAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.startTag(cameraObj_->xmlTag());
	cameraObj_->writeXml(w);
	w.endTag();
}

void SetCameraAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.isNodeName(curNode, cameraObj_->xmlTag()))
		{
			cameraObj_->readXml(r, curNode);
			curNode = curNode->next;
		}
	}
}