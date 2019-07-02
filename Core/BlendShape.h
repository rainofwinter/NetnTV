#pragma once
#include "MathStuff.h"
#include "Mesh.h"
#include "ModelKeyFrameChannel.h"

class Reader;
class Writer;

class BlendShapeTarget
{	
  friend class BlendShapeWeightChannel;
public:
	BlendShapeTarget(const std::vector<VertexDataDelta> deltas) : deltas_(deltas)
	{}

	BlendShapeTarget() {}

	unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

private:	

	///number of elements must be equivalent to number of elements 
	///of deltaIndices in BlendShapeWeightChannel class
	std::vector<VertexDataDelta> deltas_;

};

class Mesh;

class BlendShapeWeightChannel
{
public:
	BlendShapeWeightChannel() {}
	~BlendShapeWeightChannel();

	const std::string & name() const {return name_;}
	void setName(const std::string & name) {name_ = name;}

	/**
	Actually deform the given vertex list based on this blendshape, use the 
	specified weight value
	*/
	void apply(std::vector<VertexData> * vertices, float weight);

	const std::vector<unsigned> & vertexIndices() const {return vertexIndices_;}

	void setVertexIndices(const std::vector<unsigned int> & vertexIndices)
	{
		vertexIndices_ = vertexIndices;
	}
	void addBlendShapeTarget(float weight, BlendShapeTarget * target)
	{
		weights_.push_back(weight);
		blendShapeTargets_.push_back(target);
	}

	/**
	Rearrange vertex indices based on the given old to new index map
	*/
	void rearrangeVertexIndices(const std::vector<unsigned> & oldToNewIndexMap);
  
	/**
	\param oldToNewIndices
	oldToNewIndices[i] corresponds to render set i
	for each oldToNewIndices[i], key is the old vertexData index, value is the
	new vertexData index. An old vertexData index may end up getting duplicated 
	(if it appears in both oldToNewIndices[i] and oldToNewIndices[j] where i != j
	)
	*/
	void rearrangeVertexIndices(
		const std::vector<std::map<unsigned, unsigned> > & oldToNewIndices);

	unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

private:

	std::string name_;


	///list of weights for this channel. Corresponds to # of inbetweens / targets
	std::vector<float> weights_;

	///list of mesh vertices that are affect by this channel.
	std::vector<unsigned> vertexIndices_;

	///the actual targets themselves. blendShapeTargets[i] corresponds to weights[i]
	std::vector<BlendShapeTarget *> blendShapeTargets_;

};

///////////////////////////////////////////////////////////////////////////////

class BlendShape
{	
public:	
	BlendShape() {}
	~BlendShape();

	void applyToMesh(std::vector<VertexData> * vertices);

	bool setTime(float time);

	/**
	get the indices of all mesh vertex data elements that are affected by this
	blendshape.
	Note: the function does NOT clear indices. (So if anything is already there,
	it is added on to)
	\param indices the indices are output into this set
	*/
	void getAffectedVertexIndices(
	std::set<unsigned> & indices) const;

	void setName(const std::string & name) {name_ = name;}
	const std::string & name() const {return name_;}


	///Adds a weightchannel to this blendshape.
	void addWeightChannel(BlendShapeWeightChannel * channel);

	void addWeightAnimationChannel(ModelKeyFrameChannel<float> * animChannel)
	{
		weightAnimationChannels_.push_back(animChannel);
	}
	
	///return the total number of weightChannels in this blendShape
	int GetNumWeightChannels() const 
	{return (int)weightChannels.size();}

	///return the weightChannel at the specified index
	BlendShapeWeightChannel & GetWeightChannel(
	unsigned index) const;

	unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

private:

  std::string name_;
  std::vector<BlendShapeWeightChannel *> weightChannels;  
  std::vector< ModelKeyFrameChannel<float> * > weightAnimationChannels_;
  
};
