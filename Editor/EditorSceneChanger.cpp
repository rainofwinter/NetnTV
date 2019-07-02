#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorSceneChanger.h"
#include "GenThumbsDlg.h"
#include "SceneChangerDlg.h"
#include "SceneChanger.h"
#include "GLWidget.h"
#include "Command.h"
#include "Scene.h"
#include "Utils.h"
#include "SceneChangerDlg.h"
#include "MaskObject.h"

using namespace std;

EditorSceneChanger::EditorSceneChanger()
{
}

EditorSceneChanger::~EditorSceneChanger()
{
}

SceneObject * EditorSceneChanger::createSceneObject(
	EditorDocument * document) const
{
	SceneChangerDlg dlg(0, document, 
		document->selectedScene(), 
		vector<Scene *>(), 
		true);

	if (dlg.exec())
	{
		SceneChanger * obj = new SceneChanger;
		vector<Scene *> initScenes = dlg.scenes();
		

		int initWidth = 320, initHeight = 200;

		if (!initScenes.empty())
		{
			initWidth = initScenes.front()->screenWidth();
			initHeight = initScenes.front()->screenHeight();
		}

		obj->setWidth(initWidth);
		obj->setHeight(initHeight);
		obj->setScenes(initScenes);
		obj->setAllowDrag(dlg.allowDragging());
		return obj;
	}
	else
		return 0;

	
}

boost::uuids::uuid EditorSceneChanger::sceneObjectType() const
{
	return SceneChanger().type();
}

PropertyPage * EditorSceneChanger::propertyPage() const
{
	return new EditorSceneChangerPropertyPage;
}

void EditorSceneChanger::drawObject(
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
		SceneChanger * obj = (SceneChanger *)sceneObject_;
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
EditorSceneChangerPropertyPage::EditorSceneChangerPropertyPage()
: validator_(0, 9000, 4, 0)
{	
	setGroupBox(false);

	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage(false);
	
	setName(tr("Rectangle"));
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);

	startGroup(tr("SceneChanger"));

	scenesButton_ = new QPushButton(tr("Scenes..."));
	connect(scenesButton_, SIGNAL(clicked()), this, SLOT(onArticles()));
	addWidget(scenesButton_);

	transparentBg_ = new BoolProperty(tr("transparent bg"));
	transparentBg_->setValue(true);
	addProperty(transparentBg_);
	
	cameraMode_ = new ComboBoxProperty(tr("camera mode"));
	cameraMode_->addItem(tr("child scene"));
	cameraMode_->addItem(tr("parent scene"));	
	addProperty(cameraMode_);

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

	transitionEffect_ = new ComboBoxProperty(tr("transition effect"));
	transitionEffect_->addItem(tr("Sliding"));
	transitionEffect_->addItem(tr("Inverted Page flip"));
	transitionEffect_->addItem(tr("Page flip"));
	addProperty(transitionEffect_);

	scrollDirection_ = new ComboBoxProperty(tr("transition direction"));
	scrollDirection_->addItem(tr("horizontal"));
	scrollDirection_->addItem(tr("vertical"));	
	addProperty(scrollDirection_);
	
	addSpacing(4);

	allowDrag_ = new BoolProperty(tr("allow dragging"));
	addProperty(allowDrag_);
		
	addSpacing(4);

	preloadAdjacent_ = new BoolProperty(tr("preload adjacent scenes"));
	addProperty(preloadAdjacent_, PropertyPage::Vertical);

	addSpacing(4);
	linkWithViewerLevelObjects_ = new BoolProperty(tr("link with viewer-level objects"));
	addProperty(linkWithViewerLevelObjects_, PropertyPage::Vertical);	

}

EditorSceneChangerPropertyPage::~EditorSceneChangerPropertyPage()
{
}

void EditorSceneChangerPropertyPage::onArticles()
{
	SceneChanger * sceneChanger = 
			(SceneChanger *)document_->selectedObject();

	QWidget * dlgParent = topWidget_;
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();
	SceneChangerDlg dlg(dlgParent, document_, 
		sceneChanger->parentScene(), sceneChanger->scenes(), sceneChanger->allowDrag());
	
	if (dlg.exec() == QDialog::Accepted)
	{
		document_->doCommand(new ChangeSceneChangerScenesCmd(
			document_, sceneChanger, dlg.scenes(), dlg.allowDragging()));		
	}
}

void EditorSceneChangerPropertyPage::onShowMarkers(bool val)
{
	activeMarker_->setEnabled(val);
	inactiveMarker_->setEnabled(val);
	markerLocation_->setEnabled(val);
}

void EditorSceneChangerPropertyPage::onAllowDrag(bool val)
{
	//scrollDirection_->setEnabled(val);
}

void EditorSceneChangerPropertyPage::update()
{
	core_->update();
	visual_->update();
	SceneChanger * obj = (SceneChanger *)document_->selectedObject();	

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

	int modeIndex = (int)obj->cameraMode();
	cameraMode_->setValue(modeIndex);

	linkWithViewerLevelObjects_->setValue(obj->linkWithViewerLevelObjects());
	preloadAdjacent_->setValue(obj->preloadAdjacent());

	markerLocation_->setValue((int)obj->markerLocation());
	scrollDirection_->setValue((int)obj->scrollDirection());

	transitionEffect_->setValue((int)obj->transitionMode());
}

void EditorSceneChangerPropertyPage::onChanged(Property * property)
{	
	SceneChanger * subScene = (SceneChanger *)document_->selectedObject();

	if (property == markerLocation_)
	{
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene,
			(SceneChanger::MarkerLocation)markerLocation_->value(),
			&SceneChanger::markerLocation, 
			&SceneChanger::setMarkerLocation));
	}

	if (property == scrollDirection_)
	{
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, 
			(SceneChanger::ScrollDirection)scrollDirection_->value(),
			&SceneChanger::scrollDirection, 
			&SceneChanger::setScrollDirection));
	}
	
	if (property == transitionEffect_)
	{
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, 
			(SceneChanger::TransitionMode)transitionEffect_->value(),
			&SceneChanger::transitionMode, 
			&SceneChanger::setTransitionMode));
	}

	if (property == cameraMode_)
	{
		Subscene::CameraMode val = (Subscene::CameraMode)cameraMode_->value();
		document_->doCommand(makeChangeObjectCmd(
			subScene, val, 
			&SceneChanger::cameraMode, 
			&SceneChanger::setCameraMode));
	}

	if (property == width_)
	{
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, width_->value().toFloat(),
			&SceneChanger::width, 
			&SceneChanger::setWidth));
	}

	if (property == height_)
	{
		document_->doCommand(makeChangeObjectCmd(
			subScene, height_->value().toFloat(),
			&SceneChanger::height, 
			&SceneChanger::setHeight));
	}

	if (property == showMarkers_)
	{
		document_->doCommand(makeChangeObjectCmd(
			subScene, showMarkers_->value(),
			&SceneChanger::showMarkers, 
			&SceneChanger::setShowMarkers));

		onShowMarkers(subScene->showMarkers());
	}

	if (property == activeMarker_)
	{
		document_->doCommand(makeChangeObjectCmd(
			subScene, qStringToStdString(activeMarker_->value()),
			&SceneChanger::activeMarker, 
			&SceneChanger::setActiveMarker));
	}

	if (property == inactiveMarker_)
	{
		document_->doCommand(makeChangeObjectCmd(
			subScene, qStringToStdString(inactiveMarker_->value()),
			&SceneChanger::inactiveMarker, 
			&SceneChanger::setInactiveMarker));

		onAllowDrag(subScene->allowDrag());
	}

	if (property == allowDrag_)
	{
		document_->doCommand(makeChangeObjectCmd(
			subScene, allowDrag_->value(),
			&SceneChanger::allowDrag, 
			&SceneChanger::setAllowDrag));
	}

	if (property == transparentBg_)
	{
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, transparentBg_->value(),
			&SceneChanger::transparentBg, 
			&SceneChanger::setTransparentBg));
	}

	if (property == linkWithViewerLevelObjects_)
	{
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, linkWithViewerLevelObjects_->value(),
			&SceneChanger::linkWithViewerLevelObjects, 
			&SceneChanger::setLinkWithViewerLevelObjects));
	}

	if (property == preloadAdjacent_)
	{		
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, preloadAdjacent_->value(),
			&SceneChanger::preloadAdjacent, 
			&SceneChanger::setPreloadAdjacent));
	}
}

void EditorSceneChangerPropertyPage::setDocument(EditorDocument * document)
{
	PropertyPage::setDocument(document);
}
