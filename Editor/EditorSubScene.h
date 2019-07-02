#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"
#include "Types.h"

class EditorSubScene : public EditorObject
{
public:
	EditorSubScene();
	virtual ~EditorSubScene();
	virtual EditorObject * clone() const {return new EditorSubScene(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:

};

///////////////////////////////////////////////////////////////////////////////
class Scene;


class EditorSubScenePropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorSubScenePropertyPage();
	~EditorSubScenePropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

	virtual void setDocument(EditorDocument * document);

private slots:

	void getScenesList(std::vector<SceneSPtr> * scenes);

	void onSceneListChanged();

private:
	void enableModeFields();

	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	ComboBoxProperty * scene_;

	ComboBoxProperty * cameraMode_;

	ComboBoxProperty * mode_;

	EditProperty * windowWidth_, * windowHeight_;
	EditProperty * maxMinusDrag_, * maxPlusDrag_;
	EditProperty * maxTopDrag_, * maxBotDrag_;
	ComboBoxProperty * dragDirection_;
	BoolProperty * dragSnap_;

	BoolProperty * transparentBg_;
	BoolProperty * linkWithViewerLevelObjects_;

	QDoubleValidator validator_;

	



};