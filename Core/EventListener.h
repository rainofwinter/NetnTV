#pragma once
#include "Types.h"

class Actions;
class Event;
class SceneObject;
class AppObject;
class Writer;
class XmlReader;
class XmlWriter;
class Reader;
class ElementMapping;

class EventListener
{
public:
	EventListener();
	EventListener(const EventSPtr & event);
	EventListener(const EventListener & rhs);
	EventListener & operator = (const EventListener & rhs);
	
	void remapReferences(const ElementMapping & mapping);

	void referencedFiles(std::vector<std::string> * refFiles);

	int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex);

	~EventListener();
	bool handle(SceneObject * object, const Event * event, float sceneTime);
	bool handleAppObj(AppObject * object, const Event * event, float sceneTime);

	Event * event() const {return event_.get();}
	void setEvent(const EventSPtr & event) 
	{event_ = event;}

	bool start(SceneObject * object, float sceneTime);
	void scenePreStart(SceneObject * object, float sceneTime);
	bool startAppObj(AppObject * object, float sceneTime);
	void scenePreStartAppObj(AppObject * object, float sceneTime);
	void stop();
	bool update(SceneObject * object, float sceneTime);
	bool updateAppObj(AppObject * object, float sceneTime);

	void addAction(const ActionSPtr & action);
	ActionSPtr findAction(Action * action) const;
	ActionSPtr deleteAction(Action * action);
	void changeAction(Action * action, const ActionSPtr & newAction);
	
	const std::vector<ActionSPtr> & actions() const {return actions_;}
	
	void setActions(const std::vector<ActionSPtr> & actions) 
	{actions_ = actions;}

	void insertBefore(const ActionSPtr & newObj, Action * refObj);

	bool dependsOn(Animation * animation) const;	
	bool dependsOn(SceneObject * obj) const;
	bool dependsOn(Scene * scene) const;

	unsigned char version() const {return 1;}
	void write(Writer & writer) const;
	void read(Reader & reader, unsigned char);

	void writeXml(XmlWriter & w) const;
	void readXml(XmlReader & r, xmlNode * node);

	bool isWorking(){return isWorking_;}

private:
	

	void create();
	///the event to listen for
	EventSPtr event_;


	///list of actions to execute upon receiving the event	
	std::vector<ActionSPtr> actions_;

	float startedSceneTime_;

	///Local actions time
	float time_;
	int curActionIndex_;
	bool curActionStarted_;
	bool started_;

	/**
	Flag whether an update is in the middle of being carried out.

	This is necessary for very subtle reasons:
	In the update call, it is possible that the same update call will be called 
	again. We want to prevent this sort of thing because it messes up state out
	from under us (from the point of view of the original update call).

	An example of where this happened is: 
	a click event is fired
	one of the actions is a SceneChange action.
	SceneChange changes to the next scene over.
	start(...) is called for the original scene.
	This involves an initial update(...) call for that scene.
	Which leads to the currently running click event listener's update function
	to be called again.
	... crash (due to the curActionIndex_ being incremented out of bounds)
	*/
	bool updating_;

	bool isWorking_;
};