
#if defined(MSVC)
#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)

#if defined(ES_EMULATOR)
#include "GLES2/gl2.h"
#else
#include "GL/glew.h"
#endif

#elif defined(OSX)

#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)


#elif defined(IOS)

//#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)


//#ifdef IOS_DEBUG
//    #ifdef __arm__
//        #error "DO NOT USE iOS Device Scheme in DEBUG mode!!!"
//    #endif
//#else   //RELEASE
//    #ifdef i386
//        #error "DO NOT USE Simulator Scheme in RELEASE mode!!!"
//    #endif
//#endif


#elif defined(ANDROID)

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/log.h>
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE  , "libplayercore", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , "libplayercore", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , "libplayercore", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , "libplayercore", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "libplayercore", __VA_ARGS__)

#endif

#define GL_COVERAGE_BUFFER_BIT_NV 0x8000

///////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <float.h>
#include <algorithm>
#include <vector>
#include <set>
#include <stack>
#include <map>
#include <fstream>
#include <sstream>
#include <bitset>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/uuid/uuid.hpp>
//#include <boost/uuid/uuid_generators.hpp>
//#include <boost/uuid/uuid_io.hpp>
#include <boost/type_traits.hpp>
#include <boost/scope_exit.hpp>
#include <boost/lexical_cast.hpp>
#include <jsapi.h>

#include "utf8.h"
#include "libxml.h"
#include "libxml/xmlwriter.h"

#ifndef NO_WIDE_CHARACTERS

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#else

#define wchar_t unsigned short
namespace std
{

typedef vector<wchar_t> wstring;

}

namespace boost {
namespace uuids {

// generate a uuid from a string
// lexical_cast works fine using uuid_io.hpp
// but this generator should accept more forms
// and be more efficient
// would like to accept the following forms:
// 0123456789abcdef0123456789abcdef
// 01234567-89ab-cdef-0123456789abcdef
// {01234567-89ab-cdef-0123456789abcdef}
// {0123456789abcdef0123456789abcdef}
// others?
struct string_generator {
    typedef uuid result_type;
    
    template <typename ch, typename char_traits, typename alloc>
    uuid operator()(std::basic_string<ch, char_traits, alloc> const& s) const {
        return operator()(s.begin(), s.end());
    };

    uuid operator()(char const*const s) const {
        return operator()(s, s+std::strlen(s));
    }

    template <typename CharIterator>
    uuid operator()(CharIterator begin, CharIterator end) const
    {
        typedef typename std::iterator_traits<CharIterator>::value_type char_type;

        // check open brace
        char_type c = get_next_char(begin, end);
        bool has_open_brace = is_open_brace(c);
        char_type open_brace_char = c;
        if (has_open_brace) {
            c = get_next_char(begin, end);
        }

        bool has_dashes = false;

        uuid u;
        int i=0;
        for (uuid::iterator it_byte=u.begin(); it_byte!=u.end(); ++it_byte, ++i) {
            if (it_byte != u.begin()) {
                c = get_next_char(begin, end);
            }
            
            if (i == 4) {
                has_dashes = is_dash(c);
                if (has_dashes) {
                    c = get_next_char(begin, end);
                }
            }
            
            if (has_dashes) {
                if (i == 6 || i == 8 || i == 10) {
                    if (is_dash(c)) {
                        c = get_next_char(begin, end);
                    } else {
                        throw_invalid();
                    }
                }
            }

            *it_byte = get_value(c);

            c = get_next_char(begin, end);
            *it_byte <<= 4;
            *it_byte |= get_value(c);
        }

        // check close brace
        if (has_open_brace) {
            c = get_next_char(begin, end);
            check_close_brace(c, open_brace_char);
        }
        
        return u;
    }
    
private:
    template <typename CharIterator>
    typename std::iterator_traits<CharIterator>::value_type
    get_next_char(CharIterator& begin, CharIterator end) const {
        if (begin == end) {
            throw_invalid();
        }
        return *begin++;
    }

    unsigned char get_value(char c) const {
        static char const*const digits_begin = "0123456789abcdefABCDEF";
        static char const*const digits_end = digits_begin + 22;

        static unsigned char const values[] =
            { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,10,11,12,13,14,15
            , static_cast<unsigned char>(-1) };

        char const* d = std::find(digits_begin, digits_end, c);
        return values[d - digits_begin];
    }

    bool is_dash(char c) const {
        return c == '-';
    }
    
    // return closing brace
    bool is_open_brace(char c) const {
        return (c == '{');
    }
    
    void check_close_brace(char c, char open_brace) const {
        if (open_brace == '{' && c == '}') {
            //great
        } else {
            throw_invalid();
        }
    }
    
    void throw_invalid() const {

    }
};

}} // namespace boost::uuids

#endif
