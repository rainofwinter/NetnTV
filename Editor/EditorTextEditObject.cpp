#include "stdafx.h"
#include "EditorTextEditObject.h"
#include "TextEditObject.h"
#include "EditorDocument.h"
#include "Command.h"
#include "Utils.h"
#include "EditorScene.h"
#include "GLWidget.h"

using namespace std;

EditorTextEditObject::EditorTextEditObject() 
{
}

EditorTextEditObject::~EditorTextEditObject()
{
}

SceneObject * EditorTextEditObject::createSceneObject(EditorDocument * document) const
{	
	TextEditObject * textEditObj = new TextEditObject();
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
	textEditObj->setFont(font);
	textEditObj->setTextString(L"");
	textEditObj->inputTextDraw();
	return textEditObj;
}

boost::uuids::uuid EditorTextEditObject::sceneObjectType() const
{
	return TextEditObject().type();
}

PropertyPage * EditorTextEditObject::propertyPage() const
{
	return new EditorTextEditObjectPropertyPage;
}
void EditorTextEditObject::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{	
	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{
		gl->useColorProgram();
		gl->setColorProgramColor(0.75f, 0.75f, 0.75f, 0.75f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);

		TextEditObject * obj = (TextEditObject *)sceneObject_;

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
EditorTextEditObjectPropertyPage::EditorTextEditObjectPropertyPage() :
sizeValidator_(0, 9999, 0),
dimValidator_(0, 9999, 0)
{
	setGroupBox(false);

	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("TextEdit"));
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);

	startGroup(tr("TextEdit"));

	fontFileName_ = new FileNameProperty(
		tr("font file"), tr("Fonts (*.ttf *.otf *.ttc)"));
	addProperty(fontFileName_);

	font_ = new FontProperty(tr("font"));
	addProperty(font_);

	fontSize_ = new EditProperty(tr("size"), &sizeValidator_);
	addProperty(fontSize_);

	color_ = new ColorProperty(tr("color"));
	addProperty(color_);

	addSpacing(4);
	startGroup(tr("Line Spacing"));	
	lineSpacingMode_ = new ComboBoxProperty(tr("mode"));
	lineSpacingMode_->addItem(tr("Auto"));
	lineSpacingMode_->addItem(tr("Custom"));	
	addProperty(lineSpacingMode_);
	lineSpacing_ = new EditProperty(tr("spacing"), &sizeValidator_);
	addProperty(lineSpacing_);
	endGroup();
	addSpacing(4);

	width_ = new EditProperty(tr("width"), &dimValidator_);
	addProperty(width_);

	height_ = new EditProperty(tr("height"), &dimValidator_);
	addProperty(height_);

	endGroup();
}


EditorTextEditObjectPropertyPage::~EditorTextEditObjectPropertyPage()
{
}

void EditorTextEditObjectPropertyPage::update()
{
	core_->update();
	visual_->update();

	TextEditObject * textEditObj = (TextEditObject *)document_->selectedObject();

	const TextProperties_2 & properties = textEditObj->properties().front();
	const TextFont & font = properties.font;

	fontFileName_->setValue(stdStringToQString(font.fontFile));

	QString str;
	str.sprintf("%d", font.pointSize);
	fontSize_->setValue(str);

	color_->setValue(&textEditObj->color());
	
	str.sprintf("%d", (int)textEditObj->boundaryWidth());
	width_->setValue(str);

	str.sprintf("%d", (int)textEditObj->boundaryHeight());
	height_->setValue(str);

	font_->setValue(&font);

	lineSpacingMode_->setValue((int)textEditObj->lineSpacingMode());
	lineSpacing_->setEnabled(
		textEditObj->lineSpacingMode() != Text_2::LineSpacingAuto);

	str.sprintf("%d", textEditObj->lineSpacing());
	lineSpacing_->setValue(str);
}

void EditorTextEditObjectPropertyPage::onChanged(Property * property)
{
	TextEditObject * textEditObj = (TextEditObject *)document_->selectedObject();

	if (property == lineSpacingMode_)
	{
		Text_2::LineSpacingMode mode = 
			(Text_2::LineSpacingMode)lineSpacingMode_->value();

		document_->doCommand(makeChangeObjectCmd(
			textEditObj, mode, 
			&TextEditObject::lineSpacingMode, &TextEditObject::setLineSpacingMode));
		
		lineSpacing_->setEnabled(
			mode != TextProperties::LineSpacingAuto);
	}

	if (property == lineSpacing_)
	{
		int spacing = lineSpacing_->value().toInt();

		document_->doCommand(makeChangeObjectCmd(
			textEditObj, spacing, 
			&TextEditObject::lineSpacing, &TextEditObject::setLineSpacing));
	}

	if (property == fontFileName_)
	{
		
		string fileName = qStringToStdString(fontFileName_->value());
		int index = 0;

		TextFont font = textEditObj->font();
		font.fontFile = fileName;
		font.faceIndex = index;

		document_->doCommand(makeChangeObjectCmd(
			textEditObj, font, 
			&TextEditObject::font, &TextEditObject::setFont));		
	}

	if (property == font_)
	{
		const TextFont * font = font_->value();				
		
		if (font)
		{
			document_->doCommand(makeChangeObjectCmd(
				textEditObj, *font, 
				&TextEditObject::font, &TextEditObject::setFont));
		}
		
	}

	if (property == fontSize_)
	{
		int fontSize = fontSize_->value().toInt();
		TextFont font = textEditObj->font();
		font.pointSize = fontSize;
		document_->doCommand(makeChangeObjectCmd(
			textEditObj, font, 
			&TextEditObject::font, &TextEditObject::setFont));
	}

	if (property == color_)
	{
		Color color = color_->value();
		document_->doCommand(makeChangeObjectCmd(
			textEditObj, color, 
			&TextEditObject::color, &TextEditObject::setColor));
	}

	if (property == width_)
	{
		float width = (float)width_->value().toInt();
		document_->doCommand(makeChangeObjectCmd(
			textEditObj, width, 
			&TextEditObject::boundaryWidth, &TextEditObject::setBoundaryWidth));
	}

	if (property == height_)
	{
		float height = (float)height_->value().toInt();
		document_->doCommand(makeChangeObjectCmd(
			textEditObj, height, 
			&TextEditObject::boundaryHeight, &TextEditObject::setBoundaryHeight));
	}
}