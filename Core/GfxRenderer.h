#pragma once 
#include "MathStuff.h"
#include "Types.h"

#ifdef OSX
#include "TextFont.h"
#endif

#ifdef DX
#include <d3d9.h>
#include <d3dx9.h>
class HWND;
#endif
const GLuint AttribPosition = 0;
const GLuint AttribTexCoord = 1;
const GLuint AttribNormal = 2;
const GLuint AttribTangent = 3;
const GLuint AttribBinormalFactor = 4;
class Matrix;
class MatrixStack;
class Texture;
class TextHandler;
class TextHandlerOld;
class TextFont;
class SceneObject;
class TextureRenderer;
class LightObject;
class Material;
class Color;

struct StencilState
{
	GLint func, ref, mask;
	int stencilLevel;
	GLint sfail, dpfail, dppass;
};

struct BlendState
{
	GLint srcRgb, dstRgb, srcA, dstA;
};

class GfxShader;
class GfxTextureProgram;
class GfxTextureAlphaProgram;
class GfxShowMaskedProgram;
class GfxColorProgram;
class GfxGradationProgram;
class GfxHorizontalGaussianProgram;
class GfxVerticalGaussianProgram;
class GfxTextProgram;
class GfxMeshProgram;
class GfxLoadingProgram;
class GfxSingleShader;

class GfxHBlurProgram;
class GfxVBlurProgram;
class GfxConvolutionProgram;

enum ConvolutionType{
	Sharpness = 0, GausBlur, EdgeDetect, Emboss
};

class GfxRenderer
{
public:
	enum GradDir { R, RU, U, LU, L, LD, D, RD };
	GfxRenderer(int width, int height);
	virtual ~GfxRenderer();

	void init(bool isMobile);
	void uninit();

	void initShaders(bool isMobile);
	void uninitShaders();
	void uninitMaskShaders();

	void beginDraw();
	void endDraw();

	void useTextureProgram();
	void setTextureProgramOpacity(float opacity);
	void setTextureProgramFactorST(float fs, float ft);

	void useTextureAlphaProgram();	
	void setTextureAlphaProgramOpacity(float opacity);
	void setTextureAlphaProgramFactorST(float fs, float ft);
	
	/**
	customProgram must have valid locMvpMatrix
	*/
	void useCustomProgram(GfxShader * customProgram);

	/**
	deprecated
	*/
	void useCustomProgram(GLuint program, GLuint locMvpMatrix);

	void useColorProgram();
	void setColorProgramColor(float r, float g, float b, float a);
	void useGradationProgram() ;
	void useHorizontalGaussianProgram();
	void useVerticalGaussianProgram();

	void useLoadingProgram();
	void setLoadingProgramAngle(float);
	void setLoadingProgramDim(float);
	
	void useTextProgram();
	void setTextProgramColor(float r, float g, float b, float a);
	void setTextProgramOpacity(float opacity);

	void useHBlurProgram();
	void useVBlurProgram();
	void setHBlurProgramOpacity(float opacity);
	void setVBlurProgramOpacity(float opacity);
	void setHBlurProgramShadowSize(float shadow);
	void setVBlurProgramShadowSize(float shadow);

	void useConvProgram();
	void setConvProgramType(int type);
	void setConvProgramSize(float size);
	void setConvProgramOpacity(float opacity);

	void setNeedAlphaBlending(bool val);

	void useMeshProgram(float opacity, const Material * material);
	void initMeshProgramLights(const Vector3 & camDir, float ambient, const std::vector<LightObject *> & lights);
	
	void setCurTexRenderer(TextureRenderer * texRenderer);

	void setCameraMatrix(const Matrix & matrix);
	const Matrix  & cameraMatrix() const {return *cameraMatrix_;}
	void pushMatrix();
	void preMultMatrix(const Matrix & matrix);
	void multMatrix(const Matrix & matrix);
	void loadMatrix(const Matrix & matrix);
	void popMatrix();
	const Matrix & modelMatrix() const;
	void applyCurrentShaderMatrix();

	void drawRect(float width, float height, const Matrix & transform);
	void drawRect(float x, float y, float width, float height);

	void drawBox(const Vector3 & minCorner, const Vector3 & maxCorner);

	void drawBoundary(float x, float y, float width, float height);

	void enableVertexAttribArrayTexCoord();
	void disableVertexAttribArrayTexCoord();
	void enableVertexAttribArrayPosition();
	void disableVertexAttribArrayPosition();
	void vertexAttribPositionPointer(int stride, char * ptr);
	void vertexAttribTexCoordPointer(int stride, char * ptr);

	
	void resetStencil();
	void beginIncStencilDrawing();
	void beginDecStencilDrawing();
	void endStencilDrawing();

	void pushStencilState();
	void popStencilState();

	void pushBlendState();
	void popBlendState();
	
	void clearZBuffer();

	void use(Texture * texture);

	void unproject(const Vector2 & projCoords, Vector3 * a, Vector3 * b) const;

	Ray unproject(const Vector2 & coords) const;
	Vector3 project(const Vector3 & worldCoord) const;
	
	void setMaskSize(int width, int height);


	void beginMaskDrawing();
	void endMaskDrawing();

	void beginMaskedDrawing();
	void endMaskedDrawing();

	/**
	project coords ( (-1, 1) - (1, -1) )
	*/
	//void showMasked(float x0, float y0, float x1, float y1);

	void registerMaskObject(SceneObject * obj);
	void unregisterMaskObject(SceneObject * obj);
	void unregisterAllMaskObjects();

	void setDataForGaussianProgram(GLuint texture, float width, float height);
	void makeGradationTexture(Texture *texture, Color color, Color secondaryColor, GradDir gradDir, float gradStop);
	void makeDashedTexture();
	void makeShadowTexture(Texture *texture, Color color, const std::vector<Vector3> vertices_, std::vector<unsigned short> indices_);
	GLuint dashedTexture(int i) { return (i<4 && i >= 0)? dashedTexture_[i] : dashedTexture_[0];}
	
	GLuint lineColorTexture(){return lineColorTexture_;}
	GLuint shadowFBO(){return shadowFBO_;};
	GLuint shapeFBO(){return shapeFBO_;}

#ifndef DX
	void use(GLuint texture, GLuint textureAlpha = 0);

	void bindElementArrayBuffer(GLuint buffer);
	void bindArrayBuffer(GLuint buffer);

	GLuint defaultDiffuseMap() const {return defaultDiffuseMap_;}
	GLuint defaultPhongSpecularMap() const {return defaultSpecularMap_;}
	GLuint defaultLambertSpecularMap() const {return defaultLambertSpecularMap_;}
	GLuint defaultNormalMap() const {return defaultNormalMap_;}
#endif

	Matrix computeMvpMatrix() const;

	TextHandler * textHandler(const TextFont & font);
	TextHandlerOld * textHandlerOld(const TextFont & font);

	void resizeText();

private:
	void destroyTextData();	
	
	void handleMaskInit(int depth);

private:
	Matrix * cameraMatrix_;
	MatrixStack * matrixStack_;

	int curStencilLevel_;

#ifndef DX
	GfxShader * curProgram_;
		

	boost::scoped_ptr<GfxTextureProgram> texProgram_;
	boost::scoped_ptr<GfxTextureAlphaProgram> texAlphaProgram_;
	boost::scoped_ptr<GfxColorProgram> colorProgram_;
	boost::scoped_ptr<GfxGradationProgram> gradationProgram_;
	boost::scoped_ptr<GfxHorizontalGaussianProgram> hGaussianProgram_;
	boost::scoped_ptr<GfxVerticalGaussianProgram> vGaussianProgram_;
	boost::scoped_ptr<GfxTextProgram> textProgram_;
	boost::scoped_ptr<GfxMeshProgram> meshProgram_;
	boost::scoped_ptr<GfxLoadingProgram> loadingProgram_;

	boost::scoped_ptr<GfxVBlurProgram> vBlurProgram_;
	boost::scoped_ptr<GfxHBlurProgram> hBlurProgram_;
	boost::scoped_ptr<GfxConvolutionProgram> convProgram_;
	
	GLuint rectVertexBuffer_, rectIndexBuffer_;
	GLuint gradationFBO_;



	GLuint defaultDiffuseMap_, defaultSpecularMap_, defaultNormalMap_, defaultLambertSpecularMap_;
#endif

	bool init_;


	float curSceneScaleX_;
	float curSceneScaleY_;


	std::map<TextFont, TextHandler *> fontData_;
	std::map<TextFont, TextHandlerOld *> fontData_old_;

	std::stack<StencilState> stencilStateStack_;
	std::stack<BlendState> blendStateStack_;
#ifdef DX
	LPDIRECT3D9       pd3d_;
	LPDIRECT3DDEVICE9 pd3dDevice_;

	LPD3DXEFFECT      pd3dShader_;

	HWND hwnd_;
	float displayWidth_, displayHeight_;

	int curPass_;
	typedef enum ShaderPass_
	{
		COLOR = 0,
		TEXTURE = 1,
		LOADING = 2,
		TEXT = 3,
		HBLUR = 4,
		VBLUR = 5,
	} ShaderPass;
#endif
	std::vector<TextureSPtr> maskBuffers_;
	std::vector<TextureRendererSPtr> maskBufferRenderers_;
	int curMaskBufferDepth_;
	std::set<SceneObject *> maskBufferUsers_;	
	std::stack<int> useMaskStack_;
	std::stack<int> drawingMaskStack_;
	int maskWidth_, maskHeight_;

	bool isMobile_;

	boost::scoped_ptr<GfxSingleShader> shaderWrapper_;

	TextureRenderer * curTexRenderer_;
	GLuint dashedTexture_[4];

	GLuint lineColorTexture_;
	GLuint shadowFBO_;
	GLuint shapeFBO_;
};