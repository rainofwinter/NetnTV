#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorTextEditObject : public EditorObject
{
public:
	EditorTextEditObject();
	virtual ~EditorTextEditObject();
	virtual EditorObject * clone() const {return new EditorTextEditObject(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorTextEditObjectPropertyPage : public PropertyPage
{
public:
	EditorTextEditObjectPropertyPage();
	~EditorTextEditObjectPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	FileNameProperty * fontFileName_;
	FontProperty * font_;
	EditProperty * fontSize_;
	ColorProperty * color_;

	EditProperty * width_;
	EditProperty * height_;

	EditProperty * lineSpacing_;
	ComboBoxProperty * lineSpacingMode_;

	QIntValidator sizeValidator_;	
	QIntValidator dimValidator_;
};