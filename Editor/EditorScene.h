#pragma once

#include "EditorTypes.h"

class Scene;
class SceneObject;
class AppObject;
class Action;
class PropertyPage;
class GLWidget;

class EditorDocument;

class EditorScene : public QObject
{
	Q_OBJECT
public:
	EditorScene();
	EditorScene(Scene * scene);
	~EditorScene();

	void init();

	void setScene(Scene * scene) {scene_ = scene;}

	void Draw(GfxRenderer * gl) const;

	Scene * scene() const {return scene_;}

	EditorObjectSPtr editorObject(SceneObject * obj);
	EditorAppObjectSPtr editorAppObject(AppObject * obj);
	EditorActionSPtr editorAction(Action * action);

	void addEditorObject(const EditorObjectSPtr & edObj);
	void addEditorAppObject(const EditorAppObjectSPtr & edObj);
	void deleteEditorObject(EditorObject * edObj);
	void deleteEditorAppObject(EditorAppObject * edObj);

	void addEditorAction(const EditorActionSPtr & edAction);
	void deleteEditorAction(EditorAction * edAction);

	/**
	retrieve all objects that intersect the given ray
	*/
	void intersectAll(const EditorDocument * document, 
		const Ray & ray, std::vector<SceneObject *> * objects);
private:

	void intersectAll(const EditorDocument * document, 
		const SceneObject * parentObject, const Ray & ray,
		std::vector<SceneObject *> * objects);

private:
	Scene * scene_;		
	std::map<SceneObject *, EditorObjectSPtr> editorObjects_;
	std::map<AppObject *, EditorAppObjectSPtr> editorAppObjects_;
	std::map<Action *, EditorActionSPtr> editorActions_;
	
};