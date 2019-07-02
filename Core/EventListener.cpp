#include "stdafx.h"
#include "EventListener.h"
#include "Event.h"
#include "Action.h"
#include "Exception.h"
#include "Writer.h"
#include "Reader.h"

#include "Xml.h"

#include "SceneObject.h"
#include "AppObject.h"

#include "SceneStartEvent.h"
#include "ScenePreStartEvent.h"


#include "ElementMapping.h"

using namespace std;

EventListener::EventListener()
{
	create();
}

EventListener::EventListener(const EventSPtr & event)
{
	create();
	event_ = event;
}

void EventListener::create()
{
	curActionStarted_ = false;
	started_ = false;
	updating_ = false;
	isWorking_ = false;
}

EventListener::EventListener(const EventListener & rhs)
{
	*this = rhs;
}

EventListener & EventListener::operator = (const EventListener & rhs)
{
	if (this == &rhs) return *this;
	event_ = EventSPtr(rhs.event_->clone());


	actions_.clear();
	actions_.reserve(rhs.actions_.size());
	BOOST_FOREACH(const ActionSPtr & action, rhs.actions_)
	{
		actions_.push_back(ActionSPtr(action->clone()));
	}

	startedSceneTime_ = rhs.startedSceneTime_;
	time_ = rhs.time_;
	curActionIndex_ = rhs.curActionIndex_;
	started_ = rhs.started_;

	return *this;
}

void EventListener::remapReferences(const ElementMapping & mapping)
{
	vector<ActionSPtr>::iterator iter = actions_.begin();
	for (; iter != actions_.end(); )
	{
		ActionSPtr & action = *iter;
		if (!action->remapReferences(mapping))
		{
			iter = actions_.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void EventListener::referencedFiles(std::vector<std::string> * refFiles)
{
	BOOST_FOREACH(ActionSPtr action, actions_)
		action->referencedFiles(refFiles);
}

int EventListener::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	BOOST_FOREACH(ActionSPtr action, actions_)
		index = action->setReferencedFiles(baseDirectory, refFiles, index);

	return index;
}


EventListener::~EventListener()
{
}

bool EventListener::handle(SceneObject * object, const Event * event, float sceneTime)
{
	static boost::uuids::uuid scenePreStartEventType = ScenePreStartEvent().type();
	static boost::uuids::uuid sceneStartEventType = SceneStartEvent().type();

	if (event_->equals(*event)) 
	{
		start(object, sceneTime);
		return true;
	}
	else if (event_->type() == sceneStartEventType && event->type() == scenePreStartEventType)
	{
		scenePreStart(object, sceneTime);
		return true;
	}
	return false;
}

bool EventListener::handleAppObj(AppObject * object, const Event * event, float sceneTime)
{
	static boost::uuids::uuid scenePreStartEventType = ScenePreStartEvent().type();
	static boost::uuids::uuid sceneStartEventType = SceneStartEvent().type();

	if (event_->equals(*event)) 
	{
		startAppObj(object, sceneTime);
		return true;
	}
	else if (event_->type() == sceneStartEventType && event->type() == scenePreStartEventType)
	{
		scenePreStartAppObj(object, sceneTime);
		return true;
	}
	return false;
}


bool EventListener::start(SceneObject * object, float sceneTime)
{
	if (started_ && curActionStarted_)
	{
		if (curActionIndex_ >= 0 && curActionIndex_ < actions_.size())
			actions_[curActionIndex_]->stop();
	}
	startedSceneTime_ = sceneTime;
	curActionIndex_ = 0;
	curActionStarted_ = false;
	started_ = true;
	isWorking_ = true;
	return update(object, sceneTime);
}

bool EventListener::startAppObj(AppObject * object, float sceneTime)
{
	if (started_ && curActionStarted_)
	{
		if (curActionIndex_ >= 0 && curActionIndex_ < actions_.size())
			actions_[curActionIndex_]->stop();
	}
	startedSceneTime_ = sceneTime;
	curActionIndex_ = 0;
	curActionStarted_ = false;
	started_ = true;
	return updateAppObj(object, sceneTime);
}

/**
This function is based on the start and update functions
*/
void EventListener::scenePreStart(SceneObject * object, float sceneTime)
{
	if (started_ && curActionStarted_)
	{
		if (curActionIndex_ >= 0 && curActionIndex_ < actions_.size())
			actions_[curActionIndex_]->stop();
	}
	startedSceneTime_ = sceneTime;
	curActionIndex_ = 0;
	
	time_ = sceneTime - startedSceneTime_;	
	curActionStarted_ = true;
	while (curActionIndex_ < (int)actions_.size())
	{	
		actions_[curActionIndex_]->scenePreStart(object, time_);
		if (actions_[curActionIndex_]->hasDuration()) break;
		++curActionIndex_;
	}

	curActionIndex_ = 0;
	curActionStarted_ = false;
	started_ = false;
}
void EventListener::scenePreStartAppObj(AppObject * object, float sceneTime)
{
	if (started_ && curActionStarted_)
	{
		if (curActionIndex_ >= 0 && curActionIndex_ < actions_.size())
			actions_[curActionIndex_]->stop();
	}
	startedSceneTime_ = sceneTime;
	curActionIndex_ = 0;
	
	time_ = sceneTime - startedSceneTime_;	
	curActionStarted_ = true;
	while (curActionIndex_ < (int)actions_.size())
	{	
		actions_[curActionIndex_]->scenePreStartAppObj(object, time_);
		if (actions_[curActionIndex_]->hasDuration()) break;
		++curActionIndex_;
	}

	curActionIndex_ = 0;
	curActionStarted_ = false;
	started_ = false;
}


void EventListener::stop()
{
	if (started_ && curActionStarted_)
	{
		if (curActionIndex_ >= 0 && curActionIndex_ < actions_.size())
			actions_[curActionIndex_]->stop();
	}
	started_ = false;
	curActionStarted_ = false;
}


bool EventListener::update(SceneObject * object, float sceneTime)
{
	bool needToRedraw = false;
	if (!started_ || actions_.empty()) return needToRedraw;

	
	//prevent update from running within itself (see below)
	if (updating_) return needToRedraw;
	updating_ = true;

	time_ = sceneTime - startedSceneTime_;

	if (!curActionStarted_)
	{
		curActionStarted_ = true;
		while (curActionIndex_ < (int)actions_.size())
		{	
			/*
			In some cases, the following start call can lead to this update function 
			being called again. So we must prevent the update function executing
			from within itself because that will mess things up
			*/
			needToRedraw |= actions_[curActionIndex_]->start(object, time_);
			if (actions_[curActionIndex_]->hasDuration()) break;
			++curActionIndex_;
		}
	}

	if (curActionIndex_ < (int)actions_.size())
	{
		if (actions_[curActionIndex_]->isExecuting())	
			needToRedraw |= actions_[curActionIndex_]->update(object, time_);
		else
		{			
			++curActionIndex_;
			curActionStarted_ = false;
			needToRedraw |= update(object, sceneTime);
		}		
	}

	if (curActionIndex_ >= (int)actions_.size()) {
		isWorking_ = false;
		started_ = false;
	}


	updating_ = false;
	return needToRedraw;	
}

bool EventListener::updateAppObj(AppObject * object, float sceneTime)
{
	bool needToRedraw = false;
	if (!started_ || actions_.empty()) return needToRedraw;

	
	//prevent update from running within itself (see below)
	if (updating_) return needToRedraw;
	updating_ = true;

	time_ = sceneTime - startedSceneTime_;

	if (!curActionStarted_)
	{
		curActionStarted_ = true;
		while (curActionIndex_ < (int)actions_.size())
		{	
			/*
			In some cases, the following start call can lead to this update function 
			being called again. So we must prevent the update function executing
			from within itself because that will mess things up
			*/
			needToRedraw |= actions_[curActionIndex_]->startAppObj(object, time_);
			if (actions_[curActionIndex_]->hasDuration()) break;
			++curActionIndex_;
		}
	}

	if (curActionIndex_ < (int)actions_.size())
	{
		if (actions_[curActionIndex_]->isExecuting())	
			needToRedraw |= actions_[curActionIndex_]->updateAppObj(object, time_);
		else
		{			
			++curActionIndex_;
			curActionStarted_ = false;
			needToRedraw |= updateAppObj(object, sceneTime);
		}		
	}

	if (curActionIndex_ >= (int)actions_.size())
		started_ = false;


	updating_ = false;
	return needToRedraw;	
}


void EventListener::addAction(const ActionSPtr & action)
{
	actions_.push_back(action);
}

ActionSPtr EventListener::findAction(Action * action) const
{
	vector<ActionSPtr>::const_iterator iter = actions_.begin();
	for (; iter != actions_.end(); ++iter)
	{
		const ActionSPtr & curAction = *iter;
		if (curAction.get() == action) return curAction;
	}

	return ActionSPtr();
}

ActionSPtr EventListener::deleteAction(Action * action)
{
	vector<ActionSPtr>::iterator iter = actions_.begin();
	for (; iter != actions_.end(); ++iter)
	{
		ActionSPtr & curAction = *iter;
		if (curAction.get() == action)
		{
			actions_.erase(iter);
			return curAction;
		}
	}

	return ActionSPtr();
}

void EventListener::changeAction(Action * action, const ActionSPtr & newAction)
{
	vector<ActionSPtr>::iterator iter = actions_.begin();
	for (; iter != actions_.end(); ++iter)
	{
		ActionSPtr & curAction = *iter;
		if (curAction.get() == action)
		{
			iter = actions_.erase(iter);
			actions_.insert(iter, newAction);
			break;
			
		}
	}

}


void EventListener::insertBefore(
	const ActionSPtr & newObj, Action * refObj)
{
	vector<ActionSPtr>::iterator insertIter, removeIter, iter;
	if (newObj.get() == refObj) return;

	removeIter = actions_.end();
	insertIter = actions_.end();	
	for (iter = actions_.begin(); iter != actions_.end(); ++iter)	
	{
		if ((*iter) == newObj) removeIter = iter;	
		if ((*iter).get() == refObj) insertIter = iter;
	}

	if (insertIter == actions_.end() && refObj) throw Exception(InvalidArguments);
	if (removeIter != actions_.end()) actions_.erase(removeIter);

	insertIter = actions_.end();	
	for (iter = actions_.begin(); iter != actions_.end(); ++iter)	
		if ((*iter).get() == refObj) insertIter = iter;	

	actions_.insert(insertIter, newObj);
}


void EventListener::write(Writer & writer) const
{	
	writer.write(event_, "event");

	vector<ActionSPtr> actions = actions_;

	//make sure not to write actions that have invalid target objects.
	//This is to prevent memory leaks when reading.
	//Also, ensure that actions are properly deleted when their target object
	//is deleted.

	vector<ActionSPtr>::iterator iter;
	for (iter = actions.begin(); iter != actions.end();)
	{
		ActionSPtr a = *iter;
		if (a->targetObject() && !a->targetObject()->parentScene())
		{
			iter = actions.erase(iter);
		}
		else
			++iter;
	}

	unsigned int numObjs = (unsigned int)actions.size();
	writer.write(numObjs, "count");
	std::stringstream ss;
	for (unsigned int i = 0; i < numObjs; ++i)
	{
		ss.str("");	
		ss << "action" << "[" << i << "]";
		writer.write(actions[i], ss.str().c_str());
	}
	
}

void EventListener::writeXml(XmlWriter & w) const
{
	w.startTag("Event");
	w.writeEvent(event_.get());
	w.endTag();

	w.startTag("Actions");
	BOOST_FOREACH(ActionSPtr action, actions_)
	{
		w.writeAction(action.get());
	}
	w.endTag();
}

void EventListener::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.isNodeName(curNode, "Event"))
		{
			for(xmlNode * eNode = curNode->children; eNode; eNode = eNode->next)
			{		
				if (eNode->type != XML_ELEMENT_NODE) continue;
				if (r.getEvent(event_, eNode)) break;
			}			
		}
		else if (r.isNodeName(curNode, "Actions"))
		{
			for(xmlNode * aNode = curNode->children; aNode; aNode = aNode->next)
			{		
				if (aNode->type != XML_ELEMENT_NODE) continue;
				ActionSPtr a;
				if (r.getAction(a, aNode))
				{
					actions_.push_back(a);
				}
			}
		}
	}
}

void EventListener::read(Reader & reader, unsigned char version)
{
	if (version == 0)
	{
		std::string name;
		reader.read(name);
	}
	
	reader.read(event_);
	reader.read(actions_);
}

bool EventListener::dependsOn(Animation * animation) const
{
	BOOST_FOREACH(ActionSPtr action, actions_)
	{
		if (action->dependsOn(animation)) return true;
	}

	return false;
}

bool EventListener::dependsOn(SceneObject * object) const
{
	BOOST_FOREACH(ActionSPtr action, actions_)
	{
		if (action->dependsOn(object)) return true;
	}

	return false;
}

bool EventListener::dependsOn(Scene * scene) const
{
	BOOST_FOREACH(ActionSPtr action, actions_)
	{
		if (action->dependsOn(scene)) return true;
	}

	return false;
}