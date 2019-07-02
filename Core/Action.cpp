#include "stdafx.h"
#include "Action.h"
#include "Writer.h"
#include "Reader.h"
#include "SceneObject.h"
#include "AppObject.h"
#include "Xml.h"
#include "ElementMapping.h"

bool Action::remapReferences(const ElementMapping & mapping)
{
	if (!targetObject_) return true;

	targetObject_ = mapping.mapObject(targetObject_);
	if (!targetObject_) return false;
	return true;
}

bool Action::dependsOn(SceneObject * obj) const
{
	SceneObject * curObj = targetObject_;
	while (curObj)
	{
		if (curObj == obj) return true;
		curObj = curObj->parent();
	}

	return false;	
}


unsigned char Action::version() const
{
	return 0;
}

void Action::write(Writer & writer) const
{		
	writer.write(targetObject_, "targetObject");	
}

void Action::writeXml(XmlWriter & w) const
{
	w.writeTag("TargetObject", targetObject_);
}

void Action::read(Reader & reader, unsigned char)
{
	reader.read(targetObject_);
}

void Action::readXml(XmlReader & r, xmlNode * parent)
{
	unsigned int objId = 0;

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(objId, curNode, "TargetObject"))
		{
			targetObject_ = (SceneObject *)objId;
		}
	}

}

bool Action::scenePreStart(SceneObject * object, float time)
{
	if (needsScenePreStart()) return start(object, time);
	return false;
}

bool Action::scenePreStartAppObj(AppObject * object, float time)
{
	if (needsScenePreStart()) return startAppObj(object, time);
	return false;
}