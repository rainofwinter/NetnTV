#include "stdafx.h"
#include "Command.h"
#include "Document.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Animation.h"
#include "AnimationChannel.h"
#include "Exception.h"
#include "EditorDocument.h"
#include "EventListener.h"
#include "Event.h"
#include "Attrib.h"
#include "Action.h"
#include "ElementMapping.h"
#include "SceneChanger.h"
#include "Image.h"
#include "Utils.h"
#include "FileUtils.h"
#include "Texture.h"
#include "GlWidget.h"
#include "EditorAppImage.h"
#include "CameraObject.h"
#include "TextTable.h"
#include "EditorMaskObject.h"
#include "AppImage.h"
#include "PhotoObject.h"

#include "Map.h"

#include "EditorGlobal.h"

#include "SingleSceneDocumentTemplate.h"
#include "SceneChangerDocumentTemplate.h"
#include "CatalogDocumentTemplate.h"

Q_DECLARE_METATYPE(AnimationChannel *)

using namespace std;

///////////////////////////////////////////////////////////////////////////////

NewAnimationCmd::NewAnimationCmd(
	Scene * scene, const AnimationSPtr & newAnimation)
:
scene_(scene), newAnimation_(newAnimation)
{
}

void NewAnimationCmd::doCommand()
{	
	scene_->addAnimation(newAnimation_);
	emit animationListChanged();
}
void NewAnimationCmd::undoCommand()
{
	scene_->deleteAnimation(newAnimation_.get());
	emit animationListChanged();
}

///////////////////////////////////////////////////////////////////////////////

AnimationPropertiesCmd::AnimationPropertiesCmd(
	Animation * animation, bool repeat, SceneObject * syncObj)
{
	Scene * parentScene = animation->parentScene();

	animation_ = animation;
	origRepeat_ = animation->repeat();
	origSyncObj_ = parentScene->syncObject(animation);

	repeat_ = repeat;
	syncObj_ = syncObj;		
}

void AnimationPropertiesCmd::doCommand()
{	
	Scene * parentScene = animation_->parentScene();
	animation_->setRepeat(repeat_);
	parentScene->setSyncObject(animation_, syncObj_);
	emit sceneChanged();
	emit animationChanged();
}
void AnimationPropertiesCmd::undoCommand()
{	
	Scene * parentScene = animation_->parentScene();
	animation_->setRepeat(origRepeat_);
	parentScene->setSyncObject(animation_, origSyncObj_);
	emit sceneChanged();
	emit animationChanged();
}


///////////////////////////////////////////////////////////////////////////////

DeleteAnimationsCmd::DeleteAnimationsCmd(
	Scene * scene, const std::vector<Animation *> & animations)
:
scene_(scene)
{	
	BOOST_FOREACH(Animation * anim, animations)
		animations_.push_back(scene_->findAnimation(anim));

	vector<EventListenerSPtr> allListeners;
	scene->allListeners(&allListeners);
	
	BOOST_FOREACH(Animation * animation, animations)
	{
		BOOST_FOREACH(EventListenerSPtr listener, allListeners)
		{
			if (listener->dependsOn(animation)) 
				actionsBackup_[listener.get()] = listener->actions();
		}

		syncObjsBackup_[animation] = scene->syncObject(animation);
	}	


	backup_ = scene->animations();
}

void DeleteAnimationsCmd::doCommand()
{	
	BOOST_FOREACH(AnimationSPtr anim, animations_)
	{
		ActionsBackup::iterator iter = actionsBackup_.begin();
		for (;iter != actionsBackup_.end(); ++iter)
		{
			EventListener * listener = (*iter).first;
			BOOST_FOREACH(const ActionSPtr & action, (*iter).second)
			{
				//TODO delete editorAction
				if (action->dependsOn(anim.get()))
					listener->deleteAction(action.get());
			}
		}

		scene_->deleteAnimation(anim.get());
		scene_->removeSyncedAnimation(anim.get());
	}


	emit actionListChanged();
	emit animationListChanged();
	emit animationChanged();
}
void DeleteAnimationsCmd::undoCommand()
{
	scene_->setAnimations(backup_);
	ActionsBackup::iterator iter = actionsBackup_.begin();
	for (;iter != actionsBackup_.end(); ++iter)
	{
		EventListener * listener = (*iter).first;
		listener->setActions((*iter).second);
	}

	map<Animation *, SceneObject *>::iterator itr;
	for (itr = syncObjsBackup_.begin(); itr != syncObjsBackup_.end(); ++itr)
	{
		scene_->setSyncObject((*itr).first, (*itr).second);
	}
	
	emit animationListChanged();
	emit actionListChanged();
	emit animationChanged();
}


///////////////////////////////////////////////////////////////////////////////


MoveAnimationsCmd::MoveAnimationsCmd(
	Scene * scene, const std::vector<Animation *> & objsToMove,
	Animation * moveTarget)
:
scene_(scene), moveTarget_(moveTarget)
{	
	foreach(Animation * anim, objsToMove)
		objsToMove_.push_back(scene->findAnimation(anim));

	objsBackup_ = scene->animations();
}

void MoveAnimationsCmd::doCommand()
{	
	BOOST_FOREACH(AnimationSPtr obj, objsToMove_)	
		scene_->insertBefore(obj, moveTarget_);
		
	emit animationListChanged();
}
void MoveAnimationsCmd::undoCommand()
{
	scene_->setAnimations(objsBackup_);
	emit animationListChanged();
}


///////////////////////////////////////////////////////////////////////////////

MoveChannelsCmd::MoveChannelsCmd(Animation * animation,
	const std::vector<AnimationChannelSPtr> & objsToMove, 
	AnimationChannel * moveTarget)
:
animation_(animation), objsToMove_(objsToMove), moveTarget_(moveTarget)
{	
	if (objsToMove.empty()) throw Exception("No channels to move");
	object_ = objsToMove.front()->object();	
	objsBackup_ = *animation_->channels(object_);
}

MoveChannelsCmd::~MoveChannelsCmd()
{
}

void MoveChannelsCmd::doCommand()
{			
	BOOST_FOREACH(AnimationChannelSPtr obj, objsToMove_)
		animation_->insertBefore(obj, moveTarget_);

	emit channelListChanged();

}
void MoveChannelsCmd::undoCommand()
{
	animation_->setChannels(object_, objsBackup_);
	emit channelListChanged();
}

///////////////////////////////////////////////////////////////////////////////
SetKeyFramesCmd::SetKeyFramesCmd(EditorDocument * document, Animation * anim,
	SceneObject * obj, const std::vector<AnimationChannelSPtr> & channels, 
	float time)
:
time_(time), document_(document), animation_(anim), object_(obj), 
channels_(channels)
{

	const vector<AnimationChannelSPtr> * curChannels = anim->channels(obj);

	foreach(AnimationChannelSPtr channel, channels)
		oldChannels_.push_back(AnimationChannelSPtr(channel->clone()));


	foreach(AnimationChannelSPtr channel, channels)
	{
		AnimationChannel * newChannel = channel->clone();
		newChannels_.push_back(AnimationChannelSPtr(newChannel));

		KeyFrame * oldKeyFrame = newChannel->keyFrame(time);
		if (oldKeyFrame)
		{
			newChannel->removeKeyFrame(oldKeyFrame);
			removedKeyFrames_.push_back(oldKeyFrame);			
		}
		else
			removedKeyFrames_.push_back(0);		

		newChannel->setKeyFrame(time);		
	}
}

SetKeyFramesCmd::~SetKeyFramesCmd()
{
	BOOST_FOREACH(KeyFrame * keyframe, removedKeyFrames_)
		delete keyframe;
}

void SetKeyFramesCmd::doCommand()
{		
	for (int i = 0; i < (int)channels_.size(); ++i)
		channels_[i]->equals(*newChannels_[i]);

	emit channelChanged();
}
void SetKeyFramesCmd::undoCommand()
{
	for (int i = 0; i < (int)channels_.size(); ++i)
		channels_[i]->equals(*oldChannels_[i]);
	emit keyFrameDeleted();	
	emit channelChanged();
}
////////////////////////////////////////////////////////////////////////////////
SetKeyFramePropertiesCmd::SetKeyFramePropertiesCmd(
	EditorDocument * document, const KeyFrameData & keyFrame, 
	float time, Interpolator * interpolator)
:
time_(time), interpolator_(interpolator)
{
	document_ = document;
	keyFrame_ = new KeyFrameData(keyFrame);
	backupInterpolator_ = keyFrame.ptr()->interpolator();
	backupTime_ = keyFrame.ptr()->time();	
}

SetKeyFramePropertiesCmd::~SetKeyFramePropertiesCmd()
{
	delete keyFrame_;
}

void SetKeyFramePropertiesCmd::doCommand()
{
	KeyFrameAccess access(*keyFrame_);
	if (backupTime_ != time_)
	{
		
		if (access.setTime(time_) != Ok)
		{
			throw Exception("Key frame with the same time already exists");
		}
			
		std::vector<float> newTimes;
		std::vector<KeyFrameData> oldKeyFrames;
		oldKeyFrames.push_back(KeyFrameData(*keyFrame_));
		newTimes.push_back(time_);

		keyFrame_->time = time_;

		emit keyFrameTimeChanged(oldKeyFrames, newTimes);
	}

	access.setInterpolator(interpolator_);
	emit channelChanged();
}
void SetKeyFramePropertiesCmd::undoCommand()
{
	std::vector<float> newTimes;
	std::vector<KeyFrameData> oldKeyFrames;
	oldKeyFrames.push_back(KeyFrameData(*keyFrame_));
	newTimes.push_back(backupTime_);


	float time;
	KeyFrameAccess access(*keyFrame_);
	access.time(&time);
	access.setTime(backupTime_);
	access.setInterpolator(backupInterpolator_);
	keyFrame_->time = backupTime_;

	

	emit keyFrameTimeChanged(oldKeyFrames, newTimes);
	emit channelChanged();
} 
/*
SetKeyFramePropertiesCmd::SetKeyFramePropertiesCmd(EditorDocument * document,
		const std::vector<KeyFrameData> & keyFrames,
		const std::vector<Interpolator *> & interpolators ,Interpolator * interpolator)
:
keyFrames_(keyFrames), interpolators_(interpolators)
{	
	BOOST_FOREACH(KeyFrameData data, keyFrames_)
	{
		backupTimes_.push_back(data.time);
	}

	for (int i = 0; i < keyFrames.size() ;i++ )
	{
		backupInterpolators_[i] = keyFrames[i].ptr()->interpolator();
	}
}

SetKeyFramePropertiesCmd::~SetKeyFramePropertiesCmd()
{
}

void SetKeyFramePropertiesCmd::doCommand()
{
	std::vector<KeyFrameData> oldKeyFrames = keyFrames_;

	for (int i = 0; i <(int)keyFrames_.size() ; i++ )
	{
		KeyFrameAccess access(keyFrames_[i]);
		access.setTime(backupTimes_[i]);
		keyFrames_[i].time = backupTimes_[i];
		interpolators_[i] = backupInterpolators_[i];
	}

	emit keyFrameTimeChanged(oldKeyFrames, backupTimes_);
	emit channelChanged();
}

void SetKeyFramePropertiesCmd::undoCommand()
{
	std::vector<KeyFrameData> oldKeyFrames = keyFrames_;

	for (int i = (int)keyFrames_.size() - 1; i >= 0; i-- )
	{
		KeyFrameAccess access(keyFrames_[i]);
		access.setTime(backupTimes_[i]);
		keyFrames_[i].time = backupTimes_[i];
		interpolators_[i] = backupInterpolators_[i];
	}

	emit keyFrameTimeChanged(oldKeyFrames, backupTimes_);
	emit channelChanged();
} */
////////////////////////////////////////////////////////////////////////////////
bool keyFrameCompare(bool reverse, const KeyFrameData & lhs, const KeyFrameData & rhs)
{
	if (reverse)
		return lhs.time > rhs.time;
	else
		return lhs.time < rhs.time;
}


ChangeKeyFrameTimesCmd::ChangeKeyFrameTimesCmd(EditorDocument * document, 
	const std::vector<KeyFrameData> & keyFrames, float timeDelta)
:
document_(document), keyFrames_(keyFrames), timeDelta_(timeDelta)
{	
	bool reverse = false;
	if (timeDelta > 0) reverse = true;

	std::sort(keyFrames_.begin(), keyFrames_.end(), 		
		boost::bind(keyFrameCompare, reverse, _1, _2));

	BOOST_FOREACH(KeyFrameData data, keyFrames_)
	{
		backupTimes_.push_back(data.time);
		newTimes_.push_back(data.time + timeDelta);
	}

	if (keyFrames_.empty())
		throw Exception("No keyframes");

	//check for keyframe time conflicts
	BOOST_FOREACH(KeyFrameData data, keyFrames_)
	{
		KeyFrameAccess access(data);				
		
		KeyFrame * existing = data.channel->keyFrame(data.time + timeDelta);		
		
		bool falseAlarm = false;

		BOOST_FOREACH(KeyFrameData testData, keyFrames_)
		{
			if (testData.ptr() == existing) falseAlarm = true;
		}

		if (existing && !falseAlarm)
			throw Exception("Key frame with that time already exists");	
	}
}

ChangeKeyFrameTimesCmd::~ChangeKeyFrameTimesCmd()
{
}

void ChangeKeyFrameTimesCmd::doCommand()
{
	std::vector<KeyFrameData> oldKeyFrames = keyFrames_;	
		
	for (int i = 0; i < (int)keyFrames_.size(); ++i)
	{
		KeyFrameAccess access(keyFrames_[i]);
		access.setTime(newTimes_[i]);
		keyFrames_[i].time = newTimes_[i];
	}

	emit keyFrameTimeChanged(oldKeyFrames, newTimes_);
	emit channelChanged();
}
void ChangeKeyFrameTimesCmd::undoCommand()
{
	std::vector<KeyFrameData> oldKeyFrames = keyFrames_;

	for (int i = (int)keyFrames_.size() - 1; i >= 0; --i)
	{
		KeyFrameAccess access(keyFrames_[i]);
		access.setTime(backupTimes_[i]);
		keyFrames_[i].time = backupTimes_[i];
	}

	emit keyFrameTimeChanged(oldKeyFrames, backupTimes_);
	emit channelChanged();
}

////////////////////////////////////////////////////////////////////////////////
DeleteKeyFramesCmd::DeleteKeyFramesCmd(
	EditorDocument * document, const std::vector<KeyFrameData> & keyFrames)
{	
	data_ = keyFrames;
	document_ = document;
}

DeleteKeyFramesCmd::~DeleteKeyFramesCmd()
{

	foreach(KeyFrame * key, removedKeyFrames_) delete key;
}

void DeleteKeyFramesCmd::doCommand()
{	
	foreach(KeyFrameData data, data_)
	{
		KeyFrame * key = data.ptr();
		data.channel->removeKeyFrame(key);		
		removedKeyFrames_.push_back(key);
	}
	emit keyFrameDeleted();	
	emit channelChanged();
}
void DeleteKeyFramesCmd::undoCommand()
{
	for (int i = 0; i < (int)removedKeyFrames_.size(); ++i)
	{
		data_[i].channel->insertKeyFrame(removedKeyFrames_[i]);
	}
	removedKeyFrames_.clear();
	emit channelChanged();
}

////////////////////////////////////////////////////////////////////////////////
NewChannelCmd::NewChannelCmd(Animation * animation, SceneObject * object, 
	AnimationChannelSPtr newChannel)
{
	animation_ = animation;
	object_ = object;
	newChannel_ = newChannel;
	const vector<AnimationChannelSPtr> * channels = animation_->channels(object_);
	if (channels)
	{
		oldChannels_ = *channels;		
	}
}


NewChannelCmd::~NewChannelCmd()
{	
}

void NewChannelCmd::doCommand()
{	
	animation_->addChannel(newChannel_);
	emit channelListChanged();
}
void NewChannelCmd::undoCommand()
{
	animation_->setChannels(object_, oldChannels_);
	emit channelListChanged();
}

////////////////////////////////////////////////////////////////////////////////

DeleteChannelsCmd::DeleteChannelsCmd(Animation * animation, 
	SceneObject * object, const vector<AnimationChannelSPtr> & channels)
:	
animation_(animation), object_(object)
{
	delChannels_ = channels;
	oldChannels_ = *animation_->channels(object_);
}

DeleteChannelsCmd::~DeleteChannelsCmd()
{	
}

void DeleteChannelsCmd::doCommand()
{	
	foreach(AnimationChannelSPtr channel, delChannels_)
		animation_->deleteChannel(channel.get());

	emit channelListChanged();

	//need to refresh object list because highlighting of objects affected by 
	//animation might need to be changed
	emit objectListChanged();
}
void DeleteChannelsCmd::undoCommand()
{
	animation_->setChannels(object_, oldChannels_);
	emit channelListChanged();	
	emit objectListChanged();
}

////////////////////////////////////////////////////////////////////////////////

MoveSceneObjectsCmd::MoveSceneObjectsCmd(
	EditorDocument * document, SceneObject * parent, 
	int index, const std::vector<SceneObject *> & objs)
:	
document_(document), parent_(parent), index_(index), objs_(objs)
{
	scene_ = document_->selectedScene();
	objsBackup_ = parent->children();

	int i = 0;
	moveTarget_ = 0;
	BOOST_FOREACH(SceneObjectSPtr child, parent->children())
	{
		if (i == index)
			moveTarget_ = child.get();
		++i;
	}

	vector<SceneObject *>::iterator iter = objs_.begin();
	while(iter != objs_.end())
	{
		if (*iter == moveTarget_) iter = objs_.erase(iter);		
		else ++iter;
	}
	
	BOOST_FOREACH(SceneObject * obj, objs_)
	{
		SceneObject * parent = obj->parent();
		if (childrenBackup_.find(parent) == childrenBackup_.end())
		{
			childrenBackup_[parent] = parent->children();
		}
	}


	vector<EventListenerSPtr> allListeners;
	scene_->allListeners(&allListeners);
	
	BOOST_FOREACH(SceneObject * obj, objs_)
	{
		BOOST_FOREACH(EventListenerSPtr listener, allListeners)
		{
			if (listener->dependsOn(obj)) 
			{
				actionsBackup_[listener.get()] = listener->actions();
				
				BOOST_FOREACH(ActionSPtr action, listener->actions())
				{
					if (action->dependsOn(obj->parent(), obj))
						editorActionsBackup_[action.get()] = 
						document_->editorAction(scene_, action.get());
				}


			}
		}
	}	

	
}

MoveSceneObjectsCmd::~MoveSceneObjectsCmd()
{	
}

void MoveSceneObjectsCmd::doCommand()
{
	try {
	BOOST_FOREACH(SceneObject * obj, objs_)
	{
		SceneObject * oldParent = obj->parent();

		//delete actions appropriately
		ActionsBackup::iterator iter = actionsBackup_.begin();
		for (;iter != actionsBackup_.end(); ++iter)
		{
			EventListener * listener = (*iter).first;
			BOOST_FOREACH(const ActionSPtr & action, (*iter).second)
			{
				if (parent_ != oldParent && action->dependsOn(oldParent, obj))
				{					
					listener->deleteAction(action.get());
					document_->deleteEditorAction(
						editorActionsBackup_[action.get()].get());
				}
			}
		}



		SceneObjectSPtr objSPtr = oldParent->deleteChild(obj);
		parent_->insertBefore(objSPtr, moveTarget_);
	}
	} catch (Exception & e) {
	undoCommand();
	throw e;
	}



	emit actionListChanged();
	emit objectListChanged();	
}
void MoveSceneObjectsCmd::undoCommand()
{
	parent_->setChildren(objsBackup_);

	ObjChildrenMap::iterator iter = childrenBackup_.begin();
	for (; iter != childrenBackup_.end(); ++iter)
	{
		SceneObject * parent = (*iter).first;
		vector<SceneObjectSPtr> & children = (*iter).second;
		parent->setChildren(children);
	}

	//restore actions
	ActionsBackup::iterator iterA = actionsBackup_.begin();
	for (;iterA != actionsBackup_.end(); ++iterA)
	{
		EventListener * listener = (*iterA).first;
		listener->setActions((*iterA).second);
	}

	map<Action *, EditorActionSPtr>::iterator iterE = editorActionsBackup_.begin();
	for (;iterE != editorActionsBackup_.end(); ++iterE)
	{
		document_->addEditorAction((*iterE).second);
	}
	
	emit actionListChanged();
	emit objectListChanged();	
}
////////////////////////////////////////////////////////////////////////////////

MoveAppObjectsCmd::MoveAppObjectsCmd(
	EditorDocument * document, 
	int index, const std::vector<AppObject *> & objs)
:	
document_(document), index_(index), objs_(objs)
{
	scene_ = document_->selectedScene();
	objsBackup_ = scene_->appObjects();

	moveTarget_ = 0;
	if (0 <= index && index < scene_->appObjects().size())		
		moveTarget_ = scene_->appObjects()[index].get();

	vector<AppObject *>::iterator iter = objs_.begin();
	while(iter != objs_.end())
	{
		if (*iter == moveTarget_) iter = objs_.erase(iter);		
		else ++iter;
	}		
}

MoveAppObjectsCmd::~MoveAppObjectsCmd()
{	
}

void MoveAppObjectsCmd::doCommand()
{
	try {
	BOOST_FOREACH(AppObject * obj, objs_)
	{
		AppObjectSPtr objSPtr = scene_->deleteAppObject(obj);
		scene_->insertBefore(objSPtr, moveTarget_);
	}
	} catch (Exception & e) {
	undoCommand();
	throw e;
	}

	emit appObjectListChanged();	
}
void MoveAppObjectsCmd::undoCommand()
{
	scene_->setAppObjects(objsBackup_);
	
	
	emit appObjectListChanged();	
}

////////////////////////////////////////////////////////////////////////////////

DeleteObjectsCmd::DeleteObjectsCmd(EditorDocument * document, Scene * scene, 
	const vector<SceneObject *> & delObjs)
:	
scene_(scene), delObjs_(delObjs), document_(document)
{	
	//make sure none of the objects to be deleted is the current camera
	BOOST_FOREACH(SceneObject * obj, delObjs)
	{
		if ((SceneObject *)scene->userCamera() == obj)
		{
			throw Exception("Can't delete the current camera");
		}
	}

	vector<AnimationSPtr> animations = scene->animations();
	vector<SceneObject *> animObjs;
	BOOST_FOREACH(SceneObject * obj, delObjs)
	{
		BOOST_FOREACH(AnimationSPtr animation, animations)
		{
			animation->objects(&animObjs);

			BOOST_FOREACH(SceneObject * animObj, animObjs)
			{
				if (isDescendant(obj, animObj) || obj == animObj)
				{
					AnimBackupKey d(animation.get(), animObj);
					channelsBackup_[d] = *animation->channels(animObj);
				}
			}
			
		}
	}

	vector<SceneObject *>::iterator iter = delObjs_.begin();
	while (iter != delObjs_.end())
	{
		SceneObject * curObject = *iter;
		bool removeCur = false;
		BOOST_FOREACH(SceneObject * obj, delObjs_)		
			if (isDescendant(obj, curObject)) removeCur = true;		

		if (removeCur) iter = delObjs_.erase(iter);		
		else ++iter;
	}

	BOOST_FOREACH(SceneObject * obj, delObjs_)
	{
		childrenBackup_[obj->parent()] = obj->parent()->children();
		editorObjectsBackup_.push_back(document_->editorObject(obj));

		syncedAnimationsBackup_[obj] = scene_->syncedAnimations(obj);		
	}


	vector<EventListenerSPtr> allListeners;
	scene->allListeners(&allListeners);
	
	BOOST_FOREACH(SceneObject * obj, delObjs_)
	{
		BOOST_FOREACH(EventListenerSPtr listener, allListeners)
		{
			if (listener->dependsOn(obj)) 
			{
				actionsBackup_[listener.get()] = listener->actions();
				
				BOOST_FOREACH(ActionSPtr action, listener->actions())
				{
					if (action->dependsOn(obj))
						editorActionsBackup_[action.get()] = 
						document_->editorAction(scene_, action.get());
				}
			}
		}
	}	
	
}

DeleteObjectsCmd::~DeleteObjectsCmd()
{	
}

bool DeleteObjectsCmd::isDescendant(SceneObject * obj1, SceneObject * obj2) const
{
	SceneObject * ancestor = obj2;
	while (ancestor)
	{
		ancestor = ancestor->parent();
		if (ancestor == obj1) return true;
	}
	return false;
}

void DeleteObjectsCmd::doCommand()
{	
	BOOST_FOREACH(SceneObject * obj, delObjs_)
	{
		ActionsBackup::iterator iter = actionsBackup_.begin();
		for (;iter != actionsBackup_.end(); ++iter)
		{
			EventListener * listener = (*iter).first;
			BOOST_FOREACH(const ActionSPtr & action, (*iter).second)
			{
				if (action->dependsOn(obj))
				{					
					listener->deleteAction(action.get());
					document_->deleteEditorAction(
						editorActionsBackup_[action.get()].get());
				}
			}
		}

		obj->parent()->deleteChild(obj);	
		document_->setObjectSelected(obj, false);

		scene_->removeSyncObject(obj);
	}

	BOOST_FOREACH(EditorObjectSPtr edObj, editorObjectsBackup_)
	{
		if (edObj) document_->deleteEditorObject(edObj.get());
	}

	map<AnimBackupKey, vector<AnimationChannelSPtr> >::iterator chanIter;
	for (chanIter = channelsBackup_.begin(); chanIter != channelsBackup_.end();
		++chanIter)
	{
		Animation * anim = (*chanIter).first.animation;
		SceneObject * obj = (*chanIter).first.object;
		anim->setChannels(obj, vector<AnimationChannelSPtr>());
	}


	emit animationChanged();
	emit actionListChanged();
	emit channelListChanged();	
	emit objectListChanged();
}
void DeleteObjectsCmd::undoCommand()
{
	map<SceneObject *, vector<SceneObjectSPtr> >::iterator iter;
	for (iter = childrenBackup_.begin(); iter != childrenBackup_.end(); ++iter)
	{
		SceneObject * parent = (*iter).first;
		vector<SceneObjectSPtr> & children = (*iter).second;

		parent->setChildren(children);
	}

	BOOST_FOREACH(EditorObjectSPtr edObj, editorObjectsBackup_)
	{
		if (edObj) document_->addEditorObject(edObj);
	}

	map<AnimBackupKey, vector<AnimationChannelSPtr> >::iterator chanIter;
	for (chanIter = channelsBackup_.begin(); chanIter != channelsBackup_.end();
		++chanIter)
	{
		Animation * anim = (*chanIter).first.animation;
		SceneObject * obj = (*chanIter).first.object;
		vector<AnimationChannelSPtr> & channels = (*chanIter).second;
		anim->setChannels(obj, channels);
	}

	//restore actions
	ActionsBackup::iterator iterA = actionsBackup_.begin();
	for (;iterA != actionsBackup_.end(); ++iterA)
	{
		EventListener * listener = (*iterA).first;
		listener->setActions((*iterA).second);
	}

	map<Action *, EditorActionSPtr>::iterator iterE = editorActionsBackup_.begin();
	for (;iterE != editorActionsBackup_.end(); ++iterE)
	{
		document_->addEditorAction((*iterE).second);
	}

	//restore synced animations

	map<SceneObject *, vector<Animation *> >::iterator itr;
	for (itr = syncedAnimationsBackup_.begin(); itr != syncedAnimationsBackup_.end(); ++itr)
	{
		SceneObject * syncObj = (*itr).first;
		BOOST_FOREACH(Animation * anim, (*itr).second)
		{
			scene_->setSyncObject(anim, syncObj);
		}
	}
	
	emit animationChanged();
	emit actionListChanged();
	emit channelListChanged();
	emit objectListChanged();
}

////////////////////////////////////////////////////////////////////////////////

DeleteAppObjectsCmd::DeleteAppObjectsCmd(EditorDocument * document, Scene * scene, 
	const vector<AppObject *> & delObjs)
:	
scene_(scene), delObjs_(delObjs), document_(document)
{	
	objsBackup_ = scene->appObjects();

	BOOST_FOREACH(AppObject * obj, delObjs_)
	{
		editorAppObjectsBackup_.push_back(document_->editorAppObject(obj));
	}
}

DeleteAppObjectsCmd::~DeleteAppObjectsCmd()
{	
}

void DeleteAppObjectsCmd::doCommand()
{	
	BOOST_FOREACH(AppObject * obj, delObjs_)
	{
		scene_->deleteAppObject(obj);
	}

	BOOST_FOREACH(EditorAppObjectSPtr edObj, editorAppObjectsBackup_)
	{
		if (edObj) document_->deleteEditorAppObject(edObj.get());
	}

	emit appObjectListChanged();
}
void DeleteAppObjectsCmd::undoCommand()
{	
	BOOST_FOREACH(EditorAppObjectSPtr edObj, editorAppObjectsBackup_)
	{
		if (edObj) document_->addEditorAppObject(edObj);
	}

	scene_->setAppObjects(objsBackup_);

	emit appObjectListChanged();
}

////////////////////////////////////////////////////////////////////////////////

NewObjectsCmd::NewObjectsCmd(
	EditorDocument * document, SceneObject * parent, int index,
	const std::vector<SceneObjectSPtr> & newObjs)
:	
document_(document), scene_(document->selectedScene()), 
newObjects_(newObjs), parent_(parent)
{	
	moveTarget_ = 0;
	int i = 0;
	BOOST_FOREACH(SceneObjectSPtr child, parent->children())
	{
		if (i == index)
			moveTarget_ = child.get();
		++i;
	}
}

void NewObjectsCmd::doCommand()
{	
	vector<SceneObject *> selObjs;
	BOOST_FOREACH(SceneObjectSPtr newObj, newObjects_)
	{		
		parent_->insertBefore(newObj, moveTarget_);
		selObjs.push_back(newObj.get());
		edObjects_.push_back(document_->editorObject(newObj.get()));

		newObj->init(document_->document()->renderer());
	}

	document_->setSelectedObjects(selObjs);
	
	emit objectListChanged();
}
void NewObjectsCmd::undoCommand()
{
	for (int i = 0; i < (int)newObjects_.size(); ++i)
	{
		newObjects_[i]->uninit();
		newObjects_[i]->parent()->deleteChild(newObjects_[i].get());
		document_->deleteEditorObject(edObjects_[i].get());		
	}

	emit objectListChanged();
}

////////////////////////////////////////////////////////////////////////////////

NewAppObjectsCmd::NewAppObjectsCmd(
	EditorDocument * document, int index,
	const std::vector<AppObjectSPtr> & newObjs)
:	
document_(document), scene_(document->selectedScene()), 
newObjects_(newObjs)
{	/*
	if (scene_->userCamera() != NULL)
	{
		throw Exception("Scenes with custom cameras cannot contain app objects");
	}

	if (scene_->zBuffer())
	{
		throw Exception("Scenes z-buffering enabled cannot contain app objects");
	}
	*/

	moveTarget_ = 0;
	int i = 0;
	BOOST_FOREACH(AppObjectSPtr child, scene_->appObjects())
	{
		if (i == index)
			moveTarget_ = child.get();
		++i;
	}
}

void NewAppObjectsCmd::doCommand()
{	
	vector<AppObject *> selObjs;
	BOOST_FOREACH(AppObjectSPtr newObj, newObjects_)
	{
		scene_->insertBefore(newObj, moveTarget_);
		selObjs.push_back(newObj.get());
		edObjects_.push_back(document_->editorAppObject(newObj.get()));
	}

	document_->setSelectedAppObjects(selObjs);
	
	emit appObjectSelectionChanged();
	emit appObjectListChanged();
}

void NewAppObjectsCmd::undoCommand()
{
	for (int i = 0; i < (int)newObjects_.size(); ++i)
	{
		scene_->deleteAppObject(newObjects_[i].get());
		document_->deleteEditorAppObject(edObjects_[i].get());
	}

	emit appObjectListChanged();
}

////////////////////////////////////////////////////////////////////////////////
NewEventListenerCmd::NewEventListenerCmd(SceneObject * object, 
	const EventListenerSPtr & newListener)
{
	appObj_ = NULL;
	object_ = object;
	newListener_ = newListener;	
}
NewEventListenerCmd::NewEventListenerCmd(AppObject * object, 
	const EventListenerSPtr & newListener)
{
	appObj_ = object;
	newListener_ = newListener;	
}

void NewEventListenerCmd::doCommand()
{
	if (appObj_)
		appObj_->addListener(newListener_);
	else
		object_->addListener(newListener_);
	emit listenerListChanged();
}
void NewEventListenerCmd::undoCommand()
{
	if (appObj_)
		appObj_->deleteListener(newListener_.get());
	else
		object_->deleteListener(newListener_.get());
	emit listenerListChanged();
}


////////////////////////////////////////////////////////////////////////////////
NewActionCmd::NewActionCmd(
	EventListener * listener, const ActionSPtr & newAction)
{
	listener_ = listener;
	newAction_ = newAction;
}

void NewActionCmd::doCommand()
{	
	listener_->addAction(newAction_);	
	emit actionListChanged();
}
void NewActionCmd::undoCommand()
{
	//TODO delete editorAction
	listener_->deleteAction(newAction_.get());
	emit actionListChanged();
}

////////////////////////////////////////////////////////////////////////////////
ChangeActionCmd::ChangeActionCmd(
	EventListener * listener, Action * oldAction, const ActionSPtr & newAction)
{
	listener_ = listener;
	oldAction_ = listener->findAction(oldAction);
	newAction_ = newAction;
}

void ChangeActionCmd::doCommand()
{	
	listener_->changeAction(oldAction_.get(), newAction_);	
	emit actionChanged(oldAction_.get(), newAction_.get());
}
void ChangeActionCmd::undoCommand()
{
	listener_->changeAction(newAction_.get(), oldAction_);	
	emit actionChanged(newAction_.get(), oldAction_.get());
}

///////////////////////////////////////////////////////////////////////////////

MoveActionsCmd::MoveActionsCmd(EventListener * listener,
	const std::vector<ActionSPtr> & objsToMove, Action * moveTarget)
:
listener_(listener), objsToMove_(objsToMove), moveTarget_(moveTarget)
{
	objsBackup_ = listener->actions();
}


void MoveActionsCmd::doCommand()
{			
	BOOST_FOREACH(ActionSPtr obj, objsToMove_)
		listener_->insertBefore(obj, moveTarget_);

	emit actionListChanged();

}
void MoveActionsCmd::undoCommand()
{
	listener_->setActions(objsBackup_);
	emit actionListChanged();
}

////////////////////////////////////////////////////////////////////////////////

DeleteActionsCmd::DeleteActionsCmd(
	EditorDocument * document, 							   
	EventListener * listener, 
	const vector<ActionSPtr> & actions)
:	
delActions_(actions), listener_(listener), document_(document)
{
	oldActions_ = listener_->actions();
	Scene * scene = document->selectedScene();
	BOOST_FOREACH(ActionSPtr action, actions)
	{
		EditorActionSPtr edAction = 
			document_->editorAction(scene, action.get());
		if (edAction)
			edActionsBackup_.push_back(edAction);
	}
}

void DeleteActionsCmd::doCommand()
{
	BOOST_FOREACH(ActionSPtr action, delActions_)
	{
		listener_->deleteAction(action.get());
	}

	BOOST_FOREACH(EditorActionSPtr edAction, edActionsBackup_)
	{		
		document_->deleteEditorAction(edAction.get());
	}

	emit actionListChanged();	
}
void DeleteActionsCmd::undoCommand()
{
	listener_->setActions(oldActions_);

	BOOST_FOREACH(EditorActionSPtr edAction, edActionsBackup_)
	{
		document_->addEditorAction(edAction);
	}

	emit actionListChanged();	
}

////////////////////////////////////////////////////////////////////////////////

DeleteEventListenersCmd::DeleteEventListenersCmd(SceneObject * object, 
		const std::vector<EventListenerSPtr> & delListeners)
:	
delListeners_(delListeners), object_(object)
{
	oldListeners_ = object->eventListeners();
	appObj_ = NULL;
}

DeleteEventListenersCmd::DeleteEventListenersCmd(AppObject * object, 
		const std::vector<EventListenerSPtr> & delListeners)
:	
delListeners_(delListeners), appObj_(object)
{
	oldListeners_ = object->eventListeners();
}

void DeleteEventListenersCmd::doCommand()
{
	if (appObj_)
	{
		foreach(EventListenerSPtr listener, delListeners_)
			appObj_->deleteListener(listener.get());
	}
	else
	{
		foreach(EventListenerSPtr listener, delListeners_)
			object_->deleteListener(listener.get());
	}

	emit listenerListChanged();	
}
void DeleteEventListenersCmd::undoCommand()
{
	if (appObj_)
	{
		appObj_->setEventListeners(oldListeners_);
	}
	else
	{
		object_->setEventListeners(oldListeners_);
	}

	emit listenerListChanged();	
}

///////////////////////////////////////////////////////////////////////////////

MoveEventListenersCmd::MoveEventListenersCmd(SceneObject * sceneObj,
	const std::vector<EventListenerSPtr> & objsToMove, EventListener * moveTarget)
:
object_(sceneObj), objsToMove_(objsToMove), moveTarget_(moveTarget)
{
	objsBackup_ = object_->eventListeners();
}


void MoveEventListenersCmd::doCommand()
{			
	BOOST_FOREACH(EventListenerSPtr obj, objsToMove_)
		object_->insertBefore(obj, moveTarget_);

	emit listenerListChanged();	

}
void MoveEventListenersCmd::undoCommand()
{
	object_->setEventListeners(objsBackup_);
	emit listenerListChanged();	
}
////////////////////////////////////////////////////////////////////////////////
ChangeEventListenerCmd::ChangeEventListenerCmd(
	EventListener * listener, const EventSPtr & newEvent)
{
	listener_ = listener;
	oldEvent_.reset(listener->event()->clone());
	newEvent_ = newEvent;
}

void ChangeEventListenerCmd::doCommand()
{	
	listener_->setEvent(newEvent_);
	emit listenerChanged();
}
void ChangeEventListenerCmd::undoCommand()
{
	listener_->setEvent(oldEvent_);
	emit listenerChanged();
}

////////////////////////////////////////////////////////////////////////////////
NudgeCmd::NudgeCmd(
	const vector<SceneObject *> & nudgeObjs, 
	const Vector3 & nudgeVec)
{
	nudgeObjs_ = nudgeObjs;

	BOOST_FOREACH(SceneObject * obj, nudgeObjs)
	{
		VisualAttrib * attr = obj->visualAttrib();
		if (!attr) continue;
		origTransforms_.push_back(attr->transform());

		Transform newTransform = attr->transform();
		Vector3 newPos = newTransform.translation() + nudgeVec;
		newTransform.setTranslation(ceil(newPos.x), ceil(newPos.y), ceil(newPos.z));
		newTransforms_.push_back(newTransform);
		
	}
}

void NudgeCmd::doCommand()
{
	int i = 0;
	BOOST_FOREACH(SceneObject * obj, nudgeObjs_)
	{
		VisualAttrib * attr = obj->visualAttrib();
		if (!attr) continue;		
		attr->setTransform(newTransforms_[i++]);		
	}
	emit objectChanged();
}

void NudgeCmd::undoCommand()
{
	int i = 0;
	BOOST_FOREACH(SceneObject * obj, nudgeObjs_)
	{
		VisualAttrib * attr = obj->visualAttrib();
		if (!attr) continue;		
		attr->setTransform(origTransforms_[i++]);		
	}

	emit objectChanged();
}

bool NudgeCmd::shouldMerge(Command * command) const
{
	return typeid(*command) == typeid(NudgeCmd);
}

void NudgeCmd::merge(Command * command)
{
	NudgeCmd * nudgeCmd = (NudgeCmd *)command;
	newTransforms_ = nudgeCmd->newTransforms_;
}

///////////////////////////////////////////////////////////////////////////////
int NewSceneCmd::SceneCnt_=1;
NewSceneCmd::NewSceneCmd(EditorDocument * edDoc, int width, int height)
:
edDoc_(edDoc)
{		
	scene_.reset(new Scene);
	scene_->setScreenWidth(width);
	scene_->setScreenHeight(height);	
	curScene_ = edDoc->selectedScene();
	
	const vector<SceneSPtr> & scenes = edDoc->document()->scenes();
	BOOST_FOREACH(SceneSPtr scene, scenes)
	{		
		scenes_.push_back(scene.get());		
	}
	
	int nSceneNum = findSceneCnt();
		
	std::stringstream ss;
	ss << "Scene" << nSceneNum;
	scene_->setName(ss.str());	
}

bool NewSceneCmd::CompareNum(int lhs, int rhs)
{
	return lhs < rhs;
}

int NewSceneCmd::findSceneCnt()
{
	int nNum = 0;
	int nKeyNum = 20000000;	//maximum int
	int nTempNum = 0;
	vector<int> SceneNum;
	
	BOOST_FOREACH(const Scene *scene, scenes_)
	{
		std::string st = scene->name();
		std::string stNum;

		if(st.length()>4)
			stNum = st.substr( 5 ,st.length());	///////		
		
		std::stringstream strStream(stNum);
		strStream >> nNum;											
		SceneNum.push_back(nNum);			
		
	}	
	
	//std::sort(SceneNum.begin(),SceneNum.end());						//sort descending
	std::sort(SceneNum.begin(),SceneNum.end(),greater<int>());			//sort ascending
		
	int SceneCnt = 0;
	BOOST_FOREACH(int &iterSearcher, SceneNum)
	{
		int nGap =  nTempNum - iterSearcher;
		
		switch(nGap)
		{
			case 0:
				continue;
			case 1:				
				nTempNum = iterSearcher;	break;
			default:
				nKeyNum = iterSearcher;	
				nTempNum = iterSearcher;	break;
		}			
		if(iterSearcher == nKeyNum)
			continue;
		++SceneCnt;	
	}
	if(SceneCnt < 1)
		nKeyNum = 0;
	
	return ++nKeyNum;
}


void NewSceneCmd::doCommand()
{	
	edDoc_->document()->addScene(scene_);
	
	Scene * refScene = NULL;
	bool reachedScene = false;
	BOOST_FOREACH(SceneSPtr curScene, edDoc_->document()->scenes())
	{
		if (reachedScene)
		{
			refScene = curScene.get();
			break;
		}
		if (curScene.get() == curScene_) reachedScene = true;			
	}

	edDoc_->document()->insertBefore(scene_, refScene);
	vector<Scene *> selScenes;
	selScenes.push_back(scene_.get());
	edDoc_->setSelectedScenes(selScenes);
	edScene_ = edDoc_->editorScene(scene_.get());
	emit sceneListChanged();
	emit sceneSelectionChanged();
}
void NewSceneCmd::undoCommand()
{
	edDoc_->document()->deleteScene(scene_.get());
	edDoc_->deleteEditorScene(edScene_.get());
	emit sceneListChanged();
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
bool contains(T obj, vector<T> objs)
{
	return find(objs.begin(), objs.end(), obj) != objs.end();
}

bool DeleteScenesCmd::dependsOnADeletedScene(EventListener * listener) const
{
	BOOST_FOREACH(Scene * delScene, delScenes_)	
		if (listener->dependsOn(delScene)) return true;
	
	return false;
}

bool DeleteScenesCmd::dependsOnADeletedScene(Action * action) const
{
	BOOST_FOREACH(Scene * delScene, delScenes_)	
		if (action->dependsOn(delScene)) return true;
	
	return false;
}


DeleteScenesCmd::DeleteScenesCmd(
	EditorDocument * edDoc, const std::vector<Scene *> & delScenes)
:
edDoc_(edDoc)
{		
	Scene * sceneToSelect = 0;
	bool reachedDelScene = false;
	BOOST_REVERSE_FOREACH(const SceneSPtr & scene, edDoc->document()->scenes())
	{
		bool isGoingToBeDeleted = false;
		BOOST_FOREACH(Scene * delScene, delScenes)
		{
			if (delScene == scene.get()) isGoingToBeDeleted = true;
		}

		if (isGoingToBeDeleted) reachedDelScene = true;
		if (reachedDelScene && sceneToSelect) break;
		if (!isGoingToBeDeleted) sceneToSelect = scene.get();
	}


	BOOST_FOREACH(Scene * scene, delScenes) delScenes_.insert(scene);

	if (delScenes.size() >= edDoc_->document()->scenes().size())
		throw Exception("Cannot delete all scenes in the document");
	backup_ = edDoc_->document()->scenes();

	BOOST_FOREACH(SceneSPtr scene, backup_)
	{
		BOOST_FOREACH(Scene * delScene, delScenes_)
		{
			if (scene.get() == delScene) continue;
			if (scene->dependsOn(delScene) && delScenes_.find(scene.get()) == delScenes_.end())
			{				
				throw Exception("Cannot delete a scene that is referenced in another scene");
			}
		}

	}

	BOOST_FOREACH(Scene * delScene, delScenes_)
	{		
		if (edDoc->document()->documentTemplate()->dependsOn(delScene))		
			throw Exception("Cannot delete a scene that is referenced in the document template");
		
	}

	const vector<SceneSPtr> & docScenes = edDoc->document()->scenes();

	vector<EventListenerSPtr> listeners;
	BOOST_FOREACH(SceneSPtr scene, docScenes)
	{		
		if (contains(scene.get(), delScenes)) continue;

		scene->allListeners(&listeners);
		BOOST_FOREACH(EventListenerSPtr listener, listeners)
		{
			if (dependsOnADeletedScene(listener.get()))
			{
				actionBackup_[ActionBackupKey(scene.get(), listener.get())] = 
					listener->actions();
			}
		}
	}

	vector<Scene *> selScenes;
	selScenes.push_back(sceneToSelect);
	edDoc_->setSelectedScenes(selScenes);

}

void DeleteScenesCmd::doCommand()
{	
	BOOST_FOREACH(Scene * scene, delScenes_) 
		edDoc_->document()->deleteScene(scene);

	ActionBackupMap::iterator iter = actionBackup_.begin();
	for (; iter != actionBackup_.end(); ++iter)
	{
		Scene * scene = (*iter).first.scene;
		EventListener * listener = (*iter).first.listener;

		BOOST_FOREACH(ActionSPtr action, (*iter).second)
		{
			//TODO delete editorAction
			if (dependsOnADeletedScene(action.get()))
				listener->deleteAction(action.get());
		}

	}
/*
	vector<Scene *> selScenes;
	selScenes.push_back(edDoc_->document()->scenes().front().get());
	edDoc_->setSelectedScenes(selScenes);
*/
	emit actionListChanged();
	emit sceneListChanged();
}
void DeleteScenesCmd::undoCommand()
{
	edDoc_->document()->setScenes(
		edDoc_->document()->documentTemplate(),
		backup_);

	ActionBackupMap::iterator iter = actionBackup_.begin();
	for (; iter != actionBackup_.end(); ++iter)
	{
		Scene * scene = (*iter).first.scene;
		EventListener * listener = (*iter).first.listener;

		listener->setActions((*iter).second);
	}
	

	emit actionListChanged();
	emit sceneListChanged();
}

///////////////////////////////////////////////////////////////////////////////

ChangeSceneCmd::ChangeSceneCmd(EditorDocument * edDoc, 
	const std::vector<Scene *> & oldScenes,
	const std::vector<Scene *> & newScenes)
:
edDoc_(edDoc), oldScenes_(oldScenes), newScenes_(newScenes)
{	
}

void ChangeSceneCmd::doCommand()
{	
	edDoc_->setSelectedScenes(newScenes_);
	emit sceneSelectionChanged();
}
void ChangeSceneCmd::undoCommand()
{
	edDoc_->setSelectedScenes(oldScenes_);
	emit sceneSelectionChanged();
}

////////////////////////////////////////////////////////////////////////////////

PasteObjectsCmd::PasteObjectsCmd(
	EditorDocument * document, const std::vector<SceneObjectSPtr> & pasteObjs)
:
document_(document), scene_(document->selectedScene()), pasteObjs_(pasteObjs)
{		
	refObj_ = document_->selectedObject();
}

void PasteObjectsCmd::doCommand()
{	
	edObjs_.clear();
	vector<SceneObject *> selObjs;


	SceneObject * refParent = (SceneObject *)scene_->root();
	if (refObj_)
	{
		refParent = refObj_->parent();
		BOOST_FOREACH(SceneObjectSPtr obj, pasteObjs_)
		{
			refParent->insertAfter(obj, refObj_);
			Document * sceneDoc = scene_->parentDocument();
			obj->init(document_->renderer());
			obj->start(0);
			edObjs_.push_back(document_->editorObject(obj.get()));
			selObjs.push_back(obj.get());
		}	
	}

	else
	{
		BOOST_FOREACH(SceneObjectSPtr obj, pasteObjs_)
		{
			refParent->insertBefore(obj, refObj_);
			Document * sceneDoc = scene_->parentDocument();
			obj->init(document_->renderer());
			obj->start(0);
			edObjs_.push_back(document_->editorObject(obj.get()));
			selObjs.push_back(obj.get());
			refObj_ = obj.get();
			refParent = refObj_->parent();
		}
	}

	document_->setSelectedObjects(selObjs);



	emit objectSelectionChanged();
	emit objectListChanged();
}
void PasteObjectsCmd::undoCommand()
{
	BOOST_FOREACH(SceneObjectSPtr obj, pasteObjs_)
	{
		obj->uninit();
		obj->parent()->deleteChild(obj.get());		
	}

	BOOST_FOREACH(EditorObjectSPtr edObj, edObjs_)
	{
		document_->deleteEditorObject(edObj.get());
	}

	emit objectListChanged();
}
////////////////////////////////////////////////////////////////////////////////

PasteScenesCmd::PasteScenesCmd(
	EditorDocument * document, Scene * insertScene, const std::vector<SceneSPtr> & pasteScenes)
:	
document_(document), sceneDoc_(document->document()), pasteScenes_(pasteScenes)
{		
	const vector<SceneSPtr> & scenes = document->scenes();
	vector<SceneSPtr>::iterator iter;
	
	insertScene_ = 0;

	int sceneIndex = (int)scenes.size();
	for (int i = 0; i < (int)scenes.size(); ++i)
	{
		if (scenes[i].get() == insertScene)
		{
			sceneIndex = i;
			break;
		}
	}

	sceneIndex++;
	if (sceneIndex < (int)scenes.size())
		insertScene_ = scenes[sceneIndex].get();	

	BOOST_FOREACH(SceneSPtr scene, pasteScenes_)
	{
		edScenes_.push_back(document_->editorScene(scene.get()));
	}
}

void PasteScenesCmd::doCommand()
{	
	BOOST_FOREACH(SceneSPtr scene, pasteScenes_)
	{
		sceneDoc_->insertBefore(scene, insertScene_);		
	}	

	BOOST_FOREACH(EditorSceneSPtr edScene, edScenes_)
		document_->addEditorScene(edScene);

	emit sceneListChanged();
}
void PasteScenesCmd::undoCommand()
{
	BOOST_FOREACH(SceneSPtr scene, pasteScenes_)
	{
		sceneDoc_->deleteScene(scene.get());
	}

	BOOST_FOREACH(EditorSceneSPtr edScene, edScenes_)
		document_->deleteEditorScene(edScene.get());
	
	emit sceneListChanged();
}


////////////////////////////////////////////////////////////////////////////////
MoveScenesCmd::MoveScenesCmd(
	Document * document, const std::vector<Scene *> & objsToMove,
	Scene * moveTarget)
:
document_(document), moveTarget_(moveTarget)
{	
	foreach(Scene * obj, objsToMove)
		objsToMove_.push_back(document_->findScene(obj));

	objsBackup_ = document_->scenes();
}

void MoveScenesCmd::doCommand()
{	
	BOOST_FOREACH(SceneSPtr obj, objsToMove_)	
		document_->insertBefore(obj, moveTarget_);
		
	emit sceneListChanged();
}
void MoveScenesCmd::undoCommand()
{
	document_->setScenes(
		document_->documentTemplate(),
		objsBackup_);
	emit sceneListChanged();
}

////////////////////////////////////////////////////////////////////////////////
ChangeScenesPropertiesCmd::ChangeScenesPropertiesCmd(
	const std::vector<Scene *> & scenes, float width, float height, bool zoom, Color color)
:
scenes_(scenes), width_(width), height_(height), zoom_(zoom), color_(color)
{	
	BOOST_FOREACH(Scene * scene, scenes)
	{
		propertiesBackup_[scene] = 
			Properties(scene->screenWidth(), scene->screenHeight(), scene->zoom(), scene->bgColor());
	}
}

void ChangeScenesPropertiesCmd::doCommand()
{	
	BOOST_FOREACH(Scene * scene, scenes_)	
	{
		scene->setScreenWidth(width_);
		scene->setScreenHeight(height_);
		scene->setZoom(zoom_);
		scene->setBgColor(color_);
	}
		
	emit sceneChanged();
}
void ChangeScenesPropertiesCmd::undoCommand()
{
	BOOST_FOREACH(Scene * scene, scenes_)	
	{
		scene->setScreenWidth(propertiesBackup_[scene].width);
		scene->setScreenHeight(propertiesBackup_[scene].height);
		scene->setZoom(propertiesBackup_[scene].zoom);
		scene->setBgColor(propertiesBackup_[scene].color);
	}
	emit sceneChanged();
}
////////////////////////////////////////////////////////////////////////////////
ChangeArticlesCmd::ChangeArticlesCmd(
	Document * document,
	MagazineDocumentTemplate * articleManager, 
	const std::vector<MagazineArticleSPtr> & articles,
	const std::vector< std::string > & uiImgFiles,
	bool resetArticleFirstPage,
	bool bookmarks, 
	bool aspect,
	MagazineDocumentTemplate::HomeButtonFunction homeButtonFunction,
	bool doToContents,
	int toContentsIndex,
	MagazineDocumentTemplate::TransitionMode transitionMode
	)
:
object_(articleManager), 
articles_(articles), 
origArticles_(articleManager->articles()),
uiImgFiles_(uiImgFiles),
document_(document),
resetArticleFirstPage_(resetArticleFirstPage),
origResetArticleFirstPage_(articleManager->resetArticleFirstPage()),
bookmarks_(bookmarks),
origBookmarks_(articleManager->doBookmarks()),
aspect_(aspect),
origAspect_(articleManager->doAspect()),
homeButtonFunction_(homeButtonFunction),
origHomeButtonFunction_(articleManager->homeButtonFunction()),
origDoToContents_(articleManager->doToContentsPage()),
doToContents_(doToContents),
origToContentsIndex_(articleManager->contentsArticleIndex()),
toContentsIndex_(toContentsIndex),
origTransitionMode_(articleManager->transitionMode()),
transitionMode_(transitionMode)
{
	articleManager->uiImgFiles(&origUiImgFiles_);
}


void ChangeArticlesCmd::doCommand()
{	
	object_->setArticles(articles_);
	object_->setUiImgFiles(uiImgFiles_);
	object_->setResetArticleFirstPage(resetArticleFirstPage_);
	object_->setDoBookmarks(bookmarks_);
	object_->setDoAspect(aspect_);
	object_->setHomeButtonFunction(homeButtonFunction_);
	object_->setDoToContentsPage(doToContents_);
	object_->setContentsArticleIndex(toContentsIndex_);
	object_->setTransitionMode(transitionMode_);
	object_->init(document_->renderer());
	object_->start(0);	
	emit objectChanged();
}
void ChangeArticlesCmd::undoCommand()
{
	object_->setArticles(origArticles_);
	object_->setUiImgFiles(origUiImgFiles_);
	object_->setResetArticleFirstPage(origResetArticleFirstPage_);
	object_->setDoBookmarks(origBookmarks_);
	object_->setDoAspect(origAspect_);
	object_->setHomeButtonFunction(origHomeButtonFunction_);
	object_->setDoToContentsPage(origDoToContents_);
	object_->setContentsArticleIndex(origToContentsIndex_);
	object_->setTransitionMode(origTransitionMode_);
	object_->init(document_->renderer());
	object_->start(0);
	emit objectChanged();
}

////////////////////////////////////////////////////////////////////////////////
ChangeImageFileNameCmd::ChangeImageFileNameCmd(
	EditorDocument * document, 
	Image * image, const std::string & fileName)
:
object_(image), fileName_(fileName), origFileName_(image->fileName()),
origWidth_(image->width()), origHeight_(image->height()),
document_(document)
{}


void ChangeImageFileNameCmd::doCommand()
{		
	object_->setFileName(fileName_);	
	object_->init(document_->renderer());	

	int trWidth, trHeight;
	Global::instance().getImageDims(fileName_, &trWidth, &trHeight);

	object_->setWidth(trWidth);
	object_->setHeight(trHeight);
	emit objectChanged();
}
void ChangeImageFileNameCmd::undoCommand()
{
	object_->setFileName(origFileName_);	
	object_->init(document_->renderer());
	object_->setWidth(origWidth_);
	object_->setHeight(origHeight_);
	emit objectChanged();
}
////////////////////////////////////////////////////////////////////////////////
ChangeAppImageFileNameCmd::ChangeAppImageFileNameCmd(
	EditorDocument * doc,
	AppImage * image, const std::string & fileName)
:
object_(image), fileName_(fileName), origFileName_(image->fileName()),
origWidth_(image->width()), origHeight_(image->height())
{
	doc_ = doc;
}


void ChangeAppImageFileNameCmd::doCommand()
{	
	object_->setFileName(fileName_);	
	object_->init();
	
	int newWidth;
	int newHeight;

	QImage image(stdStringToQString(fileName_));	
	object_->setWidth(image.width());
	object_->setHeight(image.height());


	emit appObjectChanged();
	
}
void ChangeAppImageFileNameCmd::undoCommand()
{
	object_->setFileName(origFileName_);			
	object_->init();
	object_->setWidth(origWidth_);
	object_->setHeight(origHeight_);
	emit appObjectChanged();
	
}
////////////////////////////////////////////////////////////////////////////////
GizmoTransformCmd::GizmoTransformCmd(
	const std::vector<SceneObject *> & objects,
	const std::vector<Transform> & initTransforms, 
	const std::vector<Transform> & transforms,
	Transform * overallTransform,
	const Transform & initOverallTransform, 
	const Transform & finalOverallTransform)
: objects_(objects), initTransforms_(initTransforms), transforms_(transforms),
overallTransform_(overallTransform), finalOverallTransform_(finalOverallTransform),
initOverallTransform_(initOverallTransform)

{
}

void GizmoTransformCmd::doCommand()
{	
	for (int i = 0; i < (int)objects_.size(); ++i)
	{
		objects_[i]->setTransform(transforms_[i]);
	}

	if (overallTransform_)
		*overallTransform_ = finalOverallTransform_;
	emit objectChanged();
}

void GizmoTransformCmd::undoCommand()
{
	for (int i = 0; i < (int)objects_.size(); ++i)
	{
		objects_[i]->setTransform(initTransforms_[i]);
	}
	if (overallTransform_)
		*overallTransform_ = initOverallTransform_;
	emit objectChanged();
}

////////////////////////////////////////////////////////////////////////////////
ChangeSceneChangerScenesCmd::ChangeSceneChangerScenesCmd(
	EditorDocument * doc,
	SceneChanger * sceneChanger, 
	const std::vector<Scene *> & scenes,
	bool allowDragging)
:
document_(doc),
object_(sceneChanger), 
scenes_(scenes), 
origScenes_(sceneChanger->scenes()),
allowDragging_(allowDragging),
origAllowDragging_(sceneChanger->allowDrag())
{
}


void ChangeSceneChangerScenesCmd::doCommand()
{	
	object_->setScenes(scenes_);
	object_->setAllowDrag(allowDragging_);
	object_->init(document_->renderer());
	object_->start(0);
	emit objectChanged();
}
void ChangeSceneChangerScenesCmd::undoCommand()
{
	object_->setScenes(origScenes_);
	object_->setAllowDrag(origAllowDragging_);
	object_->init(document_->renderer());
	object_->start(0);
	emit objectChanged();
}

////////////////////////////////////////////////////////////////////////////////
ChangeDocumentPropertiesCmd::ChangeDocumentPropertiesCmd(EditorDocument * document, 
	Orientation orientation, int width, int height,
	const DocumentTemplateSPtr & docTemplate, const std::vector<std::string> & scriptFiles,
	const std::map<std::string, std::string> & scriptAccessibleFiles,
	const std::string & remoteReadServer, const bool & allowMultitouch, const bool & preDownload)
:
edDoc_(document),
document_(document->document()), 
orientation_(orientation), 
width_(width), height_(height),
scriptFiles_(scriptFiles),
scriptAccessibleFiles_(scriptAccessibleFiles),
remoteReadServer_(remoteReadServer),
multitouch_(allowMultitouch),
preDownload_(preDownload)
{
	origOrientation_ = document_->orientation();
	origWidth_ = document_->width();
	origHeight_ = document_->height();
	docTemplate_ = docTemplate;
	if (!docTemplate_) docTemplate_ = document_->documentTemplate();
	origDocTemplate_ = document_->documentTemplate();
	origScriptFiles_ = document_->scriptFiles();
	origScriptAccessibleFiles_ = document_->scriptAccessibleFiles();
	origRemoteReadServer_ = document_->remoteReadServer();
	origMultitouch_ = document_->allowMultitouch();
	origPreMultAlpha_ = document_->allowMultitouch();
	origPreDownload_ = document_->documentTemplate()->isPreDownload();
}


ChangeDocumentPropertiesCmd::~ChangeDocumentPropertiesCmd()
{
}

void ChangeDocumentPropertiesCmd::doCommand()
{		
	document_->setOrientation(orientation_);	
	document_->setSize(width_, height_, 1, 1, false);
	document_->setScriptFiles(scriptFiles_);
	document_->setScriptAccessibleFiles(scriptAccessibleFiles_);
	document_->setRemoteReadServer(remoteReadServer_);
	document_->setDocumentTemplate(docTemplate_);
	document_->setAllowMultitouch(multitouch_);
	document_->documentTemplate()->setPreDownload(preDownload_);
	/*
	edDoc_->renderer()->initShaders(false, preMultAlpha_);
	*/
}
void ChangeDocumentPropertiesCmd::undoCommand()
{
	document_->setOrientation(origOrientation_);
	document_->setSize(origWidth_, origHeight_, 1, 1, false);
	document_->setScriptFiles(origScriptFiles_);
	document_->setScriptAccessibleFiles(origScriptAccessibleFiles_);
	document_->setRemoteReadServer(origRemoteReadServer_);
	document_->setDocumentTemplate(origDocTemplate_);	
	document_->setAllowMultitouch(origMultitouch_);
	document_->documentTemplate()->setPreDownload(origPreDownload_);
	/*
	edDoc_->renderer()->initShaders(false, origPreMultAlpha_);
	*/
}


////////////////////////////////////////////////////////////////////////////////
ChangeSingleSceneDocumentCmd::ChangeSingleSceneDocumentCmd(
	Document * document,
	SingleSceneDocumentTemplate * docTemplate, Scene * scene)
{
	document_ = document;
	scene_ = scene;
	docTemplate_ = docTemplate;
	origScene_ = docTemplate_->scene();
}

void ChangeSingleSceneDocumentCmd::doCommand()
{		
	docTemplate_->setScene(scene_);
	docTemplate_->init(document_->renderer());
	docTemplate_->start(0);
}
void ChangeSingleSceneDocumentCmd::undoCommand()
{
	docTemplate_->setScene(origScene_);
	docTemplate_->init(document_->renderer());
	docTemplate_->start(0);
}

////////////////////////////////////////////////////////////////////////////////
MultiCommandCmd::MultiCommandCmd(
	const QString & name, const std::vector<Command *> & cmds)
{
	name_ = name;
	commands_ = cmds;
}

MultiCommandCmd::~MultiCommandCmd()
{
	BOOST_FOREACH(Command * cmd, commands_) delete cmd;	
}

QString MultiCommandCmd::name() const
{
	return name_;
}

void MultiCommandCmd::doCommand()
{		
	BOOST_FOREACH(Command * command, commands_)
	{
		command->doCommand();
	}
}
void MultiCommandCmd::undoCommand()
{
	BOOST_REVERSE_FOREACH(Command * command, commands_)
	{
		command->undoCommand();
	}
}


////////////////////////////////////////////////////////////////////////////////
ChangeSceneChangerDocumentCmd::ChangeSceneChangerDocumentCmd(
	Document * document,
	SceneChangerDocumentTemplate * sceneChanger, 
	const std::vector<Scene *> & scenes, bool allowDragging)
:
document_(document),
object_(sceneChanger), 
scenes_(scenes), 
origScenes_(sceneChanger->scenes()),
allowDragging_(allowDragging),
origAllowDragging_(sceneChanger->allowDrag())
{
}

void ChangeSceneChangerDocumentCmd::doCommand()
{	
	object_->setScenes(scenes_);
	object_->setAllowDrag(allowDragging_);
	object_->init(document_->renderer());
	object_->start(0);
}
void ChangeSceneChangerDocumentCmd::undoCommand()
{
	object_->setScenes(origScenes_);
	object_->setAllowDrag(origAllowDragging_);
	object_->init(document_->renderer());
	object_->start(0);
}

////////////////////////////////////////////////////////////////////////////////

ChangeCatalogDocumentCmd::ChangeCatalogDocumentCmd(
		Document * document,
		CatalogDocumentTemplate * catalog, 
		const std::vector< Scene * > & scenes,
		const std::vector< std::string > & uiImgFiles,
		const std::map<int, std::string> & thumbFileMap)
:
document_(document),
object_(catalog), 
scenes_(scenes), 
origScenes_(catalog->scenes()),
uiImgFiles_(uiImgFiles),
thumbFileMap_(thumbFileMap)
{
	object_->uiImgFiles(&origUiImgFiles_);
	object_->thumbFileMap(&origThumbFileMap_);
}
void ChangeCatalogDocumentCmd::doCommand()
{	
	object_->setScenes(scenes_);
	object_->setUiImgFiles(uiImgFiles_);
	object_->setThumbFileMap(thumbFileMap_);
	object_->init(document_->renderer());
	object_->start(0);
}
void ChangeCatalogDocumentCmd::undoCommand()
{
	object_->setScenes(origScenes_);
	object_->setUiImgFiles(origUiImgFiles_);
	object_->setThumbFileMap(origThumbFileMap_);
	object_->init(document_->renderer());
	object_->start(0);
}

////////////////////////////////////////////////////////////////////////////////

PasteActionsCmd::PasteActionsCmd(
	EditorDocument * document, EventListener * listener, 
	const std::vector<ActionSPtr> & pasteObjs)
:
document_(document), scene_(document->selectedScene()), pasteObjs_(pasteObjs),
listener_(listener)
{		
	const vector<Action *> & selActions = document_->selectedActions();
	refAction_ = 0;
	if (!selActions.empty())
		refAction_ = selActions.back();

	bool refActionReached = false;
	BOOST_FOREACH(const ActionSPtr & action, listener_->actions())
	{
		if (refActionReached)
		{
			refAction_ = action.get();
			break;
		}
		if (action.get() == refAction_) refActionReached = true;
	}


	if (pasteObjs_.empty())
	{
		throw Exception("No actions could be pasted. No actions had valid references within the current context.");
	}
}

void PasteActionsCmd::doCommand()
{	
	vector<Action *> selObjs;

	BOOST_FOREACH(ActionSPtr obj, pasteObjs_)
	{
		listener_->insertBefore(obj, refAction_);
		selObjs.push_back(obj.get());
	}	

	document_->setSelectedActions(selObjs);

	emit actionSelectionChanged();
	emit actionListChanged();
}
void PasteActionsCmd::undoCommand()
{
	BOOST_FOREACH(ActionSPtr obj, pasteObjs_)
	{
		listener_->deleteAction(obj.get());	
	}

	emit actionListChanged();
}

////////////////////////////////////////////////////////////////////////////////

PasteAnimChannelsCmd::PasteAnimChannelsCmd(
	EditorDocument * document,
	const std::vector<AnimationChannelSPtr> & pasteObjs)
:
document_(document), pasteObjs_(pasteObjs)
{		
	anim_ = document_->selectedAnimation();
	obj_ = document_->selectedObject();

	vector<AnimationChannelSPtr>::iterator iter;
	for (iter = pasteObjs_.begin(); iter != pasteObjs_.end();)
	{
		AnimationChannelSPtr a = *iter;
		if (!a->supportsObject(obj_))
		{
			iter = pasteObjs_.erase(iter);
		}		
		else
			++iter;
	}

	if (anim_->channels(obj_))
		channelsBackup_ = *anim_->channels(obj_);

	if (pasteObjs_.empty())
	{
		throw Exception("No copied channel supports this object type");
	}
}

void PasteAnimChannelsCmd::doCommand()
{	
	vector<AnimationChannel *> selObjs;

	BOOST_FOREACH(AnimationChannelSPtr p, pasteObjs_)
	{
		BOOST_FOREACH(AnimationChannelSPtr a, channelsBackup_)
		{
			if (p->type() == a->type())
			{
				anim_->deleteChannel(a.get());
			}
		}

		p->setObject(obj_);
		anim_->addChannel(p);
		selObjs.push_back(p.get());
	}
	
	document_->setSelectedChannels(selObjs);
	emit channelSelectionChanged();
	emit channelListChanged();
}
void PasteAnimChannelsCmd::undoCommand()
{
	anim_->setChannels(obj_, channelsBackup_);
	emit channelListChanged();
}

///////////////////////////////////////////////////////////////////////////////

ChangeSceneCameraCmd::ChangeSceneCameraCmd(
	GLWidget * view, Scene * scene, CameraObject * camObject) 
{
	scene_ = scene;
	camObject_ = camObject;
	origCamObject_ = scene_->userCamera();	
	view_ = view;
	origCamState_ = *view_->camera();	
}

void ChangeSceneCameraCmd::doCommand()
{
	scene_->setUserCamera(camObject_);	
	if (!camObject_)
	{
		view_->resetCamera();
	}
	emit sceneChanged();
}

void ChangeSceneCameraCmd::undoCommand()
{
	scene_->setUserCamera(origCamObject_);	
	view_->setCamera(origCamState_);
	emit sceneChanged();
}

///////////////////////////////////////////////////////////////////////////////

ChangeCameraTransformCmd::ChangeCameraTransformCmd(
	CameraObject * camObject, 
	const Camera & oldCam, const Camera & newCam)
{
	origCamera_ = oldCam;
	camera_ = newCam;
	camObject_ = camObject;
}

void ChangeCameraTransformCmd::doCommand()
{
	camObject_->setCamera(camera_);

	/*
	scene camera is a scene property so emit this
	this will alert GlWidget to its resync camera to the newly changed scene 
	camera
	*/
	emit sceneChanged();
}

void ChangeCameraTransformCmd::undoCommand()
{
	camObject_->setCamera(origCamera_);
	emit sceneChanged();
}

///////////////////////////////////////////////////////////////////////////////
ChangeTextTableStateCmd::ChangeTextTableStateCmd(
	TextTable * textTableObj, TextTableStateSPtr oldState, TextTableStateSPtr newState)
{
	textTableObj_ = textTableObj;
	origTableState_ = oldState;
	newTableState_ = newState;
}

void ChangeTextTableStateCmd::doCommand()
{
	textTableObj_->setState(newTableState_.get());
}

void ChangeTextTableStateCmd::undoCommand()
{
	textTableObj_->setState(origTableState_.get());
}

///////////////////////////////////////////////////////////////////////////////
ChangeTextGridSizesCmd::ChangeTextGridSizesCmd(
	TextTable * textTableObj, TextTableGridSizesSPtr oldGridSizes, TextTableGridSizesSPtr newGridSizes)
{
	textTableObj_ = textTableObj;
	oldGridSizes_ = oldGridSizes;
	newGridSizes_ = newGridSizes;
}

void ChangeTextGridSizesCmd::doCommand()
{
	textTableObj_->setGridSizes(newGridSizes_.get());
}

void ChangeTextGridSizesCmd::undoCommand()
{
	textTableObj_->setGridSizes(oldGridSizes_.get());
}

///////////////////////////////////////////////////////////////////////////////

SetTrackingUrlsCmd::SetTrackingUrlsCmd(const std::vector<Scene *> & scenes, const std::string & url)
{
	scenes_ = scenes;
	url_ = url;
	BOOST_FOREACH(Scene * scene, scenes_)
		origUrls_.insert(make_pair(scene, scene->trackingUrl()));	
}

void SetTrackingUrlsCmd::doCommand()
{
	for (int i = 0; i < (int)scenes_.size(); ++i)
	{
		std::string urlRep = url_;
		regexReplace(&urlRep, "\\$\\(SCENE_NAME\\)", UriEncode(scenes_[i]->name()).c_str());
		stringstream ss;
		ss << "i";		
		regexReplace(&urlRep, "\\$\\(SCENE_INDEX\\)", ss.str().c_str());
		regexReplace(&urlRep, "^(.*://|)(.*)$", "http://$2");

		scenes_[i]->setTrackingUrl(urlRep);
	}

	emit sceneChanged();
}

void SetTrackingUrlsCmd::undoCommand()
{
	for (int i = 0; i < (int)scenes_.size(); ++i)
	{
		scenes_[i]->setTrackingUrl(origUrls_[scenes_[i]]);
	}

	emit sceneChanged();
}

///////////////////////////////////////////////////////////////////////////////
ChangeMapFileNameCmd::ChangeMapFileNameCmd(
	EditorDocument * document, 
	Map * object, const std::string & fileName, const std::string & pathFileName)
	:
	document_(document),
	object_(object), 
	fileName_(fileName), origFileName_(object->fileName()),	
	pathFileName_(pathFileName), origPathFileName_(object->pathMeshFileName())
{}


void ChangeMapFileNameCmd::doCommand()
{	
	object_->setFileName(fileName_);	
	object_->setPathMeshFileName(pathFileName_);
	object_->init(document_->renderer());
	emit objectChanged();
}
void ChangeMapFileNameCmd::undoCommand()
{
	object_->setFileName(origFileName_);	
	object_->setPathMeshFileName(origPathFileName_);
	object_->init(document_->renderer());
	emit objectChanged();
}

///////////////////////////////////////////////////////////////////////////////

ChangeShowMaskingCmd::ChangeShowMaskingCmd(EditorDocument * edDoc, EditorMaskObject * edMaskObj, bool val)
	:
	edDoc_(edDoc), edMaskObj_(edMaskObj), newVal_(val)
{	
	oldVal_ = edMaskObj->showMasking();
}

void ChangeShowMaskingCmd::doCommand()
{	
	edMaskObj_->setShowMasking(newVal_);
	emit objectChanged();
}
void ChangeShowMaskingCmd::undoCommand()
{
	edMaskObj_->setShowMasking(oldVal_);
	emit objectChanged();
}

////////////////////////////////////////////////////////////////////////////////
ChangePhotoImageFileNameCmd::ChangePhotoImageFileNameCmd(
	EditorDocument * document, 
	PhotoObject * photoObject, const std::string & fileName)
:
object_(photoObject), fileName_(fileName), origFileName_(photoObject->getPhotoName()),
origWidth_(photoObject->width()), origHeight_(photoObject->height()),
document_(document)
{}


void ChangePhotoImageFileNameCmd::doCommand()
{		
	object_->setFileName(fileName_);	
	object_->init(document_->renderer());	

	object_->setWidth(object_->width());
	object_->setHeight(object_->height());
	emit objectChanged();
}
void ChangePhotoImageFileNameCmd::undoCommand()
{
	object_->setFileName(origFileName_);	
	object_->init(document_->renderer());
	object_->setWidth(origWidth_);
	object_->setHeight(origHeight_);
	emit objectChanged();
}

////////////////////////////////////////////////////////////////////////////////
MergeTextsCmd::MergeTextsCmd(EditorDocument * document, Scene * scene, std::vector<Text_2*> texts)
	:origTexts_(texts), scene_(scene), document_(document)
{
	vector<AnimationSPtr> animations = scene->animations();
	vector<SceneObject *> animObjs;
	BOOST_FOREACH(SceneObject * obj, origTexts_)
	{
		BOOST_FOREACH(AnimationSPtr animation, animations)
		{
			animation->objects(&animObjs);

			BOOST_FOREACH(SceneObject * animObj, animObjs)
			{
				if (isDescendant(obj, animObj) || obj == animObj)
				{
					AnimBackupKey d(animation.get(), animObj);
					channelsBackup_[d] = *animation->channels(animObj);
				}
			}
			
		}
	}

	vector<Text_2 *>::iterator iter = origTexts_.begin();
	while (iter != origTexts_.end())
	{		
		SceneObject * curObject = *iter;
		bool removeCur = false;
		BOOST_FOREACH(SceneObject * obj, origTexts_)		
			if (isDescendant(obj, curObject)) removeCur = true;		

		if (removeCur) iter = origTexts_.erase(iter);		
		else ++iter;		
	}

	BOOST_FOREACH(SceneObject * obj, origTexts_)
	{
		childrenBackup_[obj->parent()] = obj->parent()->children();
		editorObjectsBackup_.push_back(document_->editorObject(obj));

		syncedAnimationsBackup_[obj] = scene_->syncedAnimations(obj);		
	}

	vector<EventListenerSPtr> allListeners;
	scene->allListeners(&allListeners);
	
	BOOST_FOREACH(SceneObject * obj, origTexts_)
	{
		BOOST_FOREACH(EventListenerSPtr listener, allListeners)
		{
			if (listener->dependsOn(obj)) 
			{
				actionsBackup_[listener.get()] = listener->actions();
				
				BOOST_FOREACH(ActionSPtr action, listener->actions())
				{
					if (action->dependsOn(obj))
						editorActionsBackup_[action.get()] = 
						document_->editorAction(scene_, action.get());
				}
			}
		}
	}

	for(int i = 0; i < origTexts_[0]->getLinesStyle().size(); i++)
		tempLinStyle.push_back(origTexts_[0]->getLinesStyle()[i]);
	for(int i = 0; i < origTexts_[0]->properties().size(); i++)
		tempPropertis.push_back(origTexts_[0]->properties()[i]);

	tempStr = origTexts_[0]->textString();
	
}
bool MergeTextsCmd::isDescendant(SceneObject * obj1, SceneObject * obj2) const
{
	SceneObject * ancestor = obj2;
	while (ancestor)
	{
		ancestor = ancestor->parent();
		if (ancestor == obj1) return true;
	}
	return false;
}

void MergeTextsCmd::doCommand()
{
	mergeText();

	emit animationChanged();
	emit actionListChanged();
	emit channelListChanged();	
	emit objectListChanged();
	emit objectChanged();
}
void MergeTextsCmd::undoCommand()
{
	map<SceneObject *, vector<SceneObjectSPtr> >::iterator iter;
	for (iter = childrenBackup_.begin(); iter != childrenBackup_.end(); ++iter)
	{
		SceneObject * parent = (*iter).first;
		vector<SceneObjectSPtr> & children = (*iter).second;

		parent->setChildren(children);
	}

	BOOST_FOREACH(EditorObjectSPtr edObj, editorObjectsBackup_)
	{
		if (edObj) document_->addEditorObject(edObj);
	}

	map<AnimBackupKey, vector<AnimationChannelSPtr> >::iterator chanIter;
	for (chanIter = channelsBackup_.begin(); chanIter != channelsBackup_.end();
		++chanIter)
	{
		Animation * anim = (*chanIter).first.animation;
		SceneObject * obj = (*chanIter).first.object;
		vector<AnimationChannelSPtr> & channels = (*chanIter).second;
		anim->setChannels(obj, channels);
	}

	//restore actions
	ActionsBackup::iterator iterA = actionsBackup_.begin();
	for (;iterA != actionsBackup_.end(); ++iterA)
	{
		EventListener * listener = (*iterA).first;
		listener->setActions((*iterA).second);
	}

	map<Action *, EditorActionSPtr>::iterator iterE = editorActionsBackup_.begin();
	for (;iterE != editorActionsBackup_.end(); ++iterE)
	{
		document_->addEditorAction((*iterE).second);
	}

	//restore synced animations

	map<SceneObject *, vector<Animation *> >::iterator itr;
	for (itr = syncedAnimationsBackup_.begin(); itr != syncedAnimationsBackup_.end(); ++itr)
	{
		SceneObject * syncObj = (*itr).first;
		BOOST_FOREACH(Animation * anim, (*itr).second)
		{
			scene_->setSyncObject(anim, syncObj);
		}
	}

	origTexts_[0]->setProperties(tempPropertis);
	origTexts_[0]->setLinesStyle(tempLinStyle);
	origTexts_[0]->setTextString(tempStr);
	origTexts_[0]->init(document_->document()->renderer());

	emit animationChanged();
	emit actionListChanged();
	emit channelListChanged();
	emit objectListChanged();
	emit objectChanged();
}

void MergeTextsCmd::mergeText()
{
	origTexts_[0]->mergeProperties();

	std::vector<SceneObject *> delobjs;

	for(int i = 1; i < origTexts_.size(); i++)
	{
		Text_2* mergedText = origTexts_[i];

		if(i>0)
		{
			mergedText->mergeProperties();

			std::vector<TextProperties_2> origprops = origTexts_[0]->properties();
			std::vector<TextProperties_2> mergedprops = mergedText->properties();
			bool prevObjectBind = false;
			for(int j = 0 ; j < mergedprops.size() ; j++)
			{
				TextProperties_2 prop = mergedprops[j];
				TextProperties_2 refprop = origprops.back();
				if (prevObjectBind || refprop.color != prop.color ||
					refprop.font.bold != prop.font.bold ||
					refprop.font.italic != prop.font.italic ||
					refprop.font.pointSize != prop.font.pointSize ||
					refprop.font.fontFile != prop.font.fontFile ||
					refprop.font.shadow != prop.font.shadow ||
					refprop.underline_ != prop.underline_ ||
					refprop.cancleline_ != prop.underline_ ||
					prop.isObjectProperty())
				{
					prop.index += (origTexts_[0]->textString().length() + 1);
					origprops.push_back(prop);
				
					if(prop.isObjectProperty())
						prevObjectBind = true;
					else
						prevObjectBind = false;
				}
			}
			origTexts_[0]->addLineStyle(mergedText->getLinesStyle(), origTexts_[0]->getLinesStyle().size() - 1);
			origTexts_[0]->setTextString(origTexts_[0]->textString() + L"\n" + mergedText->textString());
			origTexts_[0]->setProperties(origprops);
			origTexts_[0]->mergeProperties();
		
		}
		delobjs.push_back(mergedText);
	}
	deleteText(delobjs);

	origTexts_[0]->init(document_->document()->renderer());
}
void MergeTextsCmd::deleteText(std::vector<SceneObject *> delobjs)
{
	BOOST_FOREACH(SceneObject * obj, delobjs)
	{
		ActionsBackup::iterator iter = actionsBackup_.begin();
		for (;iter != actionsBackup_.end(); ++iter)
		{
			EventListener * listener = (*iter).first;
			BOOST_FOREACH(const ActionSPtr & action, (*iter).second)
			{
				if (action->dependsOn(obj))
				{					
					listener->deleteAction(action.get());
					document_->deleteEditorAction(
						editorActionsBackup_[action.get()].get());
				}
			}
		}
		obj->parent()->deleteChild(obj);	
		document_->setObjectSelected(obj, false);
		
		scene_->removeSyncObject(obj);
	}
	BOOST_FOREACH(EditorObjectSPtr edObj, editorObjectsBackup_)
	{
		if (edObj) document_->deleteEditorObject(edObj.get());
	}
	map<AnimBackupKey, vector<AnimationChannelSPtr> >::iterator chanIter;
	for (chanIter = channelsBackup_.begin(); chanIter != channelsBackup_.end();
		++chanIter)
	{
		Animation * anim = (*chanIter).first.animation;
		SceneObject * obj = (*chanIter).first.object;
		anim->setChannels(obj, vector<AnimationChannelSPtr>());
	}
}