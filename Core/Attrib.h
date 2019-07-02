#pragma once
#include "Transform.h"

class Reader;
class Writer;
class SceneObject;
class XmlReader;
class XmlWriter;

class VisualAttrib
{
public:
	VisualAttrib();

	virtual const Transform & transform() const 
	{
		onGetTransform();
		return transform_;
	}

	void setTransform(const Transform & transform) 
	{
		transform_ = transform;
		transformMatrix_ = transform.computeMatrix();
		onSetTransform();
	}

	void setTransformMatrix(const Matrix & matrix)
	{
		transform_.setMatrix(matrix);
		transformMatrix_ = matrix;
		onSetTransform();
	}

	const Matrix & transformMatrix() const {return transformMatrix_;}

	const float & opacity() const {return opacity_;}
	void setOpacity(const float & opacity) {opacity_ = opacity;}

	const bool & isVisible() const {return visible_;}
	void setVisible(const bool & visible) {visible_ = visible;}

	unsigned char version() const {return 0;}
	void write(Writer & writer) const;
	void read(Reader & reader, unsigned char);

	void writeXml(XmlWriter & w) const;
	void readXml(XmlReader & r, xmlNode * node);

	virtual void onSetTransform() {}
	virtual void onGetTransform() const {}
	void triggerOnSetTransform(SceneObject * sceneObject);
	
protected:
	Transform transform_;
	float opacity_;
	bool visible_;
	///cached matrix representation of the transform
	Matrix transformMatrix_;
};

