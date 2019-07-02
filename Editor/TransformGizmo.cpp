#include "stdafx.h"
#include "TransformGizmo.h"
#include "TransformTool.h"
#include "GLWidget.h"

TransformGizmo::TransformGizmo(GLWidget * glWidget, TransformTool * transformTool)
{
	glWidget_ = glWidget;
	gl_ = glWidget->renderer();
	transform_ = 0;
	parentTransform_ = Matrix::Identity();
	transformTool_ = transformTool;
}

void TransformGizmo::transformStarted()
{
	transformTool_->onTransformStarted();
}

void TransformGizmo::transformEnded()
{
	transformTool_->onTransformEnded();
}

void TransformGizmo::transformChanged()
{
	transformTool_->onTransformChanged();
}

