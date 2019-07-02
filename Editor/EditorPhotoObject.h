#pragma once

#include "EditorObject.h"
#include "PropertyPage.h"
#include "PhotoObject.h"

class EditorPhotoObject :public EditorObject
{
public:
	EditorPhotoObject();
	virtual ~EditorPhotoObject();
	virtual EditorObject * clone() const {return new EditorPhotoObject(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:
};

/////////////////////////////////////////////////////////////

class Scene;

class EditorPhotoObjectPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorPhotoObjectPropertyPage();
	~EditorPhotoObjectPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);
	virtual void setDocument(EditorDocument * document);

private:
	QIntValidator dimValidator_;
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	FileNameProperty * fileName_;
	ComboBoxProperty * rate_;
	EditProperty * x_, * y_, * width_, *height_;
};