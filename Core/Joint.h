#pragma once	
#include "MathStuff.h"
#include "ModelKeyFrameChannel.h"

class Reader;
class Writer;

class Joint
{
	friend class Model;
public:
	Joint();
	~Joint();
	
	void setName(const std::string & name) {name_ = name;}
	const std::string & name() const {return name_;}

	void setParent(int parent) {parent_ = parent;}
	int parent() const {return parent_;}
	bool hasParent() const {return parent_ >= 0;}

	void setTransform(const Matrix & transform) {transform_ = transform;}
	const Matrix & transform() const {return transform_;}

	const Matrix & globalTransform() const {return globalTransform_;}

	std::vector<int> & children() {return children_;}
	
	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
private:
	
	void setAnimationChannel(ModelKeyFrameChannel<Matrix> * channel)
	{
		animationChannel_ = channel;
	}

	ModelKeyFrameChannel<Matrix> * animationChannel() const 
	{
		return animationChannel_;
	}

	void setGlobalTransform(const Matrix & transform)
	{
		globalTransform_ = transform;
	}

	Matrix * localTransform() const {return localTransform_;}
	void setLocalTransform(const Matrix & localTransform);

	/**
	certain data should be recomputed after Joint gets copied
	(animationChannel_ which is a pointer reference not owned by this object)
	*/
	Joint(const Joint & rhs);
	Joint & operator = (const Joint & rhs);
private:
	std::string name_;

	/**
	-1 indicates no parent
	*/
	int parent_;
	
	/**
	local transform of joint
	*/
	Matrix transform_;

	mutable Matrix globalTransform_;

	//The following fields are computed after the fact and not actually stored 
	//in the model file

	std::vector<int> children_;
	ModelKeyFrameChannel<Matrix> * animationChannel_;
	Matrix * localTransform_;	
};