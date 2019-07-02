#pragma once

#include "EditorObject.h"
#include "PropertyPage.h"
#include "Types.h"

class EditorButtonObject : public EditorObject
{
public:
	EditorButtonObject();
	virtual ~EditorButtonObject();
	virtual EditorObject * clone() const {return new EditorButtonObject(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
};

/////////////////////////////////////////////////////////

class Scene;

class EditorButtonObjectPropertyPage : public PropertyPage
{
public:
	EditorButtonObjectPropertyPage();
	~EditorButtonObjectPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);
	virtual void setDocument(EditorDocument * document);

private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	FileNameProperty * pressFileName_, * releaseFileName_;
	EditProperty * width_, * height_;

	QDoubleValidator dimValidator_;
};

//////////////////////////////////////////////////////
#include "Command.h"

class ButtonObject;

class SetButtonObjectPressFileName : public Command
{
public:
	SetButtonObjectPressFileName(EditorDocument * document, 
		ButtonObject * obj, const std::string pressName, const std::string releaseName,
		const std::vector<std::string> & fileNames);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Set image file");}
private:
	EditorDocument * doc_;
	ButtonObject * obj_;
	std::string pressName_, releaseName_;
	std::vector<std::string> origFileName_ , fileNames_;
	int origWidth_, origHeight_;
};

class SetButtonObjectReleaseFileName : public Command
{
public:
	SetButtonObjectReleaseFileName(EditorDocument * document, 
		ButtonObject * obj, const std::string pressName, const std::string releaseName,
		const std::vector<std::string> & fileNames);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Set image file");}
private:
	EditorDocument * doc_;
	ButtonObject * obj_;
	std::string pressName_, releaseName_;
	std::vector<std::string> origFileName_ , fileNames_;
	int origWidth_, origHeight_;
};