#include "stdafx.h"
#include "EditorGlobal.h"
#include "Scene.h"
#include "CustomObject.h"

#include "EditorRectangle.h"
#include "EditorImage.h"
#include "EditorBoundingBoxObject.h"
#include "EditorCameraObject.h"
#include "EditorLightObject.h"
#include "EditorAudioObject.h"
#include "EditorVideoObject.h"
#include "EditorGroup.h"
#include "EditorRoot.h"
#include "EditorSubScene.h"
#include "EditorSceneChanger.h"
#include "EditorImageChanger.h"
#include "EditorImageAnimation.h"
#include "EditorTAnimation.h"
#include "EditorTurnTable.h"
#include "EditorPanorama.h"
#include "EditorImageGrid.h"
#include "EditorText.h"
#include "EditorText_2.h"
#include "EditorTextTable.h"
#include "EditorTextEditObject.h"
#include "EditorModel.h"
#include "EditorMap.h"
#include "EditorMultiChoiceQuestion.h"
#include "EditorMaskObject.h"
#include "EditorButtonObject.h"
#include "EditorPhotoObject.h"

#include "EditorVideoPlayAction.h"
#include "EditorAudioPlayAction.h"
#include "EditorImageSetFileAction.h"
#include "EditorAnimateAction.h"
#include "EditorPlayVideoAction.h"
#include "EditorPlayAudioAction.h"
#include "EditorOpenUrlAction.h"
#include "EditorLaunchAppAction.h"
#include "EditorShowOnlyAction.h"
#include "EditorShowAllAction.h"
#include "EditorWaitUntilAction.h"
#include "EditorEffectAction.h"
#include "EditorResetAnimationAction.h"
#include "EditorStopAnimationAction.h"
#include "EditorSceneChangeAction.h"
#include "EditorBroadcastMessageAction.h"
#include "EditorSceneChangerShowSceneAction.h"
#include "EditorSceneChangerSceneSetAction.h"
#include "EditorImageChangerShowImageAction.h"
#include "EditorImageChangerSetImageAction.h"
#include "EditorSetVisibilityAction.h"
#include "EditorSetOpacityAction.h"
#include "EditorResumeAnimationAction.h"
#include "EditorSetCameraAction.h"
#include "EditorShareAction.h"
#include "EditorImageAnimationPauseAction.h"
#include "EditorImageAnimationResumeAction.h"

#include "AppObject.h"
#include "EditorAppObject.h"
#include "EditorAppImage.h"
#include "EditorAppWebContent.h"

#include "EditorReceiveMessageEvent.h"
#include "EditorImageGridImageClickedEvent.h"
#include "EditorSceneChangerPageChangedEvent.h"
#include "EditorAudioPlayTimeEvent.h"
#include "EditorVideoPlayTimeEvent.h"

#include "EditorSingleSceneDocumentTemplate.h"
#include "EditorSceneChangerDocumentTemplate.h"
#include "EditorMagazineDocumentTemplate.h"
#include "EditorCatalogDocumentTemplate.h"

#include "ElementMapping.h"

#include "AnimationChannel.h"

#include "Root.h"

using namespace std;

boost::mt19937 EditorGlobal::ran_;

EditorGlobal::EditorGlobal() : 
settings_("Net&TV", "Studio4UX")
{
	clipboardObjMapping_.reset(new ElementMapping);
	clipboardSceneMapping_.reset(new ElementMapping);
	
	EditorImage * edImg = new EditorImage;
	EditorBoundingBoxObject * edBbObj = new EditorBoundingBoxObject;
	EditorCameraObject * edCam = new EditorCameraObject;
	EditorLightObject * edLight = new EditorLightObject;
	EditorAudioObject * edAudio = new EditorAudioObject;
	EditorVideoObject * edVideo = new EditorVideoObject;
	EditorGroup * edGrp = new EditorGroup;
	EditorRoot * edRoot = new EditorRoot;
	EditorSubScene * edSubScene = new EditorSubScene;
	EditorTurnTable * edTurnTable = new EditorTurnTable;
	EditorPanorama * edPanorama = new EditorPanorama;
	EditorRectangle * edRect = new EditorRectangle;
	EditorSceneChanger * edSceneChanger = new EditorSceneChanger;
	EditorImageChanger *  edImgChanger = new EditorImageChanger;
	EditorImageAnimation *edImgAnim = new EditorImageAnimation;
	EditorTAnimation * edTAnim = new EditorTAnimation;
	EditorText * edText = new EditorText;
	EditorText_2 * edText_2 = new EditorText_2;
	EditorTextTable * edTextTable = new EditorTextTable;
	EditorTextEditObject * edTextEditObj = new EditorTextEditObject;
	EditorModel * edModel = new EditorModel;
	EditorMap * edMap = new EditorMap;
	EditorImageGrid * edImgGrid = new EditorImageGrid;
	EditorMaskObject * edMaskObj = new EditorMaskObject;
	EditorButtonObject * edButtonObj = new EditorButtonObject;
	EditorPhotoObject * edPhotoObj = new EditorPhotoObject;
	
	editorObjs_[edImg->sceneObjectType()] = edImg;
	editorObjs_[edBbObj->sceneObjectType()] = edBbObj;
	editorObjs_[edCam->sceneObjectType()] = edCam;
	editorObjs_[edLight->sceneObjectType()] = edLight;
	editorObjs_[edGrp->sceneObjectType()] = edGrp;
	editorObjs_[edRoot->sceneObjectType()] = edRoot;
	editorObjs_[edRect->sceneObjectType()] = edRect;
	editorObjs_[edSubScene->sceneObjectType()] = edSubScene;
	editorObjs_[edSceneChanger->sceneObjectType()] = edSceneChanger;
	editorObjs_[edTurnTable->sceneObjectType()] = edTurnTable;
	editorObjs_[edPanorama->sceneObjectType()] = edPanorama;
	editorObjs_[edText->sceneObjectType()] = edText;
	editorObjs_[edText_2->sceneObjectType()] = edText_2;
	editorObjs_[edTextTable->sceneObjectType()] = edTextTable;
	editorObjs_[edTextEditObj->sceneObjectType()] = edTextEditObj;
	editorObjs_[edModel->sceneObjectType()] = edModel;
	editorObjs_[edMap->sceneObjectType()] = edMap;
	editorObjs_[edImgGrid->sceneObjectType()] = edImgGrid;
	editorObjs_[edImgChanger->sceneObjectType()] = edImgChanger;
	editorObjs_[edImgAnim->sceneObjectType()] = edImgAnim;
	editorObjs_[edTAnim->sceneObjectType()] = edTAnim;
	editorObjs_[edAudio->sceneObjectType()] = edAudio;
	editorObjs_[edVideo->sceneObjectType()] = edVideo;
	editorObjs_[edMaskObj->sceneObjectType()] = edMaskObj;
	editorObjs_[edButtonObj->sceneObjectType()] = edButtonObj;
	editorObjs_[edPhotoObj->sceneObjectType()] = edPhotoObj;

	EditorMultiChoiceQuestion * edMultiChoiceQuestion = new EditorMultiChoiceQuestion;
	editorCustomObjs_[edMultiChoiceQuestion->customType()] = edMultiChoiceQuestion;

	EditorAppImage * edAppImage = new EditorAppImage;
	editorAppObjs_[edAppImage->appObjectType()] = edAppImage;
	EditorAppWebContent * edAppWebContent = new EditorAppWebContent;
	editorAppObjs_[edAppWebContent->appObjectType()] = edAppWebContent;

	EditorAnimateAction * edAnimAction = new EditorAnimateAction;
	editorActions_[edAnimAction->actionType()] = edAnimAction;	
	EditorShowOnlyAction * edShowOnlyAction = new EditorShowOnlyAction;
	editorActions_[edShowOnlyAction->actionType()] = edShowOnlyAction;
	EditorShowAllAction * edShowAllAction = new EditorShowAllAction;
	editorActions_[edShowAllAction->actionType()] = edShowAllAction;
	EditorWaitUntilAction * edWaitUntilAction = new EditorWaitUntilAction;
	editorActions_[edWaitUntilAction->actionType()] = edWaitUntilAction;
	EditorEffectAction * edEffectAction = new EditorEffectAction;
	editorActions_[edEffectAction->actionType()] = edEffectAction;
	EditorResumeAnimationAction * edResAnimAction = new EditorResumeAnimationAction;
	editorActions_[edResAnimAction->actionType()] = edResAnimAction;
	EditorSetCameraAction * edSetCameraAction = new EditorSetCameraAction;
	editorActions_[edSetCameraAction->actionType()] = edSetCameraAction;
	
	EditorResetAnimationAction * edResetAnimAction = new EditorResetAnimationAction;
	editorActions_[edResetAnimAction->actionType()] = edResetAnimAction;

	EditorStopAnimationAction * edStopAnimAction = new EditorStopAnimationAction;
	editorActions_[edStopAnimAction->actionType()] = edStopAnimAction;

	EditorSceneChangeAction * edSceneChangeAction = new EditorSceneChangeAction;
	editorActions_[edSceneChangeAction->actionType()] = edSceneChangeAction;	
	EditorPlayVideoAction * edPlayVideoAction = new EditorPlayVideoAction;
	editorActions_[edPlayVideoAction->actionType()] = edPlayVideoAction;
	EditorPlayAudioAction * edPlayAudioAction = new EditorPlayAudioAction;
	editorActions_[edPlayAudioAction->actionType()] = edPlayAudioAction;
	EditorOpenUrlAction * edOpenUrlAction = new EditorOpenUrlAction;
	editorActions_[edOpenUrlAction->actionType()] = edOpenUrlAction;
	EditorLaunchAppAction * edLaunchAppAction = new EditorLaunchAppAction;
	editorActions_[edLaunchAppAction->actionType()] = edLaunchAppAction;
	EditorBroadcastMessageAction * edBroadcastMsgAction = new EditorBroadcastMessageAction;
	editorActions_[edBroadcastMsgAction->actionType()] = edBroadcastMsgAction;
	EditorSceneChangerShowSceneAction * edSceneChangerShowPageAction = new EditorSceneChangerShowSceneAction;
	editorActions_[edSceneChangerShowPageAction->actionType()] = edSceneChangerShowPageAction;	
	EditorSceneChangerSceneSetAction * edsss = new EditorSceneChangerSceneSetAction;
	editorActions_[edsss->actionType()] = edsss;
	EditorImageSetFileAction * edImgSetFileAction = new EditorImageSetFileAction;
	editorActions_[edImgSetFileAction->actionType()] = edImgSetFileAction;

	EditorImageChangerShowImageAction * edImageChangerShowPageAction = new EditorImageChangerShowImageAction;
	editorActions_[edImageChangerShowPageAction->actionType()] = edImageChangerShowPageAction;	
	EditorImageChangerSetImageAction * edImageChangerSetImageAcion = new EditorImageChangerSetImageAction;
	editorActions_[edImageChangerSetImageAcion->actionType()] = edImageChangerSetImageAcion;
	
	EditorSetVisibilityAction * edSva = new EditorSetVisibilityAction;
	editorActions_[edSva->actionType()] = edSva;
	EditorSetOpacityAction * edSoa = new EditorSetOpacityAction;
	editorActions_[edSoa->actionType()] = edSoa;

	EditorReceiveMessageEvent * edRecvMsgEvent = new EditorReceiveMessageEvent;
	editorEvents_[edRecvMsgEvent->eventType()] = edRecvMsgEvent;
	EditorImageGridImageClickedEvent * edImageGridImageClickedEvent = new EditorImageGridImageClickedEvent;
	editorEvents_[edImageGridImageClickedEvent->eventType()] = edImageGridImageClickedEvent;
	EditorSceneChangerPageChangedEvent * edSceneChangedPageChangedEvent = new EditorSceneChangerPageChangedEvent;
	editorEvents_[edSceneChangedPageChangedEvent->eventType()] = edSceneChangedPageChangedEvent;
	EditorAudioPlayTimeEvent * edAudioPlayTimeEvent = new EditorAudioPlayTimeEvent;
	editorEvents_[edAudioPlayTimeEvent->eventType()] = edAudioPlayTimeEvent;
	EditorVideoPlayTimeEvent * edVideoPlayTimeEvent = new EditorVideoPlayTimeEvent;
	editorEvents_[edVideoPlayTimeEvent->eventType()] = edVideoPlayTimeEvent;

	EditorSingleSceneDocumentTemplate * edSst = new EditorSingleSceneDocumentTemplate;
	editorDocumentTemplates_[edSst->templateType()] = edSst;
	EditorSceneChangerDocumentTemplate * edSct = new EditorSceneChangerDocumentTemplate;
	editorDocumentTemplates_[edSct->templateType()] = edSct;
	EditorMagazineDocumentTemplate * edMt = new EditorMagazineDocumentTemplate;
	editorDocumentTemplates_[edMt->templateType()] = edMt;
	EditorCatalogDocumentTemplate * edCt = new EditorCatalogDocumentTemplate;
	editorDocumentTemplates_[edCt->templateType()] = edCt;

	EditorAudioPlayAction * edAPA = new EditorAudioPlayAction;
	editorActions_[edAPA->actionType()] = edAPA;

	EditorVideoPlayAction * edVPA = new EditorVideoPlayAction;
	editorActions_[edVPA->actionType()] = edVPA;

	EditorShareAction * edShareAction = new EditorShareAction;
	editorActions_[edShareAction->actionType()] = edShareAction;

	EditorImageAnimationPauseAction * edIAPauseAction = new EditorImageAnimationPauseAction;
	editorActions_[edIAPauseAction->actionType()] = edIAPauseAction;

	EditorImageAnimationResumeAction * edIAResumeAction = new EditorImageAnimationResumeAction;
	editorActions_[edIAResumeAction->actionType()] = edIAResumeAction;

	setDirectories();

	HINSTANCE hDLL = LoadLibrary(L"GDI32.DLL");	
	getFontResourceInfoFunc_ = (PGFRI)GetProcAddress(hDLL, "GetFontResourceInfoW");

	HDC hdc = GetDC(0);
	logPixelsY_ = GetDeviceCaps(hdc, LOGPIXELSY);
	ReleaseDC(0, hdc);

	ran_.seed((unsigned int)time(0) % UINT_MAX);
}

EditorGlobal::~EditorGlobal()
{
	{
	std::map<boost::uuids::uuid, EditorObject *>::iterator iter;
	for (iter = editorObjs_.begin(); iter != editorObjs_.end(); ++iter)
		delete (*iter).second;
	}

	{
	std::map<boost::uuids::uuid, EditorAppObject *>::iterator iter;
	for (iter = editorAppObjs_.begin(); iter != editorAppObjs_.end(); ++iter)
		delete (*iter).second;
	}

	{
	std::map<boost::uuids::uuid, EditorAction *>::iterator iter;
	for (iter = editorActions_.begin(); iter != editorActions_.end(); ++iter)
		delete (*iter).second;
	}

	{
	std::map<boost::uuids::uuid, EditorEvent *>::iterator iter;
	for (iter = editorEvents_.begin(); iter != editorEvents_.end(); ++iter)
		delete (*iter).second;
	}

	{
	std::map<boost::uuids::uuid, EditorDocumentTemplate *>::iterator iter;
	for (iter = editorDocumentTemplates_.begin(); iter != editorDocumentTemplates_.end(); ++iter)
		delete (*iter).second;
	}

	{
	std::map<boost::uuids::uuid, EditorCustomObject *>::iterator iter;
	for (iter = editorCustomObjs_.begin(); iter != editorCustomObjs_.end(); ++iter)
		delete (*iter).second;
	}

}

BOOL EditorGlobal::getFontResourceInfo(LPCWSTR p1, DWORD * p2, LPVOID p3, DWORD p4)
{
	return getFontResourceInfoFunc_(p1, p2, p3, p4);
}

EditorCustomObject * EditorGlobal::createEditorCustomObject(
	EditorScene * edScene, EditorCustomObject * edCustomObj) const
{		
	EditorCustomObject * cloned = (EditorCustomObject *)edCustomObj->clone();	
	cloned->setScene(edScene);
	return cloned;
}

EditorObject * EditorGlobal::createEditorObject(
	EditorScene * edScene, SceneObject * sceneObj) const
{	
	if (sceneObj->type() == CustomObject().type())
	{
		CustomObject * customObj = (CustomObject *)sceneObj;
		std::map<boost::uuids::uuid, EditorCustomObject *>::const_iterator iter;
		iter = editorCustomObjs_.find(customObj->customType());

		if (iter != editorCustomObjs_.end())
		{
			EditorCustomObject * obj = (*iter).second;
			EditorCustomObject * cloned = (EditorCustomObject *)obj->clone();	
			cloned->setSceneObject(sceneObj);
			cloned->setScene(edScene);
			return cloned;
		}
	}
	else
	{
		std::map<boost::uuids::uuid, EditorObject *>::const_iterator iter;
		iter = editorObjs_.find(sceneObj->type());

		if (iter != editorObjs_.end())
		{
			EditorObject * obj = (*iter).second;
			EditorObject * cloned = obj->clone();
			cloned->setSceneObject(sceneObj);
			cloned->setScene(edScene);
			return cloned;
		}
	}

	//todo: create some kind of default editor object
	return 0;
}

EditorAppObject * EditorGlobal::createEditorAppObject(
	EditorScene * edScene, AppObject * appObject) const
{
	std::map<boost::uuids::uuid, EditorAppObject *>::const_iterator iter;
	iter = editorAppObjs_.find(appObject->type());

	if (iter != editorAppObjs_.end())
	{
		EditorAppObject * obj = (*iter).second;
		EditorAppObject * cloned = obj->clone();
		cloned->setAppObject(appObject);
		cloned->setScene(edScene);
		return cloned;
	}

	//todo: create some kind of default editor object
	return 0;
}


EditorAction * EditorGlobal::createEditorAction(
	EditorScene * edScene, Action * action) const
{
	std::map<boost::uuids::uuid, EditorAction *>::const_iterator iter;
	iter = editorActions_.find(action->type());

	if (iter != editorActions_.end())
	{
		EditorAction * obj = (*iter).second;
		EditorAction * cloned = obj->clone();
		cloned->setAction(action);
		cloned->setScene(edScene);
		return cloned;
	}
	return 0;
}

EditorAction * EditorGlobal::editorAction(Action * action) const
{
	std::map<boost::uuids::uuid, EditorAction *>::const_iterator iter;
	iter = editorActions_.find(action->type());

	if (iter != editorActions_.end()) return (*iter).second;
		
	return 0;
}

EditorDocumentTemplate * EditorGlobal::editorDocumentTemplate(
	DocumentTemplate * docTemplate) const
{
	std::map<boost::uuids::uuid, EditorDocumentTemplate *>::const_iterator iter;
	iter = editorDocumentTemplates_.find(docTemplate->type());

	if (iter != editorDocumentTemplates_.end()) return (*iter).second;
		
	return 0;
}

EditorEvent * EditorGlobal::editorEvent(Event * event) const
{
	std::map<boost::uuids::uuid, EditorEvent *>::const_iterator iter;
	iter = editorEvents_.find(event->type());

	if (iter != editorEvents_.end()) return (*iter).second;
		
	return 0;
}

void EditorGlobal::setDirectories()
{
	programDirectory_ = QDir::current().absolutePath();

}

void EditorGlobal::copyObjects(const std::vector<SceneObject *> & objects)
{
	clipboardObjMapping_.reset(new ElementMapping);
	clipboardObjs_.clear();
	BOOST_FOREACH(SceneObject * obj, objects)
	{
		clipboardObjs_.push_back(SceneObjectSPtr(
			obj->clone(clipboardObjMapping_.get())));
	}
}

vector<SceneObjectSPtr> EditorGlobal::
	pasteObjects(Document * document, Scene * scene) const
{
	vector<SceneObjectSPtr> pastedObjs;

	ElementMapping pasteMapping;

	BOOST_FOREACH(SceneObjectSPtr obj, clipboardObjs_)
	{
		pastedObjs.push_back(SceneObjectSPtr(
			obj->clone(&pasteMapping)));
	}

	pasteMapping.setDestinationData(document, scene);
	clipboardObjMapping_->setDestinationData(document, scene);

	BOOST_FOREACH(SceneObjectSPtr obj, pastedObjs)
	{
		obj->remapReferences(*clipboardObjMapping_);
		obj->remapReferences(pasteMapping);
	}

	return pastedObjs;
}


void EditorGlobal::copyActions(const std::vector<Action *> & actions)
{
	clipboardActions_.clear();
	BOOST_FOREACH(Action * obj, actions)
	{
		clipboardActions_.push_back(ActionSPtr(
			obj->clone()));
	}
}

vector<ActionSPtr> EditorGlobal::pasteActions(
	Document * document, Scene * scene) const
{
	vector<ActionSPtr> pastedObjs;

	ElementMapping pasteMapping;

	BOOST_FOREACH(ActionSPtr obj, clipboardActions_)
	{
		pastedObjs.push_back(ActionSPtr(obj->clone()));
	}

	pasteMapping.setDestinationData(document, scene);

	vector<ActionSPtr>::iterator iter;
	for (iter = pastedObjs.begin(); iter != pastedObjs.end();)
	{
		ActionSPtr a = *iter;
		if (!a->remapReferences(pasteMapping))
		{
			iter = pastedObjs.erase(iter);
		}
		else ++iter;
	}
	
	return pastedObjs;
}


void EditorGlobal::copyAnimChannels(const std::vector<AnimationChannel *> & channels)
{
	clipboardAnimChannels_.clear();
	BOOST_FOREACH(AnimationChannel * channel, channels)
	{
		clipboardAnimChannels_.push_back(AnimationChannelSPtr(
			channel->clone()));
	}
}

std::vector<AnimationChannelSPtr> EditorGlobal::pasteAnimChannels()
{
	vector<AnimationChannelSPtr> pastedObjs;

	ElementMapping pasteMapping;

	BOOST_FOREACH(AnimationChannelSPtr obj, clipboardAnimChannels_)
	{
		pastedObjs.push_back(AnimationChannelSPtr(obj->clone()));
	}
	
	return pastedObjs;
}


void EditorGlobal::copyScenes(const std::vector<Scene *> & scenes)
{
	clipboardSceneMapping_.reset(new ElementMapping);
	clipboardScenes_.clear();
	BOOST_FOREACH(Scene * scene, scenes)
	{
		clipboardScenes_.push_back(SceneSPtr(
			new Scene(*scene, clipboardSceneMapping_.get())));			
	}
}

vector<SceneSPtr> EditorGlobal::
	pasteScenes(Document * document) const
{
	vector<SceneSPtr> pastedScenes;

	ElementMapping pasteMapping;

	BOOST_FOREACH(SceneSPtr scene, clipboardScenes_)
	{
		pastedScenes.push_back(SceneSPtr(
			new Scene(*scene, &pasteMapping)));
	}

	pasteMapping.setDestinationData(document, 0);
	clipboardSceneMapping_->setDestinationData(document, 0);

	BOOST_FOREACH(SceneSPtr scene, pastedScenes)
	{
		scene->remapReferences(*clipboardSceneMapping_);
		scene->remapReferences(pasteMapping);
		scene->setName(scene->name() + "_copy");
	}

	return pastedScenes;
}

float EditorGlobal::gridSpacing() const
{	
	return settings_.value("gridSpacing", 20.0f).toFloat();
}

float EditorGlobal::jointDrawSize() const
{
	return 2.0f;
}

void EditorGlobal::setGridSpacing(float gridSpacing)
{
	settings_.setValue("gridSpacing", gridSpacing);
}

bool EditorGlobal::showGrid() const
{
	return settings_.value("showGrid", false).toBool();
}

void EditorGlobal::setShowGrid(bool showGrid)
{
	settings_.setValue("showGrid", showGrid);
}

Color EditorGlobal::sceneOutlineColor() const
{
	float r, g, b;
	r = settings_.value("sceneOutlineColorR", 0.8f).toFloat();
	g = settings_.value("sceneOutlineColorG", 0.8f).toFloat();
	b = settings_.value("sceneOutlineColorB", 0.8f).toFloat();

	return Color(r, g, b, 1.0f);
}

Color EditorGlobal::AnimationGuidlineColor() const
{
	float r, g, b;
	r = settings_.value("AnimationGuidelineColorR", 1.0f).toFloat();
	g = settings_.value("AnimationGuidelineColorG", 0.0f).toFloat();
	b = settings_.value("AnimationGuidelineColorB", 0.0f).toFloat();

	return Color(r, g, b, 1.0f);
}

void EditorGlobal::setTempfileInterval(int interval)
{
	settings_.setValue("tempfileInterval", interval);
}

int EditorGlobal::tempfileInterval() const
{
	return settings_.value("tempfileInterval", 5).toInt();;
}


void EditorGlobal::setSceneOutlineColor(const Color & color)
{
	settings_.setValue("sceneOutlineColorR", color.r);
	settings_.setValue("sceneOutlineColorG", color.g);
	settings_.setValue("sceneOutlineColorB", color.b);
}

boost::uuids::uuid EditorGlobal::randomUuid()
{
	boost::uuids::basic_random_generator<boost::mt19937> gen(&ran_);
	return gen();
}


void EditorGlobal::editorCustomObjects(std::vector<EditorCustomObject *> * editorCustomObjects) const
{
	map<boost::uuids::uuid, EditorCustomObject *>::const_iterator iter;
	for (iter = editorCustomObjs_.begin(); iter != editorCustomObjs_.end(); ++iter)
	{
		editorCustomObjects->push_back((*iter).second);
	}

	//todo sort
}

QString EditorGlobal::playerLocation() const
{
	if (settings_.value("player").isNull())
		return QString("C:/git_working/GlContentEditor/Debug/player.exe");
	return settings_.value("player").toString();
}

QString EditorGlobal::pdf2imgLocation() const 
{
#ifdef _DEBUG
	return QString("C:/git_working/GlContentEditor/Debug/pdf2img.exe");
#else
	return settings_.value("pdf2img").toString();
#endif
}

QString EditorGlobal::pdf2textLocation() const 
{
#ifdef _DEBUG
	return QString("C:/git_working/GlContentEditor/Debug/pdftoxml.exe");
#else
	return settings_.value("pdf2xml").toString();
#endif
}

QString EditorGlobal::xpdfrcLocation() const 
{
#ifdef _DEBUG
	return QString("C:/git_working/GlContentEditor/Debug/xpdfrc");
#else
	return settings_.value("xpdfrc").toString();
#endif
}