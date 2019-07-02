#include "stdafx.h"
#include "PageChangingEvent.h"
#include "Reader.h"
#include "Writer.h"

void PageChangingEvent::read(Reader & reader, unsigned char)
{
	reader.readParent<Event>(this);
	
}

void PageChangingEvent::write(Writer & writer) const
{
	writer.writeParent<Event>(this);
}