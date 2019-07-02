#include "stdafx.h"
#include "Exception.h"
#include "Attrib.h"
#include "ClickEvent.h"
#include "SceneObject.h"
#include "AppObject.h"

bool ClickEvent::supportsObject(SceneObject * obj) const
{
	return obj->visualAttrib() != NULL;
}

bool ClickEvent::supportsObject(AppObject * obj) const
{
	return obj->visualAttrib() != NULL;
}