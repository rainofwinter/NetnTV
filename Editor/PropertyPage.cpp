#include "stdafx.h"
#include "PropertyPage.h"
#include "EditorDocument.h"
#include "EditorObject.h"
#include "SceneObject.h"
#include "AppObject.h"
#include "Transform.h"
#include "Attrib.h"
#include "Command.h"
#include "Utils.h"
#include "Document.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

PropertyPage::PropertyPage()
{
	document_ = 0;
	topWidget_ = 0;
	margins_ = QMargins(0, 0, 0, 0);
	setGroupBox(true);
}

PropertyPage::~PropertyPage()
{
	for (int i = 0; i < (int)properties_.size(); ++i)
	{
		delete properties_[i];
	}
	delete topWidget_;
}

void PropertyPage::setName(const QString & name)
{
	Property::setName(name);
	if (groupBox_) groupBox_->setTitle(name);
}

void PropertyPage::setGroupBox(bool isGroupBox)
{
	delete topWidget_;
	layout_ = new QGridLayout;	
	layout_->setMargin(5);
	layout_->setSpacing(0);	
	layout_->setHorizontalSpacing(5);
	layouts_.push(layout_);

	if (isGroupBox)
	{
		groupBox_ = new QGroupBox(name_);
		topWidget_ = groupBox_;		
	}
	else
	{
		groupBox_ = 0;
		topWidget_ = new QWidget();		
		layout_->setMargin(0);
	}	
		
	topWidget_->setContentsMargins(margins_);
	topWidget_->setLayout(layout_);
}

void PropertyPage::setMargins(int l, int t, int r, int b)
{
	margins_ = QMargins(l, t, r, b);
	topWidget_->setContentsMargins(l, t, r, b);
}

void PropertyPage::setDocument(EditorDocument * document)
{
	document_ = document;
	foreach(Property * property, properties_)
		property->setDocument(document);
}

void PropertyPage::setView(GLWidget * widget)
{
	view_ = widget;
	foreach(Property * property, properties_)
		property->setView(widget);
}

QGroupBox * PropertyPage::startGroup(const QString & groupTitle)
{
	QGridLayout * groupLayout = new QGridLayout;	
	groupLayout->setMargin(5);
	groupLayout->setSpacing(0);	
	groupLayout->setHorizontalSpacing(5);

	QGroupBox * groupBox = new QGroupBox(groupTitle);
	layout_->addWidget(groupBox, layout_->rowCount(), 0, 1, 2);
	groupBox->setLayout(groupLayout);

	layouts_.push(groupLayout);
	layout_ = groupLayout;

	return groupBox;
}

void PropertyPage::endGroup()
{
	layouts_.pop();
	layout_ = layouts_.top();

}

void PropertyPage::addWidget(QWidget * widget)
{
	layout_->addWidget(widget, layout_->rowCount(), 0, 1, 2);
}

void PropertyPage::addLabel(const QString & labelStr)
{
	QLabel * label = new QLabel(labelStr, topWidget_);
	label->setAlignment(Qt::AlignCenter);
	layout_->addWidget(label, layout_->rowCount(), 0, 1, 2);
}

void PropertyPage::addProperty(Property * property, Arrangement arrangement)
{
	properties_.push_back(property);
	QLabel * label = new QLabel(property->name(), topWidget_);

	if (arrangement == Horizontal)
	{	
		layout_->addWidget(label, layout_->rowCount(), 0);	
		layout_->addWidget(property->widget(), layout_->rowCount() - 1, 1);
	}
	else
	{
		label->setMinimumHeight(label->sizeHint().height() + 2);
		layout_->addWidget(label, layout_->rowCount(), 0, 1, 2);			
		layout_->addWidget(property->widget(), layout_->rowCount(), 0, 1, 2);
	}

	label->setBuddy(property->widget());

	connect(property, SIGNAL(changed(Property *)), 
		this, SLOT(onChanged(Property * )));
}

void PropertyPage::addPropertyPage(PropertyPage * page)
{
	properties_.push_back(page);
	layout_->addWidget(page->widget(), layout_->rowCount(), 0, 1, 2);
}

void PropertyPage::addSpacing(int size)
{
	layout_->addItem(new QSpacerItem(0, size), layout_->rowCount(),  2, 1);
}

void PropertyPage::addSpring()
{
	layout_->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), layout_->rowCount(),  2, 1);
}

///////////////////////////////////////////////////////////////////////////////
CorePropertiesPage::CorePropertiesPage()
{
	setName(tr("Scene Object"));

	id_ = new EditProperty(tr("id"));
	addProperty(id_);
}

void CorePropertiesPage::update()
{
	SceneObject * obj = document_->selectedObject();
	
	id_->setValue(stdStringToQString(obj->id()));
}

void CorePropertiesPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	SceneObject * obj = document_->selectedObject();	
	if (property == id_)
	{
		string id = qStringToStdString(id_->value());
		document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,  
			obj, id, &SceneObject::id, &SceneObject::setId));
	}

	update();
}
///////////////////////////////////////////////////////////////////////////////
CoreAppObjectPropertiesPage::CoreAppObjectPropertiesPage()
{
	setName(tr("App Object"));

	id_ = new EditProperty(tr("id"));
	addProperty(id_);
}

void CoreAppObjectPropertiesPage::update()
{
	AppObject * obj = document_->selectedAppObject();
	
	id_->setValue(stdStringToQString(obj->id()));
}

void CoreAppObjectPropertiesPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	AppObject * obj = document_->selectedAppObject();	
	if (property == id_)
	{
		string id = qStringToStdString(id_->value());
		document_->doCommand(makeChangePropertyCmd(ChangeAppObjectProperty,  
			obj, id, &AppObject::id, &AppObject::setId));
	}

	update();
}
///////////////////////////////////////////////////////////////////////////////

ScalePropertiesPage::ScalePropertiesPage()
{
	setName(tr("Scale"));
	scaleX_ = new EditProperty(tr("scale x"), &doubleValidator_);
	scaleY_ = new EditProperty(tr("scale y"), &doubleValidator_);
	scaleZ_ = new EditProperty(tr("scale z"), &doubleValidator_);

	mode_ = new ComboBoxProperty(tr("mode"));
	mode_->addItem(tr("Proportion"));
	mode_->addItem(tr("Size"));

	addProperty(mode_);
	addProperty(scaleX_);
	addProperty(scaleY_);
	addProperty(scaleZ_);
}

void ScalePropertiesPage::update()
{
	SceneObject * object = document_->selectedObject();
	VisualAttrib * attr = document_->selectedObject()->visualAttrib();
	
	const Transform & transform = attr->transform();
	QString str;
	
	if (mode_->value() == Proportion)
	{

		str.sprintf("%.4f", transform.scaling().x);
		scaleX_->setValue(str);
		str.sprintf("%.4f", transform.scaling().y);
		scaleY_->setValue(str);
		str.sprintf("%.4f", transform.scaling().z);
		scaleZ_->setValue(str);
	}
	else
	{
		BoundingBox extents = object->extents();
		extents = extents.scale(transform);

		str.sprintf("%.4f", extents.lengthX());
		scaleX_->setValue(str);
		str.sprintf("%.4f", extents.lengthY());
		scaleY_->setValue(str);
		str.sprintf("%.4f", extents.lengthZ());
		scaleZ_->setValue(str);
	}
	
}

void ScalePropertiesPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	VisualAttrib * attr = document_->selectedObject()->visualAttrib();

	Transform transform = attr->transform();
	Vector3 t = transform.translation();
	Vector3 r = transform.rotation();
	Vector3 s = transform.scaling();
	Vector3 p = transform.pivot();
	
	if (mode_->value() == Proportion)
	{
		if (property == scaleX_)
			transform.setScaling(scaleX_->value().toFloat(), s.y, s.z);

		if (property == scaleY_)
			transform.setScaling(s.x, scaleY_->value().toFloat(), s.z);

		if (property == scaleZ_)
			transform.setScaling(s.x, s.y, scaleZ_->value().toFloat());	
	}
	else
	{
		SceneObject * object = document_->selectedObject();
		BoundingBox extents = object->extents();

		if (property == scaleX_ && extents.lengthX())
			transform.setScaling(
				scaleX_->value().toFloat()/extents.lengthX(), s.y, s.z);

		if (property == scaleY_ && extents.lengthY())
			transform.setScaling(
				s.x, scaleY_->value().toFloat()/extents.lengthY(), s.z);

		if (property == scaleZ_ && extents.lengthZ())
			transform.setScaling(
			s.x, s.y, scaleZ_->value().toFloat()/extents.lengthZ());
	}

	if (transform != attr->transform())
	{
		document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty, 
			attr, transform, 
			&VisualAttrib::transform, &VisualAttrib::setTransform));
	}

	update();
}
///////////////////////////////////////////////////////////////////////////////

ScaleAppObjectPropertiesPage::ScaleAppObjectPropertiesPage()
{
	setName(tr("Scale"));
	scaleX_ = new EditProperty(tr("scale x"), &doubleValidator_);
	scaleY_ = new EditProperty(tr("scale y"), &doubleValidator_);

	mode_ = new ComboBoxProperty(tr("mode"));
	mode_->addItem(tr("Proportion"));
	mode_->addItem(tr("Size"));

	addProperty(mode_);
	addProperty(scaleX_);
	addProperty(scaleY_);


}

void ScaleAppObjectPropertiesPage::update()
{
	AppObject * object = document_->selectedAppObject();
	VisualAttrib * attr = document_->selectedAppObject()->visualAttrib();

	const Transform & transform = attr->transform();
	QString str;

	if (mode_->value() == Proportion)
	{

		str.sprintf("%.4f", transform.scaling().x);
		scaleX_->setValue(str);
		str.sprintf("%.4f", transform.scaling().y);
		scaleY_->setValue(str);
	}
	else
	{
		BoundingBox extents = object->extents();
		extents = extents.scale(transform);

		str.sprintf("%.4f", extents.lengthX());
		scaleX_->setValue(str);
		str.sprintf("%.4f", extents.lengthY());
		scaleY_->setValue(str);
	}
}

void ScaleAppObjectPropertiesPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	VisualAttrib * attr = document_->selectedAppObject()->visualAttrib();

	Transform transform = attr->transform();
	Vector3 t = transform.translation();
	Vector3 r = transform.rotation();
	Vector3 s = transform.scaling();
	Vector3 p = transform.pivot();

	if (mode_->value() == Proportion)
	{
		if (property == scaleX_)
			transform.setScaling(scaleX_->value().toFloat(), s.y, s.z);

		if (property == scaleY_)
			transform.setScaling(s.x, scaleY_->value().toFloat(), s.z);
	}
	else
	{
		AppObject * object = document_->selectedAppObject();
		BoundingBox extents = object->extents();

		if (property == scaleX_ && extents.lengthX())
			transform.setScaling(
				scaleX_->value().toFloat()/extents.lengthX(), s.y, s.z);

		if (property == scaleY_ && extents.lengthY())
			transform.setScaling(
				s.x, scaleY_->value().toFloat()/extents.lengthY(), s.z);
	}

	if (transform != attr->transform())
	{
		document_->doCommand(makeChangePropertyCmd(ChangeAppObjectProperty, 
			attr, transform, 
			&VisualAttrib::transform, &VisualAttrib::setTransform));
	}

	update();
}

///////////////////////////////////////////////////////////////////////////////
TransformPropertiesPage::TransformPropertiesPage()
{
	setName(tr("Transform"));

	//doubleValidator_.setNotation(QDoubleValidator::StandardNotation);

	transX_ = new EditProperty(tr("translate x"), &doubleValidator_);
	transY_ = new EditProperty(tr("translate y"), &doubleValidator_);
	transZ_ = new EditProperty(tr("translate z"), &doubleValidator_);
	addProperty(transX_);
	addProperty(transY_);
	addProperty(transZ_);

	addSpacing(4);

	rotX_ = new EditProperty(tr("rotate x"), &doubleValidator_);
	rotY_ = new EditProperty(tr("rotate y"), &doubleValidator_);
	rotZ_ = new EditProperty(tr("rotate z"), &doubleValidator_);
	addProperty(rotX_);
	addProperty(rotY_);
	addProperty(rotZ_);

	addSpacing(4);

	scale_ = new ScalePropertiesPage();
	addPropertyPage(scale_);

	addSpacing(4);
	
	pivotX_= new EditProperty(tr("pivot x"), &doubleValidator_);
	pivotY_ = new EditProperty(tr("pivot y"), &doubleValidator_);
	pivotZ_ = new EditProperty(tr("pivot z"), &doubleValidator_);
	addProperty(pivotX_);
	addProperty(pivotY_);
	addProperty(pivotZ_);

	

}

void TransformPropertiesPage::update()
{
	VisualAttrib * attr = document_->selectedObject()->visualAttrib();
	
	const Transform & transform = attr->transform();
	QString str;

	str.sprintf("%.4f", transform.translation().x);
	transX_->setValue(str);
	str.sprintf("%.4f", transform.translation().y);
	transY_->setValue(str);
	str.sprintf("%.4f", transform.translation().z);
	transZ_->setValue(str);

	str.sprintf("%.4f", RadToDeg(transform.rotation().x));
	rotX_->setValue(str);
	str.sprintf("%.4f", RadToDeg(transform.rotation().y));
	rotY_->setValue(str);
	str.sprintf("%.4f", RadToDeg(transform.rotation().z));
	rotZ_->setValue(str);

	scale_->update();

	str.sprintf("%.4f", transform.pivot().x);
	pivotX_->setValue(str);
	str.sprintf("%.4f", transform.pivot().y);
	pivotY_->setValue(str);
	str.sprintf("%.4f", transform.pivot().z);
	pivotZ_->setValue(str);
}

void TransformPropertiesPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	VisualAttrib * attr = document_->selectedObject()->visualAttrib();

	Transform transform = attr->transform();
	Vector3 t = transform.translation();
	Vector3 r = transform.rotation();
	Vector3 s = transform.scaling();
	Vector3 p = transform.pivot();

	if (property == transX_)
		transform.setTranslation(transX_->value().toFloat(), t.y, t.z);

	if (property == transY_)
		transform.setTranslation(t.x, transY_->value().toFloat(), t.z);

	if (property == transZ_)
		transform.setTranslation(t.x, t.y, transZ_->value().toFloat());

	if (property == rotX_)
		transform.setRotation(DegToRad(rotX_->value().toFloat()), r.y, r.z);

	if (property == rotY_)
		transform.setRotation(r.x, DegToRad(rotY_->value().toFloat()), r.z);

	if (property == rotZ_)
		transform.setRotation(r.x, r.y, DegToRad(rotZ_->value().toFloat()));


	if (property == pivotX_)
		transform.setPivot(pivotX_->value().toFloat(), p.y, p.z);

	if (property == pivotY_)
		transform.setPivot(p.x, pivotY_->value().toFloat(), p.z);

	if (property == pivotZ_)
		transform.setPivot(p.x, p.y, pivotZ_->value().toFloat());

	document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty, 
		attr, transform, &VisualAttrib::transform, &VisualAttrib::setTransform));

	update();
}
///////////////////////////////////////////////////////////////////////////////
TransformAppObjectPropertiesPage::TransformAppObjectPropertiesPage()
{
	setName(tr("Transform"));

	//doubleValidator_.setNotation(QDoubleValidator::StandardNotation);

	transX_ = new EditProperty(tr("translate x"), &doubleValidator_);
	transY_ = new EditProperty(tr("translate y"), &doubleValidator_);
	addProperty(transX_);
	addProperty(transY_);

	addSpacing(4);
	/*
	rotation_ = new EditProperty(tr("rotate"), &doubleValidator_);
	addProperty(rotation_);
	addSpacing(4);
	*/

	scale_ = new ScaleAppObjectPropertiesPage();
	addPropertyPage(scale_);

	addSpacing(4);
	
	pivotX_= new EditProperty(tr("pivot x"), &doubleValidator_);
	pivotY_ = new EditProperty(tr("pivot y"), &doubleValidator_);
	addProperty(pivotX_);
	addProperty(pivotY_);

	

}

void TransformAppObjectPropertiesPage::update()
{
	VisualAttrib * attr = document_->selectedAppObject()->visualAttrib();
	
	const Transform & transform = attr->transform();
	QString str;

	str.sprintf("%.4f", transform.translation().x);
	transX_->setValue(str);
	str.sprintf("%.4f", transform.translation().y);
	transY_->setValue(str);

	/*
	str.sprintf("%.4f", RadToDeg(transform.rotation().z));
	rotation_->setValue(str);
	*/

	scale_->update();

	str.sprintf("%.4f", transform.pivot().x);
	pivotX_->setValue(str);
	str.sprintf("%.4f", transform.pivot().y);
	pivotY_->setValue(str);
}

void TransformAppObjectPropertiesPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	VisualAttrib * attr = document_->selectedAppObject()->visualAttrib();

	Transform transform = attr->transform();
	Vector3 t = transform.translation();
	Vector3 r = transform.rotation();
	Vector3 s = transform.scaling();
	Vector3 p = transform.pivot();

	if (property == transX_)
		transform.setTranslation(transX_->value().toFloat(), t.y, t.z);

	if (property == transY_)
		transform.setTranslation(t.x, transY_->value().toFloat(), t.z);
	/*
	if (property == rotation_)
		transform.setRotation(r.x, r.y, DegToRad(rotation_->value().toFloat()));
	*/
	if (property == pivotX_)
		transform.setPivot(pivotX_->value().toFloat(), p.y, p.z);

	if (property == pivotY_)
		transform.setPivot(p.x, pivotY_->value().toFloat(), p.z);

	document_->doCommand(makeChangePropertyCmd(ChangeAppObjectProperty, 
		attr, transform, &VisualAttrib::transform, &VisualAttrib::setTransform));

	update();
}
///////////////////////////////////////////////////////////////////////////////
VisualPropertiesPage::VisualPropertiesPage(bool hasOpacity)
{
	setName(tr("Visual Attributes"));

	setGroupBox(false);
	
	transform_ = new TransformPropertiesPage;
	opacityValidator_ = new UnitDoubleValidator();	
	visible_ = new BoolProperty(tr("visible"));
	addPropertyPage(transform_);
	addSpacing(4);

	if (hasOpacity) 
	{
		opacity_ = new EditProperty(tr("opacity"), opacityValidator_);	
		addProperty(opacity_);
	}
	else opacity_ = 0;

	addProperty(visible_);
}

VisualPropertiesPage::~VisualPropertiesPage()
{
	delete opacityValidator_;
}

void VisualPropertiesPage::update()
{
	transform_->update();

	VisualAttrib * attr = document_->selectedObject()->visualAttrib();

	if (opacity_)
	{
		QString str;
		str.sprintf("%.4f", attr->opacity());
		opacity_->setValue(str);
	}

	visible_->setValue(attr->isVisible());
}


void VisualPropertiesPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	VisualAttrib * attr = document_->selectedObject()->visualAttrib();

	if (opacity_ && property == opacity_)
	{
		float opacity = opacity_->value().toFloat();
		document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
			attr, opacity, &VisualAttrib::opacity, &VisualAttrib::setOpacity));		
	}

	if (property == visible_)
	{
		bool visible = visible_->value();
		document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
			attr, visible, &VisualAttrib::isVisible, &VisualAttrib::setVisible));		
	}

	update();
}


///////////////////////////////////////////////////////////////////////////////
VisualAppObjectPropertiesPage::VisualAppObjectPropertiesPage() :
posValidator_(-2048, 2048, 4, 0), dimValidator_(0, 2048, 4, 0)
{
	setName(tr("Visual Attributes"));

	setGroupBox(false);
	
	x_ = new EditProperty(tr("x"), &posValidator_);
	addProperty(x_);
	y_ = new EditProperty(tr("y"), &posValidator_);
	addProperty(y_);
	width_ = new EditProperty(tr("width"), &dimValidator_);
	addProperty(width_);
	height_ = new EditProperty(tr("height"), &dimValidator_);
	addProperty(height_);
	addSpacing(4);

	opacityValidator_ = new UnitDoubleValidator();	
	opacity_ = new EditProperty(tr("opacity"), opacityValidator_);	
	visible_ = new BoolProperty(tr("visible"));
	addProperty(opacity_);
	addProperty(visible_);

}

VisualAppObjectPropertiesPage::~VisualAppObjectPropertiesPage()
{
	delete opacityValidator_;
}

void VisualAppObjectPropertiesPage::update()
{
	AppObject * obj = document_->selectedAppObject();

	QString str;
	str.sprintf("%.4f", obj->opacity());
	opacity_->setValue(str);
	visible_->setValue(obj->visible());

	int x = obj->x();
	int y = obj->y();
	int width = obj->width();
	int height = obj->height();

	str.sprintf("%d", obj->x());
	x_->setValue(str);

	str.sprintf("%d", obj->y());
	y_->setValue(str);

	str.sprintf("%d", obj->width());
	width_->setValue(str);

	str.sprintf("%d", obj->height());
	height_->setValue(str);
}


void VisualAppObjectPropertiesPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	VisualAttrib * attr = document_->selectedAppObject()->visualAttrib();
	AppObject * appObj = document_->selectedAppObject();

	if (property == opacity_)
	{
		float opacity = opacity_->value().toFloat();
		document_->doCommand(makeChangePropertyCmd(ChangeAppObjectProperty,
			appObj, appObj->opacity(), opacity, &AppObject::opacity, &AppObject::setOpacity));
	}

	if (property == visible_)
	{
		bool visible = visible_->value();
		document_->doCommand(makeChangePropertyCmd(ChangeAppObjectProperty,
			appObj, appObj->visible(), visible, &AppObject::visible, &AppObject::setVisible));
	}

	if (property == x_)
	{
		int x = x_->value().toInt();		
		document_->doCommand(makeChangePropertyCmd(ChangeAppObjectProperty,
			appObj, x, &AppObject::x, &AppObject::setX));
	}

	if (property == y_)
	{
		int y = y_->value().toInt();
		document_->doCommand(makeChangePropertyCmd(ChangeAppObjectProperty,
			appObj, y, &AppObject::y, &AppObject::setY));
	}

	if (property == width_)
	{
		int width = width_->value().toInt();
		document_->doCommand(makeChangePropertyCmd(ChangeAppObjectProperty,
			appObj, width, &AppObject::width, &AppObject::setWidth));
	}

	if (property == height_)
	{
		int height = height_->value().toInt();
		document_->doCommand(makeChangePropertyCmd(ChangeAppObjectProperty,
			appObj, height, &AppObject::height, &AppObject::setHeight));
	}


	update();
}
