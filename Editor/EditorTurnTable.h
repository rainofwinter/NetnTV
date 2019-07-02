#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorTurnTable : public EditorObject
{
public:
	EditorTurnTable();
	virtual ~EditorTurnTable();
	virtual EditorObject * clone() const {return new EditorTurnTable(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:
	static QString lastDir_;
};

///////////////////////////////////////////////////////////////////////////////

class EditorTurnTablePropertyPage : public PropertyPage
{
public:
	EditorTurnTablePropertyPage();
	~EditorTurnTablePropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	EditProperty * speed_;
	QDoubleValidator validator_;

};