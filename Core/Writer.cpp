#include "stdafx.h"
#include "Writer.h"
#include "Document.h"
#include "Scene.h"
#include "SceneObject.h"
#include "EventListener.h"
#include "Event.h"
#include "Action.h"
#include "Animation.h"
#include "MathStuff.h"
#include "FileUtils.h"

template <typename T>
struct SetVal
{
	SetVal(T & val, const T & newVal) : val_(val)
	{
		origVal_ = val;
		val_ = newVal;
	}

	~SetVal()
	{
		val_ = origVal_;
	}
private:
	T origVal_;
	T & val_;
};


Writer::Writer(const char * fileName, const char * logFileName)
{
	file_ = fopen(fileName, "wb");

	if (!file_) throw Exception(FileIO, "could not open file");
	indentLevel_ = 0;
	loggingEnabled_ = false;
	if (logFileName)
	{
		logFileStream_.open(logFileName);
		loggingEnabled_ = true;
	}
	curPtrIndex_ = 1;

}

Writer::~Writer()
{
	fclose(file_);	
	logFileStream_.close();
}


void Writer::write(const boost::uuids::uuid & val, const char * logMsg)
{	
	writeLogMsg(val, logMsg);
	SetVal<bool> setVal(loggingEnabled_, false);	
	using boost::uuids::uuid;	
	uuid::const_iterator iter;	
	for (iter = val.begin(); iter != val.end(); ++iter)
	{
		write(*iter);
	}
}


void Writer::write(const std::wstring & val, const char * logMsg)
{	
	writeLogMsg(val, logMsg);
	SetVal<bool> setVal(loggingEnabled_, false);	
	unsigned int len = (unsigned int)val.size();	
	write(len);
	for (unsigned int i = 0; i < len; ++i) 
	{
		unsigned short c = (unsigned short)val[i];
		write(c);
	}
}


void Writer::write(const std::string & val, const char * logMsg)
{	
	writeLogMsg(val, logMsg);
	SetVal<bool> setVal(loggingEnabled_, false);	
	unsigned int len = (unsigned int)val.length();	
	write(len);
	for (unsigned int i = 0; i < len; ++i) write(val[i]);
}

bool Writer::writePtrIndex(void * val, const char * logMsg)
{
	if (!val) 
	{
		write((unsigned int)0); 
		return false;
	}

	AddressData ptrData;
	bool writtenBefore;
	std::map<void *, AddressData>::iterator iter = ptrData_.find(val);
	//if a ptr at this address has already been written before
	if (iter != ptrData_.end())
	{
		ptrData = (*iter).second;			
		writtenBefore = true;
	}
	else
	{
		ptrData.index = curPtrIndex_++;
		ptrData.sharedPtrAddr = 0;
		ptrData_[val] = ptrData;
		writtenBefore = false;
	}
	std::stringstream ss;	
	if (logMsg) ss << logMsg << " ptr-index";	
	else ss << "ptr-index";	

	write(ptrData.index, ss.str().c_str());	
	return writtenBefore;
}