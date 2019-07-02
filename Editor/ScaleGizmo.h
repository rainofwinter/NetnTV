#pragma once
#include "TransformGizmo.h"
#include "MathStuff.h"
#include "Transform.h"

class VisualAttrib;
class Mesh;
class Material;
class TransformTool;

class ScaleGizmo : public TransformGizmo
{
public:
	enum Mode
	{
		X, Y, Z, ALL, NONE
	};
public:
	ScaleGizmo(GLWidget * gl, TransformTool * transformTool_);
	~ScaleGizmo();

	virtual void init();
	virtual void uninit();

	virtual void mousePressEvent(QMouseEvent * event, bool transformableObjClicked);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void mouseDoublePressEvent(QMouseEvent * event);

	virtual void draw();

	Mode mode() const {return mode_;}

	virtual bool isActive() const {return mode_ != NONE;}
	virtual bool intersect(const QPoint & mousePos) const;

	virtual void setParentTransform(const Matrix & parentTransform);
private:

	Mode doHitTest(const Vector2 & mousePos) const;

	bool checkAxisNotTooFlush(const Vector3 & axis) const;

	Mesh * makeCubeMesh(Material * material) const;

	/**
	calculate parameters transforms coordinates etc for gizmo display
	*/
	void update();
	

private:
	Mode mode_;

	Mesh * cubeMesh_;
	Material * gizmoMaterial_;
	Transform initTransform_;
	float axisLenPixels_;
	float grabLen_;
	float centerSquareSize_;

	Vector3 origin_, xAxisTip_, yAxisTip_, zAxisTip_, cAxisTip_;;
	Vector3 xAxis_, yAxis_, zAxis_, cAxis_;
	float cubeLen_;

	///Original click location in coords
	Vector2 origMousePt_;

	Matrix parentRotMatrix_;
};