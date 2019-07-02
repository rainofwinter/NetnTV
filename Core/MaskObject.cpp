#include "stdafx.h"
#include "Group.h"
#include "MaskObject.h"
#include "Writer.h"
#include "Reader.h"
#include "Scene.h"
#include "Global.h"
#include "Document.h"
#include "GfxRenderer.h"
#include "ElementMapping.h"
#include "EventListener.h"
#include "Xml.h"

using namespace std;
///////////////////////////////////////////////////////////////////////////////

MaskObjectVisualAttrib::MaskObjectVisualAttrib(MaskObject * sceneObject)
{
	sceneObject_ = sceneObject;
}

MaskObjectVisualAttrib & MaskObjectVisualAttrib::operator = (const MaskObjectVisualAttrib & rhs)
{
	*(VisualAttrib *)this = *(VisualAttrib *)&rhs;
	return *this;
}

MaskObjectVisualAttrib & MaskObjectVisualAttrib::operator = (const VisualAttrib & rhs)
{
	*(VisualAttrib *)this = rhs;
	return *this;
}

void MaskObjectVisualAttrib::onSetTransform()
{
	triggerOnSetTransform(sceneObject_->maskingObjectGroup());
	BOOST_FOREACH(const SceneObjectSPtr & child, sceneObject_->children())
	{
		triggerOnSetTransform(child.get());		
	}

}

void MaskObjectVisualAttrib::write(Writer & writer) const
{
	writer.writeParent<VisualAttrib>(this);
}

void MaskObjectVisualAttrib::read(Reader & reader, unsigned char version)
{
	reader.readParent<VisualAttrib>(this);
}

//void MaskObjectVisualAttrib::writeXml(XmlWriter & w) const
//{
//	w.write
//}
//
//void MaskObjectVisualAttrib::readXml(XmlReader & r, xmlNode * node)
//{
//	reader.readParent<VisualAttrib>(this);
//}
///////////////////////////////////////////////////////////////////////////////

MaskObject::MaskObject() : visualAttrib_(this)
{
	maskingObjectGroup_.reset(new Group);
	maskingObjectGroup_->setId("<masking objects>");
	maskingObjectGroup_->setParent(this);
	setId("MaskObject");
}

MaskObject::MaskObject(const MaskObject & rhs, ElementMapping * elementMapping)
	: SceneObject(rhs, elementMapping), visualAttrib_(this)
{
	visualAttrib_ = rhs.visualAttrib_;
	maskingObjectGroup_.reset((Group *)rhs.maskingObjectGroup_->clone(elementMapping));
	maskingObjectGroup_->parent_ = this;
}

void MaskObject::remapReferences(const ElementMapping & elementMapping)
{
	SceneObject::remapReferences(elementMapping);

	maskingObjectGroup_->remapReferences(elementMapping);
}


void MaskObject::init(GfxRenderer * gl, bool firstTime)
{		
	gl->registerMaskObject(this);
	SceneObject::init(gl, firstTime);
	maskingObjectGroup_->init(gl, firstTime);	
}

void MaskObject::uninit()
{
	if (gl_) gl_->unregisterMaskObject(this);
	maskingObjectGroup_->uninit();
	SceneObject::uninit();
	
	
}

bool MaskObject::update(float sceneTime)
{
	bool needToRedraw = SceneObject::update(sceneTime);
	needToRedraw |= maskingObjectGroup_->update(sceneTime);

	return needToRedraw;
}

bool MaskObject::asyncLoadUpdate()
{
	bool everythingLoaded = true;
	
	everythingLoaded = SceneObject::asyncLoadUpdate();
	everythingLoaded &= maskingObjectGroup_->asyncLoadUpdate();

	return everythingLoaded;
}


void MaskObject::referencedFiles(std::vector<std::string> * refFiles) const
{
	BOOST_FOREACH(EventListenerSPtr listener, eventListeners_)
	{
		listener->referencedFiles(refFiles);
	}

	maskingObjectGroup_->referencedFiles(refFiles);

	BOOST_FOREACH(SceneObjectSPtr child, children_)		
		child->referencedFiles(refFiles);
}

int MaskObject::setReferencedFiles(
	const std::string & baseDirectory,
	const vector<string> & refFiles, int startIndex)
{
	int i = startIndex;
	BOOST_FOREACH(EventListenerSPtr listener, eventListeners_)
	{
		i = listener->setReferencedFiles(baseDirectory, refFiles, i);
	}

	i = maskingObjectGroup_->setReferencedFiles(baseDirectory, refFiles, i);

	BOOST_FOREACH(SceneObjectSPtr child, children_)		
		i = child->setReferencedFiles(baseDirectory, refFiles, i);

	return i;
}

void MaskObject::setParentScene(Scene * parentScene)
{
	maskingObjectGroup_->setParentScene(parentScene);
	SceneObject::setParentScene(parentScene);	
}

void MaskObject::drawObject(GfxRenderer * gl) const
{	
}

void MaskObject::draw(GfxRenderer * gl) const
{	
	if (!visualAttrib_.isVisible() || totalOpacity() == 0) return;

	gl->pushMatrix();
	gl->multMatrix(visualAttrib_.transformMatrix());

	
	gl->beginMaskDrawing();		
	maskingObjectGroup_->draw(gl);
	gl->endMaskDrawing();

	gl->beginMaskedDrawing();
	BOOST_FOREACH(const SceneObjectSPtr & obj, children_)obj->draw(gl);
	gl->endMaskedDrawing();
	
	gl->popMatrix();
	
}

void MaskObject::computeMaskRegion(GfxRenderer * gl, float & x0, float & y0, float & x1, float & y1) const
{
	BoundingBox b = maskingObjectGroup_->extents();
	Matrix mvpMatrix = gl->computeMvpMatrix() * maskingObjectGroup_->visualAttrib()->transformMatrix();

	tempVerts_[0] = Vector3(b.minPt.x, b.minPt.y, b.minPt.z);
	tempVerts_[1] = Vector3(b.minPt.x, b.maxPt.y, b.minPt.z);
	tempVerts_[2] = Vector3(b.maxPt.x, b.maxPt.y, b.minPt.z);
	tempVerts_[3] = Vector3(b.maxPt.x, b.minPt.y, b.minPt.z);

	tempVerts_[4] = Vector3(b.minPt.x, b.minPt.y, b.maxPt.z);
	tempVerts_[5] = Vector3(b.minPt.x, b.maxPt.y, b.maxPt.z);
	tempVerts_[6] = Vector3(b.maxPt.x, b.maxPt.y, b.maxPt.z);
	tempVerts_[7] = Vector3(b.maxPt.x, b.minPt.y, b.minPt.z);

	float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;

	for (int i = 0; i < 8; ++i)
	{
		tempVerts_[i] = mvpMatrix * tempVerts_[i];

		if (tempVerts_[i].x < minX) minX = tempVerts_[i].x;
		if (tempVerts_[i].y < minY) minY = tempVerts_[i].y;
		if (tempVerts_[i].x > maxX) maxX = tempVerts_[i].x;
		if (tempVerts_[i].y > maxY) maxY = tempVerts_[i].y;
	}

	x0 = minX;
	y0 = maxY;
	x1 = maxX;
	y1 = minY;
}


BoundingBox MaskObject::extents() const
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

void MaskObject::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_);
	writer.write(maskingObjectGroup_);
}

void MaskObject::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	reader.read(maskingObjectGroup_);
	maskingObjectGroup_->parent_ = this;
}

void MaskObject::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	w.startTag("MaskingObjectGroups");
	Group* group = maskingObjectGroup_.get();
	w.startTag(group->xmlTag());
	group->writeXml(w);
	w.endTag();
	w.endTag();
}

void MaskObject::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;		
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.isNodeName(curNode, "MaskingObjectGroups"))
		{
			maskingObjectGroup_.get()->readXml(r, curNode);
			curNode = curNode->next;
			maskingObjectGroup_->parent_ = this;
		}
	}
}