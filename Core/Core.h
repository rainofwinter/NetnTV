#pragma once

#include <algorithm>
#include <vector>
#include <set>
#include <stack>
#include <map>
#include <fstream>
#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/type_traits.hpp>

#include <jsapi.h>

#include "Global.h"
#include "Document.h"
#include "Scene.h"
#include "Reader.h"

#include "libxml.h"
#include "libxml/xmlwriter.h"