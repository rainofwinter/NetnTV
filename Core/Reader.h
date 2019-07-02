#ifndef Reader_h__
#define Reader_h__

#include "Traits.h"
#include "Types.h"
#include "Exception.h"

class DocumentTemplate;
class Scene;
class SceneObject;
class AppObject;
class Action;
class Event;
class AnimationChannel;
class Global;

class Reader
{
private:
	struct AddressData
	{
		void * addr;
		void * sharedPtrAddr;
	};
	std::map<unsigned int, AddressData> ptrData_;
	unsigned int curPtrIndex_;

	template <bool N> struct BoolType {};
	
public:
	Reader(const char * fileName);
	Reader(FILE * file);
	~Reader();


	//-------------------------------------------------------------------------

	template <typename T>
	void read(std::vector<T> & objVector);

	template <typename T>
	void read(std::set<T> & objSet);
	
	template <typename T, typename S>
	void read(std::map<T, S> & objMap);

	template <typename T>
	void read(boost::shared_ptr<T> & sharedPtr);

	void read(boost::uuids::uuid & val);
	void read(std::string & val);	
	void read(std::wstring & val);
	//-------------------------------------------------------------------------	
	template <typename T>
	void read(T & val)
	{
		read(val, BoolType<boost::is_pointer<T>::value>());
	}

	template <typename T>
	void read(T & val, BoolType<true> isPointer)
	{
		readPtr(val, 
			BoolType<HasTypeMethod<typename boost::remove_pointer<T>::type>::value>());
	
	}

	template <typename T>
	void read(T & val, BoolType<false> isPointer)
	{
		readVal(val);
	}
	//-------------------------------------------------------------------------

	
	unsigned int readPtr(SceneObject *& val, BoolType<true> hasTypeMethod);
	unsigned int readPtr(AppObject *& val, BoolType<true> hasTypeMethod);
	unsigned int readPtr(AnimationChannel *& val, BoolType<true> hasTypeMethod);
	unsigned int readPtr(Event *& val, BoolType<true> hasTypeMethod);
	unsigned int readPtr(Action *& val, BoolType<true> hasTypeMethod);
	unsigned int readPtr(DocumentTemplate *& val, BoolType<true> hasTypeMethod);
	
	template <typename T>
	friend unsigned int readTypedPtr(Reader & reader, T *&val, 
	T * (Global::*createFunc)(const boost::uuids::uuid &) const);

	template <typename T>
	unsigned int readPtr(T *& val, BoolType<true> hasTypeMethod)
	{
		return readPtr((typename T::base_type *&)val, hasTypeMethod);
	}


	template <typename T>
	unsigned int readPtr(T *& val, BoolType<false> hasTypeMethod)
	{
		unsigned int index = readPtrIndex();

		if (index == 0)
		{
			val = 0; return index;
		}

		std::map<unsigned int, AddressData>::iterator iter = ptrData_.find(index);
		AddressData ptrData = (*iter).second;

		if (ptrData.addr)
		{
			val = (T*)ptrData.addr;
		}
		else
		{
			val = new T;
			ptrData.addr = (void *)val;
			ptrData_[index] = ptrData;
			readVal(*val);			
		}

		return index;
	}

	unsigned int readPtrIndex();
	//-------------------------------------------------------------------------
	/**
	If val has a void T::read(Reader &) 
	member function, use that. Otherwise just do an fread.
	*/
	template <typename T>
	void readVal(T & val)
	{
		readVal(val, BoolType<HasReadMethod<T>::value>());
	}

	template <typename T>
	void readVal(T & val, BoolType<true> hasRead)
	{
		unsigned char version;
		readVal(version);
		if (version > val.version()) 
			throw Exception("Object version mismatch");
		val.read(*this, version);
	}

	void readVal(bool & val, BoolType<false> hasRead)
	{
		unsigned char byte;
		fread((void *)&byte, 1, 1, file_);
		val = (bool)byte;
	}

	template <typename T>
	void readVal(T & val, BoolType<false> hasRead)
	{
		fread((void *)&val, sizeof(val), 1, file_);
	}


	//-------------------------------------------------------------------------
	
	
	template <typename ParentType, typename Type>
	void readParent(Type * type)
	{
		
		unsigned char version;
		readVal(version);
		if (version > type->ParentType::version())
			throw Exception("Object version mismatch");
		type->ParentType::read(*this, version);
		
		//type->ParentType::read(*this, 0);
	}
private:
	FILE * file_;
	bool closeFile_;
};











template <typename T>
void Reader::read(std::vector<T> & objVector)
{
	unsigned int count;
	read(count);
	objVector.resize(count);
	for (unsigned int i = 0; i < count; ++i)
	{
		read(objVector[i]);
	}
}

template <typename T>
void Reader::read(std::set<T> & objSet)
{
	objSet.clear();
	unsigned int count;	
	read(count);
	for (unsigned int i = 0; i < count; ++i)
	{
		T elem;
		read(elem);
		objSet.insert(elem);
	}
}

template <typename T, typename S>
void Reader::read(std::map<T, S> & objMap)
{
	objMap.clear();
	unsigned int count;
	read(count);
	for (unsigned int i = 0; i < count; ++i)
	{
		T key;
		S value;
		read(key);
		read(value);
		objMap[key] = value;
	}
}


template <typename T>
void Reader::read(boost::shared_ptr<T> & sharedPtr)
{		
	T * ptr;
	unsigned int index = readPtr(ptr, BoolType<HasTypeMethod<T>::value>());		

	if (index == 0)
	{
		sharedPtr.reset();
		return;
	}

	std::map<unsigned int, AddressData>::iterator iter = ptrData_.find(index);
	AddressData ptrData = (*iter).second;

	if (ptrData.sharedPtrAddr)
	{		
		sharedPtr = *(boost::shared_ptr<T> *)ptrData.sharedPtrAddr;
	}
	else
	{
		sharedPtr.reset(ptr);
		ptrData.sharedPtrAddr = &sharedPtr;
		ptrData_[index] = ptrData;	
	}	
}




#endif // Reader_h__