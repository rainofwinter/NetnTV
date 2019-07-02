#ifndef Writer_h__
#define Writer_h__

#include "Traits.h"
class SceneObject;
class EventListener;
class Event;
class Action;
class Vector3;
class Document;
class Scene;
class Animation;


class Writer
{
private:

	struct AddressData
	{
		void * sharedPtrAddr;
		unsigned int index;
	};
	std::map<void *, AddressData> ptrData_;
	unsigned int curPtrIndex_;

	template <bool N> struct BoolType {};
public:
	Writer(const char * fileName, const char * logFileName);
	~Writer();

	//-------------------------------------------------------------------------
	template <typename T>
	void write(const std::vector<T> & objVector, const char * logMsg = 0);

	template <typename T>
	void write(const std::set<T> & objSet, const char * logMsg = 0);

	template <typename T, typename S>
	void write(const std::map<T, S> & objMap, const char * logMsg = 0);

	template <typename T>
	void write(const boost::shared_ptr<T> & sharedPtr, const char * logMsg = 0);

	void write(const boost::uuids::uuid & val, const char * logMsg = 0);
	void write(const std::wstring & val, const char * logMsg = 0);	
	void write(const std::string & val, const char * logMsg = 0);
	
	//-------------------------------------------------------------------------
	//if val is a pointer, take appropriate steps
	template <typename T>
	void write(const T & val, const char * logMsg = 0)
	{
		write(val, logMsg, BoolType<boost::is_pointer<T>::value>());
	}

	template <typename T>
	void write(const T & val, const char * logMsg, BoolType<true> isPointer)
	{
		writePtr(val, logMsg, 
			BoolType<HasTypeMethod<typename boost::remove_pointer<T>::type>::value>());
		
	}

	template <typename T>
	void write(const T & val, const char * logMsg, BoolType<false> isPointer)
	{
		writeVal(val, logMsg);
	}
	//-------------------------------------------------------------------------
	///write pointer differently based on whether it has a type() method
	///caution: if the type does not have a type() method it should not be
	///polymorphic
	template <typename T>
	void writePtr(const T * val, const char * logMsg, BoolType<true> hasTypeMethod)
	{		
		bool writtenBefore = writePtrIndex((void *)val, logMsg);
		if (!val) return;

		if (!writtenBefore)
		{
			std::stringstream ss;
			if (logMsg) ss << logMsg << " ptr-type";	
			else ss << "ptr-type";
			write(val->type(), ss.str().c_str());
			write(*val, logMsg);
		}
	}

	template <typename T>
	void writePtr(const T * val, const char * logMsg, BoolType<false> hasTypeMethod)
	{
		bool writtenBefore = writePtrIndex((void *)val, logMsg);
		if (!val) return;

		if (!writtenBefore)
		{
			write(*val, logMsg);
		}
	}

	/**
	@return whether this address has been written before
	*/
	bool writePtrIndex(void * val, const char * logMsg);
	//-------------------------------------------------------------------------
	/**
	If val has a void T::write(Writer &) const 
	member function, use that. Otherwise just do an fwrite.
	*/
	template <typename T>
	void writeVal(const T & val, const char * logMsg = 0)
	{		
		writeVal(val, logMsg, BoolType<HasWriteMethod<T>::value>());
	}

	template <typename T>
	void writeVal(const T & val, const char * logMsg, BoolType<true> hasWrite)
	{
		writeLogMsg_Empty(val, logMsg);
		addIndentLevel();
		
		//version info
		unsigned char version = val.version();
		writeLogMsg(version, "version");
		fwrite(&version, sizeof(version), 1, file_);
		
		val.write(*this);
		removeIndentLevel();
	}

	template <typename T>
	void writeVal(const T & val, const char * logMsg, BoolType<false> hasWrite)
	{
		writeLogMsg(val, logMsg);
		fwrite(&val, sizeof(val), 1, file_);
	}
	
	//-------------------------------------------------------------------------

	template <typename ParentType, typename Type>
	void writeParent(Type * type)
	{		
		unsigned char version = type->ParentType::version();
		writeLogMsg(version, "version");
		fwrite(&version, sizeof(version), 1, file_);
		type->ParentType::write(*this);		
	}
	
	//-------------------------------------------------------------------------
	void addIndentLevel() {indentLevel_++;}
	void removeIndentLevel() {indentLevel_--;}
private:
	template <typename T>
	void writeLogMsg_Empty(const T & val, const char * logMsg);

	template <typename T>
	void writeLogMsg(const T & val, const char * logMsg);

	template <typename T>
	void writeLogMsg(const boost::shared_ptr<T> & sharedPtr, const char * logMsg);

	template <typename T>
	void writeLogMsg(const T & val, const char * logMsg, const char * type);

	

	//-------------------------------------------------------------------------
	template <typename T>
	void writeLogValue(const T & val)
	{
		logFileStream_ << "<value>";
	}
	void writeLogValue(const float & val) {logFileStream_ << val;}
	void writeLogValue(const unsigned int & val) {logFileStream_ << val;}
	void writeLogValue(const std::string & val) {logFileStream_ << val;}
	void writeLogValue(const bool & val) {logFileStream_ << val;}
	void writeLogValue(const unsigned char & val) {logFileStream_ << val;}
	void writeLogValue(int & val) {logFileStream_ << val;}

private:
	FILE * file_;
	std::ofstream logFileStream_;
	const Scene * curScene_;
	bool loggingEnabled_;
	int indentLevel_;
	
};

///////////////////////////////////////////////////////////////////////////////


template <typename T>
void Writer::write(const std::vector<T> & objVector, const char * logMsg)
{
	writeLogMsg_Empty(objVector, logMsg);
	unsigned int numObjs = (unsigned int)objVector.size();
	write(numObjs, "count");

	std::stringstream ss;

	for (int i = 0; i < (int)objVector.size(); ++i)
	{
		ss.str("");	
		if (logMsg)	ss << logMsg << "[" << i << "]";
		else ss << "[" << i << "]";

		write(objVector[i], ss.str().c_str());		
	}
}


template <typename T>
void Writer::write(const std::set<T> & objSet, const char * logMsg)
{
	writeLogMsg_Empty(objSet, logMsg);
	unsigned int numObjs = (unsigned int)objSet.size();
	write(numObjs, "count");

	std::stringstream ss;

	typename std::set<T>::const_iterator iter = objSet.begin();

	int i = 0;
	for (;iter != objSet.end(); ++iter, ++i)
	{
		ss.str("");	
		if (logMsg)	ss << logMsg << "[" << i << "]";
		else ss << "[" << i << "]";

		write(*iter, ss.str().c_str());	
	}	
}


template <typename T, typename S>
void Writer::write(const std::map<T, S> & objMap, const char * logMsg)
{
	writeLogMsg_Empty(objMap, logMsg);
	unsigned int numObjs = (unsigned int)objMap.size();
	write(numObjs, "count");

	std::stringstream ss;

	typename std::map<T, S>::const_iterator iter = objMap.begin();

	int i = 0;
	for (;iter != objMap.end(); ++iter, ++i)
	{
		ss.str("");	
		if (logMsg)	ss << logMsg << "[" << i << "] key";
		else ss << "[" << i << "] value";

		write((*iter).first, ss.str().c_str());	

		ss.str("");	
		if (logMsg)	ss << logMsg << "[" << i << "] key";
		else ss << "[" << i << "] value";

		write((*iter).second, ss.str().c_str());
	}	
}


template <typename T>
void Writer::write(const boost::shared_ptr<T> & sharedPtr, const char * logMsg)
{		
	write(sharedPtr.get(), logMsg);
}

template <typename T>
void Writer::writeLogMsg_Empty(const T & val, const char * logMsg)
{	
#ifndef ANDROID
	if (!loggingEnabled_) return;	
	for (int i = 0; i < indentLevel_; ++i) logFileStream_ << " ";	
	if (logMsg) logFileStream_ << logMsg;
	logFileStream_ << "\t----\t";
	logFileStream_ << typeid(val).name() << "\n";
#endif
}

template <typename T>
void Writer::writeLogMsg(const T & val, const char * logMsg)
{	
#ifndef ANDROID
	writeLogMsg(val, logMsg, typeid(val).name());	
#endif
}

template <typename T>
void Writer::writeLogMsg(const boost::shared_ptr<T> & val, const char * logMsg)
{	
#ifndef ANDROID	
	std::stringstream ss;
	ss << "shared_ptr: " << typeid(*val.get()).name();
	writeLogMsg(val, logMsg, ss.str().c_str());	
#endif
}

template <typename T>
void Writer::writeLogMsg(const T & val, const char * logMsg, const char * type)
{
#ifndef ANDROID	
	if (!loggingEnabled_) return;	
	for (int i = 0; i < indentLevel_; ++i) logFileStream_ << " ";	
	if (logMsg) logFileStream_ << logMsg;
	logFileStream_ << "\t";
	writeLogValue(val);
	logFileStream_ << "\t";
	logFileStream_ << type << "\n";
#endif
}


#endif // Writer_h__