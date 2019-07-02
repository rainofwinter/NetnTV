#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorImage.h"
#include "Image.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "Texture.h"
#include "FileUtils.h"
#include "MaskObject.h"

using namespace std;
EditorImage::EditorImage()
{
}

EditorImage::~EditorImage()
{
}

SceneObject * EditorImage::createSceneObject(EditorDocument * document) const
{	
	
	QString fileName = getOpenFileName(QObject::tr("Insert Image"), QString(), 
		QObject::tr("Images (*.png *.jpg *.jpeg)"));


	if (!fileName.isEmpty())
	{
		Image * img = new Image();
		img->setFileName(convertToRelativePath(qStringToStdString(fileName)));	
		//extraneous init... but needed for now so that img width/height is fully 
		//determined at this point
		img->init(renderer());
		
		int trWidth, trHeight;
		Global::instance().getImageDims(img->fileName(), &trWidth, &trHeight);
		
		if (trWidth > 2048 || trHeight > 2048)
		{
			QMessageBox::information(
				0, 
				QObject::tr("Error"), 
				QObject::tr("Image width or height cannot exceed 2048 pixels"));
			delete img;
			return 0;
		}


		img->setId(getFileTitle(qStringToStdString(fileName)));
		return img;
	}
	else
		return 0;
}

boost::uuids::uuid EditorImage::sceneObjectType() const
{
	return Image().type();
}

PropertyPage * EditorImage::propertyPage() const
{
	return new EditorImagePropertyPage;
}

void EditorImage::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
	gl->useTextureProgram();
	
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
		Image * image = (Image *)sceneObject_;
		GLfloat vertices[] = {
			0, 0, 0, 
			static_cast<GLfloat>(image->width()), 0, 0,
			static_cast<GLfloat>(image->width()), static_cast<GLfloat>(image->height()), 0,
			0, static_cast<GLfloat>(image->height()), 0
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
EditorImagePropertyPage::EditorImagePropertyPage() :
dimValidator_(0, 2048, 0)
{
	
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("Image"));
	/*
	AddLabel(tr("Image"));
	AddSpacing(4);*/
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);
		
	startGroup(tr("Image"));
	
	fileName_ = new FileNameProperty(
		tr("filename"), tr("Images (*.png *.jpg *.jpeg)"));
	addProperty(fileName_);

	addSpacing(4);
	width_ = new EditProperty(tr("width"), &dimValidator_);
	addProperty(width_);
	height_ = new EditProperty(tr("height"), &dimValidator_);
	addProperty(height_);

	addSpacing(4);
	pixelDraw_ = new BoolProperty(tr("PixelDraw"));
	addProperty(pixelDraw_);

	endGroup();
	
}

EditorImagePropertyPage::~EditorImagePropertyPage()
{
}

void EditorImagePropertyPage::update()
{
	core_->update();
	visual_->update();
	Image * image = (Image *)document_->selectedObject();

	fileName_->setValue(stdStringToQString(image->fileName()));

	QString str;
	str.sprintf("%d", image->width());
	width_->setValue(str);
	str.sprintf("%d", image->height());
	height_->setValue(str);

	bool pixelDraw = image->pixelDraw();
	pixelDraw_->setValue(pixelDraw);

}

void EditorImagePropertyPage::onChanged(Property * property)
{
	Image * image = (Image *)document_->selectedObject();
	
	if (property == fileName_)
	{
		string fileName = qStringToStdString(fileName_->value());
		document_->doCommand(new ChangeImageFileNameCmd(
			document_, image, fileName));
	}

	if (property == width_)
	{
		int val = width_->value().toInt();
		document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
			image, val, &Image::width, &Image::setWidth));
	}

	if (property == height_)
	{
		int val = height_->value().toInt();
		document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
			image, val, &Image::height, &Image::setHeight));
	}

	if (property == pixelDraw_)
	{
		bool pixelDraw = pixelDraw_->value();
		document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
			image, pixelDraw, &Image::pixelDraw, &Image::setPixelDraw));
	}


	update();
}
