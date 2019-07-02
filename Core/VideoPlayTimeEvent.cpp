#include "stdafx.h"
#include "VideoPlayTimeEvent.h"
#include "Reader.h"
#include "Writer.h"
#include "Xml.h"

void VideoPlayTimeEvent::read(Reader & reader, unsigned char)
{
	reader.readParent<Event>(this);
	reader.read(time_);
	
}

void VideoPlayTimeEvent::write(Writer & writer) const
{
	writer.writeParent<Event>(this);
	writer.write(time_);
}

void VideoPlayTimeEvent::writeXml(XmlWriter & w) const
{
	w.writeTag("Time", time_);
}

void VideoPlayTimeEvent::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(time_, curNode, "Time"));
	}
}