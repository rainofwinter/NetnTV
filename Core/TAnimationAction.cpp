#include "stdafx.h"
#include "TAnimationAction.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Reader.h"
#include "TAnimation.h"
#include "ElementMapping.h"
#include "Global.h"
///////////////////////////////////////////////////////////////////////////////
TAnimationPlayAction::TAnimationPlayAction()
{
}

TAnimationPlayAction::~TAnimationPlayAction()
{	
}

bool TAnimationPlayAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

Action & TAnimationPlayAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	*this = (TAnimationPlayAction &)rhs;
	return *this;
}


bool TAnimationPlayAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == TAnimationObject().type();
}

bool TAnimationPlayAction::start(SceneObject * object, float time)
{	
	TAnimationObject * obj = (TAnimationObject *)targetObject_;
	obj->play(Global::currentTime());
	return true;
}

void TAnimationPlayAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);	
}

void TAnimationPlayAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}
///////////////////////////////////////////////////////////////////////////////

TAnimationStopAction::TAnimationStopAction()
{
}

TAnimationStopAction::~TAnimationStopAction()
{	
}

bool TAnimationStopAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

Action & TAnimationStopAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	*this = (TAnimationStopAction &)rhs;
	return *this;
}


bool TAnimationStopAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == TAnimationObject().type();
}

bool TAnimationStopAction::start(SceneObject * object, float time)
{	
	TAnimationObject * obj = (TAnimationObject *)targetObject_;
	obj->stop();
	return true;
}

void TAnimationStopAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);	
}

void TAnimationStopAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}


///////////////////////////////////////////////////////////////////////////////

TAnimationResetAction::TAnimationResetAction()
{
}

TAnimationResetAction::~TAnimationResetAction()
{	
}

bool TAnimationResetAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

Action & TAnimationResetAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	*this = (TAnimationResetAction &)rhs;
	return *this;
}


bool TAnimationResetAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == TAnimationObject().type();
}

bool TAnimationResetAction::start(SceneObject * object, float time)
{	
	TAnimationObject * obj = (TAnimationObject *)targetObject_;
	obj->reset();
	return true;
}

void TAnimationResetAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);	
}

void TAnimationResetAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}