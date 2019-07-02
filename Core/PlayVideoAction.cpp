#include "stdafx.h"
#include "PlayVideoAction.h"
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
#include "Document.h"
#include "DocumentTemplate.h"

using namespace std;

PlayVideoAction::PlayVideoAction()
{	
	playInRegion_ = false;
	x_ = 0;
	y_ = 0;
	width_ = 320;
	height_ = 240;
	sourceType_ = SourceFile;
	showPlaybackControls_ = true;
	startTime_ = 0.0f;
	isPlaying_ = false;
}

PlayVideoAction::~PlayVideoAction()
{	
}


void PlayVideoAction::referencedFiles(
	std::vector<std::string> * refFiles) const
{
	if (sourceType_ == SourceFile) refFiles->push_back(source_);
}

int PlayVideoAction::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	if (sourceType_ == SourceFile) source_ = refFiles[index++];
	return index;
}

Action & PlayVideoAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	
	*this = (PlayVideoAction &)rhs;
	return *this;
}

bool PlayVideoAction::start(SceneObject * object, float time)
{
	Scene * scene = object->parentScene();
	Document * document = scene->parentDocument();
	
	if (!scene->isCurrentScene())
		return true;

	ContainerObjects objs;
	scene->getContainerObjects(&objs);

	if (playInRegion_)
	{
		document->playVideo(
			NULL, objs, source_,
			x_, y_, width_, height_, sourceType_ == SourceUrl, 
			showPlaybackControls_, startTime_);
	}
	else
	{
		document->playVideoFullscreen(NULL,
			objs, source_, sourceType_ == SourceUrl, 
			showPlaybackControls_, startTime_);
	}
	
	isPlaying_ = true;
	
	return true;
}

bool PlayVideoAction::hasDuration() const
{
	return false;
}

bool PlayVideoAction::isExecuting() const
{
	return isPlaying_;
}

bool PlayVideoAction::update(SceneObject * object, float time)
{
	return false;
}

void PlayVideoAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);
	writer.write(source_, "video filename");	

	writer.write(playInRegion_, "playInRegion");
	writer.write(x_, "x");
	writer.write(y_, "y");
	writer.write(width_, "width");
	writer.write(height_, "height");
	writer.write(sourceType_, "sourceType");
	writer.write(showPlaybackControls_, "showControls");
	writer.write(startTime_, "startTime");
}

void PlayVideoAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);	
	reader.read(source_);

	if (version >= 1)
	{
		reader.read(playInRegion_);
		reader.read(x_);
		reader.read(y_);
		reader.read(width_);
		reader.read(height_);
	}


	if (version >= 2)
	{
		reader.read(sourceType_);
	}
	else sourceType_ = SourceFile;

	if (version >= 3)
	{
		reader.read(showPlaybackControls_);
	}	

	if (version >= 4)
	{
		reader.read(startTime_);
	}
}

void PlayVideoAction::writeXml(XmlWriter & w) const
{
	Action::writeXml(w);
	w.writeTag("Source", source_);
	w.writeTag("PlayInRegion", playInRegion_);
	w.writeTag("X", x_);
	w.writeTag("Y", y_);
	w.writeTag("Width", width_);
	w.writeTag("Height", height_);
	w.writeTag("SourceType", (unsigned int)sourceType_);
	w.writeTag("ShowPlaybackControls", showPlaybackControls_);
	w.writeTag("StartTime", startTime_);
}

void PlayVideoAction::readXml(XmlReader & r, xmlNode * parent)
{
	Action::readXml(r, parent);

	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(source_, curNode, "Source"));
		else if (r.getNodeContentIfName(playInRegion_, curNode, "PlayInRegion"));
		else if (r.getNodeContentIfName(x_, curNode, "X"));
		else if (r.getNodeContentIfName(y_, curNode, "Y"));
		else if (r.getNodeContentIfName(width_, curNode, "Width"));
		else if (r.getNodeContentIfName(height_, curNode, "Height"));
		else if (r.getNodeContentIfName(val, curNode, "SourceType")) sourceType_ = (SourceType)val;
		else if (r.getNodeContentIfName(showPlaybackControls_, curNode, "ShowPlaybackControls"));
		else if (r.getNodeContentIfName(startTime_, curNode, "StartTime"));
	}
}