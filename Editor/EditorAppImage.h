#pragma once
#include "EditorAppObject.h"
#include "PropertyPage.h"

class EditorAppImage : public EditorAppObject
{
public:
	EditorAppImage();
	virtual ~EditorAppImage();
	virtual EditorAppObject * clone() const {return new EditorAppImage(*this);}
	virtual AppObject * createAppObject() const;
	virtual boost::uuids::uuid appObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
	void resetImageLoadFlag() {loadedFileName_ = "";}
private:
	static QString lastDir_;
	mutable GLuint texture_;
	mutable std::string loadedFileName_;
};

///////////////////////////////////////////////////////////////////////////////

class EditorAppImagePropertyPage : public PropertyPage
{
public:
	EditorAppImagePropertyPage();
	~EditorAppImagePropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

private:
	CoreAppObjectPropertiesPage * core_;
	VisualAppObjectPropertiesPage * visual_;
	FileNameProperty * fileName_;

	QIntValidator dimValidator_;

};