#include "stdafx.h"
#include "EditorDocument.h"
#include "Document.h"
#include "CommandHistory.h"
#include "EditorScene.h"
#include "EditorObject.h"
#include "EditorAppObject.h"
#include "EditorAction.h"
#include "PropertyPage.h"
#include "Scene.h"
#include "SceneObject.h"
#include "AppObject.h"
#include "Animation.h"
#include "AnimationChannel.h"
#include "EventListener.h"
#include "FileUtils.h"
#include "Utils.h"
#include "Camera.h"
#include "GLWidget.h"
#include "Root.h"
#include "Attrib.h"
#include "Image.h"
#include "EditorGlobal.h"
#include "CameraObject.h"
#include "Command.h"
#include "ModelFile.h"

using namespace std;

EditorDocument::EditorDocument(GLWidget * glWidget, Document * document)
{
	commandHistory_ = new CommandHistory(this);	
	connect(commandHistory_, SIGNAL(commandHistoryChanged()), 
		this, SIGNAL(commandHistoryChanged()));

	glWidget_ = glWidget;
	document_ = document;

	timeLineTime_ = 0;
	updateOnTimeChange_ =  true;
	dirty_ = false;
	sceneListScrollPos_ = 0;

}


EditorDocument::~EditorDocument()
{
	delete commandHistory_;
		
	{
	map<boost::uuids::uuid, PropertyPage *>::iterator iter;
	for (iter = objPageMap_.begin(); iter != objPageMap_.end(); ++iter)
		delete (*iter).second;
	}

	{
	map<boost::uuids::uuid, PropertyPage *>::iterator iter;
	for (iter = appObjPageMap_.begin(); iter != appObjPageMap_.end(); ++iter)
		delete (*iter).second;
	}

	selectedScene()->uninit();
}

void EditorDocument::init()
{
	BOOST_FOREACH(const SceneSPtr & scene, document_->scenes())
	{
		EditorScene * edScene = editorScene(scene.get()).get();
		if (edScene) edScene->init();
	}
}

void EditorDocument::draw()
{
	GfxRenderer * gl = glWidget_->renderer();
	Scene * scene = selectedScene();

	const Color & color = scene->bgColor();
	glClearColor(color.r, color.g, color.b, color.a);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);		
	scene->handleObjectsListChanged();
	scene->initSceneLighting(gl, glWidget_->camera());
	
	gl->loadMatrix(Matrix::Identity());	

	//render objects
	if (scene->zBuffer())
		glDepthFunc(GL_LEQUAL); 
	else
		glDepthFunc(GL_ALWAYS);
	draw(scene->root(), gl);	

	//draw animation guide line
	Animation * anim = selectedAnimation();		
	if(anim)			
	{			
		drawAniLine(scene->root(), gl);
	}
	
	///draw alpha meshes last
	if (scene->zBuffer())
	{
		const std::vector<ModelFile *> & models = scene->models();
		for (int i = 0; i < (int)models.size(); ++i)
		{		
			models[i]->drawAlpha(glWidget_->renderer());
		}
	}
	//end draw alpha meshes

	Matrix prevCamMatrix;
	Camera appLayerCam;
	glWidget_->appLayerCamera(&appLayerCam, &prevCamMatrix);
	
	gl->setCameraMatrix(appLayerCam.GetMatrix());

	//draw app-level objects
	BOOST_FOREACH(const AppObjectSPtr & appObject, scene->appObjects())
	{
		draw(appObject.get(), gl);		
	}
		
	//draw the screen outline
	gl->clearZBuffer();
	float screenWidth = scene->screenWidth();
	float screenHeight = scene->screenHeight();
	Color outlineColor = EditorGlobal::instance().sceneOutlineColor();
	

	pageVertices_.clear();
	pageVertices_.push_back(Vector3(0.0f, 0.0f, 0.0f));
	pageVertices_.push_back(Vector3(0.0f, screenHeight, 0.0f));

	pageVertices_.push_back(Vector3(screenWidth, 0.0f, 0.0f));
	pageVertices_.push_back(Vector3(screenWidth, screenHeight, 0.0f));

	pageVertices_.push_back(Vector3(0.0f, 0.0f, 0.0f));
	pageVertices_.push_back(Vector3(screenWidth, 0.0f, 0.0f));

	pageVertices_.push_back(Vector3(0.0f, screenHeight, 0.0f));
	pageVertices_.push_back(Vector3(screenWidth, screenHeight, 0.0f));

	gl->useColorProgram();
	gl->setColorProgramColor(outlineColor.r, outlineColor.g, outlineColor.b, 1.0f);	
	gl->enableVertexAttribArrayPosition();
	gl->bindArrayBuffer(0);
	gl->vertexAttribPositionPointer(0, (char *)&pageVertices_[0]);	
	gl->applyCurrentShaderMatrix();

	glDrawArrays(GL_LINES, 0, pageVertices_.size());

	//draw the grid
	pageVertices_.clear();

	float gridSpacing = EditorGlobal::instance().gridSpacing();	
	bool showGrid = EditorGlobal::instance().showGrid();
	

	if (gridSpacing <= 0) showGrid = false;

	if (showGrid)
	{		
		int numVLines = screenWidth / gridSpacing;
		int numHLines = screenHeight / gridSpacing;

		for (int i = -numVLines; i < 2 * numVLines + 1; ++i)
		{
			pageVertices_.push_back(Vector3(gridSpacing * i, gridSpacing * -numHLines, 0.0f));
			pageVertices_.push_back(Vector3(gridSpacing * i, gridSpacing * 2 * numHLines, 0.0f));
		}

		
		for (int i = -numHLines; i < 2 * numHLines + 1; ++i)
		{
			pageVertices_.push_back(Vector3(gridSpacing * -numVLines, gridSpacing * i, 0.0f));
			pageVertices_.push_back(Vector3(gridSpacing * 2 * numVLines, gridSpacing * i, 0.0f));
		}	
	
		glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(GL_XOR);

		if (!pageVertices_.empty())
		{
			gl->useColorProgram();
			gl->setColorProgramColor(outlineColor.r, outlineColor.g, outlineColor.b, 0.3f);	
			gl->enableVertexAttribArrayPosition();
			gl->bindArrayBuffer(0);
			gl->vertexAttribPositionPointer(0, (char *)&pageVertices_[0]);	
			gl->applyCurrentShaderMatrix();
			glDrawArrays(GL_LINES, 0, pageVertices_.size());

		}
		glDisable(GL_COLOR_LOGIC_OP);
		//end draw screen outline
	}	

	emit sceneDrawn();
}

GfxRenderer * EditorDocument::renderer() const
{
	if (document_)
	{
		return document_->renderer();
	}

	return 0;
}

void EditorDocument::drawAniLine(SceneObject * sceneObject, GfxRenderer * gl)
{
	VisualAttrib * attr = sceneObject->visualAttrib();
	Animation * anim = selectedAnimation();		

	if (attr)
	{				
		gl->pushMatrix();
		gl->multMatrix(attr->transformMatrix());		
		
		const vector<AnimationChannelSPtr>* channels = anim->channels(selectedObject());
		if (channels)
		{
			GLfloat lineWidth = 2.0f;

			animationLine_.clear();		
			Color guidelineColor = EditorGlobal::instance().AnimationGuidlineColor();				
			Image * image = (Image *)selectedObject();

			BOOST_FOREACH(const AnimationChannelSPtr & channel, *channels)
			{
				int numKeyFrames = channel->numKeyFrames();				
				for (int i = 0; i < numKeyFrames; ++i)
				{																		
					GuideLinePos_ = channel->PosFromKeyFrameIndex(i);
						
					GuideLinePos_.x += image->width()/2.;
					GuideLinePos_.y += image->height()/2.;

					animationLine_.push_back(GuideLinePos_);
				
				}	
				if(!animationLine_.empty())
				{																
					gl->useColorProgram();
					gl->setColorProgramColor(guidelineColor.r, guidelineColor.g, guidelineColor.b, 0.5f);								
					gl->enableVertexAttribArrayPosition();
					gl->bindArrayBuffer(0);
					gl->vertexAttribPositionPointer(0, (char *)&animationLine_[0]);	
					gl->applyCurrentShaderMatrix();			
					glLineWidth(lineWidth);
					glDrawArrays(GL_LINE_STRIP, 0, animationLine_.size());								
							
				}
			}
			animationLine_.clear();
		}				
	}	
	if (attr) gl->popMatrix();
}


void EditorDocument::draw(SceneObject * sceneObject, GfxRenderer * gl)
{
	EditorObject * edObj = editorObject(sceneObject).get();	
	VisualAttrib * attr = sceneObject->visualAttrib();

	if (attr)
	{				
		gl->pushMatrix();
		gl->multMatrix(attr->transformMatrix());					
	}
	
	if (edObj)
	{
		edObj->drawObject(gl, this);			
	}
	else
		sceneObject->drawObject(gl);

	BOOST_FOREACH(const SceneObjectSPtr & child, sceneObject->children())
	{
		draw(child.get(), gl);
	}	
	
	if (attr) gl->popMatrix();
}

void EditorDocument::draw(AppObject * appObject, GfxRenderer * gl)
{
	EditorAppObject * edObj = editorAppObject(appObject).get();
	if (edObj) edObj->drawObject(gl, this);
}

//------------------------------------------------------------------------------
EditorSceneSPtr EditorDocument::editorScene(Scene * scene) const
{
	map<Scene *, EditorSceneSPtr>::const_iterator iter = editorScenes_.find(scene);
	if (iter != editorScenes_.end()) return (*iter).second;

	EditorScene * editorScene = new EditorScene(scene);
	EditorSceneSPtr sptr(editorScene);
	editorScenes_[scene] = sptr;
	return sptr;
}

//------------------------------------------------------------------------------
bool EditorDocument::isAnimationSelected(const Animation * anim) const
{
	foreach(Animation * selAnim, selectedAnimations_)
	{
		if (selAnim == anim) return true;
	}
	return false;
}
//------------------------------------------------------------------------------
Animation * EditorDocument::selectedAnimation() const
{
	if (selectedAnimations_.empty()) return 0; 
	return selectedAnimations_.front();
}

//------------------------------------------------------------------------------
const std::vector<SceneSPtr> & EditorDocument::scenes() const
{
	return document_->scenes();
}
//------------------------------------------------------------------------------
bool EditorDocument::isSceneSelected(const Scene * scene) const
{
	BOOST_FOREACH(Scene * curScene, selectedScenes_)
	{
		if (curScene == scene) return true;
	}

	return false;
}
//------------------------------------------------------------------------------
void EditorDocument::setSelectedScenes(const std::vector<Scene *> & scenes)
{
	Scene * prevSelScene = selectedScene();
	selectedScenes_ = scenes;
	Scene * selScene = selectedScene();
	if (selScene != prevSelScene)
	{
		//unload previous scene.
		//Caution must make sure that prevSelScene actually exists and has not
		//been deleted before calling Uninit
		SceneSPtr prevSelSceneSPtr = document_->findScene(prevSelScene);
		if (prevSelSceneSPtr)
		{
			//uninit if there are no active subscene references to the scene
			prevSelSceneSPtr->setKeepInit(false);			
		}
		//clear scene object selection
		vector<SceneObject *> selObjs;
		setSelectedObjects(selObjs);

		//clear app object selection
		vector<AppObject *> selAppObjs;
		setSelectedAppObjects(selAppObjs);

		//clear animation selection
		vector<Animation *> selAnims;
		setSelectedAnimations(selAnims);


		//load new scene
		Scene * scene = selectedScene();		

		/*
		//increment reference count to protect against the scene being uninited
		//out from under us
		//note: this also does an ->init() and ->start(0)		
		bool wasInit = scene->isinit();
		scene->addSubSceneRef(0, 0);
		if (wasInit)
		{
			//reinitialize
			//comment this out, it is causing delays when changing scenes
			//scene->init(document_->renderer());
			
			//Try commenting this out to prevent abrupt scene resets in the editor
			//scene->start(0);
		}*/
		if (!scene->isInit())
			scene->init(scene->parentDocument()->renderer());
		scene->setKeepInit(true);



	}

	emit sceneSelectionChanged();
}

//------------------------------------------------------------------------------
const std::vector<Scene *> & EditorDocument::selectedScenes() const
{
	return selectedScenes_;
}
//------------------------------------------------------------------------------
Scene * EditorDocument::selectedScene() const
{
	if (selectedScenes_.empty()) return 0;
	return selectedScenes_.front();
}
//------------------------------------------------------------------------------
void EditorDocument::setSelectedAnimations(
	const std::vector<Animation *> & animations)
{
	selectedAnimations_ = animations;
	setSelectedChannels(vector<AnimationChannel*>());
	setSelectedKeyFrames(vector<KeyFrameData>());
	emit animationSelectionChanged();
}

//------------------------------------------------------------------------------
bool EditorDocument::isChannelSelected(const AnimationChannel * obj) const
{
	foreach(AnimationChannel * selObj, selectedChannels_)
	{
		if (selObj == obj) return true;
	}
	return false;
}
//------------------------------------------------------------------------------
void EditorDocument::setSelectedChannels(
	const std::vector<AnimationChannel *> & channels)
{
	selectedChannels_ = channels;
	emit channelSelectionChanged();
}
//------------------------------------------------------------------------------
void EditorDocument::setTimeLineTime(float time)
{
	timeLineTime_ = time;

	if (updateOnTimeChange_)
	{
		Animation * anim = selectedAnimation();
		if (anim)
		{
			anim->update(time);
		}
	}

	emit timeLineTimeChanged();
}
//------------------------------------------------------------------------------
void EditorDocument::scrubTimeLineTime(float time)
{
	timeLineTime_ = time;

	if (updateOnTimeChange_)
	{
		Animation * anim = selectedAnimation();
		if (anim)
		{
			anim->update(time);
		}
	}

	emit timeLineTimeChanging();
}
//------------------------------------------------------------------------------
void EditorDocument::setSelectedKeyFrames(const vector<KeyFrameData> &keyFrames)
{
	selectedKeyFrames_ = keyFrames;
	emit keyFrameSelectionChanged();
}
//------------------------------------------------------------------------------
bool EditorDocument::isKeyFrameSelected(
	AnimationChannel * channel, int index) const
{
	
	foreach(KeyFrameData key, selectedKeyFrames_)
	{
		if (key.channel == channel)
		{
			KeyFrame * keyFrame = channel->keyFrame(index);	
			if (key.time == keyFrame->time()) return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------
bool EditorDocument::isKeyFrameSelected(const KeyFrameData & data) const
{
	BOOST_FOREACH(KeyFrameData curData, selectedKeyFrames_)
	{
		if (data == curData) return true;
	}

	return false;
}

//------------------------------------------------------------------------------
EventListener * EditorDocument::selectedListener() const
{
	if (selectedListeners_.empty()) return 0; 
	return selectedListeners_.front();
}
//------------------------------------------------------------------------------
void EditorDocument::setSelectedListeners(
	const std::vector<EventListener *> & listeners)
{
	selectedListeners_ = listeners;
	handleListenerSelChange();
	emit listenerSelectionChanged();
}
//------------------------------------------------------------------------------
bool EditorDocument::isListenerSelected(EventListener * listener) const
{
	BOOST_FOREACH(EventListener * selListener, selectedListeners_)
		if (selListener == listener) return true;
	return false;
}
//------------------------------------------------------------------------------
Action * EditorDocument::selectedAction() const
{
	if (selectedActions_.empty()) return 0; 
	return selectedActions_.front();
}
//------------------------------------------------------------------------------
void EditorDocument::setSelectedActions(
	const std::vector<Action *> & actions)
{
	selectedActions_ = actions;
	emit actionSelectionChanged();
}
//------------------------------------------------------------------------------
bool EditorDocument::isActionSelected(Action * action) const
{
	BOOST_FOREACH(Action * selAction, selectedActions_)
		if (selAction == action) return true;
	return false;
}
//------------------------------------------------------------------------------
void EditorDocument::addCommand(Command * command)
{
	commandHistory_->doCommand(command);
}
//------------------------------------------------------------------------------
void EditorDocument::doCommand(Command * command)
{
	try {
		commandHistory_->doCommand(command);
		dirty_ = true;
	} catch (const Exception & e)
	{
		QMessageBox::information(0, tr("Error"), e.what());
	}
}
//------------------------------------------------------------------------------
void EditorDocument::undoCommand()
{
	commandHistory_->undoCommand();
}
//------------------------------------------------------------------------------
void EditorDocument::redoCommand()
{
	commandHistory_->redoCommand();
}
//------------------------------------------------------------------------------
bool EditorDocument::undoAvailable() const
{
	return commandHistory_->undoAvailable();
}
//------------------------------------------------------------------------------
bool EditorDocument::redoAvailable() const
{
	return commandHistory_->redoAvailable();
}
//------------------------------------------------------------------------------
void EditorDocument::setSelectedAppObjects(
	const std::vector<AppObject *> & objs)
{
	selectedAppObjects_ = objs;
	emit appObjectSelectionChanged();
	handleAppObjSelChange();
}
//------------------------------------------------------------------------------
bool EditorDocument::isAppObjectSelected(AppObject * object) const
{
	foreach(AppObject * selObj, selectedAppObjects_)
	{
		if (selObj == object) return true;
	}
	return false;
}

//------------------------------------------------------------------------------
void EditorDocument::setSelectedObjects(const vector<SceneObject *> & objs)
{
	selectedObjects_ = objs;
	emit objectSelectionChanged();
	handleObjSelChange();	
}

//------------------------------------------------------------------------------
void EditorDocument::setSelectedRecurse(SceneObject * object, bool sel)
{
	setSelectedRecurseHelper(object, sel);
	emit objectSelectionChanged();
}
void EditorDocument::setSelectedRecurseHelper(SceneObject * object, bool sel)
{
	vector<SceneObject *>::iterator iter;
	iter = find(selectedObjects_.begin(), selectedObjects_.end(), object);
	if (!sel)
	{
		if (iter != selectedObjects_.end())
			selectedObjects_.erase(iter);
	}
	else
	{
		if (iter == selectedObjects_.end())
			selectedObjects_.push_back(object);
	}

	BOOST_FOREACH(SceneObjectSPtr child, object->children())
	{
		setSelectedRecurseHelper(child.get(), sel);
	}
}

//------------------------------------------------------------------------------
PropertyPage * EditorDocument::getPropertyPage(SceneObject * sceneObj)
{
	EditorObject * obj = editorObject(sceneObj).get();
	if (!obj) return 0;
	map<boost::uuids::uuid, PropertyPage *>::iterator iter;
	iter = objPageMap_.find(sceneObj->type());
	if (iter == objPageMap_.end())
	{
		PropertyPage * page = obj->propertyPage();
		page->setDocument(this);
		objPageMap_[sceneObj->type()] = page;
		return page;
	}
	else return (*iter).second;	
}
//------------------------------------------------------------------------------
PropertyPage * EditorDocument::getPropertyPage(AppObject * sceneObj)
{
	if (!sceneObj) return 0;
	EditorAppObject * obj = editorAppObject(sceneObj).get();
	if (!obj) return 0;
	map<boost::uuids::uuid, PropertyPage *>::iterator iter;
	iter = appObjPageMap_.find(sceneObj->type());
	if (iter == appObjPageMap_.end())
	{
		PropertyPage * page = obj->propertyPage();
		page->setDocument(this);
		appObjPageMap_[sceneObj->type()] = page;
		return page;
	}
	else return (*iter).second;	
}

//------------------------------------------------------------------------------
bool EditorDocument::isObjectSelected(SceneObject * object) const
{
	foreach(SceneObject * selObj, selectedObjects_)
	{
		if (selObj == object) return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool EditorDocument::isObjectAncestorSelected(SceneObject * object) const
{
	SceneObject * curObj = object->parent();
	bool sel = false;
	while(curObj)
	{
		if (isObjectSelected(curObj)) sel = true;		
		curObj = curObj->parent();
	}

	return sel;
	
}


//------------------------------------------------------------------------------
void EditorDocument::selectObject(SceneObject * editorObject)
{
	selectedObjects_.clear();
	selectedObjects_.push_back(editorObject);	

	emit objectSelectionChanged();

	handleObjSelChange();
}
//------------------------------------------------------------------------------
void EditorDocument::selectAppObject(AppObject * editorObject)
{
	selectedAppObjects_.clear();
	selectedAppObjects_.push_back(editorObject);	

	emit appObjectSelectionChanged();

	handleAppObjSelChange();
}
//------------------------------------------------------------------------------
void EditorDocument::setObjectSelected(SceneObject * obj, bool selected)
{
	
	if (!selected)
	{
		vector<SceneObject *>::iterator iter = selectedObjects_.begin();
		for (; iter != selectedObjects_.end(); ++iter)
		{
			if (*iter == obj) 
			{
				selectedObjects_.erase(iter); 
				break;
			}
		}
	}
	else
	{
		selectedObjects_.push_back(obj);
	}	

	emit objectSelectionChanged();
	handleObjSelChange();
}
//------------------------------------------------------------------------------
void EditorDocument::setAppObjectSelected(AppObject * obj, bool selected)
{
	
	if (!selected)
	{
		vector<AppObject *>::iterator iter = selectedAppObjects_.begin();
		for (; iter != selectedAppObjects_.end(); ++iter)
		{
			if (*iter == obj) 
			{
				selectedAppObjects_.erase(iter); 
				break;
			}
		}
	}
	else
	{
		selectedAppObjects_.push_back(obj);
	}	

	emit appObjectSelectionChanged();
	handleAppObjSelChange();
}
//------------------------------------------------------------------------------
void EditorDocument::deselectAll()
{
	selectedObjects_.clear();
	emit objectSelectionChanged();

	handleObjSelChange();
}
//------------------------------------------------------------------------------
void EditorDocument::deselectAllAppObjects()
{
	selectedAppObjects_.clear();
	emit appObjectSelectionChanged();

	handleAppObjSelChange();
}
//------------------------------------------------------------------------------

EditorObjectSPtr EditorDocument::editorObject(SceneObject * obj)
{
	EditorScene * scene = editorScene(obj->parentScene()).get();
	if (!scene) return EditorObjectSPtr();
	return scene->editorObject(obj);
}
//------------------------------------------------------------------------------

EditorAppObjectSPtr EditorDocument::editorAppObject(AppObject * obj)
{
	EditorScene * scene = editorScene(obj->parentScene()).get();
	if (!scene) return EditorAppObjectSPtr();
	return scene->editorAppObject(obj);
}

//------------------------------------------------------------------------------

EditorActionSPtr EditorDocument::editorAction(Scene * scene, Action * action)
{
	EditorScene * edScene = editorScene(scene).get();
	if (!edScene) return EditorActionSPtr();
	return edScene->editorAction(action);
}

//------------------------------------------------------------------------------
void EditorDocument::handleListenerSelChange()
{
	if (!selectedActions_.empty())
	{
		EventListener * selListener = selectedListener();	
		Action * selAction = selectedAction();
		bool hasAction = false;		
		if (selListener)
		{
			BOOST_FOREACH(const ActionSPtr & action, selListener->actions())			
				if (action.get() == selAction) hasAction = true;			
		}

		if (!hasAction)
		{
			selectedActions_.clear();
			emit actionSelectionChanged();
		}
		
	}	
}
//------------------------------------------------------------------------------
void EditorDocument::handleAppObjSelChange()
{
	AppObject* appObj = selectedAppObject();
	if (!selectedListeners_.empty())
	{
		bool hasListener = false;
		EventListener * selListener = selectedListener();
		AppObject* listenerObj = appObj;
		if (!listenerObj) return;
		
		BOOST_FOREACH(
			const EventListenerSPtr & listener, listenerObj->eventListeners())
		{
			if (listener.get() == selListener) hasListener = true;
		}
		

		if (!hasListener)
		{
			selectedListeners_.clear();
			handleListenerSelChange();
			emit listenerSelectionChanged();
		}
	}
}
//------------------------------------------------------------------------------
void EditorDocument::handleObjSelChange()
{
	SceneObject * selObj = selectedObject();
	
	if (!selectedChannels_.empty())
	{		
		if (selectedChannels_.front()->object() != selObj)
		{
			selectedChannels_.clear();
			emit channelSelectionChanged();
		}
	}

	if (!selectedKeyFrames_.empty())
	{
		AnimationChannel * channel = selectedKeyFrames_.front().channel;
		if (channel->object() != selObj)
		{
			selectedKeyFrames_.clear();
			emit keyFrameSelectionChanged();
		}
	}
	

	if (!selectedListeners_.empty())
	{
		bool hasListener = false;
		EventListener * selListener = selectedListener();
		SceneObject * listenerObj = selObj;
		if (!listenerObj) listenerObj = (SceneObject *)selectedScene()->root();
		
		BOOST_FOREACH(
			const EventListenerSPtr & listener, listenerObj->eventListeners())
		{
			if (listener.get() == selListener) hasListener = true;
		}
		

		if (!hasListener)
		{
			selectedListeners_.clear();
			handleListenerSelChange();
			emit listenerSelectionChanged();
		}
	}

	
}


void EditorDocument::onKeyFrameTimeChanged(
	const std::vector<KeyFrameData> & oldKeyFrames, const std::vector<float> & newTimes)
{
	BOOST_FOREACH(KeyFrameData & data, selectedKeyFrames_)
	{
		for (int i = 0; i < (int)oldKeyFrames.size(); ++i)
		{
			if (data == oldKeyFrames[i])
				data.time = newTimes[i];
		}		
	}
}

void EditorDocument::onKeyFrameDeleted()
{
	vector<KeyFrameData>::iterator iter = selectedKeyFrames_.begin();
	while (iter != selectedKeyFrames_.end())
	{
		KeyFrameData & data = *iter;
		if (!data.ptr())
		{
			iter = selectedKeyFrames_.erase(iter);			
		}
		else ++iter;
	}

	emit keyFrameSelectionChanged();
}

void EditorDocument::onAnimationListChanged()
{
	vector<Animation *>::iterator iter = selectedAnimations_.begin();
	while (iter != selectedAnimations_.end())
	{
		if (!selectedScene()->findAnimation(*iter))
			iter = selectedAnimations_.erase(iter);
		else
			++iter;
	}
	emit animationSelectionChanged();
	emit animationListChanged();
}

void EditorDocument::onChannelListChanged()
{
	vector<AnimationChannel *>::iterator iter = selectedChannels_.begin();
	while (iter != selectedChannels_.end())
	{		
		if (!selectedAnimation()->findChannel(*iter))
			iter = selectedChannels_.erase(iter);
		else
			++iter;
	}
	emit channelSelectionChanged();
	emit channelListChanged();
}

void EditorDocument::onActionChanged(
	Action * oldActionAddr, Action * newActionAddr)
{
	vector<Action *>::iterator iter = selectedActions_.begin();
	while (iter != selectedActions_.end())
	{
		if (*iter == oldActionAddr) *iter = newActionAddr;
		if (!selectedListener()->findAction(*iter))
			iter = selectedActions_.erase(iter);
		else
			++iter;
	}
	emit actionSelectionChanged();
	emit actionChanged();
}


void EditorDocument::onListenerListChanged()
{
	vector<EventListener *>::iterator iter = selectedListeners_.begin();

	if (!selectedObject()) 
	{
		selectedListeners_.clear();
	}
	else
	{
		while (iter != selectedListeners_.end())
		{		
			if (!selectedObject()->findListener(*iter))
				iter = selectedListeners_.erase(iter);
			else
				++iter;
		}
	}

	emit listenerSelectionChanged();	
	handleListenerSelChange();
	emit listenerListChanged();
}

void EditorDocument::onActionListChanged()
{
	vector<Action *>::iterator iter = selectedActions_.begin();
	while (iter != selectedActions_.end())
	{		
		if (!selectedListener()->findAction(*iter))
			iter = selectedActions_.erase(iter);
		else
			++iter;
	}
	emit actionSelectionChanged();	
	emit actionListChanged();
}



void EditorDocument::onObjectListChanged()
{
	/*
	Note, objectListChanged() should be emitted before objectSelectionChanged()
	In the object list, if it's attempted to change the selection state before
	the list is properly updated to clear outdated elements, a crash can occur
	*/

	emit objectListChanged();

	vector<SceneObject *>::iterator iter = selectedObjects_.begin();
	bool selectionChanged = false;	
	while (iter != selectedObjects_.end())
	{		
		SceneObject * obj = *iter;
		if (!obj->parentScene())
		{
			iter = selectedObjects_.erase(iter);
			selectionChanged = true;
		}
		else
			++iter;
	}

	if (selectionChanged) emit objectSelectionChanged();	
}

void EditorDocument::onAppObjectListChanged()
{	
	emit appObjectListChanged();

	vector<AppObject *>::iterator iter = selectedAppObjects_.begin();
	bool selectionChanged = false;	
	while (iter != selectedAppObjects_.end())
	{		
		AppObject * obj = *iter;
		if (!selectedScene()->findAppObject(obj))		
		{
			iter = selectedAppObjects_.erase(iter);
			selectionChanged = true;
		}
		else
			++iter;
	}

	if (selectionChanged) emit appObjectSelectionChanged();	
}

void EditorDocument::onSceneListChanged()
{
	emit sceneListChanged();
	
	Scene * prevSelScene = selectedScene();

	vector<Scene *> newSelectedScenes = selectedScenes_;
	vector<Scene *>::iterator iter = newSelectedScenes.begin();
	bool selectionChanged = false;	
	while (iter != newSelectedScenes.end())
	{		
		Scene * scene = *iter;
		if (!document_->findScene(scene))
		{
			iter = newSelectedScenes.erase(iter);
			selectionChanged = true;
		}
		else
			++iter;
	}
	if (newSelectedScenes.empty())
		newSelectedScenes.push_back(document_->scenes().front().get());
	if (selectionChanged)
	{
		setSelectedScenes(newSelectedScenes);
	}	
}

void EditorDocument::addEditorScene(const EditorSceneSPtr & edScene)
{
	editorScenes_[edScene->scene()] = edScene;
}

void EditorDocument::deleteEditorScene(EditorScene * edScene)
{
	map<Scene *, EditorSceneSPtr>::iterator iter;
	for (iter = editorScenes_.begin(); iter != editorScenes_.end(); ++iter)
	{
		if ((*iter).second.get() == edScene)
		{
			editorScenes_.erase(iter);
			break;
		}
	}
}



void EditorDocument::addEditorObject(const EditorObjectSPtr &edObj)
{
	edObj->parentScene()->addEditorObject(edObj);
}

void EditorDocument::addEditorAppObject(const EditorAppObjectSPtr &edObj)
{
	edObj->parentScene()->addEditorAppObject(edObj);
}


void EditorDocument::deleteEditorObject(EditorObject *edObj)
{
	edObj->parentScene()->deleteEditorObject(edObj);
}
void EditorDocument::deleteEditorAppObject(EditorAppObject *edObj)
{
	edObj->parentScene()->deleteEditorAppObject(edObj);
}

void EditorDocument::addEditorAction(const EditorActionSPtr &edAction)
{
	if (edAction) edAction->parentScene()->addEditorAction(edAction);
}

void EditorDocument::deleteEditorAction(EditorAction *edAction)
{
	if (edAction) edAction->parentScene()->deleteEditorAction(edAction);
}

void EditorDocument::doUserCamChangeCmd(const Camera & origCam, const Camera & newCam)
{
	Scene * scene = selectedScene();
	
	CameraObject * obj = scene->userCamera();
	if (obj)	
		doCommand(new ChangeCameraTransformCmd(obj, origCam, newCam));	
}

void EditorDocument::texttoText_2Convert()
{
	if(selectedObject()->type() != Text().type()) return;

	QMessageBox::StandardButton reply = 
		QMessageBox::information(0, tr("Message"), 
			tr("Do You Want to Convert to the New Text Object?"),
			QMessageBox::Yes | QMessageBox::Cancel);

	if(reply == QMessageBox::Yes)
	{
		Text * text = (Text *)selectedObject();
	
		Text_2 * text2 = new Text_2;
		text2->convertText_2(text);

		SceneObject * refParent = text->parent();
	
		SceneObjectSPtr convObjs(text2);
		refParent->insertAfter(convObjs, text);
		convObjs->init(document_->renderer());
		convObjs->start(0);
	
		std::vector<SceneObject*> selobjs;
		selobjs.push_back(convObjs.get());
		setSelectedObjects(selobjs);

		Scene * scene = text2->parentScene();

		BOOST_FOREACH(AnimationSPtr anim, scene->animations())
		{
			if(anim->hasObject(text))
			{
				const std::vector<AnimationChannelSPtr> animChs = *anim->channels(text);
			
				std::vector<AnimationChannelSPtr> newAnimChs;


				for(int i = 0; i < (int)animChs.size(); i++)
				{
					newAnimChs.push_back(AnimationChannelSPtr(animChs[i]->clone()));
					newAnimChs.back()->setObject(text2);

					anim->deleteChannel(animChs[i].get());
				}
			
				anim->setChannels(text2, newAnimChs);
			}
		}

		std::vector<EventListenerSPtr> events;
		scene->allListeners(&events);

		BOOST_FOREACH(EventListenerSPtr ev, events)
		{
			BOOST_FOREACH(ActionSPtr action, ev->actions())
			{
				if(action->targetObject() == text)
					action->setTargetObject(text2);
			}
		}

		//del
		EditorObjectSPtr edObj = editorObject(text);

		text->uninit();
		text->parent()->deleteChild(text);

		deleteEditorObject(edObj.get());

		onObjectListChanged();
		onAnimationListChanged();
		onChannelListChanged();

		dirty_ = true;
	}
	else if(reply == QMessageBox::Cancel)
		return;
}
