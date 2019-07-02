#include <QtGui>
#include <QSharedMemory>

#include "GL/glew.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <direct.h>

#include <algorithm>
#include <vector>
#include <stack>
#include <set>
#include <fstream>
#include <sstream>

#include <windows.h>
#ifdef _DEBUG
#include "vld.h"
#endif

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/type_traits.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include "Core.h"

