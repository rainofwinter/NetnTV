#pragma once

class Reader;
class Writer;
class ScriptProcessor;
class JSObject;
class XmlWriter;
class XmlReader;

class ModelAnimation
{
	friend class ModelFile;
public:
	static ModelAnimation fromJsonScriptObject(ScriptProcessor * s, JSObject * jsTransform);
	ModelAnimation();

	bool isInfinite() const {return repeat < 0;}
	bool isExcludedJointIndex(int jointIndex) const
	{
		return excludedJoints.find(jointIndex) != excludedJoints.end();
	}
	void addExcludedJointIndex(int jointIndex)
	{
		excludedJoints.insert(jointIndex);
	}
	void clearExcludedJointIndices()
	{
		excludedJoints.clear();
	}

	bool isIncludedMeshIndex(int meshIndex) const
	{
		return meshIndices.find(meshIndex) != meshIndices.end();
	}

	unsigned char version() const {return 0;}
	void write(Writer & writer) const;
	void read(Reader & reader, unsigned char);

	const char * xmlTag() const {return "ModelAnimation";}
	void writeXml(XmlWriter & w) const;
	void readXml(XmlReader & r, xmlNode * parent);

public:

	unsigned int startFrame;
	unsigned int endFrame;

	/**
	How many times to loop the animation.
	< 0 means infinite
	*/
	int repeat;

	bool restrictJoints;
	/**
	Restrict joint animations to the joint indicated by jointIndex_ and its child 
	joints.
	*/
	int jointIndex;
	/**
	Excludes these joints and all child joints from the animation.
	*/
	std::set<int> excludedJoints;

	
	bool restrictMeshes;
	/**
	Restrict transformation animations / blendshape animations to the following meshes
	*/
	std::set<int> meshIndices;

private:


};