#include "stdafx.h"
#include "BlendShape.h"
#include "Mesh.h"
#include "Reader.h"
#include "Writer.h"

using namespace boost;
using std::vector;
using std::map;


void BlendShapeTarget::write(Writer & writer) const
{	
	writer.write(deltas_);
}
void BlendShapeTarget::read(Reader & reader, unsigned char version)
{
	reader.read(deltas_);
}


////////////////////////////////////////////////////////////////////////////////

BlendShapeWeightChannel::~BlendShapeWeightChannel()
{
	BOOST_FOREACH(BlendShapeTarget * target, blendShapeTargets_)
		delete target;
	blendShapeTargets_.clear();
}

void BlendShapeWeightChannel::
rearrangeVertexIndices(const std::vector<unsigned> & oldToNewIndexMap)
{
	for (unsigned i = 0; i < vertexIndices_.size(); ++i)
	{
		vertexIndices_[i] = oldToNewIndexMap[vertexIndices_[i]];
	}
}

//-----------------------------------------------------------------------------

void BlendShapeWeightChannel::rearrangeVertexIndices(
	const std::vector<std::map<unsigned, unsigned> > & oldToNewIndices)
{
	//new deltaIndices for this weightChannel
	vector<unsigned> newDeltaIndices;

	//new deltas for each blendShapeTarget
	vector<vector<VertexDataDelta> > newDeltas(blendShapeTargets_.size());


	for (unsigned i = 0; i < vertexIndices_.size(); ++i)
	{
		unsigned oldIndex = vertexIndices_[i];

		for (unsigned j = 0; j < oldToNewIndices.size(); ++j)
		{
			map<unsigned, unsigned>::const_iterator iter;
			iter = oldToNewIndices[j].find(oldIndex);

			//if oldIndex not found... 
			if(iter == oldToNewIndices[j].end())continue;

			unsigned newIndex = (*iter).second;

			newDeltaIndices.push_back(newIndex);     
			for (unsigned k = 0; k < blendShapeTargets_.size(); ++k)
			{
				newDeltas[k].push_back(blendShapeTargets_[k]->deltas_[i]);
			}

		}    
	}

	vertexIndices_.swap(newDeltaIndices);
	for (unsigned i = 0; i < blendShapeTargets_.size(); ++i)
	{
		blendShapeTargets_[i]->deltas_.swap(newDeltas[i]);
	}

  
}

//-----------------------------------------------------------------------------

void BlendShapeWeightChannel::apply(
	std::vector<VertexData> * vertices, float weight)
{
	unsigned endWeightIndex = 0;
	if (weight == 0) return;

	while(endWeightIndex < weights_.size() - 1 && weight > weights_[endWeightIndex])
		++endWeightIndex;

	if (endWeightIndex == 0) //if no inbetweens (or weight < 0)
	{
		float t = weight/weights_[0];
		const vector<VertexDataDelta> & deltas = blendShapeTargets_[0]->deltas_;

		for (int i = 0; i < (int)vertexIndices_.size(); ++i)		
			(*vertices)[vertexIndices_[i]] += t*deltas[i];
		
	}
	else
	{
		float t0 = weights_[endWeightIndex-1];
		float t1 = weights_[endWeightIndex];
		float t = (weight - t0)/(t1 - t0);

		const vector<VertexDataDelta> & deltas0 = 
			blendShapeTargets_[endWeightIndex-1]->deltas_;
		const vector<VertexDataDelta> & deltas1 = 
			blendShapeTargets_[endWeightIndex]->deltas_;
		
		for (int i = 0; i < (int)vertexIndices_.size(); ++i)		
			(*vertices)[vertexIndices_[i]] += 
				deltas0[i] + t * (deltas1[i] - deltas0[i]);		
	}

}

void BlendShapeWeightChannel::write(Writer & writer) const
{	
	writer.write(name_);
	writer.write(weights_);
	writer.write(vertexIndices_);
	writer.write(blendShapeTargets_);
}
void BlendShapeWeightChannel::read(Reader & reader, unsigned char version)
{
	reader.read(name_);
	reader.read(weights_);
	reader.read(vertexIndices_);
	reader.read(blendShapeTargets_);
}

///////////////////////////////////////////////////////////////////////////////

BlendShape::~BlendShape()
{
	BOOST_FOREACH(BlendShapeWeightChannel * channel, weightChannels)
		delete channel;
	weightChannels.clear();

	BOOST_FOREACH(ModelKeyFrameChannel<float> * channel, weightAnimationChannels_)
		delete channel;
	weightAnimationChannels_.clear();
}

bool BlendShape::setTime(float time)
{
	bool ret = false;
	BOOST_FOREACH(ModelKeyFrameChannel<float> * channel, weightAnimationChannels_)
		ret |= channel->setTime(time);	

	return ret;
}	

/**
Actually deform the mesh geometry based on this blendshape
*/
void BlendShape::applyToMesh(std::vector<VertexData> * vertices)
{	
	BOOST_FOREACH(ModelKeyFrameChannel<float> * animChannel, weightAnimationChannels_)
	{
		weightChannels[animChannel->targetObjectIndex()]->apply(
			vertices, animChannel->value());
	}
	
}

void BlendShape::getAffectedVertexIndices(std::set<unsigned> & indices) const
{
  for (unsigned i = 0; i < weightChannels.size(); ++i)
  {
    const std::vector<unsigned> & vertexIndices = 
      weightChannels[i]->vertexIndices();

    indices.insert(vertexIndices.begin(), vertexIndices.end());
  }
}
//-----------------------------------------------------------------------------

void BlendShape::addWeightChannel(BlendShapeWeightChannel * channel)
{
  //make sure to grow vector only as much as necessary (by 1)
  weightChannels.reserve(weightChannels.capacity() + 1);
  weightChannels.push_back(channel);
}

BlendShapeWeightChannel & 
BlendShape::GetWeightChannel(unsigned index) const
{
  return *weightChannels[index];
}



void BlendShape::write(Writer & writer) const
{	
	writer.write(name_);
	writer.write(weightChannels);
	writer.write(weightAnimationChannels_);
}
void BlendShape::read(Reader & reader, unsigned char version)
{
	reader.read(name_);
	reader.read(weightChannels);
	reader.read(weightAnimationChannels_);
}