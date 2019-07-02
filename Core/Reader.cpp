#include "stdafx.h"
#include "Reader.h"
#include "Global.h"
#include "AppObject.h"
#include "SceneObject.h"
#include "Event.h"
#include "Action.h"
#include "DocumentTemplate.h"
#include "AnimationChannel.h"
#include "Document.h"
#include "FileUtils.h"
#include "Exception.h"

using namespace std;

void deleteDocument(Document * document)
{
	delete document;

}

Reader::Reader(const char * fileName)
{
	file_ = fopen(fileName, "rb");
	if (!file_) throw Exception("Could not open file");
	closeFile_ = true;
}

Reader::Reader(FILE * file)
{
	file_ = file;
	closeFile_ = false;
}

Reader::~Reader()
{
	if (closeFile_) fclose(file_);
}


void Reader::read(boost::uuids::uuid & val)
{
	using boost::uuids::uuid;	
	uuid::const_iterator iter;	
	for (iter = val.begin(); iter != val.end(); ++iter)
	{
		read(*iter);
	}
}


void Reader::read(std::string & val)
{	
	unsigned int len;
	read(len);	
	val.resize(len);
	for (unsigned int i = 0; i < len; ++i) read(val[i]);
}

void Reader::read(std::wstring & val)
{	
	unsigned int len;
	read(len);	
	val.resize(len);
	for (unsigned int i = 0; i < len; ++i) 
	{
		unsigned short c;
		read(c);
		val[i] = (wchar_t)c;		
	}
}

template <typename T>
unsigned int readTypedPtr(Reader & reader, T *&val, 
	T * (Global::*createFunc)(const boost::uuids::uuid &) const)
{
	unsigned int index = reader.readPtrIndex();

	if (index == 0)
	{
		val = 0; return index;
	}

	map<unsigned int, Reader::AddressData>::iterator iter = 
		reader.ptrData_.find(index);
	Reader::AddressData ptrData = (*iter).second;

	if (ptrData.addr)
	{
		val = (T *)ptrData.addr;
	}
	else
	{
		boost::uuids::uuid type; 
		reader.readVal(type);		
		val = (Global::instance().*createFunc)(type);
		ptrData.addr = (void *)val;
		reader.ptrData_[index] = ptrData;
		reader.readVal(*val);		
	}

	return index;
}



unsigned int Reader::readPtr(SceneObject *& val, BoolType<true> hasTypeMethod)
{
	return readTypedPtr<SceneObject>(*this, val, &Global::createSceneObject);
}

unsigned int Reader::readPtr(AppObject *& val, BoolType<true> hasTypeMethod)
{
	return readTypedPtr<AppObject>(*this, val, &Global::createAppObject);
}

unsigned int Reader::readPtr(AnimationChannel *& val, BoolType<true> hasTypeMethod)
{
	return readTypedPtr<AnimationChannel>(*this, val, &Global::createAnimationChannel);
}

unsigned int Reader::readPtr(Event *& val, BoolType<true> hasTypeMethod)
{
	return readTypedPtr<Event>(*this, val, &Global::createEvent);	
}

unsigned int Reader::readPtr(Action *& val, BoolType<true> hasTypeMethod)
{
	return readTypedPtr<Action>(*this, val, &Global::createAction);	
}

unsigned int Reader::readPtr(DocumentTemplate *& val, BoolType<true> hasTypeMethod)
{
	return readTypedPtr<DocumentTemplate>(*this, val, &Global::createDocumentTemplate);	
}

unsigned int Reader::readPtrIndex()
{
	unsigned int index;
	bool readBefore;
	AddressData ptrData;
	readVal(index);
	map<unsigned int, AddressData>::iterator iter;
	iter = ptrData_.find(index);
	if (iter != ptrData_.end())
	{
		ptrData = (*iter).second;
		readBefore = true;
	}
	else
	{
		ptrData.addr = 0;
		ptrData.sharedPtrAddr = 0;
		ptrData_[index] = ptrData;
		readBefore = false;
	}

	return index;

}