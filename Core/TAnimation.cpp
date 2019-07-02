#include "stdafx.h"
#include "Reader.h"
#include "Xml.h"
#include "Writer.h"
#include "Texture.h"
#include "FileUtils.h"
#include "TAnimation.h"
#include "GfxRenderer.h"
#include "libxml.h"
#include "libxml/xmlwriter.h"
#include <boost/scope_exit.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include "Global.h"
#include "AudioObject.h"
#include "Scene.h"
#include "Document.h"
using namespace std;
using namespace boost;
///////////////////////////////////////////////////////////////////////////////




float TLoadCue::loadTime(float loadSpeed) const
{
	return placeObject_->object()->loadSize() / (loadSpeed * 1024.0f);
}
bool DEBUGFLAG = false;
/*
#include "Text.h"
Scene * DEBUGSCENE = NULL;
float DEBUGTIME = 0.0f;
*/
///////////////////////////////////////////////////////////////////////////////
TFrames::TFrames()
{
	curFrameIndex_ = 0;
	isPlaying_ = false;
	duration_ = 0.0f;
	repeat_ = false;
	curRep_ = 0;

	parentDepth_ = NULL;
}

TFrames::TFrames(const TFrames & rhs)
{
	duration_ = rhs.duration_;
	repeat_ = rhs.repeat_;
	parentDepth_ = rhs.parentDepth_;

	for (int i = 0; i < (int)rhs.frames_.size(); ++i)
	{
		frames_.push_back(TAnimFrameSPtr(new TAnimFrame(*rhs.frames_[i])));
	}

	isPlaying_ = false;
	curFrameIndex_ = 0;	
	curRep_ = 0;
	animTime_ = 0.0f;
	startTime_ = 0.0f;
}

struct MaskRegion
{
	unsigned short startIndex;
	unsigned short endIndex;
	std::list<TAnimDepth *> masks;
};

void TFrames::drawDepth(GfxRenderer * gl, TAnimationObject * animObj, TAnimDepth * depth) const
{
	VisualAttrib * v = depth->visualAttrib();
	if (!v->isVisible()) return;

	TPlaceObject * placeObject = depth->placeObject();
	if (placeObject)
	{
		gl->pushMatrix();
		gl->multMatrix(v->transformMatrix());
		placeObject->draw(gl, animObj, depth);
		gl->popMatrix();
	}	
}

void TFrames::drawMasks(GfxRenderer * gl, TAnimationObject * animObj, std::list<TAnimDepth *> & masks) const
{	
	gl->beginMaskDrawing();
	
	if (masks.size() == 1)
	{		
		drawDepth(gl, animObj, masks.front());		
	}
	else if (masks.size() > 1)
	{
		list<TAnimDepth *>::iterator itr = masks.begin();
		int n = masks.size();
		for (int i = 0; i < n - 1; ++i)
		{
			gl->beginMaskDrawing();
		}

		drawDepth(gl, animObj, masks.front());
		gl->endMaskDrawing();
		gl->beginMaskedDrawing();
		
		++itr;

		for (int i = 0; i < n - 2; ++i)
		{
			TAnimDepth * mask = *itr;
			drawDepth(gl, animObj, mask);
			gl->endMaskedDrawing();
			gl->endMaskDrawing();
			gl->beginMaskedDrawing();
			++itr;
		}

		drawDepth(gl, animObj, masks.back());
		gl->endMaskedDrawing();
	}
	gl->endMaskDrawing();
}

void TFrames::drawDepths(GfxRenderer * gl, TAnimationObject * animObj) const
{
	std::vector<MaskRegion> maskRegions;

	MaskRegion curMaskRegion;
	
	//compute mask regions
	
	curMaskRegion.endIndex = 0;
	curMaskRegion.startIndex = USHRT_MAX;	
	
	for (int i = 0; i < (int)depths_.size(); ++i)
	{
		TPlaceObject * placeObject = depths_[i]->placeObject();
		
		
		if (placeObject && placeObject->clipDepth() > i)
		{			
			if (!curMaskRegion.masks.empty())
			{
				curMaskRegion.endIndex = i - 1;
				if (curMaskRegion.startIndex <= curMaskRegion.endIndex)
					maskRegions.push_back(curMaskRegion);
			}

			curMaskRegion.startIndex = i + 1;
			curMaskRegion.masks.push_back(depths_[i].get());			
		}

		list<TAnimDepth *>::iterator itr;
		bool shouldErase = false;
		for (itr = curMaskRegion.masks.begin(); itr != curMaskRegion.masks.end(); ++itr)
		{
			TAnimDepth * depth = *itr;
			TPlaceObject * placeObject = depth->placeObject();
			if (placeObject->clipDepth() < i) shouldErase = true;
		}

		if (shouldErase)
		{
			curMaskRegion.endIndex = i - 1;
			if (curMaskRegion.startIndex <= curMaskRegion.endIndex)
				maskRegions.push_back(curMaskRegion);
		}

		for (itr = curMaskRegion.masks.begin(); itr != curMaskRegion.masks.end();)
		{
			TAnimDepth * depth = *itr;
			TPlaceObject * placeObject = depth->placeObject();
			if (placeObject->clipDepth() < i)
				curMaskRegion.masks.erase(itr++);
			else ++itr;
		}
	}

	if (!curMaskRegion.masks.empty())
	{
		curMaskRegion.endIndex = (unsigned short)depths_.size() - 1;
		if (curMaskRegion.startIndex <= curMaskRegion.endIndex)
			maskRegions.push_back(curMaskRegion);
	}
	//end compute mask regions


	
	//draw...
	int curMaskRegionIndex = 0;

	for (int i = 0; i < (int)depths_.size(); ++i)
	{
		TPlaceObject * placeObject = depths_[i]->placeObject();
	
		if (curMaskRegionIndex < maskRegions.size())
		{
			if (maskRegions[curMaskRegionIndex].startIndex == i)
			{
				drawMasks(gl, animObj, maskRegions[curMaskRegionIndex].masks);				
				gl->beginMaskedDrawing();
			}
		}
		
		if (placeObject && !placeObject->clipDepth()) drawDepth(gl, animObj, depths_[i].get());
		
		if (curMaskRegionIndex < maskRegions.size())
		{
			if (maskRegions[curMaskRegionIndex].endIndex == i)
			{
				gl->endMaskedDrawing();
				curMaskRegionIndex++;
			}
		}
		
	}
}

void TFrames::unloadDepths()
{
	for (int i = 0; i < (int)depths_.size(); ++i)
	{
		TPlaceObject * placeObject = depths_[i]->placeObject();
		if (placeObject)
		{
			if (!placeObject->keepLoaded()) placeObject->unload();				
		}
	}
}

void TFrames::doFirstFrame(TAnimationObject * animObj, float atime)
{
	depths_.clear();
	curFrameIndex_ = 0;
	while(curFrameIndex_ < (int)frames_.size())
	{
		if (frames_[curFrameIndex_]->time() == 0.0f)
		{
			//process frame
			TAnimFrame * frame = frames_[curFrameIndex_].get();
			for (int i = 0; i < (int)frame->tags().size(); ++i)
			{
				frame->tags()[i]->doTag(this, animObj, atime);
			}
		}
		else break;

		curFrameIndex_++;
	}
}

bool TFrames::updateFrame(TAnimationObject * animObj, float atime)
{
	bool ret = false;
	if (!isPlaying_) return ret;
		
	if (repeat_)
	{
		int numRep = (int)((atime - startTime_) / duration_);
		animTime_ = (atime - startTime_) - numRep * duration_;

		if (numRep > curRep_)
		{
			unloadDepths();
			depths_.clear();
			curFrameIndex_ = 0;
			curRep_ = numRep;
			ret = true;
		}
	
	}	
	else
	{
		animTime_ = atime - startTime_;
	}
	
	while(curFrameIndex_ < (int)frames_.size())
	{
		if (frames_[curFrameIndex_]->time() <= animTime_)
		{
			//process frame
			TAnimFrame * frame = frames_[curFrameIndex_].get();
			for (int i = 0; i < (int)frame->tags().size(); ++i)
			{
				frame->tags()[i]->doTag(this, animObj, atime);
			}
		}
		else break;
		
		curFrameIndex_++;
	}

	for (int i = 0; i < (int)depths_.size(); ++i)
	{
		
		TPlaceObject * placeObject = depths_[i]->placeObject();

		if (placeObject)
		{			
			TAnimObject * obj = placeObject->object();
			obj->update(animObj, atime);
		}	
	}

	return ret;
}


int TFrames::setReferencedFiles(const std::string & baseDirectory, const vector<string> & refFiles, int index)
{
	for (int i = 0; i < (int)frames_.size(); ++i)
	{
		for (int j = 0; j < (int)frames_[i]->tags().size(); ++j)
		{
			TAnimTag * tag = frames_[i]->tags()[j].get();

			TPlaceObject * placeObject = tag->placeObject();
			if (placeObject)			
				index = placeObject->object()->setReferencedFiles(baseDirectory, refFiles, index);			
		}
	}

	return index;
}

void TFrames::referencedFiles(std::vector<std::string> * refFiles) const
{
	for (int i = 0; i < (int)frames_.size(); ++i)
	{
		for (int j = 0; j < (int)frames_[i]->tags().size(); ++j)
		{
			TAnimTag * tag = frames_[i]->tags()[j].get();

			TPlaceObject * placeObject = tag->placeObject();
			if (placeObject)			
				placeObject->object()->referencedFiles(refFiles);			
		}
	}
}

void TFrames::play(TAnimationObject * animObj, float time)
{
	startTime_ = time;
	isPlaying_ = true;
	curFrameIndex_ = 0;
	depths_.clear();
	curRep_ = 0;

	doFirstFrame(animObj, time);
}

void TFrames::setDuration(float duration)
{
	duration_ = duration;
}

void TFrames::addFrame(const TAnimFrameSPtr & frame)
{
	frames_.push_back(frame);
}

void TFrames::setParentDepth(TAnimDepth * parentDepth)
{
	parentDepth_ = parentDepth;
}

void TFrames::stopSounds()
{
	for (int i = 0; i < (int)frames_.size(); ++i)
	{
		const std::vector<TAnimTagSPtr> & tags = frames_[i]->tags();
		for (int j = 0; j < (int)tags.size(); ++j)
		{
			TPlaySound * playSound = tags[j]->playSound();
			if (playSound)
			{
				playSound->audio()->stop();
			}
			

			TPlaceObject * placeObject = tags[j]->placeObject();
			if (placeObject && placeObject->object())
			{
				TAnimObject * obj = placeObject->object();
				TAnimSprite * sprite = obj->sprite();
				if (sprite) sprite->stopSounds();
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
TAnimObject::TAnimObject()
{
}

TAnimObject::~TAnimObject()
{
}

///////////////////////////////////////////////////////////////////////////////

TAnimFrame::TAnimFrame()
{
	time_ = 0.0f;
}

TAnimFrame::TAnimFrame(const TAnimFrame & rhs)
{
	time_ = rhs.time_;
	for (unsigned i = 0; i < (unsigned)rhs.tags_.size(); ++i)
	{
		tags_.push_back(TAnimTagSPtr(rhs.tags_[i]->clone()));
	}
}

void TAnimFrame::addTag(const TAnimTagSPtr & tag)
{
	tags_.push_back(tag);
}


///////////////////////////////////////////////////////////////////////////////

TAnimSprite::TAnimSprite()
{
	isPlaying_ = false;
	curFrameIndex_ = -1;
	//repeat_ = true;
}

TAnimSprite::TAnimSprite(const TAnimSprite & rhs) : TAnimObject(rhs), TFrames(rhs)
{
}

TAnimSprite::~TAnimSprite()
{
	unload();
}

unsigned int TAnimSprite::loadSize() const
{
	unsigned int ret = 0;
	for (int i = 0; i < (int)frames_.size(); ++i)
	{
		for (int j = 0; j < (int)frames_[i]->tags().size(); ++j)
		{
			TAnimTag * tag = frames_[i]->tags()[j].get();
			TPlaceObject * placeObject = tag->placeObject();

			if (placeObject && placeObject->object()) 
			{
				ret += placeObject->object()->loadSize();
			}
		}
	}

	return ret;
}

bool TAnimSprite::isLoaded() const
{
	bool ret = true;
	for (int i = 0; i < (int)loadObjs_.size(); ++i)
	{
		ret &= loadObjs_[i]->isLoaded();
	}

	return ret;
}

void TAnimSprite::load(GfxRenderer * gl, TAnimationObject * animObj)
{
	loadObjs_.clear();
	for (int i = 0; i < (int)frames_.size(); ++i)
	{
		for (int j = 0; j < (int)frames_[i]->tags().size(); ++j)
		{
			TAnimTag * tag = frames_[i]->tags()[j].get();
			TPlaceObject * placeObject = tag->placeObject();

			if (placeObject && placeObject->object()) 
			{
				placeObject->load(gl, animObj);
				placeObject->setKeepLoaded(true);
				loadObjs_.push_back(placeObject);
			}
		}
	}
	doFirstFrame(animObj, Global::currentTime());
}

void TAnimSprite::unload()
{
	for (int i = 0; i < (int)loadObjs_.size(); ++i)
	{
		loadObjs_[i]->unload();
	}
}

void TAnimSprite::asyncLoadUpdate()
{
	for (int i = 0; i < (int)loadObjs_.size(); ++i)
	{
		loadObjs_[i]->asyncLoadUpdate();
	}
}

void TAnimSprite::update(TAnimationObject * obj, float time)
{
	updateFrame(obj, time);
}

void TAnimSprite::draw(GfxRenderer * gl, TAnimationObject * animObj, TAnimDepth * depth) const
{
	drawDepths(gl, animObj);
}

void TAnimSprite::referencedFiles(std::vector<std::string> * refFiles) const
{
	TFrames::referencedFiles(refFiles);
}

int TAnimSprite::setReferencedFiles(const std::string & baseDirectory, 
	const std::vector<std::string> & refFiles, int index)
{
	return TFrames::setReferencedFiles(baseDirectory, refFiles, index);
}


///////////////////////////////////////////////////////////////////////////////

TAnimImage::TAnimImage()
{
	imageSize_ = 0;
	matrix_ = Matrix::Identity();
}

TAnimImage::TAnimImage(const TAnimImage & rhs) : TAnimObject(rhs)
{
	matrix_ = rhs.matrix_;
	imageSize_ = rhs.imageSize_;	
	fileNames_ = rhs.fileNames_;
	coords_ = rhs.coords_;


	//text_ = rhs.text_;
}

TAnimImage::~TAnimImage()
{
	unload();
}

bool TAnimImage::isLoaded() const
{
	bool loaded = true;
	
	for (int i = 0; i < (int)textures_.size(); ++i)
		loaded &= textures_[i]->isLoaded();
	
	return loaded;
}



void TAnimImage::setFileNames(
	const std::vector< std::string> & fileNames,
	const std::vector< std::pair<int, int> > & coords)
{
	fileNames_ = fileNames;
	coords_ = coords;
}

void TAnimImage::load(GfxRenderer * gl, TAnimationObject * animObj)
{
	/*
	texture_.reset(new Texture);
	texture_->setFileName(fileName_);
	texture_->init(gl);
	*/

	textures_.clear();
	for (int i = 0; i < (int)fileNames_.size(); ++i)
	{
		textures_.push_back(TextureSPtr(new Texture));		
		textures_[i]->setFileName(animObj->actualFileName(fileNames_[i]));
		textures_[i]->init(gl);
	}

	/*
	text_.reset(new Text);
	text_->setParentScene(DEBUGSCENE);
	
	string str = getFileNameWithoutDirectory(fileName_);
	std::wstring wStr(str.begin(), str.end());
	text_->setTextString(wStr);
	
	TextFont font;
	font.fontFile = "e:/st/malgun.ttf";
	font.pointSize = 10;
	
	TextProperties props0;
	props0.font = font;
	props0.index = 0;
	props0.color = Color(0.0f, 0.0f, 0.0f, 1.0f);
	

	vector<TextProperties> props;
	props.push_back(props0);
	text_->setProperties(props);
	text_->init(gl);
	*/
}

void TAnimImage::unload()
{
	textures_.clear();
	//texture_.reset();	
	//text_.reset();
}

void TAnimImage::asyncLoadUpdate()
{
	for (int i = 0; i < (int)textures_.size(); ++i)
	{
		textures_[i]->asyncLoadUpdate();
	}
	
	//if (text_) text_->asyncLoadUpdate();
}

void TAnimImage::update(TAnimationObject * animObj, float time)
{
}

void TAnimImage::drawRegion(
	Texture * texture, GfxRenderer * gl, 
	TAnimDepth * depth, int x, int y, float opacity) const
{
	if (texture->needSeparateAlpha())
	{
		gl->useTextureAlphaProgram();
		gl->setTextureAlphaProgramOpacity(opacity);		
	}
	else
	{
		gl->useTextureProgram();
		gl->setTextureProgramOpacity(opacity);				
	}

	gl->pushMatrix();	
	gl->multMatrix(Matrix::Translate(x, y, 0.0f));

	gl->use(texture);
	gl->drawRect(0, 0, texture->width(), texture->height());
	
	gl->popMatrix();
}

void TAnimImage::draw(GfxRenderer * gl, TAnimationObject * animObj, TAnimDepth * depth) const
{
	float opacity = animObj->visualAttrib()->opacity();
	
	TAnimDepth * curDepth = depth;

	while (curDepth)
	{
		opacity *= curDepth->visualAttrib()->opacity();
		curDepth = curDepth->parentSprite()->parentDepth();
	}
	
	
	gl->pushMatrix();
	gl->multMatrix(matrix_);

	  
	for (int i = 0; i < textures_.size(); ++i)
	{
		drawRegion(textures_[i].get(), gl, depth, coords_[i].first, coords_[i].second, opacity);
	}


	gl->popMatrix();
}

void TAnimImage::referencedFiles(std::vector<std::string> * refFiles) const
{
	for (int i = 0; i < (int)fileNames_.size(); ++i)
	{
		refFiles->push_back(fileNames_[i]);
	}	
}

int TAnimImage::setReferencedFiles(const std::string & baseDirectory, 
	const std::vector<std::string> & refFiles, int index)
{
	for (int i = 0; i < (int)fileNames_.size(); ++i)
	{		
		fileNames_[i] = refFiles[index++];
	}	

	return index;
}

///////////////////////////////////////////////////////////////////////////////
TPlaySound::TPlaySound()
{
	
	audio_.reset(new AudioObject);
	
}

TPlaySound::TPlaySound(const TPlaySound & rhs)
{

	audio_.reset(new AudioObject);
	audio_->setFileName(rhs.audio_->fileName());
	time_ = rhs.time_;
}

void TPlaySound::setFileName(const std::string & fileName)
{
	fileName_ = fileName;
}

TPlaySound::~TPlaySound()
{
	audio_.reset();
}

void TPlaySound::doTag(TFrames * tframes, TAnimationObject * obj, float time)
{
	audio_->setParentScene(obj->parentScene());
	audio_->setFileName(obj->actualFileName(fileName_));
	audio_->play(0.0f);
}

///////////////////////////////////////////////////////////////////////////////

TPlaceObject::TPlaceObject()
{
	hasMatrix_ = hasVisibility_ = hasOpacity_ = false;		
	keepLoaded_ = false;
	clipDepth_ = 0;
	object_.reset();
}

TPlaceObject::TPlaceObject(const TPlaceObject & rhs)
{
	depth_ = rhs.depth_;
	if (rhs.object_)
		object_.reset(rhs.object_->clone());

	hasMatrix_ = rhs.hasMatrix_;
	matrix_ = rhs.matrix_;

	hasVisibility_ = rhs.hasVisibility_;
	visibility_ = rhs.visibility_;

	hasOpacity_ = rhs.hasOpacity_;
	opacity_ = rhs.opacity_;

	keepLoaded_ = rhs.keepLoaded_;

	clipDepth_ = rhs.clipDepth_;
}

void TPlaceObject::load(GfxRenderer * gl, TAnimationObject * animObj)
{
	if (!object_) return;
	object_->load(gl, animObj);	
}

void TPlaceObject::draw(GfxRenderer * gl, TAnimationObject * animObj, TAnimDepth * depth)
{
	if (object_) object_->draw(gl, animObj, depth);
}

void TPlaceObject::unload()
{
	if (object_) object_->unload();
}

void TPlaceObject::asyncLoadUpdate()
{
	if (object_) object_->asyncLoadUpdate();
}

bool TPlaceObject::isLoaded() const
{
	if (!object_) return false;
	return object_->isLoaded();
}

void TPlaceObject::doTag(TFrames * tFrames, TAnimationObject * animObj, float time)
{
	vector<TAnimFrameSPtr> & frames = tFrames->frames_;
	vector<TAnimDepthSPtr> & depths = tFrames->depths_;


	while (depths.size() <= depth_) 
	{
		depths.push_back(TAnimDepthSPtr(new TAnimDepth(tFrames, depths.size())));
	}

	if (object_)
	{
		depths[depth_]->visualAttrib()->setOpacity(1.0f);
		depths[depth_]->visualAttrib()->setVisible(true);
		//dont reset transform

		TPlaceObject * placeObject = depths[depth_]->placeObject();
		if (placeObject)
		{			
			if (!placeObject->keepLoaded()) placeObject->unload();
		}

		depths[depth_]->setPlaceObject(this);
		//object_->setParentDepth(depths[depth_].get());
		TAnimSprite * s = object_->sprite();
		if (s)
		{
			s->play(animObj, time);
		}
		//placeObject obj should already be loaded at this point
	}

	if (hasMatrix_)	
	{		
		depths[depth_]->visualAttrib()->setTransformMatrix(matrix_);
	}
	
	if (hasOpacity_)
	{
		depths[depth_]->visualAttrib()->setOpacity(opacity_);
	}

	if (hasVisibility_)
		depths[depth_]->visualAttrib()->setVisible(visibility_);
}

void TPlaceObject::setObject(TAnimObject & obj)
{
	object_.reset(obj.clone());
}

void TPlaceObject::setDepth(unsigned short depth)
{
	depth_ = depth;
}

void TPlaceObject::setMatrix(const Matrix & matrix)
{
	hasMatrix_ = true;
	matrix_ = matrix;
}

void TPlaceObject::setOpacity(float opacity)
{
	hasOpacity_ = true;
	opacity_ = opacity;
}

void TPlaceObject::setVisibility(bool visibility)
{
	hasVisibility_ = true;
	visibility_ = visibility;
}

///////////////////////////////////////////////////////////////////////////////

void TRemoveObject::setDepth(unsigned short depth)
{
	depth_ = depth;
}
 
///////////////////////////////////////////////////////////////////////////////

void TRemoveObject::doTag(TFrames * obj, TAnimationObject * animObj, float time)
{
	vector<TAnimFrameSPtr> & frames = obj->frames_;
	vector<TAnimDepthSPtr> & depths = obj->depths_;

	if (depths.size() > depth_)
	{
		TPlaceObject * placeObject = depths[depth_]->placeObject();
		if (placeObject)
		{
			if (!placeObject->keepLoaded()) placeObject->unload();
		}
		depths[depth_]->setPlaceObject(NULL);
	}
}
///////////////////////////////////////////////////////////////////////////////

TAnimDepth::TAnimDepth(TFrames * parentDepth, unsigned short index)
{
	placeObject_ = NULL;
	parentSprite_ = parentDepth;
	visualAttrib_.setOpacity(1.0f);
	visualAttrib_.setVisible(true);
	index_ = index;
}

void TAnimDepth::setPlaceObject(TPlaceObject * placeObject)
{
	if (placeObject_)
	{
		TAnimSprite * sprite = placeObject_->object()->sprite();
		if (sprite) sprite->setParentDepth(NULL);
	}
	placeObject_ = placeObject;
	if (placeObject_)
	{		
		TAnimSprite * sprite = placeObject_->object()->sprite();
		if (sprite) sprite->setParentDepth(this);
	}
	
}

void TAnimDepth::setParentSprite(TFrames * parent)
{
	parentSprite_ = parent;
}

TFrames * TAnimDepth::parentSprite() const
{
	return parentSprite_;
}
///////////////////////////////////////////////////////////////////////////////

TAnimationObject::TAnimationObject()
{
	create();
	setId("TAnimation");
}

TAnimationObject::TAnimationObject(
	const TAnimationObject & rhs, ElementMapping * elementMapping)
	: SceneObject(rhs, elementMapping), TFrames(rhs)
{
	create();
	visualAttrib_ = rhs.visualAttrib_;
	loadSpeed_ = rhs.loadSpeed_;
	animDataFileName_ = rhs.animDataFileName_;	
	actualFileNames_ = rhs.actualFileNames_;
}

TAnimationObject::~TAnimationObject()
{
	uninit();
}

void TAnimationObject::create()
{
	duration_ = 0.0f;
	isPlaying_ = false;
	width_ = height_ = 0;
	needsMask_ = false;
	
	//in kbytes per second
	loadSpeed_ = 256.0f;
}

void TAnimationObject::stop()
{
	isPlaying_ = false;
	curFrameIndex_ = 0;
	curRep_ = 0;
	curLoadCueIndex_ = 0;
	stopSounds();
}

void TAnimationObject::play(float time)
{
	stop();
	curFrameIndex_ = 0;
	curRep_ = 0;
	curLoadCueIndex_ = 0;
	TFrames::play(this, time);
}

void TAnimationObject::reset()
{	
	stop();
	doFirstFrame(this, startTime_);	
}

void TAnimationObject::init(GfxRenderer * gl, bool firstTime)
{	
	SceneObject::init(gl, firstTime);
	//DEBUGSCENE = parentScene_;
	loadXml(animDataFileName_);
	computeLoadCues();

	for (int i = 0; i < (int)loadCues_.size(); ++i)
	{
		TAnimObject * obj = loadCues_[i].placeObject()->object();	
	}

	if(needsMask_) gl->registerMaskObject(this);
	
	
	

	preloadObjs_.clear();
	curLoadCueIndex_ = 0;
	for (int i = 0; i < (int)loadCues_.size(); ++i)
	{
		if (loadCues_[i].time() == 0.0f)
		{
			TPlaceObject * placeObject = loadCues_[i].placeObject();
			preloadObjs_.push_back(placeObject);
			placeObject->setKeepLoaded(true);
			placeObject->load(gl, this);
		}
		else break;
	}

	loadObjs_.clear();	
	doFirstFrame(this, Global::currentTime());
}

void TAnimationObject::uninit()
{		
	if (!gl_) return;

	gl_->unregisterMaskObject(this);

	unloadXml();
	
	depths_.clear();
	preloadObjs_.clear();
	loadObjs_.clear();
	loadCues_.clear();
	SceneObject::uninit();
}

void TAnimationObject::unloadXml()
{
	defineObjects_.clear();
	frames_.clear();	
}


bool TAnimationObject::update(float sceneTime)
{	
	if (!isPlaying_) return false;

	//sets animTime_
	bool didRepeat = updateFrame(this, sceneTime);	

	//DEBUGTIME = animTime_;
	//cout << DEBUGTIME << "\n";

	if (didRepeat) curLoadCueIndex_ = 0;

	for (; curLoadCueIndex_ < loadCues_.size(); curLoadCueIndex_++)
	{
		if (loadCues_[curLoadCueIndex_].time() == 0.0f) continue;
		if (loadCues_[curLoadCueIndex_].time() <= animTime_)			
		{
			if (animTime_ <= loadCues_[curLoadCueIndex_].endTime())
			{
				TPlaceObject * placeObject = loadCues_[curLoadCueIndex_].placeObject();
				loadObjs_.push_back(placeObject);
				placeObject->load(gl_, this);
			}			
		}
		else break;
	}
	

	return true;
}

bool TAnimationObject::asyncLoadUpdate() 
{
	bool everythingLoaded = SceneObject::asyncLoadUpdate();

	for (int i = 0; i < (int)preloadObjs_.size(); ++i)
	{
		preloadObjs_[i]->asyncLoadUpdate();
	}

	list<TPlaceObject *>::iterator itr = loadObjs_.begin();
	for (;itr != loadObjs_.end();)
	{
		TPlaceObject * placeObject = *itr;
		placeObject->asyncLoadUpdate();
		if (placeObject->isLoaded())
		{
			loadObjs_.erase(itr++);
		}
		else ++itr;
	}

	return everythingLoaded;
}

bool TAnimationObject::isLoaded() const
{
	if (!gl_) return false;
	bool ret = true;
	
	for (int i = 0; i < (int)preloadObjs_.size(); ++i)
	{
		ret &= preloadObjs_[i]->isLoaded();			
	}
	
	return ret;
}

void TAnimationObject::drawObject(GfxRenderer * gl) const
{	
	gl->beginIncStencilDrawing();
	gl->useColorProgram();
	gl->setColorProgramColor(0,0,0,0);
	gl->drawRect(0, 0, width_, height_);
	gl->endStencilDrawing();
		
	drawDepths(gl, const_cast<TAnimationObject *>(this));

	gl->beginDecStencilDrawing();
	gl->useColorProgram();
	gl->setColorProgramColor(0,0,0,0);
	gl->drawRect(0, 0, width_, height_);
	gl->endStencilDrawing();
}

void TAnimationObject::start(float docTime)
{
	SceneObject::start(docTime);	
}

void TAnimationObject::preStart(float docTime)
{
	SceneObject::preStart(docTime);
}

BoundingBox TAnimationObject::extents() const
{
	return BoundingBox(Vector3(0, 0, 0), Vector3(width_, height_, 0));
	return BoundingBox();
}

void TAnimationObject::remapReferences(const ElementMapping & elementMapping)
{
	SceneObject::remapReferences(elementMapping);
}

void TAnimationObject::referencedFiles(std::vector<std::string> * refFiles) const
{	
	SceneObject::referencedFiles(refFiles);

	bool wasInit = isInit();

	refFiles->push_back(animDataFileName_);
	
	if (!wasInit)
	{
		const_cast<TAnimationObject *>(this)->loadXml(animDataFileName_);
	}
	
	map<string, string>::const_iterator itr;
	for (itr = actualFileNames_.begin(); itr != actualFileNames_.end(); ++itr)
	{		
		refFiles->push_back((*itr).second);
	}	

	if (!wasInit)
	{
		const_cast<TAnimationObject *>(this)->unloadXml();
	}
}

int TAnimationObject::setReferencedFiles(
	const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);

	animDataFileName_ = refFiles[index++];

	bool wasInit = isInit();

	if (!wasInit)
	{
		const_cast<TAnimationObject *>(this)->loadXml(animDataFileName_);
	}


	map<string, string>::iterator itr;
	for (itr = actualFileNames_.begin(); itr != actualFileNames_.end(); ++itr)
	{	
		(*itr).second = refFiles[index++];		
	}	

	if (!wasInit)
	{
		const_cast<TAnimationObject *>(this)->unloadXml();
	}

	return index;
}

bool TAnimationObject::dependsOn(Scene * scene) const
{
	return false;
}

void TAnimationObject::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	
	writer.write(animDataFileName_);
	writer.write(loadSpeed_);
	writer.write(actualFileNames_);
	writer.write(repeat_);
}

void TAnimationObject::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	if (version >= 1)
	{
		reader.read(animDataFileName_);
		reader.read(loadSpeed_);
	}	

	if (version >= 2)
	{
		reader.read(actualFileNames_);
	}

	if (version >= 3)
	{
		reader.read(repeat_);
	}
}

void TAnimationObject::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	w.writeTag("AnimDataFileName", animDataFileName_);
	w.writeTag("LoadSpeed", loadSpeed_);
	std::map<std::string, std::string>::const_iterator it;
	for(it = actualFileNames_.begin() ; it != actualFileNames_.end() ; it++)
	{
		string attrStr = "Key = \"" + it->first + "\"";
		w.writeTag("ActualFileNames", attrStr, it->second);
	}
}

void TAnimationObject::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);
	std::string value;
	std::string key = "";
	actualFileNames_.clear();

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;		
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(animDataFileName_, curNode, "AnimDataFileName"));
		else if (r.getNodeContentIfName(loadSpeed_, curNode, "LoadSpeed"));
		else if (r.getNodeContentIfName(value, curNode, "ActualFileNames"))
		{
			r.getNodeAttribute(key, curNode, "Key");
			if (key.empty())
				throw XmlException(parent, "No Key");
			actualFileNames_[key] = value;
		}
	}
}




/**
get greatest index for which loadCues_[index].time() < time
*/
int TAnimationObject::getLoadCueIndex(float time)
{
	if (loadCues_.empty()) return -1;
	
	int index0, index1;
	index0 = 0;
	index1 = loadCues_.size() - 1;


	if (loadCues_[index1].time() < time) return index1;

	while (index1 - index0 > 1)
	{
		int indexMid = (index0 + index1) / 2;

		if (loadCues_[indexMid].time() < time)
		{
			index0 = indexMid;
		}
		else
		{
			index1 = indexMid;
		}
	}


	if (loadCues_[index0].time() < time) return index0;
	else return -1;
}

void TAnimationObject::addNewLoadCue(float time, TPlaceObject * placeObject)
{
	float loadTime = placeObject->object()->loadSize() / (loadSpeed_ * 1024.0f);
	if (loadTime < 0.1f) loadTime = 0.1f;
	//REMOVE REMOVE REMOVE!!
	//loadTime = 9999999999.9f;
	TLoadCue loadCue;
	loadCue.setPlaceObject(placeObject);

	if (loadCues_.empty())
	{	
		float loadStartTime = time - loadTime;
		if (loadStartTime < 0.0f) loadStartTime = 0.0f;
		loadCue.setTime(loadStartTime);
		loadCues_.push_back(loadCue);
		return;
	}
	
	int index = getLoadCueIndex(time);

	if (index < 0)
	{
		float loadStartTime = time - loadTime;
		if (loadStartTime < 0.0f) loadStartTime = 0.0f;
		loadCue.setTime(loadStartTime);
		loadCues_.push_front(loadCue);
		//loadCues_.insert(loadCues_.begin(), loadCue);
		return;
	}

	bool loadCueAdded = false;
	float curTime = time;
	for (int i = index; i >= 0; --i)
	{
		float loadEndTime = loadCues_[i].time() + loadTime;
		float curStartTime = curTime - loadTime;
		if (curStartTime < 0.0f) curStartTime = 0.0f;
		if (loadEndTime < curStartTime)
		{
			loadCue.setTime(curStartTime);
			loadCues_.insert(loadCues_.begin() + i + 1, loadCue);
			loadCueAdded = true;
			break;
		}		

		curTime = loadCues_[i].time();
	}

	if (!loadCueAdded)
	{
		loadCue.setTime(0.0f);
		loadCues_.push_front(loadCue);
		//loadCues_.insert(loadCues_.begin(), loadCue);
	}
}

void TAnimationObject::computeLoadCues()
{
	std::vector<TPlaceObject *> depths;
	map<TPlaceObject *, float> endTimes;

	loadCues_.clear();

	//calculate endTimes
	for (int i = 0; i < (int)frames_.size(); ++i)
	{
		float time = frames_[i]->time();
		const vector<TAnimTagSPtr> & tags = frames_[i]->tags();
		for (int j = 0; j < (int)tags.size(); ++j)
		{
			TPlaceObject * placeObject = tags[j]->placeObject();
			TRemoveObject * removeObject = tags[j]->removeObject();

			if (placeObject && placeObject->object())
			{
				unsigned short depth = placeObject->depth();
				while (depth >= depths.size()) depths.push_back(NULL);

				TPlaceObject * prevPlaceObj = depths[depth];
				if (prevPlaceObj) endTimes[prevPlaceObj] = time;
				depths[depth] = placeObject;
			}
			else if (removeObject)
			{
				unsigned short depth = removeObject->depth();
				while (depth >= depths.size()) depths.push_back(NULL);
				endTimes[depths[depth]] = time;
			}	
		}
	}


	for (int i = (int)frames_.size() - 1; i >= 0; --i)
	{
		float time = frames_[i]->time();
		const vector<TAnimTagSPtr> & tags = frames_[i]->tags();
		for (int j = (int)tags.size() - 1; j >= 0; --j)
		{
			TPlaceObject * placeObject = tags[j]->placeObject();

			if (placeObject && placeObject->object())
			{
				addNewLoadCue(time, placeObject);
			}					
		}
	}

	for (unsigned i = 0; i < (unsigned)loadCues_.size(); ++i)
	{
		map<TPlaceObject *, float>::iterator itr = endTimes.find(loadCues_[i].placeObject());
		if (itr == endTimes.end())
			loadCues_[i].setEndTime(duration_);
		else
			loadCues_[i].setEndTime((*itr).second);
	}

}


template <typename T>
bool getXmlNodeContent(T & val, xmlNode * node, const char * name)
{
	if (!node) return false;
	if (!xmlStrEqual(node->name, (xmlChar *)name)) return false;
	if (node->type != XML_ELEMENT_NODE) return false;
	if (node->children->type != XML_TEXT_NODE) return false;
	
	try 
	{
		val = lexical_cast<T>(node->children->content);
	} catch(...) 
	{
		return false;
	}

	return true;
}

template <typename T>
bool setXmlNodeName(T & val, xmlNode * node, const char * newname)
{
	if (!node) return false;
	if (node->type != XML_ELEMENT_NODE) return false;
	if (node->children->type != XML_TEXT_NODE) return false;
		
	try 
	{
		node->children->content = (xmlChar *)newname;
	} catch(...) 
	{
		return false;
	}

	return true;
}

void loadImgRegionXml(xmlNode * nodeParent, string & fileName, int & x, int & y)
{
	string coordStr;
	for (xmlNode * node = nodeParent->children; node; node = node->next)
	{
		if (node->type != XML_ELEMENT_NODE) continue;
		if (getXmlNodeContent(fileName, node, "fileName")) continue;
		if (getXmlNodeContent(coordStr, node, "coords")) 
		{
			char_separator<char> sep(", ");
			tokenizer < char_separator<char> > tokens(coordStr, sep);
			vector<string> tokenStrs(tokens.begin(), tokens.end());
			if (tokenStrs.size() >= 2)
			{				
				try {
					x = lexical_cast<int>(tokenStrs[0]);
					y = lexical_cast<int>(tokenStrs[1]);		
				} catch(...) {}
			}
			continue;	
		}		
	}
}

const std::string & TAnimationObject::actualFileName(const std::string & fileName) const
{
	map<string, string>::const_iterator itr = actualFileNames_.find(fileName);
	if (itr == actualFileNames_.end()) return fileName;
	else return (*itr).second;	
}

TPlaySound * TAnimationObject::loadXmlPlaySound(xmlNode * nodeParent)
{
	std::string fileName;

	for (xmlNode * node = nodeParent->children; node; node = node->next)
	{
		if (node->type != XML_ELEMENT_NODE) continue;

		if (getXmlNodeContent(fileName, node, "fileName")) 
		{			
			actualFileNames_[fileName] = getAbsFileName(directory_, fileName);
			fileNames_.push_back( getFileNameWithoutDirectory( actualFileNames_[fileName] ) );
			continue;	
		}	
	}

	
	TPlaySound * ret;
	ret = new TPlaySound;
	ret->setFileName(fileName);
	return ret;
}

/**
<DefineSprite>
	<id>3</id>
	<duration>34.4</duration>
	<Frame>
		<time>3.4</time>
		<Place>
		...
		</Place>
		...
	</Frame>
	....
</DefineSprite>
*/
void TAnimationObject::loadXmlDefineSprite(xmlNode * nodeParent)
{
	bool hasId = false;
	unsigned short id = 0;
	
	TAnimSprite * sprite = new TAnimSprite;

	float duration = -1.0f;

	for (xmlNode * node = nodeParent->children; node; node = node->next)
	{
		if (node->type != XML_ELEMENT_NODE) continue;
		if (getXmlNodeContent(id, node, "id")) 
		{
			hasId = true;
			continue;	
		}
		else if (getXmlNodeContent(duration, node, "duration")) continue;
		else if (xmlStrEqual(node->name, (xmlChar *)"frame"))
		{
			TAnimFrame * frame = loadXmlFrame(node);
			if (frame) sprite->addFrame(TAnimFrameSPtr(frame));
		}

	}

	if (!hasId || duration < 0.0f)
	{
		delete sprite;
		return;
	}

	sprite->setId(id);
	sprite->setDuration(duration);
	defineObjects_[id] = TAnimObjectSPtr(sprite);	
}

/*
void loadImgRegionXml(xmlNode * nodeParent, string & fileName, int & x, int & y)
{
	string coordStr;
	for (xmlNode * node = nodeParent->children; node; node = node->next)
	{
		if (node->type != XML_ELEMENT_NODE) continue;
		if (getXmlNodeContent(fileName, node, "fileName")) continue;
		if (getXmlNodeContent(coordStr, node, "coords")) 
		{
			char_separator<char> sep(", ");
			tokenizer < char_separator<char> > tokens(coordStr, sep);
			vector<string> tokenStrs(tokens.begin(), tokens.end());
			if (tokenStrs.size() >= 2)
			{				
				try {
					x = lexical_cast<int>(tokenStrs[0]);
					y = lexical_cast<int>(tokenStrs[1]);		
				} catch(...) {}
			}
			continue;	
		}		
	}
}
*/

/**
<DefineImage>
	<id>3</id>
	<fileName>abc.jpg</fileName>	
</DefineImage>
*/
void TAnimationObject::loadXmlDefineImage(xmlNode * nodeParent)
{
	
	bool hasId = false;
	unsigned short id = 0;
	
	string matrixString;
	Matrix matrix = Matrix::Identity();

	std::string curFileName;
	std::vector< std::string> fileNames;
	std::vector< std::pair<int, int> > coords;
	
	for (xmlNode * node = nodeParent->children; node; node = node->next)
	{
		if (node->type != XML_ELEMENT_NODE) continue;
		if (getXmlNodeContent(curFileName, node, "fileName")) 
		{
			actualFileNames_[curFileName] = getAbsFileName(directory_, curFileName);			
			fileNames.push_back(actualFileNames_[curFileName]);
			//file stack
			fileNames_.push_back( getFileNameWithoutDirectory( actualFileNames_[curFileName] ) );

			coords.push_back( make_pair(0, 0) );
			
			continue;	
		}
		
		if (xmlStrEqual(node->name, (xmlChar *)"region"))
		{

			string fileName;
			int x = 0, y = 0;
			loadImgRegionXml(node, fileName, x, y);
			actualFileNames_[fileName] = getAbsFileName(directory_, fileName);

			if (!fileName.empty())
			{
				fileNames.push_back(actualFileNames_[fileName]);
				coords.push_back( make_pair(x, y) );
			}
		}

		if (getXmlNodeContent(id, node, "id")) 
		{
			hasId = true;
			continue;	
		}
		if (getXmlNodeContent(matrixString, node, "matrix"))
		{
			char_separator<char> sep(", ");
			tokenizer < char_separator<char> > tokens(matrixString, sep);
			vector<string> tokenStrs(tokens.begin(), tokens.end());
			if (tokenStrs.size() >= 6)
			{				
				try {
					float scaleX = lexical_cast<float>(tokenStrs[0]);
					float rotateSkew0 = lexical_cast<float>(tokenStrs[1]);
					float rotateSkew1 = lexical_cast<float>(tokenStrs[2]);
					float scaleY = lexical_cast<float>(tokenStrs[3]);
					float dx = lexical_cast<float>(tokenStrs[4]);
					float dy = lexical_cast<float>(tokenStrs[5]);

					matrix._11 = scaleX;
					matrix._22 = scaleY;

					matrix._12 = rotateSkew1;
					matrix._21 = rotateSkew0;

					matrix._14 = dx;	
					matrix._24 = dy;					
				} catch(...) {}
			}
		}
	}

	if (coords.size() > 1)
	{
		int a = 4;
	}
	if (hasId && !fileNames.empty())
	{
		long cumFileSize = 0;

		for (int i = 0; i < (int)fileNames.size(); ++i)
		{
			FILE * file = fopen(fileNames[i].c_str(), "rb");
			if (!file) return;
			fseek(file, 0, SEEK_END);	
			cumFileSize += ftell(file);
			fclose(file);
		}
		
		TAnimImage * image = new TAnimImage();
		image->setFileNames(fileNames, coords);
		image->setFileSize(cumFileSize);
		image->setMatrix(matrix);
		image->setId(id);
		defineObjects_[id] = TAnimObjectSPtr(image);
	}
}

/**
<Place>
	<id>3</id>
	<depth>4</depth>
	<matrix>1, 2, 3, 4, 5, 6, 7</matrix>
	<opacity>1</opacity>
	<visibility>true</visibility>
</Place>
*/
TPlaceObject * TAnimationObject::loadXmlPlaceObject(xmlNode * nodeParent)
{
	unsigned short id = 0;
	unsigned short depth = 0;
	bool hasId = false, hasDepth = false;
	bool hasMatrix = false, hasOpacity = false, hasVisibility = false;

	string matrixString;
	Matrix matrix = Matrix::Identity();

	float opacity = 1.0f;

	string visibilityString;
	bool visibility;

	unsigned short clipDepth = 0;

	TPlaceObject * ret = NULL;

	for (xmlNode * node = nodeParent->children; node; node = node->next)
	{
		if (node->type != XML_ELEMENT_NODE) continue;
		if (getXmlNodeContent(id, node, "id")) 
		{
			hasId = true;
			continue;
		}
		if (getXmlNodeContent(depth, node, "depth")) 
		{
			hasDepth = true;
			continue;
		}
		if (getXmlNodeContent(clipDepth, node, "clipDepth")) 
		{
			needsMask_ = true;
			continue;
		}
		if (getXmlNodeContent(opacity, node, "opacity"))
		{
			hasOpacity = true;
			continue;
		}
		if (getXmlNodeContent(matrixString, node, "matrix"))
		{
			char_separator<char> sep(", ");
			tokenizer < char_separator<char> > tokens(matrixString, sep);
			vector<string> tokenStrs(tokens.begin(), tokens.end());
			bool suc = true;

			if (tokenStrs.size() >= 6)
			{				
				try {
					float scaleX = lexical_cast<float>(tokenStrs[0]);
					float rotateSkew0 = lexical_cast<float>(tokenStrs[1]);
					float rotateSkew1 = lexical_cast<float>(tokenStrs[2]);
					float scaleY = lexical_cast<float>(tokenStrs[3]);
					float dx = lexical_cast<float>(tokenStrs[4]);
					float dy = lexical_cast<float>(tokenStrs[5]);
					
					matrix._11 = scaleX;
					matrix._22 = scaleY;

					matrix._12 = rotateSkew1;
					matrix._21 = rotateSkew0;

					matrix._14 = dx;	
					matrix._24 = dy;					
				} catch(...) {suc = false;}

				if (suc)
				{
					hasMatrix = true;
				}
			}			
		}
		if (getXmlNodeContent(visibilityString, node, "visibility"))
		{
			hasVisibility = true;
			continue;
		}
	}

	
	if (hasId) 
	{
		bool objectFound = false;
		if (defineObjects_.find(id) == defineObjects_.end()) return NULL;		
	}

	if (
		(hasId && hasDepth) ||
		(hasDepth && (hasOpacity || hasVisibility || hasMatrix))
		)
	{
		ret = new TPlaceObject;
		if (hasId) ret->setObject(*defineObjects_[id]);
		if (hasDepth) ret->setDepth(depth);
		if (hasMatrix) ret->setMatrix(matrix);
		if (hasOpacity) ret->setOpacity(opacity);
		if (hasVisibility) ret->setVisibility(visibility);
		ret->setClipDepth(clipDepth);
	}

	return ret;
}

/**
<Remove>
	<depth>4</depth>	
</Remove>
*/
TRemoveObject * TAnimationObject::loadXmlRemoveObject(xmlNode * nodeParent)
{
	bool hasDepth = false;
	unsigned short depth = 0;

	for (xmlNode * node = nodeParent->children; node; node = node->next)
	{
		if (node->type != XML_ELEMENT_NODE) continue;
		if (getXmlNodeContent(depth, node, "depth")) 
		{
			hasDepth = true;
			continue;
		}		
	}

	if (!hasDepth) return NULL;

	TRemoveObject * ret = new TRemoveObject;
	ret->setDepth(depth);
	return ret;
}

TAnimFrame * TAnimationObject::loadXmlFrame(xmlNode * nodeParent)
{	
	TAnimFrame * frame = new TAnimFrame;
	float time = -1.0f;

	std::vector<TPlaySound *> playSounds;

	for (xmlNode * node = nodeParent->children; node; node = node->next)
	{
		if (node->type != XML_ELEMENT_NODE) continue;
		if (getXmlNodeContent(time, node, "time")) continue;
		if (xmlStrEqual(node->name, (xmlChar *)"PlaceObject"))
		{
			TPlaceObject * ret = loadXmlPlaceObject(node);
			if (ret)		
				frame->addTag(TAnimTagSPtr(ret));
		}
		else if (xmlStrEqual(node->name, (xmlChar *)"PlaySound"))
		{
			TPlaySound * ret = loadXmlPlaySound(node);
			if (ret)
			{
				playSounds.push_back(ret);
				frame->addTag(TAnimTagSPtr(ret));
			}
		}
		else if (xmlStrEqual(node->name, (xmlChar *)"RemoveObject"))
		{
			TRemoveObject * ret = loadXmlRemoveObject(node);
			if (ret)
				frame->addTag(TAnimTagSPtr(ret));
		}
	}

	if (time < 0.0f)
	{
		delete frame;
		return NULL;
	}
	
	BOOST_FOREACH(TPlaySound * playSound, playSounds)
	{
		playSound->setTime(time);
	}

	frame->setTime(time);
	
	return frame;
}


bool TAnimationObject::exchangeXmltext(const std::string & fileName, const std::string & origfile, std::vector<std::string> &targettexts, const std::string & newtext)
{
	directory_ = getDirectory(getAbsFileName(Global::instance().readDirectory(), animDataFileName_));
	hasActualFileNames_ = !actualFileNames_.empty();

	xmlDocPtr doc = NULL;

	BOOST_SCOPE_EXIT( (&doc)) {
		if (doc) xmlFreeDoc(doc);
	} BOOST_SCOPE_EXIT_END

	doc = xmlReadFile(fileName.c_str(), NULL, 0);
	if (!doc) return false;


	xmlNode * root = xmlDocGetRootElement(doc);
	if (!root) return false;

	BOOST_FOREACH(std::string targettext, targettexts)
	{	
		if(!origfile.compare(targettext))
		{

			for (xmlNode * node = root->children; node; node = node->next)
			{
				std::string curFileName;

				if(xmlStrEqual(node->name, (xmlChar *)"DefineImage"))
				{			
					for (xmlNode * node_child = node->children; node_child; node_child = node_child->next)
					{		
						if (node_child->type != XML_ELEMENT_NODE) continue;
						if (getXmlNodeContent(curFileName, node_child, "fileName")) 
						{					
							if(!curFileName.compare(targettext))
							{																					
								xmlNodeSetContent(node_child, (xmlChar *)newtext.c_str());									
								continue;	
							}					
						}		
					}	
				}
				else if(xmlStrEqual(node->name, (xmlChar *)"frame"))
				{
					for (xmlNode * node_child = node->children; node_child; node_child = node_child->next)
					{

						if(xmlStrEqual(node->name, (xmlChar *)"PlaySound"))
						{			
							for (xmlNode * node_chchild = node_child->children; node_chchild; node_chchild = node_chchild->next)
							{		
								if (node_child->type != XML_ELEMENT_NODE) continue;
								if (getXmlNodeContent(curFileName, node_chchild, "fileName")) 
								{					
									if(!curFileName.compare(targettext))
									{																					
										xmlNodeSetContent(node_chchild, (xmlChar *)newtext.c_str());									
										continue;	
									}					
								}		
							}	
						}
					}
				}
			}
		}
	}
	xmlSaveFile(fileName.c_str(), doc);
	//xmlSaveFormatFileEnc(fileName.c_str(), doc, "UTF-8", 1);

	return true;
}

/**
<Root>
	<width>340</width>
	<height>480</height>
	<duration>34.4</duration>
	<Frame>
		<time>3.4</time>
		<Place>
		...
		</Place>
		...
	</Frame>
	....
</Root>
*/
bool TAnimationObject::loadXml(const std::string & fileName)
{
	directory_ = getDirectory(getAbsFileName(Global::instance().readDirectory(), animDataFileName_));
	hasActualFileNames_ = !actualFileNames_.empty();

	xmlDocPtr doc = NULL;

	BOOST_SCOPE_EXIT( (&doc)) {
		if (doc) xmlFreeDoc(doc);
	} BOOST_SCOPE_EXIT_END

		int rc = 0;

	doc = xmlReadFile(fileName.c_str(), NULL, 0);
	if (!doc) return false;


	xmlNode * root = xmlDocGetRootElement(doc);
	if (!root) return false;

	actualFileNames_.clear();
	frames_.clear();	
	duration_ = -1.0f;
	isPlaying_ = false;
	curFrameIndex_ = 0;
	//repeat_ = true;
	curRep_ = 0;
	needsMask_ = false;

	bool hasWidth = false, hasHeight = false;

	for (xmlNode * node = root->children; node; node = node->next)
	{
		if (node->type != XML_ELEMENT_NODE) continue;
		if (getXmlNodeContent(width_, node, "width")) continue;
		if (getXmlNodeContent(height_, node, "height"))
		{
			hasWidth = true;
		}
		if (getXmlNodeContent(duration_, node, "duration"))
		{
			hasHeight = true;
		}
		else if (xmlStrEqual(node->name, (xmlChar *)"frame"))
		{
			TAnimFrame * frame = loadXmlFrame(node);
			if (frame) addFrame(TAnimFrameSPtr(frame));
		}	
		else if (xmlStrEqual(node->name, (xmlChar *)"DefineSprite"))
		{
			loadXmlDefineSprite(node);
		}
		else if (xmlStrEqual(node->name, (xmlChar *)"DefineImage"))
		{
			loadXmlDefineImage(node);
		}
	}

	if (duration_ < 0.0f || !hasWidth || !hasHeight) return false;

	return true;
}


void TAnimationObject::setAnimDataFileName(const std::string & animDataFileName)
{
	if (animDataFileName == animDataFileName_) return;
	animDataFileName_ = animDataFileName;
	actualFileNames_.clear();
}


SceneObject * TAnimationObject::intersect(Vector3 * intPt, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;
	if (intersectRect(intPt, ray)) return this;
	else return 0;
}

bool TAnimationObject::intersectRect(Vector3 * out, const Ray & ray)
{
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();

	Vector3 a = trans * Vector3(0, 0, 0);
	Vector3 b = trans * Vector3(0, height(), 0);
	Vector3 c = trans * Vector3(width(), height(), 0);
	Vector3 d = trans * Vector3(width(), 0, 0);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return true;
	}
	else return false;
}