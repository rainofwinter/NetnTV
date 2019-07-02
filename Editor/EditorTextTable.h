#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorTextTable : public EditorObject
{
public:
	EditorTextTable();
	virtual ~EditorTextTable();
	virtual EditorObject * clone() const {return new EditorTextTable(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:
};

///////////////////////////////////////////////////////////////////////////////
class TextTableTool;

class EditorTextTablePropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorTextTablePropertyPage();
	~EditorTextTablePropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

	virtual void setDocument(EditorDocument * document);

private:
	void doToolCtrlsIsEnabled();
private slots:
	void onToolChanged();
	void onTextTableTool();

private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	ComboBoxProperty * toolMode_;
	QGroupBox * toolGrpBox_;

	QPushButton * textTableToolButton_;
	boost::scoped_ptr<TextTableTool> textTableTool_;

	bool disableUpdate_;

};