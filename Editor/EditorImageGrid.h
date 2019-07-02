#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorImageGrid : public EditorObject
{
public:
	EditorImageGrid();
	virtual ~EditorImageGrid();
	virtual void init();
	virtual EditorObject * clone() const {return new EditorImageGrid(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:
	static QString lastDir_;
};

///////////////////////////////////////////////////////////////////////////////

class EditorImageGridPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorImageGridPropertyPage();
	~EditorImageGridPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

private slots:

	void onFiles();

private:

	void enableAnimationFields(bool enable);
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	
	EditProperty * thumbWidth_, *thumbHeight_, *gap_;
	EditProperty * duration_, *startTime_;
	EditProperty * width_, * height_;
	BoolProperty * doAnimation_;

	QPushButton * filesButton_;
	QIntValidator dimValidator_;

	QDoubleValidator timeValidator_;

};