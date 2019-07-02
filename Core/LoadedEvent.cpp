#include "stdafx.h"
#include "LoadedEvent.h"
#include "Reader.h"
#include "Writer.h"

boost::uuids::uuid LoadedEvent::sUuid_ = boost::uuids::string_generator()("A222122B-AB19-2212-FB6A-333471A31216");


///////////////////////////////////////////////////////////////////////////////

void LoadedEvent::read(Reader & reader, unsigned char)
{
	reader.readParent<Event>(this);
	
}

void LoadedEvent::write(Writer & writer) const
{
	writer.writeParent<Event>(this);
}
