#include "stdafx.h"
#include "GfxRenderer.h"
#include "MathStuff.h"
#include "MatrixStack.h"
#include "Texture.h"
#include "GLUtils.h"
#include "GfxShaders.h"
#include "TextHandler.h"
#include "TextHandlerOld.h"
#include "LightObject.h"
#include "TextureRenderer.h"
#include "Exception.h"
#include "Color.h"

#if defined(ANDROID)
#include <android/log.h>
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE  , "libplayercore", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , "libplayercore", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , "libplayercore", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , "libplayercore", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "libplayercore", __VA_ARGS__)
void gfxLog(const std::string & str)
{
	//LOGI(str.c_str());
	LOGI("%s", str.c_str());
}
#else
void gfxLog(const std::string & str)
{
	std::cout << str << "\n";
}
#endif



///////////////////////////////////////////////////////////////////////////////
GfxRenderer::GfxRenderer(int width, int height)
{
	curTexRenderer_ = NULL;
	curMaskBufferDepth_ = 0;
	isMobile_ = false;
	maskWidth_ = width;
	maskHeight_ = height;
	matrixStack_ = new MatrixStack;
	cameraMatrix_ = new Matrix;
	*cameraMatrix_ = Matrix::Identity();
	curStencilLevel_ = 0;
	init_ = false;
	curSceneScaleX_ = 1;
	curSceneScaleY_ = 1;

	defaultDiffuseMap_ = defaultSpecularMap_ = defaultNormalMap_ = defaultLambertSpecularMap_ = 0;
#ifdef DX
	pd3d_ = NULL;
	pd3dDevice_ = NULL;
	pd3dShader_ = NULL;

	hwnd_ = NULL;
	displayWidth_ = 1024;
	displayHeight_ = 768;

	curPass_ = 0;
#endif
}

GfxRenderer::~GfxRenderer()
{
	uninit();
	delete matrixStack_;
	delete cameraMatrix_;
}

void GfxRenderer::uninit()
{
	if (!init_) return;
	init_ = false;
#ifdef DX
	if (pd3dShader_)
	{
		pd3dShader_->Release();
		pd3dShader_ = NULL;
	}

	if (pd3dDevice_)
	{
		pd3dDevice_->Release();
		pd3dDevice_ = NULL;
	}

	if (pd3d_)
	{
		pd3d_->Release();
		pd3d_ = NULL;
	}
#else	// GL 2.0
	uninitShaders();


	glDeleteBuffers(1, &rectVertexBuffer_);
	glDeleteBuffers(1, &rectIndexBuffer_);
	
	glDeleteTextures(1, &defaultDiffuseMap_);
	glDeleteTextures(1, &defaultSpecularMap_);
	glDeleteTextures(1, &defaultLambertSpecularMap_);
	glDeleteTextures(1, &defaultNormalMap_);
#endif	// GL 2.0 end

	destroyTextData();

	unregisterAllMaskObjects();
	handleMaskInit(1);
	glDeleteTextures(4, dashedTexture_);
	glDeleteFramebuffers(1, &gradationFBO_);
	curTexRenderer_ = NULL;
	glDeleteFramebuffers(1, &shadowFBO_);
	glDeleteFramebuffers(1, &shapeFBO_);
	glDeleteTextures(1, &lineColorTexture_);

}

void GfxRenderer::uninitShaders()
{
	texProgram_->uninit();
	texAlphaProgram_->uninit();
	colorProgram_->uninit();
	gradationProgram_->uninit();
	hGaussianProgram_->uninit();
	vGaussianProgram_->uninit();
	textProgram_->uninit();
	meshProgram_->uninit();
	loadingProgram_->uninit();
	vBlurProgram_->uninit();
	hBlurProgram_->uninit();
	convProgram_->uninit();
}

void GfxRenderer::uninitMaskShaders()
{
	GfxShaderParams params;
	params.isMobile = isMobile_;
	params.withMask = true;
	texProgram_->uninit(params);
	texAlphaProgram_->uninit(params);
	colorProgram_->uninit(params);
	gradationProgram_->uninit(params);
	hGaussianProgram_->uninit(params);
	vGaussianProgram_->uninit(params);
	textProgram_->uninit(params);
	meshProgram_->uninit(params);
	loadingProgram_->uninit(params);
	vBlurProgram_->uninit(params);
	hBlurProgram_->uninit(params);
	convProgram_->uninit(params);
}

void GfxRenderer::initShaders(bool isMobile)
{
	GfxShaderParams params;
	params.withMask = false;
	params.isMobile = isMobile;

	texProgram_->init(params);
	texAlphaProgram_->init(params);
	colorProgram_->init(params);
	gradationProgram_->init(params);
	hGaussianProgram_->init(params);
	vGaussianProgram_->init(params);
	textProgram_->init(params);
	meshProgram_->init(params);
	loadingProgram_->init(params);
	vBlurProgram_->init(params);
	hBlurProgram_->init(params);
	convProgram_->init(params);
}

void GfxRenderer::init(bool isMobile)
{
	uninit();
	
	init_ = true;
#ifdef DX
	pd3d_ = Direct3DCreate9( D3D_SDK_VERSION ); 
    if ( !pd3d_ )
    {
		std::cout << "Can't make Direct3D\n";
        return;
    }
	D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );

    d3dpp.BackBufferWidth            = displayWidth_;
    d3dpp.BackBufferHeight           = displayHeight_;
    d3dpp.BackBufferFormat           = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount            = 1;
    d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality         = 0;
    d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow              = hwnd_;
    d3dpp.Windowed                   = TRUE;
    d3dpp.EnableAutoDepthStencil     = TRUE;
    d3dpp.AutoDepthStencilFormat     = D3DFMT_D24X8;
    d3dpp.Flags                      = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    d3dpp.FullScreen_RefreshRateInHz = 0;
    d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_ONE;

	if( FAILED( pd3d_->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd_,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp, &pd3dDevice_ ) ) )
    {
        std::cout << "Can't make Direct3D Device\n";
        return;
    }
	LPD3DXEFFECT ret = NULL;
    LPD3DXBUFFER pError = NULL;
    DWORD dwShaderFlags = 0;

#if _DEBUG
    dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

    D3DXCreateEffectFromFile(pd3dDevice_, "DXShder.fx",
        NULL, NULL, dwShaderFlags, NULL, &ret, &pError);

    if(!ret && pError)
    {
        int size  = pError->GetBufferSize();
        void *ack = pError->GetBufferPointer();

        if(ack)
        {
            char* str = new char[size];
            sprintf(str, (const char*)ack, size);
            OutputDebugString(str);
            delete [] str;
        }
    }
#else	// GL 2.0
	
	isMobile_ = isMobile;
	texProgram_.reset(new GfxTextureProgram);
	texAlphaProgram_.reset(new GfxTextureAlphaProgram);
	colorProgram_.reset(new GfxColorProgram);
	gradationProgram_.reset(new GfxGradationProgram);
	hGaussianProgram_.reset(new GfxHorizontalGaussianProgram);
	vGaussianProgram_.reset(new GfxVerticalGaussianProgram);
	textProgram_.reset(new GfxTextProgram);
	meshProgram_.reset(new GfxMeshProgram);
	loadingProgram_.reset(new GfxLoadingProgram);
	vBlurProgram_.reset(new GfxVBlurProgram);
	hBlurProgram_.reset(new GfxHBlurProgram);
	convProgram_.reset(new GfxConvolutionProgram);

	initShaders(isMobile);

	unsigned short rectIndices[] = 
	{0, 1, 2, 0, 2, 3};

	GLfloat rectVertices[] = {
		0, 0, 0, 0, 0,
		0, 1, 0, 0, 1,
		1, 1, 0, 1, 1,		
		1, 0, 0, 1, 0
	};	

	glGenBuffers(1, &rectVertexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, rectVertexBuffer_);
	glBufferData(GL_ARRAY_BUFFER, 
		sizeof(GLfloat)*20, rectVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &rectIndexBuffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectIndexBuffer_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
		sizeof(unsigned short) * 6, rectIndices, GL_STATIC_DRAW);



	unsigned char pixel[4];

	pixel[0] = 128; pixel[1] = 128; pixel[2] = 128; pixel[3] = 255;
	glGenTextures(1, &defaultDiffuseMap_);			
	glBindTexture(GL_TEXTURE_2D, defaultDiffuseMap_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
		1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

	pixel[0] = 255; pixel[1] = 255; pixel[2] = 255; pixel[3] = 255;
	glGenTextures(1, &defaultSpecularMap_);			
	glBindTexture(GL_TEXTURE_2D, defaultSpecularMap_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
		1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

	pixel[0] = 0; pixel[1] = 0; pixel[2] = 0; pixel[3] = 255;
	glGenTextures(1, &defaultLambertSpecularMap_);			
	glBindTexture(GL_TEXTURE_2D, defaultLambertSpecularMap_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
		1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

	pixel[0] = 127; pixel[1] = 127; pixel[2] = 255; pixel[3] = 255;
	glGenTextures(1, &defaultNormalMap_);			
	glBindTexture(GL_TEXTURE_2D, defaultNormalMap_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
		1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

	glGenFramebuffers(1, &gradationFBO_);
	makeDashedTexture();
	glGenFramebuffers(1, &shadowFBO_);
	glGenFramebuffers(1, &shapeFBO_);
	glGenTextures(1, &lineColorTexture_);

#endif	// GL 2.0 end

}

void GfxRenderer::beginDraw()
{
#ifdef DX
	pd3dDevice_->BeginScene();
	pd3dDevice_->Begin(0, NULL);
	pd3dShader_->BeginPass(0);
#else	// GL 2.0
	curProgram_ = NULL;
	useColorProgram();
	resetStencil();
#endif	// GL 2.0 end

	handleMaskInit(1);	
}

void GfxRenderer::endDraw()
{
#ifdef DX
	pd3dShader_->EndPass(curPass_);
	pd3dDevice_->End();
	pd3dDevice_->EndScene();
	pd3dDevice_->Present(NULL, NULL, NULL, NULL);
#endif	// DX end
}

const Matrix & GfxRenderer::modelMatrix() const
{
	return matrixStack_->matrix();
}

void GfxRenderer::pushMatrix()
{	
	matrixStack_->pushMatrix();
}

void GfxRenderer::popMatrix()
{
	matrixStack_->popMatrix();
}

void GfxRenderer::multMatrix(const Matrix & matrix)
{
	matrixStack_->multMatrix(matrix);
}

void GfxRenderer::preMultMatrix(const Matrix & matrix)
{
	matrixStack_->preMultMatrix(matrix);
}

void GfxRenderer::loadMatrix(const Matrix & matrix)
{
	matrixStack_->loadMatrix(matrix);
}

void GfxRenderer::setCameraMatrix(const Matrix & matrix)
{
	*cameraMatrix_ = matrix;
}


Matrix GfxRenderer::computeMvpMatrix() const
{
	return (*cameraMatrix_)*matrixStack_->matrix();
}


void GfxRenderer::bindElementArrayBuffer(GLuint buffer)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}

void GfxRenderer::bindArrayBuffer(GLuint buffer)
{
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void GfxRenderer::enableVertexAttribArrayTexCoord()
{
#ifdef DX
#else	//OpenGL 2.0
	glEnableVertexAttribArray(AttribTexCoord);
#endif
}

void GfxRenderer::disableVertexAttribArrayTexCoord()
{
#ifdef DX
#else	//OpenGL 2.0
	glDisableVertexAttribArray(AttribTexCoord);
#endif
}

void GfxRenderer::enableVertexAttribArrayPosition()
{
#ifdef DX
#else	//OpenGL 2.0
	glEnableVertexAttribArray(AttribPosition);
#endif
}

void GfxRenderer::disableVertexAttribArrayPosition()
{
	glDisableVertexAttribArray(AttribPosition);
}

void GfxRenderer::vertexAttribPositionPointer(int stride, char * ptr)
{
#ifdef DX
#else	//OpenGL 2.0
	glVertexAttribPointer(
		AttribPosition, 3, GL_FLOAT, GL_FALSE, stride, ptr);
#endif
}

void GfxRenderer::vertexAttribTexCoordPointer(int stride, char * ptr)
{	
#ifdef DX
#else	//OpenGL 2.0
	glVertexAttribPointer(
		AttribTexCoord, 2, GL_FLOAT, GL_FALSE, stride, ptr);
#endif
}

void GfxRenderer::drawRect(float width, float height, const Matrix & transform)
{	
#ifdef DX
#else	// GL 2.0
	bindElementArrayBuffer(rectIndexBuffer_);
	bindArrayBuffer(rectVertexBuffer_);
	enableVertexAttribArrayPosition();	
	vertexAttribPositionPointer(sizeof(GLfloat)*5, (char *)0);

	enableVertexAttribArrayTexCoord();
	vertexAttribTexCoordPointer(sizeof(GLfloat)*5, (char *)0 + sizeof(GLfloat)*3);


	pushMatrix();

	float matrixVals[] =
	{
		width, 0, 0, 0,
		0, height, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	multMatrix(*reinterpret_cast<Matrix *>(matrixVals));
	multMatrix(transform);

	applyCurrentShaderMatrix();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	popMatrix();
#endif
}


void GfxRenderer::drawRect(float x, float y, float width, float height)
{	
#ifdef DX
#else	// GL 2.0
	bindElementArrayBuffer(rectIndexBuffer_);
	bindArrayBuffer(rectVertexBuffer_);
	enableVertexAttribArrayPosition();
	vertexAttribPositionPointer(sizeof(GLfloat)*5, (char *)0);

	enableVertexAttribArrayTexCoord();
	vertexAttribTexCoordPointer(sizeof(GLfloat)*5, (char *)0 + sizeof(GLfloat)*3);
	

	pushMatrix();

	float matrixVals[] =
	{
		width, 0, 0, 0,
		0, height, 0, 0,
		0, 0, 1, 0,
		x, y, 0, 1
	};
	
	multMatrix(*reinterpret_cast<Matrix *>(matrixVals));

	applyCurrentShaderMatrix();	

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	popMatrix();
#endif
}

void GfxRenderer::drawBoundary(float x, float y, float width, float height)
{
#ifdef DX
#else	// GL 2.0
	unsigned char outlineColor[4];
	outlineColor[0] = 1.0f * 255;
	outlineColor[1] = 0.75f * 255;
	outlineColor[2] = 0.75f * 255;
	outlineColor[3] = 0.75f * 255;
	glBindTexture(GL_TEXTURE_2D, lineColorTexture());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, outlineColor);

	int boundaryHeight = height;
	int boundaryWidth = width;
	GLfloat vertices[] = {
		x, y, 0, 
		static_cast<GLfloat>(boundaryWidth), y, 0,
		static_cast<GLfloat>(boundaryWidth), static_cast<GLfloat>(boundaryHeight), 0,
		x, static_cast<GLfloat>(boundaryHeight), 0
	};

	pushMatrix();
	{		
		enableVertexAttribArrayPosition();
		bindArrayBuffer(0);
		bindElementArrayBuffer(0);
		vertexAttribPositionPointer(0, (char *)vertices);

		applyCurrentShaderMatrix();		
		glDrawArrays(GL_LINE_LOOP, 0, sizeof(vertices)/sizeof(Vector3));
	}
	popMatrix();	
#endif
}



void GfxRenderer::drawBox(const Vector3 & minCorner, const Vector3 & maxCorner)
{
#ifdef MSVC
	GLfloat vertices[] = {
		static_cast<GLfloat>(minCorner.x), static_cast<GLfloat>(minCorner.y), static_cast<GLfloat>(minCorner.z), 
		static_cast<GLfloat>(maxCorner.x), static_cast<GLfloat>(minCorner.y), static_cast<GLfloat>(minCorner.z),
		static_cast<GLfloat>(maxCorner.x), static_cast<GLfloat>(maxCorner.y), static_cast<GLfloat>(minCorner.z),
		static_cast<GLfloat>(minCorner.x), static_cast<GLfloat>(maxCorner.y), static_cast<GLfloat>(minCorner.z),

		static_cast<GLfloat>(minCorner.x), static_cast<GLfloat>(minCorner.y), static_cast<GLfloat>(maxCorner.z), 
		static_cast<GLfloat>(maxCorner.x), static_cast<GLfloat>(minCorner.y), static_cast<GLfloat>(maxCorner.z),
		static_cast<GLfloat>(maxCorner.x), static_cast<GLfloat>(maxCorner.y), static_cast<GLfloat>(maxCorner.z),
		static_cast<GLfloat>(minCorner.x), static_cast<GLfloat>(maxCorner.y), static_cast<GLfloat>(maxCorner.z),

		static_cast<GLfloat>(minCorner.x), static_cast<GLfloat>(minCorner.y), static_cast<GLfloat>(minCorner.z), 
		static_cast<GLfloat>(minCorner.x), static_cast<GLfloat>(minCorner.y), static_cast<GLfloat>(maxCorner.z),
		static_cast<GLfloat>(minCorner.x), static_cast<GLfloat>(maxCorner.y), static_cast<GLfloat>(maxCorner.z),
		static_cast<GLfloat>(minCorner.x), static_cast<GLfloat>(maxCorner.y), static_cast<GLfloat>(minCorner.z),

		static_cast<GLfloat>(maxCorner.x), static_cast<GLfloat>(minCorner.y), static_cast<GLfloat>(minCorner.z), 
		static_cast<GLfloat>(maxCorner.x), static_cast<GLfloat>(minCorner.y), static_cast<GLfloat>(maxCorner.z),
		static_cast<GLfloat>(maxCorner.x), static_cast<GLfloat>(maxCorner.y), static_cast<GLfloat>(maxCorner.z),
		static_cast<GLfloat>(maxCorner.x), static_cast<GLfloat>(maxCorner.y), static_cast<GLfloat>(minCorner.z),

		static_cast<GLfloat>(minCorner.x), static_cast<GLfloat>(minCorner.y), static_cast<GLfloat>(minCorner.z), 
		static_cast<GLfloat>(minCorner.x), static_cast<GLfloat>(minCorner.y), static_cast<GLfloat>(maxCorner.z),
		static_cast<GLfloat>(maxCorner.x), static_cast<GLfloat>(minCorner.y), static_cast<GLfloat>(maxCorner.z),
		static_cast<GLfloat>(maxCorner.x), static_cast<GLfloat>(minCorner.y), static_cast<GLfloat>(minCorner.z),

		static_cast<GLfloat>(minCorner.x), static_cast<GLfloat>(maxCorner.y), static_cast<GLfloat>(minCorner.z), 
		static_cast<GLfloat>(minCorner.x), static_cast<GLfloat>(maxCorner.y), static_cast<GLfloat>(maxCorner.z),
		static_cast<GLfloat>(maxCorner.x), static_cast<GLfloat>(maxCorner.y), static_cast<GLfloat>(maxCorner.z),
		static_cast<GLfloat>(maxCorner.x), static_cast<GLfloat>(maxCorner.y), static_cast<GLfloat>(minCorner.z)
	};
	
	enableVertexAttribArrayPosition();
	bindArrayBuffer(0);
	vertexAttribPositionPointer(0, (char *)vertices);
	applyCurrentShaderMatrix();
	glDrawArrays(GL_QUADS, 0, sizeof(vertices)/sizeof(Vector3));
#endif
}

void GfxRenderer::resetStencil()
{
	curStencilLevel_ = 0;
#ifdef DX
#else	// GL 2.0
	glStencilFunc(GL_ALWAYS, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
#endif
}

void GfxRenderer::beginIncStencilDrawing()
{
	curStencilLevel_++;	
	//glColorMask(0,0,0,0);
#ifdef DX
#else	// GL 2.0
	glStencilFunc(GL_ALWAYS, 0, ~0);
	glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
#endif
}

void GfxRenderer::beginDecStencilDrawing()
{
	curStencilLevel_--;	
	//glColorMask(0,0,0,0);
#ifdef DX
#else	// GL 2.0
	glStencilFunc(GL_ALWAYS, 0, ~0);
	glStencilOp(GL_KEEP, GL_DECR, GL_DECR);
#endif
}

void GfxRenderer::endStencilDrawing()
{
	//glColorMask(1, 1, 1, 1);
	if (curStencilLevel_ > 0)
	{
#ifdef DX
#else	// GL 2.0
		glStencilFunc(GL_LEQUAL, curStencilLevel_, ~0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
#endif
	}
	else
		resetStencil();
}

void GfxRenderer::pushStencilState()
{
	StencilState state;
	state.stencilLevel = curStencilLevel_;
	glGetIntegerv(GL_STENCIL_FAIL, &state.sfail);
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &state.dpfail);
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &state.dppass);
	glGetIntegerv(GL_STENCIL_FUNC, &state.func);
	glGetIntegerv(GL_STENCIL_REF, &state.ref);
	glGetIntegerv(GL_STENCIL_VALUE_MASK, &state.mask);

	stencilStateStack_.push(state);
	resetStencil();
}

void GfxRenderer::popStencilState()
{
	StencilState & state = stencilStateStack_.top();

	glStencilFunc(state.func, state.ref, state.mask);
	glStencilOp(state.sfail, state.dpfail, state.dppass);
	curStencilLevel_ = state.stencilLevel;

	stencilStateStack_.pop();
}

void GfxRenderer::pushBlendState()
{
	BlendState state;
	glGetIntegerv(GL_BLEND_SRC_RGB, &state.srcRgb);
	glGetIntegerv(GL_BLEND_DST_RGB, &state.dstRgb);
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &state.srcA);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &state.dstA);

	blendStateStack_.push(state);	
}

void GfxRenderer::popBlendState()
{
	BlendState & state = blendStateStack_.top();

	glBlendFuncSeparate(state.srcRgb, state.dstRgb, state.srcA, state.dstA);
	
	blendStateStack_.pop();
}

void GfxRenderer::clearZBuffer()
{
#ifdef DX
#else	// GL 2.0
	glClear(GL_DEPTH_BUFFER_BIT);
#endif
}


void GfxRenderer::handleMaskInit(int maxDepth)
{
	if (!maskBufferUsers_.empty())
	{
		for (unsigned i = maskBuffers_.size(); i <= maxDepth; ++i)
		{
			TextureSPtr tex(new Texture);
			TextureRendererSPtr ren(new TextureRenderer);

			maskBuffers_.push_back(tex);
			maskBufferRenderers_.push_back(ren);
		}


		for (unsigned i = 0; i < maskBuffers_.size(); ++i)
		{
			if (maskBuffers_[i]->width() != maskWidth_ || maskBuffers_[i]->height() != maskHeight_)
			{
				maskBuffers_[i]->init(this, maskWidth_, maskHeight_, Texture::UncompressedRgba32, NULL);
				maskBufferRenderers_[i]->init(this, maskWidth_, maskHeight_, true);
				maskBufferRenderers_[i]->setTexture(maskBuffers_[i].get());
			}
		}
	}
	else
	{
		maskBufferRenderers_.clear();
		maskBuffers_.clear();	
		uninitMaskShaders();
	}
}

void GfxRenderer::setMaskSize(int width, int height)
{
	maskWidth_ = width;
	maskHeight_ = height;
}

void GfxRenderer::registerMaskObject(SceneObject * maskObject)
{
	maskBufferUsers_.insert(maskObject);	
}


void GfxRenderer::unregisterMaskObject(SceneObject * maskObject)
{
	maskBufferUsers_.erase(maskObject);
}


void GfxRenderer::unregisterAllMaskObjects()
{
	maskBufferUsers_.clear();
}

void GfxRenderer::setCurTexRenderer(TextureRenderer * tex)
{
	curTexRenderer_ = tex;
}

void GfxRenderer::beginMaskDrawing()
{
	int curMask = curMaskBufferDepth_;
	drawingMaskStack_.push(curMaskBufferDepth_++);

	handleMaskInit(curMask);
	
	maskBufferRenderers_[curMask]->beginRenderToTexture(this, false);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	pushBlendState();
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
	
}

void GfxRenderer::endMaskDrawing()
{	
	int curMask = drawingMaskStack_.top();
	maskBufferRenderers_[curMask]->endRenderToTexture(this);

	popBlendState();
	useMaskStack_.push(drawingMaskStack_.top());
	drawingMaskStack_.pop();
}

void GfxRenderer::beginMaskedDrawing()
{		

	GfxShaderParams params;
	params.withMask = true;
	params.isMobile = isMobile_;
	curProgram_ = NULL;
	texProgram_->setCurParams(params);
	texAlphaProgram_->setCurParams(params);
	colorProgram_->setCurParams(params);
	gradationProgram_->setCurParams(params);
	hGaussianProgram_->setCurParams(params);
	vGaussianProgram_->setCurParams(params);
	textProgram_->setCurParams(params);
	meshProgram_->setCurParams(params);
	loadingProgram_->setCurParams(params);
	hBlurProgram_->setCurParams(params);
	vBlurProgram_->setCurParams(params);
	convProgram_->setCurParams(params);
}

void GfxRenderer::endMaskedDrawing()
{

	useMaskStack_.pop();

	GfxShaderParams params;
	params.withMask = !useMaskStack_.empty();
	params.isMobile = isMobile_;
	curProgram_ = NULL;
	texProgram_->setCurParams(params);
	texAlphaProgram_->setCurParams(params);
	colorProgram_->setCurParams(params);
	gradationProgram_->setCurParams(params);
	hGaussianProgram_->setCurParams(params);
	vGaussianProgram_->setCurParams(params);
	textProgram_->setCurParams(params);
	meshProgram_->setCurParams(params);
	loadingProgram_->setCurParams(params);
	hBlurProgram_->setCurParams(params);
	vBlurProgram_->setCurParams(params);
	convProgram_->setCurParams(params);
	curMaskBufferDepth_--;
}

/*
void GfxRenderer::showMasked(float x0, float y0, float x1, float y1)
{	
	useCustomProgram(showMaskedProgram_->glProgram(), showMaskedProgram_->locMvpMatrix());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, maskedTexture_->glTexture());		
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, maskTexture_->glTexture());
	
	GLfloat vertices[] = {
		x0, y0, 0.0f,
		x1, y0, 0.0f,
		x1, y1, 0.0f,		
		x0, y1, 0.0f
	};	

	enableVertexAttribArrayPosition();
	bindArrayBuffer(0);
	vertexAttribPositionPointer(0, (char *)vertices);
	glDrawArrays(GL_QUADS, 0, sizeof(vertices)/sizeof(Vector3));
	
	disableVertexAttribArrayPosition();
}
*/

void GfxRenderer::unproject(const Vector2 & projCoords, Vector3 * a, Vector3 * b) const
{
	const Matrix & camMatrix = cameraMatrix();
	Matrix invMat = camMatrix.inverse();
	Vector3 vec(projCoords.x, projCoords.y, -1.0f);
	*a = invMat * vec;
	vec.z = 1;
	*b = invMat * vec;
}

Ray GfxRenderer::unproject(const Vector2 & projCoords) const
{
	Vector3 a, b;
	unproject(projCoords, &a, &b);
	return Ray(a, b - a);
}

Vector3 GfxRenderer::project(const Vector3 & worldCoord) const
{
	return (*cameraMatrix_) * worldCoord;	
}

void GfxRenderer::use(Texture * texture)
{
#ifdef DX
#else	// GL 2.0

	
	if (texture->textureAlpha_)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture->textureAlpha_);	
	}	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->texture_);	

	float fs = 1.0f;
	float ft = 1.0f;

	if (texture->width() > 0 && texture->height() > 0)
	{
		fs = (float)texture->activeWidth() / texture->width();
		ft = (float)texture->activeHeight() / texture->height();
	}

	if (curProgram_ == texProgram_.get())
	{
		setTextureProgramFactorST(fs, ft);			
	}
	else if (curProgram_ == texAlphaProgram_.get())
	{
		setTextureAlphaProgramFactorST(fs, ft);
    }
	
#endif
}

void GfxRenderer::use(GLuint texture, GLuint textureAlpha)
{
#ifdef DX
#else	// GL 2.0
	
	if (textureAlpha)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureAlpha);
	}	
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);	

	if (curProgram_ == texProgram_.get())
		setTextureProgramFactorST(1.0f, 1.0f);
    else if (curProgram_ == texAlphaProgram_.get())
        setTextureAlphaProgramFactorST(1.0f, 1.0f);
#endif
}

void GfxRenderer::useTextureProgram() 
{
#ifdef DX
	if (curPass_ == ShaderPass::TEXTURE) return;
	pd3dShader_->EndPass(curPass_);
	curPass_ = ShaderPass::TEXTURE;
	pd3dShader_->BeginPass(curPass_);
#else	// GL 2.0
	if (curProgram_ ==  texProgram_.get()) return;
	glUseProgram(texProgram_->glProgram());
	curProgram_ = texProgram_.get();

	GfxShader * shader = texProgram_.get();
	if (!useMaskStack_.empty()) shader->setMaskTexture(maskBuffers_[useMaskStack_.top()].get());
#endif
}

void GfxRenderer::setTextureProgramOpacity(float opacity)
{	
	glUniform1f(texProgram_->data()->locOpacity, opacity);
}

void GfxRenderer::setTextureProgramFactorST(float fs, float ft)
{
	glUniform1f(texProgram_->data()->locFS, fs);
	glUniform1f(texProgram_->data()->locFT, ft);
}

void GfxRenderer::useTextureAlphaProgram() 
{
	if (curProgram_ == texAlphaProgram_.get()) return;
	glUseProgram(texAlphaProgram_->glProgram());
	curProgram_ = texAlphaProgram_.get();

	GfxShader * shader = texAlphaProgram_.get();
	if (!useMaskStack_.empty()) shader->setMaskTexture(maskBuffers_[useMaskStack_.top()].get());
}

void GfxRenderer::setTextureAlphaProgramOpacity(float opacity)
{	
	glUniform1f(texAlphaProgram_->data()->locOpacity, opacity);
}

void GfxRenderer::setTextureAlphaProgramFactorST(float fs, float ft)
{
	glUniform1f(texAlphaProgram_->data()->locFS, fs);
	glUniform1f(texAlphaProgram_->data()->locFT, ft);
}


void GfxRenderer::useColorProgram() 
{
#ifdef DX
	if (curPass_ == ShaderPass::COLOR) return;
	pd3dShader_->EndPass(curPass_);
	curPass_ = ShaderPass::COLOR;
	pd3dShader_->BeginPass(curPass_);
#else	// GL 2.0
	if (curProgram_ == colorProgram_.get()) return;
	glUseProgram(colorProgram_->glProgram());
	curProgram_ = colorProgram_.get();

	GfxShader * shader = colorProgram_.get();
	if (!useMaskStack_.empty()) shader->setMaskTexture(maskBuffers_[useMaskStack_.top()].get());

#endif
}

void GfxRenderer::useGradationProgram() 
{
#ifdef DX
	if (curPass_ == ShaderPass::COLOR) return;
	pd3dShader_->EndPass(curPass_);
	curPass_ = ShaderPass::COLOR;
	pd3dShader_->BeginPass(curPass_);
#else	// GL 2.0
	if (curProgram_ == gradationProgram_.get()) return;
	glUseProgram(gradationProgram_->glProgram());
	curProgram_ = gradationProgram_.get();

	GfxShader * shader = gradationProgram_.get();
	if (!useMaskStack_.empty()) shader->setMaskTexture(maskBuffers_[useMaskStack_.top()].get());

#endif
}

void GfxRenderer::useHorizontalGaussianProgram() 
{
#ifdef DX
	if (curPass_ == ShaderPass::COLOR) return;
	pd3dShader_->EndPass(curPass_);
	curPass_ = ShaderPass::COLOR;
	pd3dShader_->BeginPass(curPass_);
#else	// GL 2.0
	if (curProgram_ == hGaussianProgram_.get()) return;
	glUseProgram(hGaussianProgram_->glProgram());
	curProgram_ = hGaussianProgram_.get();

	GfxShader * shader = hGaussianProgram_.get();
	if (!useMaskStack_.empty()) shader->setMaskTexture(maskBuffers_[useMaskStack_.top()].get());

#endif
}

void GfxRenderer::useVerticalGaussianProgram() 
{
#ifdef DX
	if (curPass_ == ShaderPass::COLOR) return;
	pd3dShader_->EndPass(curPass_);
	curPass_ = ShaderPass::COLOR;
	pd3dShader_->BeginPass(curPass_);
#else	// GL 2.0
	if (curProgram_ == vGaussianProgram_.get()) return;
	glUseProgram(vGaussianProgram_->glProgram());
	curProgram_ = vGaussianProgram_.get();

	GfxShader * shader = vGaussianProgram_.get();
	if (!useMaskStack_.empty()) shader->setMaskTexture(maskBuffers_[useMaskStack_.top()].get());

#endif
}

void GfxRenderer::useTextProgram()
{
#ifdef DX
	if (curPass_ == ShaderPass::TEXT) return;
	pd3dShader_->EndPass(curPass_);
	curPass_ = ShaderPass::TEXT;
	pd3dShader_->BeginPass(curPass_);
#else	// GL 2.0
	if (curProgram_ == textProgram_.get()) return;
	glUseProgram(textProgram_->glProgram());
	curProgram_ = textProgram_.get();

	GfxShader * shader = textProgram_.get();
	if (!useMaskStack_.empty()) shader->setMaskTexture(maskBuffers_[useMaskStack_.top()].get());
#endif
}

void GfxRenderer::useHBlurProgram(){
#ifdef DX
	if (curPass_ == ShaderPass::HBLUR) return;
	pd3dShader_->EndPass(curPass_);
	curPass_ = ShaderPass::HBLUR;
	pd3dShader_->BeginPass(curPass_);
#else	// GL 2.0
	if (curProgram_ == hBlurProgram_.get()) return;
	glUseProgram(hBlurProgram_->glProgram());
	curProgram_ = hBlurProgram_.get();

	GfxShader * shader = hBlurProgram_.get();
	if (!useMaskStack_.empty()) shader->setMaskTexture(maskBuffers_[useMaskStack_.top()].get());
#endif
}

void GfxRenderer::useVBlurProgram(){
#ifdef DX
	if (curPass_ == ShaderPass::VBLUR) return;
	pd3dShader_->EndPass(curPass_);
	curPass_ = ShaderPass::VBLUR;
	pd3dShader_->BeginPass(curPass_);
#else	// GL 2.0
	if (curProgram_ == vBlurProgram_.get()) return;
	glUseProgram(vBlurProgram_->glProgram());
	curProgram_ = vBlurProgram_.get();

	GfxShader * shader = vBlurProgram_.get();
	if (!useMaskStack_.empty()) shader->setMaskTexture(maskBuffers_[useMaskStack_.top()].get());
#endif
}

void GfxRenderer::useLoadingProgram()
{
#ifdef DX
	if (curPass_ == ShaderPass::LOADING) return;
	pd3dShader_->EndPass(curPass_);
	curPass_ = ShaderPass::LOADING;
	pd3dShader_->BeginPass(curPass_);
#else	// GL 2.0
	if (curProgram_ == loadingProgram_.get()) return;
	glUseProgram(loadingProgram_->glProgram());
	curProgram_ = loadingProgram_.get();

	GfxShader * shader = loadingProgram_.get();
	if (!useMaskStack_.empty()) shader->setMaskTexture(maskBuffers_[useMaskStack_.top()].get());
#endif
}

void GfxRenderer::setLoadingProgramDim(float dim)
{
#ifdef DX
#else	// GL 2.0
	glUniform1f(loadingProgram_->data()->locDim, dim);	
#endif
}

void GfxRenderer::setLoadingProgramAngle(float t)
{
	#ifdef DX
#else	// GL 2.0
	glUniform1f(loadingProgram_->data()->locAngle, t);
#endif
}


void GfxRenderer::setNeedAlphaBlending(bool val)
{	
	if (!drawingMaskStack_.empty()) 
	{
		if (curTexRenderer_ = maskBufferRenderers_[drawingMaskStack_.top()].get())
			return;
	}
	if (val)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}

void GfxRenderer::useMeshProgram(float opacity, const Material * material)
{
	curProgram_ = meshProgram_.get();
	meshProgram_->setCurMaterial(opacity, material);
	GfxShader * shader = meshProgram_.get();
	if (!useMaskStack_.empty()) shader->setMaskTexture(maskBuffers_[useMaskStack_.top()].get());
}


void GfxRenderer::initMeshProgramLights(const Vector3 & camDir, float ambient, const std::vector<LightObject *> & lights)
{
	meshProgram_->initLights(camDir, ambient, lights);

}

void GfxRenderer::useCustomProgram(GfxShader * program)
{
#ifdef DX
#else	// GL 2.0

	GfxShaderParams params;
	params.withMask = !useMaskStack_.empty();
	params.isMobile = isMobile_;
	program->setCurParams(params);
	glUseProgram(program->glProgram());
	curProgram_ = program;

	if (!useMaskStack_.empty()) program->setMaskTexture(maskBuffers_[useMaskStack_.top()].get());
#endif
}


void GfxRenderer::useCustomProgram(GLuint program, GLuint locMvpMatrix)
{
	shaderWrapper_.reset(new GfxSingleShader(program, locMvpMatrix));
	glUseProgram(program);
	curProgram_ = shaderWrapper_.get();
}


void GfxRenderer::setColorProgramColor(float r, float g, float b, float a)
{
#ifdef DX
#else	// GL 2.0
	glUniform4f(colorProgram_->data()->locColor, r, g, b, a);
#endif
}

void GfxRenderer::setTextProgramOpacity(float opacity)
{
#ifdef DX
#else	// GL 2.0
	glUniform1f(textProgram_->data()->locOpacity, opacity);
#endif
}

void GfxRenderer::setHBlurProgramOpacity(float opacity){
#ifdef DX
#else	// GL 2.0
	glUniform1f(hBlurProgram_->data()->locOpacity, opacity);
#endif
}
void GfxRenderer::setVBlurProgramOpacity(float opacity){
#ifdef DX
#else	// GL 2.0
	glUniform1f(vBlurProgram_->data()->locOpacity, opacity);
#endif
}


void GfxRenderer::setHBlurProgramShadowSize(float shadow){
#ifdef DX
#else	// GL 2.0
	glUniform1f(hBlurProgram_->data()->locShadow, shadow);
#endif
}

void GfxRenderer::setVBlurProgramShadowSize(float shadow){
#ifdef DX
#else	// GL 2.0
	glUniform1f(vBlurProgram_->data()->locShadow, shadow);
#endif
}

void GfxRenderer::useConvProgram(){
#ifdef DX
	if (curPass_ == ShaderPass::VBLUR) return;
	pd3dShader_->EndPass(curPass_);
	curPass_ = ShaderPass::VBLUR;
	pd3dShader_->BeginPass(curPass_);
#else	// GL 2.0
	if (curProgram_ == convProgram_.get()) return;
	glUseProgram(convProgram_->glProgram());
	curProgram_ = convProgram_.get();

	GfxShader * shader = convProgram_.get();
	if (!useMaskStack_.empty()) shader->setMaskTexture(maskBuffers_[useMaskStack_.top()].get());
#endif
}
void GfxRenderer::setConvProgramType(int type){
#ifdef DX
#else	// GL 2.0
	glUniform1i(convProgram_->data()->locConvType, type);
#endif
}

void GfxRenderer::setConvProgramSize(float size){
#ifdef DX
#else	// GL 2.0
	glUniform1f(convProgram_->data()->locTexSize, size);
#endif
}

void GfxRenderer::setConvProgramOpacity(float opacity){
#ifdef DX
#else	// GL 2.0
	glUniform1f(convProgram_->data()->locOpacity, opacity);
#endif
}

void GfxRenderer::setTextProgramColor(float r, float g, float b, float a)
{
#ifdef DX
#else	// GL 2.0
	glUniform4f(textProgram_->data()->locColor, r, g, b, a);
#endif
}

void GfxRenderer::applyCurrentShaderMatrix()
{
#ifdef DX
	D3DXMATRIXA16 mat = D3DXMATRIXA16(computeMvpMatrix().vals());
	pd3dShader_->SetMatrix("MVPMatrix", &mat);
#else	// GL 2.0
	if (curProgram_ == texProgram_.get())
		glUniformMatrix4fv(texProgram_->data()->locMvpMatrix, 1, GL_FALSE, computeMvpMatrix().vals());
    else if (curProgram_ == texAlphaProgram_.get())
		glUniformMatrix4fv(texAlphaProgram_->data()->locMvpMatrix, 1, GL_FALSE, computeMvpMatrix().vals());
	else if (curProgram_ == colorProgram_.get())
		glUniformMatrix4fv(colorProgram_->data()->locMvpMatrix, 1, GL_FALSE, computeMvpMatrix().vals());
	else if (curProgram_ == gradationProgram_.get())
		glUniformMatrix4fv(gradationProgram_->data()->locMvpMatrix, 1, GL_FALSE, computeMvpMatrix().vals());
	else if (curProgram_ == vGaussianProgram_.get())
		glUniformMatrix4fv(vGaussianProgram_->data()->locMvpMatrix, 1, GL_FALSE, computeMvpMatrix().vals());
	else if (curProgram_ == hGaussianProgram_.get())
		glUniformMatrix4fv(hGaussianProgram_->data()->locMvpMatrix, 1, GL_FALSE, computeMvpMatrix().vals());
	else if (curProgram_ == textProgram_.get())
		glUniformMatrix4fv(textProgram_->data()->locMvpMatrix, 1, GL_FALSE, computeMvpMatrix().vals());
	else if (curProgram_ == meshProgram_.get())
	{
		glUniformMatrix4fv(meshProgram_->data()->locVPMatrix, 1, GL_FALSE, cameraMatrix_->vals());
		glUniformMatrix4fv(meshProgram_->data()->locMMatrix, 1, GL_FALSE, matrixStack_->matrix().vals());
	}
	else if (curProgram_ == loadingProgram_.get())
	{
		glUniformMatrix4fv(loadingProgram_->data()->locMvpMatrix, 1, GL_FALSE, computeMvpMatrix().vals());
	}else if (curProgram_ == vBlurProgram_.get())
	{
		glUniformMatrix4fv(vBlurProgram_->data()->locMvpMatrix, 1, GL_FALSE, computeMvpMatrix().vals());
	}else if (curProgram_ == hBlurProgram_.get())
	{
		glUniformMatrix4fv(hBlurProgram_->data()->locMvpMatrix, 1, GL_FALSE, computeMvpMatrix().vals());
	}else if (curProgram_ == convProgram_.get())
	{
		glUniformMatrix4fv(convProgram_->data()->locMvpMatrix, 1, GL_FALSE, computeMvpMatrix().vals());
	}else if (curProgram_)
		glUniformMatrix4fv(curProgram_->data()->locMvpMatrix, 1, GL_FALSE, computeMvpMatrix().vals());
#endif
}


void GfxRenderer::destroyTextData()
{
	map<TextFont, TextHandler *>::iterator iter = fontData_.begin();
	for (;iter != fontData_.end(); ++iter)
	{
		TextHandler * glFont = (*iter).second;
		glFont->uninit();
		delete glFont;
	}
	fontData_.clear();

	map<TextFont, TextHandlerOld *>::iterator iter2 = fontData_old_.begin();
	for (;iter2 != fontData_old_.end(); ++iter2)
	{
		TextHandlerOld * glFont = (*iter2).second;
		glFont->uninit();
		delete glFont;
	}
	fontData_old_.clear();
}

#include "Global.h"
TextHandler * GfxRenderer::textHandler(const TextFont & font)
{
	TextHandler * glFont = 0;
	map<TextFont, TextHandler *>::iterator iter = fontData_.find(font);
	if (iter == fontData_.end())
	{
		
		glFont = new TextHandler(font);
		glFont->setMagnification(Global::instance().curSceneMagnification());
		glFont->init();
		
		
		fontData_[font] = glFont;		
	}
	else 
		glFont = (*iter).second;

	return glFont;	
}

TextHandlerOld * GfxRenderer::textHandlerOld(const TextFont & font)
{
	TextHandlerOld * glFont = 0;
	map<TextFont, TextHandlerOld *>::iterator iter = fontData_old_.find(font);
	if (iter == fontData_old_.end())
	{
		
		glFont = new TextHandlerOld(font);
		glFont->init();
		
		fontData_old_[font] = glFont;		
	}
	else 
		glFont = (*iter).second;

	return glFont;	
}

void GfxRenderer::resizeText()
{
	//map<TextFont, TextHandler *>::iterator it, it_next;
	//for(it=fontData_.begin(), it_next=it; it!=fontData_.end(); it=it_next) {
	//	++it_next;
	//	TextHandler * handler = (*it).second;
	//	float textureScaleSize = handler->getMagnification();
	//	bool bNeedInit = true;

	//	if(textureScaleSize == 1.0 || textureScaleSize == 2.0 || textureScaleSize == 4.0)
	//		bNeedInit = false;

	//	if(bNeedInit){			

	//		handler->uninit();
	//		fontData_.erase(it);

	//		delete handler;
	//	}
	//}
}

void GfxRenderer::makeGradationTexture(Texture *texture, Color color, Color secondaryColor, GradDir gradDir, float gradStop)
{
	GLuint oldFBO;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&oldFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gradationFBO_);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->glTexture(), 0);
	GLenum buffer[] = {GL_COLOR_ATTACHMENT0};
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	unsigned char pattern[12];
	pattern[0] = color.r * 255;
	pattern[1] = color.g * 255;
	pattern[2] = color.b * 255;
	pattern[3] = color.a * 255;
	pattern[4] = (color.r + secondaryColor.r)*0.5 * 255;
	pattern[5] = (color.g + secondaryColor.g)*0.5 * 255;
	pattern[6] = (color.b + secondaryColor.b)*0.5 * 255;
	pattern[7] = (color.a + secondaryColor.a)*0.5 * 255;
	pattern[8] = secondaryColor.r * 255;
	pattern[9] = secondaryColor.g * 255;
	pattern[10] = secondaryColor.b * 255;
	pattern[11] = secondaryColor.a * 255;

	pushMatrix();
	useTextureProgram();
	setTextureProgramOpacity(1);
	use(tex);
	Matrix mat = Matrix::Identity();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, texture->width(), texture->height());

	enableVertexAttribArrayPosition();
	enableVertexAttribArrayTexCoord();
	bindArrayBuffer(0);
	bindElementArrayBuffer(0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 3, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pattern);

	Vector3 stopVertex;
	Vector3 vertices[6];
	Vector2 texcoords[6];
	float stopCoord = -1 + 2*gradStop;
	float offset0 = 0.5/3;
	float offset1 = 1-offset0;
	vertices[0] = Vector3(-1, 1, 0);
	vertices[1] = Vector3(-1, -1, 0);
	vertices[2] = Vector3(stopCoord, 1, 0);
	vertices[3] = Vector3(stopCoord, -1, 0);
	vertices[4] = Vector3(1, 1, 0);
	vertices[5] = Vector3(1, -1, 0);
	texcoords[0] = Vector2(offset0, 0.5);
	texcoords[1] = Vector2(offset0, 0.5);
	texcoords[2] = Vector2(0.5, 0.5);
	texcoords[3] = Vector2(0.5, 0.5);
	texcoords[4] = Vector2(offset1, 0.5);
	texcoords[5] = Vector2(offset1, 0.5);

	vertexAttribPositionPointer(0, (char *)vertices);
	vertexAttribTexCoordPointer(0, (char *)texcoords);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (gradDir == RU || gradDir == LU || gradDir == LD || gradDir == RD)
	{
		float s = sqrtf(2.0f);
		mat = Matrix::Scale(s, s, s);
	}
	mat *= Matrix::Rotate((float)gradDir*45/180*M_PI, 0, 0, -1);

	glUniformMatrix4fv(texProgram_->data()->locMvpMatrix, 1, GL_FALSE, mat.vals());
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
	popMatrix();
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
	glDeleteTextures(1, &tex);
}

void GfxRenderer::makeDashedTexture()
{
	unsigned char pattern[4][16] = { {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
	,{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}
	,{1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0}
	,{1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 }};
	unsigned char patternRGBA[4][16*4];
	glGenTextures(4, dashedTexture_);
	for (int i = 0 ; i < 4 ; i++)
	{
		glBindTexture(GL_TEXTURE_2D, dashedTexture_[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		for (int j = 0 ; j < 16 ; j++)
		{
			patternRGBA[i][j*4] = 
				patternRGBA[i][j*4+1] = 
				patternRGBA[i][j*4+2] = 
				patternRGBA[i][j*4+3] = (pattern[i][j])? 255 : 0;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, patternRGBA[i]);
	}
}

void GfxRenderer::makeShadowTexture(Texture *texture, Color color, const std::vector<Vector3> vertices, std::vector<unsigned short> indices)
{
	//GLuint tex[2];
	//glGenTextures(2, tex);
	//glBindTexture(GL_TEXTURE_2D, tex[0]);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	//char pattern[4] = {color.r * 255, color.g * 255, color.b * 255, color.a * 255};
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pattern);
	//	
	//glBindTexture(GL_TEXTURE_2D, tex[1]);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	//GLuint oldFBO;
	//glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&oldFBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, gradationFBO_);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex[1], 0);
	//GLenum buffer[] = {GL_COLOR_ATTACHMENT0};
	//
	//pushMatrix();
	//useHorizontalGaussianProgram();
	//Matrix mat = Matrix::Identity();
	//GLint viewport[4];
	//glGetIntegerv(GL_VIEWPORT, viewport);
	//glViewport(0, 0, texture->width(), texture->height());
	//enableVertexAttribArrayPosition();
	//enableVertexAttribArrayTexCoord();
	//bindArrayBuffer(0);
	//bindElementArrayBuffer(0);
	//std::vector<Vector2> texcoords(vertices.size(), Vector2(0.5,0.5));
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, tex[0]);	
	//glUniform1f(hGaussianProgram_->data()->invWidth, 1.0/texture->width());
	//
	//vertexAttribPositionPointer(0, (char *)vertices);
	//vertexAttribPositionPointer(0, (char *)texcoords);
	//if (indices.si

	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->glTexture(), 0);
	//useVerticalGaussianProgram();
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, tex[1]);	
	//glUniform1f(vGaussianProgram_->data()->invHeight, 1.0/texture->height());



	//Vector3 stopVertex;
	//Vector3 vertices[6];
	//Vector2 texcoords[6];
	//float stopCoord = -1 + 2*gradStop;
	//float offset0 = 0.5/3;
	//float offset1 = 1-offset0;
	//vertices[0] = Vector3(-1, 1, 0);
	//vertices[1] = Vector3(-1, -1, 0);
	//vertices[2] = Vector3(stopCoord, 1, 0);
	//vertices[3] = Vector3(stopCoord, -1, 0);
	//vertices[4] = Vector3(1, 1, 0);
	//vertices[5] = Vector3(1, -1, 0);
	//texcoords[0] = Vector2(offset0, 0.5);
	//texcoords[1] = Vector2(offset0, 0.5);
	//texcoords[2] = Vector2(0.5, 0.5);
	//texcoords[3] = Vector2(0.5, 0.5);
	//texcoords[4] = Vector2(offset1, 0.5);
	//texcoords[5] = Vector2(offset1, 0.5);

	//vertexAttribPositionPointer(0, (char *)vertices);
	//vertexAttribTexCoordPointer(0, (char *)texcoords);
	//
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//if (gradDir == RU || gradDir == LU || gradDir == LD || gradDir == RD)
	//{
	//	float s = sqrtf(2.0f);
	//	mat = Matrix::Scale(s, s, s);
	//}
	//mat *= Matrix::Rotate((float)gradDir*45/180*M_PI, 0, 0, -1);
	//
	//glUniformMatrix4fv(texProgram_->data()->locMvpMatrix, 1, GL_FALSE, mat.vals());
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
	//popMatrix();
	//glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	//glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
	//glDeleteTextures(1, &tex);
}

void GfxRenderer::setDataForGaussianProgram(GLuint texture, float width, float height)
{
#ifdef DX
#else	// GL 2.0

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);	

	if (curProgram_ == hGaussianProgram_.get() && width > 0 )
	{
		glUniform1f(hGaussianProgram_->data()->invWidth, 1.0/width);
	}
	else if (curProgram_ == vGaussianProgram_.get() && height > 0 )
	{
		glUniform1f(vGaussianProgram_->data()->invHeight, 1.0/height);
	}

#endif
}
