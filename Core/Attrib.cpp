#include "stdafx.h"
#include "Attrib.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "SceneObject.h"

VisualAttrib::VisualAttrib()
{	
	transformMatrix_ = transform_.computeMatrix();
	opacity_ = 1;
	visible_ = true;
}

void VisualAttrib::write(Writer & writer) const
{
	writer.write(transform_, "transform");
	writer.write(opacity_, "opacity");
	writer.write(visible_, "visible");
}

void VisualAttrib::writeXml(XmlWriter & w) const
{
	w.startTag("Transform");
	transform_.writeXml(w);
	w.endTag();

	w.writeTag("Opacity", opacity_);
	w.writeTag("Visible", visible_);
}

void VisualAttrib::read(Reader & reader, unsigned char version)
{
	reader.read(transform_);
	transformMatrix_ = transform_.computeMatrix();
	reader.read(opacity_);
	reader.read(visible_);
}

void VisualAttrib::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(curNode, "Transform"))
		{
			transform_.readXml(r, curNode);
			transformMatrix_ = transform_.computeMatrix();
		}
		else if (r.getNodeContentIfName(opacity_, curNode, "Opacity"));
		else if (r.getNodeContentIfName(opacity_, curNode, "Visible"));
	}
}

void VisualAttrib::triggerOnSetTransform(SceneObject * sceneObject)
{
	sceneObject->triggerOnSetTransform();
}