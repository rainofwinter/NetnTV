#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"
#include "Types.h"

class EditorImageAnimation : public EditorObject
{
public:
	EditorImageAnimation();
	virtual ~EditorImageAnimation();
	virtual EditorObject * clone() const {return new EditorImageAnimation(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:

};

///////////////////////////////////////////////////////////////////////////////
class Scene;


class EditorImageAnimationPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorImageAnimationPropertyPage();
	~EditorImageAnimationPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);
	virtual void setDocument(EditorDocument * document);

	private slots:

		void onImages();

private:

	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;

	QPushButton * imagesButton_;
	EditProperty * width_, * height_;
	QDoubleValidator validator_;

	EditProperty * fps_;
	QDoubleValidator fpsValidator_;
	BoolProperty * repeat_;


	EditProperty * maxMemFrames_;
	QIntValidator maxMemFramesVal_;

	ComboBoxProperty * format_;
};

///////////////////////////////////////////////////////////////////////////////
#include "Command.h"

class ImageAnimation;

class SetImageAnimationFileNames : public Command
{
public:
	SetImageAnimationFileNames(EditorDocument * document, ImageAnimation * obj, const std::vector<std::string> & fileNames);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Set image animation files");}
private:
	EditorDocument * doc_;
	ImageAnimation * obj_;
	std::vector<std::string> fileNames_;
	int width_, height_;

	std::vector<std::string> origFileNames_;
	int origWidth_, origHeight_;
};
