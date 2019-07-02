#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorModel.h"
#include "ModelFile.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "Texture.h"
#include "FileUtils.h"
#include "Reader.h"
#include "Model.h"
#include "Joint.h"
#include "InspectComponentTool.h"
#include "ModelAnimationPanel.h"
#include "EditorGlobal.h"

using namespace std;
QString EditorModel::lastDir_;

EditorModel::EditorModel()
{
}

EditorModel::~EditorModel()
{
}

SceneObject * EditorModel::createSceneObject(EditorDocument * document) const
{		
	QString fileName = getOpenFileName(QObject::tr("Insert Model"), lastDir_, 
		QObject::tr("ST Model (*.stm)"));


	if (!fileName.isEmpty())
	{
		lastDir_ = QFileInfo(fileName).path();
		ModelFile * model;			
		model = new ModelFile;
		model->setFileName(convertToRelativePath(qStringToStdString(fileName)));

		return model;
	}
	else
		return 0;
}

boost::uuids::uuid EditorModel::sceneObjectType() const
{
	return ModelFile().type();
}

PropertyPage * EditorModel::propertyPage() const
{
	return new EditorModelPropertyPage;
}

void EditorModel::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
	gl->useTextureProgram();
	
	if (/*isThisAndAncestorsVisible() && */sceneObject_->isThisAndAncestorsVisible())
		sceneObject_->drawObject(gl);

	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{		
		
		gl->useColorProgram();
		gl->setColorProgramColor(0, 1, 0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);
		
		ModelFile * obj = (ModelFile *)sceneObject_;
		BoundingBox bbox = obj->extents();
		
		GLfloat vertices[] = {
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z),

			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z),

			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z),

			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z),

			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z),

			bbox.minPt.x, bbox.maxPt.y, bbox.minPt.z, 
			bbox.minPt.x, bbox.maxPt.y, bbox.maxPt.z, 
			bbox.maxPt.x, bbox.maxPt.y, bbox.maxPt.z, 
			bbox.maxPt.x, bbox.maxPt.y, bbox.minPt.z
		};
		
		GLint prevDepthFunc;
		glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);
		glDepthFunc(GL_LEQUAL);
		gl->enableVertexAttribArrayPosition();
		gl->bindArrayBuffer(0);
		gl->vertexAttribPositionPointer(0, (char *)vertices);
		gl->applyCurrentShaderMatrix();
		glDrawArrays(GL_QUADS, 0, 6*4);
		glPolygonOffset(0, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		GLWidget * glWidget = document->glWidget();
		Model * jointModel = glWidget->jointModel();
		Model * boneModel = glWidget->boneModel();

		glDepthFunc(GL_ALWAYS);
		GLboolean prevDepthMask;
		glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);
		glDepthMask(GL_FALSE);
		gl->useColorProgram();
		gl->setColorProgramColor(0.75f, 0.75f, 0.75f, 1.0f);
		
		Model * model = ((ModelFile *)sceneObject_)->model();
		if (!model) return;

		const vector<Joint *> & joints = model->joints();
		
		float jointSize = EditorGlobal::instance().jointDrawSize();

		BOOST_FOREACH(Joint * joint, joints)
		{
			gl->pushMatrix();			
			gl->multMatrix(joint->globalTransform());

			Vector3 s = Transform(gl->modelMatrix()).scaling();

			gl->multMatrix(Matrix::Scale(jointSize/s.x, jointSize/s.y, jointSize/s.z));			
						
			jointModel->drawWireframe(gl);

			gl->popMatrix();

			if (joint->parent() < 0) continue;
			Joint * parent = joints[joint->parent()];

			gl->pushMatrix();
			Vector3 boneOrigin = joint->globalTransform() * Vector3(0.0f, 0.0f, 0.0f);
			Vector3 parentOrigin = parent->globalTransform() * Vector3(0.0f, 0.0f, 0.0f);
			gl->multMatrix(Matrix::Translate(parentOrigin.x, parentOrigin.y, parentOrigin.z));			
			gl->multMatrix(Matrix::Rotate(Vector3(0.0f, 0.0f, 1.0f), boneOrigin - parentOrigin));

			s = Transform(gl->modelMatrix()).scaling();

			gl->multMatrix(Matrix::Scale(jointSize / s.x, jointSize / s.y, (boneOrigin - parentOrigin).magnitude()));

			boneModel->drawWireframe(gl);
			gl->popMatrix();
		}		

		glDepthFunc(prevDepthFunc);
		glDepthMask(prevDepthMask);
	}
}


///////////////////////////////////////////////////////////////////////////////
ModelAnimationPanel * EditorModelPropertyPage::modelAnimationPanel_;


EditorModelPropertyPage::EditorModelPropertyPage()
{
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("Model"));
	/*
	AddLabel(tr("Model"));
	AddSpacing(4);*/
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);
	
	startGroup(tr("Model"));
	
	selectJointButton_ = new QPushButton(tr("Inspect Component"));
	connect(selectJointButton_, SIGNAL(clicked()), this, SLOT(onInspectComponentTool()));
	addWidget(selectJointButton_);
	selectJointButton_->setCheckable(true);

	modelAnimationPanelButton_ = new QPushButton(tr("Model Animation Panel"));
	connect(modelAnimationPanelButton_, SIGNAL(clicked()), this, SLOT(onModelAnimationPanel()));
	addWidget(modelAnimationPanelButton_);
	modelAnimationPanelButton_->setCheckable(true);

	endGroup();

	
}

EditorModelPropertyPage::~EditorModelPropertyPage()
{
}

void EditorModelPropertyPage::onInspectComponentTool()
{
	ModelFile * modelObject = (ModelFile *)document_->selectedObject();
	inspectComponentTool_->setModel(modelObject);

	if (selectJointButton_->isChecked())
	{
		document_->glWidget()->setTool(inspectComponentTool_.get());
	}
	else
	{
		document_->glWidget()->setToolMode(GLWidget::Select);
	}
	
}

void EditorModelPropertyPage::onModelAnimationPanel()
{
	if (modelAnimationPanelButton_->isChecked())
		modelAnimationPanel_->show();
	else
		modelAnimationPanel_->close();
}

void EditorModelPropertyPage::setDocument(EditorDocument * document)
{
	if (document_)
	{
		GLWidget * glWidget = document_->glWidget();
		disconnect(glWidget, 0, this, 0);
	}

	//this function sets document_ field appropriately
	PropertyPage::setDocument(document);
	
	if (!document)
	{

	}

	if (document)
	{
		GLWidget * glWidget = document->glWidget();

		if (!inspectComponentTool_)
			inspectComponentTool_.reset(new InspectComponentTool(glWidget));
		connect(glWidget, SIGNAL(toolChanged()), this, SLOT(onToolChanged()));
		
		if (!modelAnimationPanel_)
		{
			modelAnimationPanel_ = new ModelAnimationPanel(glWidget->mainWindow());
			connect(modelAnimationPanel_, SIGNAL(closed()), this, SLOT(onModelAnimationPanelClosed()));			
		}
	}
	
}

void EditorModelPropertyPage::update()
{
	core_->update();
	visual_->update();
	Model * image = (Model *)document_->selectedObject();

	selectJointButton_->setChecked(inspectComponentTool_.get() == document_->glWidget()->tool());
	modelAnimationPanelButton_->setChecked(modelAnimationPanel_->isVisible());

}

void EditorModelPropertyPage::onChanged(Property * property)
{
	Model * image = (Model *)document_->selectedObject();


	update();
}

void EditorModelPropertyPage::onModelAnimationPanelClosed()
{
	modelAnimationPanelButton_->setChecked(false);
}

void EditorModelPropertyPage::onToolChanged()
{
	selectJointButton_->setChecked(inspectComponentTool_.get() == document_->glWidget()->tool());
}