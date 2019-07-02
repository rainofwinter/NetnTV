#pragma once
#include "MathStuff.h"
#include "TempRenderObject.h"
#include "Camera.h"
#include "PageFlipInterface.h"

class GfxRenderer;
class Document;
class SceneObject;
class TextureRenderer;
class Texture;


class Vector2Compare
{
public:
	bool operator()(const Vector2 & lhs, const Vector2 & rhs) const
	{
		if (lhs.x != rhs.x) return lhs.x < rhs.x;
		return lhs.y < rhs.y;
	}
};

class Vector3Compare
{
public:
	bool operator()(const Vector3 & lhs, const Vector3 & rhs) const
	{
		if (lhs.x != rhs.x) return lhs.x < rhs.x;
		if (lhs.y != rhs.y) return lhs.y < rhs.y;
		return lhs.z < rhs.z;
	}
};


///////////////////////////////////////////////////////////////////////////////

class PageFlip : public PageFlipInterface
{
public:

	void setTexture(Texture * texture);
	void setGlTexture(GLuint tetxure);

	void init(GfxRenderer * gl);
	void uninit();
	void draw(GfxRenderer * gl);
	bool update();

	void animate(TransitionDirection direction);
	void reset();

	GLuint texture() const {return texture_;}

	void pressEvent(const Vector2 & startPos, TransitionDirection pageFlipDir);
	void releaseEvent(const Vector2 & pos);
	void moveEvent(const Vector2 & pos);
	
	bool doPageChange() const;

	TransitionDirection curTransitionDirection() const;

	PageFlip();
	~PageFlip();
private:
	Vector2 convCoords(const Vector2 & pos);
	/**
	rollingDir must be a unit vector, and point left....	
	*/
	void computeVerts(const Vector2 & rollingPos, const Vector2 & rollingDir);

	bool computeIntPts(const Vector2 & r, const Vector2 & axis, Vector2 * intPt1Out, Vector2 * intPt2Out) const;

	void cutPoly(const std::vector<Vector2> & origVerts, const Vector2 & intPt1, const Vector2 & intPt2, std::vector<Vector2> * outVerts1, std::vector<Vector2> * outVerts2) const;
	void cutPolyOnSide(const std::vector<Vector2> & origVerts, const Vector2 & intPt1, const Vector2 & intPt2, float side, std::vector<Vector2> * outVerts) const;
	void insertVert(const Vector2 & intPt, std::vector<Vector2> * origVerts) const;

	void makeMesh(const Vector2 & r, const Vector2 & axis, float c1, float r1, float t1, float c2, float r2, float t2);

private:
	GfxRenderer * gl_;

	Camera camera_;
	Matrix cameraMatrix_;

	struct Vertex
	{
		Vector3 position;
		Vector2 texCoord;
		Vector3 normal;
	};

	std::vector<Vertex> vertices_;
	std::vector<GLushort> indices_;


	GLuint indexBuffer_;
	GLuint vertexBuffer_;

	bool isPressed_;
	Vector2 pressedPos_;
	float pressedTime_;
	Vector2 pressDir_;

	float t_;
	float tStep_;
	Vector2 targetR_;
	Vector2 curR_;
	Vector2 startR_;
	Vector2 targetDir_;
	Vector2 curDir_;
	Vector2 startDir_;
	float prevTime_;
	bool needComputeVertex_;

	GLuint texture_;
	GLuint blankTexture_;

	float lightT_;

	TransitionDirection transitionDirection_;
	
	mutable bool doPageChange_;

	GLuint programFront_, programFrontV_;
	GLint locFrontMVPMatrix_, locLightT_;

	GLuint programBack_;
	GLint locBackMVPMatrix_, locBackR_, locBackEdge_, locBackCurlAmt_;

	GLuint programHBlur_;
	GLint locHBlurMVPMatrix_, locHBlurSize_;

	GLuint programVBlur_;
	GLint locVBlurMVPMatrix_, locVBlurSize_;

	GLuint programDepth_;
	GLint locDepthMVP_, locDepthLightMVP_;
		
	GLuint programWriteDepth_;
	GLint locWriteDepthMVPMatrix_;

	int segments_;

	int renderTexDim_;
	Texture * renderTex1_, * renderTex2_, * shadowTex_;
	TextureRenderer * texRenderer1_, * texRenderer2_, * shadowRenderer_;

	Camera lightCam_;
};
