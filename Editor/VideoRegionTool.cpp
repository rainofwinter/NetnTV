#include "stdafx.h"
#include "VideoRegionTool.h"
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

#include "VideoObject.h"
#include "Command.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
class VideoRegionCmd : public Command
{
public:
	VideoRegionCmd(
		VideoObject * object,
		const VideoRegion & origRegion,
		const VideoRegion & newRegion
		);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Video region tool");}
	
private:	
	VideoRegion origRegion_;
	VideoRegion newRegion_;

	VideoObject * object_;
};

///////////////////////////////////////////////////////////////////////////////
VideoRegionCmd::VideoRegionCmd(
	VideoObject * object,
	const VideoRegion & origRegion,
	const VideoRegion & newRegion)	
{	
	object_ = object;
	origRegion_ = origRegion;
	newRegion_ = newRegion;	
}

void VideoRegionCmd::doCommand()
{
	object_->setX(newRegion_.x);
	object_->setY(newRegion_.y);
	object_->setWidth(newRegion_.width);
	object_->setHeight(newRegion_.height);
	emit objectChanged();
}

void VideoRegionCmd::undoCommand()
{
	object_->setX(origRegion_.x);
	object_->setY(origRegion_.y);
	object_->setWidth(origRegion_.width);
	object_->setHeight(origRegion_.height);
	emit objectChanged();
}

///////////////////////////////////////////////////////////////////////////////



VideoRegionTool::VideoRegionTool(GLWidget * gl) : Tool(gl)
{	
	transformStarted_ = false;
	object_ = NULL;
	handlePixelSize_ = 6;

	toolName_ = "VideoRegionTool";
}

VideoRegionTool::~VideoRegionTool()
{	
	
}


void VideoRegionTool::init()
{		
	transformStarted_ = false;
}

void VideoRegionTool::uninit()
{	
	transformStarted_ = false;
}

void VideoRegionTool::setObject(VideoObject * obj)
{
	object_ = obj;
}

VideoRegionTool::TransformMode 
	VideoRegionTool::transformMode(const Vector3 & mousePos)
{
	computeBBoxAndHandlePts();

	float handleScreenSizeX = 2 * (float)handlePixelSize_ / glWidget_->width();
	float handleScreenSizeY = 2 * (float)handlePixelSize_ / glWidget_->height();

	Camera camera = glWidget_->appLayerCamera();
	for(int i = 7; i >= 0; --i)
	{
		Vector3 mouseDevicePos = camera.project(mousePos);
		Vector3 handlePt = camera.project(handlePts_[i]);
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

void VideoRegionTool::computeBBoxAndHandlePts()
{
	bBox_ = BoundingBox();
	
	bBox_.minPt.x = object_->x();
	bBox_.minPt.y = object_->y();
	bBox_.minPt.z = 0.0f;

	bBox_.maxPt.x = object_->x() + object_->width();
	bBox_.maxPt.y = object_->y() + object_->height();
	bBox_.maxPt.z = 0.0f;

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

void VideoRegionTool::mousePressEvent(QMouseEvent * event)
{
	Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));

	Camera camera;
	Matrix prevCamMatrix;
	glWidget_->appLayerCamera(&camera, &prevCamMatrix);
	Ray mouseRay = camera.unproject(mousePosDevice);
	
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
		glWidget_->setToolMode(GLWidget::Other);

	setCursor(worldMousePt);
}

void VideoRegionTool::startTransform(const Vector3 & mousePt)
{
	initRegion_.x = object_->x();
	initRegion_.y = object_->y();
	initRegion_.width = object_->width();
	initRegion_.height = object_->height();

	transformMode_ = transformMode(mousePt);
	if (transformMode_ == None) return;

	transformStarted_ = true;
	transformed_ = false;
	startMousePt_ = mousePt;	

	computeBBoxAndHandlePts();
	startBBox_ = bBox_;
	startHandlePts_ = handlePts_;
}


void VideoRegionTool::mouseReleaseEvent(QMouseEvent * event)
{
	transformStarted_ = false;

	if (!transformed_) return;

	VideoRegion region;
	region.x = object_->x();
	region.y = object_->y();
	region.width = object_->width();
	region.height = object_->height();
	
	document_->doCommand(new VideoRegionCmd(
		object_, initRegion_, region));

}

bool VideoRegionTool::hoveringOverGizmo(QMouseEvent * event)
{
	Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));

	Ray mouseRay = glWidget_->appLayerCamera().unproject(mousePosDevice);
	
	Plane plane = Plane::fromPointNormal(Vector3(0, 0, 0), Vector3(0, 0, 1));
	Vector3 worldMousePt;
	plane.intersect(&worldMousePt, mouseRay);
	return transformMode(worldMousePt) != None;
}

void VideoRegionTool::setCursor(const Vector3 & mousePos)
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

void VideoRegionTool::mouseMoveEvent(QMouseEvent * event)
{
	Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));

	Ray mouseRay = glWidget_->appLayerCamera().unproject(mousePosDevice);
	
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
	

	VideoRegion t = initRegion_;
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
	
	object_->setX(t.x);
	object_->setY(t.y);
	object_->setWidth(t.width);
	object_->setHeight(t.height);	

	emit objectChanged();
	transformed_ = true;
}



void VideoRegionTool::setActive(bool val)
{
	Tool::setActive(val);		

	document_->disconnect(this);
	if (val)
	{

		connect(document_, SIGNAL(objectSelectionChanged()), 
			this, SLOT(onObjectSelectionChanged()));
	}
}


void VideoRegionTool::onObjectSelectionChanged()
{	
	if (!document_->selectedObject() || document_->selectedObject() != object_)
	{
		object_ = NULL;
		document_->glWidget()->setToolMode(GLWidget::Select);
	}
}


void VideoRegionTool::draw()
{	
	if (!object_) return;

	computeBBoxAndHandlePts();

	Matrix prevCamMatrix;
	Camera camera; 
	glWidget_->appLayerCamera(&camera, &prevCamMatrix);
	gl_->setCameraMatrix(camera.GetMatrix());

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
	
	float handleScreenSizeX = 2 * (float)handlePixelSize_ / glWidget_->width();
	float handleScreenSizeY = 2 * (float)handlePixelSize_ / glWidget_->height();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	BOOST_FOREACH(Vector3 handlePt, handlePts_)
	{
		Vector3 screenPt = camera.project(handlePt);

		gl_->drawRect(
			screenPt.x - handleScreenSizeX / 2, 
			screenPt.y - handleScreenSizeY / 2, 
			handleScreenSizeX, handleScreenSizeY);
	}

	gl_->setCameraMatrix(prevCamMatrix);
	

}

bool VideoRegionTool::isBeingUsed() const
{
	return transformStarted_;
}

