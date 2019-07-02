#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorTAnimation.h"
#include "Texture.h"
#include "TAnimation.h"
#include "GLWidget.h"
#include "Command.h"
#include "Scene.h"
#include "Utils.h"
#include "MaskObject.h"

using namespace std;

EditorTAnimation::EditorTAnimation()
{
}

EditorTAnimation::~EditorTAnimation()
{
}

SceneObject * EditorTAnimation::createSceneObject(EditorDocument * document) const
{
	TAnimationObject * obj = new TAnimationObject;	
	return obj;	
}

boost::uuids::uuid EditorTAnimation::sceneObjectType() const
{
	return TAnimationObject().type();
}

PropertyPage * EditorTAnimation::propertyPage() const
{
	return new EditorTAnimationPropertyPage;
}

void EditorTAnimation::drawObject(
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

	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{		

		gl->useColorProgram();
		gl->setColorProgramColor(0, 1, 0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);
		TAnimationObject * obj = (TAnimationObject *)sceneObject_;
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
EditorTAnimationPropertyPage::EditorTAnimationPropertyPage()
	: assumedLoadSpeedValidator_(1, INT_MAX)
{	
	setGroupBox(false);

	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;

	setName(tr("TAnimation"));
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);

	startGroup(tr("TAnimation"));

	loadSpeed_ = new EditProperty(tr("Assumed load speed (kb/s)"), &assumedLoadSpeedValidator_);
	addProperty(loadSpeed_, PropertyPage::Vertical);

	repeat_ = new BoolProperty(tr("repeat"));
	addProperty(repeat_);

	addSpacing(4);

	animXmlFile_ = new FileNameProperty(tr("Xml Animation Data"), tr("Xml Animation Data (*.xml)"));

	addProperty(animXmlFile_);
}

EditorTAnimationPropertyPage::~EditorTAnimationPropertyPage()
{
}

void EditorTAnimationPropertyPage::update()
{
	core_->update();
	visual_->update();
	TAnimationObject * obj = (TAnimationObject *)document_->selectedObject();	
	
	animXmlFile_->setValue(stdStringToQString(obj->animDataFileName()));
	
	QString str;
	str.sprintf("%d", (int)obj->loadSpeed());
	loadSpeed_->setValue(str);

	repeat_->setValue(obj->repeat());	
}

void EditorTAnimationPropertyPage::onChanged(Property * property)
{	
	TAnimationObject * obj = (TAnimationObject *)document_->selectedObject();

	if (property == animXmlFile_)
	{
		string val = qStringToStdString(animXmlFile_->value());

		document_->doCommand(makeChangeObjectCmd(obj, val, 
			&TAnimationObject::animDataFileName, 
			&TAnimationObject::setAnimDataFileName));
	}

	if (property == loadSpeed_)
	{
		float val = loadSpeed_->value().toInt();

		document_->doCommand(makeChangeObjectCmd(
			obj, val,
			&TAnimationObject::loadSpeed, 
			&TAnimationObject::setLoadSpeed));
	}	

	if (property == repeat_)
	{
		document_->doCommand(makeChangeObjectCmd(
			obj, repeat_->value(),
			&TAnimationObject::repeat, 
			&TAnimationObject::setRepeat));
	}	
}

void EditorTAnimationPropertyPage::setDocument(EditorDocument * document)
{
	PropertyPage::setDocument(document);
}

