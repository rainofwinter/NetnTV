#pragma once

#include "Types.h"
#include "EditorTypes.h"
#include "KeyFrame.h"

class CommandHistory;
class EditorScene;
class EditorObject;
class PropertyPage;
class Command;
class Scene;
class Animation;
class AnimationChannel;
class SceneObject;
class Document;
class GLWidget;
class Image;
class GfxRenderer;


class EditorDocument : public QObject
{
	Q_OBJECT
public:
	
public:
	EditorDocument(GLWidget * glWidget, Document * document);
	~EditorDocument();

	void init();

	Document * document() const {return document_;}

	EditorSceneSPtr editorScene(Scene * scene) const;	

	bool isAnimationSelected(const Animation * anim) const;
	void setSelectedAnimations(const std::vector<Animation *> & animation);	
	const std::vector<Animation *> & selectedAnimations() const 
	{return selectedAnimations_;}
	Animation * selectedAnimation() const;

	const std::vector<SceneSPtr> & scenes() const;	
	bool isSceneSelected(const Scene * scene) const;
	void setSelectedScenes(const std::vector<Scene *> & scenes);
	const std::vector<Scene *> & selectedScenes() const;
	Scene * selectedScene() const;
	void draw();

	GLWidget * glWidget() const {return glWidget_;}
	

	//-------------------------------------------------------------------------
	bool isChannelSelected(const AnimationChannel * channel) const;
	void setSelectedChannels(const std::vector<AnimationChannel *> & channels);
	const std::vector<AnimationChannel *> & selectedChannels() const
	{return selectedChannels_;}


	void setUpdateOnTimeChange(bool val)
	{updateOnTimeChange_ = val;}
	bool updateOnTimeChange() const {return updateOnTimeChange_;}

	float timeLineTime() const {return timeLineTime_;}
	void setTimeLineTime(float time);
	void scrubTimeLineTime(float time);

	//-------------------------------------------------------------------------
	const std::vector<KeyFrameData> & selectedKeyFrames() const
	{return selectedKeyFrames_;}

	void setSelectedKeyFrames(const std::vector<KeyFrameData> & keyFrames);
	bool isKeyFrameSelected(AnimationChannel * channel, int index) const;
	bool isKeyFrameSelected(const KeyFrameData & data) const;

	//-------------------------------------------------------------------------
	const std::vector<EventListener *> & selectedListeners() const
	{return selectedListeners_;}

	EventListener * selectedListener() const;
	void setSelectedListeners(const std::vector<EventListener *> & listeners);
	bool isListenerSelected(EventListener * listener) const;
	//-------------------------------------------------------------------------
	const std::vector<Action *> & selectedActions() const
	{return selectedActions_;}

	Action * selectedAction() const;
	void setSelectedActions(const std::vector<Action *> & actions);
	bool isActionSelected(Action * action) const;
	
	void addCommand(Command * command);
	void doCommand(Command * command);
	void undoCommand();
	void redoCommand();
	bool undoAvailable() const;
	bool redoAvailable() const;

	void setSelectedAppObjects(const std::vector<AppObject *> & objs);
	bool isAppObjectSelected(AppObject * object) const;

	void setSelectedObjects(const std::vector<SceneObject *> & objs);
	const std::vector<SceneObject *> & selectedObjects() const 
	{
		return selectedObjects_;
	}
	void setSelectedRecurse(SceneObject * object, bool val);

	const std::vector<AppObject *> selectedAppObjects() const
	{
		return selectedAppObjects_;
	}
	

	SceneObject * selectedObject() const
	{
		if (selectedObjects_.empty()) return 0;
		else return selectedObjects_.front();
	}

	AppObject * selectedAppObject() const
	{
		if (selectedAppObjects_.empty()) return 0;
		else return selectedAppObjects_.front();
	}

	bool isObjectSelected(SceneObject * object) const;
	bool isObjectAncestorSelected(SceneObject * object) const;

	void selectObject(SceneObject * editorObject);
	void selectAppObject(AppObject * appObject);
	void setObjectSelected(SceneObject * obj, bool selected);
	void setAppObjectSelected(AppObject * obj, bool selected);
	void deselectAll();
	void deselectAllAppObjects();

	PropertyPage * getPropertyPage(SceneObject * obj);
	PropertyPage * getPropertyPage(AppObject * obj);
		
	EditorObjectSPtr editorObject(SceneObject * obj);
	EditorAppObjectSPtr editorAppObject(AppObject * obj);
	EditorActionSPtr editorAction(Scene * scene, Action * action);

	void setDirty(bool dirty) {dirty_ = dirty;}

	void addEditorScene(const EditorSceneSPtr & edScene);
	void deleteEditorScene(EditorScene * edScene);
	
	void addEditorObject(const EditorObjectSPtr & edObj);
	void addEditorAppObject(const EditorAppObjectSPtr & edObj);
	void deleteEditorObject(EditorObject * edObj);
	void deleteEditorAppObject(EditorAppObject * edObj);

	void addEditorAction(const EditorActionSPtr & edAction);
	void deleteEditorAction(EditorAction * edAction);

	GfxRenderer * renderer() const;

	bool isDirty() const {return dirty_;}

	void setSceneListScrollPos(int pos) {sceneListScrollPos_ = pos;}
	int sceneListScrollPos() const {return sceneListScrollPos_;}
	
	void doUserCamChangeCmd(const Camera & origCam, const Camera & newCam);
	void callSceneListChanged() {emit sceneListChanged();}

	void texttoText_2Convert();

signals:	
	void sceneSelectionChanged();
	void sceneListChanged();
	void sceneChanged();
	void objectChanged();
	void appObjectChanged();
	void objectListChanged();
	void appObjectListChanged();
	void objectSelectionChanged();
	void appObjectSelectionChanged();
	void listenerChanged();
	void listenerListChanged();
	void actionChanged();
	void actionListChanged();
	void actionSelectionChanged();
	void listenerSelectionChanged();
	void animationSelectionChanged();
	void animationListChanged();
	void animationChanged();
	void channelSelectionChanged();
	void channelListChanged();
	void channelChanged();
	void keyFrameSelectionChanged();	
	void commandHistoryChanged();
	void timeLineTimeChanged();
	void timeLineTimeChanging();
	void toolChanged();
	void sceneDrawn();

private slots:
	void onKeyFrameTimeChanged(
		const std::vector<KeyFrameData> & oldKeyFrames, const std::vector<float> & newTimes);		
	void onKeyFrameDeleted();
	void onAnimationListChanged();
	void onChannelListChanged();
	void onActionChanged(Action * oldActionAddr, Action * newActionAddr);
	void onListenerListChanged();
	void onActionListChanged();
	void onObjectListChanged();
	void onAppObjectListChanged();
	void onSceneListChanged();

public:
	void setSelectedRecurseHelper(SceneObject * object, bool val);
	void handleAppObjSelChange();
	void handleObjSelChange();
	void handleListenerSelChange();
	void draw(SceneObject * sceneObject, GfxRenderer * gl);	
	void draw(AppObject * appObject, GfxRenderer * gl);
	void drawAniLine(SceneObject * sceneObject, GfxRenderer * gl);

private:

	mutable std::map<Scene *, EditorSceneSPtr> editorScenes_;

	CommandHistory * commandHistory_;

	std::vector<Scene *> selectedScenes_;
	std::vector<SceneObject *> selectedObjects_;		
	std::vector<Animation *> selectedAnimations_;
	std::vector<AnimationChannel *> selectedChannels_;
	std::vector<KeyFrameData> selectedKeyFrames_;
	std::vector<EventListener *> selectedListeners_;
	std::vector<Action *> selectedActions_;
	std::vector<AppObject *> selectedAppObjects_;
	
	mutable std::map<boost::uuids::uuid, PropertyPage *> objPageMap_;
	mutable std::map<boost::uuids::uuid, PropertyPage *> appObjPageMap_;

	Document * document_;
	GLWidget * glWidget_;

	///Whether to update the scene when the user changes the animation time	
	bool updateOnTimeChange_;

	float timeLineTime_;

	bool dirty_;
	bool needRedraw_;
	bool isTransAnimation_;

	int sceneListScrollPos_;

	std::vector<Vector3> KeyPoses_;
	std::vector<Vector3> pageVertices_;
	std::vector<KeyFrameData> KeyDatas_;

	std::vector<Vector3> animationLine_;
	Vector3 GuideLinePos_;	
};