#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"
#include "Text_2.h"

class Command;

class EditorText_2 : public EditorObject
{
public:
	EditorText_2();
	virtual ~EditorText_2();
	virtual EditorObject * clone() const {return new EditorText_2(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:
};

///////////////////////////////////////////////////////////////////////////////
class TextEditTool;
class TextProperties_2;

class EditorText_2PropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorText_2PropertyPage();
	~EditorText_2PropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

	virtual void setDocument(EditorDocument * document);
	
	void onEditObject() { 
		bDoubleClick_ = true;
		onEdit(); 
		bDoubleClick_ = false;
	};
	void refreshTool(const EditorDocument * document);
private slots:
	void onInsertImg();
	void onEdit();
	void onToolChanged();
	void onMergeText();


private:
	TextProperties_2 getCurProperties() const;

	void propertiesChange(Text_2 *text, TextProperties_2 properties, TextProperties_2::Type type);
	void lineStyleChange(Text_2 *text, LineStyle lineStyle, LineStyle::Type type);
	void textChange(Command * changeCommand);

	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	FileNameProperty * fontFileName_;
	FontProperty * font_;
	EditProperty * fontSize_;
	ColorProperty * color_;

	QPushButton * insertImgButton_;

	ButtonProperty * moveLeftTapLeveButton_;
	ButtonProperty * moveRightTapLeveButton_;


	EditProperty * width_;
	EditProperty * height_;

	EditProperty * lineSpacing_;
	ComboBoxProperty * lineSpacingMode_;
	ComboBoxProperty * lineIncleaseMode_;
	ComboBoxProperty * textDirectionMode_;

	ToolButtonProperty * alignLeft_;
	ToolButtonProperty * alignCenter_;
	ToolButtonProperty * alignRight_;
	ToolButtonProperty * alignJustifyLeft_;

	CheckBoxProperty *cancleline_;
	CheckBoxProperty *underline_;
	CheckBoxProperty *shadow_;

	EditProperty * letterSpacing_;
	EditProperty * letterWidthRatio_;

	BoolProperty * canSeletPart_;

	QIntValidator sizeValidator_;
	QIntValidator spacingValidator_;
	QDoubleValidator spacingValidatorF_;
	FontValidator fontValidator_;

	boost::scoped_ptr<TextEditTool> textEditTool_;

	bool disableUpdate_;

	QPushButton * mergeButton_;

	QGroupBox *pLineStyleGroup_;

	bool bDoubleClick_;

	void updateEnable();
};