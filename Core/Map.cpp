#include "stdafx.h"
#include "Map.h"
#include "Mesh.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "FileUtils.h"
#include "Global.h"
#include "Model.h"
#include "ModelAnimation.h"
#include "Scene.h"
#include "ScriptProcessor.h"
#include "SpatialPartitioning.h"
#include "MeshPart.h"
#include "BoundingBoxObject.h"
#include "Camera.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

void Path_finalize(JSContext * cx, JSObject * obj)
{		
	Path * privateData = (Path *)JS_GetPrivate(cx, obj);
	delete privateData;
}

JSClass jsPathClass = InitClass(
	"Path", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, Path_finalize, 0);


//-----------------------------------------------------------------------------

JSBool Path_constructor(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	Path * path = new Path;

	JSObject * newObject = JS_NewObject(cx, &jsPathClass, s->pathProto(), 0);	
	JS_SetPrivate(cx, newObject, path);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(newObject));
	return JS_TRUE;
};

JSBool Path_getPathPts(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Path * path = (Path *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	
	JSObject * jsChildren = JS_NewArrayObject(cx, (jsint)path->pathPts().size(), 0);

	for (int i = 0; i < (int)path->pathPts().size(); ++i)
	{
		jsval val;
		val = OBJECT_TO_JSVAL(path->pathPts()[i].createScriptObject(s));
		JS_SetElement(cx, jsChildren, (jsint)i, &val);
	}

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsChildren));
	return JS_TRUE;
};

JSBool Path_delete(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Path * path = (Path *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	path->clear();
	return JS_TRUE;
}

JSFunctionSpec PathFuncs[] = {
	JS_FS("getPathPts", Path_getPathPts, 0, 0),
	JS_FS("delete", Path_delete, 0, 0),
	JS_FS_END
};


JSPropertySpec PathProps[] = {
	/*
	{"x",	x, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Vector3_getProperty, Vector3_setProperty},

	{"y",	y, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Vector3_getProperty, Vector3_setProperty},

	{"z",	z, JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED, 
	Vector3_getProperty, Vector3_setProperty},	
	*/
	{0}
};


JSObject * Path::scriptObjectProto(ScriptProcessor * s, JSObject * global)
{
	JSObject * jsPathProto = JS_InitClass(
		s->jsContext(), global, 0, &jsPathClass, Path_constructor, 0, 
		PathProps, PathFuncs, 
		0, 0);
	s->rootJSObject(jsPathProto);
	return jsPathProto;	
}

void Path::clear()
{
	vector<int>().swap(pathIndices_);
	vector<Vector3>().swap(pathPts_);
}

///////////////////////////////////////////////////////////////////////////////

JSClass jsMapClass = InitClass(
	"Map", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);


JSBool Map_intersectObject(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Map * map = (Map *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	


	if (!isSceneObject(s, jsObj))
	{
		JS_ReportError(cx, "Map::intersectObject - parameter must be a SceneObject");
		return JS_FALSE;
	}

	if (!map->isLoaded())
	{
		JS_ReportError(cx, "Map::intersectObject - map not yet loaded");
		return JS_FALSE;
	}

	SceneObject * obj = (SceneObject *)JS_GetPrivate(cx, jsObj);

	boost::uuids::uuid type = obj->type();
	if (type != BoundingBoxObject().type())
	{
		JS_ReportError(cx, "Map::intersectObject - parameter must be a BoundingBoxObject");
		return JS_FALSE;
	}

	BoundingBoxObject * bbo = (BoundingBoxObject *)obj;
	set<IntTriData> normals;
	bool ret = map->intersect(&normals, bbo);

	JSObject * retObj = JS_NewObject(cx, 0, 0, 0);

	jsval val = BOOLEAN_TO_JSVAL(ret);
	JS_SetProperty(cx, retObj, "intersect", &val);


	JSObject * jsNormals = JS_NewArrayObject(cx, (jsint)normals.size(), 0);
	set<IntTriData>::iterator iter;
	int i = 0;
	for (iter = normals.begin(); iter != normals.end(); ++iter, ++i)
	{
		jsval val;
		val = OBJECT_TO_JSVAL((*iter).normal.createScriptObject(s));
		JS_SetElement(cx, jsNormals, (jsint)i, &val);
	}

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsNormals));	

	val = OBJECT_TO_JSVAL(jsNormals);
	JS_SetProperty(cx, retObj, "intersectNormals", &val);


	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retObj));
	return JS_TRUE;
};

JSBool Map_intersectSegment(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Map * map = (Map *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsObj1, * jsObj2;
	if (!JS_ConvertArguments(cx, argc, argv, "oo", &jsObj1, &jsObj2)) return JS_FALSE;	

	if (!map->isLoaded())
	{
		JS_ReportError(cx, "Map::intersectObject - map not yet loaded");
		return JS_FALSE;
	}

	Vector3 a = Vector3::fromScriptObject(s, jsObj1);
	Vector3 b = Vector3::fromScriptObject(s, jsObj2);

	Vector3 intPt, normal;
	bool ret = map->intersect(&intPt, &normal, a, b);

	JSObject * retObj = JS_NewObject(cx, 0, 0, 0);

	jsval val = BOOLEAN_TO_JSVAL(ret);
	JS_SetProperty(cx, retObj, "intersect", &val);

	val = OBJECT_TO_JSVAL(intPt.createScriptObject(s));
	JS_SetProperty(cx, retObj, "intersectPoint", &val);

	val = OBJECT_TO_JSVAL(normal.createScriptObject(s));
	JS_SetProperty(cx, retObj, "intersectNormal", &val);


	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retObj));
	return JS_TRUE;
};

JSBool Map_pathPtAlongSegment(JSContext * cx, uintN argc, jsval * vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Map * map = (Map *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsObj1, * jsObj2;
	if (!JS_ConvertArguments(cx, argc, argv, "oo", &jsObj1, &jsObj2)) return JS_FALSE;	

	if (!map->isLoaded())
	{
		JS_ReportError(cx, "Map::pathPtAlongSegment - map not yet loaded");
		return JS_FALSE;
	}

	Vector3 a = Vector3::fromScriptObject(s, jsObj1);
	Vector3 b = Vector3::fromScriptObject(s, jsObj2);

	//a = Vector3(1248.195f, 0.0f, -372.783f);
	//b = Vector3(930.228f, 0.0f, -339.579f);
	//b = Vector3(930.228, 0.0f, -532.492);
	//b = Vector3(1845.222f, 0.0f, -65.485);

	Vector3 pOut;
	int ret = map->pathPtAlongSegment(&pOut, a, b);

	if (ret >= 0)	
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(pOut.createScriptObject(s)));	
	else	
		JS_SET_RVAL(cx, vp, JSVAL_NULL);
	
	return JS_TRUE;
}

JSBool Map_randomPt(JSContext * cx, uintN argc, jsval * vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Map * map = (Map *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble margin;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &margin)) return JS_FALSE;	

	if (!map->isLoaded())
	{
		JS_ReportError(cx, "Map::randomPt - map not yet loaded");
		return JS_FALSE;
	}

	Vector3 pt;
	bool ret = map->randomPt(&pt, (float)margin);

	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	if (ret)
	{
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(pt.createScriptObject(s)));
	}
	
	return JS_TRUE;
}

JSBool Map_pathFind(JSContext * cx, uintN argc, jsval * vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	Map * map = (Map *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsObjPath;
	JSObject * jsObj1, * jsObj2;
	jsdouble margin;
	if (!JS_ConvertArguments(cx, argc, argv, "oood", &jsObjPath, &jsObj1, &jsObj2, &margin)) return JS_FALSE;	

	if (!map->isLoaded())
	{
		JS_ReportError(cx, "Map::pathFind - map not yet loaded");
		return JS_FALSE;
	}
		
	if (!jsObjPath || JS_GetClass(jsObjPath) != &jsPathClass)
	{
		JS_ReportError(cx, "Map::pathFind - parameter 1 must be a Path object");
		return JS_FALSE;	
	}

	Path * path = (Path *)JS_GetPrivate(cx, jsObjPath);

	Vector3 startPt = Vector3::fromScriptObject(s, jsObj1);
	Vector3 endPt = Vector3::fromScriptObject(s, jsObj2);


	//startPt = Vector3(8168.949f, 0.001f, -5407.232f);
	//startPt = Vector3(4122.602f, 0.001f, -4628.37f);
	//startPt = Vector3(4750.751f, 0.001f, -7152.595f);
	//startPt = Vector3(6066.182f, 0.001f, -7882.804f);
	//startPt = Vector3(6720.233f, 0.001f, -8541.182f);
	//startPt = Vector3(7164.902f, 0.001f, -8541.182f);
	//endPt = Vector3(7326.078f, 0.001f, -9212.106f);
	//endPt = Vector3(6613.948f, 0.001f, -9209.658f);	
	
	bool ret = map->pathFindPts(path, startPt, endPt, (float)margin);	
	
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ret));
	
	return JS_TRUE;
}


JSFunctionSpec MapFuncs[] = {
	JS_FS("intersectObject", Map_intersectObject, 1, 0),
	JS_FS("intersectSegment", Map_intersectSegment, 1, 0),
	JS_FS("pathPtAlongSegment", Map_pathPtAlongSegment, 2, 0),
	JS_FS("pathFind", Map_pathFind, 4, 0),
	JS_FS("randomPt", Map_randomPt, 1, 0),	
	JS_FS_END
};



///////////////////////////////////////////////////////////////////////////////
JSObject * Map::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsMapClass,
		0, 0, 0, MapFuncs, 0, 0);

	return proto;
}

JSObject * Map::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsMapClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void Map::create()
{
	model_ = 0;
	loadRequested_ = false;

	pathMeshLoadReq_ = false;
	pathMesh_ = NULL;

	collisionTree_ = NULL;
#ifdef CULLING
	renderTree_ = NULL;
	indexBuffer_ = 0;
#endif
	
}

Map::Map() : visualAttrib_(this)
{
	create();
	setId("Map");
}

Map::Map(const Map & rhs, ElementMapping * elementMapping)
	: SceneObject(rhs, elementMapping), visualAttrib_(this)
{
	create();
	if (rhs.model_) model_ = new Model(*rhs.model_);
	else model_ = 0;

	visualAttrib_ = rhs.visualAttrib_;
	visualAttrib_.setModelObject(this);
}


Map::~Map()
{
	delete collisionTree_;
#ifdef CULLING
	delete renderTree_;
#endif
	delete model_;	
}

const Matrix & Map::inverseTotalTransform() const
{
	return visualAttrib_.inverseTotalTransform();
}

void Map::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);
	refFiles->push_back(fileName_);
	refFiles->push_back(pathMeshFileName_);
}

int Map::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);	
	fileName_ = refFiles[index++];		
	pathMeshFileName_ = refFiles[index++];
	return index;
}


void Map::setFileName(const std::string & fileName)
{
	fileName_ = fileName;	
}

void Map::setPathMeshFileName(const std::string & fileName)
{
	pathMeshFileName_ = fileName;
}

void Map::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
	Global & g = Global::instance();	
	loadRequested_ = true;
	g.requestObjectLoad(this, fileName_, LoadRequest::LoadRequestModel);
	pathMeshLoadReq_ = true;
	g.requestObjectLoad(this, pathMeshFileName_, LoadRequest::LoadRequestModel);
}
#ifdef CULLING
void Map::initRenderGeometry()
{
	const vector<Mesh *> & meshes = model_->meshes();
	int numMeshes = (int)model_->meshes().size();
	vertexBuffers_ = new GLuint[numMeshes];	

	for (int i = 0; i < numMeshes; ++i)
	{
		glGenBuffers( 1, &vertexBuffers_[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[i]);

		const std::vector<VertexData> & verts = meshes[i]->vertices();

		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * verts.size(), 
			&verts[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glGenBuffers(1, &indexBuffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);	

	indexBufferSize_ = 0;
	maxTrisAtATime_ = 0;
	for (int i = 0; i < meshes.size(); ++i)
	{
		
		const vector<MeshPart *> & meshParts = meshes[i]->meshParts();
		for (int j = 0; j < meshParts.size(); ++j)
		{
			maxTrisAtATime_ = max(maxTrisAtATime_, meshParts[j]->numTriangles());
			indexBufferSize_ = max(indexBufferSize_, 2*meshParts[j]->numTriangles());			
		}
	}
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*indexBufferSize_*3, 0, GL_DYNAMIC_DRAW);

	drawMeshData_.resize(numMeshes);
	for (int i = 0; i < numMeshes; ++i)
	{
		const std::vector<MeshPart *> & meshParts = model_->meshes()[i]->meshParts();
		drawMeshData_[i].meshPartData.resize(meshParts.size());		
	}
	
	const vector<KDTreeNode> & nodes = renderTree_->nodes();
	nodeData_.resize(nodes.size());

	vector<unsigned short> indices;
	for (int i = 0; i < nodes.size(); ++i)
	{
		
		nodeData_[i].triListData.resize(nodes[i].numTriLists);		

		for (int j = 0; j < nodes[i].numTriLists; ++j)
		{
			vector<unsigned short> & indices = nodeData_[i].triListData[j].indices;
			indices.clear();
			const KDTreeTriList & triList = nodes[i].triLists[j];

			int meshIndex = triList.meshIndex;
			int meshPartIndex = triList.meshPartIndex;

			const vector<unsigned short> & meshPartIndices = 
				meshes[meshIndex]->meshParts()[meshPartIndex]->indices();

			for (int k = 0; k < triList.numTris; ++k)
			{
				int index = 3*triList.triIndices[k];
				indices.push_back(meshPartIndices[index++]);
				indices.push_back(meshPartIndices[index++]);
				indices.push_back(meshPartIndices[index++]);
			}
		}
	}
	
}

void Map::uninitRenderGeometry()
{
	int numMeshes = (int)model_->meshes().size();
	for (int i = 0; i < numMeshes; ++i)
	{
		glDeleteBuffers(1, &vertexBuffers_[i]);
	}

	delete [] vertexBuffers_;
	vertexBuffers_ = NULL;

	const vector<KDTreeNode> & nodes = renderTree_->nodes();


	glDeleteBuffers(1, &indexBuffer_);
	indexBuffer_ = 0;

	vector<MeshData>().swap(drawMeshData_);
	vector<NodeData>().swap(nodeData_);

}
#endif

void Map::uninit()
{
	if (loadRequested_)
	{		
		loadRequested_ = false;
		Global::instance().cancelObjectLoad(this, fileName_);		
	}

	if (pathMeshLoadReq_)
	{
		pathMeshLoadReq_ = false;
		Global::instance().cancelObjectLoad(this, pathMeshFileName_);
	}

	if (model_) 
	{		
#ifdef CULLING
		uninitRenderGeometry();
		delete renderTree_;
		renderTree_ = NULL;
#endif
		BOOST_FOREACH(Texture * texture, model_->textures()) texture->uninit();
		delete model_;	
		model_ = NULL;
		delete collisionTree_;
		collisionTree_ = NULL;
	}

	delete pathMesh_;
	pathMesh_ = NULL;

	SceneObject::uninit();
}

bool Map::update(float sceneTime)
{	
	bool needRedraw = SceneObject::update(sceneTime);
	return needRedraw;
}

bool Map::asyncLoadUpdate()
{
	bool everythingLoaded = true;
	if (model_ == 0 && !fileName_.empty())
	{
		bool loaded = Global::instance().getLoadedModel(this, fileName_, &model_);

		if (loaded)
		{
			//TODO: move tree construction to another thread
			collisionTree_ = new KDTree(48);
			collisionTree_->genFromModel(this, model_);
#ifdef CULLING
			renderTree_ = new KDTree(256);
			renderTree_->genFromModel(this, model_);

			aabb_ = model_->extents();
			
			BOOST_FOREACH(Texture * texture, model_->textures()) texture->init(gl_);

			//generate gl vertex/index buffers for render tree
			initRenderGeometry();
#else
			model_->init(gl_);
#endif
			loadRequested_ = false;	
		}		
	}

	if (model_) 
	{
		BOOST_FOREACH(Texture * texture, model_->textures())
			everythingLoaded &= texture->asyncLoadUpdate();
	}
	else everythingLoaded &= fileName_.empty();
	
	
	if (!pathMesh_ && !pathMeshFileName_.empty())
	{
		Model * pathModel;
		bool loaded = Global::instance().getLoadedModel(this, pathMeshFileName_, &pathModel);

		if (loaded)
		{
			pathMesh_ = new PathMesh;
			pathMesh_->build(this, pathModel);
					
			pathMeshLoadReq_ = false;
		}		
	}

	if (!pathMesh_) everythingLoaded &= pathMeshFileName_.empty();
		
	handleLoadedEventDispatch(everythingLoaded);
	return everythingLoaded;	
}

bool Map::isLoaded() const
{
	if (model_) return model_->isLoaded();
	else return false;	
}



void Map::drawObject(GfxRenderer * gl) const
{
	if (!model_) return;

	glEnable(GL_CULL_FACE);		
	glCullFace(GL_BACK);

#ifdef CULLING	
	gl->useMeshProgram();
	gl->setMeshProgramOpacity(1);

	int prevDepthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);
	if (prevDepthFunc == GL_ALWAYS) gl->clearZBuffer();
	glDepthFunc(GL_LEQUAL);
	
	gl->enableVertexAttribArrayPosition();
	gl->enableVertexAttribArrayTexCoord();	
	glEnableVertexAttribArray(AttribNormal);
	glEnableVertexAttribArray(AttribTangent);
	glEnableVertexAttribArray(AttribBinormalFactor);

	//calculate cam frustrum
	Matrix inverseCamMatrix = gl->cameraMatrix().inverse();
	
	Vector3 a, b;
	Vector3 fMin[4], fMax[4];

	Camera::unproject(inverseCamMatrix, Vector2(-1.0f, 1.0f), &fMin[0], &fMax[0]);
	Camera::unproject(inverseCamMatrix, Vector2(1.0f, 1.0f), &fMin[1], &fMax[1]);
	Camera::unproject(inverseCamMatrix, Vector2(1.0f, -1.0f), &fMin[2], &fMax[2]);
	Camera::unproject(inverseCamMatrix, Vector2(-1.0f, -1.0f), &fMin[3], &fMax[3]);

	for (int i = 0; i < 4; ++i)
	{
		fMin[i] = visualAttrib_.inverseTotalTransform() * fMin[i];
		fMax[i] = visualAttrib_.inverseTotalTransform() * fMax[i];
	}

	drawCamEye_ = 0.5f * (0.5f * (fMin[0] + fMin[1]) + 0.5f * (fMin[2] + fMin[3]));

	drawFrustrumPlanes_[0] = Plane::fromPoints(fMin[0], fMin[1], fMax[0]);
	drawFrustrumPlanes_[1] = Plane::fromPoints(fMin[1], fMin[2], fMax[1]);
	drawFrustrumPlanes_[2] = Plane::fromPoints(fMin[2], fMin[3], fMax[2]);
	drawFrustrumPlanes_[3] = Plane::fromPoints(fMin[3], fMin[0], fMax[3]);
	drawFrustrumPlanes_[4] = Plane::fromPoints(fMin[0], fMin[2], fMin[1]);
	drawFrustrumPlanes_[5] = Plane::fromPoints(fMax[0], fMax[1], fMax[2]);
	//end calculate cam frustrum

	for (int i = 0; i < (int)drawMeshData_.size(); ++i)
	{
		for (int j = 0; j < (int)drawMeshData_[i].meshPartData.size(); ++j)
		{
			drawMeshData_[i].meshPartData[j].indices.clear();			
		}
	}

	doNode(gl, 0, aabb_);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);	

	for (int i = 0; i < (int)drawMeshData_.size(); ++i)
	{
		gl->bindArrayBuffer(vertexBuffers_[i]);
		gl->vertexAttribPositionPointer(sizeof(VertexData), 0);
		gl->vertexAttribTexCoordPointer(sizeof(VertexData), (char*)0 + sizeof(float)*3);

		glVertexAttribPointer(AttribNormal, 3, GL_FLOAT, GL_FALSE, 
			sizeof(VertexData), (char*)0 + sizeof(float)*5);

		glVertexAttribPointer(AttribTangent, 3, GL_FLOAT, GL_FALSE, 
			sizeof(VertexData), (char*)0 + sizeof(float)*8);

		glVertexAttribPointer(AttribBinormalFactor, 1, GL_FLOAT, GL_FALSE, 
			sizeof(VertexData), (char*)0 + sizeof(float)*11);

		for (int j = 0; j < (int)drawMeshData_[i].meshPartData.size(); ++j)
		{
			const vector<unsigned short> & indices = drawMeshData_[i].meshPartData[j].indices;
			if (indices.empty()) continue;
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort)*indices.size(), &indices[0]);
			Material * mat = model_->meshes()[i]->meshParts()[j]->material();		
			mat->Use(gl);
			gl->applyCurrentShaderMatrix();
				
			glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
		}
	}
	
	gl->disableVertexAttribArrayPosition();
	gl->disableVertexAttribArrayTexCoord();
	glDisableVertexAttribArray(AttribNormal);
	glDisableVertexAttribArray(AttribTangent);
	glDisableVertexAttribArray(AttribBinormalFactor);
	
	glDepthFunc(prevDepthFunc);
#else
	model_->drawObject(gl);
#endif

	glDisable(GL_CULL_FACE);
}

#ifdef CULLING

bool Map::isAabbOutsideFrustrum(const BoundingBox & aabb) const
{	
	for (int i = 0; i < 6; ++i)
	{
		Vector3 v;

		if (drawFrustrumPlanes_[i].a > 0.0f)
		{
			v.x = aabb.minPt.x;
		}
		else
		{
			v.x = aabb.maxPt.x;
		}

		if (drawFrustrumPlanes_[i].b > 0.0f)
		{
			v.y = aabb.minPt.y;
		}
		else
		{
			v.y = aabb.maxPt.y;
		}

		if (drawFrustrumPlanes_[i].c > 0.0f)
		{
			v.z = aabb.minPt.z;
		}
		else
		{
			v.z = aabb.maxPt.z;
		}

		if (drawFrustrumPlanes_[i].dotCoord(v) > 0.0f) return true;
	}

	return false;
}

void Map::doNode(GfxRenderer * gl, int nodeIndex, const BoundingBox & aabb) const
{
	const std::vector<KDTreeNode> & nodes = renderTree_->nodes();
	int node1 = 0, node2 = 0;
	BoundingBox * node1Aabb, * node2Aabb;

	bool isLeaf = nodes[nodeIndex].triLists;

	if (!isLeaf)
	{
		//determine which node is closest, TODO: determine frustrum intersection
		BoundingBox leftAabb, rightAabb;
		renderTree_->childNodeAabb(aabb, nodeIndex, &leftAabb, &rightAabb);

		float leftDist = (drawCamEye_ - leftAabb.midPt()).magnitude();
		float rightDist = (drawCamEye_ - rightAabb.midPt()).magnitude();

		if (leftDist < rightDist)
		{
			node1 = nodes[nodeIndex].leftNode;
			node2 = nodes[nodeIndex].rightNode;
			node1Aabb = &leftAabb;
			node2Aabb = &rightAabb;
		}
		else
		{
			node1 = nodes[nodeIndex].rightNode;
			node2 = nodes[nodeIndex].leftNode;
			node1Aabb = &rightAabb;
			node2Aabb = &leftAabb;
		}		

		if (node1)
		{
			bool intersectsFrustrum = !isAabbOutsideFrustrum(*node1Aabb);
			if (intersectsFrustrum) doNode(gl, node1, *node1Aabb);

		}

		if (node2)
		{
			bool intersectsFrustrum = !isAabbOutsideFrustrum(*node2Aabb);
			if (intersectsFrustrum) doNode(gl, node2, *node2Aabb);
		}
	}
	else
	{
		const KDTreeNode & node = nodes[nodeIndex];
		for (int i = 0; i < node.numTriLists; ++i)
		{
			int meshIndex = node.triLists[i].meshIndex;
			int meshPartIndex = node.triLists[i].meshPartIndex;
			MeshPart * meshPart = model_->meshes()[meshIndex]->meshParts()[meshPartIndex];

			const vector<unsigned short> & nodeIndices = nodeData_[nodeIndex].triListData[i].indices;

			vector<unsigned short> & indices = drawMeshData_[meshIndex].meshPartData[meshPartIndex].indices;
			
			int curNumIndices = indices.size();
			int numNewIndices = nodeIndices.size();
			numNewIndices = min((int)nodeIndices.size(), maxTrisAtATime_ * 3 - curNumIndices);

			indices.insert(indices.end(), nodeIndices.begin(), nodeIndices.begin() + numNewIndices);		
		}
	}	
}
#endif

BoundingBox Map::extents() const
{
	if (model_) return model_->extents();
	return BoundingBox();
}

void Map::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	writer.write(fileName_, "fileName");	
	writer.write(pathMeshFileName_, "pathFileName");
}

void Map::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	reader.read(fileName_);
	if (version >= 1)
	{
		reader.read(pathMeshFileName_);
	}
}

void Map::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();
	
	w.writeTag("FileName", fileName_);
	w.writeTag("PathMeshFileName", pathMeshFileName_);
}

void Map::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);

	unsigned int val;

	for (xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
		}
		else if (r.getNodeContentIfName(fileName_, curNode, "FileName"));
		else if (r.getNodeContentIfName(pathMeshFileName_, curNode, "PathMeshFileName"));
	}
}


SceneObject * Map::intersect(Vector3 * out, const Ray & ray)
{
	Matrix matrix = parentTransform() * visualAttrib_.transformMatrix();
	Matrix invMatrix = matrix.inverse();
	Ray invRay;
	Vector3 b = invMatrix * (ray.origin + ray.dir);
	invRay.origin = invMatrix * ray.origin;
	invRay.dir = b - invRay.origin;

	if (model_)
	{
		if (model_->intersect(out, invRay))
		{
			*out = matrix * (*out);
			return this;		
		}
		else return 0;

	}
	return 0;
}

bool Map::pathFindPts(Path * path, const Vector3 & pStartPt, const Vector3 & pEndPt, float margin) const
{
	if (!pathMesh_) return false;
	return pathMesh_->pathFindPts(path, pStartPt, pEndPt, margin);
}

bool Map::randomPt(Vector3 * pt, float margin) const
{
	if (!pathMesh_) return false;
	int nodeIndex;
	return pathMesh_->randomPt(&nodeIndex, pt, margin);
	return true;
}

int Map::pathPtAlongSegment(Vector3 * pOut, 
	const Vector3 & startPt, const Vector3 & endPt) const
{
	if (!pathMesh_) return -1;
	return pathMesh_->pathPtAlongSegment(pOut, startPt, endPt);
}

bool Map::intersect(set<IntTriData> * intTriData, const BoundingBoxObject * bbObject) const
{
	return collisionTree_->intersect(intTriData, bbObject);
}

bool Map::intersect(Vector3 * intPt, Vector3 * normal, const Vector3 & pt0, const Vector3 & pt1) const
{
	return collisionTree_->intersect(intPt, normal, pt0, pt1);
}

///////////////////////////////////////////////////////////////////////////////

bool PathMeshNode::isAdjacentTo(const PathMeshNode & rhs) const
{
	int numSame = 0;
	for (int i = 0; i < 3; ++i)
	{
		unsigned short lIndex = p[i];
		for (int j = 0; j < 3; ++j)
		{
			unsigned short rIndex = rhs.p[j];
			if (lIndex == rIndex) ++numSame;
		}
	}

	return numSame == 2;
}

///////////////////////////////////////////////////////////////////////////////
PathMesh::PathMesh()
{
	kdTree_ = NULL;
	pathModel_ = NULL;
	totalArea_ = 0.0f;
}

PathMesh::~PathMesh()
{
	delete kdTree_;
	delete pathModel_;
}

struct Vector3Comp
{
	bool operator ()(const Vector3 & lhs, const Vector3 & rhs) const 
	{
		if (lhs.x != rhs.x) return lhs.x < rhs.x;
		if (lhs.y != rhs.y) return lhs.y < rhs.y;
		return lhs.z < rhs.z;		
	}
};

void PathMesh::build(Map * mapObj, Model * pathModel)
{
	triMap_.clear();
	delete kdTree_;
	map_ = mapObj;
	kdTree_ = new KDTree(48);
	kdTree_->genFromModel(mapObj, pathModel);
	Mesh * mesh = pathModel->meshes()[0];
	

	pathModel_ = pathModel;
	
	vector<PathMeshNode>().swap(nodes_);
	vector<Vector3>().swap(verts_);
	MeshPart * meshPart = mesh->meshParts()[0];

	const vector<VertexData> & vertexData = mesh->vertices();
	const vector<unsigned short> & indices = meshPart->indices();

	typedef map<Vector3, unsigned short, Vector3Comp> VertMap;
	VertMap vertMap;

	Matrix totalTrans = mapObj->parentTransform() * mapObj->visualAttrib()->transformMatrix();

	BoundingBox bBox = mapObj->worldExtents();
	float EPSILON = bBox.minPt.distanceFrom(bBox.maxPt)/1000;

	float cumArea = 0.0f; 
		
	for (int i = 0, triIndex = 0; i < (int)indices.size();)
	{
		Vector3 p[3];
		p[0] = vertexData[indices[i++]].position;
		p[1] = vertexData[indices[i++]].position;
		p[2] = vertexData[indices[i++]].position;

		PathMeshNode node;

		for (int k = 0; k < 3; ++k)
		{			
			unsigned short index;
			VertMap::iterator iter = vertMap.find(p[k]);
			if (iter != vertMap.end())
				index = (*iter).second;
			else
			{
				index = (unsigned short)vertMap.size();
				vertMap[p[k]] = index;
			}
			node.p[k] = index;
		}

		cumArea += triangleArea(p[0], p[1], p[2]);
		node.cumArea = cumArea;

		IntTriData0 data;
		data.meshIndex = 0;
		data.meshPartIndex = 0;
		data.triIndex = triIndex++;
		triMap_[data] = (int)nodes_.size();
		nodes_.push_back(node);
		
	}

	totalArea_ = cumArea;

	VertMap::iterator iter;
	verts_.resize(vertMap.size());
	for (iter = vertMap.begin(); iter != vertMap.end(); ++iter)
	{
		unsigned short index = (*iter).second;
		Vector3 vert = (*iter).first;
		verts_[index] = totalTrans * vert;
	}

	//determine node adjacencies

	set<IntTriData0> nearByTris;
	for (int i = 0; i < (int)nodes_.size(); ++i)
	{
		nearByTris.clear();
		BoundingBox aabb;
		if (i == 875)
		{
			int a = 4;
		}
		Vector3 minPt(FLT_MAX, FLT_MAX, FLT_MAX), maxPt(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (int k = 0; k < 3; ++k)
		{
			float x = verts_[nodes_[i].p[k]].x;
			float y = verts_[nodes_[i].p[k]].y;
			float z = verts_[nodes_[i].p[k]].z;

			if (x < minPt.x) minPt.x = x;
			if (y < minPt.y) minPt.y = y;
			if (z < minPt.z) minPt.z = z;

			if (x > maxPt.x) maxPt.x = x;
			if (y > maxPt.y) maxPt.y = y;
			if (z > maxPt.z) maxPt.z = z;			
		}

		minPt.x -= EPSILON;
		minPt.y -= EPSILON;
		minPt.z -= EPSILON;

		maxPt.x += EPSILON;
		maxPt.y += EPSILON;
		maxPt.z += EPSILON;

		aabb.minPt = minPt;
		aabb.maxPt = maxPt;

		kdTree_->getIntersectingTriangles(&nearByTris, aabb.toObb(Matrix::Identity()));

		BOOST_FOREACH(const IntTriData0 & data, nearByTris)
		{
			int adjNodeIndex = triMap_[data];
			if (nodes_[adjNodeIndex].isAdjacentTo(nodes_[i]))
				nodes_[i].adjNodes.push_back(adjNodeIndex);
		}
	}
}

bool PathMesh::pathFindPts(Path * path, 
	const Vector3 & pStartPt, const Vector3 & pEndPt, float margin) const
{
	if (margin < 0.0f) margin = 0.0f;
	Vector3 startPt(pStartPt);
	Vector3 endPt(pEndPt);
	int startNodeIndex = ptToNode(&startPt);
	int endNodeIndex = ptToNode(&endPt);

	if (startNodeIndex < 0 || endNodeIndex < 0) return false;

	vector<int> pathNodeIndices;
	bool ret = pathFind(&pathNodeIndices, startNodeIndex, endNodeIndex);
	indexToPtPath(path, pathNodeIndices, startPt, endPt, margin);

	return ret;
}

void PathMesh::getAdjEdge(int * e0, int * e1, int node1, int node2) const
{
	const PathMeshNode & node = nodes_[node1];
	const PathMeshNode & nextNode = nodes_[node2];

	for (int i = 0; i < 3; ++i)
	{
		int nextI = i + 1;
		if (nextI >= 3) nextI = 0;
		int edge00 = node.p[i];
		int edge01 = node.p[nextI];
		for (int j = 0; j < 3; ++j)
		{
			int nextJ = j + 1;
			if (nextJ >= 3) nextJ = 0;
			int edge10 = nextNode.p[j];
			int edge11 = nextNode.p[nextJ];

			if ((edge00 == edge10 && edge01 == edge11) || (edge00 == edge11 && edge01 == edge10))
			{
				*e0 = edge00;
				*e1 = edge01;
				return;
			}

		}
	}

	*e0 = -1;
	*e1 = -1;
}

bool PathMesh::isSameEdge(int a0, int a1, int b0, int b1) const
{
	return (a0 == b0 && a1 == b1) || (a0 == b1 && a1 == b0);
}

void PathMesh::indexToPtPath(
	Path * path,
	const vector<int> & pathNodeIndices, 
	const Vector3 & startPt, const Vector3 & endPt, float margin) const
{
	int fromEdge0 = -1, fromEdge1 = -1;

	Vector3 curCandNextPt;
	float curCandNextPtDist;

	const vector<int> & prevPathIndices = path->pathIndices_;
	const vector<Vector3> & prevPathPts = path->pathPts_;

	int overlapPrevStart = -1, overlapPrevEnd = -1;
	int overlapStart = -1, overlapEnd = -1;
	
	if (prevPathIndices.size() > 1 && pathNodeIndices.size() > 1)
	{
		for (int i = 1; i < (int)prevPathIndices.size() || i < (int)pathNodeIndices.size(); ++i)
		{
			if (i < (int)prevPathIndices.size() && prevPathIndices[i] == pathNodeIndices[1])
			{
				//overlap found
				overlapPrevStart = overlapPrevEnd = i;
				overlapStart = overlapEnd = 1;				
				break;
			}

			if (i < (int)pathNodeIndices.size() && pathNodeIndices[i] == prevPathIndices[1])
			{
				//overlap found
				overlapPrevStart = overlapPrevEnd = 1;
				overlapStart = overlapEnd = i;
				break;
			}
		}
	}

	if (overlapStart > 0)
	{
		for (int i = overlapStart, j = overlapPrevStart; i < (int)pathNodeIndices.size() && j < (int)prevPathIndices.size(); ++i, ++j)
		{
			if (pathNodeIndices[i] != prevPathIndices[j]) break;
			overlapEnd = i;
			overlapPrevEnd = j;
		}
	}

	vector<Vector3> pathPts;
	pathPts.reserve(pathNodeIndices.size() + 1);


	pathPts.push_back(startPt);
	for (int i = 0; i < (int)pathNodeIndices.size() - 1; ++i)
	{
		const PathMeshNode & node = nodes_[pathNodeIndices[i]];
		const PathMeshNode & nextNode = nodes_[pathNodeIndices[i + 1]];

		int adjEdge0, adjEdge1;
		getAdjEdge(&adjEdge0, &adjEdge1, pathNodeIndices[i], pathNodeIndices[i + 1]);
		
		if (i + 1 < overlapStart || i + 1> overlapEnd)
		{
			curCandNextPtDist = FLT_MAX;

			for (int j = 0; j < 3; ++j)
			{
				Vector3 adjV;
				/*
				get pt on adjEdge that corresponds to traveling from startPt along 
				e1 - e0 axis
				*/
				Vector3 nextPt;

				int nextJ = j + 1;
				if (nextJ >= 3) nextJ = 0;			
				int e0 = node.p[j];
				int e1 = node.p[nextJ];

				if (isSameEdge(e0, e1, adjEdge0, adjEdge1) || isSameEdge(e0, e1, fromEdge0, fromEdge1))
					continue;

				if (adjEdge0 != e0) swap(adjEdge0, adjEdge1);
				if (adjEdge0 != e0) swap(e0, e1);

				const Vector3 & startPt = pathPts[i];
				Vector3 ue = (verts_[e1] - verts_[e0]).normalize();
				Vector3 v =  startPt - verts_[e0];
				float x = ue * v;
				Vector3 xVec = x * ue;
				Vector3 yVec = v - xVec;
				xVec = xVec.normalize();
				float y = yVec.magnitude();
				yVec = (1.0f / y) * yVec;			
			
				adjV = verts_[adjEdge1] - verts_[adjEdge0];		

				float adjX = adjV * xVec;
				float adjY = adjV * yVec;

				float sx = 0.0f;
				if (adjX != 0.0f)					
				{
					float m = adjY / adjX;
					sx = y / m;
				}

				nextPt = verts_[e0] + (sx * xVec) + (y * yVec);			
			

				//make sure nextPt isn't too close to an adj edge end point 
				//use margin as the min dist
				float adjVMag = adjV.magnitude();
				adjV = (1.0f/adjVMag)*adjV;

				if (adjVMag < 2.0f*margin)
				{
					curCandNextPt = verts_[adjEdge0] + (adjVMag/2.0f) * adjV;
				}
				else
				{
					float distAdj0 = (curCandNextPt - verts_[adjEdge0]).magnitude();
					if (distAdj0 < margin)				
						curCandNextPt = verts_[adjEdge0] + margin * adjV;				
					else if (adjVMag - distAdj0 < margin)
						curCandNextPt = verts_[adjEdge1] - margin * adjV;
				}


				float dist = nextPt.distanceFrom(startPt);
				if (dist < curCandNextPtDist)
				{
					curCandNextPt = nextPt;
					curCandNextPtDist = dist;
				}			
			}

			
			pathPts.push_back(curCandNextPt);
		}
		else
		{
			pathPts.push_back(prevPathPts[overlapPrevStart + (i + 1 - overlapStart)]);
		}
		

		fromEdge0 = adjEdge0;
		fromEdge1 = adjEdge1;
	}

	pathPts.push_back(endPt);

	straightenPath(&pathPts, pathNodeIndices, margin);
	path->pathPts_ = pathPts;
	path->pathIndices_ = pathNodeIndices;
	
	
	//doPathMargins(&pathPts, pathNodeIndices, margin);
}

void PathMesh::doPathMargins(std::vector<Vector3> * pathPts, 
	const std::vector<int> & pathNodeIndices, float margin) const
{
	for (int i = 0; i < (int)pathNodeIndices.size() - 1; ++i)
	{
		const PathMeshNode & node = nodes_[pathNodeIndices[i]];
		const PathMeshNode & nextNode = nodes_[pathNodeIndices[i + 1]];

		int adjEdge0, adjEdge1;
		getAdjEdge(&adjEdge0, &adjEdge1, pathNodeIndices[i], pathNodeIndices[i + 1]);
				
		Vector3 adjV;
		adjV = verts_[adjEdge1] - verts_[adjEdge0];	
		float adjVMag = adjV.magnitude();
		adjV = (1.0f/adjVMag)*adjV;
		
		if (adjVMag < 2.0f*margin)
		{
			(*pathPts)[i + 1] =
				verts_[adjEdge0] + (adjVMag/2.0f) * adjV;
		}
		else
		{
			float distAdj0 = ((*pathPts)[i + 1] - verts_[adjEdge0]).magnitude();
			if (distAdj0 < margin)				
				(*pathPts)[i + 1] = verts_[adjEdge0] + margin * adjV;				
			else if (adjVMag - distAdj0 < margin)
				(*pathPts)[i + 1] = verts_[adjEdge1] - margin * adjV;
		}
	}
}

void PathMesh::straightenPath(std::vector<Vector3> * pathPts, const std::vector<int> & pathNodeIndices, float margin) const
{
	const Vector3 & endPt = pathPts->back();
	const Vector3 & startPt = pathPts->front();
	

	vector<Vector3> oldPathPts = *pathPts;

	int fromIndex;
	int toIndex = pathPts->size() - 1;

	int maxSegments = 1;
	int numSegments = 0;

	while(toIndex > 1 && numSegments < maxSegments)
	{	
		fromIndex = 1;
		for (int i = 0; i < toIndex - 1; ++i)
		{
			const Vector3 & curPt = (*pathPts)[i];

			//project endPt onto plane of current triangle

			const PathMeshNode & node = nodes_[pathNodeIndices[i]];
			Vector3 v1 = verts_[node.p[1]] - verts_[node.p[0]];
			Vector3 v2 = verts_[node.p[2]] - verts_[node.p[0]];
			Plane plane = Plane::fromPoints(verts_[node.p[0]], verts_[node.p[1]], verts_[node.p[2]]);

			Vector3 projEndPt = (*pathPts)[toIndex];
			projEndPt.projectOnToPlane(plane);

			//get intersection of direct line to projected endPt with adjEdge

			int adjEdge0, adjEdge1;
			getAdjEdge(&adjEdge0, &adjEdge1, pathNodeIndices[i], pathNodeIndices[i + 1]);

			Vector3 xAxis = verts_[adjEdge1] - verts_[adjEdge0];
			float adjMag = xAxis.magnitude();		
			xAxis *= (1.0f/adjMag);
			Vector3 yAxis = (xAxis ^ plane.normal()).normalize();

			float curPtX = (curPt - verts_[adjEdge0]) * xAxis;
			float curPtY = (curPt - verts_[adjEdge0]) * yAxis;

			float endPtX = (projEndPt - verts_[adjEdge0]) * xAxis;
			float endPtY = (projEndPt - verts_[adjEdge0]) * yAxis;

			float intX, intY = 0.0f;
			if (endPtX == curPtX)
			{
				intX = endPtX;			
			}
			else
			{
				float m = (endPtY - curPtY) / (endPtX - curPtX);
				float b = endPtY - m * endPtX;
				intX = -b/m;
			}

			bool withinAdjEdge = margin < intX && intX < adjMag - margin;
		
			if (withinAdjEdge)
			{
				(*pathPts)[i + 1] = verts_[adjEdge0] + intX * xAxis;
			}
			else
			{
				memcpy(&(*pathPts)[fromIndex], &oldPathPts[fromIndex], sizeof(Vector3)*(i - fromIndex + 1));
				fromIndex = i + 1;
			}
		}
		++numSegments;
		toIndex = fromIndex;
	}



}

bool PathMesh::randomPt(int * nodeIndex, Vector3 * randPtOut, float margin) const
{
	Vector3 randPt;

	int maxTrys = 500;
	int curTry = 1;

	while (1)
	{
		float r = randf();

		int startIndex = 0, endIndex = (int)nodes_.size() - 1;
		int triIndex;
		while (startIndex < endIndex)
		{
			int midIndex = (endIndex + startIndex) / 2;
			float midVal = nodes_[midIndex].cumArea / totalArea_;
			if (midVal < r)
			{
				startIndex = midIndex + 1;
			}
			else
			{
				endIndex = midIndex;
			}
		}
		triIndex = startIndex;
		randPt = triangleRandomPoint(
			verts_[nodes_[triIndex].p[0]],
			verts_[nodes_[triIndex].p[1]],
			verts_[nodes_[triIndex].p[2]]);		

		bool isOk = checkMargin(randPt, triIndex, -1, -1, margin);
		if (isOk) break;

		if (curTry >= maxTrys)
		{
			//failed
			return false;
		}

		curTry++;
	}

	*randPtOut = randPt;
	return true;
}

bool PathMesh::checkMargin(const Vector3 & pt, int triIndex, int fromEdge0, int fromEdge1, float margin) const
{
	if (margin < 0.0f) return true;
	for (int i = 0; i < 3; ++i)
	{
		int nextI = i + 1;
		if (nextI == 3) nextI = 0;

		Vector3 intPt;
		int e0 = nodes_[triIndex].p[i], e1 = nodes_[triIndex].p[nextI];
		
		if (isSameEdge(e0, e1, fromEdge0, fromEdge1)) continue;
		
		float dist = distancePointSegment(&intPt, pt, verts_[e0], verts_[e1]);

		if (dist < margin)
		{
			bool foundAdjEdge = false;
			BOOST_FOREACH(int adjIndex, nodes_[triIndex].adjNodes)
			{
				int adj0, adj1;
				getAdjEdge(&adj0, &adj1, adjIndex, triIndex);
				if (isSameEdge(adj0, adj1, e0, e1))
				{
					foundAdjEdge = true;
					if (!checkMargin(intPt, adjIndex, e0, e1, margin - dist))
						return false;
				}
			}

			if (!foundAdjEdge) return false;
		}

	}
	return true;
}

float PathMesh::calcH(int startNodeIndex, int endNodeIndex) const
{
	return dist(startNodeIndex, endNodeIndex);	
}

float PathMesh::dist(int startNodeIndex, int endNodeIndex) const
{
	Vector3 ptStart = nodeCenterPt(startNodeIndex);
	Vector3 ptEnd = nodeCenterPt(endNodeIndex);
	return ptStart.distanceFrom(ptEnd);
}

Vector3 PathMesh::nodeCenterPt(int nodeIndex) const
{
	return (1.0f/3.0f)*(verts_[nodes_[nodeIndex].p[0]] + verts_[nodes_[nodeIndex].p[1]] + verts_[nodes_[nodeIndex].p[2]]);
}

bool PathMesh::pathFind(std::vector<int> * pathNodeIndices, int startNodeIndex, int endNodeIndex) const
{
	set<int> closedSet;
	set<int> openSet;
	map<int, int> cameFrom;

	openSet.insert(startNodeIndex);
	pathNodeIndices->clear();

	nodes_[startNodeIndex].gScore = 0.0f;
	nodes_[startNodeIndex].fScore = calcH(startNodeIndex, endNodeIndex);

	while (!openSet.empty())
	{
		float minF = FLT_MAX;
		int currentNodeIndex = -1;

		BOOST_FOREACH(int iIndex, openSet)
		{			
			if (nodes_[iIndex].fScore < minF) 
			{
				minF = nodes_[iIndex].fScore;
				currentNodeIndex = iIndex;
			}
		}

		if (currentNodeIndex == endNodeIndex) 
		{
			reconstructPath(pathNodeIndices, cameFrom, endNodeIndex);
			return true;
		}

		openSet.erase(currentNodeIndex);
		closedSet.insert(currentNodeIndex);
		//pathNodeIndices->push_back(currentNodeIndex);

		BOOST_FOREACH(int adjIndex, nodes_[currentNodeIndex].adjNodes)
		{
			if (closedSet.find(adjIndex) != closedSet.end()) continue;
			float tentativeGScore = nodes_[currentNodeIndex].gScore + dist(currentNodeIndex, adjIndex);

			if (openSet.find(adjIndex) == openSet.end() || tentativeGScore <= nodes_[adjIndex].gScore)
			{
				cameFrom[adjIndex] = currentNodeIndex;
				nodes_[adjIndex].gScore = tentativeGScore;
				nodes_[adjIndex].fScore = nodes_[adjIndex].gScore + calcH(adjIndex, endNodeIndex);
				if (openSet.find(adjIndex) == openSet.end()) openSet.insert(adjIndex);
			}
		}		
	}

	return false;
}

void PathMesh::reconstructPath(std::vector<int> * pathNodeIndices, const std::map<int, int> & cameFrom, int curIndex) const
{
	map<int, int>::const_iterator iter = cameFrom.find(curIndex);
	if (iter != cameFrom.end())
	{
		reconstructPath(pathNodeIndices, cameFrom, (*iter).second);
		
	}
	pathNodeIndices->push_back(curIndex);	
}

int PathMesh::pathPtAlongSegment(Vector3 * pOut, const Vector3 & argStartPt, const Vector3 & argEndPt) const
{
	Vector3 curPt(argStartPt);
	int nodeIndex = ptToNode(&curPt);
	if (nodeIndex < 0) return nodeIndex;
	
	Vector3 endPt(argEndPt);
	int endNodeIndex = ptToNode(&endPt);

	const float EPSILON = 0.0001f;
	int prevNodeIndex = -1;
	int prevEdge0 = -1, prevEdge1 = -1;

	bool deadEnd;
	do
	{
		if (nodeIndex == endNodeIndex)
		{
			curPt = endPt;
			break;
		}

		deadEnd = true;
		//project endPt onto plane of current triangle

		const PathMeshNode & node = nodes_[nodeIndex];		
		Plane plane = Plane::fromPoints(verts_[node.p[0]], verts_[node.p[1]], verts_[node.p[2]]);

		Vector3 projEndPt = argEndPt;
		projEndPt.projectOnToPlane(plane);
		
		for (int j = 0; j < 3; ++j)
		{
			//get intersection of direct line to projected endPt with adjEdge

			int nextJ = j + 1;
			if (nextJ == 3) nextJ = 0;
			int adjEdge0 = node.p[j], adjEdge1 = node.p[nextJ];

			if (isSameEdge(adjEdge0, adjEdge1, prevEdge0, prevEdge1)) continue;
			
			Vector3 xAxis = verts_[adjEdge1] - verts_[adjEdge0];
			float adjMag = xAxis.magnitude();		
			xAxis *= (1.0f/adjMag);
			Vector3 yAxis = (xAxis ^ plane.normal()).normalize();

			float curPtX = (curPt - verts_[adjEdge0]) * xAxis;
			float curPtY = (curPt - verts_[adjEdge0]) * yAxis;

			float endPtX = (projEndPt - verts_[adjEdge0]) * xAxis;
			float endPtY = (projEndPt - verts_[adjEdge0]) * yAxis;

			float intX, intY = 0.0f;
			if (endPtX == curPtX)
			{
				intX = endPtX;			
			}
			else
			{
				float m = (endPtY - curPtY) / (endPtX - curPtX);
				float b = endPtY - m * endPtX;
				intX = -b/m;
			}

			bool withinAdjEdge = -EPSILON < intX && intX < adjMag + EPSILON;
			Vector3 intPt = verts_[adjEdge0] + intX * xAxis;
			bool correctDir = (intPt - curPt) * (projEndPt - curPt) > 0.0f;

			if (withinAdjEdge && correctDir)
			{				
				curPt = intPt;

				bool adjTriFound = false;
				//check if there is an adjacent triangle with adjEdge as the adjacent edge
				for (int k = 0; k < (int)node.adjNodes.size(); ++k)
				{
					if (node.adjNodes[k] == prevNodeIndex) continue;
					int triAdjEdge0, triAdjEdge1;
					getAdjEdge(&triAdjEdge0, &triAdjEdge1, nodeIndex, node.adjNodes[k]);
					if (isSameEdge(triAdjEdge0, triAdjEdge1, adjEdge0, adjEdge1))
					{
						curPt = verts_[adjEdge0] + intX * xAxis;						
						prevNodeIndex = nodeIndex;
						prevEdge0 = adjEdge0;
						prevEdge1 = adjEdge1;
						nodeIndex = node.adjNodes[k];						
						adjTriFound = true;
						deadEnd = false;
						break;
					}
				}

				if (adjTriFound) break;
			}
		}
	} while(!deadEnd);
	
	*pOut = curPt;
	return nodeIndex;
}

int PathMesh::ptToNode(Vector3 * ppt) const
{	
	BoundingBox aabb;
	Vector3 pt(*ppt);
	
	BoundingBox wExtents = map_->worldExtents();
	const float margin = wExtents.minPt.distanceFrom(wExtents.maxPt)/1000;
	aabb.minPt = pt - Vector3(margin, margin, margin);
	aabb.maxPt = pt + Vector3(margin, margin, margin);

	set<IntTriData0> intTriData;
	kdTree_->getIntersectingTriangles(&intTriData, aabb.toObb(Matrix::Identity()));


	BOOST_FOREACH(const IntTriData0 & data, intTriData)
	{
		TriMap::const_iterator iter = triMap_.find(data);
		int nodeIndex = (*iter).second;
		float tOut;

		Vector3 normal = 
			(verts_[nodes_[nodeIndex].p[1]] - verts_[nodes_[nodeIndex].p[0]]) ^ 
			(verts_[nodes_[nodeIndex].p[2]] - verts_[nodes_[nodeIndex].p[0]]);
		
		bool intRet = triangleIntersectSegment(ppt, &tOut, 
			verts_[nodes_[nodeIndex].p[0]], verts_[nodes_[nodeIndex].p[1]], verts_[nodes_[nodeIndex].p[2]],
			pt - normal, pt + normal);

		if (intRet)	return nodeIndex;		
	}

	return -1;
}

