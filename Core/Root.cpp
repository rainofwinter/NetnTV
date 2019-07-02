#include "stdafx.h"
#include "Root.h"
#include "Reader.h"
#include "Writer.h"
#include "Attrib.h"
#include "Xml.h"
#include "Global.h"

using namespace std;

Root::Root()
{
	setId("Root");
}

Root::Root(const Root & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
}


void Root::draw(GfxRenderer * gl) const
{
	BOOST_FOREACH(const SceneObjectSPtr & obj, children_)
	{
		obj->draw(gl);
	}
}

BoundingBox Root::extents() const
{
	BoundingBox ret;
	BOOST_FOREACH(const SceneObjectSPtr & child, children_)
	{
		VisualAttrib * attr = child->visualAttrib();
		if (!attr) continue;
		const Matrix & matrix = attr->transform().computeMatrix();
		ret = ret.unite(child->extents().transform(matrix));
	}
	return ret;
}

void Root::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
}

void Root::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("Children");
	for (unsigned i = 0; i < (unsigned)children_.size(); ++i)
	{
		w.writeObject(children_[i].get());
	}
	w.endTag();
}

void Root::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
}

void Root::readXml(XmlReader & r, xmlNode * parent)
{	
	SceneObject::readXml(r, parent);

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(curNode, "Children"))
		{
			readXmlChildren(r, curNode);
		}
	}
}

void Root::readXmlChildren(XmlReader & r, xmlNode * parent)
{	
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{				
		if (curNode->type != XML_ELEMENT_NODE) continue;
		SceneObjectSPtr s;
		if (r.getSceneObject(s, curNode)) children_.push_back(s);
	}
}

void Root::requestLocalSaveObject()
{
	BOOST_FOREACH(const SceneObjectSPtr & obj, children_)
	{
		obj->requestLocalSaveObject();
	}
}