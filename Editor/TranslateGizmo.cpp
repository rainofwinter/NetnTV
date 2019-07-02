#include "stdafx.h"
#include "TranslateGizmo.h"
#include "GLWidget.h"
#include "Attrib.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "TransformTool.h"

using namespace std;

TranslateGizmo::TranslateGizmo(GLWidget * gl, TransformTool * transformTool) : 
	TransformGizmo(gl, transformTool)
{
	axisLenPixels_ = 50;
	coneMesh_ = 0;
	grabLen_ = 5;
	centerSquareSize_ = 10;

	mode_ = NONE;
}

TranslateGizmo::~TranslateGizmo()
{
	delete coneMesh_;
	delete coneMaterial_;
}


void TranslateGizmo::init()
{
	coneMaterial_ = new Material;
	coneMesh_ = makeConeMesh(coneMaterial_);
	
}

void TranslateGizmo::uninit()
{
}

Mesh * TranslateGizmo::makeConeMesh(Material * material) const
{
	Mesh * coneMesh = new Mesh;
	int numBaseVerts = 14;

	vector<Vector3> verts;
	vector<unsigned short> indices;
	verts.push_back(Vector3(0, 1, 0));	

	for (int i = 0; i < numBaseVerts; ++i)
	{
		float angle = 2*M_PI*i/numBaseVerts;
		verts.push_back(Vector3(sin(angle), 0, cos(angle)));

	}
	
	verts.push_back(Vector3(0, 0, 0));

	//tris along the side
	for (int i = 1; i <= numBaseVerts; ++i)
	{
		unsigned short next = i + 1; if (next > numBaseVerts) next = 1;
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(next);
	}

	//tris along the bottom
	int bottomVertIndex = verts.size() - 1;
	for (int i = 1; i <= numBaseVerts; ++i)
	{
		int next = i + 1; if (next > numBaseVerts) next = 1;
		indices.push_back(bottomVertIndex);
		indices.push_back(next);
		indices.push_back(i);
	}

	
	std::vector<VertexData> vertices;
	int numVerts = (int)verts.size();

	for(int i = 0; i < numVerts; ++i)
	{
		VertexData vertexData;
		vertexData.texCoords = Vector2(0, 0);
		vertexData.position = verts[i];
		vertices.push_back(vertexData);
	}


	coneMesh->setVertices(vertices);	
	coneMaterial_->setDiffuseColor(Color(1, 0, 0, 1));
	coneMesh->addMeshPart(indices, material);
	coneMesh->init(gl_);
	return coneMesh;
}

void TranslateGizmo::update()
{
	origin_ = parentTransform_ * transform_->globalPivot();		
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
	coneLen_ = axisLenWorld * 0.4f;
	coneRad_ = coneLen_ * 0.3f;		
	xAxisTip_ = origin_ + axisLenWorld*Vector3(1, 0, 0);
	yAxisTip_ = origin_ + axisLenWorld*Vector3(0, 1, 0);
	zAxisTip_ = origin_ + axisLenWorld*Vector3(0, 0, 1);
}



void TranslateGizmo::mousePressEvent(QMouseEvent * event, bool transformableObjClicked)
{	
	if (!transform_) return;	

	

	const Camera * cam = glWidget_->camera();
	Vector2 mousePt = Vector2(event->posF().x(), event->posF().y());
	origMousePt_ = glWidget_->windowToDeviceCoords(mousePt);
	Vector2 originPt = glWidget_->deviceToWindowCoords(glWidget_->renderer()->project(origin_));
	float dist;

	mode_ = doHitTest(mousePt);
	
	Vector2 v = originPt - mousePt;
	dist = max(fabs(v.x), fabs(v.y));
	bool dragObj = mode_ == NONE && transformableObjClicked;
	if (dist < centerSquareSize_/2 || dragObj)
	{
		mode_ = CAMERA;
	}
	
	update();

	if (mode_ != NONE)
	{
		transformStarted();
		initTransform_ = *transform_;
	}
}

void TranslateGizmo::mouseReleaseEvent(QMouseEvent * event)
{
	if (mode_ != NONE) transformEnded();
	mode_ = NONE;
	
}

void TranslateGizmo::mouseDoublePressEvent(QMouseEvent * event)
{	
	if (mode_ != NONE) transformEnded();
	mode_ = NONE;

}

void TranslateGizmo::mouseMoveEvent(QMouseEvent * event)
{
	if (mode_ == NONE) return;

	const Camera * cam = glWidget_->camera();	
	Vector3 origMouseA, origMouseB, mouseA, mouseB;
	Vector2 mousePt(event->posF().x(), event->posF().y());
	mousePt = glWidget_->windowToDeviceCoords(mousePt);

	glWidget_->renderer()->unproject(origMousePt_, &origMouseA, &origMouseB);
	glWidget_->renderer()->unproject(mousePt, &mouseA, &mouseB);
	
	Vector3 axis(0, 0, 0);
	if (mode_ == X) axis = Vector3(1, 0, 0);
	if (mode_ == Y) axis = Vector3(0, 1, 0);
	if (mode_ == Z) axis = Vector3(0, 0, 1);

	if (axis.x != 0 || axis.y != 0 || axis.z != 0)
	{
		Vector3 planeNormal = makeNormalTo(origMouseB - origMouseA, axis);
		Plane plane = Plane::fromPointNormal(origin_, planeNormal);

		Vector3 pt, pt1, pt2;
		bool intersected = true;
		intersected &= plane.intersectLine(&pt1, origMouseA, origMouseB);
		intersected &= plane.intersectLine(&pt2, mouseA, mouseB);	
		if (!intersected) return;
		pt = pt2 - pt1;		
		Transform trans = initTransform_;
		Matrix invParent = parentTransform_.inverse();
		Vector3 v = trans.translation() + 
			invParent * ((pt * axis) * axis) - invParent * Vector3(0, 0, 0);

		trans.setTranslation(roundFloat(v.x), roundFloat(v.y), roundFloat(v.z));
		*transform_ = trans;	
	}
	else if (mode_ == CAMERA)
	{
		Vector3 pt, pt1, pt2;
		
		Plane plane = Plane::fromPointNormal(origin_, cam->lookAtPlane().normal());

		plane.intersectLine(&pt1, origMouseA, origMouseB);
		plane.intersectLine(&pt2, mouseA, mouseB);
		pt = pt2 - pt1;
		Transform trans = initTransform_;
		Matrix invParent = parentTransform_.inverse();
		Vector3 v = trans.translation() + 
			invParent * pt - invParent * Vector3(0, 0, 0);
		trans.setTranslation(roundFloat(v.x), roundFloat(v.y), roundFloat(v.z));
		*transform_ = trans;	
	}

	emit transformChanged();
}

bool TranslateGizmo::checkAxisNotTooFlush(const Vector3 & axis) const
{
	Vector3 eyeToGizmo = (origin_ - glWidget_->camera()->eye()).normalize();
	const float BIAS = 0.05f;
	if (fabs(eyeToGizmo * axis) > 1 - BIAS) return false;
	return true;

}

void TranslateGizmo::draw()
{
	if (!transform_) return;
	glClear(GL_DEPTH_BUFFER_BIT);

	int prevDepthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);
	glDepthFunc(GL_LEQUAL);
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
	gl_->useColorProgram();
	gl_->setColorProgramColor(0.75, 0.75, 0.75, 1);

	update();

	GLfloat vertices[] = {
		static_cast<GLfloat>(origin_.x), static_cast<GLfloat>(origin_.y), static_cast<GLfloat>(origin_.z),
		0, 0, 0};
	
	memcpy(&vertices[3], &xAxisTip_, sizeof(Vector3));
	if (mode_ == X || mode_ == CAMERA) gl_->setColorProgramColor(1, 1, 0, 1);
	else gl_->setColorProgramColor(1, 0, 0, 1);
	gl_->enableVertexAttribArrayPosition();
	gl_->bindArrayBuffer(0);
	gl_->vertexAttribPositionPointer(0, (char *)vertices);
	gl_->applyCurrentShaderMatrix();
	glDrawArrays(GL_LINES, 0, 2);
	coneMesh_->setTransformMatrix(
		Matrix::Translate(xAxisTip_.x, xAxisTip_.y, xAxisTip_.z) * 
		Matrix::Rotate(M_PI_2, 0, 0, -1) * 
		Matrix::Scale(coneRad_, coneLen_, coneRad_));	
	coneMesh_->drawBare(gl_);

	memcpy(&vertices[3], &yAxisTip_, sizeof(Vector3));
	if (mode_ == Y || mode_ == CAMERA) gl_->setColorProgramColor(1, 1, 0, 1);
	else gl_->setColorProgramColor(0, 1, 0, 1);
	gl_->enableVertexAttribArrayPosition();
	gl_->bindArrayBuffer(0);
	gl_->vertexAttribPositionPointer(0, (char *)vertices);
	gl_->applyCurrentShaderMatrix();
	glDrawArrays(GL_LINES, 0, 2);
	coneMesh_->setTransformMatrix(
		Matrix::Translate(yAxisTip_.x, yAxisTip_.y, yAxisTip_.z) * 		 
		Matrix::Scale(coneRad_, coneLen_, coneRad_));
	coneMesh_->drawBare(gl_);

	memcpy(&vertices[3], &zAxisTip_, sizeof(Vector3));	
	if (mode_ == Z || mode_ == CAMERA) gl_->setColorProgramColor(1, 1, 0, 1);
	else gl_->setColorProgramColor(0, 0, 1, 1);
	gl_->enableVertexAttribArrayPosition();
	gl_->bindArrayBuffer(0);
	gl_->vertexAttribPositionPointer(0, (char *)vertices);
	gl_->applyCurrentShaderMatrix();
	glDrawArrays(GL_LINES, 0, 2);
	coneMesh_->setTransformMatrix(
		Matrix::Translate(zAxisTip_.x, zAxisTip_.y, zAxisTip_.z) * 
		Matrix::Rotate(M_PI_2, 1, 0, 0) * 
		Matrix::Scale(coneRad_, coneLen_, coneRad_));
	coneMesh_->drawBare(gl_);		
	glDepthFunc(prevDepthFunc);
}


TranslateGizmo::Mode TranslateGizmo::doHitTest(const Vector2 & mousePos) const
{
	const Camera * cam = glWidget_->camera();
	Mode mode = NONE;
	if (!transform_) return mode;

	//check intersection with axes
	Vector2 originPt = glWidget_->deviceToWindowCoords(glWidget_->renderer()->project(origin_));
	Vector2 xAxisTipPt = glWidget_->deviceToWindowCoords(glWidget_->renderer()->project(xAxisTip_));
	Vector2 yAxisTipPt = glWidget_->deviceToWindowCoords(glWidget_->renderer()->project(yAxisTip_));
	Vector2 zAxisTipPt = glWidget_->deviceToWindowCoords(glWidget_->renderer()->project(zAxisTip_));
	float dist;
	Vector2 closestPt;

	dist = distancePointSegment(&closestPt, mousePos, originPt, xAxisTipPt);
	if (dist < grabLen_ && checkAxisNotTooFlush(Vector3(1, 0, 0)))
	{
		mode = X;
	}

	dist = distancePointSegment(&closestPt, mousePos, originPt, yAxisTipPt);
	if (dist < grabLen_ && checkAxisNotTooFlush(Vector3(0, 1, 0)))
	{
		mode = Y;
	}

	dist = distancePointSegment(&closestPt, mousePos, originPt, zAxisTipPt);
	if (dist < grabLen_ && checkAxisNotTooFlush(Vector3(0, 0, 1)))
	{
		mode = Z;
	}

	Vector3 intPt;
	Vector3 a, b;
	Vector2 devicePt = glWidget_->windowToDeviceCoords(mousePos);
	glWidget_->renderer()->unproject(devicePt, &a, &b);
	Vector3 invA, invB;

	Matrix invMatrix;

	coneMesh_->setTransformMatrix(
		Matrix::Translate(xAxisTip_.x, xAxisTip_.y, xAxisTip_.z) * 
		Matrix::Rotate(M_PI_2, 0, 0, -1) * 
		Matrix::Scale(coneRad_, coneLen_, coneRad_));
	invMatrix = coneMesh_->transform().inverse();
	invA = invMatrix * a; 
	invB = invMatrix * b;
	if (coneMesh_->intersectLine(&intPt, invA, invB)) mode = X;
	
	coneMesh_->setTransformMatrix(
		Matrix::Translate(yAxisTip_.x, yAxisTip_.y, yAxisTip_.z) * 		 
		Matrix::Scale(coneRad_, coneLen_, coneRad_));
	invMatrix = coneMesh_->transform().inverse();
	invA = invMatrix * a; 
	invB = invMatrix * b;
	if (coneMesh_->intersectLine(&intPt, invA, invB)) mode = Y;
	
	coneMesh_->setTransformMatrix(
		Matrix::Translate(zAxisTip_.x, zAxisTip_.y, zAxisTip_.z) * 
		Matrix::Rotate(M_PI_2, 1, 0, 0) * 
		Matrix::Scale(coneRad_, coneLen_, coneRad_));
	invMatrix = coneMesh_->transform().inverse();
	invA = invMatrix * a; 
	invB = invMatrix * b;
	if (coneMesh_->intersectLine(&intPt, invA, invB)) mode = Z;


	//check intersection with center square
	Vector2 v = originPt - mousePos;
	dist = max(fabs(v.x), fabs(v.y));
	if (dist < centerSquareSize_/2)
	{
		mode = CAMERA;
	}
	
	return mode;
}

bool TranslateGizmo::intersect(const QPoint & mousePos) const
{
	return doHitTest(Vector2(mousePos.x(), mousePos.y())) != NONE;
}