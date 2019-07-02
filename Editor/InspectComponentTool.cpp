#include "stdafx.h"
#include "InspectComponentTool.h"
#include "GLWidget.h"
#include "Attrib.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "EditorDocument.h"
#include "EditorObject.h"
#include "Command.h"
#include "Scene.h"
#include "Joint.h"
#include "ModelFile.h"
#include "Model.h"
#include "MainWindow.h"
#include "Utils.h"
#include "EditorGlobal.h"
#include "EditorScene.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

InspectComponentWindow::InspectComponentWindow(MainWindow * mainWindow) : QWidget(mainWindow, Qt::Window)
{
	ui.setupUi(this);

	connect(ui.doneButton, SIGNAL(clicked()), this, SLOT(onDone()));
}

void InspectComponentWindow::onDone()
{
	close();
}

void InspectComponentWindow::closeEvent(QCloseEvent * evt)
{
	QWidget::closeEvent(evt);
	emit closed();	
}

void InspectComponentWindow::setText(const QString & text)
{
	ui.componentText->setPlainText(text);
}

///////////////////////////////////////////////////////////////////////////////

InspectComponentTool::InspectComponentTool(GLWidget * gl) : Tool(gl)
{	
	disableWindowClosedHandling_ = false;
	inspectWnd_ = new InspectComponentWindow(gl->mainWindow());
	connect(inspectWnd_, SIGNAL(closed()), this, SLOT(onWindowClosed()));
	connect(document_, SIGNAL(objectSelectionChanged()), this, SLOT(onLostObjSelection()));
	connect(document_, SIGNAL(objectListChanged()), this, SLOT(onLostObjSelection()));
	selJoint_ = -1;
	selMesh_ = -1;

}

InspectComponentTool::~InspectComponentTool()
{
}

void InspectComponentTool::endTool()
{
	modelObj_ = NULL;
	glWidget_->setToolMode(GLWidget::Select);
}

void InspectComponentTool::onLostObjSelection()
{
	if (modelObj_ != document_->selectedObject())
	{
		endTool();
	}
}

void InspectComponentTool::onWindowClosed()
{
	if (disableWindowClosedHandling_) return;
	endTool();
}

void InspectComponentTool::setModel(ModelFile * modelObj)
{
	modelObj_ = modelObj;
	model_ = modelObj->model();
	syncTextToSelection();
}

void InspectComponentTool::init()
{
}

void InspectComponentTool::uninit()
{
}


void InspectComponentTool::mousePressEvent(QMouseEvent * event)
{		
	Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));

	Ray ray = glWidget_->renderer()->unproject(mousePosDevice);

	const vector<Joint *> & joints = model_->joints();

	Matrix globalObjTrans = 
		modelObj_->parentTransform() * modelObj_->visualAttrib()->transformMatrix();

	Model * jointModel = glWidget_->jointModel();
	Model * boneModel = glWidget_->boneModel();


	const Vector3 & camEye = glWidget_->camera()->eye();

	float intDist = FLT_MAX;
	int intJoint = -1;
	int intMesh = -1;

	float jointSize = EditorGlobal::instance().jointDrawSize();
	Matrix m;

	for (int i = 0; i < (int)joints.size(); ++i)
	{
		Joint * joint = joints[i];
		Vector3 intersectPt;
		bool intersected;

		m = globalObjTrans * joint->globalTransform();
		Vector3 s = Transform(m).scaling();

		intersected = jointModel->intersect(&intersectPt, ray, 
			m * Matrix::Scale(jointSize/s.x, jointSize/s.y, jointSize/s.z));

		if (intersected)
		{
			float dist = (intersectPt - camEye).magnitude();
			if (dist < intDist)
			{
				intDist = dist;
				intJoint = i;
			}
		}

		if (joint->parent() < 0) continue;
		Joint * parent = joints[joint->parent()];

		Vector3 boneOrigin = joint->globalTransform() * Vector3(0.0f, 0.0f, 0.0f);
		Vector3 parentOrigin = parent->globalTransform() * Vector3(0.0f, 0.0f, 0.0f);
		
		m = globalObjTrans * 
			Matrix::Translate(parentOrigin.x, parentOrigin.y, parentOrigin.z) * 
			Matrix::Rotate(Vector3(0.0f, 0.0f, 1.0f), boneOrigin - parentOrigin);
		s = Transform(m).scaling();
		
		intersected = boneModel->intersect(&intersectPt, ray, 
			m *
			Matrix::Scale(jointSize / s.x, jointSize / s.y, (boneOrigin - parentOrigin).magnitude()));

		if (intersected)
		{
			float dist = (intersectPt - camEye).magnitude();
			if (dist < intDist)
			{
				intDist = dist;
				intJoint = joint->parent();
			}
		}
	}


	if (intJoint == -1)
	{
		intDist = FLT_MAX;
		const std::vector<Mesh *> & meshes = model_->meshes();
		for (int i = 0; i < (int)meshes.size(); ++i)
		{
			Mesh * mesh = meshes[i];
			Vector3 intersectPt;
			bool intersected;

			Matrix invMatrix = (globalObjTrans * mesh->transform()).inverse();

			
			Ray invRay;
			Vector3 b = invMatrix * (ray.origin + ray.dir);
			invRay.origin = invMatrix * ray.origin;
			invRay.dir = b - invRay.origin;

			intersected = mesh->intersect(&intersectPt, invRay);
			
			if (intersected)
			{
				Vector3 gIntPt = globalObjTrans * mesh->transform() * intersectPt;
				float dist = (gIntPt - camEye).magnitude();
				if (dist < intDist)
				{
					intDist = dist;
					intMesh = i;
				}
			}
			
		}
	}

	selJoint_ = intJoint;
	selMesh_ = intMesh;

	syncTextToSelection();
}

void InspectComponentTool::syncTextToSelection()
{
	QString str;

	if (selJoint_ >= 0)
	{
		QString line;
		str += "Joint\n";
		
		const vector<Joint *> & joints = model_->joints();
		line.sprintf("Index: %d\n", selJoint_);		
		str += line;

		str += "Name: " + stdStringToQString(joints[selJoint_]->name()) + "\n";
	}
	else if (selMesh_ >= 0)
	{
		QString line;
		str += "Mesh\n";

		const vector<Mesh *> & meshes = model_->meshes();
		line.sprintf("Index: %d\n", selMesh_);
		str += line;

		str += "Name: " + stdStringToQString(meshes[selMesh_]->id()) + "\n";
	}
	else
	{
		str = "No component selected";	
	}

	inspectWnd_->setText(str);
	
}


void InspectComponentTool::mouseReleaseEvent(QMouseEvent * event)
{	
}

void InspectComponentTool::mouseMoveEvent(QMouseEvent * event)
{
}

void InspectComponentTool::setActive(bool val)
{
	Tool::setActive(val);

	selJoint_ = -1;
	selMesh_ = -1;
	
	if (val)	
		inspectWnd_->show();
	else
	{
		disableWindowClosedHandling_ = true;
		inspectWnd_->close();
		disableWindowClosedHandling_ = false;
	}
	
}

void InspectComponentTool::draw()
{
	
	Matrix globalObjTrans = 
		modelObj_->parentTransform() * modelObj_->visualAttrib()->transformMatrix();


	GfxRenderer * gl = gl_;

	gl->pushMatrix();
	gl->loadMatrix(globalObjTrans);

	GLint prevDepthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);
	
	gl_->useColorProgram();
	gl_->setColorProgramColor(0.0f, 1.0f, 0.0f, 1.0f);

	float jointSize = EditorGlobal::instance().jointDrawSize();

	if (selJoint_ >= 0)
	{		
		glDepthFunc(GL_ALWAYS);
		const vector<Joint *> & joints = model_->joints();
		Joint * joint = joints[selJoint_];
		Model * jointModel = glWidget_->jointModel();
		Model * boneModel = glWidget_->boneModel();

		gl->pushMatrix();			
		gl->multMatrix(joint->globalTransform());

		Vector3 s = Transform(gl->modelMatrix()).scaling();

		gl->multMatrix(Matrix::Scale(jointSize/s.x, jointSize/s.y, jointSize/s.z));			
					
		jointModel->drawWireframe(gl);

		gl->popMatrix();

		BOOST_FOREACH(int jointIndex, joint->children())
		{
			Joint * joint = joints[jointIndex];
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
	}

	if (selMesh_ >= 0)
	{		
		glDepthFunc(GL_ALWAYS);
		const std::vector<Mesh *> & meshes = model_->meshes();
		Mesh * mesh = meshes[selMesh_];
		mesh->drawWireframe(gl);
	}

	glDepthFunc(prevDepthFunc);
	gl->popMatrix();
}

bool InspectComponentTool::isBeingUsed() const
{
	return false;
}


