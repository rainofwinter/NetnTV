#pragma once
#include "Error.h"


class Exception : public std::exception
{
public:
	Exception(ErrorCode err, const char * whatStr = "")
	{
		err_ = err;
		what_ = strdup(whatStr);
	}

	Exception(const char * whatStr = "")
	{
		err_ = GenericError;
		what_ = strdup(whatStr);		
	}

	Exception(const Exception & rhs)
	{
		*this = rhs;
	}

	const Exception & operator = (const Exception & rhs)
	{
		if (this == &rhs) return *this;
		err_ = rhs.err_;
		what_ = strdup(rhs.what_);
		return *this;
	}

	~Exception() throw()
	{
		free(what_);
	}

	virtual const char * what() const throw() {return what_;}
private:
	ErrorCode err_;
	char * what_;
};



