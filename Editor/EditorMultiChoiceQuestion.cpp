#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorMultiChoiceQuestion.h"
#include "GLWidget.h"
#include "CustomObject.h"
#include <boost/scope_exit.hpp>
#include "MultiChoiceQuestionDlg.h"
#include "MainWindow.h"
#include "Command.h"
using namespace std;

EditorMultiChoiceQuestion::EditorMultiChoiceQuestion()
{
	QFile file(":/data/Resources/multiChoice.js");
	file.open(QIODevice::ReadOnly);

	QByteArray bytes = file.readAll();
	script_.clear();
	script_.insert(script_.begin(), bytes.begin(), bytes.end());
	file.close();
}

EditorMultiChoiceQuestion::~EditorMultiChoiceQuestion()
{
}

boost::uuids::uuid EditorMultiChoiceQuestion::customType() const
{
	boost::uuids::string_generator gen;
	return gen("99ED2222-3AB7-2799-1717-82D5364987AF");
}



SceneObject * EditorMultiChoiceQuestion::createSceneObject(EditorDocument * edocument) const
{	
	/*
	string scriptFile = "C:/git_working/GlContentEditor/Editor/Resources/multiChoice.js";
	FILE * file = 0;	
	std::vector<char> buffer;
	BOOST_SCOPE_EXIT( (&file)) {
		if (file) fclose(file);
    } BOOST_SCOPE_EXIT_END

	file = fopen(scriptFile.c_str(), "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);	
		long fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);
		buffer.resize(fileSize);
		fread(&buffer[0], 1, fileSize, file);
		EditorMultiChoiceQuestion * o = const_cast<EditorMultiChoiceQuestion *>(this);
		o->script_.clear();
		o->script_.insert(script_.begin(), buffer.begin(), buffer.end());
	}
	*/

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

	Document * document = edocument->document();
	
	
	QByteArray fontFileBytes = fontFile.toUtf8();
	std::string fontFileStr(fontFileBytes.begin(), fontFileBytes.end());

	string str = string("\
<data>\
<font size='18'>" + fontFileBytes + "</font>\
<fontSize>18</fontSize>\
<x>0</x>\
<y>0</y>\
<width>640</width>\
<height>480</height>\
<text>This is the question text</text>\
<uncheckedImage></uncheckedImage>\
<checkedImage></checkedImage>\
<checkAnswerImage></checkAnswerImage>\
<choice>choice 1</choice>\
<choice>choice 2</choice>\
<choice>choice 3</choice>\
<choice>choice 4</choice>\
<correctChoice>0</correctChoice>\
</data>\
	");
	
	CustomObject * obj = new CustomObject;
	obj->setData(str);
	

	MultiChoiceQuestionDlg dlg(edocument->glWidget()->mainWindow(), obj);
	if (!dlg.exec())
	{
		delete obj;
		return NULL;
	}

	QByteArray utf8Str = dlg.xml().toUtf8();
	std::string utf8StdStr(utf8Str.begin(), utf8Str.end());
	obj->setData(utf8StdStr);	

	obj->setCustomType(customType());
	document->registerCustomObject(customType(), name(), script_);

	return obj;
}

PropertyPage * EditorMultiChoiceQuestion::propertyPage() const
{
	return new EditorMultiChoiceQuestionPropertyPage;
}

void EditorMultiChoiceQuestion::drawObject(
	GfxRenderer * gl, const EditorDocument * document) const
{
	gl->useColorProgram();
	if (/*isThisAndAncestorsVisible() && */sceneObject_->isThisAndAncestorsVisible())
		sceneObject_->drawObject(gl);


	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{		
		gl->useColorProgram();
		gl->setColorProgramColor(0, 1, 0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);
		BoundingBox bb = sceneObject_->extents();
		GLfloat vertices[] = {
			static_cast<GLfloat>(bb.minPt.x), static_cast<GLfloat>(bb.minPt.y, 0), 
			static_cast<GLfloat>(bb.maxPt.x), static_cast<GLfloat>(bb.minPt.y, 0),
			static_cast<GLfloat>(bb.maxPt.x), static_cast<GLfloat>(bb.maxPt.y, 0),
			static_cast<GLfloat>(bb.minPt.x), static_cast<GLfloat>(bb.maxPt.y, 0)
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
EditorMultiChoiceQuestionPropertyPage::EditorMultiChoiceQuestionPropertyPage()
{
	
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("MultiChoiceQuestion"));
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);

	startGroup(tr("MultiChoiceQuestion"));
	properties_ = new QPushButton(tr("Properties..."));
	addWidget(properties_);
	endGroup();

	connect(properties_, SIGNAL(clicked()), this, SLOT(onProperties()));
		
}

EditorMultiChoiceQuestionPropertyPage::~EditorMultiChoiceQuestionPropertyPage()
{
}

void EditorMultiChoiceQuestionPropertyPage::onProperties()
{
	CustomObject * obj = (CustomObject *)document_->selectedObject();
	MultiChoiceQuestionDlg dlg(document_->glWidget()->mainWindow(), obj);
	if (!dlg.exec()) return;

	QByteArray utf8Str = dlg.xml().toUtf8();
	std::string utf8StdStr(utf8Str.begin(), utf8Str.end());
	document_->doCommand(makeChangeObjectCmd(
		obj, utf8StdStr, 
		&CustomObject::data, &CustomObject::setData));

}

void EditorMultiChoiceQuestionPropertyPage::update()
{
	core_->update();
	visual_->update();
	CustomObject * obj = (CustomObject *)document_->selectedObject();
}

void EditorMultiChoiceQuestionPropertyPage::onChanged(Property * property)
{	
	CustomObject * obj = (CustomObject *)document_->selectedObject();
	
}
