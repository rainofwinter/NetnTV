#include "stdafx.h"
#include "VideoStoppedEvent.h"
#include "Reader.h"
#include "Writer.h"

void VideoStoppedEvent::read(Reader & reader, unsigned char)
{
	reader.readParent<Event>(this);
	
}

void VideoStoppedEvent::write(Writer & writer) const
{
	writer.writeParent<Event>(this);
}