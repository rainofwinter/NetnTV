#ifndef Traits_h__
#define Traits_h__





class Writer;

///structure to test whether a given type has a void write(Writer &) const
///member function
template <typename T>
struct HasWriteMethod
{
	template<typename U, void (U::*)(Writer &) const> struct SFINAE {};
	template<typename U> static char test(SFINAE<U, &U::write> *);
	template<typename U> static int test(...);
	static const bool value = sizeof(test<T>(0)) == sizeof(char);
};


template <typename T>
struct HasVersionMethod
{
	template<typename U, unsigned char (U::*)() const> struct SFINAE {};
	template<typename U> static char test(SFINAE<U, &U::version> *);
	template<typename U> static int test(...);
	static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

template <typename T>
struct HasTypeMethod
{
	template<typename U, boost::uuids::uuid (U::*)() const> struct SFINAE {};
	template<typename U> static char test(SFINAE<U, &U::type> *);
	template<typename U> static int test(...);
	static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

class Reader;

///structure to test whether a given type has a void read(Reader &)
///member function
template <typename T>
struct HasReadMethod
{
	template<typename U, void (U::*)(Reader &, unsigned char)> struct SFINAE {};
	template<typename U> static char test(SFINAE<U, &U::read> *);
	template<typename U> static int test(...);
	static const bool value = sizeof(test<T>(0)) == sizeof(char);
};


#endif // Traits_h__