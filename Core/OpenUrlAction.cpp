#include "stdafx.h"
#include "OpenUrlAction.h"
#include "Animation.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "ElementMapping.h"
#include "Global.h"
#include "VideoPlayer.h"
#include "FileUtils.h"

using namespace std;

OpenUrlAction::OpenUrlAction()
{	
	mode_ = Regular;
}

OpenUrlAction::~OpenUrlAction()
{	
}

Action & OpenUrlAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (OpenUrlAction &)rhs;
	return *this;
}


bool OpenUrlAction::start(SceneObject * object, float time)
{
	Scene * scene = object->parentScene();
	if (!scene->isCurrentScene()) return true;

	Global::instance().openUrl(url_, mode_ == Mobile);
	return true;
}

bool OpenUrlAction::hasDuration() const
{
	return false;
}

bool OpenUrlAction::isExecuting() const
{
	return false;
}

bool OpenUrlAction::update(SceneObject * object, float time)
{
	return false;
}

void OpenUrlAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(url_, "url");	
	writer.write(mode_, "mode");
}

void OpenUrlAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
	reader.read(url_);
	if (version >= 1)
	{
		reader.read(mode_);
	}
}

void OpenUrlAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Url", url_);
	w.writeTag("Mode", (unsigned int)mode_);
}

void OpenUrlAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(url_, curNode, "Url"));
		else if (r.getNodeContentIfName(val, curNode, "Mode")) mode_ = (Mode)val;
	}
}