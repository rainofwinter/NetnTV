#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class Model;

class EditorCameraObject : public EditorObject
{
public:
	EditorCameraObject();
	virtual ~EditorCameraObject();
	virtual EditorObject * clone() const {return new EditorCameraObject(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;

	virtual SceneObject * intersect(const EditorDocument * document, const Ray & ray, 
		Vector3 * intPt);

private:

	Model * camModel_;
	Matrix camModelMatrix_;
};

///////////////////////////////////////////////////////////////////////////////

class EditorCameraObjectPropertyPage : public PropertyPage
{
public:
	EditorCameraObjectPropertyPage();
	~EditorCameraObjectPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);	

private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;


	EditProperty * fovY_;
	QDoubleValidator fovValidator_;

	EditProperty * nearPlane_;
	EditProperty * farPlane_;
	QDoubleValidator nearPlaneValidator_;
	QDoubleValidator farPlaneValidator_;

};