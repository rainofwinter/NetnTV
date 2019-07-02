#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorAppImage.h"
#include "AppImage.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "Texture.h"
#include "FileUtils.h"
#include "Global.h"

using namespace std;
QString EditorAppImage::lastDir_;
EditorAppImage::EditorAppImage()
{
	texture_ = 0;
	loadedFileName_ = "";
}

EditorAppImage::~EditorAppImage()
{
	if (texture_ != 0) glDeleteTextures(1, &texture_);
}

AppObject * EditorAppImage::createAppObject() const
{	
	
	QString fileName = getOpenFileName(QObject::tr("Insert AppImage"), lastDir_, 
		QObject::tr("AppImages (*.png *.jpg *.jpeg)"));

	if (!fileName.isEmpty())
	{
		lastDir_ = QFileInfo(fileName).path();
		AppImage * img = new AppImage();		
		img->setFileName(convertToRelativePath(qStringToStdString(fileName)));	
		img->init();		
		img->setId(getFileTitle(qStringToStdString(fileName)));
		
		QImage qimage(fileName);
		img->setWidth(qimage.width());
		img->setHeight(qimage.height());
		return img;
	}
	else
		return 0;
}

boost::uuids::uuid EditorAppImage::appObjectType() const
{
	return AppImage().type();
}

PropertyPage * EditorAppImage::propertyPage() const
{
	return new EditorAppImagePropertyPage;
}

void EditorAppImage::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{
	AppImage * appImage = (AppImage *)appObject_;


	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
	gl->useTextureProgram();
	
	if (loadedFileName_ != appImage->fileName()) 
	{
		loadedFileName_ = appImage->fileName();
		unsigned char * bits;
		int width, height;
		Texture::Format compressionType;
		Global::instance().readImage(
			getAbsFileName(Global::instance().readDirectory(), loadedFileName_), 
			&compressionType, &bits, &width, &height);
		glGenTextures(1, &texture_);
		glBindTexture(GL_TEXTURE_2D, texture_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 
		glBindTexture(GL_TEXTURE_2D, texture_);		

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
			width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);

		free(bits);
	}
		
	glBindTexture(GL_TEXTURE_2D, texture_);
	
	gl->setTextureProgramOpacity(appImage->opacity());
	
	if (appImage->visible())
	{
		gl->drawRect(appImage->x(), appImage->y(), appImage->width(), appImage->height());
	}


	if (document->isAppObjectSelected(appObject_))
	{		
		gl->useColorProgram();
		gl->setColorProgramColor(1, 0, 0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);
		AppImage * image = (AppImage *)appObject_;
		int x = image->x();
		int y = image->y();
		GLfloat vertices[] = {
			static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0, 
			static_cast<GLfloat>(x + image->width()), static_cast<GLfloat>(y), 0,
			static_cast<GLfloat>(x + image->width()), static_cast<GLfloat>(y+ image->height()), 0,
			static_cast<GLfloat>(x), static_cast<GLfloat>(y + image->height()), 0
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
EditorAppImagePropertyPage::EditorAppImagePropertyPage() :
dimValidator_(0, 2048, 0)
{
	
	setGroupBox(false);
	
	core_ = new CoreAppObjectPropertiesPage;
	visual_ = new VisualAppObjectPropertiesPage;
	
	setName(tr("AppImage"));

	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);
	
	startGroup(tr("AppImage"));
	
	fileName_ = new FileNameProperty(
		tr("filename"), tr("AppImages (*.png *.jpg *.jpeg)"));
	addProperty(fileName_);

	
	endGroup();
	
}

EditorAppImagePropertyPage::~EditorAppImagePropertyPage()
{
}

void EditorAppImagePropertyPage::update()
{
	core_->update();
	visual_->update();
	AppImage * image = (AppImage *)document_->selectedAppObject();

	fileName_->setValue(stdStringToQString(image->fileName()));

}

void EditorAppImagePropertyPage::onChanged(Property * property)
{
	AppObject * obj = document_->selectedAppObject();
	AppImage * image = (AppImage *)obj;
	
	if (property == fileName_)
	{
		string fileName = qStringToStdString(fileName_->value());
		document_->doCommand(new ChangeAppImageFileNameCmd(
			document_, image, fileName));
	}

	update();
}