#pragma once
#include "TransformGizmo.h"
#include "MathStuff.h"
#include "Transform.h"

class VisualAttrib;
class Mesh;
class Material;
class TransformTool;

class TranslateGizmo : public TransformGizmo
{
public:
	enum Mode
	{
		X, Y, Z, CAMERA, NONE
	};
public:
	TranslateGizmo(GLWidget * gl, TransformTool * transformTool);
	~TranslateGizmo();

	virtual void init();
	virtual void uninit();

	virtual void mousePressEvent(QMouseEvent * event, bool transformableObjClicked);
	virtual void mouseDoublePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);

	virtual void draw();

	virtual bool intersect(const QPoint & mousePos) const;

	Mode mode() const {return mode_;}

	virtual bool isActive() const {return mode_ != NONE;}

private:

	Mode doHitTest(const Vector2 & mousePos) const;

	bool checkAxisNotTooFlush(const Vector3 & axis) const;

	Mesh * makeConeMesh(Material * material) const;

	/**
	calculate parameters transforms coordinates etc for gizmo display
	*/
	void update();
	

private:
	Mode mode_;

	Mesh * coneMesh_;
	Material * coneMaterial_;
	Transform initTransform_;
	float axisLenPixels_;
	float grabLen_;
	float centerSquareSize_;

	Vector3 origin_, xAxisTip_, yAxisTip_, zAxisTip_;
	float coneRad_, coneLen_;

	///Original click location in coords
	Vector2 origMousePt_;

};