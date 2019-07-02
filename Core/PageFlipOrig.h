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

class PageFlipOrig : public PageFlipInterface
{
public:

	void setPageDims(float w, float h);

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

	PageFlipOrig();
	~PageFlipOrig();
private:
	Vector2 convCoords(const Vector2 & pos);

	/**
	rollingDir must be a unit vector, and point left....	
	*/
	void computeVerts(const Vector2 & rollingPos, const Vector2 & rollingDir);

	bool computeIntPts(const Vector2 & r, const Vector2 & axis, Vector2 * intPt1Out, Vector2 * intPt2Out) const;

	float computeEdgeX2(const Vector2 & rollingPos, const Vector2 & rollingDir, float height) const;
	void computeEdgeVec(const Vector2 & pRollingPos, const Vector2 & pRollingDir, Vector2 * edgeTop, Vector2 * edgeBottom) const;
	/**
	@param height in actual screen coords
	@param edgeX in actual screen coords
	*/
	Vector2 solveForRx2(const Vector2 & rollingDir, float height, float edgeX) const;


	void cutPoly(const std::vector<Vector2> & origVerts, const Vector2 & intPt1, const Vector2 & intPt2, std::vector<Vector2> * outVerts1, std::vector<Vector2> * outVerts2) const;
	void cutPolyOnSide(const std::vector<Vector2> & origVerts, const Vector2 & intPt1, const Vector2 & intPt2, float side, std::vector<Vector2> * outVerts) const;
	void insertVert(const Vector2 & intPt, std::vector<Vector2> * origVerts) const;

	void makeMesh(const Vector2 & r, const Vector2 & axis, float bottomRadius, float topRadius);

	void computeShadowPolys();

	Vector2 convertUnitPos(const Vector2 & pos) const;
	Vector2 convertUnitVec(const Vector2 & vec) const;
	void computeWH();
private:
	GfxRenderer * gl_;

	Camera camera_;
	Matrix cameraMatrix_;

	struct Vertex
	{
		Vector3 position;
		Vector2 texCoord;
	};

	std::vector<Vertex> vertices_;
	std::vector<GLushort> indices_;
/*
	std::vector<Vertex> frontShadeVertices_;
	std::vector<GLushort> frontShadeIndices_;
	std::vector<Vertex> backShadeVertices_;
	std::vector<GLushort> backShadeIndices_;
	*/
	std::vector<Vertex> bottomShadeVertices_;
	std::vector<GLushort> bottomShadeIndices_;


	Vector3 shadowSrc_;	
	std::vector<Vertex> shadowVertices_;
	std::vector<GLushort> shadowIndices_;

	GLuint indexBuffer_;
	GLuint vertexBuffer_;
	GLuint shadowVertexBuffer_;
	GLuint shadowIndexBuffer_;

	GLuint bottomShadeIndexBuffer_;
	GLuint bottomShadeVertexBuffer_;

	float rRadius_;	

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
	float curCurlAmt_;
	Vector2 curEdgeVec_;
	Vector2 curEdgeTop_, curEdgeBottom_;
	float prevTime_;
	bool needComputeVertex_;

	GLuint texture_;
	GLuint blankTexture_;

	TransitionDirection transitionDirection_;
	
	mutable bool doPageChange_;

	GLuint programFront_, programFrontV_;
	GLint locFrontMVPMatrix_, locFrontR_, locFrontEdge_, locFrontCurlAmt_;

	GLuint programBack_, programBackV_;
	GLint locBackMVPMatrix_, locBackR_, locBackEdge_, locBackCurlAmt_;

	GLuint programBottom_, programBottomV_;
	GLint locBottomMVPMatrix_, locBottomR_, locBottomEdge_;

	GLuint programHBlur_;
	GLint locHBlurMVPMatrix_, locHBlurSize_;

	GLuint programVBlur_;
	GLint locVBlurMVPMatrix_, locVBlurSize_;

	int segments_, dropShadowSegments_;

	int renderTexDim_;
	Texture * renderTex1_, * renderTex2_;
	TextureRenderer * texRenderer1_, * texRenderer2_;

	float width_, height_;
	float docW_, docH_;
};
