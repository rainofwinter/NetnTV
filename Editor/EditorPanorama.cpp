#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorPanorama.h"
#include "Panorama.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "MaskObject.h"

using namespace std;
QString EditorPanorama::lastDir_;
EditorPanorama::EditorPanorama()
{
}

EditorPanorama::~EditorPanorama()
{
}

SceneObject * EditorPanorama::createSceneObject(EditorDocument * document) const
{	
	Panorama * obj = new Panorama();		
	return obj;
}

boost::uuids::uuid EditorPanorama::sceneObjectType() const
{
	return Panorama().type();
}

PropertyPage * EditorPanorama::propertyPage() const
{
	return new EditorPanoramaPropertyPage;
}

void EditorPanorama::drawObject(GfxRenderer * gl, const EditorDocument * document) const
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
		Panorama * image = (Panorama *)sceneObject_;
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
EditorPanoramaPropertyPage::EditorPanoramaPropertyPage() :
validator_(0, 1024, 4, 0)
{
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("Panorama"));

	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);
	
	startGroup(tr("Panorama"));	
	
	width_ = new EditProperty(tr("width"), &validator_);
	addProperty(width_);

	height_ = new EditProperty(tr("height"), &validator_);
	addProperty(height_);

	addSpacing(4);

	QString filter = QObject::tr("Images (*.png *.jpg *.jpeg)");

	front_ = new FileNameProperty(tr("front"), filter);
	addProperty(front_);
	left_ = new FileNameProperty(tr("left"), filter);
	addProperty(left_);
	right_ = new FileNameProperty(tr("right"), filter);
	addProperty(right_);
	top_ = new FileNameProperty(tr("top"), filter);
	addProperty(top_);
	bottom_ = new FileNameProperty(tr("bottom"), filter);
	addProperty(bottom_);
	back_ = new FileNameProperty(tr("back"), filter);
	addProperty(back_);

	endGroup();
	
}

EditorPanoramaPropertyPage::~EditorPanoramaPropertyPage()
{
}

void EditorPanoramaPropertyPage::update()
{
	core_->update();
	visual_->update();
	Panorama * obj = (Panorama *)document_->selectedObject();

	QString str;
	str.sprintf("%.4f", obj->width());
	width_->setValue(str);
	str.sprintf("%.4f", obj->height());
	height_->setValue(str);	

	front_->setValue(stdStringToQString(obj->frontFile()));
	left_->setValue(stdStringToQString(obj->leftFile()));
	right_->setValue(stdStringToQString(obj->rightFile()));
	top_->setValue(stdStringToQString(obj->topFile()));
	bottom_->setValue(stdStringToQString(obj->downFile()));
	back_->setValue(stdStringToQString(obj->backFile()));
}

void EditorPanoramaPropertyPage::onChanged(Property * property)
{
	Panorama * obj = (Panorama *)document_->selectedObject();
	
	if (property == width_)
	{
		float val = width_->value().toFloat();
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &Panorama::width, &Panorama::setWidth));
	}

	if (property == height_)
	{
		float val = height_->value().toFloat();
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &Panorama::height, &Panorama::setHeight));
	}

	if (property == front_)
	{
		string val = qStringToStdString(front_->value());
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &Panorama::frontFile, &Panorama::setFrontFile));
	}
	if (property == back_)
	{
		string val = qStringToStdString(back_->value());
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &Panorama::backFile, &Panorama::setBackFile));
	}
	if (property == left_)
	{
		string val = qStringToStdString(left_->value());
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &Panorama::leftFile, &Panorama::setLeftFile));
	}
	if (property == right_)
	{
		string val = qStringToStdString(right_->value());
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &Panorama::rightFile, &Panorama::setRightFile));
	}
	if (property == top_)
	{
		string val = qStringToStdString(top_->value());
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &Panorama::topFile, &Panorama::setTopFile));
	}
	if (property == bottom_)
	{
		string val = qStringToStdString(bottom_->value());
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &Panorama::downFile, &Panorama::setDownFile));
	}


}
