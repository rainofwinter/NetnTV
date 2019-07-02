#include "stdafx.h"
#include "ModelAnimation.h"
#include "ScriptProcessor.h"
#include "Reader.h"
#include "Writer.h"
#include "Xml.h"

///////////////////////////////////////////////////////////////////////////////

ModelAnimation ModelAnimation::fromJsonScriptObject(ScriptProcessor * s, JSObject * jsModelAnimation)
{
	ModelAnimation ret;
	JSContext * cx = s->jsContext();
	jsval val;

	getPropertyUint(cx, jsModelAnimation, "startFrame", &ret.startFrame);
	getPropertyUint(cx, jsModelAnimation, "endFrame", &ret.endFrame);
	getPropertyInt(cx, jsModelAnimation, "repeat", &ret.repeat);
	getPropertyBool(cx, jsModelAnimation, "restrictJoints", &ret.restrictJoints);
	getPropertyInt(cx, jsModelAnimation, "jointIndex", &ret.jointIndex);

	JS_GetProperty(cx, jsModelAnimation, "excludedJoints", &val);	
	if (JSVAL_IS_OBJECT(val))
	{
		JSObject * jsExcludedJoints = JSVAL_TO_OBJECT(val);
		if (JS_IsArrayObject(cx, jsExcludedJoints))
		{
			jsuint jsLen;
			JS_GetArrayLength(cx, jsExcludedJoints, &jsLen);
			for (int i = 0; i < (int)jsLen; ++i)
			{
				jsval jsCurVal;
				JS_GetElement(cx, jsExcludedJoints, i, &jsCurVal);
				int index;
				if (!getInt(cx, jsCurVal, &index)) continue;
				ret.excludedJoints.insert(index);
			}
		}
	}

	getPropertyBool(cx, jsModelAnimation, "restrictMeshes", &ret.restrictMeshes);

	JS_GetProperty(cx, jsModelAnimation, "meshIndices", &val);	
	if (JSVAL_IS_OBJECT(val))
	{
		JSObject * jsMeshIndices = JSVAL_TO_OBJECT(val);
		if (JS_IsArrayObject(cx, jsMeshIndices))
		{
			jsuint jsLen;
			JS_GetArrayLength(cx, jsMeshIndices, &jsLen);
			for (int i = 0; i < (int)jsLen; ++i)
			{
				jsval jsCurVal;
				JS_GetElement(cx, jsMeshIndices, i, &jsCurVal);
				int index;
				if (!getInt(cx, jsCurVal, &index)) continue;
				ret.meshIndices.insert(index);
			}
		}
	}

	/*

	JS_GetProperty(cx, jsTransform, "rotation", &val);	
	if (JSVAL_IS_OBJECT(val))
	{
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "x", &ret.rot_.x);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "y", &ret.rot_.y);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "z", &ret.rot_.z);
	}

	JS_GetProperty(cx, jsTransform, "scale", &val);	
	if (JSVAL_IS_OBJECT(val))
	{
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "x", &ret.scale_.x);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "y", &ret.scale_.y);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "z", &ret.scale_.z);
	}

	JS_GetProperty(cx, jsTransform, "pivot", &val);	
	if (JSVAL_IS_OBJECT(val))
	{
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "x", &ret.pivot_.x);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "y", &ret.pivot_.y);
		getPropertyFloat(cx, JSVAL_TO_OBJECT(val), "z", &ret.pivot_.z);
	}*/

	return ret;
}

ModelAnimation::ModelAnimation()
{
	startFrame = 0;
	endFrame = UINT_MAX;
	repeat = 1;
	jointIndex = 0;

	restrictJoints = false;
	restrictMeshes = false;
}

void ModelAnimation::write(Writer & writer) const
{
	writer.write(startFrame);
	writer.write(endFrame);
	writer.write(repeat);
	writer.write(restrictJoints);
	writer.write(jointIndex);
	writer.write(excludedJoints);
	writer.write(restrictMeshes);
	writer.write(meshIndices);
}

void ModelAnimation::read(Reader & reader, unsigned char)
{
	reader.read(startFrame);
	reader.read(endFrame);
	reader.read(repeat);
	reader.read(restrictJoints);
	reader.read(jointIndex);
	reader.read(excludedJoints);
	reader.read(restrictMeshes);
	reader.read(meshIndices);
}

void ModelAnimation::writeXml(XmlWriter & w) const
{
	w.writeTag("StartFrame", startFrame);
	w.writeTag("EndFrame", endFrame);
	w.writeTag("Repeat", repeat);
	w.writeTag("RestrictJoints", restrictJoints);
	w.writeTag("JointIndex", jointIndex);
	std::set<int>::const_iterator itr = excludedJoints.begin();
	for( ; itr != excludedJoints.end() ; itr++)
	{
		w.writeTag("ExcludedJoints", (*itr));
	}
	w.writeTag("RestrictMeshes", restrictMeshes);
	itr = meshIndices.begin();
	for( ; itr != meshIndices.end() ; itr++)
	{
		w.writeTag("MeshIndices", (*itr));
	}
}

void ModelAnimation::readXml(XmlReader & r, xmlNode * parent)
{
	unsigned int val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(startFrame, curNode, "StartFrame"));
		else if (r.getNodeContentIfName(endFrame, curNode, "EndFrame"));
		else if (r.getNodeContentIfName(repeat, curNode, "Repeat"));
		else if (r.getNodeContentIfName(restrictJoints, curNode, "RestrictJoints"));
		else if (r.getNodeContentIfName(jointIndex, curNode, "JointIndex"));
		else if (r.getNodeContentIfName(val, curNode, "ExcludedJoints")) excludedJoints.insert(val);
		else if (r.getNodeContentIfName(restrictMeshes, curNode, "RestrictMeshes"));
		else if (r.getNodeContentIfName(val, curNode, "MeshIndices")) meshIndices.insert(val);
	}
}