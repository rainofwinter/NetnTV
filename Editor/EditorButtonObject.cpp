#include "StdAfx.h"
#include "EditorButtonObject.h"
#include "EditorDocument.h"
#include "ButtonObject.h"
#include "GLWidget.h"
#include "Command.h"
#include "Texture.h"
#include "Utils.h"
#include "FileUtils.h"
#include "ButtonObjectDlg.h"
#include "MaskObject.h"

using namespace std;

EditorButtonObject::EditorButtonObject()
{
}

EditorButtonObject::~EditorButtonObject()
{
}

SceneObject * EditorButtonObject::createSceneObject(EditorDocument * document) const
{
	 //Studio::ButtonObject * buttonObject = new  Studio::ButtonObject;
	ButtonObject * buttonObject = new  ButtonObject;
	
	QWidget * dlgParent = document->glWidget();
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();
	ButtonObjectDlg dlg(dlgParent, document->document(), buttonObject->fileNames());

	if (dlg.exec() == QDialog::Accepted)
	{
		buttonObject->setFileNames(dlg.fileNames());
		buttonObject->init(renderer());

		const vector<TextureSPtr> & buffers = buttonObject->buffers();

		if (!buffers.empty())
		{
			int trWidth, trHeight;
			Global::instance().getImageDims(buffers[0]->fileName(), &trWidth, &trHeight);

			buttonObject->setWidth(trWidth);
			buttonObject->setHeight(trHeight);
		}

		return buttonObject;
	}else{
		delete buttonObject;
		return NULL;
	}
}

boost::uuids::uuid EditorButtonObject::sceneObjectType() const
{
	//return  Studio::ButtonObject().type();
	return  ButtonObject().type();
}

PropertyPage * EditorButtonObject::propertyPage() const
{
	return new EditorButtonObjectPropertyPage;
}

void EditorButtonObject::drawObject(GfxRenderer * gl, const EditorDocument * document) const
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
	gl->resetStencil();
	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{		

		gl->useColorProgram();
		gl->setColorProgramColor(0, 1, 0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);
		//Studio::ButtonObject * obj = (Studio::ButtonObject *)sceneObject_;
		ButtonObject * obj = (ButtonObject *)sceneObject_;
		GLfloat vertices[] = {
			0, 0, 0, 
			static_cast<GLfloat>(obj->width()), 0, 0,
			static_cast<GLfloat>(obj->width()), static_cast<GLfloat>(obj->height()), 0,
			0, static_cast<GLfloat>(obj->height()), 0
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

EditorButtonObjectPropertyPage::EditorButtonObjectPropertyPage() :
dimValidator_(0, 2048, 0)
{
	setGroupBox(false);

	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;

	setName(tr("ButtonObject"));

	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);

	startGroup(tr("ButtonObject"));

	pressFileName_ = new FileNameProperty(
		tr("pressFile"), tr("Images(*.png *.jpg *.jpeg)"));
	addProperty(pressFileName_);
	addSpacing(4);

	releaseFileName_ = new FileNameProperty(
		tr("ReleaseFile"), tr("Images(*.png *.jpg *.jpeg)"));
	addProperty(releaseFileName_);
	addSpacing(4);

	width_ = new EditProperty(tr("width"), &dimValidator_);
	addProperty(width_);
	height_ = new EditProperty(tr("height"), &dimValidator_);
	addProperty(height_);

	endGroup();

}

EditorButtonObjectPropertyPage::~EditorButtonObjectPropertyPage()
{

}

void EditorButtonObjectPropertyPage::update()
{
	core_->update();
	visual_->update();

	//Studio::ButtonObject * buttonObject = 
		//(Studio::ButtonObject *)document_->selectedObject();
	ButtonObject * buttonObject = 
		(ButtonObject *)document_->selectedObject();

	QString str;
	str.sprintf("%.4f", buttonObject->width());
	width_->setValue(str);
	str.sprintf("%.4f", buttonObject->height());
	height_->setValue(str);
	pressFileName_->setValue(stdStringToQString(buttonObject->fileNames()[0]));
	releaseFileName_->setValue(stdStringToQString(buttonObject->fileNames()[1]));
}

void EditorButtonObjectPropertyPage::onChanged(Property * property)
{
	ButtonObject * buttonObject = (ButtonObject *)document_->selectedObject();

	if (property == pressFileName_)
	{
		string pressName = qStringToStdString(pressFileName_->value());
		string releaseName = qStringToStdString(releaseFileName_->value());
		document_->doCommand(new SetButtonObjectPressFileName(
			document_, buttonObject, pressName, releaseName, buttonObject->fileNames()));
	}

	if (property == releaseFileName_)
	{
		string pressName = qStringToStdString(pressFileName_->value());
		string releaseName = qStringToStdString(releaseFileName_->value());
		document_->doCommand(new SetButtonObjectPressFileName(
			document_, buttonObject, pressName, releaseName, buttonObject->fileNames()));
	}

	if (property == width_)
	{		
		document_->doCommand(makeChangeObjectCmd(
			buttonObject, width_->value().toFloat(), 
			&ButtonObject::width, &ButtonObject::setWidth));
	}

	if (property == height_)
	{
		document_->doCommand(makeChangeObjectCmd(
			buttonObject, height_->value().toFloat(), 
			&ButtonObject::height, &ButtonObject::setHeight));
	}
}

void EditorButtonObjectPropertyPage::setDocument(EditorDocument * document)
{
	PropertyPage::setDocument(document);
}

//////////////////////////////////////////////////////

SetButtonObjectPressFileName::SetButtonObjectPressFileName(
	EditorDocument * doc, ButtonObject * obj, const std::string pressName, const std::string releaseName,
	const std::vector<std::string> & fileNames)
{
	doc_ = doc;
	obj_ = obj;
	origFileName_ = obj->fileNames();	
	pressName_ = pressName;
	releaseName_ = releaseName;
	origWidth_ = obj->width();
	origHeight_ = obj->height();
}

void SetButtonObjectPressFileName::doCommand()
{
	fileNames_.clear();
	fileNames_.push_back(pressName_);
	fileNames_.push_back(releaseName_);
	obj_->setFileNames(fileNames_);
	obj_->init(doc_->renderer());
	const vector<TextureSPtr> & buffers = obj_->buffers();
	if (!buffers.empty())
	{
		int trWidth, trHeight;
		Global::instance().getImageDims(buffers[0]->fileName(), &trWidth, &trHeight);

		obj_->setWidth(trWidth);
		obj_->setHeight(trHeight);
	}
	emit objectChanged();
}

void SetButtonObjectPressFileName::undoCommand()
{
	obj_->setWidth(origWidth_);
	obj_->setHeight(origHeight_);
	obj_->setFileNames(origFileName_);	
	obj_->init(doc_->renderer());
	emit objectChanged();
}

SetButtonObjectReleaseFileName::SetButtonObjectReleaseFileName(
	EditorDocument * doc, ButtonObject * obj, const std::string pressName, const std::string releaseName,
	const std::vector<std::string> & fileNames)
{
	doc_ = doc;
	obj_ = obj;
	origFileName_ = obj->fileNames();	
	pressName_ = pressName;
	releaseName_ = releaseName;
	origWidth_ = obj->width();
	origHeight_ = obj->height();
}

void SetButtonObjectReleaseFileName::doCommand()
{
	fileNames_.clear();
	fileNames_.push_back(pressName_);
	fileNames_.push_back(releaseName_);
	obj_->setFileNames(fileNames_);
	obj_->init(doc_->renderer());
	const vector<TextureSPtr> & buffers = obj_->buffers();
	if (!buffers.empty())
	{
		int trWidth, trHeight;
		Global::instance().getImageDims(buffers[0]->fileName(), &trWidth, &trHeight);

		obj_->setWidth(trWidth);
		obj_->setHeight(trHeight);
	}
	emit objectChanged();
}

void SetButtonObjectReleaseFileName::undoCommand()
{
	obj_->setWidth(origWidth_);
	obj_->setHeight(origHeight_);
	obj_->setFileNames(origFileName_);	
	obj_->init(doc_->renderer());
	emit objectChanged();
}