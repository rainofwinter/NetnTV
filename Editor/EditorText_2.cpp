#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorText_2.h"
#include "Text_2.h"
#include "TextDlg.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "EditorScene.h"
#include "TextEditTool.h"
#include "MaskObject.h"

using namespace std;

EditorText_2::EditorText_2()
{
}

EditorText_2::~EditorText_2()
{
}

SceneObject * EditorText_2::createSceneObject(EditorDocument * document) const
{	
	Text_2 * text = new Text_2();
	text->setTextString(L"Text");
	QString fontDir = 
		QDesktopServices::storageLocation(QDesktopServices::FontsLocation);

	
	QString fontFileChoice1 = fontDir + "/malgun.ttf";
	QString fontFileChoice2 = fontDir + "/arial.ttf";
	QString fontFile;

	bool choice1Exists = QFile(fontFileChoice1).exists();

	if (choice1Exists)
		fontFile = fontFileChoice1;
	else
		fontFile = fontFileChoice2;

	TextFont font;
	font.fontFile = convertToRelativePath(qStringToStdString(fontFile));
	font.faceIndex = 0;
	font.bold = 0;
	font.italic = 0;
	font.pointSize = 16;

	std::vector<TextProperties_2> props = text->properties();
	props.front().font = font;
	text->setProperties(props);

	return text;
	
}

boost::uuids::uuid EditorText_2::sceneObjectType() const
{
	return Text_2().type();
}

PropertyPage * EditorText_2::propertyPage() const
{
	return new EditorText_2PropertyPage;
}

void EditorText_2::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{	
	SceneObject * parent = sceneObject_->parent();
	
	bool check = true;
	while(parent)
	{
		check = !(parent->type() == MaskObject().type());

		if(!check) break;
		parent = parent->parent();
	}

	if (/*isThisAndAncestorsVisible() && */sceneObject_->isThisAndAncestorsVisible() && check)
		sceneObject_->drawObject(gl);

	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{
		gl->useColorProgram();
		gl->setColorProgramColor(0.75f, 0.75f, 0.75f, 0.75f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);

		Text_2 * obj = (Text_2 *)sceneObject_;

		int boundaryHeight = obj->boundaryHeight();
		int boundaryWidth = obj->boundaryWidth();

		GLfloat vertices[] = {
			0, 0, 0, 
			static_cast<GLfloat>(boundaryWidth), 0, 0,
			static_cast<GLfloat>(boundaryWidth), static_cast<GLfloat>(boundaryHeight), 0,
			0, static_cast<GLfloat>(boundaryHeight), 0
		};
		gl->enableVertexAttribArrayPosition();
		gl->bindArrayBuffer(0);
		gl->vertexAttribPositionPointer(0, (char *)vertices);

		gl->applyCurrentShaderMatrix();
		glDrawArrays(GL_QUADS, 0, 4);

		vertices[0] = -2.0f;
		vertices[1] = -2.0f;
		vertices[4] = -2.0f;
		vertices[9] = -2.0f;
		vertices[3] = obj->width()+2.0f;
		vertices[6] = obj->width()+2.0f;
		vertices[7] = obj->height()+2.0f;
		vertices[10] = obj->height()+2.0f;
		
		gl->setColorProgramColor(0, 1, 0, 1);		
		glDrawArrays(GL_QUADS, 0, 4);

		glPolygonOffset(0, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	
}


///////////////////////////////////////////////////////////////////////////////
EditorText_2PropertyPage::EditorText_2PropertyPage() :
sizeValidator_(0, 9999, 0),
fontValidator_(0.1, 511.9, 1, 0),
spacingValidator_(-5000, 5000, 0),
spacingValidatorF_(-5000.0, 5000.0, 2)
{
	disableUpdate_ = false;
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("Text"));
	/*
	AddLabel(tr("Text"));
	AddSpacing(4);*/
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);
	
	startGroup(tr("Text"));
	
	insertImgButton_ = new QPushButton(tr("Image"));
	addWidget(insertImgButton_);
	connect(insertImgButton_, SIGNAL(clicked()), this, SLOT(onInsertImg()));

	//insertlGroupBox->setLayout(hinsertlayout);


	addSpacing(4);


	fontFileName_ = new FileNameProperty(
		tr("Font File"), tr("Fonts (*.ttf *.otf *.ttc)"));
	addProperty(fontFileName_);

	font_ = new FontProperty(tr("Font"));
	addProperty(font_);

	fontSize_ = new EditProperty(tr("Size"), &fontValidator_);
	addProperty(fontSize_);

	color_ = new ColorProperty(tr("Color"));
	addProperty(color_);

	letterWidthRatio_ = new EditProperty(tr("Letter Width Ratio"), &fontValidator_);
	addProperty(letterWidthRatio_);

	letterSpacing_ = new EditProperty(tr("Letter Spacing"), &spacingValidatorF_);
	addProperty(letterSpacing_);

	QHBoxLayout* heffectlayout = new QHBoxLayout();
	QGroupBox * effectlGroupBox = new QGroupBox(tr("Effect"));
	layout_->addWidget(effectlGroupBox, layout_->rowCount(), 0, 1, 2);

	underline_ = new CheckBoxProperty(tr("Under"));
	cancleline_ = new CheckBoxProperty(tr("Cancel"));
	shadow_ = new CheckBoxProperty(tr("Shadow"));

	heffectlayout->addWidget(underline_->widget());
	heffectlayout->addWidget(cancleline_->widget());
	heffectlayout->addWidget(shadow_->widget());


	properties_.push_back(underline_);
	properties_.push_back(cancleline_);
	properties_.push_back(shadow_);

	
	connect(underline_, SIGNAL(changed(Property *)), 
		this, SLOT(onChanged(Property * )));
	connect(cancleline_, SIGNAL(changed(Property *)), 
		this, SLOT(onChanged(Property * )));
	connect(shadow_, SIGNAL(changed(Property *)),
		this, SLOT(onChanged(Property * )));

	effectlGroupBox->setLayout(heffectlayout);

	addSpacing(4);

	pLineStyleGroup_ = startGroup(tr("Line Style"));

	lineIncleaseMode_ = new ComboBoxProperty(tr("Bullets"));
	lineIncleaseMode_->addItem(tr("None"));
	lineIncleaseMode_->addItem(QString(L'1.'));
	lineIncleaseMode_->addItem(QString(L'a.'));
	lineIncleaseMode_->addItem(QString(L'A.'));

	lineIncleaseMode_->addItem(QString(L'ич'));
	lineIncleaseMode_->addItem(QString(L'и═'));
	lineIncleaseMode_->addItem(QString(L'йч'));
	lineIncleaseMode_->addItem(QString(L'й═'));
	
	lineIncleaseMode_->addItem(QString(L'б╪'));
	lineIncleaseMode_->addItem(QString(L'бс'));
	lineIncleaseMode_->addItem(QString(L'бу'));
	lineIncleaseMode_->addItem(QString(L'в║'));
	lineIncleaseMode_->addItem(QString(L'б┌'));
	

	addProperty(lineIncleaseMode_);

	addSpacing(4);

	QHBoxLayout* hLineLevellayout = new QHBoxLayout();
	QGroupBox * lineLevelGroupBox = new QGroupBox(tr("Line Level"));
	layout_->addWidget(lineLevelGroupBox, layout_->rowCount(), 0, 1, 2);

	moveLeftTapLeveButton_ = new ButtonProperty(tr("<"));
	moveRightTapLeveButton_ = new ButtonProperty(tr(">"));

	hLineLevellayout->addWidget(moveLeftTapLeveButton_->widget());
	hLineLevellayout->addWidget(moveRightTapLeveButton_->widget());

 	properties_.push_back(moveLeftTapLeveButton_);
 	properties_.push_back(moveRightTapLeveButton_);

	connect(moveLeftTapLeveButton_, SIGNAL(changed(Property *)), 

		this, SLOT(onChanged(Property * )));
	connect(moveRightTapLeveButton_, SIGNAL(changed(Property *)), 
		this, SLOT(onChanged(Property * )));

	lineLevelGroupBox->setLayout(hLineLevellayout);

	addSpacing(4);

	QHBoxLayout* hLinelayout = new QHBoxLayout();
	QGroupBox * lineGroupBox = new QGroupBox(tr("Line Align"));
	layout_->addWidget(lineGroupBox, layout_->rowCount(), 0, 1, 2);
	
	alignLeft_ = new ToolButtonProperty(tr("left"));
	alignCenter_ = new ToolButtonProperty(tr("center"));
	alignRight_ = new ToolButtonProperty(tr("right"));
	alignJustifyLeft_ = new ToolButtonProperty(tr("justify_left"));

	hLinelayout->addWidget(alignLeft_->widget());
	hLinelayout->addWidget(alignCenter_->widget());
	hLinelayout->addWidget(alignRight_->widget());
	hLinelayout->addWidget(alignJustifyLeft_->widget());

	properties_.push_back(alignLeft_);
	properties_.push_back(alignCenter_);
	properties_.push_back(alignRight_);
	properties_.push_back(alignJustifyLeft_);

	connect(alignLeft_, SIGNAL(changed(Property *)), 
		this, SLOT(onChanged(Property * )));
	connect(alignCenter_, SIGNAL(changed(Property *)), 
		this, SLOT(onChanged(Property * )));
	connect(alignRight_, SIGNAL(changed(Property *)), 
		this, SLOT(onChanged(Property * )));
	connect(alignJustifyLeft_, SIGNAL(changed(Property *)), 
		this, SLOT(onChanged(Property * )));

	lineGroupBox->setLayout(hLinelayout);

	

	//pLineStyleGroup_->setEnabled(false);
	addSpacing(4);

	startGroup(tr("Line Spacing"));	
	lineSpacingMode_ = new ComboBoxProperty(tr("Mode"));
	lineSpacingMode_->addItem(tr("Auto"));
	lineSpacingMode_->addItem(tr("Custom"));	
	addProperty(lineSpacingMode_);
	lineSpacing_ = new EditProperty(tr("Spacing"), &spacingValidatorF_);
	addProperty(lineSpacing_);
	endGroup();
	endGroup();

	addSpacing(4);

	addSpacing(4);

	width_ = new EditProperty(tr("Width"), &sizeValidator_);
	addProperty(width_);

	height_ = new EditProperty(tr("Height"), &sizeValidator_);
	addProperty(height_);
	
	addSpacing(4);

	canSeletPart_ = new BoolProperty(tr("Select Part"));

	canSeletPart_->setValue(false);
	addProperty(canSeletPart_);
	addSpacing(4);
	
	textDirectionMode_ = NULL;

	mergeButton_ = new QPushButton(tr("Text Merge"));
	connect(mergeButton_, SIGNAL(clicked()), this, SLOT(onMergeText()));
	addWidget(mergeButton_);


	endGroup();
}

EditorText_2PropertyPage::~EditorText_2PropertyPage()
{
}

TextProperties_2 EditorText_2PropertyPage::getCurProperties() const
{
	TextProperties_2 ret;
	ret.index = 0;
	ret.color = color_->value();
	
	if (font_->value()) 
		ret.font = *font_->value();

	ret.font.pointSize = fontSize_->value().toFloat();

	ret.cancleline_ = cancleline_->value();
	ret.underline_ = underline_->value();
	ret.font.shadow = shadow_->value();
	ret.letterwidthratio_ = letterWidthRatio_->value().toFloat();
	return ret;
}

void EditorText_2PropertyPage::update()
{
	if(document_->selectedObject()->type() != Text_2().type())
		return;
	if (disableUpdate_) return;
	//	131218
	if (!document_) return;
	core_->update();
	visual_->update();



	Text_2 * text = (Text_2 *)document_->selectedObject();
	
	if(text->type() != Text_2().type())
		return;

	unsigned charStartIndex = 0;
	unsigned charEndIndex = UINT_MAX;

	if (textEditTool_->isActive() && text == textEditTool_->textObj())
	{
		charStartIndex = textEditTool_->charStartIndex();
		charEndIndex = textEditTool_->charEndIndex();

		
		if (charStartIndex == charEndIndex && charStartIndex > 0)
		{
			charStartIndex--;
			charEndIndex--;
		}		
	}

	vector<TextProperties_2> properties;
	
	if (textEditTool_->isActive() && textEditTool_->nextCharProps())
	{
		properties.push_back(*textEditTool_->nextCharProps());
	}
	else
	{
		text->getProperties(properties, charStartIndex, charEndIndex, false);
		assert(!properties.empty());
	}
	
	QString str;

	bool hasPointSize = true;
	bool hasFontFile = true;
	bool hasFont = true;
	bool hasColor = true;
	bool hasUndelline = true;
	bool hasCancleline = true;
	bool hasShadow = true;
	bool hasLetterWidthRatio = true;

	for (int i = 1; i < (int)properties.size(); ++i)
	{
		if (properties[0].font.pointSize != properties[i].font.pointSize)
			hasPointSize = false;
		if (properties[0].font.fontFile != properties[i].font.fontFile)
			hasFontFile = false;
		if (properties[0].color != properties[i].color)
			hasColor = false;
		
		if (properties[0].font.fontFile != properties[i].font.fontFile ||
			properties[0].font.faceIndex != properties[i].font.faceIndex ||
			properties[0].font.italic != properties[i].font.italic || 
			properties[0].font.bold != properties[i].font.bold)
			hasFont = false;

		if (properties[0].underline_ != properties[i].underline_)
			hasUndelline = false;

		if (properties[0].cancleline_ != properties[i].cancleline_)
			hasCancleline = false;

		if (properties[0].letterwidthratio_ != properties[i].letterwidthratio_)
			hasLetterWidthRatio = false;

		if (properties[0].font.shadow != properties[i].font.shadow)
			hasShadow = false;

	}

	if (hasFontFile)
		fontFileName_->setValue(stdStringToQString(properties[0].font.fontFile));
	else
		fontFileName_->setValue("");

	if (hasPointSize)
	{
		str.sprintf("%f", properties[0].font.pointSize);
		fontValidator_.fixup(str);
		//QStringList list = str.split(".");
		//int length = list[0].length() + 2;
		//((QLineEdit*)fontSize_->widget())->setMaxLength(length);
		fontSize_->setValue(str);
	}
	else
		fontSize_->setValue("");

	if (hasColor)
		color_->setValue(&properties[0].color);
	else
		color_->setValue(NULL);
	
	str.sprintf("%d", (int)text->boundaryWidth());
	width_->setValue(str);

	str.sprintf("%d", (int)text->boundaryHeight());
	height_->setValue(str);
	
	if (hasFont)
		font_->setValue(&properties[0].font);
	else
		font_->setValue(NULL);

	if(hasUndelline)
		underline_->setValue(properties[0].underline_);
	else
		underline_->setValue(0);

	if(hasCancleline)
		cancleline_->setValue(properties[0].cancleline_);
	else
		cancleline_->setValue(0);

	if(hasLetterWidthRatio){
		str.sprintf("%f", properties[0].letterwidthratio_);
		fontValidator_.fixup(str);
		letterWidthRatio_->setValue(str + "%");
	}else
		letterWidthRatio_->setValue("");

	if(hasShadow)
		shadow_->setValue(properties[0].font.shadow);
	else
		shadow_->setValue(0);
	
		

	bool hasLineIncreasemetStyle = true;
	bool hasLineAlignStyle = true;
	bool hasLineTapStyle = true;
	vector<LineStyle> lineStyle;

	if(textEditTool_->isActive() && text == textEditTool_->textObj())
	{
		int nStart = 0, nEnd = 0;
		textEditTool_->getCurLineIndex(nStart, nEnd);
		text->getLinesStyle(lineStyle, nStart, nEnd, false);

	}else{
		lineStyle = text->getLinesStyle();
	}

	for (int i = 1; i < (int)lineStyle.size(); ++i)
	{
		if (lineStyle[0].lineIncreasement_.type != lineStyle[i].lineIncreasement_.type)
			hasLineIncreasemetStyle = false;

		if (lineStyle[0].textAlign_ != lineStyle[i].textAlign_)
			hasLineAlignStyle = false;

		if (lineStyle[0].tabIndex_ != lineStyle[i].tabIndex_)
			hasLineTapStyle = false;

	}

	if (hasLineIncreasemetStyle){
		lineIncleaseMode_->setValue(LineStyle::getLineIncreasementIndex(lineStyle[0].lineIncreasement_.type));
	}else
		lineIncleaseMode_->setValue(-1);

	if(hasLineAlignStyle){

		ToolButtonProperty* arr[] = {alignLeft_, alignCenter_, alignRight_, alignJustifyLeft_};


		BOOST_FOREACH(LineStyle &curLineStyle, lineStyle){


			for (int i = 0 ; i < 4 ; i++)
			{
				if (i == curLineStyle.textAlign_ )
					arr[i]->setValue(true);
				else
					arr[i]->setValue(false);
			}

		}

	}else{
		alignLeft_->setValue(false);
		alignCenter_->setValue(false);
		alignRight_->setValue(false);
		alignJustifyLeft_->setValue(false);
	}

	if(hasLineTapStyle){

		BOOST_FOREACH(LineStyle &curLineStyle, lineStyle){

			if(text->textDirection() == Text_2::DRight){
				if(curLineStyle.tabIndex_ == 0)
					moveLeftTapLeveButton_->setEnable(false);
				else
					moveLeftTapLeveButton_->setEnable(true);
			}else{
				if(curLineStyle.tabIndex_ == 0)
					moveLeftTapLeveButton_->setEnable(true);
				else
					moveLeftTapLeveButton_->setEnable(false);
			}
		}
	}


	lineSpacingMode_->setValue((int)text->lineSpacingMode());

	lineSpacing_->setEnabled(
		text->lineSpacingMode() != Text_2::LineSpacingAuto);

	str.sprintf("%f", text->lineSpacing());

	str = QString::number(str.toFloat(),'f',2);

	lineSpacing_->setValue(str);
	

	str.sprintf("%f", text->letterSpacing());

	str = QString::number(str.toFloat(),'f',2);

	letterSpacing_->setValue(str);

	bool canSelectPart = text->canSelectPart();
	canSeletPart_->setValue(canSelectPart);

	if(textDirectionMode_)
		textDirectionMode_->setValue(text->textDirection());
	
	updateEnable();
	
}

void EditorText_2PropertyPage::updateEnable(){
	bool bToolActive = textEditTool_->isActive();

	insertImgButton_->setEnabled(bToolActive);
}
void EditorText_2PropertyPage::textChange(Command * command)
{
	if (textEditTool_->isActive())
		textEditTool_->doOp(new GlobalTextOperation(textEditTool_.get(), command), false);
	else
		document_->doCommand(command);
}

void EditorText_2PropertyPage::lineStyleChange(Text_2 *text, LineStyle lineStyle, LineStyle::Type type){
	
	std::vector<LineStyle> prevLineStyle = text->getLinesStyle();
	std::vector<LineStyle> newLineStyle;
	

	int nCharIndex = 0;
	int nStart = 0, nEnd = 0;

	if (textEditTool_->isActive())
	{
		textEditTool_->getCurLineIndex(nStart, nEnd);

	}else{
		nEnd = text->getLinesStyle().size() - 1;
	}

	lineStyle.index_ = nStart;

	text->applyLineStyle(lineStyle, type, nEnd - nStart);

	newLineStyle = text->getLinesStyle();


	if (textEditTool_->isActive())
	{
		

		LineStyleOperation * op = new LineStyleOperation(textEditTool_.get(), prevLineStyle, newLineStyle);

		textEditTool_->doOp(op, false);
		

	}else{

		text->setLinesStyle(prevLineStyle);

		document_->doCommand(makeChangeObjectCmd(
			text, newLineStyle, 
			&Text_2::getLinesStyle, &Text_2::setLinesStyle));
		text->init(document_->renderer());

		
	}

	//update();
	
}
void EditorText_2PropertyPage::propertiesChange(Text_2 *text, TextProperties_2 properties, TextProperties_2::Type type)
{

	if (textEditTool_->isActive())
	{
		if (textEditTool_->isSelectedPart())
		{
			properties.index = textEditTool_->charStartIndex();
			int charLen = textEditTool_->charEndIndex() - textEditTool_->charStartIndex();	

			std::vector<TextProperties_2> prevProps;
			text->getProperties(prevProps, properties.index, properties.index + charLen, true);

			text->applyProperties(properties, type, charLen);

			std::vector<TextProperties_2> newProps;
			text->getProperties(newProps, properties.index, properties.index + charLen, true);

			text->applyProperties(prevProps, properties.index, properties.index + charLen);

			PropertiesOperation * op = new PropertiesOperation(textEditTool_.get(), newProps, properties.index, properties.index + charLen);
			textEditTool_->doOp(op, false);

		}
		else
		{
			TextProperties_2 properties = getCurProperties();
			textEditTool_->setNextCharProps(properties);
		}
		
	}
	else
	{
		properties.index = 0;
		
		std::vector<TextProperties_2> prevProps = text->properties();
		text->applyProperties(properties, type);
		std::vector<TextProperties_2> newProps = text->properties();
		text->setProperties(prevProps);
		document_->doCommand(makeChangeObjectCmd(
			text, newProps, 
			&Text_2::properties, &Text_2::setProperties));
		text->init(document_->renderer());
	}
}


void EditorText_2PropertyPage::onChanged(Property * property)
{
	
	std::vector<SceneObject *> selObjects = document_->selectedObjects();
	
	BOOST_FOREACH(SceneObject * selObj, selObjects){

		if(!strcmp(selObj->typeStr(), "TextObject")){

			Text_2 * text = (Text_2 *)selObj;

			if(property == lineIncleaseMode_){
				LineStyle properties;		
				int mode = lineIncleaseMode_->value();

				if(mode > 0){
					int qValue = lineIncleaseMode_->value();

					if(qValue == 1)
						properties.lineIncreasement_.type = NUMBER;
					else if(qValue == 2)
						properties.lineIncreasement_.type = LOWERER_CASE_ALPHABET;
					else if(qValue == 3)
						properties.lineIncreasement_.type = UPPER_CASE_ALPHABET;
					else if(qValue == 4)
						properties.lineIncreasement_.type = CIRCLE_NUMBER;
					else if(qValue == 5)
						properties.lineIncreasement_.type = CIRCLE_ALPHABET;
					else if(qValue == 6)
						properties.lineIncreasement_.type = BRACKET_NUMBER;
					else if(qValue == 7)
						properties.lineIncreasement_.type = BRACKET_ALPHABET;
					else if(qValue == 8)
						properties.lineIncreasement_.type = EXCLAMATION_POINT;
					else if(qValue == 9)
						properties.lineIncreasement_.type = RECTANGLE;
					else if(qValue == 10)
						properties.lineIncreasement_.type = TRIAGLE_1;
					else if(qValue == 11)
						properties.lineIncreasement_.type = TRIAGLE_2;
					else if(qValue == 12)
						properties.lineIncreasement_.type = STAR;
				}


				LineStyle::Type type = LineStyle::Type::LineStyleIncleasement;

				lineStyleChange(text, properties, type);


			}

			if (property == alignLeft_ || property == alignCenter_ || 
				property == alignRight_ || property == alignJustifyLeft_)
			{
				LineStyle properties;

				LineStyle::Type type = LineStyle::Type::LineStyleAlign;

				if(property == alignLeft_)
					properties.textAlign_ = LineStyle::Left;
				else if(property == alignCenter_)
					properties.textAlign_ = LineStyle::Center;
				else if(property == alignRight_)
					properties.textAlign_ = LineStyle::Right;
				else if(property == alignJustifyLeft_)
					properties.textAlign_ = LineStyle::Justify_Left;
				
				lineStyleChange(text, properties, type);



				ToolButtonProperty* arr[] = {alignLeft_, alignCenter_, alignRight_, alignJustifyLeft_};


				for (int i = 0 ; i < 4 ; i++)
				{
					if (arr[i] == property)
						arr[i]->setValue(true);
					else
						arr[i]->setValue(false);
				}
			}

			if (property == moveLeftTapLeveButton_ || property == moveRightTapLeveButton_)
			{
				LineStyle properties;

				LineStyle::Type type = LineStyle::Type::LineStyleTap;


				if(text->textDirection() == Text_2::DRight){
					if(property == moveLeftTapLeveButton_)
						properties.tabIndex_ = -1;
					else if(property == moveRightTapLeveButton_)
						properties.tabIndex_ = 1;
				}else{
					if(property == moveLeftTapLeveButton_)
						properties.tabIndex_ = 1;
					else if(property == moveRightTapLeveButton_)
						properties.tabIndex_ = -1;
				}

				lineStyleChange(text, properties, type);


			}

			if (property == fontFileName_)
			{		
				string fileName = qStringToStdString(fontFileName_->value());

				TextProperties_2 properties;		
				properties.font.fontFile = fileName;
				TextProperties_2::Type type = TextProperties_2::PropertyFontFileName;

				propertiesChange(text, properties, type);
			}

			if (property == font_)
			{
				const TextFont * font = font_->value();

				if (font)
				{
					TextProperties_2 properties;		
					properties.font = *font;
					TextProperties_2::Type type = TextProperties_2::PropertyFont;

					propertiesChange(text, properties, type);
				}

			}

			if (property == underline_)
			{
				TextProperties_2 properties;		
				properties.underline_ = underline_->value();
				TextProperties_2::Type type = TextProperties_2::PropertyUnderLine;

				propertiesChange(text, properties, type);

			}

			if (property == cancleline_)
			{
				TextProperties_2 properties;		
				properties.cancleline_ = cancleline_->value();
				TextProperties_2::Type type = TextProperties_2::PropertyCancleLine;

				propertiesChange(text, properties, type);

			}

			if(property == letterWidthRatio_){
				TextProperties_2 properties;		
				properties.letterwidthratio_ = letterWidthRatio_->value().toFloat();
				TextProperties_2::Type type = TextProperties_2::PropertyLetterWidthRatio;

				propertiesChange(text, properties, type);
			}

			if (property == shadow_)
			{
				TextProperties_2 properties;		
				properties.font.shadow = shadow_->value();
				TextProperties_2::Type type = TextProperties_2::PropertyFontShandow;

				propertiesChange(text, properties, type);

			}

			if (property == fontSize_)
			{
				float fontSize = fontSize_->value().toFloat();

				TextProperties_2 properties;		
				properties.font.pointSize = fontSize;
				TextProperties_2::Type type = TextProperties_2::PropertyFontSize;

				propertiesChange(text, properties, type);
			}

			if (property == color_)
			{
				Color color = color_->value();

				TextProperties_2 properties;		
				properties.color = color;
				TextProperties_2::Type type = TextProperties_2::PropertyColor;

				propertiesChange(text, properties, type);	
			}

			if (property == width_)
			{
				float width = (float)width_->value().toInt();

				Command * command = makeChangeObjectCmd(
					text, width, 
					&Text_2::boundaryWidth, &Text_2::setBoundaryWidth);

				textChange(command);		
			}

			if (property == height_)
			{
				float height = (float)height_->value().toInt();
				textChange(makeChangeObjectCmd(
					text, height, 
					&Text_2::boundaryHeight, &Text_2::setBoundaryHeight));
			}

			if (property == letterSpacing_)
			{
				float letterSpacing = letterSpacing_->value().toFloat();

				textChange(makeChangeObjectCmd(
					text, letterSpacing, 
					&Text_2::letterSpacing, &Text_2::setLetterSpacing));
			}

			if (property == lineSpacingMode_)
			{
				Text_2::LineSpacingMode lineSpacingMode = (Text_2::LineSpacingMode)lineSpacingMode_->value();

				textChange(makeChangeObjectCmd(
					text, lineSpacingMode, 
					&Text_2::lineSpacingMode, &Text_2::setLineSpacingMode));
			}

			if (property == lineSpacing_)
			{
				float spacing = lineSpacing_->value().toFloat();

				textChange(makeChangeObjectCmd(
					text, spacing, &Text_2::lineSpacing, &Text_2::setLineSpacing));
			}

			if (property == canSeletPart_)
			{
				bool canSelectPart = canSeletPart_->value();
				textChange(makeChangeObjectCmd(
					text, canSelectPart, 
					&Text_2::canSelectPart, &Text_2::setCanSelectPart));
			}

			if (property == textDirectionMode_)
			{
				int direction = textDirectionMode_->value();
				textChange(makeChangeObjectCmd(
					text, direction, 
					&Text_2::textDirection, &Text_2::setTextDirection));
			}
		}
	}


	if (textEditTool_->isActive())
		textEditTool_->restoreFocus();

	update();
}

void EditorText_2PropertyPage::onInsertImg()
{
	QString fileName = getOpenFileName(QObject::tr("Insert Image"), QString(), 
		QObject::tr("Images (*.png *.jpg *.jpeg)"));
	
	if (!fileName.isEmpty())
	{
		textEditTool_->insertImg(convertToRelativePath(qStringToStdString(fileName)));

	}
}

void EditorText_2PropertyPage::onEdit()
{
	if(!document_)
		return;

	if (bDoubleClick_ == true)
	{
		Text_2 * text = (Text_2 *)document_->selectedObject();
		textEditTool_->setTextObj(text);
		document_->glWidget()->setTool(textEditTool_.get());
	}
	else
	{
		document_->glWidget()->setToolMode(document_->glWidget()->prevMode());
	}

	updateEnable();
}

void EditorText_2PropertyPage::setDocument(EditorDocument * document)
{
	if (document_)
	{
		GLWidget * glWidget = document_->glWidget();
		disconnect(glWidget, 0, this, 0);
	}

	//this function sets document_ field appropriately
	PropertyPage::setDocument(document);

	if (document)
	{
		GLWidget * glWidget = document->glWidget();

		connect(glWidget, SIGNAL(toolChanged()), this, SLOT(onToolChanged()));

		if (!textEditTool_)
		{
			textEditTool_.reset(new TextEditTool(glWidget));
		}
		Text_2 * text = (Text_2 *)document_->selectedObject();
		textEditTool_->setTextObj(text);
		textEditTool_->refresh();
		if (glWidget->tool() == textEditTool_.get()) glWidget->setToolMode(GLWidget::Select);
		
	}
}

void EditorText_2PropertyPage::refreshTool(const EditorDocument * document)
{
	//GLWidget * glWidget = document->glWidget();
	if (textEditTool_)
	{
		Text_2 * text = (Text_2 *)document->selectedObject();
		textEditTool_->refresh();
		textEditTool_->setTextObj(text);
	}
}

void EditorText_2PropertyPage::onToolChanged()
{
	updateEnable();
}

void EditorText_2PropertyPage::onMergeText()
{
	std::vector<SceneObject *> objs = document_->selectedObjects();
	if (objs.size() < 2) return;

	std::vector<Text_2 *>::iterator itr;

	std::vector<Text_2* >texts;
	static boost::uuids::uuid type = Text_2().type();

	for(int i = 0 ; i < objs.size() ; i++)
	{
		if(objs[i]->type() != type)
			return;

		if (texts.empty())
		{
			texts.push_back((Text_2*)objs[i]);
		}
		else
		{
			bool isInsert = false;
			for(itr = texts.begin() ; itr != texts.end() ; itr++)
			{
				Transform refT = (*itr)->transform();
				Transform curT = objs[i]->transform();

				if (curT.translation().y < refT.translation().y)
				{
					texts.insert(itr, (Text_2*)objs[i]);
					isInsert = true;
					break;
				}
			}
			if (!isInsert)
				texts.push_back((Text_2*)objs[i]);
		}
	}
	document_->doCommand(new MergeTextsCmd(document_, document_->selectedScene(), texts));
}