#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"
#include "Types.h"

class EditorTAnimation : public EditorObject
{
public:
	EditorTAnimation();
	virtual ~EditorTAnimation();
	virtual EditorObject * clone() const {return new EditorTAnimation(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:

};

///////////////////////////////////////////////////////////////////////////////
class Scene;


class EditorTAnimationPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorTAnimationPropertyPage();
	~EditorTAnimationPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);
	virtual void setDocument(EditorDocument * document);

private:

	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;

	FileNameProperty * animXmlFile_;

	EditProperty * loadSpeed_;
	QIntValidator assumedLoadSpeedValidator_;

	BoolProperty * repeat_;
	
};

