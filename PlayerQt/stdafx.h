#include <QtGui>
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <GdiPlus.h>

#ifdef _DEBUG
#include "vld.h"
#endif

#if defined(ES_EMULATOR)
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglplatform.h"

#pragma comment(lib, "libGLESv2.lib")
#pragma comment(lib, "libEGL.lib")



#else
#include "GL/glew.h"

#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#endif

#include <vector>
#include <set>
#include <stack>
#include <map>

#include <boost/cstdint.hpp>
#include <boost/thread.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <dsound.h>

#include "Core.h"