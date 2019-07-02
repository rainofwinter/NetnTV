#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorImage : public EditorObject
{
public:
	EditorImage();
	virtual ~EditorImage();
	virtual EditorObject * clone() const {return new EditorImage(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorImagePropertyPage : public PropertyPage
{
public:
	EditorImagePropertyPage();
	~EditorImagePropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);	

private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	FileNameProperty * fileName_;
	EditProperty * width_, * height_;
	BoolProperty * pixelDraw_;

	QIntValidator dimValidator_;

};