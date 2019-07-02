#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorRectangle.h"
#include "Rectangle.h"
#include "GLWidget.h"
#include "Command.h"
#include "MaskObject.h"

using namespace std;

EditorRectangle::EditorRectangle()
{
}

EditorRectangle::~EditorRectangle()
{
}

SceneObject * EditorRectangle::createSceneObject(EditorDocument * document) const
{
	Studio::Rectangle * rect = new Studio::Rectangle;
	return rect;
}

boost::uuids::uuid EditorRectangle::sceneObjectType() const
{
	return Studio::Rectangle().type();
}

PropertyPage * EditorRectangle::propertyPage() const
{
	return new EditorRectanglePropertyPage;
}

void EditorRectangle::drawObject(
	GfxRenderer * gl, const EditorDocument * document) const
{
	gl->useColorProgram();
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
		Studio::Rectangle * retangle = (Studio::Rectangle *)sceneObject_;
		GLfloat vertices[] = {
			0, 0, 0, 
			static_cast<GLfloat>(retangle->width()), 0, 0,
			static_cast<GLfloat>(retangle->width()), static_cast<GLfloat>(retangle->height()), 0,
			0, static_cast<GLfloat>(retangle->height()), 0
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
EditorRectanglePropertyPage::EditorRectanglePropertyPage() :
dimValidator_(0, 2048, 0)
{
	
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("Rectangle"));
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);

	startGroup(tr("Rectangle"));
	color_ = new ColorProperty(tr("color"));
	addProperty(color_);
	addSpacing(4);
	width_ = new EditProperty(tr("width"), &dimValidator_);
	addProperty(width_);
	height_ = new EditProperty(tr("height"), &dimValidator_);
	addProperty(height_);
	endGroup();
		
}

EditorRectanglePropertyPage::~EditorRectanglePropertyPage()
{
}

void EditorRectanglePropertyPage::update()
{
	core_->update();
	visual_->update();
	Studio::Rectangle * rect = (Studio::Rectangle *)document_->selectedObject();

	color_->setValue(&rect->color());

	QString str;
	str.sprintf("%.4f", rect->width());
	width_->setValue(str);
	str.sprintf("%.4f", rect->height());
	height_->setValue(str);
}

void EditorRectanglePropertyPage::onChanged(Property * property)
{	
	Studio::Rectangle * rect = (Studio::Rectangle *)document_->selectedObject();
	if (property == color_)
	{
		Color color = color_->value();
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, rect, color, 
			&Studio::Rectangle::color, 
			&Studio::Rectangle::setColor));
	}

	if (property == width_)
	{
		float val = width_->value().toFloat();
		document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
			rect, val, &Studio::Rectangle::width, &Studio::Rectangle::setWidth));
	}

	if (property == height_)
	{
		float val = height_->value().toFloat();
		document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
			rect, val, &Studio::Rectangle::height, &Studio::Rectangle::setHeight));
	}
}
