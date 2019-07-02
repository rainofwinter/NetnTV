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

class Document;
typedef boost::shared_ptr<Document> DocumentSPtr;

class DocumentTemplate;
typedef boost::shared_ptr<DocumentTemplate> DocumentTemplateSPtr;

class Image;
typedef boost::shared_ptr<Image> ImageSPtr;

class Texture;
typedef boost::shared_ptr<Texture> TextureSPtr;

typedef std::vector<std::pair<SceneObject *, Scene *> > ContainerObjects;

class TextureRenderer;
typedef boost::shared_ptr<TextureRenderer> TextureRendererSPtr;

typedef struct _xmlNode xmlNode;
typedef struct _xmlDoc xmlDoc;
typedef xmlDoc *xmlDocPtr;