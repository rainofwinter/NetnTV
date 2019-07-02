#pragma once

class SceneObject;
class PropertyPage;
class GLWidget;
class EditorDocument;
class EditorScene;

class EditorObject
{
public:	
	EditorObject();
	virtual ~EditorObject();
	virtual EditorObject * clone() const = 0;
	virtual PropertyPage * propertyPage() const {return 0;}
	virtual void init();

	///response to a New Object command (may show appropriate object creation 
	///dialog box)
	virtual SceneObject * createSceneObject(EditorDocument * document) const = 0;
	virtual boost::uuids::uuid sceneObjectType() const = 0;
	virtual void draw(GfxRenderer * gl, const EditorDocument * document) const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;

	void setSceneObject(SceneObject * obj) {sceneObject_ = obj;}
	SceneObject * sceneObject() const {return sceneObject_;}

	/*
	const bool & isUiVisible() const {return uiVisible_;}
	void setUiVisible(const bool & vis) {uiVisible_ = vis;}
*/
	const bool & isUiLocked() const {return uiLocked_;}
	void setUiLocked(const bool & locked) {uiLocked_ = locked;}

	bool isAncestorUiLocked() const;
	//bool isAncestorNotUiVisible() const;

	//bool isThisAndAncestorsVisible() const;
	bool isThisAndAncestorsUnlocked() const;

	void setScene(EditorScene * scene) {parentScene_ = scene;}
	EditorScene * parentScene() const {return parentScene_;}

	GfxRenderer * renderer() const;

	virtual SceneObject * intersect(const EditorDocument * document, const Ray & ray, 
		Vector3 * intPt);

protected:
	EditorScene * parentScene_;
	SceneObject * sceneObject_;
	//bool uiVisible_;
	bool uiLocked_;
};

///////////////////////////////////////////////////////////////////////////////

class EditorCustomObject : public EditorObject
{
public:
	virtual const char * name() const = 0;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual boost::uuids::uuid customType() const = 0;
	virtual const std::string & script() const = 0;
private:

};