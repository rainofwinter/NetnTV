#include "stdafx.h"
#include "InternalSubscene.h"
#include "SingleSceneDocumentTemplate.h"
#include "Document.h"
#include "Scene.h"
#include "Reader.h"
#include "Writer.h"
#include "Xml.h"
#include "TempRenderObject.h"
#include "GfxRenderer.h"
#include "Camera.h"
#include "Global.h"
#include "LoadingPage.h"
#include "VideoPlayer.h"
#include "AudioPlayer.h"

#include "SceneChanger.h"
#include "ImageChanger.h"

using namespace std;

SingleSceneDocumentTemplate::SingleSceneDocumentTemplate()
{
	displayingAppObjects_ = false;
	isLoaded_ = false;
	redrawTriggered_ = false;
	//loading page
	loadingPage_.reset(new LoadingScreen);
	subscene_.reset(new InternalSubscene);
	curPageStarted_ = false;

	width_ = 768;
	height_ = 1024;

	isSceneCreated_ = false;
	sceneCheck_ = false;

	resetComplete_ = false;
	searchQ_.clear();
}

SingleSceneDocumentTemplate::~SingleSceneDocumentTemplate()
{
	uninit();
}

bool SingleSceneDocumentTemplate::isCurrentScene(Scene * scene) const
{
	//return scene_ && (scene_ == scene || scene_->isShowing(scene));
	return subscene_->scene() && (subscene_->scene() == scene || subscene_->scene()->isShowing(scene));
}

void SingleSceneDocumentTemplate::init(GfxRenderer * gl)
{
	subscene_->setMode(InternalSubscene::ModeFixed);
	subscene_->setLocked(true);		
	subscene_->init(gl, true);
	//scene_->init(gl);

	/*
	Commenting out:
	bug fix: In the editor, when changing the SingleSceneDocumentTemplate scene
	to the current scene, the camera changes so that the scene fills up the 
	whole canvas
	*/
	//scene_->resetCamera(scene_->screenWidth() / scene_->screenHeight());

	//loadingPage_->init(gl, scene_->screenWidth(), scene_->screenHeight());
#ifdef WATERMARK
	sceneChangerCheck(scene());
	sceneCheck_ = true;
	sceneChangerCheck(scene());
#endif
}

void SingleSceneDocumentTemplate::sceneChangerCheck(Scene * scene)
{
	std::vector<SceneObject *> sceneObject;
	boost::uuids::uuid type = SceneChanger().type();
	scene->getSceneObjectsByID(type, &sceneObject);
	if(!sceneObject.empty())
	{
		BOOST_FOREACH(SceneObject * obj, sceneObject)
		{				
			SceneChanger* sceneChanger = (SceneChanger*)obj;
			if(!sceneChanger->scenes().empty())
				if(!sceneCheck_)
					sceneChangerCheck(sceneChanger->scenes()[0]);
				else if(sceneCheck_)
					sceneChangerCheck(sceneChanger->scenes().back());
			else 
				imageChangerCheck(scene);		
		}
	}else{
		scene->waterMark();
	}
}

void SingleSceneDocumentTemplate::imageChangerCheck(Scene * scene)
{
	std::vector<SceneObject *> sceneObject;
	static boost::uuids::uuid type = ImageChanger().type();
	scene->getSceneObjectsByID(type, &sceneObject);
	if(!sceneObject.empty())
	{
		BOOST_FOREACH(SceneObject * obj, sceneObject)
		{
			ImageChanger* imageChanger = (ImageChanger *)obj;
			if(!imageChanger->fileNames().empty())
			{
				for (int j = 0 ; j <imageChanger->pageIndex() ; j++)
				{
					if(!sceneCheck_)
					{
						if (j == 0)
							scene->waterMark();
					} else if(sceneCheck_){
						if(j == imageChanger->pageIndex())
							scene->waterMark();
					}
				}
			}
			else
				scene->waterMark();
		}
	}else{
		scene->waterMark();
	}
}

void SingleSceneDocumentTemplate::uninit()
{
	loadingPage_->uninit();
	subscene_->uninit();
	//if (scene_) scene_->uninit();
	
}

void SingleSceneDocumentTemplate::setScene(Scene * scene)
{
	subscene_->setScene(scene);
}

void SingleSceneDocumentTemplate::curSceneAppObjectStart() const
{
	if (!displayingAppObjects_)
	{
		//scene_->initAppObjects();
		subscene_->initAppObjects();
		displayingAppObjects_ = true;
	}
}

void SingleSceneDocumentTemplate::curSceneAppObjectStop() const
{
	if (displayingAppObjects_)
	{
		//scene_->uninitAppObjects();
		subscene_->uninitAppObjects();
		displayingAppObjects_ = false;
	}
}

void SingleSceneDocumentTemplate::sceneChangeTo(Scene * scene)
{
	//if (scene_) 
	if (subscene_->scene())
	{
		//stop any videos or audio that were playing on the previous page
		Global::instance().videoPlayer()->deactivate();
		Global::instance().audioPlayer()->stopAll();
	//	Global::instance().cameraOff();
		
		curSceneAppObjectStop();

		//scene_->stop();
		//scene_->uninit();		
		subscene_->stop();
		subscene_->uninit();
	}
	//scene->init(document_->renderer());
	subscene_->setScene(scene);
	subscene_->init(document_->renderer(), true);
	isLoaded_ = false;
	curPageStarted_ = false;		
}

void SingleSceneDocumentTemplate::draw(GfxRenderer * gl) const
{
	//const Color & bgColor = scene_->bgColor();
	const Color & bgColor = subscene_->scene()->bgColor();
#ifdef DRAW_SCENE_BACKGROUND
	glClearColor(bgColor.r, bgColor.g, bgColor.b, 1);
#endif
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | Global::extraGlClearOptions());
	gl->resetStencil();

	if (isLoaded_)
	{
		//gl->setCameraMatrix(scene_->camera()->GetMatrix());
		//gl->setCameraMatrix(subscene_->scene()->camera()->GetMatrix());
		gl->setCameraMatrix(camera_.GetMatrix());
		gl->loadMatrix(Matrix::Identity());

		if (!curPageStarted_)
		{
			subscene_->start(Global::currentTime());
			curSceneAppObjectStart();			
			curPageStarted_ = true;
		}

		//draw scene background
		//const Color & bgColor = scene_->bgColor();
		const Color & bgColor = subscene_->scene()->bgColor();
#ifdef DRAW_SCENE_BACKGROUND
		glClearColor(bgColor.r, bgColor.g, bgColor.b, 1);
#endif
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | Global::extraGlClearOptions());

		subscene_->draw(gl);
		//scene_->draw(gl);
	
	BOOST_FOREACH(TempRenderObject * obj, document_->tempRenderObjects())
		obj->draw(gl);
	}
	else
	{
		gl->setCameraMatrix(loadingCamera_.GetMatrix());
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		loadingPage_->draw(gl, 0, 0);
		document_->triggerRedraw();		
	}

	//glClear(GL_DEPTH_BUFFER_BIT);	
	//glClearColor(0, 0.5, 0.5, 1);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	//gl->drawRect(0, 0, 512, 512);	

	//glFlush();
}


bool SingleSceneDocumentTemplate::clickEvent(const Vector2 & mousePos, int pressId)
{
	if (!propagate_[pressId]) return false;
	//return subscene_->clickEvent(toLocalCoords(mousePos), pressId);
	return subscene_->clickEvent(mousePos, pressId);
}

bool SingleSceneDocumentTemplate::doubleClickEvent(const Vector2 & mousePos, int pressId)
{	
	if (!propagate_[pressId]) return false;
	//return subscene_->doubleClickEvent(toLocalCoords(mousePos), pressId);
	return subscene_->doubleClickEvent(mousePos, pressId);
}

bool SingleSceneDocumentTemplate::pressEvent(const Vector2 & startPos, int pressId, bool propagate)
{
	propagate_[pressId] = propagate;
	if (!propagate) return false;
	//return subscene_->pressEvent(toLocalCoords(startPos), pressId);
	return subscene_->pressEvent(startPos, pressId);
}

bool SingleSceneDocumentTemplate::moveEvent(const Vector2 & pos, int pressId)
{
	if (!propagate_[pressId]) return false;
	//return subscene_->moveEvent(toLocalCoords(pos), pressId);
	return subscene_->moveEvent(pos, pressId);
}

bool SingleSceneDocumentTemplate::releaseEvent(const Vector2 & pos, int pressId)
{
	if (!propagate_[pressId]) return false;
	//return subscene_->releaseEvent(toLocalCoords(pos), pressId);
	return subscene_->releaseEvent(pos, pressId);
}

bool SingleSceneDocumentTemplate::keyPressEvent(int keyCode)
{
	return subscene_->keyPressEvent(keyCode);
}

bool SingleSceneDocumentTemplate::keyReleaseEvent(int keyCode)
{
	return subscene_->keyReleaseEvent(keyCode);
}

Vector2 SingleSceneDocumentTemplate::toLocalCoords(const Vector2 & coords) const
{
	//float sx = (float)scene_->screenWidth() / document_->width();
	//float sy = (float)scene_->screenHeight() / document_->height();
	float sx = (float)subscene_->sceneWidth() / document_->width();
	float sy = (float)subscene_->sceneHeight() / document_->height();

	return Vector2(sx * coords.x, sy * coords.y);
}

void SingleSceneDocumentTemplate::start(float docTime)
{
	if (isPreDownload_)
		resetSaveLocalRequest();
}


void SingleSceneDocumentTemplate::setSize(int width, int height, bool resizeGl)
{	
	/*document_->setCurSceneScale(
		(float)width/scene_->screenWidth(), 
		(float)height/scene_->screenHeight());*/
	document_->setCurSceneScale(
		(float)width/subscene_->sceneWidth(), 
		(float)height/subscene_->sceneHeight());

	//width_ = width;
	//height_ = height;

	//camera_.SetTo2DArea((float)width/2, (float)height/2, (float)width, (float)height, 60.0f, (float)width/height);
	//float cWidth = (float)document_->origWidth();
	//float cHeight = (float)document_->origHeight();
	//camera_.SetTo2DArea(cWidth/2, cHeight/2, cWidth, cHeight, 60.0f, cWidth/cHeight);

	width_ = (float)width;
	height_ = (float)height;

	camera_.SetTo2DArea(width_/2, height_/2, width_, height_, 60.0f, width_/height_);

	setSubsceneScaling();
		
	setBaseAppObjectOffset(
		0, 0, 
		document_->curSceneScaleX(), document_->curSceneScaleY(),
		0, 0, width, height);
	
	reInitCurScene(document_->renderer());

	if (resizeGl)
	{
		loadingPage_->init(document_->renderer(), width, height);
		loadingCamera_.SetTo2DArea(width_/2.0f, height_/2.0f, width_, height_, 60.0f, (float)width_/height_);
		static boost::uuids::uuid type = SceneChanger().type();
		for(int i = 0 ; i < document_->scenes().size() ; i++)
		{
			std::vector<SceneObject *> scenechanger;
			document_->scenes()[i]->getSceneObjectsByID(type, &scenechanger);
			BOOST_FOREACH(SceneObject * obj, scenechanger)
			{
				SceneChanger* sc = (SceneChanger*)obj;
				sc->reSize();
			}
		}
	}
}

bool SingleSceneDocumentTemplate::update(float sceneTime)
{
	bool needRedraw = false;
	
	if ((subscene_->sceneWidth() != 0 ||
		subscene_->sceneHeight() != 0) && !isSceneCreated_)
	{
		document_->setCurSceneScale(
		(float)width_/subscene_->sceneWidth(), 
		(float)height_/subscene_->sceneHeight());

		setBaseAppObjectOffset(
		0, 0, 
		document_->curSceneScaleX(), document_->curSceneScaleY(),
		0, 0, width_, height_);
		isSceneCreated_ = true;
	}
	
	if (!isLoaded_)
	{
		needRedraw = true;
		Global::instance().callbackPageChanged(curScene()->name());
	}
	else
		needRedraw |= subscene_->update(sceneTime);
		//needRedraw |= scene_->update(sceneTime);

	if (redrawTriggered_)
	{
		needRedraw = true;
		redrawTriggered_ = false;
	}

	if (subscene_->isLoaded())
	{
		Matrix m = subscene_->transform().computeMatrix();
		setBaseAppObjectOffset(
			m._14, m._24,
			m._11, m._22,
			m._14, m._24, width_ * m._11 / subscene_->normalScaleX() , height_ * m._22 / subscene_->normalScaleY());
	}

	return needRedraw;
}

bool SingleSceneDocumentTemplate::asyncLoadUpdate()
{
	//bool everythingLoaded = scene_->asyncLoadUpdate();
	bool everythingLoaded = subscene_->asyncLoadUpdate();

	//Once the scene is loaded we want it to
	//always be reported as loaded until the scene changes.
	//if (!isLoaded_) isLoaded_ = scene_->isLoaded();
	if (!isLoaded_) isLoaded_ = subscene_->isLoaded();

	if (!everythingLoaded) redrawTriggered_ = true;

	if(!searchQ_.empty() && resetComplete_)
	{
		Scene * scene = searchQ_.front();
		searchQ_.pop_front();
		scene->requestLocalSaveObject();
	}

	return everythingLoaded;
}

void SingleSceneDocumentTemplate::stop()
{
	//scene_->stop();
	//scene_->uninitAppObjects();	
	subscene_->stop();
	subscene_->uninitAppObjects();	
}

Scene * SingleSceneDocumentTemplate::dependsOn(Scene * scene) const
{
	if (subscene_->dependsOn(scene)) return subscene_->scene();

	else
		return 0;
}

void SingleSceneDocumentTemplate::pageScenes(std::vector<Scene *> * scenes) const
{
	scenes->push_back(subscene_->scene());
}

void SingleSceneDocumentTemplate::referencedFiles(
	std::vector<std::string> * refFiles) const
{
}

int SingleSceneDocumentTemplate::setReferencedFiles(
	const std::string & baseDirectory, 
	const vector<string> & refFiles, int startIndex)
{
	return startIndex;
}

void SingleSceneDocumentTemplate::read(Reader & reader, unsigned char version)
{
	reader.readParent<DocumentTemplate>(this);
	if (version >= 1)
	{
		Scene * scene;
		reader.read(scene);
		subscene_->setScene(scene);
	}
	if (version >= 2)
		reader.read(isPreDownload_);
}

void SingleSceneDocumentTemplate::write(Writer & writer) const
{
	writer.writeParent<DocumentTemplate>(this);
	writer.write(subscene_->scene(), "Scene");

	writer.write(isPreDownload_, "preDownload");
}

void SingleSceneDocumentTemplate::writeXml(XmlWriter & w) const
{
	using namespace boost;
	unsigned sceneId = w.idMapping().getId(subscene_->scene());	
	w.writeTag("Scene", sceneId);
}

void SingleSceneDocumentTemplate::readXml(XmlReader & r, xmlNode * parent)
{
	unsigned sceneId = 0;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(sceneId, curNode, "Scene"));
	}

	if (!sceneId) throw XmlException(parent, "No Scene Id");

	subscene_->setScene(r.idMapping().getScene(sceneId));
}

void SingleSceneDocumentTemplate::reInitCurScene(GfxRenderer * gl)
{
	//if (!scene_->isInit()) return;
	//scene_->init(gl);
	if (!subscene_->isInit()) return;
	subscene_->resizeText(gl);
}

Scene * SingleSceneDocumentTemplate::scene() const
{
	return subscene_->scene();
}

void SingleSceneDocumentTemplate::resetSaveLocalRequest()
{
    Global & g = Global::instance();
    g.resetSaveLocalRequest();
	resetComplete_ = false;
	searchQ_.clear();

	searchQ_.push_back(scene());
	resetComplete_ = true;
}


void SingleSceneDocumentTemplate::setSubsceneScaling()
{
	Scene * scene = subscene_->scene();	
	if (scene)
	{
		Transform transform = subscene_->transform();
		float sx = width_ / scene->screenWidth();
		float sy = height_ / scene->screenHeight();
		transform.setScaling(sx, sy, (sx + sy)/2);
		subscene_->setTransform(transform);			
		subscene_->setWindowWidth(scene->screenWidth());
		subscene_->setWindowHeight(scene->screenHeight());
		subscene_->setNormalScaleX(sx);
		subscene_->setNormalScaleY(sy);
	}

	Vector3 curSceneScale = subscene_->transform().scaling();
	document_->setCurSceneScale(curSceneScale.x, curSceneScale.y);
}