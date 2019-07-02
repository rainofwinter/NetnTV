#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Texture.h"
#include "MathStuff.h"
#include "Model.h"

///////////////////////////////////////////////////////////////////////////////
class ModelFile;
class Model;
class ModelAnimation;


class KDTree;

class ModelFile : public SceneObject
{

public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "Model";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("24ED0DF2-2D32-2199-1A12-26D53333845F");
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new ModelFile(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	bool update(float sceneTime);
	Model * model() const {return model_;}

	virtual const Matrix & inverseTotalTransform() const;

	virtual bool asyncLoadUpdate();
	virtual bool isLoaded() const;

	ModelFile();
	ModelFile(const ModelFile & rhs, ElementMapping * elementMapping);

	~ModelFile();

	/**
	Default behavior is to set width and height to the size of the image file
	*/
	void setFileName(const std::string & fileName);
	const std::string & fileName() const {return fileName_;}

	void drawAlpha(GfxRenderer * gl) const;
	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "ModelFile";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	int playAnimation(const ModelAnimation & animation);
	bool isAnimationPlaying(int animIndex) const;
	void stopAnimation(int animationIndex);
	void stopAllAnimations();
	void applyAnimation(const ModelAnimation & animation, float time);
private:
	ModelFile(const ModelFile & rhs);
	ModelFile & operator = (const ModelFile & rhs);
	void create();

	void setTime(float time, bool * jointTransformed);

	void animationSetTime(const ModelAnimation * data, float startedTime, float animTime,
		bool * isOver, bool * jointTransformed);

private:

	std::string fileName_;
	ModelVisualAttrib visualAttrib_;
	Model * model_;
	bool loadRequested_;

	int nextAnimationIndex_;

	struct ModelAnimationData
	{
		ModelAnimationData();
		ModelAnimationData(ModelAnimation * animation, float startedTime);
		~ModelAnimationData();

		ModelAnimation * animation;
		float time;	
		float startedTime;
	};

	std::vector<ModelAnimationData *> playingAnimations_;	
	std::map<int, ModelAnimationData *> playingAnimationIds_;
};