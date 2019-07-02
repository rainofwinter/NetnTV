#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"
#include "Text.h"

class Command;

class EditorText : public EditorObject
{
public:
	EditorText();
	virtual ~EditorText();
	virtual EditorObject * clone() const {return new EditorText(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:
};

///////////////////////////////////////////////////////////////////////////////
class TextEditTool;
struct TextProperties;

class EditorTextPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorTextPropertyPage();
	~EditorTextPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

	virtual void setDocument(EditorDocument * document);
private slots:
	void onConvert();
private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;

	QPushButton * convertBtn_;
};