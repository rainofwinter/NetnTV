#ifndef ElementMapping_h__
#define ElementMapping_h__

class Document;
class Scene;
class SceneObject;
class Animation;
class AppObject;

class ElementMapping
{
	friend class XmlReader;
public:
	ElementMapping();
	~ElementMapping();

	void setDestinationData(Document * document, Scene * scene);

	Document * destinationDocument() const {return document_;}
	Scene * destinationScene() const {return scene_;}

	SceneObject * mapObject(SceneObject * obj) const;
	Scene * mapScene(Scene * scene) const;
	Animation * mapAnimation(Animation * animation) const;
	AppObject * mapAppObject(AppObject * appObject) const;

	void addAnimationMapping(Animation * key, Animation * value);
	void addSceneMapping(Scene * key, Scene * value);
	void addObjectMapping(SceneObject * key, SceneObject * value);
	void addAppObjectMapping(AppObject * key, AppObject * value);
private:
	Document * document_;
	Scene * scene_;
	
	std::map<SceneObject *, SceneObject *> objMap_;
	std::map<Scene *, Scene *> sceneMap_;
	std::map<Animation *, Animation *> animationMap_;

	std::map<AppObject *, AppObject *> appObjMap_;
};


#endif // ElementMapping_h__