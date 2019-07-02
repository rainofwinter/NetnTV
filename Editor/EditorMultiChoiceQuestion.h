#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorMultiChoiceQuestion : public EditorCustomObject
{
public:
	EditorMultiChoiceQuestion();
	virtual const char * name() const {return "MultiChoiceQuestion";}
	virtual ~EditorMultiChoiceQuestion();
	virtual boost::uuids::uuid customType() const;
	virtual const std::string &  script() const {return script_;}

	virtual EditorObject * clone() const {return new EditorMultiChoiceQuestion(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:
	std::string script_;

};

///////////////////////////////////////////////////////////////////////////////

class EditorMultiChoiceQuestionPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorMultiChoiceQuestionPropertyPage();
	~EditorMultiChoiceQuestionPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

private slots:
	void onProperties();

private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	QPushButton * properties_;
};