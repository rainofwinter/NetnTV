#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorSubScene.h"
#include "SubScene.h"
#include "GLWidget.h"
#include "Command.h"
#include "Scene.h"
#include "Utils.h"
#include "NewSubSceneDlg.h"
#include "MaskObject.h"

using namespace std;

EditorSubScene::EditorSubScene()
{
}

EditorSubScene::~EditorSubScene()
{
}

SceneObject * EditorSubScene::createSceneObject(EditorDocument * document) const
{
	//Ask user to specify which scene should be contained in the new subscene
	//object

	NewSubSceneDlg dlg(0, document);
	
	if (dlg.exec())
	{
		Subscene * obj = new Subscene;
		Scene * subSceneScene = dlg.scene();	
		Scene * curScene = document->selectedScene();

		if (curScene == subSceneScene || subSceneScene->dependsOn(curScene))
		{
			QMessageBox::information(0, QObject::tr("Error"), 
				QObject::tr("Could not create subscene because doing so would result in a cycle."));

			return 0;			
		}

		if (curScene->userCamera() != NULL)
		{
			QMessageBox::information(0, QObject::tr("Error"), 
				QObject::tr("There can be no subscenes in a scene with a custom camera."));

			return 0;
		}		
		
		obj->setScene(dlg.scene());
		return obj;
	}
	else
		return 0;	
}

boost::uuids::uuid EditorSubScene::sceneObjectType() const
{
	return Subscene().type();
}

PropertyPage * EditorSubScene::propertyPage() const
{
	return new EditorSubScenePropertyPage;
}

void EditorSubScene::drawObject(
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
		gl->setColorProgramColor(0.75f, 0.75f, 0.75f, 0.75f);
		gl->applyCurrentShaderMatrix();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);


		Subscene * obj = (Subscene *)sceneObject_;

		float width = 0, height = 0;
		if (obj->mode() == Subscene::ModeScrollable)
		{
			width = obj->windowWidth();
			height = obj->windowHeight();
		}
		else if (obj->scene())
		{
			width = obj->scene()->screenWidth();
			height = obj->scene()->screenHeight();
		}

		

		float left = 0.0f;
		float up = 0.0f;
		float right = 0.0f;
		float down = 0.0f;

		if (obj->dragVertical() == 1)
		{
			up = -obj->maxMinusDrag();
			down = obj->maxPlusDrag();
		}
		else if (obj->dragVertical() == 0)
		{
			left = -obj->maxMinusDrag();
			right = obj->maxPlusDrag();
		}
		else if (obj->dragVertical() == 2)
		{
			left = -obj->maxMinusDrag();
			right = obj->maxPlusDrag();
			up = -obj->maxTopDrag();
			down = obj->maxBotDrag();
		}

		GLfloat vertices[] = {
			static_cast<GLfloat>(left), static_cast<GLfloat>(up), 0, 
			static_cast<GLfloat>(width + right), static_cast<GLfloat>(up), 0,
			static_cast<GLfloat>(width + right), static_cast<GLfloat>(height + down), 0,
			static_cast<GLfloat>(left), static_cast<GLfloat>(height + down), 0
		};
		

		gl->enableVertexAttribArrayPosition();
		gl->bindArrayBuffer(0);
		gl->vertexAttribPositionPointer(0, (char *)vertices);		

		if (obj->mode() == Subscene::ModeDraggable)
		{			
			glDrawArrays(GL_QUADS, 0, 4);
		}

		vertices[0] = 0;
		vertices[1] = 0;
		vertices[3] = width;
		vertices[4] = 0;
		vertices[6] = width;		
		vertices[7] = height;
		vertices[9] = 0;
		vertices[10] = height;
		
		gl->setColorProgramColor(0, 1, 0, 1);		
		glDrawArrays(GL_QUADS, 0, 4);

		glPolygonOffset(0, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}
}


///////////////////////////////////////////////////////////////////////////////
EditorSubScenePropertyPage::EditorSubScenePropertyPage()
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

	startGroup(tr("Subscene"));
	scene_ = new ComboBoxProperty(tr("current scene"));
	addProperty(scene_);

	transparentBg_ = new BoolProperty(tr("transparent bg"));
	addProperty(transparentBg_);


	cameraMode_ = new ComboBoxProperty(tr("camera mode"));
	cameraMode_->addItem(tr("child scene"));
	cameraMode_->addItem(tr("parent scene"));	
	addProperty(cameraMode_);

	mode_ = new ComboBoxProperty(tr("mode"));
	mode_->addItem(tr("fixed"));
	mode_->addItem(tr("scrollable"));
	mode_->addItem(tr("draggable"));
	addProperty(mode_);

	addSpacing(4);

	windowWidth_ = new EditProperty(tr("window width"), &validator_);
	addProperty(windowWidth_);
	windowHeight_ = new EditProperty(tr("window height"), &validator_);
	addProperty(windowHeight_);

	addSpacing(4);

	dragDirection_ = new ComboBoxProperty(tr("drag direction"));
	dragDirection_->addItem(tr("horizontal"));
	dragDirection_->addItem(tr("vertical"));
	dragDirection_->addItem(tr("flat"));
	addProperty(dragDirection_);
	maxMinusDrag_ = new EditProperty(tr("max - drag"), &validator_);
	addProperty(maxMinusDrag_);
	maxPlusDrag_ = new EditProperty(tr("max + drag"), &validator_);
	addProperty(maxPlusDrag_);
	maxTopDrag_ = new EditProperty(tr("max top drag"), &validator_);
	addProperty(maxTopDrag_);	
	maxBotDrag_ = new EditProperty(tr("max bot drag"), &validator_);
	addProperty(maxBotDrag_);	
	dragSnap_ = new BoolProperty(tr("snap drag"));
	addProperty(dragSnap_);

	addSpacing(4);

	linkWithViewerLevelObjects_ = new BoolProperty(tr("link with viewer-level objects"));
	addProperty(linkWithViewerLevelObjects_, PropertyPage::Vertical);
	
	endGroup();

		
}

void EditorSubScenePropertyPage::enableModeFields()
{
	Subscene * obj = (Subscene *)document_->selectedObject();
	Subscene::Mode mode = obj->mode();

	bool enableWindow = false;
	bool enableDrag = false;

	switch(mode)
	{
	case Subscene::ModeFixed:
		break;
	case Subscene::ModeDraggable:
		enableDrag = true;
		break;
	case Subscene::ModeScrollable:
		enableWindow = true;
		break;	
	}

	windowWidth_->setEnabled(enableWindow);
	windowHeight_->setEnabled(enableWindow);

	dragDirection_->setEnabled(enableDrag);
	maxPlusDrag_->setEnabled(enableDrag);
	maxMinusDrag_->setEnabled(enableDrag);
	dragSnap_->setEnabled(enableDrag);

	bool isflat = obj->dragVertical() == 2;
	maxTopDrag_->setEnabled(isflat);
	maxBotDrag_->setEnabled(isflat);
}

EditorSubScenePropertyPage::~EditorSubScenePropertyPage()
{
}

void EditorSubScenePropertyPage::getScenesList(std::vector<SceneSPtr> * scenes)
{
	scenes->clear();
	Subscene * subScene = (Subscene *)document_->selectedObject();
	
	*scenes = document_->scenes();
	vector<SceneSPtr>::iterator iter = scenes->begin();
	for (; iter != scenes->end(); ++iter)
	{
		if ((*iter).get() == document_->selectedScene())
		{
			scenes->erase(iter);
			break;
		}
	}

	
}

void EditorSubScenePropertyPage::update()
{
	core_->update();
	visual_->update();
	Subscene * subScene = (Subscene *)document_->selectedObject();
	
	vector<SceneSPtr> scenes;
	getScenesList(&scenes);

	for (int i = 0; i < (int)scenes.size(); ++i)
	{
		if (scenes[i].get() == subScene->scene()) scene_->setValue(i);
	}

	QString str;
	str.sprintf("%.4f", subScene->windowWidth());
	windowWidth_->setValue(str);
	str.sprintf("%.4f", subScene->windowHeight());
	windowHeight_->setValue(str);	

	int modeIndex = (int)subScene->mode();
	mode_->setValue(modeIndex);

	modeIndex = (int)subScene->cameraMode();
	cameraMode_->setValue(modeIndex);

	float maxPlusDrag = subScene->maxPlusDrag();
	float maxMinusDrag = subScene->maxMinusDrag();
	float maxTopDrag = subScene->maxTopDrag();
	float maxBotDrag = subScene->maxBotDrag();

	//if (subScene->dragVertical()) dragDirection_->setValue(1);
	//else dragDirection_->setValue(0);
	dragDirection_->setValue(subScene->dragVertical());
	str.sprintf("%.4f", maxPlusDrag);
	maxPlusDrag_->setValue(str);
	str.sprintf("%.4f", maxMinusDrag);
	maxMinusDrag_->setValue(str);
	if (subScene->dragVertical() == 2)
	{
		str.sprintf("%.4f", maxTopDrag);
		maxTopDrag_->setValue(str);
		str.sprintf("%.4f", maxBotDrag);
		maxBotDrag_->setValue(str);
	}
	dragSnap_->setValue(subScene->dragSnap());

	transparentBg_->setValue(subScene->transparentBg());
	linkWithViewerLevelObjects_->setValue(subScene->linkWithViewerLevelObjects());
	
	enableModeFields();
}

void EditorSubScenePropertyPage::onChanged(Property * property)
{	
	Subscene * subScene = (Subscene *)document_->selectedObject();
	if (property == scene_)
	{
		vector<SceneSPtr> scenes;
		getScenesList(&scenes);
		
		int index = scene_->value();
		Scene * selScene = 0;
		if (index >= 0)
			selScene = scenes[index].get();
	
		
		Scene * docScene = subScene->parentScene();
		if (selScene == docScene || selScene->dependsOn(docScene))
		{
			QMessageBox::information(0, tr("Error"), 
				tr("Could not add scene because doing so would result in a cycle."));

			//restore old scene value
			update();			
		}
		else
		{
			document_->doCommand(makeChangePropertyCmd(
				ChangeObjectProperty, subScene, selScene, 
				&Subscene::scene, 
				&Subscene::setScene));
		}
	}

	if (property == dragDirection_)
	{
		int dragVertical = dragDirection_->value();		
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, dragVertical, 
			&Subscene::dragVertical, 
			&Subscene::setDragVertical));
		/*bool dragVertical = (bool)dragDirection_->value();		
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, dragVertical, 
			&Subscene::dragVertical, 
			&Subscene::setDragVertical));*/
	}

	if (property == maxMinusDrag_)
	{
		float val = maxMinusDrag_->value().toFloat();		
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, val, 
			&Subscene::maxMinusDrag, 
			&Subscene::setMaxMinusDrag));
	}

	if (property == maxPlusDrag_)
	{
		float val = maxPlusDrag_->value().toFloat();		
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, val, 
			&Subscene::maxPlusDrag, 
			&Subscene::setMaxPlusDrag));
	}

	if (property == maxTopDrag_)
	{
		float val = maxTopDrag_->value().toFloat();		
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, val, 
			&Subscene::maxTopDrag, 
			&Subscene::setMaxTopDrag));
	}

	if (property == maxBotDrag_)
	{
		float val = maxBotDrag_->value().toFloat();		
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, val, 
			&Subscene::maxBotDrag, 
			&Subscene::setMaxBotDrag));
	}

	if (property == dragSnap_)
	{
		bool val = dragSnap_->value();
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, val, 
			&Subscene::dragSnap, 
			&Subscene::setDragSnap));
	}

	if (property == mode_)
	{
		Subscene::Mode val = (Subscene::Mode)mode_->value();
		document_->doCommand(makeChangeObjectCmd(
			subScene, val, 
			&Subscene::mode, 
			&Subscene::setMode));
	}

	if (property == cameraMode_)
	{
		Subscene::CameraMode val = (Subscene::CameraMode)cameraMode_->value();
		document_->doCommand(makeChangeObjectCmd(
			subScene, val, 
			&Subscene::cameraMode, 
			&Subscene::setCameraMode));
	}

	if (property == windowWidth_)
	{
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, windowWidth_->value().toFloat(),
			&Subscene::windowWidth, 
			&Subscene::setWindowWidth));
	}

	if (property == windowHeight_)
	{
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, windowHeight_->value().toFloat(),
			&Subscene::windowHeight, 
			&Subscene::setWindowHeight));
	}

	if (property == transparentBg_)
	{
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, transparentBg_->value(),
			&Subscene::transparentBg, 
			&Subscene::setTransparentBg));
	}

	if (property == linkWithViewerLevelObjects_)
	{
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, subScene, linkWithViewerLevelObjects_->value(),
			&Subscene::linkWithViewerLevelObjects, 
			&Subscene::setLinkWithViewerLevelObjects));
	}

}

void EditorSubScenePropertyPage::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);

	PropertyPage::setDocument(document);

	if (document_)
	{
		connect(document_, SIGNAL(sceneListChanged()), 
			this, SLOT(onSceneListChanged()));

		onSceneListChanged();	
	}
}

void EditorSubScenePropertyPage::onSceneListChanged()
{
	Subscene * subScene = (Subscene *)document_->selectedObject();
	vector<SceneSPtr> scenes;
	getScenesList(&scenes);

	scene_->clearItems();
	
	bool sceneFound = false;
	for (int i = 0; i < (int)scenes.size(); ++i)
	{
		scene_->addItem(stdStringToQString(scenes[i]->name()));
		if (scenes[i].get() == subScene->scene()) 
		{
			scene_->setValue(i);
			sceneFound = true;
		}
	}
	if (!sceneFound)scene_->setValue(-1);	

}