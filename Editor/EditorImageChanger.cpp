#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorImageChanger.h"
#include "GenThumbsDlg.h"
#include "ImageGridDlg.h"
#include "ImageChanger.h"
#include "GLWidget.h"
#include "Command.h"
#include "Scene.h"
#include "Utils.h"
#include "MaskObject.h"

using namespace std;

EditorImageChanger::EditorImageChanger()
{
}

EditorImageChanger::~EditorImageChanger()
{
}

SceneObject * EditorImageChanger::createSceneObject(EditorDocument * document) const
{
	ImageChanger * obj = new ImageChanger;
	obj->init(renderer());
	obj->asyncLoadUpdate();
	return obj;
}

boost::uuids::uuid EditorImageChanger::sceneObjectType() const
{
	return ImageChanger().type();
}

PropertyPage * EditorImageChanger::propertyPage() const
{
	return new EditorImageChangerPropertyPage;
}

void EditorImageChanger::drawObject(
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
		ImageChanger * obj = (ImageChanger *)sceneObject_;
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
EditorImageChangerPropertyPage::EditorImageChangerPropertyPage()
: validator_(0, 9000, 4, 0), autoTransitionValidator_(0, 10000000, 3, 0)
{	
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("Rectangle"));
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);

	startGroup(tr("ImageChanger"));

	imagesButton_ = new QPushButton(tr("Images..."));
	connect(imagesButton_, SIGNAL(clicked()), this, SLOT(onArticles()));
	addWidget(imagesButton_);

	width_ = new EditProperty(tr("width"), &validator_);
	addProperty(width_);
	height_ = new EditProperty(tr("height"), &validator_);
	addProperty(height_);
	addSpacing(4);
	startGroup(tr("Markers"));

	showMarkers_ = new BoolProperty(tr("show markers"));
	addProperty(showMarkers_);
	addSpacing(1);
	markerLocation_ = new ComboBoxProperty(tr("marker location"));
	addProperty(markerLocation_);
	markerLocation_->addItem(tr("left"));
	markerLocation_->addItem(tr("top"));
	markerLocation_->addItem(tr("right"));
	markerLocation_->addItem(tr("bottom"));
	addSpacing(4);
	activeMarker_ = new FileNameProperty(
		tr("active"), tr("Images (*.png *.jpg *.jpeg)"));
	addProperty(activeMarker_);
	inactiveMarker_ = new FileNameProperty(
		tr("inactive"), tr("Images (*.png *.jpg *.jpeg)"));
	addProperty(inactiveMarker_);
	endGroup();

	addSpacing(4);
	allowDrag_ = new BoolProperty(tr("allow dragging"));
	addProperty(allowDrag_);
	addSpacing(1);
	scrollDirection_ = new ComboBoxProperty(tr("drag direction"));
	scrollDirection_->addItem(tr("horizontal"));
	scrollDirection_->addItem(tr("vertical"));	
	addProperty(scrollDirection_);
	autoTransitionTime_ = new EditProperty(tr("auto transition time"), &autoTransitionValidator_);
	addProperty(autoTransitionTime_);

}

EditorImageChangerPropertyPage::~EditorImageChangerPropertyPage()
{
}

void EditorImageChangerPropertyPage::onArticles()
{
	ImageChanger * obj = 
			(ImageChanger *)document_->selectedObject();

	QWidget * dlgParent = topWidget_;
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();
	ImageGridDlg dlg(dlgParent, document_->document(), obj->fileNames());
	
	if (dlg.exec() == QDialog::Accepted)
	{
		document_->doCommand(makeChangeObjectCmd(obj, dlg.fileNames(),
			&ImageChanger::fileNames, &ImageChanger::setFileNames));		
	}
}

void EditorImageChangerPropertyPage::onShowMarkers(bool val)
{
	activeMarker_->setEnabled(val);
	inactiveMarker_->setEnabled(val);
	markerLocation_->setEnabled(val);
}

void EditorImageChangerPropertyPage::onAllowDrag(bool val)
{
	scrollDirection_->setEnabled(val);
}

void EditorImageChangerPropertyPage::update()
{
	core_->update();
	visual_->update();
	ImageChanger * obj = (ImageChanger *)document_->selectedObject();	

	QString str;
	str.sprintf("%.4f", obj->width());
	width_->setValue(str);
	str.sprintf("%.4f", obj->height());
	height_->setValue(str);	

	showMarkers_->setValue(obj->showMarkers());
	onShowMarkers(obj->showMarkers());
	activeMarker_->setValue(stdStringToQString(obj->activeMarker()));
	inactiveMarker_->setValue(stdStringToQString(obj->inactiveMarker()));

	bool allowDrag = obj->allowDrag();
	allowDrag_->setValue(allowDrag);
	onAllowDrag(obj->allowDrag());

	markerLocation_->setValue((int)obj->markerLocation());
	scrollDirection_->setValue((int)obj->scrollDirection());

	autoTransitionTime_->setValue(QString::number(obj->autoTransitionTime(), 'g', 3));
}

void EditorImageChangerPropertyPage::onChanged(Property * property)
{	
	ImageChanger * subScene = (ImageChanger *)document_->selectedObject();

	if (property == markerLocation_)
	{
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene,
			(ImageChanger::MarkerLocation)markerLocation_->value(),
			&ImageChanger::markerLocation, 
			&ImageChanger::setMarkerLocation));
	}

	if (property == scrollDirection_)
	{
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, 
			(ImageChanger::ScrollDirection)scrollDirection_->value(),
			&ImageChanger::scrollDirection, 
			&ImageChanger::setScrollDirection));
	}

	if (property == width_)
	{
		document_->doCommand(makeChangeObjectCmd(
			subScene, width_->value().toFloat(),
			&ImageChanger::width, 
			&ImageChanger::setWidth));
	}

	if (property == height_)
	{
		document_->doCommand(makeChangeObjectCmd(
			subScene, height_->value().toFloat(),
			&ImageChanger::height, 
			&ImageChanger::setHeight));
	}

	if (property == showMarkers_)
	{
		document_->doCommand(makeChangeObjectCmd(
			subScene, showMarkers_->value(),
			&ImageChanger::showMarkers, 
			&ImageChanger::setShowMarkers));

		onShowMarkers(subScene->showMarkers());
	}

	if (property == activeMarker_)
	{
		document_->doCommand(makeChangeObjectCmd(
			subScene, qStringToStdString(activeMarker_->value()),
			&ImageChanger::activeMarker, 
			&ImageChanger::setActiveMarker));
	}

	if (property == inactiveMarker_)
	{
		document_->doCommand(makeChangeObjectCmd(
			subScene, qStringToStdString(inactiveMarker_->value()),
			&ImageChanger::inactiveMarker, 
			&ImageChanger::setInactiveMarker));

	}

	if (property == allowDrag_)
	{
		document_->doCommand(makeChangeObjectCmd(
			subScene, allowDrag_->value(),
			&ImageChanger::allowDrag, 
			&ImageChanger::setAllowDrag));

		onAllowDrag(subScene->allowDrag());
	}

	if (property == autoTransitionTime_)
	{
		float val = autoTransitionTime_->value().toFloat();
		document_->doCommand(makeChangeObjectCmd(
			subScene, val,
			&ImageChanger::autoTransitionTime, 
			&ImageChanger::setAutoTransitionTime));

		onAllowDrag(subScene->allowDrag());
	}
}

void EditorImageChangerPropertyPage::setDocument(EditorDocument * document)
{
	PropertyPage::setDocument(document);
}
