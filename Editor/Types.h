#pragma once

class Scene;
typedef boost::shared_ptr<Scene> SceneSPtr;

class SceneObject;
typedef boost::shared_ptr<SceneObject> SceneObjectSPtr;

class AppObject;
typedef boost::shared_ptr<AppObject> AppObjectSPtr;

class Animation;
typedef boost::shared_ptr<Animation> AnimationSPtr;

class AnimationChannel;
typedef boost::shared_ptr<AnimationChannel> AnimationChannelSPtr;

class EventListener;
typedef boost::shared_ptr<EventListener> EventListenerSPtr;

class Event;
typedef boost::shared_ptr<Event> EventSPtr;

class Action;
typedef boost::shared_ptr<Action> ActionSPtr;

class Actions;
typedef boost::shared_ptr<Actions> ActionsSPtr;

class Camera;
typedef boost::shared_ptr<Camera> CameraSPtr;

