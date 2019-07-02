#include "stdafx.h"
#include "RotateGizmo.h"
#include "GLWidget.h"
#include "Attrib.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"

using namespace std;

RotateGizmo::RotateGizmo(GLWidget * gl, TransformTool * transformTool) : 
TransformGizmo(gl, transformTool)
{
	const int numCircleVerts = 50;
	circleVerts_.resize(numCircleVerts + 1);	

	float theta;
	for (int i = 0; i < numCircleVerts; ++i)
	{
		theta = 2 * M_PI * ((float)i/numCircleVerts);
		circleVerts_[i].x = cos(theta);
		circleVerts_[i].y = sin(theta);
		circleVerts_[i].z = 0;
	}
	circleVerts_[numCircleVerts] = circleVerts_[0];
	
	
	
	
	gizmoRadiusPixels_ = 50;
	grabLen_ = 5;
	centerSquareSize_ = 10;




	mode_ = NONE;
}

RotateGizmo::~RotateGizmo()
{
}


void RotateGizmo::init()
{	
}

void RotateGizmo::uninit()
{
}

void RotateGizmo::setTransformAttrib(Transform * transform)
{
	TransformGizmo::setTransformAttrib(transform);
	if (transform) initTransform_ = *transform;
}

void RotateGizmo::update()
{	
	const Camera * camera = glWidget_->camera();

	Matrix initRot = Transform(parentTransform_).rotationMatrix() * transform_->rotationMatrix();

	origin_ = parentTransform_*initTransform_.globalPivot();
	Plane plane = camera->lookAtPlane();
	plane = Plane::fromPointNormal(origin_, plane.normal());
	Vector3 a, b, intPt1, intPt2;
	Vector2 devicePt = glWidget_->windowToDeviceCoords(Vector2(0, 0));
	glWidget_->renderer()->unproject(devicePt, &a, &b);
	plane.intersectLine(&intPt1, a, b);
	devicePt = glWidget_->windowToDeviceCoords(Vector2(gizmoRadiusPixels_, 0));
	glWidget_->renderer()->unproject(devicePt, &a, &b);
	plane.intersectLine(&intPt2, a, b);
	gizmoRadiusWorld_ = (intPt2 - intPt1).magnitude();	
	//x axis circle
	xCircleTransform_ = initRot * Matrix::Rotate(M_PI_2, 0, 1, 0);
	xCircleTransform_ = 
		Matrix::Scale(gizmoRadiusWorld_, gizmoRadiusWorld_, gizmoRadiusWorld_) *
		xCircleTransform_;
	xCircleTransform_ = 
		Matrix::Translate(origin_.x, origin_.y, origin_.z) * xCircleTransform_;

	//y axis circle
	yCircleTransform_ = initRot * Matrix::Rotate(M_PI_2, 1, 0, 0);
	yCircleTransform_ = 
		Matrix::Scale(gizmoRadiusWorld_, gizmoRadiusWorld_, gizmoRadiusWorld_) *
		yCircleTransform_;
	yCircleTransform_ = 
		Matrix::Translate(origin_.x, origin_.y, origin_.z) * yCircleTransform_;

	//z axis circle	
	zCircleTransform_ = initRot *
		Matrix::Scale(gizmoRadiusWorld_, gizmoRadiusWorld_, gizmoRadiusWorld_);		
	zCircleTransform_ = 
		Matrix::Translate(origin_.x, origin_.y, origin_.z) * zCircleTransform_;
	
	camCircleTransform_ =
		Matrix::Rotate(Vector3(0, 0, 1), camera->direction());
	camCircleTransform_ = 
		Matrix::Scale(gizmoRadiusWorld_, gizmoRadiusWorld_, gizmoRadiusWorld_) *
		camCircleTransform_;
	camCircleTransform_ = 
		Matrix::Translate(origin_.x, origin_.y, origin_.z) * camCircleTransform_;

}

void RotateGizmo::computeCurRotatePlane()
{
	switch (mode_)
	{
		case X: rotatePlane_ = xPlane_; break;
		case Y: rotatePlane_ = yPlane_; break;
		case Z:	rotatePlane_ = zPlane_; break;
		case CAMERA: rotatePlane_ = camPlane_; break;
	}
}

void RotateGizmo::computeInitRotatePlanes()
{
	Matrix * transforms[] = 
	{&xCircleTransform_, &yCircleTransform_, &zCircleTransform_, &camCircleTransform_};
	int numElem = sizeof(transforms)/sizeof(Matrix *);


	Plane * planes[] = 
	{&xPlane_, &yPlane_, &zPlane_, &camPlane_};


	for (int i = 0; i < numElem; ++i)
	{
		Vector3 pt1(0, 0, 0);
		Vector3 pt2(0, 1, 0);
		Vector3 pt3(1, 0, 0);

		pt1 = *transforms[i] * pt1;
		pt2 = *transforms[i] * pt2;
		pt3 = *transforms[i] * pt3;
		*planes[i] = Plane::fromPoints(pt1, pt2, pt3);
	}

}

void RotateGizmo::mousePressEvent(QMouseEvent * event, bool transformableObjClicked)
{	
	if (event->button() == Qt::LeftButton)
	{
		if (!transform_) return;		

		const Camera * cam = glWidget_->camera();
		Vector2 mousePt = Vector2(event->posF().x(), event->posF().y());
		origMousePt_ = glWidget_->windowToDeviceCoords(mousePt);
		Vector2 originPt = glWidget_->deviceToWindowCoords(glWidget_->renderer()->project(origin_));
		float dist;

		mode_ = doHitTest(mousePt);	

		
		update();

		if (mode_ != NONE) 
		{
			transformStarted();
			computeInitRotatePlanes();
			initTransform_ = *transform_;
		}
	}
}

void RotateGizmo::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (mode_ != NONE) transformEnded();
		mode_ = NONE;
	}
}

void RotateGizmo::mouseDoublePressEvent(QMouseEvent * event)
{
	if (mode_ != NONE) transformEnded();
	mode_ = NONE;
}

void RotateGizmo::mouseMoveEvent(QMouseEvent * event)
{
	if (mode_ == NONE) return;

	const Camera * cam = glWidget_->camera();	
	Vector3 origMouseA, origMouseB, mouseA, mouseB;
	Vector2 mousePt(event->posF().x(), event->posF().y());
	mousePt = glWidget_->windowToDeviceCoords(mousePt);
	glWidget_->renderer()->unproject(origMousePt_, &origMouseA, &origMouseB);
	glWidget_->renderer()->unproject(mousePt, &mouseA, &mouseB);

	Sphere gizmoSphere(origin_, gizmoRadiusWorld_);
	Vector3 mouseIntPt, origMouseIntPt, temp;

	Plane intPlane = Plane::fromPointNormal(origin_, cam->eye() - origin_);
	bool mouseIntersectSphere = gizmoSphere.
		intersectLine(&mouseIntPt, &temp, mouseA, mouseB);
	
	if (!mouseIntersectSphere)
		intPlane.intersectLine(&mouseIntPt, mouseA, mouseB);

	bool mouseOrigIntersectSphere = gizmoSphere.
		intersectLine(&origMouseIntPt, &temp, origMouseA, origMouseB);

	if (!mouseOrigIntersectSphere)
		intPlane.intersectLine(&origMouseIntPt, origMouseA, origMouseB);

	computeCurRotatePlane();

	mouseIntPt.projectOnToPlane(rotatePlane_);
	origMouseIntPt.projectOnToPlane(rotatePlane_);

	Matrix newRotation = 
		Matrix::Translate(origin_.x, origin_.y, origin_.z) * 
		Matrix::Rotate(origMouseIntPt - origin_, mouseIntPt - origin_) *
		Matrix::Translate(-origin_.x, -origin_.y, -origin_.z);

	Transform trans = initTransform_;
	trans.concatenate(
		parentTransform_.inverse() * newRotation * parentTransform_,
		true
		);

	
	Matrix rotOnlyMatrix = trans.rotationMatrix();
	trans = initTransform_;
	trans.setRotation(rotOnlyMatrix);
	

	*transform_ = trans;

	transformChanged();
}


void RotateGizmo::draw()
{
	if (!transform_) return;

	int prevDepthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
	gl_->useColorProgram();
	gl_->setColorProgramColor(0.75, 0.75, 0.75, 1);
	
	update();

	Vector3 projOrigin = glWidget_->renderer()->project(origin_);
	
/*
	float data[200];
	glReadBuffer(GL_FRONT);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);	
	glReadPixels(viewport[2]/2, viewport[3]/2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, data);
	qDebug() << data[0] << 0.5*(projOrigin.z + 1) << gizmoRadiusWorld_;
*/
	
	gl_->enableVertexAttribArrayPosition();
	gl_->bindArrayBuffer(0);
	gl_->vertexAttribPositionPointer(0, (char *)&circleVerts_[0]);

	gl_->pushMatrix();
	gl_->loadMatrix(camCircleTransform_);
	if (mode_ == CAMERA) gl_->setColorProgramColor(1, 1, 0, 1);
	else gl_->setColorProgramColor(0.8, 0.8, 0.8, 1);
	gl_->applyCurrentShaderMatrix();
	glDrawArrays(GL_LINE_STRIP, 0, circleVerts_.size());

	
	
	glClearDepth(0.5*(projOrigin.z + 1) + 0.00001f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearDepth(1);

	gl_->loadMatrix(xCircleTransform_);
	if (mode_ == X)
	gl_->setColorProgramColor(1, 1, 0, 1);
	else
	gl_->setColorProgramColor(1, 0, 0, 1);
	gl_->applyCurrentShaderMatrix();
	glDrawArrays(GL_LINE_STRIP, 0, circleVerts_.size());


	gl_->loadMatrix(yCircleTransform_);
	if (mode_ == Y) gl_->setColorProgramColor(1, 1, 0, 1);
	else gl_->setColorProgramColor(0, 1, 0, 1);
	gl_->applyCurrentShaderMatrix();
	glDrawArrays(GL_LINE_STRIP, 0, circleVerts_.size());

	gl_->loadMatrix(zCircleTransform_);
	if (mode_ == Z) gl_->setColorProgramColor(1, 1, 0, 1);
	else gl_->setColorProgramColor(0, 0, 1, 1);
	gl_->applyCurrentShaderMatrix();
	glDrawArrays(GL_LINE_STRIP, 0, circleVerts_.size());

	gl_->popMatrix();
	glDepthFunc(prevDepthFunc);
	
	
	return;
}


RotateGizmo::Mode RotateGizmo::doHitTest(const Vector2 & mousePos) const
{
	if (!transform_) return NONE;
	const Camera * cam = glWidget_->camera();
	Mode mode = NONE;	

	const int numCircles = 4;	
	const Matrix transforms[numCircles] = 
	{camCircleTransform_, xCircleTransform_, yCircleTransform_, zCircleTransform_};
	Mode modes[numCircles];
	
	modes[0] = CAMERA;
	modes[1] = X;
	modes[2] = Y;
	modes[3] = Z;
	
	
	int numCircleVerts = circleVerts_.size() - 1;
	Vector3 projOrigin = glWidget_->renderer()->project(origin_);

	for (int i = 0; i < numCircles; ++i)
	{
		for (int j = 0; j < numCircleVerts; ++j)
		{
			int index1 = j;
			int index2 = j + 1;
			Vector3 a, b;
			a = glWidget_->renderer()->project(transforms[i] * circleVerts_[index1]);
			b = glWidget_->renderer()->project(transforms[i] * circleVerts_[index2]);		

			//if this line segment of the circle corresponds to a back part of 
			//the circle that isn't displayed then just skip it
			if (a.z > projOrigin.z && b.z > projOrigin.z)
			{
				continue;
			}

			Vector2 aScreen = glWidget_->deviceToWindowCoords(Vector2(a.x, a.y));
			Vector2 bScreen = glWidget_->deviceToWindowCoords(Vector2(b.x, b.y));
			Vector2 temp;

			float distance = 
				distancePointSegment(&temp, mousePos, aScreen, bScreen);

			if (distance < grabLen_)
			{
				mode = modes[i];
				break;
			}
		}	
	}
	
	return mode;
}

bool RotateGizmo::intersect(const QPoint & mousePos) const
{
	return doHitTest(Vector2(mousePos.x(), mousePos.y())) != NONE;
}