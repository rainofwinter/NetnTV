#include "stdafx.h"
#include "EffectAction.h"
#include "Image.h"
#include "Scene.h"
#include "Group.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"

enum EffectV0
{
	OldFadeIn = 0,
	OldFlipDown,
	OldSpinHorz,
	OldSpinVert,
	OldFadeOut
};

EffectAction::EffectAction()
{
	endTime_ = 0;
	duration_ = 0.5f;
	effect_ = FadeIn;

	isExecuting_ = false;
}

EffectAction::~EffectAction()
{	
}

bool EffectAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
	
}

Action & EffectAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (EffectAction &)rhs;
	return *this;
}

bool EffectAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == Image().type();	
}

bool EffectAction::start(SceneObject * object, float time)
{
	isExecuting_ = true;
	startTime_ = time;
	endTime_ = time + duration_;

	initVisualAttrib_ = *targetObject_->visualAttrib();
	Matrix M = initVisualAttrib_.transformMatrix();
	if (effect_ == FlipDown)
	{
		axis_ = Vector3(1, 0, 0);		
		axis_ = (M * axis_ - M * Vector3(0, 0, 0)).normalize();
		imgOrigin_ = M * Vector3(0, 0, 0);
	}
	
	if (effect_ == SpinHorz)
	{
		BoundingBox bbox = targetObject_->extents();
		axis_ = Vector3(0, 1, 0);
		axis_ = (M * axis_ - M * Vector3(0, 0, 0)).normalize();
		imgOrigin_ = M * bbox.midPt();
	}
	if (effect_ == SpinVert)
	{
		BoundingBox bbox = targetObject_->extents();
		axis_ = Vector3(1, 0, 0);
		axis_ = (M * axis_ - M * Vector3(0, 0, 0)).normalize();
		imgOrigin_ = M * bbox.midPt();
	}

	return false;
}

void EffectAction::stop() 
{	
	VisualAttrib * targetAttr = targetObject_->visualAttrib();

	targetAttr->setTransform(initVisualAttrib_.transform());
	targetAttr->setOpacity(initVisualAttrib_.opacity());
	targetAttr->setVisible(initVisualAttrib_.isVisible());

	isExecuting_ = false;
}

bool EffectAction::hasDuration() const
{
	return true;
}

bool EffectAction::isExecuting() const
{
	return isExecuting_;
}

bool EffectAction::update(SceneObject * object, float time)
{
	float t = (time - startTime_)/duration_;
	if (t > 1) t = 1;
	t = 1-(t-1)*(t-1);
	

	VisualAttrib * attr = targetObject_->visualAttrib();

	switch(effect_)
	{
	case FlipDown:
		{
			float angle = -M_PI_2 * (1 - t);
			if (angle > 0) angle = 0;

			attr->setOpacity(t);

			Matrix rotMat = 
				Matrix::Translate(imgOrigin_.x, imgOrigin_.y, imgOrigin_.z) *
				Matrix::Rotate(angle, axis_.x, axis_.y, axis_.z) *
				Matrix::Translate(-imgOrigin_.x, -imgOrigin_.y, -imgOrigin_.z);

			Transform transform = initVisualAttrib_.transform();
			transform.concatenate(rotMat);
			attr->setTransform(transform);
			if (t==1) attr->setTransform(initVisualAttrib_.transform());
			break;
		}
	case FadeIn:
		attr->setOpacity(t);
		break;
	case FadeOut:
		attr->setOpacity(1-t);
		break;
	case SpinHorz:
	case SpinVert:
		float angle = M_PI + t*(M_PI);	
		attr->setOpacity(t);

		Matrix rotMat = 
			Matrix::Translate(imgOrigin_.x, imgOrigin_.y, imgOrigin_.z) *
			Matrix::Rotate(angle, axis_.x, axis_.y, axis_.z) *
			Matrix::Translate(-imgOrigin_.x, -imgOrigin_.y, -imgOrigin_.z);

		Transform transform = initVisualAttrib_.transform();		
		transform.concatenate(rotMat);
		attr->setTransform(transform);
		if (t==1) attr->setTransform(initVisualAttrib_.transform());

		break;
	}
	
	if (t >= 1) isExecuting_ = false;
	return true;
}

void EffectAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(effect_, "effect");	
	writer.write(duration_, "duration");
}

void EffectAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	if (version == 0)
	{
		EffectV0 oldEffect;
		reader.read(oldEffect);

		switch(oldEffect)
		{
		case OldFadeIn: effect_ = FadeIn; break;
		case OldFlipDown: effect_ = FlipDown; break;
		case OldSpinHorz: effect_ = SpinHorz; break;
		case OldSpinVert: effect_ = SpinVert; break;
		case OldFadeOut: effect_ = FadeOut; break;
		}
	}
	else
	{
		reader.read(effect_);
	}
	reader.read(duration_);
}

void EffectAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Effect", (unsigned int)effect_);
	w.writeTag("Duration", duration_);
}

void EffectAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(val, curNode, "Effect")) effect_ = (Effect)val;
		else if (r.getNodeContentIfName(duration_, curNode, "Duration"));
	}
}