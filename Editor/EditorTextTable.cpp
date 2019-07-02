#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorTextTable.h"
#include "TextTable.h"
#include "TextTableTool.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "EditorScene.h"
#include "MaskObject.h"

using namespace std;

EditorTextTable::EditorTextTable()
{
}

EditorTextTable::~EditorTextTable()
{
}

SceneObject * EditorTextTable::createSceneObject(EditorDocument * document) const
{	
	TextTable * textTable = new TextTable();
	
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
	font.fontFile = qStringToStdString(fontFile);
	font.faceIndex = 0;
	font.bold = 0;
	font.italic = 0;
	font.pointSize = 16;

	textTable->setFont(font);
	
	
	return textTable;
	
}

boost::uuids::uuid EditorTextTable::sceneObjectType() const
{
	return TextTable().type();
}

PropertyPage * EditorTextTable::propertyPage() const
{
	return new EditorTextTablePropertyPage;
}

void EditorTextTable::drawObject(GfxRenderer * gl, const EditorDocument * document) const
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
		gl->setColorProgramColor(0, 1, 0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);
		TextTable * textTable = (TextTable *)sceneObject_;

		int x = textTable->gridStartX();
		int y = textTable->gridStartY();
		GLfloat vertices[] = {
			static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0, 
			static_cast<GLfloat>(x + textTable->width()), static_cast<GLfloat>(y), 0,
			static_cast<GLfloat>(x + textTable->width()), static_cast<GLfloat>(y + textTable->height()), 0,
			static_cast<GLfloat>(x), static_cast<GLfloat>(y + textTable->height()), 0
		};
		gl->enableVertexAttribArrayPosition();
		gl->bindArrayBuffer(0);
		gl->vertexAttribPositionPointer(0, (char *)vertices);
		gl->applyCurrentShaderMatrix();
		glDrawArrays(GL_QUADS, 0, 4);
		glPolygonOffset(0, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	
}


///////////////////////////////////////////////////////////////////////////////
EditorTextTablePropertyPage::EditorTextTablePropertyPage()
{
	disableUpdate_ = false;
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("Text Table"));
	/*
	AddLabel(tr("TextTable"));
	AddSpacing(4);*/
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);
	
	startGroup(tr("TextTable"));

	toolGrpBox_ = startGroup(tr("Text Table Tool"));
	textTableToolButton_ = new QPushButton(tr("Text Table Tool"));
	textTableToolButton_->setCheckable(true);
	connect(textTableToolButton_, SIGNAL(clicked()), this, SLOT(onTextTableTool()));
	addWidget(textTableToolButton_);

	toolMode_ = new ComboBoxProperty(tr("Mode"));
	toolMode_->addItem(tr("Draw"));
	toolMode_->addItem(tr("Erase"));
	toolMode_->addItem(tr("Text Edit"));
	addSpacing(4);
	addProperty(toolMode_);
	endGroup();

	endGroup();
}

EditorTextTablePropertyPage::~EditorTextTablePropertyPage()
{
}


void EditorTextTablePropertyPage::onTextTableTool()
{
	if (!textTableToolButton_->isChecked())
		document_->glWidget()->setToolMode(GLWidget::Select);
	else
	{
		TextTable * textTable = (TextTable *)document_->selectedObject();
		textTableTool_->setTextTableObj(textTable);
		document_->glWidget()->setTool(textTableTool_.get());
		doToolCtrlsIsEnabled();
	}
}

void EditorTextTablePropertyPage::update()
{
	if (disableUpdate_) return;
	core_->update();
	visual_->update();
	TextTable * textTable = (TextTable *)document_->selectedObject();

	textTableToolButton_->setChecked(textTableTool_.get() == document_->glWidget()->tool());
	doToolCtrlsIsEnabled();

	toolMode_->setValue((int)textTableTool_->mode());
}

void EditorTextTablePropertyPage::doToolCtrlsIsEnabled()
{
	toolMode_->setEnabled(textTableToolButton_->isChecked());
}

void EditorTextTablePropertyPage::onChanged(Property * property)
{
	TextTable * textTable = (TextTable *)document_->selectedObject();

	disableUpdate_ = true;
	if (textTableTool_.get() == document_->glWidget()->tool())
	{
		//TODO
		//textTableTool_->doCommitTextCommand();
	}
	disableUpdate_ = false;

	if (property == toolMode_)
	{
		TextTableTool::Mode mode = (TextTableTool::Mode)toolMode_->value();
		textTableTool_->setMode(mode);
	}

}


void EditorTextTablePropertyPage::setDocument(EditorDocument * document)
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

		if (!textTableTool_)
			textTableTool_.reset(new TextTableTool(glWidget));
	}
}

void EditorTextTablePropertyPage::onToolChanged()
{
	textTableToolButton_->setChecked(textTableTool_.get() == document_->glWidget()->tool());
	doToolCtrlsIsEnabled();
}