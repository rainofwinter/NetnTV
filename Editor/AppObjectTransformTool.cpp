#include "stdafx.h"
#include "AppObjectTransformTool.h"
#include "GLWidget.h"
#include "Attrib.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "EditorDocument.h"
#include "EditorObject.h"
#include "Command.h"
#include "Scene.h"

#include "Utils.h"

#include "AppObject.h"
#include "Command.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
class AppObjectTransformCmd : public Command
{
public:
	AppObjectTransformCmd(
		const std::vector<InitTransformData> & initTransforms,
		const std::vector<AppObjectTransform> & transforms
		);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Set app object transforms");}
	
private:	
	std::vector<AppObjectTransform> transforms_;
	std::vector<InitTransformData> initTransforms_;

	Matrix transformMatrix_;
};

///////////////////////////////////////////////////////////////////////////////
AppObjectTransformCmd::AppObjectTransformCmd(
	const std::vector<InitTransformData> & initTransforms,
	const std::vector<AppObjectTransform> & transforms)
{	
	initTransforms_ = initTransforms;
	transforms_ = transforms;	
}

void AppObjectTransformCmd::doCommand()
{	
	for (int i = 0; i < (int)initTransforms_.size(); ++i)	
	{			
		initTransforms_[i].appObject->setTransform(transforms_[i]);			
	}
	
	emit appObjectChanged();
}

void AppObjectTransformCmd::undoCommand()
{
	for (int i = 0; i < (int)initTransforms_.size(); ++i)	
	{	
		initTransforms_[i].appObject->setTransform(initTransforms_[i].initTransform);			
	}
	emit appObjectChanged();
}

///////////////////////////////////////////////////////////////////////////////



float handlePixelSize = 6;

AppObjectTransformTool::AppObjectTransformTool(GLWidget * gl) : Tool(gl)
{	
	transformStarted_ = false;
	isShiftKey_ = false;
	isCtrlKey_ = false;
}

AppObjectTransformTool::~AppObjectTransformTool()
{	
	
}


void AppObjectTransformTool::init()
{		
	transformStarted_ = false;
}

void AppObjectTransformTool::uninit()
{	
	transformStarted_ = false;
}

AppObjectTransformTool::TransformMode 
	AppObjectTransformTool::transformMode(const Vector3 & mousePos)
{
	computeBBoxAndHandlePts();

	float handleScreenSizeX = 2 * (float)handlePixelSize / glWidget_->width();
	float handleScreenSizeY = 2 * (float)handlePixelSize / glWidget_->height();

	for(int i = 7; i >= 0; --i)
	{
		Vector3 mouseDevicePos = glWidget_->renderer()->project(mousePos);
		Vector3 handlePt = glWidget_->renderer()->project(handlePts_[i]);
		if (handlePt.x - handleScreenSizeX/2 <= mouseDevicePos.x &&
			mouseDevicePos.x <= handlePt.x + handleScreenSizeX/2 &&
			handlePt.y - handleScreenSizeY/2 <= mouseDevicePos.y &&
			mouseDevicePos.y <= handlePt.y + handleScreenSizeY/2)
		{
			return  (TransformMode)i;
		}
	}

	if (bBox_.minPt.x <= mousePos.x && mousePos.x <= bBox_.maxPt.x &&
		bBox_.minPt.y <= mousePos.y && mousePos.y <= bBox_.maxPt.y)
	{
		return Translate;
	}
	
	return None;

	
}

void AppObjectTransformTool::computeBBoxAndHandlePts()
{
	bBox_ = BoundingBox();
	BOOST_FOREACH(AppObject * appObject, document_->selectedAppObjects())
	{
		bBox_ = bBox_.unite(appObject->worldExtents());

	}	

	handlePts_.clear();

	handlePts_.push_back(bBox_.minPt);
	handlePts_.push_back(Vector3(bBox_.minPt.x, (bBox_.maxPt.y + bBox_.minPt.y) / 2, 0));
	handlePts_.push_back(Vector3(bBox_.minPt.x, bBox_.maxPt.y, 0));
	handlePts_.push_back(Vector3((bBox_.minPt.x + bBox_.maxPt.x) / 2, bBox_.maxPt.y, 0));
	handlePts_.push_back(bBox_.maxPt);	
	handlePts_.push_back(Vector3(bBox_.maxPt.x, (bBox_.maxPt.y + bBox_.minPt.y) / 2, 0));
	handlePts_.push_back(Vector3(bBox_.maxPt.x, bBox_.minPt.y, 0));
	handlePts_.push_back(Vector3((bBox_.minPt.x + bBox_.maxPt.x) / 2, bBox_.minPt.y, 0));
	
}

bool AppObjectTransformTool::keyPressEvent(QKeyEvent * event)
{
	if(event->key() == Qt::Key_Shift) isShiftKey_ = true;
	if(event->key() == Qt::Key_Control) isCtrlKey_ = true;

	return false;
}

void AppObjectTransformTool::keyReleaseEvent(QKeyEvent * event)
{
	if(event->key() == Qt::Key_Shift) isShiftKey_ = false;
	if(event->key() == Qt::Key_Control) isCtrlKey_ = false;
}

void AppObjectTransformTool::mousePressEvent(QMouseEvent * event)
{		

	Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));

	Ray mouseRay = glWidget_->renderer()->unproject(mousePosDevice);
	
	Plane plane = Plane::fromPointNormal(Vector3(0, 0, 0), Vector3(0, 0, 1));
	Vector3 worldMousePt;
	plane.intersect(&worldMousePt, mouseRay);

	bool clickedGizmo = transformMode(worldMousePt) != None;

	AppObject * hoverObj = document_->selectedScene()->intersectAppObject(mouseRay);


	if (clickedGizmo)
	{
		//transform selection
		startTransform(worldMousePt);
	}
	else
	{
		//modify selection		
		if (hoverObj)
		{
			if (isShiftKey_)			
				document_->setAppObjectSelected(hoverObj, true);
			else if (isCtrlKey_)
				document_->setAppObjectSelected(hoverObj, 
					!document_->isAppObjectSelected(hoverObj));
			else
				document_->selectAppObject(hoverObj);

			startTransform(worldMousePt);
		}
		else if (!hoverObj)
		{
			document_->deselectAllAppObjects();
		}
	}

	setCursor(worldMousePt);
}

void AppObjectTransformTool::startTransform(const Vector3 & mousePt)
{
	initTransformData_.clear();
	BOOST_FOREACH(AppObject * appObj, document_->selectedAppObjects())
	{
		InitTransformData data;
		//data.initTransform = appObj->visualAttrib()->transform();
		data.initTransform.x = appObj->x();
		data.initTransform.y = appObj->y();
		data.initTransform.width = appObj->width();
		data.initTransform.height = appObj->height();
		data.appObject = appObj;
		initTransformData_.push_back(data);
	}

	transformMode_ = transformMode(mousePt);
	if (transformMode_ == None) return;

	transformStarted_ = true;
	transformed_ = false;
	startMousePt_ = mousePt;	

	computeBBoxAndHandlePts();
	startBBox_ = bBox_;
	startHandlePts_ = handlePts_;
}


void AppObjectTransformTool::mouseReleaseEvent(QMouseEvent * event)
{
	transformStarted_ = false;

	if (!transformed_) return;

	vector<AppObjectTransform> transforms;

	for (int i = 0; i < (int)initTransformData_.size(); ++i)
	{
		transforms.push_back(initTransformData_[i].appObject->transform());
	}
	
	document_->doCommand(new AppObjectTransformCmd(
		initTransformData_, transforms));

}

bool AppObjectTransformTool::hoveringOverGizmo(QMouseEvent * event)
{
	Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));

	Ray mouseRay = glWidget_->renderer()->unproject(mousePosDevice);
	
	Plane plane = Plane::fromPointNormal(Vector3(0, 0, 0), Vector3(0, 0, 1));
	Vector3 worldMousePt;
	plane.intersect(&worldMousePt, mouseRay);
	return transformMode(worldMousePt) != None;
}

void AppObjectTransformTool::setCursor(const Vector3 & mousePos)
{
	//set proper cursor
	TransformMode mode = transformMode(mousePos);
	switch(mode)
	{
	case ResizeNW: 
	case ResizeSE:
		glWidget_->setCursor(Qt::SizeFDiagCursor);
		break;

	case ResizeW: 
	case ResizeE:
		glWidget_->setCursor(Qt::SizeHorCursor); 
		break;

	case ResizeN:
	case ResizeS:
		glWidget_->setCursor(Qt::SizeVerCursor);
		break;

	case ResizeSW:
	case ResizeNE:
		glWidget_->setCursor(Qt::SizeBDiagCursor);
		break;
	
	case Translate:
		glWidget_->setCursor(Qt::SizeAllCursor);
		break;
	default:
		glWidget_->setCursor(Qt::ArrowCursor);
	}

	return;
}

void AppObjectTransformTool::mouseMoveEvent(QMouseEvent * event)
{
	Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));

	Ray mouseRay = glWidget_->renderer()->unproject(mousePosDevice);
	
	Plane plane = Plane::fromPointNormal(Vector3(0, 0, 0), Vector3(0, 0, 1));
	Vector3 worldMousePt;
	plane.intersect(&worldMousePt, mouseRay);


	if (!transformStarted_) 
	{
		setCursor(worldMousePt);
		return;
	}
	

	Vector3 mouseDelta = worldMousePt - startMousePt_;

	float sx, sy;

	transformMatrix_ = Matrix::Identity();

	switch(transformMode_)
	{
	case ResizeNW:
		sx = (startBBox_.lengthX() + startMousePt_.x - worldMousePt.x) / 
			startBBox_.lengthX();
		sy = (startBBox_.lengthY() + startMousePt_.y - worldMousePt.y) / 
			startBBox_.lengthY();
		transformMatrix_ = Matrix::Scale(startHandlePts_[4], sx, sy, 1);
		break;
	case ResizeW:
		sx = (startBBox_.lengthX() + startMousePt_.x - worldMousePt.x) / 
			startBBox_.lengthX();		
		sy = 1;
		transformMatrix_ = Matrix::Scale(startHandlePts_[5], sx, sy, 1);
		break;
	case ResizeSW:
		sx = (startBBox_.lengthX() + startMousePt_.x - worldMousePt.x) / 
			startBBox_.lengthX();
		sy = (startBBox_.lengthY() - startMousePt_.y + worldMousePt.y) / 
			startBBox_.lengthY();
		transformMatrix_ = Matrix::Scale(startHandlePts_[6], sx, sy, 1);
		break;
	case ResizeS:
		sx = 1;
		sy = (startBBox_.lengthY() - startMousePt_.y + worldMousePt.y) / 
			startBBox_.lengthY();
		transformMatrix_ = Matrix::Scale(startHandlePts_[7], sx, sy, 1);
		break;	
	case ResizeSE:
		sx = (startBBox_.lengthX() - startMousePt_.x + worldMousePt.x) / 
			startBBox_.lengthX();
		sy = (startBBox_.lengthY() - startMousePt_.y + worldMousePt.y) / 
			startBBox_.lengthY();
		transformMatrix_ = Matrix::Scale(startHandlePts_[0], sx, sy, 1);
		break;	
	case ResizeE:
		sx = (startBBox_.lengthX() - startMousePt_.x + worldMousePt.x) / 
			startBBox_.lengthX();
		sy = 1;
		transformMatrix_ = Matrix::Scale(startHandlePts_[1], sx, sy, 1);
		break;	
	case ResizeNE:
		sx = (startBBox_.lengthX() - startMousePt_.x + worldMousePt.x) / 
			startBBox_.lengthX();
		sy = (startBBox_.lengthY() + startMousePt_.y - worldMousePt.y) / 
			startBBox_.lengthY();
		transformMatrix_ = Matrix::Scale(startHandlePts_[2], sx, sy, 1);
		break;
	case ResizeN:
		sx = 1;
		sy = (startBBox_.lengthY() + startMousePt_.y - worldMousePt.y) / 
			startBBox_.lengthY();
		transformMatrix_ = Matrix::Scale(startHandlePts_[3], sx, sy, 1);
		break;			
	case Translate:
		transformMatrix_ = Matrix::Translate(
			mouseDelta.x,
			mouseDelta.y,
			mouseDelta.z);
		break;
	}
	

	BOOST_FOREACH(InitTransformData data, initTransformData_)
	{
		//Transform transform = data.initTransform;
		AppObjectTransform t = data.initTransform;
		Vector3 v1(t.x, t.y, 0);
		Vector3 v2(t.x + t.width, t.y, 0);
		Vector3 v3(t.x + t.width, t.y + t.height, 0);
		Vector3 v4(t.x, t.y + t.height, 0);

		v1 = transformMatrix_ * v1;
		v2 = transformMatrix_ * v2;
		v3 = transformMatrix_ * v3;
		v4 = transformMatrix_ * v4;
		
		t.x = (int)roundFloat(min(v1.x, v2.x));
		t.y = (int)roundFloat(min(v1.y, v3.y));
		t.width = (int)roundFloat(fabs(v2.x - v1.x));
		t.height = (int)roundFloat(fabs(v3.y - v1.y));
		
		data.appObject->setTransform(t);		
	}

	emit appObjectChanged();
	transformed_ = true;
}



void AppObjectTransformTool::setActive(bool val)
{
	Tool::setActive(val);

	document_->disconnect(this);

	
	if (val)
	{		

		//need to update parent transforms after reparenting
		/*
		connect(document_, SIGNAL(objectListChanged()), 
			this, SLOT(initTransformObjs()));		
		*/

		//when object transform changes externally (user type-in or animation)
		//then the gizmo might need to be changed accordingly
		/*
		connect(document_, SIGNAL(appObjectChanged()),
			this, SLOT(onObjectChanged()));

		
		//same issue as above
		connect(document_, SIGNAL(timeLineTimeChanged()),
			this, SLOT(onObjectChanged()));

		connect(document_, SIGNAL(timeLineTimeChanging()),
			this, SLOT(onObjectChanged()));
		*/
	}
	
	
}


void AppObjectTransformTool::draw()
{	
	if (document_->selectedAppObjects().empty()) return;
	Matrix prevCamMatrix = gl_->cameraMatrix();
	Camera cam = glWidget_->appLayerCamera();
	gl_->setCameraMatrix(cam.GetMatrix());

	computeBBoxAndHandlePts();

	gl_->useColorProgram();
	gl_->setColorProgramColor(1, 0, 0, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonOffset(-1, 0);
	GLfloat vertices[] = {
		static_cast<GLfloat>(bBox_.minPt.x), static_cast<GLfloat>(bBox_.minPt.y), 0, 
		static_cast<GLfloat>(bBox_.maxPt.x), static_cast<GLfloat>(bBox_.minPt.y), 0,
		static_cast<GLfloat>(bBox_.maxPt.x), static_cast<GLfloat>(bBox_.maxPt.y), 0,
		static_cast<GLfloat>(bBox_.minPt.x), static_cast<GLfloat>(bBox_.maxPt.y), 0
	};
	gl_->enableVertexAttribArrayPosition();
	gl_->bindArrayBuffer(0);
	gl_->vertexAttribPositionPointer(0, (char *)vertices);
	gl_->applyCurrentShaderMatrix();
	glDrawArrays(GL_QUADS, 0, 4);
	glPolygonOffset(0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	
	gl_->setCameraMatrix(Matrix::Identity());

	
	float handleScreenSizeX = 2 * (float)handlePixelSize / glWidget_->width();
	float handleScreenSizeY = 2 * (float)handlePixelSize / glWidget_->height();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	BOOST_FOREACH(Vector3 handlePt, handlePts_)
	{
		Vector3 screenPt = cam.project(handlePt);

		gl_->drawRect(
			screenPt.x - handleScreenSizeX / 2, 
			screenPt.y - handleScreenSizeY / 2, 
			handleScreenSizeX, handleScreenSizeY);
	}

	gl_->setCameraMatrix(prevCamMatrix);
	

}

bool AppObjectTransformTool::isBeingUsed() const
{
	return transformStarted_;
}

