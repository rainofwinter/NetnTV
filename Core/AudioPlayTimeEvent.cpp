#include "stdafx.h"
#include "AudioPlayTimeEvent.h"
#include "Reader.h"
#include "Writer.h"
#include "Xml.h"

void AudioPlayTimeEvent::read(Reader & reader, unsigned char)
{
	reader.readParent<Event>(this);
	reader.read(time_);
	
}

void AudioPlayTimeEvent::write(Writer & writer) const
{
	writer.writeParent<Event>(this);
	writer.write(time_);
}

void AudioPlayTimeEvent::writeXml(XmlWriter & w) const
{
	w.writeTag("Time", time_);
}

void AudioPlayTimeEvent::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(time_, curNode, "Time"));
	}
}