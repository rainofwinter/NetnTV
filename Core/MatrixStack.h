#ifndef MatrixStack_h__
#define MatrixStack_h__
#include "MathStuff.h"

class MatrixStack
{
public:
	MatrixStack();
	void multMatrix(const Matrix & matrix);
	void preMultMatrix(const Matrix & matrix);
	void loadMatrix(const Matrix & matrix);
	void pushMatrix();
	void popMatrix();
	const Matrix & matrix() const;
private:
	std::stack<Matrix> stack_;
};

#endif // MatrixStack_h__