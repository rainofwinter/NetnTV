#include "stdafx.h"
#include "MatrixStack.h"

MatrixStack::MatrixStack()
{
	stack_.push(Matrix::Identity());
}

void MatrixStack::multMatrix(const Matrix & matrix)
{
	stack_.top() *= matrix;
}

void MatrixStack::preMultMatrix(const Matrix & matrix)
{
	stack_.top() = matrix * stack_.top();
}

void MatrixStack::loadMatrix(const Matrix & matrix)
{
	stack_.top() = matrix;
}

void MatrixStack::pushMatrix()
{
	stack_.push(stack_.top());
}

void MatrixStack::popMatrix()
{
	stack_.pop();
}

const Matrix & MatrixStack::matrix() const
{
	return stack_.top();
}