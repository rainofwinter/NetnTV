#include "stdafx.h"
#include "ScaleGizmo.h"
#include "GLWidget.h"
#include "Attrib.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "TransformTool.h"

using namespace std;

ScaleGizmo::ScaleGizmo(GLWidget * gl, TransformTool * transformTool) : 
	TransformGizmo(gl, transformTool)
{
	axisLenPixels_ = 50;
	cubeMesh_ = 0;
	grabLen_ = 5;
	centerSquareSize_ = 10;

	mode_ = NONE;
}

ScaleGizmo::~ScaleGizmo()
{
	delete cubeMesh_;
	delete gizmoMaterial_;
}


void ScaleGizmo::init()
{
	gizmoMaterial_ = new Material;
	cubeMesh_ = makeCubeMesh(gizmoMaterial_);
	
}

void ScaleGizmo::uninit()
{
}

Mesh * ScaleGizmo::makeCubeMesh(Material * material) const
{
	Mesh * cubeMesh = new Mesh;
	int numBaseVerts = 3;

	vector<Vector3> verts;

	verts.push_back(Vector3(-0.5, 1, 0.5));
	verts.push_back(Vector3(-0.5, 0, 0.5));
	verts.push_back(Vector3(0.5, 0, 0.5));
	verts.push_back(Vector3(0.5, 1, 0.5));
	verts.push_back(Vector3(-0.5, 1, -0.5));
	verts.push_back(Vector3(-0.5, 0, -0.5));
	verts.push_back(Vector3(0.5, 0, -0.5));
	verts.push_back(Vector3(0.5, 1, -0.5));
	
	unsigned short indices[] = 
	{
		0, 1, 2,
		0, 2, 3,
		
		3, 2, 6,
		3, 6, 7,
		
		1, 6, 2,
		1, 5, 6,

		0, 5, 1,
		0, 4, 5,

		0, 3, 7,
		0, 7, 4,

		4, 6, 5,
		4, 7, 6
		
	};
	
	std::vector<VertexData> vertices;
	int numVerts = (int)verts.size();

	for(int i = 0; i < numVerts; ++i)
	{
		VertexData vertexData;
		vertexData.position = verts[i];
		vertices.push_back(vertexData);
	}


	cubeMesh->setVertices(vertices);	
	gizmoMaterial_->setDiffuseColor(Color(1, 0, 0, 1));
	cubeMesh->addMeshPart(vector<unsigned short>(indices, indices + 3*6*2), material);
	cubeMesh->init(gl_);
	return cubeMesh;
}

void ScaleGizmo::update()
{
	origin_ = parentTransform_*transform_->globalPivot();
	const Camera * camera = glWidget_->camera();
	Plane plane = camera->lookAtPlane();
	plane = Plane::fromPointNormal(origin_, plane.normal());
	Vector3 a, b, intPt1, intPt2;
	Vector2 devicePt = glWidget_->windowToDeviceCoords(Vector2(0, 0));
	glWidget_->renderer()->unproject(devicePt, &a, &b);
	plane.intersectLine(&intPt1, a, b);
	devicePt = glWidget_->windowToDeviceCoords(Vector2(axisLenPixels_, 0));
	glWidget_->renderer()->unproject(devicePt, &a, &b);
	plane.intersectLine(&intPt2, a, b);
	float axisLenWorld = (intPt2 - intPt1).magnitude();
	cubeLen_ = axisLenWorld * 0.25f;	
	
	parentRotMatrix_ = 
		Transform(parentTransform_ * transform_->computeMatrix()).rotationMatrix();

	xAxis_ = Vector3(1, 0, 0);
	yAxis_ = Vector3(0, 1, 0);
	zAxis_ = Vector3(0, 0, 1);
	
	xAxis_ = (parentRotMatrix_ * xAxis_).normalize();
	yAxis_ = (parentRotMatrix_ * yAxis_).normalize();
	zAxis_ = (parentRotMatrix_ * zAxis_).normalize();
		
	xAxisTip_ = origin_ + axisLenWorld*xAxis_;
	yAxisTip_ = origin_ + axisLenWorld*yAxis_;
	zAxisTip_ = origin_ + axisLenWorld*zAxis_;

	cAxisTip_ = origin_ - 6*yAxis_ ;	
}



void ScaleGizmo::mousePressEvent(QMouseEvent * event, bool transformableObjClicked)
{
	if (transform_ && event->button() == Qt::LeftButton)
	{
		
		const Camera * cam = glWidget_->camera();
		Vector2 mousePt = Vector2(event->posF().x(), event->posF().y());
		origMousePt_ = glWidget_->windowToDeviceCoords(mousePt);
		Vector2 originPt = glWidget_->deviceToWindowCoords(glWidget_->renderer()->project(origin_));		
		float dist;

		mode_ = doHitTest(mousePt);

	}

	if (mode_ != NONE) 
	{
		transformStarted();
		initTransform_ = *transform_;
	}
}

void ScaleGizmo::mouseReleaseEvent(QMouseEvent * event)
{
	if (mode_ != NONE) transformEnded();
	mode_ = NONE;
	
}
void ScaleGizmo::mouseDoublePressEvent(QMouseEvent * event)
{	
	if (mode_ != NONE) transformEnded();
	mode_ = NONE;
}

void ScaleGizmo::mouseMoveEvent(QMouseEvent * event)
{
	if (mode_ == NONE) return;
	const Camera * cam = glWidget_->camera();	
	Vector3 origMouseA, origMouseB, mouseA, mouseB;
	Vector2 mousePt(event->posF().x(), event->posF().y());
	mousePt = glWidget_->windowToDeviceCoords(mousePt);

	glWidget_->renderer()->unproject(origMousePt_, &origMouseA, &origMouseB);
	glWidget_->renderer()->unproject(mousePt, &mouseA, &mouseB);	

	Vector3 axis(0, 0, 0);
	if (mode_ == X) axis = (xAxisTip_ - origin_).normalize();
	if (mode_ == Y) axis = (yAxisTip_ - origin_).normalize();
	if (mode_ == Z) axis = (zAxisTip_ - origin_).normalize();

	if (glWidget_->shiftPress())
	{
		if((glWidget_->shiftPress() && axis.x != 0) || (glWidget_->shiftPress() && axis.y !=0))
		{
			Vector3 planeNormal = makeNormalTo(origMouseB - origMouseA, axis);
			Plane plane = Plane::fromPointNormal(origin_, planeNormal);

			Vector3 pt, pt1, pt2;
			bool intersected = true;
			intersected &= plane.intersectLine(&pt1, origMouseA, origMouseB);
			intersected &= plane.intersectLine(&pt2, mouseA, mouseB);
			if (!intersected) return;
			pt2 = pt2 - origin_;
			pt1 = pt1 - origin_;
			pt2 = (pt2 * axis) * axis;
			pt1 = (pt1 * axis) * axis;

			float scale =
			pt2.magnitude() / pt1.magnitude();		
			if (pt1 * pt2 < 0) scale *= -1;
			Transform trans = initTransform_;

			Vector3 v = trans.scaling();
			v.x *= scale;
			v.y *= scale;

			trans.setScaling(v.x, v.y, v.z);
			*transform_ = trans;
		}
	}else{
		if (axis.x != 0 || axis.y != 0 || axis.z != 0)
		{
			Vector3 planeNormal = makeNormalTo(origMouseB - origMouseA, axis);
			Plane plane = Plane::fromPointNormal(origin_, planeNormal);

			Vector3 pt, pt1, pt2;
			bool intersected = true;
			intersected &= plane.intersectLine(&pt1, origMouseA, origMouseB);
			intersected &= plane.intersectLine(&pt2, mouseA, mouseB);
			if (!intersected) return;
			pt2 = pt2 - origin_;
			pt1 = pt1 - origin_;
			pt2 = (pt2 * axis) * axis;
			pt1 = (pt1 * axis) * axis;

			float scale =
			pt2.magnitude() / pt1.magnitude();		
			if (pt1 * pt2 < 0) scale *= -1;
			Transform trans = initTransform_;

			Vector3 v = trans.scaling();
			if (mode_ == X) v.x *= scale;
			if (mode_ == Y) v.y *= scale;
			if (mode_ == Z) v.z *= scale;

			trans.setScaling(v.x, v.y, v.z);
			*transform_ = trans;	
		}
		else if (mode_ == ALL)
		{
			Vector3 pt, pt1, pt2;
		
			Plane plane = Plane::fromPointNormal(origin_, cam->lookAtPlane().normal());

			plane.intersectLine(&pt1, origMouseA, origMouseB);
			plane.intersectLine(&pt2, mouseA, mouseB);
			pt = pt2 - pt1;
			float scale = 1 + 0.0012f*(pt.x + pt.y);
			Transform trans = initTransform_;
			Vector3 v = trans.scaling();
			v*=scale;
			trans.setScaling(v.x, v.y, v.z);
			*transform_ = trans;		
		}
	}	
	transformChanged();
}

ScaleGizmo::Mode ScaleGizmo::doHitTest(const Vector2 & mousePos) const
{
	Mode mode = NONE;
	if (!transform_) return mode;	

	Vector3 intPt;
	Vector3 a, b;
	Vector2 devicePt = glWidget_->windowToDeviceCoords(mousePos);
	glWidget_->renderer()->unproject(devicePt, &a, &b);
	Vector3 invA, invB;

	Matrix invMatrix;

	cubeMesh_->setTransformMatrix(
		Matrix::Translate(xAxisTip_.x, xAxisTip_.y, xAxisTip_.z) *
		parentRotMatrix_ *
		Matrix::Rotate(M_PI_2, 0, 0, -1) * 
		Matrix::Scale(cubeLen_, cubeLen_, cubeLen_));
	invMatrix = cubeMesh_->transform().inverse();
	invA = invMatrix * a;
	invB = invMatrix * b;
	if (cubeMesh_->intersectLine(&intPt, invA, invB)) mode = X;
	
	cubeMesh_->setTransformMatrix(
		Matrix::Translate(yAxisTip_.x, yAxisTip_.y, yAxisTip_.z) *
		parentRotMatrix_ *
		Matrix::Scale(cubeLen_, cubeLen_, cubeLen_));
	invMatrix = cubeMesh_->transform().inverse();
	invA = invMatrix * a;
	invB = invMatrix * b;
	if (cubeMesh_->intersectLine(&intPt, invA, invB)) mode = Y;

	cubeMesh_->setTransformMatrix(
		Matrix::Translate(zAxisTip_.x, zAxisTip_.y, zAxisTip_.z) *
		parentRotMatrix_ *
		Matrix::Rotate(M_PI_2, 1, 0, 0) * 
		Matrix::Scale(cubeLen_, cubeLen_, cubeLen_));
	invMatrix = cubeMesh_->transform().inverse();
	invA = invMatrix * a;
	invB = invMatrix * b;
	if (cubeMesh_->intersectLine(&intPt, invA, invB)) mode = Z;

	cubeMesh_->setTransformMatrix(
		Matrix::Translate(cAxisTip_.x, cAxisTip_.y, cAxisTip_.z) *
		parentRotMatrix_ *
		Matrix::Scale(cubeLen_, cubeLen_, cubeLen_));
	invMatrix = cubeMesh_->transform().inverse();
	invA = invMatrix * a;
	invB = invMatrix * b;
	if (cubeMesh_->intersectLine(&intPt, invA, invB)) mode = ALL;


	//check intersection with center square
	const Camera * cam = glWidget_->camera();
	Vector2 originPt = glWidget_->deviceToWindowCoords(glWidget_->renderer()->project(origin_));
	Vector2 v = originPt - mousePos;
	float dist = max(fabs(v.x), fabs(v.y));
	if (dist < centerSquareSize_/2)
	{
		mode = ALL;
	}
	
	return mode;
}

bool ScaleGizmo::checkAxisNotTooFlush(const Vector3 & axis) const
{
	Vector3 eyeToGizmo = (origin_ - glWidget_->camera()->eye()).normalize();
	const float BIAS = 0.05f;
	if (fabs(eyeToGizmo * axis) > 1 - BIAS) return false;
	return true;

}

void ScaleGizmo::draw()
{
	if (!transform_) return;
	glClear(GL_DEPTH_BUFFER_BIT);	

	int prevDepthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);
	glDepthFunc(GL_LEQUAL);
	gl_->useColorProgram();
	gl_->setColorProgramColor(0.75, 0.75, 0.75, 1);

	update();

	GLfloat vertices[] = {
		static_cast<GLfloat>(origin_.x), static_cast<GLfloat>(origin_.y), static_cast<GLfloat>(origin_.z),
		0, 0, 0};

	gl_->setColorProgramColor(0.8, 0.8, 0.8, 1);
	memcpy(&vertices[3], &xAxisTip_, sizeof(Vector3));
	gl_->enableVertexAttribArrayPosition();
	gl_->bindArrayBuffer(0);
	gl_->vertexAttribPositionPointer(0, (char *)vertices);
	gl_->applyCurrentShaderMatrix();
	glDrawArrays(GL_LINES, 0, 2);
	if (mode_ == X) gl_->setColorProgramColor(1, 1, 0, 1);
	else gl_->setColorProgramColor(1, 0, 0, 1);
	cubeMesh_->setTransformMatrix(
		Matrix::Translate(xAxisTip_.x, xAxisTip_.y, xAxisTip_.z) *	
		parentRotMatrix_ *
		Matrix::Rotate(M_PI_2, 0, 0, -1) * 
		Matrix::Scale(cubeLen_, cubeLen_, cubeLen_));
	cubeMesh_->drawBare(gl_);

	gl_->setColorProgramColor(0.8, 0.8, 0.8, 1);
	memcpy(&vertices[3], &yAxisTip_, sizeof(Vector3));
	gl_->enableVertexAttribArrayPosition();
	gl_->bindArrayBuffer(0);
	gl_->vertexAttribPositionPointer(0, (char *)vertices);
	gl_->applyCurrentShaderMatrix();
	glDrawArrays(GL_LINES, 0, 2);
	if (mode_ == Y) gl_->setColorProgramColor(1, 1, 0, 1);
	else gl_->setColorProgramColor(0, 1, 0, 1);	
	cubeMesh_->setTransformMatrix(
		Matrix::Translate(yAxisTip_.x, yAxisTip_.y, yAxisTip_.z) *
		parentRotMatrix_ *
		Matrix::Scale(cubeLen_, cubeLen_, cubeLen_));
	cubeMesh_->drawBare(gl_);

	gl_->setColorProgramColor(0.8, 0.8, 0.8, 1);
	memcpy(&vertices[3], &zAxisTip_, sizeof(Vector3));	
	gl_->enableVertexAttribArrayPosition();
	gl_->bindArrayBuffer(0);
	gl_->vertexAttribPositionPointer(0, (char *)vertices);
	gl_->applyCurrentShaderMatrix();
	glDrawArrays(GL_LINES, 0, 2);	
	if (mode_ == Z) gl_->setColorProgramColor(1, 1, 0, 1);
	else gl_->setColorProgramColor(0, 0, 1, 1);	
	cubeMesh_->setTransformMatrix(
		Matrix::Translate(zAxisTip_.x, zAxisTip_.y, zAxisTip_.z) * 	
		parentRotMatrix_ *
		Matrix::Rotate(M_PI_2, 1, 0, 0) * 
		Matrix::Scale(cubeLen_, cubeLen_, cubeLen_));
	cubeMesh_->drawBare(gl_);

	gl_->setColorProgramColor(0.8, 0.8, 0.8, 1);
	memcpy(&vertices[3], &zAxisTip_, sizeof(Vector3));	
	gl_->enableVertexAttribArrayPosition();
	gl_->bindArrayBuffer(0);
	gl_->vertexAttribPositionPointer(0, (char *)vertices);
	gl_->applyCurrentShaderMatrix();
	glDrawArrays(GL_LINES, 0, 2);	
	if (mode_ == ALL) gl_->setColorProgramColor(1, 1, 0, 1);
	else gl_->setColorProgramColor(0, 1, 1, 1);	
	cubeMesh_->setTransformMatrix(
		Matrix::Translate(cAxisTip_.x, cAxisTip_.y, cAxisTip_.z) * 	
		parentRotMatrix_ *
		Matrix::Scale(cubeLen_, cubeLen_, cubeLen_));
	cubeMesh_->draw(gl_);

	glDepthFunc(prevDepthFunc);
}

bool ScaleGizmo::intersect(const QPoint & mousePos) const
{
	return doHitTest(Vector2(mousePos.x(), mousePos.y())) != NONE;	
}

void ScaleGizmo::setParentTransform(const Matrix & parentTransform)
{
	parentTransform_ = parentTransform;
	parentRotMatrix_ = Transform(parentTransform).rotationMatrix();
}