#include "stdafx.h"
#include "ModelFile.h"
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

using namespace std;

///////////////////////////////////////////////////////////////////////////////

JSClass jsModelClass = InitClass(
	"Model", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

JSBool Model_playAnimation(JSContext *cx, uintN argc, jsval *vp)
{	
	jsval * argv = JS_ARGV(cx, vp);
	ModelFile * thisObj = (ModelFile *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	JSObject * jsModelAnimation;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsModelAnimation)) return JS_FALSE;

	ModelAnimation modelAnimation = ModelAnimation::fromJsonScriptObject(s, jsModelAnimation);
	int index = -1;
	
	index = thisObj->playAnimation(modelAnimation);		
	
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(index));

	
	return JS_TRUE;
};

JSBool Model_isAnimationPlaying(JSContext *cx, uintN argc, jsval *vp)
{	
	jsval * argv = JS_ARGV(cx, vp);
	ModelFile * thisObj = (ModelFile *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	int32 animIndex;
	if (!JS_ConvertArguments(cx, argc, argv, "i", &animIndex)) return JS_FALSE;

	bool ret = thisObj->isAnimationPlaying((int)animIndex);	

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ret));
	return JS_TRUE;
};

JSBool Model_stopAnimation(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ModelFile * thisObj = (ModelFile *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	int32 animIndex;
	if (!JS_ConvertArguments(cx, argc, argv, "i", &animIndex)) return JS_FALSE;

	thisObj->stopAnimation((int)animIndex);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool Model_stopAllAnimations(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	ModelFile * thisObj = (ModelFile *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	thisObj->stopAllAnimations();

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}


JSFunctionSpec ModelFuncs[] = {
	JS_FS("playAnimation", Model_playAnimation, 1, 0),
	JS_FS("stopAnimation", Model_stopAnimation, 1, 0),
	JS_FS("isAnimationPlaying", Model_isAnimationPlaying, 1, 0),
	JS_FS("stopAllAnimations", Model_stopAllAnimations, 0, 0),
    JS_FS_END
};
///////////////////////////////////////////////////////////////////////////////


ModelFile::ModelAnimationData::ModelAnimationData() 
{
	animation = NULL;
	startedTime = 0.0f;
	time = 0.0f;
}
	
ModelFile::ModelAnimationData::ModelAnimationData(ModelAnimation * animation, float startedTime)
{
	this->animation = animation;
	this->startedTime = startedTime;
	this->time = startedTime;
}

ModelFile::ModelAnimationData::~ModelAnimationData()
{
	delete animation;
}

///////////////////////////////////////////////////////////////////////////////
JSObject * ModelFile::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsModelClass,
		0, 0, 0, ModelFuncs, 0, 0);

	return proto;
}

JSObject * ModelFile::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsModelClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void ModelFile::create()
{
	nextAnimationIndex_ = 0;
	model_ = 0;
	loadRequested_ = false;
}

ModelFile::ModelFile() : visualAttrib_(this)
{
	create();
	setId("Model");
}

ModelFile::ModelFile(const ModelFile & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping), visualAttrib_(this)
{
	create();

	visualAttrib_ = rhs.visualAttrib_;
	visualAttrib_.modelObj_ = this;

	fileName_ = rhs.fileName_;	
}


ModelFile::~ModelFile()
{
	delete model_;	

	BOOST_FOREACH(ModelAnimationData * data, playingAnimations_)
	{
		delete data;
	}

	playingAnimations_.clear();
}

const Matrix & ModelFile::inverseTotalTransform() const
{
	return visualAttrib_.invTotalTransform_;
}

void ModelFile::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);
	refFiles->push_back(fileName_);
}

int ModelFile::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);	
	fileName_ = refFiles[index++];		
	
	return index;
}


void ModelFile::setFileName(const std::string & fileName)
{
	fileName_ = fileName;	
}

void ModelFile::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
	Global & g = Global::instance();

	loadRequested_ = true;
	g.requestObjectLoad(this, fileName_, LoadRequest::LoadRequestModel);
}

void ModelFile::uninit()
{
	if (loadRequested_)
	{		
		loadRequested_ = false;
		Global::instance().cancelObjectLoad(this, fileName_);		
	}

	if (model_) 
	{
		model_->uninit();
		delete model_;
		model_ = 0;
	}
	SceneObject::uninit();
}

void ModelFile::applyAnimation(const ModelAnimation & animation, float time)
{
	bool animationJointTransformed;
	bool animationIsOver;
	animationSetTime(&animation, 0.0f, time, &animationIsOver, &animationJointTransformed);

	
	if (animationJointTransformed && isLoaded()) model_->update(animationJointTransformed);	
}

void ModelFile::animationSetTime(const ModelAnimation * animation, float startedTime, float time,
	bool * isOver, bool * jointTransformed)
{
	*jointTransformed = false;
	*isOver = false;

	float animTime = time - startedTime;
	if (animTime < 0.0f) return;

	if (model_)
	{		
		float endTime = model_->endTime();
		float fps = model_->fps();

		unsigned int lastFrame = endTime * fps;
		float duration = (min(lastFrame, animation->endFrame) + 1 - animation->startFrame) / fps;
		int k = (int)(animTime / duration);	
		if (!animation->isInfinite() && k >= animation->repeat) 
		{
			*isOver = true;
			return;
		}

		animTime -= k * duration;
		animTime += animation->startFrame / fps;

		model_->animationSetTime(animation, animTime, jointTransformed);
	}
}

void ModelFile::setTime(float time, bool * jointTransformed)
{
	*jointTransformed = false;
	
	if (model_)
	{
		vector<ModelAnimationData *>::iterator iter;
		for (iter = playingAnimations_.begin(); iter < playingAnimations_.end();)
		{
			ModelAnimationData * data = *iter;
			ModelAnimation * animation = data->animation;
			bool animationJointTransformed;
			bool animationIsOver;
			data->time = time;
			animationSetTime(animation, data->startedTime, data->time, &animationIsOver, &animationJointTransformed);
			*jointTransformed |= animationJointTransformed;

			if (animationIsOver)
			{
				iter = playingAnimations_.erase(iter);

				map<int, ModelAnimationData *>::iterator it1;
				for (it1 = playingAnimationIds_.begin(); it1 != playingAnimationIds_.end(); ++it1)
				{
					ModelAnimationData * curData = (*it1).second;
					if (curData == data)
					{
						playingAnimationIds_.erase(it1);
						break;
					}
				}


				delete data;
			}
			else
				++iter;
		}		
	}
}

bool ModelFile::update(float sceneTime)
{	
	bool needRedraw = SceneObject::update(sceneTime);

	bool needsDeform;
	setTime(sceneTime, &needsDeform);
	if (model_) needRedraw |= model_->update(needsDeform);
	return needRedraw;
}

bool ModelFile::asyncLoadUpdate()
{
	bool everythingLoaded = true;
	
	if (!model_ && !fileName_.empty())
	{
		bool loaded = Global::instance().getLoadedModel(this, fileName_, &model_);

		if (loaded)
		{			
			loadRequested_ = false;	
			if (model_) model_->init(gl_);
			//reapply animation to mesh
			update(Global::currentTime());
		}		
	}

	if (model_) everythingLoaded &= model_->asyncLoadUpdate();
	else everythingLoaded = fileName_.empty();
	handleLoadedEventDispatch(everythingLoaded);
	return everythingLoaded;	
}

bool ModelFile::isLoaded() const
{
	if (model_) return model_->isLoaded();
	else return false;	
}

void ModelFile::drawObject(GfxRenderer * gl) const
{
	if (model_ && parentScene_) 
	{
		if (parentScene_->zBuffer())
		{
			model_->drawObject(gl, Model::DrawOpaque);
			//must draw alpha meshes after all meshes in the scene are drawn
		}
		else
			model_->drawObject(gl, Model::DrawBoth);
	}

}

void ModelFile::drawAlpha(GfxRenderer * gl) const
{
	bool hasTransform = false;

	if (!isThisAndAncestorsVisible() || !model_) return;

	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();

	gl->pushMatrix();
	gl->loadMatrix(trans);	

	model_->drawObject(gl, Model::DrawAlpha);
	gl->popMatrix();

}
BoundingBox ModelFile::extents() const
{
	if (model_) return model_->extents();
	return BoundingBox();
}

void ModelFile::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	writer.write(fileName_, "fileName");	
}

void ModelFile::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	if (version == 0)
	{
		VisualAttrib oldVisualAttrib;
		reader.read(oldVisualAttrib);
		*(VisualAttrib *)&visualAttrib_ = oldVisualAttrib;
	}
	else reader.read(visualAttrib_);
	reader.read(fileName_);
}

void ModelFile::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	w.writeTag("FileName", fileName_);
}

void ModelFile::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;		
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(fileName_, curNode, "FileName"));
	}
}


SceneObject * ModelFile::intersect(Vector3 * out, const Ray & ray)
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

int ModelFile::playAnimation(const ModelAnimation & pAnimation)
{
	ModelAnimation * animation = new ModelAnimation(pAnimation);
	float startedTime = Global::currentTime();
	ModelAnimationData * data = new ModelAnimationData(animation, startedTime);
	playingAnimations_.push_back(data);
	playingAnimationIds_[nextAnimationIndex_] = data;
	return nextAnimationIndex_++;
}

bool ModelFile::isAnimationPlaying(int animationIndex) const
{
	if (playingAnimationIds_.find(animationIndex) == playingAnimationIds_.end()) return false;
	return true;
}

void ModelFile::stopAnimation(int animationIndex)
{
	if (playingAnimationIds_.find(animationIndex) == playingAnimationIds_.end()) return;

	ModelAnimationData * data = playingAnimationIds_[animationIndex];
	ModelAnimation * animation = data->animation;
	
	vector<ModelAnimationData *>::iterator itr;
	for (itr = playingAnimations_.begin(); itr < playingAnimations_.end(); ++itr)
	{
		if ((*itr)->animation == animation)
		{
			playingAnimations_.erase(itr);
			break;
		}
	}

	playingAnimationIds_.erase(animationIndex);
	delete data;
}

void ModelFile::stopAllAnimations()
{
	vector<ModelAnimationData *>::iterator itr;
	for (itr = playingAnimations_.begin(); itr < playingAnimations_.end(); ++itr)
	{
		ModelAnimationData * data = *itr;
		delete data;
	}

	playingAnimations_.clear();
}