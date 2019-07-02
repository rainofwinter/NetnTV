#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorImageAnimation.h"
#include "Texture.h"
#include "ImageGridDlg.h"
#include "ImageAnimation.h"
#include "GLWidget.h"
#include "Command.h"
#include "Scene.h"
#include "Utils.h"
#include "MaskObject.h"

using namespace std;

EditorImageAnimation::EditorImageAnimation()
{
}

EditorImageAnimation::~EditorImageAnimation()
{
}

SceneObject * EditorImageAnimation::createSceneObject(EditorDocument * document) const
{
	ImageAnimation * obj = new ImageAnimation;
	
	QWidget * dlgParent = document->glWidget();
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();
	ImageGridDlg dlg(dlgParent, document->document(), obj->fileNames());

	if (dlg.exec() == QDialog::Accepted)
	{
		obj->setFileNames(dlg.fileNames());

		obj->init(renderer());		

		const vector<TextureSPtr> & buffers = obj->buffers();
		if (!buffers.empty())
		{
			int trWidth, trHeight;
			Global::instance().getImageDims(buffers[0]->fileName(), &trWidth, &trHeight);

			obj->setWidth(trWidth);
			obj->setHeight(trHeight);
		}


		return obj;
	}
	else
	{
		delete obj;
		return NULL;
	}
	
}

boost::uuids::uuid EditorImageAnimation::sceneObjectType() const
{
	return ImageAnimation().type();
}

PropertyPage * EditorImageAnimation::propertyPage() const
{
	return new EditorImageAnimationPropertyPage;
}

void EditorImageAnimation::drawObject(
	GfxRenderer * gl, const EditorDocument * document) const
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
		ImageAnimation * obj = (ImageAnimation *)sceneObject_;
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
EditorImageAnimationPropertyPage::EditorImageAnimationPropertyPage()
	: validator_(0, 9000, 4, 0), fpsValidator_(0.1, 10000, 3, 0),
	maxMemFramesVal_(1, INT_MAX)
{	
	setGroupBox(false);

	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;

	setName(tr("Rectangle"));
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);

	startGroup(tr("ImageAnimation"));

	imagesButton_ = new QPushButton(tr("Images..."));
	connect(imagesButton_, SIGNAL(clicked()), this, SLOT(onImages()));
	addWidget(imagesButton_);

	addSpacing(4);

	fps_ = new EditProperty(tr("fps"), &fpsValidator_);
	addProperty(fps_);

	repeat_ = new BoolProperty(tr("repeat"));
	addProperty(repeat_);

	addSpacing(4);

	width_ = new EditProperty(tr("width"), &validator_);
	addProperty(width_);
	height_ = new EditProperty(tr("height"), &validator_);
	addProperty(height_);
	addSpacing(4);

	maxMemFrames_ = new EditProperty(tr("max loaded frames"), &maxMemFramesVal_);
	addProperty(maxMemFrames_);

	format_ = new ComboBoxProperty(tr("Format"));
	format_->addItem(tr("RGB16"));
	format_->addItem(tr("RGBA16"));
	format_->addItem(tr("RGBA32"));
	addProperty(format_);
}

EditorImageAnimationPropertyPage::~EditorImageAnimationPropertyPage()
{
}

void EditorImageAnimationPropertyPage::onImages()
{
	ImageAnimation * obj = 
		(ImageAnimation *)document_->selectedObject();

	QWidget * dlgParent = topWidget_;
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();
	ImageGridDlg dlg(dlgParent, document_->document(), obj->fileNames());

	if (dlg.exec() == QDialog::Accepted)
	{
		document_->doCommand(			
			new SetImageAnimationFileNames(document_, obj, dlg.fileNames()));		
	}

	update();
}

void EditorImageAnimationPropertyPage::update()
{
	core_->update();
	visual_->update();
	ImageAnimation * obj = (ImageAnimation *)document_->selectedObject();	

	QString str;
	str.sprintf("%.4f", obj->width());
	width_->setValue(str);
	str.sprintf("%.4f", obj->height());
	height_->setValue(str);

	repeat_->setValue(obj->repeat());
	fps_->setValue(QString::number(obj->fps()));

	maxMemFrames_->setValue(QString::number(obj->maxMemFrames()));
	format_->setValue((int)obj->format());
}

void EditorImageAnimationPropertyPage::onChanged(Property * property)
{	
	ImageAnimation * obj = (ImageAnimation *)document_->selectedObject();

	if (property == width_)
	{
		document_->doCommand(makeChangeObjectCmd(
			obj, width_->value().toFloat(),
			&ImageAnimation::width, 
			&ImageAnimation::setWidth));
	}

	if (property == height_)
	{
		document_->doCommand(makeChangeObjectCmd(
			obj, height_->value().toFloat(),
			&ImageAnimation::height, 
			&ImageAnimation::setHeight));
	}	

	if (property == repeat_)
	{
		document_->doCommand(makeChangeObjectCmd(
			obj, repeat_->value(),
			&ImageAnimation::repeat, 
			&ImageAnimation::setRepeat));
	}

	if (property == fps_)
	{
		document_->doCommand(makeChangeObjectCmd(
			obj, fps_->value().toFloat(),
			&ImageAnimation::fps, 
			&ImageAnimation::setFps));
	}

	if (property == maxMemFrames_)
	{
		document_->doCommand(makeChangeObjectCmd(
			obj, maxMemFrames_->value().toInt(),
			&ImageAnimation::maxMemFrames, 
			&ImageAnimation::setMaxMemFrames));
	}

	if (property == format_)
	{
		document_->doCommand(makeChangeObjectCmd(
			obj, (Texture::Format)format_->value(),
			&ImageAnimation::format, 
			&ImageAnimation::setFormat));
	}
}

void EditorImageAnimationPropertyPage::setDocument(EditorDocument * document)
{
	PropertyPage::setDocument(document);
}

///////////////////////////////////////////////////////////////////////////////

SetImageAnimationFileNames::SetImageAnimationFileNames(EditorDocument * doc, ImageAnimation * obj, const std::vector<std::string> & fileNames)
{
	doc_ = doc;
	obj_ = obj;
	origFileNames_ = obj->fileNames();
	origWidth_ = obj->width();
	origHeight_ = obj->height();
	
	fileNames_ = fileNames;
}

void SetImageAnimationFileNames::doCommand()
{
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

void SetImageAnimationFileNames::undoCommand()
{
	obj_->setWidth(origWidth_);
	obj_->setHeight(origHeight_);
	obj_->setFileNames(origFileNames_);	
	obj_->init(doc_->renderer());
	emit objectChanged();
}
