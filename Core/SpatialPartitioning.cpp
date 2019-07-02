#include "stdafx.h"
#include "SpatialPartitioning.h"
#include "BoundingBoxObject.h"
#include "Model.h"
#include "Mesh.h"
#include "MeshPart.h"
#include "ScriptProcessor.h"
#include "ModelFile.h"

using namespace std;

void KdTree_finalize(JSContext * cx, JSObject * obj)
{		
	KDTree * privateData = (KDTree *)JS_GetPrivate(cx, obj);
	delete privateData;
}

JSClass jsKdTreeClass = InitClass(
	"KDTree", JSCLASS_HAS_PRIVATE, JS_PropertyStub, 
	JS_StrictPropertyStub, KdTree_finalize, 0);

JSBool KdTree_constructor(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);	
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	KDTree * kdTree = new KDTree(48);

	JSObject * newObject = JS_NewObject(cx, &jsKdTreeClass, s->kdTreeProto(), 0);	
	JS_SetPrivate(cx, newObject, kdTree);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(newObject));
	return JS_TRUE;
}

JSBool KdTree_genFromModel(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	KDTree * kdtree = (KDTree *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsModel;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsModel)) return JS_FALSE;	

	SceneObject * sceneObj = NULL;
	ModelFile * modelObj = NULL;
	if (isSceneObject(s, jsModel))	
	{
		sceneObj = (SceneObject *)JS_GetPrivate(cx, jsModel);
		if (sceneObj->type() == ModelFile().type())
			modelObj = (ModelFile *)sceneObj;
	}

	if (!modelObj)
	{
		JS_ReportError(cx, "KDTree::genFromModel - Parameter not a Model object");
		return JS_FALSE;
	}

	kdtree->genFromModel(modelObj, modelObj->model());

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool KdTree_intersectOBB(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	KDTree * kdtree = (KDTree *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsObb;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObb)) return JS_FALSE;	

	OBoundingBox obb = OBoundingBox::fromScriptObject(s, jsObb);	
	set<IntTriData> normals;
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(kdtree->intersect(&normals, obb)));
	return JS_TRUE;
};

JSBool KdTree_intersectObject(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	KDTree * kdtree = (KDTree *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	


	if (!isSceneObject(s, jsObj))
	{
		JS_ReportError(cx, "KdTree::intersectObject - parameter must be a SceneObject");
		return JS_FALSE;
	}

	SceneObject * obj = (SceneObject *)JS_GetPrivate(cx, jsObj);

	boost::uuids::uuid type = obj->type();
	if (type != BoundingBoxObject().type())
	{
		JS_ReportError(cx, "KdTree::intersectObject - parameter must be a BoundingBoxObject");
		return JS_FALSE;
	}

	BoundingBoxObject * bbo = (BoundingBoxObject *)obj;
	set<IntTriData> normals;
	bool ret = kdtree->intersect(&normals, bbo);

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

JSBool KdTree_intersectSegment(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	KDTree * kdtree = (KDTree *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsObj1, * jsObj2;
	if (!JS_ConvertArguments(cx, argc, argv, "oo", &jsObj1, &jsObj2)) return JS_FALSE;	

	Vector3 a = Vector3::fromScriptObject(s, jsObj1);
	Vector3 b = Vector3::fromScriptObject(s, jsObj2);

	Vector3 intPt, normal;
	bool ret = kdtree->intersect(&intPt, &normal, a, b);

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

JSFunctionSpec KdTreeFuncs[] = {
	JS_FS("genFromModel", KdTree_genFromModel, 1, 0),
	JS_FS("intersectOBB", KdTree_intersectOBB, 1, 0),
	JS_FS("intersectObject", KdTree_intersectObject, 1, 0),
	JS_FS("intersectSegment", KdTree_intersectSegment, 1, 0),
	JS_FS_END
};

///////////////////////////////////////////////////////////////////////////////

KDTreeTriList::KDTreeTriList()
{
}

void KDTreeTriList::uninit()
{
	delete [] triIndices;
	numTris = 0;
	triIndices = NULL;
}

///////////////////////////////////////////////////////////////////////////////

KDTreeNode::KDTreeNode()
{
	direction = NONE;
	leftNode = 0;
	rightNode = 0;
	numTriLists = 0;
	pos = 0.0f;
}

void KDTreeNode::uninit()
{
	for (int i = 0; i < (int)numTriLists; ++i) triLists[i].uninit();
	delete [] triLists;
	triLists = NULL;
	numTriLists = 0;
	direction = NONE;
	leftNode = 0;
	rightNode = 0;
	pos = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////

JSObject * KDTree::createScriptObjectProto(ScriptProcessor * s, JSObject * global)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), global, 0, &jsKdTreeClass,
		KdTree_constructor, 2, 0, KdTreeFuncs, 0, 0);

	return proto;
}

JSObject * KDTree::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsKdTreeClass, s->kdTreeProto(), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);
	}

	return scriptObject_;
}

KDTree::KDTree(int numLeafTris)
{
	maxDepth_ = 24;
	numLeafTris_ = numLeafTris;
	model_ = NULL;
	obj_ = NULL;
}

void KDTree::uninit()
{
	for (int i = 0; i < (int)nodes_.size(); ++i) nodes_[i].uninit();			
	vector<KDTreeNode>().swap(nodes_);
	model_ = NULL;
	obj_ = NULL;
}

KDTree::~KDTree()
{
	uninit();
}


bool coordCompareX(const Vector3 & lhs, const Vector3 & rhs)
{
	return lhs.x < rhs.x;
}

bool coordCompareY(const Vector3 & lhs, const Vector3 & rhs)
{
	return lhs.y < rhs.y;
}

bool coordCompareZ(const Vector3 & lhs, const Vector3 & rhs)
{
	return lhs.z < rhs.z;
}

Vector3 getMedianPt(vector<Vector3> & list)
{
	Vector3 pt(0.0f, 0.0f, 0.0f);
	int size = list.size();
	if (size == 0) return pt;

	sort(list.begin(), list.end(), coordCompareX);
	pt.x = list[size/2].x;
	sort(list.begin(), list.end(), coordCompareX);
	pt.y = list[size/2].y;
	sort(list.begin(), list.end(), coordCompareX);
	pt.z = list[size/2].z;
	return pt;
}


///Assume identity transforms on all meshes
void KDTree::genFromModel(SceneObject * wrapperObj, Model * model)
{
	uninit();
	obj_ = wrapperObj;
	model_ = model;
	if (!model) throw Exception("Model not loaded");

	const vector<Mesh *> & meshes = model->meshes();

	KDTreeNode root;

	for (int i = 0; i < (int)meshes.size(); ++i)
	{
		Mesh * mesh = meshes[i];
		const vector<MeshPart *> & meshParts = mesh->meshParts();

		root.numTriLists += (unsigned short)meshParts.size();		
	}

	root.triLists = new KDTreeTriList[root.numTriLists];
	unsigned int triListIndex = 0;	
	unsigned int triVertCounter = 0;
	for (int i = 0; i < (int)meshes.size(); ++i)
	{
		Mesh * mesh = meshes[i];
		const vector<MeshPart *> & meshParts = mesh->meshParts();

		const vector<VertexData> & vertices = mesh->vertices();
		for (int j = 0; j < (int)meshParts.size(); ++j)
		{
			const vector<unsigned short> & indices = meshParts[j]->indices();

			KDTreeTriList & triList = root.triLists[triListIndex++];
			triList.meshIndex = i;
			triList.meshPartIndex = j;
			triList.numTris = indices.size()/3;
			triList.triIndices = new unsigned short[triList.numTris];			

			for (int k = 0; k < triList.numTris; ++k) triList.triIndices[k] = k;

		} //end for j meshParts
	} //end for i meshes

	nodes_.push_back(root);


	KDTreeNode * node = &nodes_[0];

	//get numTris
	unsigned int numTris = 0;
	unsigned int numTriPts = 0;
	for (int i = 0; i < node->numTriLists; ++i)
	{
		KDTreeTriList & triList = node->triLists[i];
		numTris += (unsigned int)triList.numTris;
	}
	numTriPts = 3 * numTris;		

	//calculate median Pt
	Vector3 avgPt(0.0f, 0.0f, 0.0f);	
	vector<Vector3> verts;
	for (int i = 0; i < node->numTriLists; ++i)
	{
		KDTreeTriList & triList = node->triLists[i];
		Mesh * mesh = model->meshes()[triList.meshIndex];
		MeshPart * meshPart = mesh->meshParts()[triList.meshPartIndex];

		const vector<VertexData> & vertices = mesh->vertices();
		const vector<unsigned short> & indices = meshPart->indices();

		for (int j = 0; j < triList.numTris; ++j)
		{
			int index = 3*triList.triIndices[j];
			for (int k = 0; k < 3; ++k)
			{
				Vector3 tvec = mesh->transform() * vertices[indices[index]].position;
				float x = tvec.x;
				float y = tvec.y;
				float z = tvec.z;
				verts.push_back(tvec);
				avgPt.x += x/numTriPts;
				avgPt.y += y/numTriPts;
				avgPt.z += z/numTriPts;

				++index;
			}

		}
	}	
	avgPt = getMedianPt(verts);
	splitNode(0, avgPt, 0);

}



void KDTree::splitNode(unsigned int nodeIndex, const Vector3 & avgPt, unsigned int depth)
{
	if (depth > maxDepth_) return;
	KDTreeNode * node = &nodes_[nodeIndex];

	Model * model = model_;

	//get numTris
	unsigned int numTris = 0;
	unsigned int numTriPts = 0;
	for (int i = 0; i < node->numTriLists; ++i)
	{
		KDTreeTriList & triList = node->triLists[i];
		numTris += (unsigned int)triList.numTris;
	}
	numTriPts = 3 * numTris;		
	if (numTris < numLeafTris_) return;


	////////////////////////////

	unsigned rightTotX = 0, rightTotY = 0, rightTotZ = 0;
	unsigned leftTotX = 0, leftTotY = 0, leftTotZ = 0;
	unsigned nodeTot = 0;
	for (int i = 0; i < node->numTriLists; ++i)
	{
		KDTreeTriList & triList = node->triLists[i];
		nodeTot += triList.numTris;
		Mesh * mesh = model->meshes()[triList.meshIndex];
		MeshPart * meshPart = mesh->meshParts()[triList.meshPartIndex];
		const vector<VertexData> & vertices = mesh->vertices();
		const vector<unsigned short> & indices = meshPart->indices();

		for (int j = 0; j < triList.numTris; ++j)
		{
			bool toLeftX = false, toRightX = false;
			bool toLeftY= false, toRightY = false;
			bool toLeftZ = false, toRightZ = false;
			int index = 3*triList.triIndices[j];	
			for (int k = 0; k < 3; ++k, ++index)
			{
				Vector3 tvec = mesh->transform() * vertices[indices[index]].position;
				float x = tvec.x;
				float y = tvec.y;
				float z = tvec.z;							

				if (x <= avgPt.x) toLeftX= true;
				else toRightX = true;

				if (y <= avgPt.y) toLeftY= true;
				else toRightY = true;

				if (z <= avgPt.z) toLeftZ= true;
				else toRightZ = true;
			}

			if (toLeftX) leftTotX++;
			if (toRightX) rightTotX++;

			if (toLeftY) leftTotY++;
			if (toRightY) rightTotY++;

			if (toLeftZ) leftTotZ++;
			if (toRightZ) rightTotZ++;
		}//end for j triList.numTris	
	}//end for i numTriLists

	////////////////////////////



	unsigned char direction = KDTreeNode::NONE;
	float avgPos;

	unsigned maxToTX = max(leftTotX, rightTotX);
	unsigned maxTotY = max(leftTotY, rightTotY);
	unsigned maxTotZ = max(leftTotZ, rightTotZ);
	unsigned leftTot, rightTot;
	if (maxToTX <= maxTotY && maxToTX <= maxTotZ)
	{
		direction = KDTreeNode::X;
		avgPos = avgPt.x;	
		leftTot = leftTotX;
		rightTot = rightTotX;
	}
	else if (maxTotY <= maxToTX && maxTotY <= maxTotZ)
	{
		direction = KDTreeNode::Y;
		avgPos = avgPt.y;
		leftTot = leftTotY;
		rightTot = rightTotY;
	}
	else
	{
		direction = KDTreeNode::Z;
		avgPos = avgPt.z;
		leftTot = leftTotZ;
		rightTot = rightTotZ;
	}

	//Actually distribute triangles between child nodes
	KDTreeNode left;
	KDTreeNode right;

	vector<KDTreeTriList> leftTriLists;
	vector<KDTreeTriList> rightTriLists;

	Vector3 minPtLeft(FLT_MAX, FLT_MAX, FLT_MAX), maxPtLeft(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	Vector3 minPtRight(FLT_MAX, FLT_MAX, FLT_MAX), maxPtRight(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	//Vector3 avgPtLeft(0.0f, 0.0f, 0.0f), avgPtRight(0.0f, 0.0f, 0.0f);

	vector<Vector3> leftVerts, rightVerts;

	for (int i = 0; i < node->numTriLists; ++i)
	{
		KDTreeTriList & triList = node->triLists[i];		
		Mesh * mesh = model->meshes()[triList.meshIndex];
		MeshPart * meshPart = mesh->meshParts()[triList.meshPartIndex];

		const vector<VertexData> & vertices = mesh->vertices();
		const vector<unsigned short> & indices = meshPart->indices();
		vector<unsigned short> leftTris, rightTris;
		KDTreeTriList leftTriList(triList), rightTriList(triList);		
		
		

		for (int j = 0; j < triList.numTris; ++j)
		{
			bool toLeft = false, toRight = false;

			int index = 3*triList.triIndices[j];

			Vector3 triPts[3];
			for (int k = 0; k < 3; ++k, ++index)
			{
				triPts[k] = mesh->transform() * vertices[indices[index]].position;
				float x = triPts[k].x;
				float y = triPts[k].y;
				float z = triPts[k].z;

				float pos;
				switch (direction)
				{
				case KDTreeNode::X: pos = x; break;
				case KDTreeNode::Y: pos = y; break;
				case KDTreeNode::Z: pos = z; break;
				}

				if (pos <= avgPos) toLeft = true;
				else toRight = true;
			}

			if (toLeft)
			{
				leftTris.push_back(triList.triIndices[j]);
				for (int k = 0; k < 3; ++k)	leftVerts.push_back(triPts[k]);		
			}

			if (toRight)
			{
				rightTris.push_back(triList.triIndices[j]);
				for (int k = 0; k < 3; ++k)	rightVerts.push_back(triPts[k]);				
			}

		}//end for j triList.numTris

		leftTriList.numTris = (unsigned short)leftTris.size();
		rightTriList.numTris = (unsigned short)rightTris.size();

		if (leftTriList.numTris > 0)
		{
			leftTriList.triIndices = new unsigned short[leftTriList.numTris];
			memcpy(leftTriList.triIndices, &leftTris[0], sizeof(unsigned short) * leftTriList.numTris);		
			leftTriLists.push_back(leftTriList);
		}

		if (rightTriList.numTris > 0)
		{
			rightTriList.triIndices = new unsigned short[rightTriList.numTris];
			memcpy(rightTriList.triIndices, &rightTris[0], sizeof(unsigned short) * rightTriList.numTris);
			rightTriLists.push_back(rightTriList);
		}		
	}//end for i numTriLists


	if (leftTot == nodeTot || rightTot == nodeTot) 
	{
		for (int i = 0; i < (int)leftTriLists.size(); ++i)
			leftTriLists[i].uninit();
		for (int i = 0; i < (int)rightTriLists.size(); ++i)
			rightTriLists[i].uninit();
		return;	
	}

	if (rightTriLists.size() > 0 || leftTriLists.size() > 0)
	{
		node->uninit();		
	}

	if (leftTriLists.size() > 0)
	{
		KDTreeNode leftNode;
		leftNode.numTriLists = leftTriLists.size();
		leftNode.triLists = new KDTreeTriList[leftNode.numTriLists];		
		memcpy(leftNode.triLists, &leftTriLists[0], sizeof(KDTreeTriList)*leftNode.numTriLists);
		int leftNodeIndex = (int)nodes_.size();
		//push_back can invalidate node pointer
		nodes_.push_back(leftNode);	node = &nodes_[nodeIndex];
		node->leftNode = leftNodeIndex;
		node->direction = direction;
		node->pos = avgPos;		
	}

	if (rightTriLists.size() > 0)
	{
		KDTreeNode rightNode;
		rightNode.numTriLists = rightTriLists.size();
		rightNode.triLists = new KDTreeTriList[rightNode.numTriLists];		
		memcpy(rightNode.triLists, &rightTriLists[0], sizeof(KDTreeTriList)*rightNode.numTriLists);
		int rightNodeIndex = (int)nodes_.size();
		//push_back can invalidate node pointer
		nodes_.push_back(rightNode); node = &nodes_[nodeIndex];	
		node->rightNode = rightNodeIndex;
		node->direction = direction;
		node->pos = avgPos;
	}	

	vector<KDTreeTriList>().swap(leftTriLists);
	vector<KDTreeTriList>().swap(rightTriLists);	

	if (node->leftNode > 0) 
	{
		Vector3 newAvgPt = getMedianPt(leftVerts);
		splitNode(nodes_[nodeIndex].leftNode, newAvgPt, depth + 1);	
	}
	node = &nodes_[nodeIndex]; //node ptr might have been invalidated
	if (node->rightNode > 0) 
	{
		Vector3 newAvgPt = getMedianPt(rightVerts);
		splitNode(nodes_[nodeIndex].rightNode, newAvgPt, depth + 1);
	}

}

bool KDTree::intersect(set<IntTriData> * intTriData, const OBoundingBox & obb) const
{
	//apply inverse transform to obb
	ModelVisualAttrib * attr = (ModelVisualAttrib *)obj_->visualAttrib();
	const Matrix & inverseTransform = attr->inverseTotalTransform();	

	OBoundingBox invObb;

	invObb.origin = inverseTransform * obb.origin;
	invObb.xAxis = inverseTransform * (obb.xAxis + obb.origin);
	invObb.yAxis = inverseTransform * (obb.yAxis + obb.origin);
	invObb.zAxis = inverseTransform * (obb.zAxis + obb.origin);

	invObb.xAxis = invObb.xAxis - invObb.origin;
	invObb.yAxis = invObb.yAxis - invObb.origin;
	invObb.zAxis = invObb.zAxis - invObb.origin;

	BoundingBox aabb = obj_->extents();
	bool ret = intersectOBB(intTriData, 0, invObb, aabb);

	set<IntTriData>::iterator iter;

	for (iter = intTriData->begin(); iter != intTriData->end(); ++iter)
	{
		Matrix transform = obj_->parentTransform() * obj_->visualAttrib()->transformMatrix();		
		Vector3 normal = (*iter).normal;
		normal = (transform * normal - transform * Vector3(0.0f, 0.0f, 0.0f)).normalize();
		const_cast<IntTriData &>(*iter).normal = normal;		
	}

	return ret;
}

bool KDTree::intersect(std::set<IntTriData> * intTriData, const BoundingBoxObject * bbObject) const
{
	return intersect(intTriData, bbObject->toObb());	
}

bool KDTree::childNodeAabb(const BoundingBox & parentAabb, int nodeIndex, BoundingBox * leftAabb, BoundingBox * rightAabb) const
{
	const KDTreeNode & node = nodes_[nodeIndex];

	if (node.direction != KDTreeNode::NONE)
	{
		*leftAabb = parentAabb;
		*rightAabb = parentAabb;	

		if (node.direction == KDTreeNode::X)
		{
			leftAabb->maxPt.x = node.pos;
			rightAabb->minPt.x = node.pos;
		}
		else if (node.direction == KDTreeNode::Y)
		{
			leftAabb->maxPt.y = node.pos;
			rightAabb->minPt.y = node.pos;			
		}
		else
		{
			leftAabb->maxPt.z = node.pos;
			rightAabb->minPt.z = node.pos;			
		}



		return false;
	}
	else return true;
}

bool KDTree::intersectOBB(std::set<IntTriData> * intTriData, unsigned int nodeIndex, const OBoundingBox & obb, const BoundingBox & aabb) const
{
	const KDTreeNode & node = nodes_[nodeIndex];
	Model * model = model_;

	BoundingBox leftAabb, rightAabb;


	bool isLeaf = childNodeAabb(aabb, nodeIndex, &leftAabb, &rightAabb);

	if (!isLeaf)
	{
		bool intRet = false;
		if (obb.intersect(leftAabb))
		{
			if (intersectOBB(intTriData, node.leftNode, obb, leftAabb)) intRet = true;		

		}
		if (obb.intersect(rightAabb)) 
		{
			if (intersectOBB(intTriData, node.rightNode, obb, rightAabb)) intRet = true;				
		}

		if (intRet) return true;
	}
	else
	{
		//leaf node

		bool ret = false;

		for (unsigned short i = 0; i < node.numTriLists; ++i)
		{
			const KDTreeTriList & triList = node.triLists[i];
			Mesh * mesh = model->meshes()[triList.meshIndex];
			MeshPart * meshPart = mesh->meshParts()[triList.meshPartIndex];
			const vector<unsigned short> & indices = meshPart->indices();
			const vector<VertexData> & vertices = mesh->vertices();


			float minT = FLT_MAX;
			Vector3 minIntPt;
			for (unsigned short j = 0; j < triList.numTris; ++j)
			{
				int index = 3*triList.triIndices[j];

				const Vector3 & pos0 = vertices[indices[index++]].position;
				const Vector3 & pos1 = vertices[indices[index++]].position;
				const Vector3 & pos2 = vertices[indices[index]].position;

				if (obb.intersect(pos0, pos1, pos2)) 
				{						
					Vector3 normal = ((pos1 - pos0) ^ (pos2 - pos0));
					IntTriData data;
					data.meshIndex = triList.meshIndex;
					data.meshPartIndex = triList.meshPartIndex;
					data.triIndex = triList.triIndices[j];
					data.normal = normal;
					intTriData->insert(data);				
					ret = true;					
				}
			}
		}

		if (ret) return ret;
	}

	return false;

}

bool KDTree::intersect(Vector3 * intPt, Vector3 * normal, const Vector3 & pt0, const Vector3 & pt1) const
{
	//apply inverse transform to segment
	ModelVisualAttrib * attr = (ModelVisualAttrib *)obj_->visualAttrib();
	const Matrix & inverseTransform = attr->inverseTotalTransform();	
	Vector3 invPt0 = inverseTransform * pt0;
	Vector3 invPt1 = inverseTransform * pt1;

	bool ret = intersectSegment(intPt, normal, 0, invPt0, invPt1);

	if (ret)
	{
		Matrix totalTrans = obj_->parentTransform() * obj_->visualAttrib()->transformMatrix();
		*intPt = totalTrans * (*intPt);

		*normal = (totalTrans * (*normal) - totalTrans * Vector3(0.0f, 0.0f, 0.0f)).normalize();

	}

	return ret;
}


bool KDTree::intersectSegment(Vector3 * intPtOut, Vector3 * normal, unsigned int nodeIndex, const Vector3 & pt0, const Vector3 & pt1) const
{
	static const float EPSILON = 0.0001f;
	const KDTreeNode & node = nodes_[nodeIndex];

	bool leftSide = false, rightSide = false;
	Model * model = model_;


	unsigned int pt0Node, pt1Node;
	float t;
	Vector3 dir = pt1 - pt0;
	Vector3 ptInt;


	float ptCoord0, ptCoord1, dirCoord;

	if (node.direction != KDTreeNode::NONE)
	{
		if (node.direction == KDTreeNode::X)
		{
			ptCoord0 = pt0.x;
			ptCoord1 = pt1.x; 
			dirCoord = dir.x;
		}
		else if (node.direction == KDTreeNode::Y)
		{
			ptCoord0 = pt0.y;
			ptCoord1 = pt1.y; 
			dirCoord = dir.y;
		}
		else
		{
			ptCoord0 = pt0.z;
			ptCoord1 = pt1.z; 
			dirCoord = dir.z;
		}

		if (ptCoord0 <= node.pos) 
		{
			pt0Node = node.leftNode;
			leftSide = true; 
		} else {
			pt0Node = node.rightNode;
			rightSide = true;
		}

		if (ptCoord1 <= node.pos) 
		{
			pt1Node = node.leftNode;
			leftSide = true; 
		} else {
			pt1Node = node.rightNode;
			rightSide = true;		
		}

		//node.pos = pt0.x + t * dir.x;		

		if (leftSide && rightSide)
		{
			//if the line is not parallel to splitting plain
			if (dirCoord < -EPSILON || dirCoord > EPSILON)
			{
				t = (node.pos - ptCoord0) / dirCoord;
				ptInt = pt0 + t * dir;

			}
			else
			{
				//enforce left side if the line is parallel to splitting plane
				//and it was erroneously computed that it straddles (floating pt 
				//issues)
				pt0Node = node.leftNode;
				pt1Node = node.leftNode;
				rightSide = false;
			}
		}

		//if the line segment straddles the splitting plane
		if (leftSide && rightSide)
		{

			if (intersectSegment(intPtOut, normal, pt0Node, pt0, ptInt)) return true;
			if (intersectSegment(intPtOut, normal, pt1Node, ptInt, pt1)) return true;
		}
		//else if only on one side
		else
		{
			if (intersectSegment(intPtOut, normal, pt0Node, pt0, pt1)) return true;
		}
	}
	else
	{
		//leaf node
		for (unsigned short i = 0; i < node.numTriLists; ++i)
		{
			const KDTreeTriList & triList = node.triLists[i];
			Mesh * mesh = model->meshes()[triList.meshIndex];
			MeshPart * meshPart = mesh->meshParts()[triList.meshPartIndex];
			const vector<unsigned short> & indices = meshPart->indices();
			const vector<VertexData> & vertices = mesh->vertices();


			float minT = FLT_MAX;
			Vector3 minIntPt;
			Vector3 minP0, minP1, minP2;
			for (unsigned short j = 0; j < triList.numTris; ++j)
			{
				int index = 3*triList.triIndices[j];

				const Vector3 & pos0 = vertices[indices[index++]].position;
				const Vector3 & pos1 = vertices[indices[index++]].position;
				const Vector3 & pos2 = vertices[indices[index]].position;

				float t;
				Vector3 intPt;
				bool intersect = triangleIntersectSegment(
					&intPt, &t, 
					pos0, pos1, pos2,
					pt0, pt1);

				if (intersect && t < minT)
				{
					minT = t;
					minP0 = pos0;
					minP1 = pos1;
					minP2 = pos2;
					minIntPt = intPt;
				}
			}

			if (minT < FLT_MAX)
			{				
				*intPtOut =  minIntPt;

				//compute normal
				//Note: Not unit vector
				*normal = ((minP1 - minP0) ^ (minP2 - minP0));								
				return true;
			}
		}
	}

	return false;
}

void KDTree::getIntersectingTriangles(std::set<IntTriData0> * intTriData, const OBoundingBox & obb) const
{
	//apply inverse transform to obb
	ModelVisualAttrib * attr = (ModelVisualAttrib *)obj_->visualAttrib();
	const Matrix & inverseTransform = attr->inverseTotalTransform();	

	OBoundingBox invObb;

	invObb.origin = inverseTransform * obb.origin;
	invObb.xAxis = inverseTransform * (obb.xAxis + obb.origin);
	invObb.yAxis = inverseTransform * (obb.yAxis + obb.origin);
	invObb.zAxis = inverseTransform * (obb.zAxis + obb.origin);

	invObb.xAxis = invObb.xAxis - invObb.origin;
	invObb.yAxis = invObb.yAxis - invObb.origin;
	invObb.zAxis = invObb.zAxis - invObb.origin;

	BoundingBox aabb = obj_->extents();
	getIntersectingTriangles(intTriData, 0, invObb, aabb);
}

void KDTree::getIntersectingTrianglesLocal(std::set<IntTriData0> * intTriData, const OBoundingBox & obb) const
{
	BoundingBox aabb = obj_->extents();
	getIntersectingTriangles(intTriData, 0, obb, aabb);
}

void KDTree::getIntersectingTriangles(std::set<IntTriData0> * intTriData, int nodeIndex, const OBoundingBox & obb, const BoundingBox & aabb) const
{
	const KDTreeNode & node = nodes_[nodeIndex];
	Model * model = model_;

	BoundingBox leftAabb, rightAabb;


	bool isLeaf = childNodeAabb(aabb, nodeIndex, &leftAabb, &rightAabb);

	if (!isLeaf)
	{
		if (obb.intersect(leftAabb))
		{
			getIntersectingTriangles(intTriData, node.leftNode, obb, leftAabb);		

		}
		if (obb.intersect(rightAabb)) 
		{
			getIntersectingTriangles(intTriData, node.rightNode, obb, rightAabb);				
		}
	}
	else
	{
		//leaf node

		for (unsigned short i = 0; i < node.numTriLists; ++i)
		{			
			const KDTreeTriList & triList = node.triLists[i];
			Mesh * mesh = model->meshes()[triList.meshIndex];
			MeshPart * meshPart = mesh->meshParts()[triList.meshPartIndex];
			const vector<unsigned short> & indices = meshPart->indices();
			const vector<VertexData> & vertices = mesh->vertices();

			for (unsigned short j = 0; j < triList.numTris; ++j)
			{
				int index = 3*triList.triIndices[j];
				const Vector3 & pos0 = vertices[indices[index++]].position;
				const Vector3 & pos1 = vertices[indices[index++]].position;
				const Vector3 & pos2 = vertices[indices[index]].position;
				if (obb.intersect(pos0, pos1, pos2)) 
				{				
					IntTriData0 data;
					data.meshIndex = triList.meshIndex;
					data.meshPartIndex = triList.meshPartIndex;
					data.triIndex = triList.triIndices[j];
					intTriData->insert(data);						
				}
			}
		}
	}
}