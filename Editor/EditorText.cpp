#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorText.h"
#include "Text.h"
#include "TextDlg.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "EditorScene.h"

using namespace std;

EditorText::EditorText()
{
}

EditorText::~EditorText()
{
}

SceneObject * EditorText::createSceneObject(EditorDocument * document) const
{	
	Text * text = new Text();
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

	std::vector<TextProperties> props = text->properties();
	props.front().font = font;
	text->setProperties(props);

	//text->setFont(font);
	return text;
	
}

boost::uuids::uuid EditorText::sceneObjectType() const
{
	return Text().type();
}

PropertyPage * EditorText::propertyPage() const
{
	return new EditorTextPropertyPage;
}

void EditorText::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{	
	if (/*isThisAndAncestorsVisible() && */sceneObject_->isThisAndAncestorsVisible())
		sceneObject_->drawObject(gl);

	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{
		gl->useColorProgram();
		gl->setColorProgramColor(0.75f, 0.75f, 0.75f, 0.75f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);

		Text * obj = (Text *)sceneObject_;

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
EditorTextPropertyPage::EditorTextPropertyPage()
{
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("Text"));

	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);

	convertBtn_ = new QPushButton(tr("Text Convert"));
	addWidget(convertBtn_);
	connect(convertBtn_, SIGNAL(clicked()), this, SLOT(onConvert()));
}

EditorTextPropertyPage::~EditorTextPropertyPage()
{
}

void EditorTextPropertyPage::update()
{
	core_->update();
	visual_->update();
}

void EditorTextPropertyPage::onChanged(Property * property)
{
	Text * text = (Text *)document_->selectedObject();
}

void EditorTextPropertyPage::setDocument(EditorDocument * document)
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
	}
}

void EditorTextPropertyPage::onConvert()
{
	document_->texttoText_2Convert();
}