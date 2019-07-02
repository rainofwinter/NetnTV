#include "StdAfx.h"
#include "EditorPhotoObject.h"
#include "EditorDocument.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "MaskObject.h"

using namespace std;

EditorPhotoObject::EditorPhotoObject()
{
}


EditorPhotoObject::~EditorPhotoObject()
{
}


boost::uuids::uuid EditorPhotoObject::sceneObjectType() const
{
	return PhotoObject().type();
}

SceneObject * EditorPhotoObject::createSceneObject(EditorDocument * document) const
{	
	/*QString fileName = getOpenFileName(QObject::tr("Insert Image"), QString(), 
		QObject::tr("Images (*.png *.jpg *.jpeg)"));

	if(!fileName.isEmpty())
	{
		PhotoObject * photoObject = new PhotoObject;
		photoObject->setFileName(convertToRelativePath(qStringToStdString(fileName)));
		photoObject->init(renderer());

		int trWidth, trHeight;
		Global::instance().getImageDims(photoObject->getPhotoName(), &trWidth, &trHeight);
		photoObject->setWidth(trWidth);
		photoObject->setHeight(trHeight);
		if (trWidth > 2048 || trHeight > 2048)
		{
			QMessageBox::information(
				0, 
				QObject::tr("Error"), 
				QObject::tr("Image width or height cannot exceed 2048 pixels"));
			delete photoObject;
			return 0;
		}
		return photoObject;
	}else{
		return 0;
	}*/
	PhotoObject * photoObject = new PhotoObject;
	return photoObject;
}

void EditorPhotoObject::drawObject(GfxRenderer * gl, const EditorDocument * document) const
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
		PhotoObject * obj = (PhotoObject *)sceneObject_;
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

PropertyPage * EditorPhotoObject::propertyPage() const
{
	return new EditorPhotoObjectPropertyPage;
}

/////////////////////////////////////////////////////////////////
EditorPhotoObjectPropertyPage::EditorPhotoObjectPropertyPage() :
dimValidator_(0, 2048, 0)
{	
	setGroupBox(false);

	core_ = new CorePropertiesPage;

	setName(tr("Photo"));
	addPropertyPage(core_);
	addSpacing(4);
	startGroup(tr("Photo"));

	fileName_ = new FileNameProperty(
		tr("filename"), tr("Images (*.png *.jpg *.jpeg)"));
	addProperty(fileName_);

	x_ = new EditProperty(tr("x"), &dimValidator_);
	addProperty(x_);
	y_ = new EditProperty(tr("y"), &dimValidator_);
	addProperty(y_);

	width_ = new EditProperty(tr("width"), &dimValidator_);
	addProperty(width_);
	height_ = new EditProperty(tr("height"), &dimValidator_);
	addProperty(height_);
	addSpacing(4);
	rate_ = new ComboBoxProperty(tr("rate"));
	rate_->addItem(tr("4:3"));
	rate_->addItem(tr("3:4"));
	addProperty(rate_);

	endGroup();	
}

EditorPhotoObjectPropertyPage::~EditorPhotoObjectPropertyPage()
{
}

void EditorPhotoObjectPropertyPage::setDocument(EditorDocument * document)
{
	if (document_)
	{
		GLWidget * glWidget = document_->glWidget();

		disconnect(glWidget, 0, this, 0);
		
	}

	PropertyPage::setDocument(document);
}

void EditorPhotoObjectPropertyPage::update()
{
	core_->update();
	VisualAttrib * attr = document_->selectedObject()->visualAttrib();
	Transform transform = attr->transform();

	PhotoObject * obj = (PhotoObject *)document_->selectedObject();
	QString str;

	fileName_->setValue(stdStringToQString(obj->fileName()));

	if (obj->width() > obj->height())
	{
		rate_->setValue(0);
	}
	else
	{
		rate_->setValue(1);

	}

	str.sprintf("%.4f", obj->width());
	width_->setValue(str);
	str.sprintf("%.4f", obj->height());
	height_->setValue(str);
	obj->setX(obj->transform().translation().x);
	str.sprintf("%.4f", obj->x());
	x_->setValue(str);
	obj->setY(obj->transform().translation().y);
	str.sprintf("%.4f", obj->y());
	y_->setValue(str);
}

void EditorPhotoObjectPropertyPage::onChanged(Property * property)
{
	VisualAttrib * attr = document_->selectedObject()->visualAttrib();
	Transform transform = attr->transform();
	PhotoObject * obj = (PhotoObject *)document_->selectedObject();

	if (property == fileName_)
	{
		string fileName = qStringToStdString(fileName_->value());
		document_->doCommand(new ChangePhotoImageFileNameCmd(
			document_, obj, fileName));
	}

	if (property == width_)
	{	
		if (rate_->value() == 0)
		{
			float hei = width_ ->value().toFloat()/4*3;
			Vector2 trans_ = Vector2(width_->value().toFloat(), hei);
		//	document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
		//	obj, width_->value().toFloat(), 
		//	&PhotoObject::width, &PhotoObject::setWidth));
			document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
				obj, trans_, &PhotoObject::widHei, &PhotoObject::setWidHei));
		}
		else
		{
			float hei = width_ ->value().toFloat()/3*4;
			Vector2 trans_ = Vector2(width_->value().toFloat(), hei);
		//	document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
		//	obj, width_->value().toFloat(), 
		//	&PhotoObject::width, &PhotoObject::setWidth));
			document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
				obj, trans_, &PhotoObject::widHei, &PhotoObject::setWidHei));
		}
	}

	if (property == height_)
	{
		if (rate_->value() == 0)
		{
			float wid = height_->value().toFloat()/3*4;
			Vector2 trans_ = Vector2(wid, height_->value().toFloat());
			document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
				obj, trans_, &PhotoObject::widHei, &PhotoObject::setWidHei));
			//document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
			//obj, height_->value().toFloat(), 
			//&PhotoObject::height, &PhotoObject::setHeight));
			//document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
			//	obj, wid, &PhotoObject::width, &PhotoObject::setWidth));
		}
		else
		{
			float wid = height_->value().toFloat()/4*3;
			Vector2 trans_ = Vector2(wid, height_->value().toFloat());
			document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
				obj, trans_, &PhotoObject::widHei, &PhotoObject::setWidHei));
			//document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
			//obj, height_->value().toFloat(), 
			//&PhotoObject::height, &PhotoObject::setHeight));
			//document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
			//	obj, wid, &PhotoObject::width, &PhotoObject::setWidth));

		}
	}

	if (property == x_)
	{
		Vector3 t = transform.translation();
		float val = x_->value().toFloat();
		transform.setTranslation(val, t.y, t.z);
		document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty, 
		attr, transform, &VisualAttrib::transform, &VisualAttrib::setTransform));
	}

	if (property == y_)
	{
		Vector3 t = transform.translation();
		float val = y_->value().toFloat();
		transform.setTranslation(t.x, val, t.z);
		document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty, 
		attr, transform, &VisualAttrib::transform, &VisualAttrib::setTransform));
	}

	if (property == rate_)
	{
		if (rate_->value() == 0)
		{
			float hei = width_->value().toFloat()/4*3;
			document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
				obj, hei, &PhotoObject::height, &PhotoObject::setHeight));
		}
		else
		{
			float hei = width_->value().toFloat()/3*4;
			document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
				obj, hei, &PhotoObject::height, &PhotoObject::setHeight));
		}	
	}
}
