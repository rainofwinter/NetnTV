#pragma once
#include "Types.h"
#include "EditorTypes.h"
#include "MathStuff.h"
#include "Transform.h"
#include "Orientation.h"
#include "MagazineDocumentTemplate.h"
#include "Camera.h"
#include "SceneObject.h"
#include "Text.h"
#include "Text_2.h"

class KeyFrameData;
class GLWidget;

class Command : public QObject
{
	Q_OBJECT
public:
	Command() {}
	virtual ~Command() {}

	virtual void doCommand() = 0;
	virtual void undoCommand() = 0;

	virtual QString name() const = 0;

	virtual bool shouldMerge(Command * command) const {return false;}
	virtual void merge(Command * command) {}

	virtual std::vector<Command *> subCommands() const 
	{return std::vector<Command *>();}

signals:
	void sceneSelectionChanged();
	void sceneListChanged();
	void sceneChanged();
	
	void objectChanged();	
	void objectSelectionChanged();
	void objectListChanged();

	void appObjectChanged();
	void appObjectSelectionChanged();
	void appObjectListChanged();
	
	void listenerChanged();
	void listenerListChanged();
	void actionChanged(Action * oldAction, Action * newAction);
	void actionListChanged();
	void actionSelectionChanged();
	void animationListChanged();
	void animationChanged();
	void channelListChanged();
	void channelSelectionChanged();
	void channelChanged();

	void keyFrameTimeChanged(
		const std::vector<KeyFrameData> & oldKeyFrames, 
		const std::vector<float> & newTimes);	
		
	void keyFrameDeleted();
private:
};
///////////////////////////////////////////////////////////////////////////////
class EditorDocument;

enum ChangePropertyCmdType
{
	ChangeObjectProperty,
	ChangeAppObjectProperty,
	ChangeSceneProperty,
	ChangeAnimationProperty,
	ChangeDocumentProperty
};

template <typename ObjectType, typename PropertyType>
class ChangePropertyCmd : public Command
{
public:

public:	
	typedef const PropertyType & (ObjectType::*GetterFunc)() const;
	typedef void (ObjectType::*SetterFunc)(const PropertyType &);

	ChangePropertyCmd(
		ChangePropertyCmdType type,
		ObjectType * object, 
		const PropertyType & oldProperty, const PropertyType & newProperty, 		
		GetterFunc getProperty, SetterFunc setProperty);

	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change Property");}
	
private:

	ChangePropertyCmdType type_;
	const char * signalToEmit_;
	GetterFunc getProperty_;
	SetterFunc setProperty_;

	ObjectType * object_;
	PropertyType oldProperty_, newProperty_;
};
//------------------------------------------------------------------------------
template <typename ObjectType, typename PropertyType>

ChangePropertyCmd<ObjectType, PropertyType>::ChangePropertyCmd(
	ChangePropertyCmdType type,
	ObjectType * object, 
	const PropertyType & oldProperty, const PropertyType & newProperty,	
	GetterFunc getProperty, SetterFunc setProperty)
: 
type_(type),
object_(object), 
getProperty_(getProperty), 
setProperty_(setProperty),
oldProperty_(oldProperty), 
newProperty_(newProperty)

{}
//------------------------------------------------------------------------------
template <typename ObjectType, typename PropertyType>

void ChangePropertyCmd<ObjectType, PropertyType>::doCommand()
{
	(object_->*setProperty_)(newProperty_);
	switch (type_)
	{
	case ChangeSceneProperty:	
		emit sceneChanged(); break;
	case ChangeAnimationProperty:
		emit animationChanged(); break;
	case ChangeAppObjectProperty:
		emit appObjectChanged();
	default:
		emit objectChanged();
	}
}

template <typename ObjectType, typename PropertyType>
void ChangePropertyCmd<ObjectType, PropertyType>::undoCommand()
{
	(object_->*setProperty_)(oldProperty_);
	switch (type_)
	{
	case ChangeSceneProperty:	
		emit sceneChanged(); break;
	case ChangeAnimationProperty:
		emit animationChanged(); break;
	case ChangeAppObjectProperty:
		emit appObjectChanged();
	default:
		emit objectChanged();
	}
}
///////////////////////////////////////////////////////////////////////////////
template <typename ObjectType, typename PropertyType>
ChangePropertyCmd<ObjectType, PropertyType> * 
makeChangePropertyCmd(ChangePropertyCmdType type, ObjectType * object, 
	const PropertyType & oldProperty, const PropertyType & newProperty,
	const PropertyType & (ObjectType::*getterFunc)() const,
	void (ObjectType::*setterFunc)(const PropertyType &))
{
	return new ChangePropertyCmd<ObjectType, PropertyType>
		(type, object, oldProperty, newProperty, getterFunc, setterFunc);
}

template <typename ObjectType, typename PropertyType>
ChangePropertyCmd<ObjectType, PropertyType> * 
makeChangePropertyCmd(ChangePropertyCmdType type, ObjectType * object,	
	const PropertyType & property, 
	const PropertyType & (ObjectType::*getterFunc)() const,
	void (ObjectType::*setterFunc)(const PropertyType &)
	)
{
	return new ChangePropertyCmd<ObjectType, PropertyType>
		(type, object, (object->*getterFunc)(), property, getterFunc, setterFunc);
}
///////////////////////////////////////////////////////////////////////////////

/**
Like ChangePropertyCmd, but init is always called on the object
*/
template <typename ObjectType, typename PropertyType>
class ChangeObjectCmd : public Command
{
public:

public:	
	typedef const PropertyType & (ObjectType::*GetterFunc)() const;
	typedef void (ObjectType::*SetterFunc)(const PropertyType &);

	ChangeObjectCmd(
		ObjectType * object, 
		const PropertyType & oldProperty, const PropertyType & newProperty, 		
		GetterFunc getProperty, SetterFunc setProperty);

	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change Object");}
	
private:
	GetterFunc getProperty_;
	SetterFunc setProperty_;
	ObjectType * object_;
	PropertyType oldProperty_, newProperty_;
};
//------------------------------------------------------------------------------
template <typename ObjectType, typename PropertyType>

ChangeObjectCmd<ObjectType, PropertyType>::ChangeObjectCmd(
	ObjectType * object, 
	const PropertyType & oldProperty, const PropertyType & newProperty,	
	GetterFunc getProperty, SetterFunc setProperty)
: 
object_(object), 
getProperty_(getProperty), 
setProperty_(setProperty),
oldProperty_(oldProperty), 
newProperty_(newProperty)
{}
//------------------------------------------------------------------------------
template <typename ObjectType, typename PropertyType>

void ChangeObjectCmd<ObjectType, PropertyType>::doCommand()
{
	(object_->*setProperty_)(newProperty_);
	object_->init(object_->renderer());
	object_->asyncLoadUpdate();
	emit objectChanged();
	
}

template <typename ObjectType, typename PropertyType>
void ChangeObjectCmd<ObjectType, PropertyType>::undoCommand()
{
	(object_->*setProperty_)(oldProperty_);
	object_->init(object_->renderer());
	object_->asyncLoadUpdate();
	emit objectChanged();

}
///////////////////////////////////////////////////////////////////////////////
template <typename ObjectType, typename PropertyType>
ChangeObjectCmd<ObjectType, PropertyType> * 
makeChangeObjectCmd(ObjectType * object, 
	const PropertyType & property, 
	const PropertyType & (ObjectType::*getterFunc)() const,
	void (ObjectType::*setterFunc)(const PropertyType &)
	)
{
	return new ChangeObjectCmd<ObjectType, PropertyType>
		(object, (object->*getterFunc)(), property, getterFunc, setterFunc);
}






















///////////////////////////////////////////////////////////////////////////////
class Scene;
class Animation;

class NewAnimationCmd : public Command
{
public:
	NewAnimationCmd(Scene * scene, const AnimationSPtr & newAnimation);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("New animation");}
	
private:
	Scene * scene_;
	AnimationSPtr newAnimation_;
	EditorDocument * document_;
	
};

///////////////////////////////////////////////////////////////////////////////

class AnimationPropertiesCmd : public Command
{
public:
	AnimationPropertiesCmd(Animation * animation, bool repeat, SceneObject * syncObj);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Animation properties");}
	
private:
	Scene * scene_;
	Animation * animation_;

	bool origRepeat_, repeat_;
	SceneObject * origSyncObj_, * syncObj_;
	
};

///////////////////////////////////////////////////////////////////////////////

class DeleteAnimationsCmd : public Command
{
public:
	DeleteAnimationsCmd(Scene * scene, const std::vector<Animation *> & animations);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("New animation");}
	
private:
	Scene * scene_;
	std::vector<AnimationSPtr> animations_;
	std::vector<AnimationSPtr> backup_;
	EditorDocument * document_;
	
	std::map<Animation *, SceneObject *> syncObjsBackup_;


	typedef std::map<EventListener *, std::vector<ActionSPtr> > ActionsBackup;
	ActionsBackup actionsBackup_;
	
};

///////////////////////////////////////////////////////////////////////////////

class MoveAnimationsCmd : public Command
{
public:
	MoveAnimationsCmd(
		Scene * scene, const std::vector<Animation *> & objsToMove,
		Animation * moveTarget
		);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Move animations");}
	
private:
	Scene * scene_;
	std::vector<AnimationSPtr> objsToMove_;
	Animation * moveTarget_;
	std::vector<AnimationSPtr> objsBackup_;
	
};

///////////////////////////////////////////////////////////////////////////////
class Scene;
class SceneObject;
class AnimationChannel;

class MoveChannelsCmd : public Command
{
public:
	MoveChannelsCmd(Animation * animation,
		const std::vector<AnimationChannelSPtr> & objsToMove,
		AnimationChannel * moveTarget
		);
	~MoveChannelsCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Move animation channels");}
	
private:
	Animation * animation_;
	SceneObject * object_;	
	std::vector<AnimationChannelSPtr> objsToMove_;
	AnimationChannel * moveTarget_;
	std::vector<AnimationChannelSPtr> objsBackup_;
	
};

///////////////////////////////////////////////////////////////////////////////
class KeyFrame;

class SetKeyFramesCmd : public Command
{
public:
	SetKeyFramesCmd(EditorDocument * document, Animation * anim, 
		SceneObject * obj, const std::vector<AnimationChannelSPtr> & channels, 
		float time);
	~SetKeyFramesCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Set keyframes");}
	
private:
	Animation * animation_;
	SceneObject * object_;
	std::vector<int> channelIndices_;
	std::vector<AnimationChannelSPtr> oldChannels_;
	std::vector<AnimationChannelSPtr> channels_;
	std::vector<AnimationChannelSPtr> newChannels_;
	std::vector<KeyFrame *> removedKeyFrames_;
	EditorDocument * document_;
	float time_;

};

////////////////////////////////////////////////////////////////////////////////
class Interpolator;
class KeyFrameData;
class SetKeyFramePropertiesCmd : public Command
{
public:
	SetKeyFramePropertiesCmd(EditorDocument * document,
		const KeyFrameData & keyFrame, 
		float time, Interpolator * interpolator);
	~SetKeyFramePropertiesCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Set keyframe properties");}
	
private:
	KeyFrameData * keyFrame_;
	EditorDocument * document_;
	float time_, backupTime_;
	Interpolator * interpolator_, * backupInterpolator_;
	
	
}; 

/*class SetKeyFramePropertiesCmd : public Command
{
public:
	SetKeyFramePropertiesCmd(EditorDocument * document,
		const std::vector<KeyFrameData> & keyFrames,
		const std::vector<Interpolator *> & interpolators ,Interpolator * interpolator);
	~SetKeyFramePropertiesCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Set keyframe properties");}
	
private:
	std::vector<KeyFrameData> keyFrames_;
	std::vector<Interpolator *> interpolators_;
	std::vector<Interpolator *> backupInterpolators_;
	EditorDocument * document_;
	std::vector<float> backupTimes_;
	float time_;
	Interpolator * interpolator_;	
}; */

///////////////////////////////////////////////////////////////////////////////
class ChangeKeyFrameTimesCmd : public Command
{
public:
	ChangeKeyFrameTimesCmd(EditorDocument * document, 
		const std::vector<KeyFrameData> & keyFrames,
		float timeDelta);
	~ChangeKeyFrameTimesCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change keyframe times");}
	
private:
	EditorDocument * document_;
	std::vector<float> backupTimes_;
	std::vector<float> newTimes_;
	std::vector<KeyFrameData> keyFrames_;
	float timeDelta_;

};

////////////////////////////////////////////////////////////////////////////////


class DeleteKeyFramesCmd : public Command
{
public:
	DeleteKeyFramesCmd(EditorDocument * document, 
		const std::vector<KeyFrameData> & keyFrames);
	~DeleteKeyFramesCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Delete keyframes");}
	
private:
	std::map<AnimationChannel *, AnimationChannel *> channelBackups_;

	std::vector<KeyFrame *> removedKeyFrames_;
	std::vector<KeyFrameData> data_;
	EditorDocument * document_;	
	
};

////////////////////////////////////////////////////////////////////////////////
class NewChannelCmd : public Command
{
public:
	NewChannelCmd(Animation * animation, SceneObject * object,
		AnimationChannelSPtr newChannel);
	~NewChannelCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("New channel");}
	
private:
	SceneObject * object_;
	Animation * animation_;
	AnimationChannelSPtr newChannel_;
	std::vector<AnimationChannelSPtr> oldChannels_;
	
};

////////////////////////////////////////////////////////////////////////////////

class DeleteChannelsCmd : public Command
{
public:
	DeleteChannelsCmd(Animation * animation, SceneObject * object, 
		const std::vector<AnimationChannelSPtr> & channels);
	~DeleteChannelsCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Delete channels");}
	
private:
	SceneObject * object_;
	Animation * animation_;
	std::vector<AnimationChannelSPtr> delChannels_;
	std::vector<AnimationChannelSPtr> oldChannels_;
};


////////////////////////////////////////////////////////////////////////////////

class MoveSceneObjectsCmd : public Command
{
public:
	MoveSceneObjectsCmd(EditorDocument * document, SceneObject * parent, 
		int index, const std::vector<SceneObject *> & objs);
	~MoveSceneObjectsCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Reorder objects");}
	
private:
	EditorDocument * document_;
	Scene * scene_;
	SceneObject * parent_;
	SceneObject * moveTarget_;
	int index_;
	std::vector<SceneObject * > objs_;
	std::vector<SceneObjectSPtr> objsBackup_;
	typedef std::map<SceneObject *, std::vector<SceneObjectSPtr> > ObjChildrenMap;
	ObjChildrenMap childrenBackup_;

	typedef std::map<EventListener *, std::vector<ActionSPtr> > ActionsBackup;
	ActionsBackup actionsBackup_;

	std::vector<EditorObjectSPtr> editorObjectsBackup_;
	std::map<Action *, EditorActionSPtr> editorActionsBackup_;
	
};
////////////////////////////////////////////////////////////////////////////////
class AppObject;

class MoveAppObjectsCmd : public Command
{
public:
	MoveAppObjectsCmd(EditorDocument * document,  
		int index, const std::vector<AppObject *> & objs);
	~MoveAppObjectsCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Reorder app objects");}
	
private:
	EditorDocument * document_;
	Scene * scene_;
	AppObject * moveTarget_;
	int index_;
	std::vector<AppObject * > objs_;
	std::vector<AppObjectSPtr> objsBackup_;
	//std::vector<EditorObjectSPtr> editorObjectsBackup_;	
	
};

////////////////////////////////////////////////////////////////////////////////

class DeleteObjectsCmd : public Command
{
public:
	DeleteObjectsCmd(
		EditorDocument * document, Scene * scene, 
		const std::vector<SceneObject *> & delObjs);
	~DeleteObjectsCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Delete objects");}
private:
	///is obj2 a descdendant of obj1?
	bool isDescendant(SceneObject * obj1, SceneObject * obj2) const;
	
private:
	EditorDocument * document_;
	Scene * scene_;
	std::vector<SceneObject *> delObjs_;
	std::map<SceneObject *, std::vector<SceneObjectSPtr> > childrenBackup_;

	std::map<SceneObject *, std::vector<Animation *> > syncedAnimationsBackup_;

	struct AnimBackupKey
	{
		Animation * animation;
		SceneObject * object;
		AnimBackupKey(Animation * anim, SceneObject * obj)
		{
			this->animation = anim;
			this->object = obj;
		}
		bool operator < (const AnimBackupKey & rhs) const
		{
			if (animation != rhs.animation) 
				return animation < rhs.animation;
			else return object < rhs.object;
		}
	};
	std::map<AnimBackupKey, std::vector<AnimationChannelSPtr> > channelsBackup_;


	typedef std::map<EventListener *, std::vector<ActionSPtr> > ActionsBackup;
	ActionsBackup actionsBackup_;

	std::vector<EditorObjectSPtr> editorObjectsBackup_;
	std::map<Action *, EditorActionSPtr> editorActionsBackup_;
};

////////////////////////////////////////////////////////////////////////////////

class DeleteAppObjectsCmd : public Command
{
public:
	DeleteAppObjectsCmd(
		EditorDocument * document, Scene * scene, 
		const std::vector<AppObject *> & delObjs);
	~DeleteAppObjectsCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Delete app objects");}
	
private:
	EditorDocument * document_;
	Scene * scene_;
	std::vector<AppObject *> delObjs_;
	std::vector<AppObjectSPtr> objsBackup_;
	
	std::vector<EditorAppObjectSPtr> editorAppObjectsBackup_;
};

////////////////////////////////////////////////////////////////////////////////

class NewObjectsCmd : public Command
{
public:
	NewObjectsCmd(
		EditorDocument * document, SceneObject * parent, int index,
		const std::vector<SceneObjectSPtr> & newObjs);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("New object");}
	
private:
	EditorDocument * document_;
	Scene * scene_;
	std::vector<SceneObjectSPtr> newObjects_;	
	std::vector<EditorObjectSPtr> edObjects_;
	SceneObject * moveTarget_;
	SceneObject * parent_;

};

////////////////////////////////////////////////////////////////////////////////

class NewAppObjectsCmd : public Command
{
public:
	NewAppObjectsCmd(
		EditorDocument * document, int index,
		const std::vector<AppObjectSPtr> & newObjs);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("New object");}
	
private:
	EditorDocument * document_;
	Scene * scene_;
	std::vector<AppObjectSPtr> newObjects_;	
	std::vector<EditorAppObjectSPtr> edObjects_;
	AppObject * moveTarget_;

};


////////////////////////////////////////////////////////////////////////////////

class NewEventListenerCmd : public Command
{
public:
	NewEventListenerCmd(SceneObject * object,
		const EventListenerSPtr & newListener);
	NewEventListenerCmd(AppObject * object,
		const EventListenerSPtr & newListener);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("New event listener");}
	
private:
	SceneObject * object_;
	AppObject * appObj_;
	EventListenerSPtr newListener_;
	
};


////////////////////////////////////////////////////////////////////////////////

class NewActionCmd : public Command
{
public:
	NewActionCmd(EventListener * listener, const ActionSPtr & newAction);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("New action");}
	
private:
	EventListener * listener_;
	ActionSPtr newAction_;
	
};

///////////////////////////////////////////////////////////////////////////////

class ChangeActionCmd : public Command
{
public:
	ChangeActionCmd(EventListener * listener, 
		Action * oldAction, const ActionSPtr & newAction);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change action");}
	
private:
	EventListener * listener_;
	ActionSPtr newAction_, oldAction_;
	
};

///////////////////////////////////////////////////////////////////////////////
class EventListener;

class MoveActionsCmd : public Command
{
public:
	MoveActionsCmd(EventListener * listener,
		const std::vector<ActionSPtr> & objsToMove, Action * moveTarget);

	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Move actions");}
	
private:
	EventListener * listener_;
	Action * moveTarget_;
	std::vector<ActionSPtr> objsToMove_;	
	std::vector<ActionSPtr> objsBackup_;
	
};

////////////////////////////////////////////////////////////////////////////////

class DeleteActionsCmd : public Command
{
public:
	DeleteActionsCmd(EditorDocument * document, EventListener * listener, 
		const std::vector<ActionSPtr> & actions);

	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Delete channels");}
	
private:
	EditorDocument * document_;
	EventListener * listener_;
	std::vector<ActionSPtr> delActions_;
	std::vector<ActionSPtr> oldActions_;
	std::vector<EditorActionSPtr> edActionsBackup_;
};

////////////////////////////////////////////////////////////////////////////////

class DeleteEventListenersCmd : public Command
{
public:
	DeleteEventListenersCmd(SceneObject * object, 
		const std::vector<EventListenerSPtr> & delListeners);
	DeleteEventListenersCmd(AppObject * object, 
		const std::vector<EventListenerSPtr> & delListeners);

	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Delete event listeners");}
	
private:
	SceneObject * object_;
	AppObject * appObj_;
	std::vector<EventListenerSPtr> delListeners_;
	std::vector<EventListenerSPtr> oldListeners_;
};

///////////////////////////////////////////////////////////////////////////////
class EventListener;

class MoveEventListenersCmd : public Command
{
public:
	MoveEventListenersCmd(SceneObject * sceneObj,
		const std::vector<EventListenerSPtr> & objsToMove, EventListener * moveTarget);

	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Move event listeners");}
	
private:
	SceneObject * object_;
	EventListener * moveTarget_;
	std::vector<EventListenerSPtr> objsToMove_;	
	std::vector<EventListenerSPtr> objsBackup_;
	
};

///////////////////////////////////////////////////////////////////////////////
class ChangeEventListenerCmd : public Command
{
public:
	ChangeEventListenerCmd(EventListener * listener, const EventSPtr & newEvent);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change listener event");}
	
private:
	EventListener * listener_;
	EventSPtr newEvent_, oldEvent_;	
};

///////////////////////////////////////////////////////////////////////////////
class NudgeCmd : public Command
{
public:
	NudgeCmd(const std::vector<SceneObject *> & nudgeObjs, 
		const Vector3 & nudgeVec);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Nudge");}

	virtual bool shouldMerge(Command * command) const;
	virtual void merge(Command * command);
	
private:
	std::vector<SceneObject *> nudgeObjs_;
	std::vector<Transform> newTransforms_;
	std::vector<Transform> origTransforms_;
};

///////////////////////////////////////////////////////////////////////////////
class Document;
class NewSceneCmd : public Command
{
	
public:
	NewSceneCmd(EditorDocument * edDoc, int width, int height);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("New scene");}

	bool CompareNum(int lhs, int rhs);
	int findSceneCnt();

private:
	EditorDocument * edDoc_;
	EditorSceneSPtr edScene_;
	SceneSPtr scene_;	
	int width_, height_;
	Scene * curScene_;
	static int SceneCnt_;
	std::vector<Scene *> scenes_;
};
///////////////////////////////////////////////////////////////////////////////

class DeleteScenesCmd : public Command
{
public:
	DeleteScenesCmd(EditorDocument * edDoc, const std::vector<Scene *> & scenes);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Delete scenes");}
private:
	bool dependsOnADeletedScene(EventListener * listener) const;
	bool dependsOnADeletedScene(Action * action) const;
private:
	EditorDocument * edDoc_;
	std::set<Scene *> delScenes_;
	std::vector<SceneSPtr> backup_;

	struct ActionBackupKey
	{
		ActionBackupKey(Scene * scene, EventListener * listener)
		{
			this->scene = scene;
			this->listener = listener;
		}
		Scene * scene;
		EventListener * listener;
		bool operator < (const ActionBackupKey & rhs) const
		{
			if (scene != rhs.scene) return scene < rhs.scene;
			return listener < rhs.listener;
		}
	};
	typedef std::map<ActionBackupKey, std::vector<ActionSPtr> > ActionBackupMap;
	ActionBackupMap actionBackup_;
	
};

///////////////////////////////////////////////////////////////////////////////
class ChangeSceneCmd : public Command
{
public:
	ChangeSceneCmd(EditorDocument * edDoc, 
		const std::vector<Scene *> & oldScenes,
		const std::vector<Scene *> & newScenes);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change scene");}
	
private:
	EditorDocument * edDoc_;
	std::vector<Scene *> oldScenes_;
	std::vector<Scene *> newScenes_;
	
};

////////////////////////////////////////////////////////////////////////////////

class PasteObjectsCmd : public Command
{
public:
	PasteObjectsCmd(EditorDocument * document, 
		const std::vector<SceneObjectSPtr> & pasteObjs);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Paste objects");}
	
private:
	///the object to paste after (or null)
	SceneObject * refObj_;
	EditorDocument * document_;
	Scene * scene_;
	std::vector<SceneObjectSPtr> pasteObjs_;
	std::vector<EditorObjectSPtr> edObjs_;

};

////////////////////////////////////////////////////////////////////////////////

class PasteScenesCmd : public Command
{
public:
	PasteScenesCmd(
		EditorDocument * document, Scene * insertScene, const std::vector<SceneSPtr> & pasteScenes);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Paste scenes");}
	
private:
	EditorDocument * document_;
	Document * sceneDoc_;
	Scene * insertScene_;
	std::vector<SceneSPtr> pasteScenes_;
	std::vector<EditorSceneSPtr> edScenes_;

};

////////////////////////////////////////////////////////////////////////////////

class MoveScenesCmd : public Command
{
public:
	MoveScenesCmd(
		Document * document, const std::vector<Scene *> & objsToMove,
		Scene * moveTarget
		);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Move scenes");}
	
private:
	Document * document_;
	std::vector<SceneSPtr> objsToMove_;
	Scene * moveTarget_;
	std::vector<SceneSPtr> objsBackup_;
	
};

////////////////////////////////////////////////////////////////////////////////
class ChangeScenesPropertiesCmd : public Command
{
public:
	ChangeScenesPropertiesCmd(
		const std::vector<Scene *> & scenes, float width, float height, bool zoom, Color color
		);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change scene properties");}
	
private:
	std::vector<Scene *> scenes_;
	float width_, height_;
	bool zoom_;
	Color color_;
	struct Properties
	{
		Properties() {}
		Properties(float width, float height, bool zoom, Color color)
		{
			this->width = width;
			this->height = height;
			this->zoom = zoom;
			this->color = color;
		}
		float width, height;
		bool zoom;
		Color color;
	};
	std::map<Scene *, Properties> propertiesBackup_;
	
	
};

////////////////////////////////////////////////////////////////////////////////
class MagazineDocumentTemplate;
class MagazineArticle;
class ChangeArticlesCmd : public Command
{
public:
	ChangeArticlesCmd(
		Document * document,
		MagazineDocumentTemplate * articleManager, 
		const std::vector< boost::shared_ptr<MagazineArticle> > & articles,
		const std::vector< std::string > & uiImgFiles,
		bool resetArticleFirstPage,
		bool bookmarks, 
		bool aspect,
		MagazineDocumentTemplate::HomeButtonFunction homeButtonFunction,
		bool doToContents,
		int toContentsIndex,
		MagazineDocumentTemplate::TransitionMode transitionMode
		);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change articles");}
	
private:
	Document * document_;
	MagazineDocumentTemplate * object_;
	std::vector< boost::shared_ptr<MagazineArticle> > articles_;
	std::vector< boost::shared_ptr<MagazineArticle> > origArticles_;
	std::vector< std::string > origUiImgFiles_;	
	std::vector<std::string> uiImgFiles_;

	bool resetArticleFirstPage_;
	bool origResetArticleFirstPage_;
	bool bookmarks_;		
	bool origBookmarks_;
	bool aspect_;
	bool origAspect_;

	bool doToContents_, origDoToContents_;
	int toContentsIndex_, origToContentsIndex_;

	MagazineDocumentTemplate::HomeButtonFunction homeButtonFunction_;
	MagazineDocumentTemplate::HomeButtonFunction origHomeButtonFunction_;
	
	MagazineDocumentTemplate::TransitionMode transitionMode_;
	MagazineDocumentTemplate::TransitionMode origTransitionMode_;
	
};

////////////////////////////////////////////////////////////////////////////////
class Image;

class ChangeImageFileNameCmd : public Command
{
public:
	ChangeImageFileNameCmd(
		EditorDocument * document,
		Image * image, const std::string & fileName);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change image filename");}
	
private:
	EditorDocument * document_;
	Image * object_;
	std::string fileName_;
	
	std::string origFileName_;	
	int origWidth_;
	int origHeight_;
	
};
////////////////////////////////////////////////////////////////////////////////
class AppImage;

class ChangeAppImageFileNameCmd : public Command
{
public:
	ChangeAppImageFileNameCmd(EditorDocument * doc,
		AppImage * image, const std::string & fileName);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change app image filename");}
	
private:
	AppImage * object_;
	std::string fileName_;
	
	std::string origFileName_;	
	int origWidth_;
	int origHeight_;

	EditorDocument * doc_;
	
};

////////////////////////////////////////////////////////////////////////////////
/**
Address of transform gizmo must not change
*/
class GizmoTransformCmd : public Command
{
public:
	GizmoTransformCmd(
		const std::vector<SceneObject *> & objects,
		const std::vector<Transform> & initTransforms, 
		const std::vector<Transform> & transforms,
		Transform * overallTransform,
		const Transform & initOverallTransform, 
		const Transform & finalOverallTransform
		);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Set object transforms");}
	
private:
	std::vector<SceneObject *> objects_;
	std::vector<Transform> transforms_;
	std::vector<Transform> initTransforms_;

	Transform initOverallTransform_;
	Transform finalOverallTransform_;
	Transform * overallTransform_;
};



////////////////////////////////////////////////////////////////////////////////
class SceneChanger;
class ChangeSceneChangerScenesCmd : public Command
{
public:
	ChangeSceneChangerScenesCmd(
		EditorDocument * document,
		SceneChanger * sceneChanger, 
		const std::vector< Scene * > & scenes,
		bool allowDragging
		);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change scenes");}
	
private:
	EditorDocument * document_;
	SceneChanger * object_;
	std::vector<Scene *> scenes_;
	std::vector<Scene *> origScenes_;	
	bool allowDragging_;
	bool origAllowDragging_;
};


////////////////////////////////////////////////////////////////////////////////
class DocumentTemplate;

class ChangeDocumentPropertiesCmd : public Command
{
public:
	ChangeDocumentPropertiesCmd(EditorDocument * document, 
		Orientation orientation, int width, int height,
		const DocumentTemplateSPtr & docTemplate, 
		const std::vector<std::string> & scriptFiles,
		const std::map<std::string, std::string> & scriptAccessibleFiles,
		const std::string & remoteReadServer, const bool & allowMultitouch, const bool & preDownload);

	~ChangeDocumentPropertiesCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change document properties");}
	
private:
	EditorDocument * edDoc_;
	Document * document_;
	Orientation orientation_;
	int width_;
	int height_;
	std::vector<std::string> scriptFiles_;
	std::map<std::string, std::string> scriptAccessibleFiles_;
	std::string remoteReadServer_;

	Orientation origOrientation_;
	int origWidth_;
	int origHeight_;
	std::vector<std::string> origScriptFiles_;
	std::map<std::string, std::string> origScriptAccessibleFiles_;
	std::string origRemoteReadServer_;

	DocumentTemplateSPtr origDocTemplate_;
	DocumentTemplateSPtr docTemplate_;
		
	bool zBuffer_;
	bool origZBuffer_;

	bool multitouch_;
	bool origMultitouch_;

	bool preMultAlpha_;
	bool origPreMultAlpha_;

	bool preDownload_;
	bool origPreDownload_;
};

////////////////////////////////////////////////////////////////////////////////
class SingleSceneDocumentTemplate;


class ChangeSingleSceneDocumentCmd : public Command
{
public:
	ChangeSingleSceneDocumentCmd(
		Document * document,
		SingleSceneDocumentTemplate * docTemplate, 
		Scene * scene);

	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change document template properties");}
	
private:
	SingleSceneDocumentTemplate * docTemplate_;
	Scene * scene_;
	Scene * origScene_;
	Document * document_;
	
};


////////////////////////////////////////////////////////////////////////////////

class MultiCommandCmd : public Command
{
public:
	MultiCommandCmd(const QString & name, const std::vector<Command *> & cmds);
	~MultiCommandCmd();
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const;

	virtual std::vector<Command *> subCommands() const {return commands_;}

private:
	std::vector<Command *> commands_;
	QString name_;
};

////////////////////////////////////////////////////////////////////////////////
class SceneChangerDocumentTemplate;
class ChangeSceneChangerDocumentCmd : public Command
{
public:
	ChangeSceneChangerDocumentCmd(
		Document * document,
		SceneChangerDocumentTemplate * sceneChanger, 
		const std::vector< Scene * > & scenes, bool allowDragging);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change document template properties");}
	
private:
	SceneChangerDocumentTemplate * object_;
	std::vector<Scene *> scenes_;
	std::vector<Scene *> origScenes_;	
	bool origAllowDragging_;
	bool allowDragging_;
	Document * document_;
};

////////////////////////////////////////////////////////////////////////////////

class CatalogDocumentTemplate;
class ChangeCatalogDocumentCmd : public Command
{
public:
	ChangeCatalogDocumentCmd(
		Document * document,
		CatalogDocumentTemplate * catalog, 
		const std::vector< Scene * > & scenes,
		const std::vector< std::string > & uiImgFiles,
		const std::map<int, std::string> & thumbFileMap);

	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change Catalog");}
private:
	CatalogDocumentTemplate * object_;
	std::vector<Scene *> scenes_;
	std::vector<Scene *> origScenes_;	
	std::vector<std::string > origUiImgFiles_;	
	std::vector<std::string> uiImgFiles_;
	std::map<int, std::string> thumbFileMap_;
	std::map<int, std::string> origThumbFileMap_;
	Document * document_;
};

////////////////////////////////////////////////////////////////////////////////

class PasteActionsCmd : public Command
{
public:
	PasteActionsCmd(EditorDocument * document, EventListener * listener, 
		const std::vector<ActionSPtr> & pasteObjs);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Paste actions");}
	
private:
	///action to paste over
	Action * refAction_;
	EditorDocument * document_;
	EventListener * listener_;
	Scene * scene_;
	std::vector<ActionSPtr> pasteObjs_;

};

////////////////////////////////////////////////////////////////////////////////

class PasteAnimChannelsCmd : public Command
{
public:
	PasteAnimChannelsCmd(EditorDocument * document, 
		const std::vector<AnimationChannelSPtr> & pasteObjs);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Paste actions");}
	
private:
	EditorDocument * document_;
	SceneObject * obj_;
	Animation * anim_;
	std::vector<AnimationChannelSPtr> channelsBackup_;
	std::vector<AnimationChannelSPtr> pasteObjs_;

};

////////////////////////////////////////////////////////////////////////////////

class ChangeSceneCameraCmd : public Command
{
public:
	ChangeSceneCameraCmd(
		GLWidget * view, Scene * scene, CameraObject * camObject);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change camera");}
private:
	GLWidget * view_;
	Scene * scene_;
	CameraObject * origCamObject_;
	CameraObject * camObject_;

	Camera origCamState_;
	Camera camState_;


};

////////////////////////////////////////////////////////////////////////////////

class ChangeCameraTransformCmd : public Command
{
public:
	ChangeCameraTransformCmd(
		CameraObject * camObject, const Camera & oldCam, const Camera & newCam);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change camera transform");}
private:
	CameraObject * camObject_;
	Camera origCamera_;
	Camera camera_;

};

////////////////////////////////////////////////////////////////////////////////
class TextTable;
class TextTableState;
typedef boost::shared_ptr<TextTableState> TextTableStateSPtr;

class ChangeTextTableStateCmd : public Command
{
public:
	ChangeTextTableStateCmd(TextTable * textTableObj, 
		TextTableStateSPtr oldState, TextTableStateSPtr newState);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change text table state");}
private:
	TextTable * textTableObj_;
	boost::shared_ptr<TextTableState> origTableState_;
	boost::shared_ptr<TextTableState> newTableState_;

};

///////////////////////////////////////////////////////////////////////////////
class TextTableGridSizes;
typedef boost::shared_ptr<TextTableGridSizes> TextTableGridSizesSPtr;

class ChangeTextGridSizesCmd : public Command
{
public:
	ChangeTextGridSizesCmd(TextTable * textTableObj, 
		TextTableGridSizesSPtr oldGridSizes, TextTableGridSizesSPtr newGridSizes);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change text table grid sizes");}
private:
	TextTable * textTableObj_;
	TextTableGridSizesSPtr oldGridSizes_;
	TextTableGridSizesSPtr newGridSizes_;

};


///////////////////////////////////////////////////////////////////////////////

class SetTrackingUrlsCmd : public Command
{
public:
	SetTrackingUrlsCmd(const std::vector<Scene *> & scenes, const std::string & url);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Set page tracking urls");}
private:
	std::vector<Scene *> scenes_;
	std::map<Scene *, std::string> origUrls_;
	std::string url_;

};

////////////////////////////////////////////////////////////////////////////////
class Map;

class ChangeMapFileNameCmd : public Command
{
public:
	ChangeMapFileNameCmd(
		EditorDocument * document,
		Map * object, const std::string & mapFileName, const std::string & pathFileName);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change map filename");}

private:
	EditorDocument * document_;
	Map * object_;
	std::string fileName_;
	std::string pathFileName_;

	std::string origFileName_;	
	std::string origPathFileName_;
};

///////////////////////////////////////////////////////////////////////////////
class EditorMaskObject;

class ChangeShowMaskingCmd : public Command
{
public:
	ChangeShowMaskingCmd(EditorDocument * edDoc, EditorMaskObject * edMaskObj, bool val);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change show masking");}

private:
	EditorDocument * edDoc_;
	EditorMaskObject * edMaskObj_;
	bool oldVal_, newVal_;	

};
//////////////////////////////////////////////////////////////////

class PhotoObject;

class ChangePhotoImageFileNameCmd : public Command
{
public:
	ChangePhotoImageFileNameCmd(
		EditorDocument * document,
		PhotoObject * photoObject, const std::string & fileName);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change PhotoImage filename");}
	
private:
	EditorDocument * document_;
	PhotoObject * object_;
	std::string fileName_;
	
	std::string origFileName_;	
	int origWidth_;
	int origHeight_;
	
};
////////////////////////////////////////////////////////////////////////////////

class MergeTextsCmd : public Command
{
public:
	MergeTextsCmd(EditorDocument * document, Scene * scene, std::vector<Text_2*> texts);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Merge texts");}
private:
	void mergeText();
	bool isDescendant(SceneObject * obj1, SceneObject * obj2) const;
	void deleteText(std::vector<SceneObject *> delobjs);
private:
	EditorDocument * document_;
	Scene* scene_;

	std::vector<Text_2*> origTexts_;

	std::map<SceneObject *, std::vector<SceneObjectSPtr> > childrenBackup_;
	std::map<SceneObject *, std::vector<Animation *> > syncedAnimationsBackup_;
	struct AnimBackupKey
	{
		Animation * animation;
		SceneObject * object;
		AnimBackupKey(Animation * anim, SceneObject * obj)
		{
			this->animation = anim;
			this->object = obj;
		}
		bool operator < (const AnimBackupKey & rhs) const
		{
			if (animation != rhs.animation) 
				return animation < rhs.animation;
			else return object < rhs.object;
		}
	};
	std::map<AnimBackupKey, std::vector<AnimationChannelSPtr> > channelsBackup_;


	typedef std::map<EventListener *, std::vector<ActionSPtr> > ActionsBackup;
	ActionsBackup actionsBackup_;

	std::vector<EditorObjectSPtr> editorObjectsBackup_;
	std::map<Action *, EditorActionSPtr> editorActionsBackup_;

	std::vector<LineStyle> tempLinStyle;
	std::wstring tempStr;
	std::vector<TextProperties_2> tempPropertis;

};