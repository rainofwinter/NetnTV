#include "stdafx.h"
#include "Joint.h"
#include "Reader.h"
#include "Writer.h"

Joint::Joint()
{
	localTransform_ = NULL;
}

Joint::~Joint()
{
	delete localTransform_;
}

Joint::Joint(const Joint & rhs)
{
	*this = rhs;
}

Joint & Joint::operator = (const Joint & rhs)
{
	name_ = rhs.name_;
	parent_ = rhs.parent_;
	transform_ = rhs.transform_;
	globalTransform_ = rhs.globalTransform_;
	children_ = rhs.children_;

	//This field should be recomputed to the correct reference
	animationChannel_ = rhs.animationChannel_;

	if (rhs.localTransform_)
	{
		localTransform_ = new Matrix(*rhs.localTransform_);
	}
	else
		localTransform_ = NULL;

	return *this;
}

void Joint::setLocalTransform(const Matrix & localTransform)
{
	localTransform_ = new Matrix(localTransform);
}


void Joint::write(Writer & writer) const
{
	writer.write(name_);
	writer.write(parent_);
	writer.write(transform_);
}

void Joint::read(Reader & reader, unsigned char version)
{
	reader.read(name_);	
	reader.read(parent_);
	reader.read(transform_);
}