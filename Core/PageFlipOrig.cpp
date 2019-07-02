#include "stdafx.h"
#include "PageFlipOrig.h"
#include "GfxRenderer.h"
#include "Document.h"
#include "Scene.h"
#include "SceneObject.h"
#include "BoundingBox.h"
#include "Texture.h"
#include "TextureRenderer.h"

#include "ImageReader.h"
#include "Global.h"

#include "GLUtils.h"

#include "Root.h"

using namespace std;


const char gFrontShaderSrc[] =
"precision mediump float;\n\
varying vec2 vTexCoord;\n\
varying vec2 vPosition;\n\
varying float lighting;\n\
uniform sampler2D tex0;\n\
uniform sampler2D tex1;\n\
uniform vec2 R;\n\
uniform vec2 edge;\n\
uniform float curlAmt;\n\
\n\
void main()\n\
{\
	gl_FragColor = texture2D(tex0, vTexCoord.st);\n\
	float shadow = texture2D(tex1, vec2(vPosition.x, 1.0 - vPosition.y)).r;\n\
	vec2 v = vPosition - R;\n\
	float t = min(4.0*(-v.x * edge.y + v.y * edge.x), 1.0);\n\
	float lighting = mix(0.6, 0.95, t);\n\
	t = clamp((0.4 - curlAmt)/(0.4 - 1.2), 0.0, 1.0);\n\
	lighting = mix(1.0, lighting, t);\n\
	t = min(1.0, 5.0 * R.x);\n\
	lighting = mix(1.0, lighting, t);\n\
	gl_FragColor.rgb *= lighting * shadow;\n\
}\n\
";


const char gFrontShaderSrcV[] =
"precision mediump float;\n\
varying vec2 vTexCoord;\n\
varying vec2 vPosition;\n\
varying float lighting;\n\
uniform sampler2D tex0;\n\
uniform sampler2D tex1;\n\
uniform vec2 R;\n\
uniform vec2 edge;\n\
uniform float curlAmt;\n\
\n\
void main()\n\
{\
	gl_FragColor = texture2D(tex0, vec2(1.0 - vTexCoord.t, vTexCoord.s));\n\
	float shadow = texture2D(tex1, vec2(vPosition.x, 1.0 - vPosition.y)).r;\n\
	vec2 v = vPosition - R;\n\
	float t = min(4.0*(-v.x * edge.y + v.y * edge.x), 1.0);\n\
	float lighting = mix(0.6, 0.95, t);\n\
	t = clamp((0.4 - curlAmt)/(0.4 - 1.2), 0.0, 1.0);\n\
	lighting = mix(1.0, lighting, t);\n\
	t = min(1.0, 5.0 * R.x);\n\
	lighting = mix(1.0, lighting, t);\n\
	gl_FragColor.rgb *= lighting * shadow;\n\
}\n\
";


const char gFrontVertShaderSrc[] =
"\
attribute vec4 position;\
attribute vec2 texCoord;\
uniform mat4 modelViewProjectionMatrix;\n\
\n\
varying vec2 vTexCoord;\n\
varying vec2 vPosition;\n\
\n\
void main()\n\
{\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	vPosition.x = 0.5*(gl_Position.x/gl_Position.w + 1.0);\
	vPosition.y = 0.5*(1.0 - gl_Position.y/gl_Position.w);\
	vTexCoord = texCoord;\n\
}\
";


const char gFrontVertShaderSrcV[] =
"\
attribute vec4 position;\
attribute vec2 texCoord;\
uniform mat4 modelViewProjectionMatrix;\n\
\n\
varying vec2 vTexCoord;\n\
varying vec2 vPosition;\n\
\n\
void main()\n\
{\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	vPosition.y = 1.0 - 0.5*(gl_Position.x/gl_Position.w + 1.0);\
	vPosition.x = 0.5*(1.0 - gl_Position.y/gl_Position.w);\
	vTexCoord = texCoord;\n\
}\
";

const char gBackShaderSrc[] =
"precision mediump float;\n\
varying vec2 vTexCoord;\n\
varying vec2 v;\n\
varying float vLighting1;\n\
uniform sampler2D tex0;\n\
uniform vec2 R;\n\
uniform vec2 edge;\n\
\n\
void main()\n\
{\
	gl_FragColor = texture2D(tex0, vTexCoord.st);\n\
	gl_FragColor.rgb = 0.6 + 0.4*gl_FragColor.rgb;\n\
	//gl_FragColor.rgb = vec3(1.0, 1.0, 1.0);\n\
	float t = max(2.0*(v.x * edge.y - v.y * edge.x) + 0.2, 0.0);\n\
	float lighting = mix(1.0, 0.0, t);\n\
	lighting = mix(1.0, lighting, vLighting1);\n\
	gl_FragColor.rgb *= lighting;\n\
}\n\
";

const char gBackShaderSrcV[] =
"precision mediump float;\n\
varying vec2 vTexCoord;\n\
varying vec2 v;\n\
varying float vLighting1;\n\
uniform sampler2D tex0;\n\
uniform vec2 R;\n\
uniform vec2 edge;\n\
\n\
void main()\n\
{\
	gl_FragColor = texture2D(tex0, vec2(1.0 - vTexCoord.t, vTexCoord.s));\n\
	gl_FragColor.rgb = 0.6 + 0.4*gl_FragColor.rgb;\n\
	//gl_FragColor.rgb = vec3(1.0, 1.0, 1.0);\n\
	float t = max(2.0*(v.x * edge.y - v.y * edge.x) + 0.2, 0.0);\n\
	float lighting = mix(1.0, 0.0, t);\n\
	lighting = mix(1.0, lighting, vLighting1);\n\
	gl_FragColor.rgb *= lighting;\n\
}\n\
";


const char gBackVertShaderSrc[] =
"\
attribute vec4 position;\
attribute vec2 texCoord;\
uniform mat4 modelViewProjectionMatrix;\n\
varying vec2 vTexCoord;\n\
varying vec2 v;\n\
varying float vLighting1;\n\
uniform vec2 R;\n\
uniform vec2 edge;\n\
uniform float curlAmt;\n\
\n\
void main()\n\
{\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	v.x = 0.5*(gl_Position.x/gl_Position.w + 1.0);\
	v.y = 0.5*(1.0 - gl_Position.y/gl_Position.w);\
	v = v - R;\n\
	vTexCoord = texCoord;\n\
	vLighting1 = min(1.0, (0.7 - curlAmt)/(0.7 - 1.2)) * min(1.0, 5.0 * R.x);\n\
}\n\
";


const char gBackVertShaderSrcV[] =
"\
attribute vec4 position;\
attribute vec2 texCoord;\
uniform mat4 modelViewProjectionMatrix;\n\
varying vec2 vTexCoord;\n\
varying vec2 v;\n\
varying float vLighting1;\n\
uniform vec2 R;\n\
uniform vec2 edge;\n\
uniform float curlAmt;\n\
\n\
void main()\n\
{\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	v.y = 1.0 - 0.5*(gl_Position.x/gl_Position.w + 1.0);\
	v.x = 0.5*(1.0 - gl_Position.y/gl_Position.w);\
	v = v - R;\n\
	vTexCoord = texCoord;\n\
	vLighting1 = min(1.0, (0.7 - curlAmt)/(0.7 - 1.2)) * min(1.0, 5.0 * R.x);\n\
}\n\
";

const char gBottomShaderSrc[] =
"precision mediump float;\n\
varying float lighting;\n\
\n\
void main()\n\
{\
	gl_FragColor = vec4(0.0, 0.0, 0.0, lighting);\n\
}\n";


const char gBottomVertShaderSrc[] =
"\
attribute vec4 position;\
attribute vec2 texCoord;\
uniform mat4 modelViewProjectionMatrix;\
varying float lighting;\n\
uniform vec2 R;\n\
uniform vec2 edge;\n\
\n\
void main()\n\
{\n\
	\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	vec2 v;\n\
	v.x = 0.5*(gl_Position.x/gl_Position.w + 1.0);\n\
	v.y = 0.5*(1.0 - gl_Position.y/gl_Position.w);\n\
	v = v - R;\n\
	float t = clamp(5.0*(v.x * edge.y - v.y * edge.x), 0.0, 1.0);\n\
	lighting = mix(0.7, 1.0, t);\n\
	t = min(1.0, 5.0*R.x);\n\
	lighting = 1.0 - mix(1.0, lighting, t);\n\
}";

const char gBottomVertShaderSrcV[] =
"\
attribute vec4 position;\
attribute vec2 texCoord;\
uniform mat4 modelViewProjectionMatrix;\
varying float lighting;\n\
uniform vec2 R;\n\
uniform vec2 edge;\n\
\n\
void main()\n\
{\n\
	\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	vec2 v;\n\
	v.y = 1.0 - 0.5*(gl_Position.x/gl_Position.w + 1.0);\n\
	v.x = 0.5*(1.0 - gl_Position.y/gl_Position.w);\n\
	v = v - R;\n\
	float t = clamp(5.0*(v.x * edge.y - v.y * edge.x), 0.0, 1.0);\n\
	lighting = mix(0.7, 1.0, t);\n\
	t = min(1.0, 5.0*R.x);\n\
	lighting = 1.0 - mix(1.0, lighting, t);\n\
}";



const char gHBlurShaderSrc[] =
"precision mediump float;\n\
varying vec2 vTexCoord;\n\
\n\
uniform sampler2D tex0;\n\
uniform float blurSize;\n\
void main()\n\
{\n\
	vec4 sum = vec4(0.0);\n\
   sum += texture2D(tex0, vec2(vTexCoord.x - 3.4*blurSize, vTexCoord.y)) * 0.15;\n\
   sum += texture2D(tex0, vec2(vTexCoord.x - 1.4444444444*blurSize, vTexCoord.y)) * 0.27;\n\
   sum += texture2D(tex0, vec2(vTexCoord.x, vTexCoord.y)) * 0.16;\n\
   sum += texture2D(tex0, vec2(vTexCoord.x + 1.4444444444*blurSize, vTexCoord.y)) * 0.27;\n\
   sum += texture2D(tex0, vec2(vTexCoord.x + 3.4*blurSize, vTexCoord.y)) * 0.15;\n\
   gl_FragColor = sum;\n\
}\n\
";

const char gVBlurShaderSrc[] =
"precision mediump float;\n\
varying vec2 vTexCoord;\n\
\n\
uniform sampler2D tex0;\n\
uniform float blurSize;\n\
void main()\n\
{\n\
	vec4 sum = vec4(0.0);\n\
   sum += texture2D(tex0, vec2(vTexCoord.x, vTexCoord.y - 3.4*blurSize)) * 0.15;\n\
   sum += texture2D(tex0, vec2(vTexCoord.x, vTexCoord.y - 1.4444444444*blurSize)) * 0.27;\n\
   sum += texture2D(tex0, vec2(vTexCoord.x, vTexCoord.y)) * 0.16;\n\
   sum += texture2D(tex0, vec2(vTexCoord.x, vTexCoord.y + 1.4444444444*blurSize)) * 0.27;\n\
   sum += texture2D(tex0, vec2(vTexCoord.x, vTexCoord.y + 3.4*blurSize)) * 0.15;\n\
   gl_FragColor = sum;\n\
}\n\
";

const char gVertShaderSrc[] =
"\
attribute vec4 position;\
attribute vec2 texCoord;\
uniform mat4 modelViewProjectionMatrix;\
\
varying vec2 vTexCoord;\
varying vec2 vPosition;\
\
void main()\
{\
	\
	gl_Position = modelViewProjectionMatrix*position;\
	vTexCoord = texCoord;\
	vPosition.x = 0.5*(gl_Position.x/gl_Position.w + 1.0);\
	vPosition.y = 0.5*(1.0 - gl_Position.y/gl_Position.w);\
}\
";

const char gBlurVertShaderSrc[] =
"\n\
attribute vec4 position;\n\
attribute vec2 texCoord;\n\
uniform mat4 modelViewProjectionMatrix;\n\
\n\
varying vec2 vTexCoord;\n\
\n\
void main()\n\
{\n\
	\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	\n\
	vTexCoord = texCoord;\n\
}\n\
";

void gfxLog(const std::string & str);

void PageFlipOrig::computeWH()
{
	if (transitionDirection_ == TransitionDirectionUp || transitionDirection_ == TransitionDirectionDown)
	{
		width_ = docH_ / docW_;
		height_ = 1.0f;
	}
	else
	{

		width_ = docW_ / docH_;
		height_ = 1.0f;

	}

	camera_.SetTo2DArea(width_/2.0f, height_/2.0f, width_/2.0f, height_*1.25f, width_, height_, 40.0f, width_/height_);
	cameraMatrix_ = camera_.GetMatrix();
	camera_.setNearPlane(0.01f);
	camera_.setFarPlane(100.0f);
}

PageFlipOrig::PageFlipOrig()
{
	transitionDirection_ = TransitionDirectionNone;
	docW_ = 768.0f;
	docH_ = 1024.0f;
	computeWH();

	/*
	//debug
	camera_.setLookAt(Vector3(0.5f, 0.5f, 0.0f));
	camera_.setEye(Vector3(0.5f, -1.0f, -1.5f));
	cameraMatrix_ = camera_.GetMatrix();
	//end debug
	*/
	
	indexBuffer_ = vertexBuffer_ = 0;
	bottomShadeIndexBuffer_ = bottomShadeVertexBuffer_ = 0;
	shadowIndexBuffer_ = shadowVertexBuffer_ = 0;
	blankTexture_ = 0;

	programVBlur_ = programHBlur_ = programBack_ = programBackV_ = programFront_ = programFrontV_ = programBottom_ = programBottomV_ = 0;

	segments_ = 15;
	dropShadowSegments_ = 5;
	rRadius_ = 0.15f;

	gl_ = NULL;
	prevTime_ = 0.0f;
	isPressed_ = false;

	t_ = 1.0f;
	targetR_ = startR_ = curR_ = Vector2(width_, height_/2.0f);
	targetDir_ = startDir_ = curDir_ = Vector2(-width_, 0.0f);
	needComputeVertex_ = true;

	

	doPageChange_ = false;
	texture_ = NULL;

	shadowSrc_ = Vector3(1.0f, 0.0f, -2.0f);

	texRenderer1_ = new TextureRenderer();
	renderTex1_ = new Texture();

	texRenderer2_ = new TextureRenderer();
	renderTex2_ = new Texture();

	tStep_ = 7.0f;

	
}

PageFlipOrig::~PageFlipOrig()
{
	delete texRenderer1_;
	delete renderTex1_;

	delete texRenderer2_;
	delete renderTex2_;
}

void PageFlipOrig::animate(TransitionDirection dir)
{
	transitionDirection_ = dir;
	computeWH();
	if (transitionDirection_ == TransitionDirectionRight || transitionDirection_ == TransitionDirectionDown)
	{
		startR_ = curR_ = Vector2(width_, height_/2.0f);
		startDir_ = curDir_ = Vector2(-1.0f, 0.0f);
		targetDir_ = pressDir_ = Vector2(-0.5f, 0.5f).normalize();		
		targetR_ = Vector2(0.0f, height_/2.0f);
	}
	else
	{		
		startR_ = curR_ = Vector2(0.0f, height_/2.0f);
		startDir_ = curDir_ = Vector2(-1.0f, 0.0f);
		targetDir_ = pressDir_ = Vector2(-1.0f, 0.0f);
		targetR_ = Vector2(width_, height_/2.0f);
	}


	t_ = 0.0f;	
	tStep_ = 3.5f;
	startR_ = curR_;
	startDir_ = curDir_;	
	
	targetDir_ = curDir_;
	targetDir_.y = 0.0f;
	targetDir_ = targetDir_.normalize();

	update();

}

void PageFlipOrig::reset()
{
	transitionDirection_ = TransitionDirectionNone;
	if (t_ < 1.0f)
	{
		targetR_ = startR_;
		targetDir_ = startDir_;
		t_ = 1.0f;
	}	
}

bool PageFlipOrig::update()
{
	float time = Global::currentTime();
	float deltaTime = time - prevTime_;
	prevTime_ = time;

	Vector2 dir = targetDir_;
	Vector2 r = targetR_;

	if (t_ < 1.0f)
	{		
		t_ += tStep_ * deltaTime;
		if (t_ >= 1.0f) 
		{
			t_ = 1.0f;
			if (!isPressed_) 
			{
				if ((transitionDirection_ == TransitionDirectionLeft || transitionDirection_ == TransitionDirectionUp) && targetR_.x == width_)
					doPageChange_ = true;

				if ((transitionDirection_ == TransitionDirectionRight || transitionDirection_ == TransitionDirectionDown) && targetR_.x == 0.0f)
					doPageChange_ = true;

				transitionDirection_ = TransitionDirectionNone;				
			}
		}
		curR_ = startR_ + t_ * (targetR_ - startR_);		
		curDir_ = startDir_ + t_ * (targetDir_ - startDir_);

		needComputeVertex_ = true;
	}
	else
	{
		curR_ = targetR_;
		curDir_ = targetDir_;		
	}

	if (needComputeVertex_)
	{
		//debug R
		//curR_ = Vector2(0.525f, 0.508f);
		//curDir_ = Vector2(-0.93f, -0.367f).normalize();
		//curR_ = Vector2(0.5f, 0.5f);
		//curDir_ = Vector2(-1.0f, 1.0f).normalize();
		computeVerts(curR_, curDir_);
		
		needComputeVertex_ = false;

		return true;
	}

	return false;

}

inline GLuint setupShader(const char * sfragmentShader, const char * svertShader, const char * name)
{	
	std::string error;
	GLuint fragmentShader, vertShader;
	GLuint program = glCreateProgram();	
	fragmentShader = loadShader(GL_FRAGMENT_SHADER, sfragmentShader, &error);
	gfxLog(std::string("Loading ") + name + " fragment shader\n " + error);	
	vertShader = loadShader(GL_VERTEX_SHADER, svertShader, &error);
	gfxLog(std::string("Loading ") + name + " vert shader\n " + error);	
	glAttachShader(program, vertShader);   
	glAttachShader(program, fragmentShader);  
	glBindAttribLocation(program, AttribPosition, "position");
	glBindAttribLocation(program, AttribTexCoord, "texCoord");
	linkProgram(program, &error);	
	glDeleteShader(fragmentShader);
	glDeleteShader(vertShader);	
	
	return program;
}

#include "GfxShaders.h"

void PageFlipOrig::init(GfxRenderer * gl)
{
	uninit();
	gl_ = gl;

	GLint texLoc;

	programFront_ = setupShader(gFrontShaderSrc, gFrontVertShaderSrc, "Front");
	glUseProgram(programFront_);
	locFrontMVPMatrix_ = 
		glGetUniformLocation(programFront_, "modelViewProjectionMatrix");
	locFrontR_ = glGetUniformLocation(programFront_, "R");
	locFrontEdge_ = glGetUniformLocation(programFront_, "edge");
	locFrontCurlAmt_ = glGetUniformLocation(programFront_, "curlAmt");
	texLoc = glGetUniformLocation(programFront_, "tex0");
	glUniform1i(texLoc, 0);
	texLoc = glGetUniformLocation(programFront_, "tex1");
	glUniform1i(texLoc, 1);

	programFrontV_ = setupShader(gFrontShaderSrcV, gFrontVertShaderSrcV, "FrontV");
	glUseProgram(programFrontV_);
	texLoc = glGetUniformLocation(programFrontV_, "tex0");
	glUniform1i(texLoc, 0);
	texLoc = glGetUniformLocation(programFrontV_, "tex1");
	glUniform1i(texLoc, 1);

	programBack_ = setupShader(gBackShaderSrc, gBackVertShaderSrc, "Back");	
	glUseProgram(programBack_);
	locBackMVPMatrix_ = 
		glGetUniformLocation(programBack_, "modelViewProjectionMatrix");
	locBackR_ = glGetUniformLocation(programBack_, "R");
	locBackEdge_ = glGetUniformLocation(programBack_, "edge");
	locBackCurlAmt_ = glGetUniformLocation(programBack_, "curlAmt");
	
	programBackV_ = setupShader(gBackShaderSrcV, gBackVertShaderSrcV, "BackV");	

	programBottom_ = setupShader(gBottomShaderSrc, gBottomVertShaderSrc, "Bottom");
	glUseProgram(programBottom_);
	locBottomMVPMatrix_ = 
		glGetUniformLocation(programBottom_, "modelViewProjectionMatrix");
	locBottomR_ = glGetUniformLocation(programBottom_, "R");
	locBottomEdge_ = glGetUniformLocation(programBottom_, "edge");

	programBottomV_ = setupShader(gBottomShaderSrc, gBottomVertShaderSrcV, "BottomV");

	programHBlur_ = setupShader(gHBlurShaderSrc, gBlurVertShaderSrc, "HBlur");
	glUseProgram(programHBlur_);
	locHBlurMVPMatrix_ = 
		glGetUniformLocation(programHBlur_, "modelViewProjectionMatrix");
	locHBlurSize_ = glGetUniformLocation(programHBlur_, "blurSize");

	programVBlur_ = setupShader(gVBlurShaderSrc, gBlurVertShaderSrc, "VBlur");
	glUseProgram(programVBlur_);
	locVBlurMVPMatrix_ = 
		glGetUniformLocation(programVBlur_, "modelViewProjectionMatrix");
	locVBlurSize_ = glGetUniformLocation(programVBlur_, "blurSize");

	///////////////////////////
	
	
	int maxBufferSegs = 4;
	int maxVerts = (segments_ + maxBufferSegs)*2 + 4;

	glGenBuffers(1, &vertexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*maxVerts, 0, GL_DYNAMIC_DRAW);
	
	int maxTris = (segments_ + maxBufferSegs)*2 + 6 + 100;


	glGenBuffers(1, &indexBuffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*maxTris*3, 0, GL_DYNAMIC_DRAW);


	int maxShadowVerts = 2*maxVerts;

	glGenBuffers(1, &shadowVertexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, shadowVertexBuffer_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*maxShadowVerts, 0, GL_DYNAMIC_DRAW);


	int maxShadowEdges = 4 + 2*segments_ + 1;
	int maxShadowTris = maxShadowEdges * 2;

	glGenBuffers(1, &shadowIndexBuffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shadowIndexBuffer_);	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*maxShadowTris*3, 0, GL_DYNAMIC_DRAW);


	int maxDropShadowVerts = 2 + dropShadowSegments_ * 2 + 1;
	glGenBuffers(1, &bottomShadeVertexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, bottomShadeVertexBuffer_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*maxDropShadowVerts, 0, GL_DYNAMIC_DRAW);

	int maxDropShadowTris = dropShadowSegments_ * 2 + 1;
	glGenBuffers(1, &bottomShadeIndexBuffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottomShadeIndexBuffer_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*3*maxDropShadowTris, 0, GL_DYNAMIC_DRAW);


	static unsigned char pixel[4];

	pixel[0] = 255; pixel[1] = 255; pixel[2] = 255; pixel[3] = 255;
	glGenTextures(1, &blankTexture_);			
	glBindTexture(GL_TEXTURE_2D, blankTexture_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
		1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

	
	renderTexDim_ = 128.0f;

	renderTex1_->init(gl, renderTexDim_ , renderTexDim_, Texture::UncompressedRgba32, 0);
	texRenderer1_->init(gl, renderTexDim_ , renderTexDim_, true);
	texRenderer1_->setTexture(renderTex1_);

	renderTex2_->init(gl, renderTexDim_ , renderTexDim_, Texture::UncompressedRgba32, 0);
	texRenderer2_->init(gl, renderTexDim_ , renderTexDim_, true);
	texRenderer2_->setTexture(renderTex2_);
		
}

void PageFlipOrig::uninit()
{
	if (!gl_) return;
	if (indexBuffer_)
	{
		glDeleteBuffers(1, &indexBuffer_);
		indexBuffer_ = 0;
	}
	
	if (vertexBuffer_)
	{		
		glDeleteBuffers(1, &vertexBuffer_);		
		vertexBuffer_ = 0;		
	}	

	if (bottomShadeIndexBuffer_)
	{		
		glDeleteBuffers(1, &bottomShadeIndexBuffer_);		
		bottomShadeIndexBuffer_ = 0;		
	}	

	if (bottomShadeVertexBuffer_)
	{		
		glDeleteBuffers(1, &bottomShadeVertexBuffer_);		
		bottomShadeVertexBuffer_ = 0;		
	}	

	if (shadowIndexBuffer_)
	{		
		glDeleteBuffers(1, &shadowIndexBuffer_);		
		shadowIndexBuffer_ = 0;		
	}	

	if (shadowVertexBuffer_)
	{		
		glDeleteBuffers(1, &shadowVertexBuffer_);		
		shadowVertexBuffer_ = 0;		
	}	

	if (blankTexture_)
	{
		glDeleteTextures(1, &blankTexture_);
		blankTexture_ = 0;
	}

	if (programFront_) 
	{
		glDeleteProgram(programFront_);
		programFront_ = 0;
	}

	if (programFrontV_) 
	{
		glDeleteProgram(programFrontV_);
		programFrontV_ = 0;
	}

	if (programBack_) 
	{
		glDeleteProgram(programBack_);
		programBack_ = 0;
	}

	if (programBackV_) 
	{
		glDeleteProgram(programBackV_);
		programBackV_ = 0;
	}

	if (programBottom_) 
	{
		glDeleteProgram(programBottom_);
		programBottom_ = 0;
	}

	if (programBottomV_) 
	{
		glDeleteProgram(programBottomV_);
		programBottomV_ = 0;
	}

	if (programHBlur_) 
	{
		glDeleteProgram(programHBlur_);
		programHBlur_ = 0;
	}

	if (programVBlur_) 
	{
		glDeleteProgram(programVBlur_);
		programVBlur_ = 0;
	}

	renderTex1_->uninit();
	texRenderer1_->uninit();

	renderTex2_->uninit();
	texRenderer2_->uninit();

	gl_ = 0;
}

void PageFlipOrig::setTexture(Texture * texture)
{
	if (texture)
		texture_ = texture->glTexture();
	else
		texture_ = 0;
}

void PageFlipOrig::setGlTexture(GLuint texture)
{
	texture_ = texture;
}

bool PageFlipOrig::doPageChange() const
{
	return doPageChange_;
}

Vector2 PageFlipOrig::convertUnitPos(const Vector2 & pos) const
{
	Vector2 ret;
	ret.x = pos.x / width_;
	ret.y = pos.y / height_;
	return ret;
}

Vector2 PageFlipOrig::convertUnitVec(const Vector2 & vec) const
{
	Vector2 ret;
	ret.x = vec.x / width_;
	ret.y = vec.y / height_;
	return ret.normalize();
}

void PageFlipOrig::draw(GfxRenderer * gl)
{
	Vector2 v;
	GLuint useTexture = texture_;
	if (!useTexture) useTexture = blankTexture_;
	Vector2 edgeMidPt = 0.5f*(curEdgeTop_ + curEdgeBottom_);

	Matrix prevCamMatrix = gl->cameraMatrix();
	
	GLuint programFront = programFront_;
	GLuint programBack = programBack_;
	GLuint programBottom = programBottom_;

	if (transitionDirection_ == TransitionDirectionUp || transitionDirection_ == TransitionDirectionDown)
	{
		gl->setCameraMatrix(Matrix::Rotate(M_PI/2.0f, 0.0f, 0.0f, -1.0f) * cameraMatrix_);
		programFront = programFrontV_;
		programBack = programBackV_;
		programBottom = programBottomV_;
	}
	else
	{
		gl->setCameraMatrix(cameraMatrix_);
	}

	gl->pushMatrix();
	gl->loadMatrix(Matrix::Identity());

	gl->clearZBuffer();
	/*
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	*/
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glEnableVertexAttribArray(AttribPosition);
	glVertexAttribPointer(
		AttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0);
	glEnableVertexAttribArray(AttribTexCoord);
	glVertexAttribPointer(
		AttribTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0 + sizeof(Vector3));

	glEnable(GL_CULL_FACE);		
	gl->useCustomProgram(programBack, locBackMVPMatrix_);		
	gl->use(useTexture);
	v = convertUnitPos(edgeMidPt);
	glUniform2f(locBackR_, v.x, v.y);
	v = convertUnitVec(curEdgeVec_);
	glUniform2f(locBackEdge_, v.x, v.y);
	glUniform1f(locBackCurlAmt_, curCurlAmt_);
	glCullFace(GL_FRONT);
	gl->applyCurrentShaderMatrix();
	glDrawElements( GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, 0);
	
	//draw shadows to texture

	texRenderer2_->beginRenderToTexture(gl);
	if (shadowIndices_.size() > 0)
	{		
		
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		gl->setCameraMatrix(cameraMatrix_);
		gl->useColorProgram();		

		gl->applyCurrentShaderMatrix();

		const float sv = 0.8f;
		gl->setColorProgramColor(sv, sv, sv, 1.0f);
		
		glCullFace(GL_FRONT);		
		glDrawElements( GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, 0);

		gl->setColorProgramColor(sv, sv, sv, 0.0f);
		glCullFace(GL_BACK);
		glDrawElements( GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, 0);		

		glDisable(GL_CULL_FACE);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shadowIndexBuffer_);
		glBindBuffer(GL_ARRAY_BUFFER, shadowVertexBuffer_);
		glEnableVertexAttribArray(AttribPosition);
		glVertexAttribPointer(
			AttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0);
		glEnableVertexAttribArray(AttribTexCoord);
		glVertexAttribPointer(
			AttribTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0 + sizeof(Vector3));
		
		
		glStencilFunc(GL_ALWAYS, 0, ~0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		
		gl->setColorProgramColor(sv, sv, sv, 0.0f);
		glDepthMask(false);
		glDrawElements( GL_TRIANGLES, shadowIndices_.size(), GL_UNSIGNED_SHORT, 0);		
		glDepthMask(true);
		

		glStencilFunc(GL_EQUAL, 1, ~0);	
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glDisable(GL_DEPTH_TEST);
		gl->setColorProgramColor(sv, sv, sv, 1.0f);
		gl->drawRect(0.0f, 0.0f, width_, height_);

		glStencilFunc(GL_ALWAYS, 0, ~0);
	
	}
	texRenderer2_->endRenderToTexture(gl);
	
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	//blur
	
	for (int i = 0; i < 1; ++i)
	{
		texRenderer1_->beginRenderToTexture(gl);	
		gl->setCameraMatrix(cameraMatrix_);		
		gl->useCustomProgram(programVBlur_, locVBlurMVPMatrix_);
		glUniform1f(locVBlurSize_, 1.0f/renderTexDim_);	
		gl->use(renderTex2_);
		gl->drawRect(0.0f, 0.0f, width_, height_);
		texRenderer1_->endRenderToTexture(gl);


		texRenderer2_->beginRenderToTexture(gl);	
		gl->setCameraMatrix(cameraMatrix_);	
		gl->useCustomProgram(programHBlur_, locHBlurMVPMatrix_);
		glUniform1f(locHBlurSize_, 1.0f/renderTexDim_);
		gl->use(renderTex1_);
		gl->drawRect(0.0f, 0.0f, width_, height_);
		texRenderer2_->endRenderToTexture(gl);
	}
	
	//end draw shadows to texture
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glEnableVertexAttribArray(AttribPosition);
	glVertexAttribPointer(
		AttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0);
	glEnableVertexAttribArray(AttribTexCoord);
	glVertexAttribPointer(
		AttribTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0 + sizeof(Vector3));


	gl->useCustomProgram(programFront, locFrontMVPMatrix_);	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, useTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, renderTex2_->glTexture());
	glActiveTexture(GL_TEXTURE0);
	v = convertUnitPos(edgeMidPt);
	glUniform2f(locFrontR_, v.x, v.y);
	v = convertUnitVec(curEdgeVec_);
	glUniform2f(locFrontEdge_, v.x, v.y);
	glUniform1f(locFrontCurlAmt_, curCurlAmt_);
	glCullFace(GL_BACK);	
	gl->applyCurrentShaderMatrix();
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, 0);

	
	if (!bottomShadeIndices_.empty())
	{
		glCullFace(GL_BACK);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottomShadeIndexBuffer_);
		glBindBuffer(GL_ARRAY_BUFFER, bottomShadeVertexBuffer_);
		glEnableVertexAttribArray(AttribPosition);
		glVertexAttribPointer(
			AttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0);
		glEnableVertexAttribArray(AttribTexCoord);
		glVertexAttribPointer(
			AttribTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0 + sizeof(Vector3));
		gl->useCustomProgram(programBottom, locBottomMVPMatrix_);
		v = convertUnitPos(edgeMidPt);
		glUniform2f(locBottomR_, v.x, v.y);
		v = convertUnitVec(curEdgeVec_);
		glUniform2f(locBottomEdge_, v.x, v.y);
		gl->applyCurrentShaderMatrix();
		glDrawElements( GL_TRIANGLES, bottomShadeIndices_.size(), GL_UNSIGNED_SHORT, 0);
	}

	gl->setCameraMatrix(prevCamMatrix);
	gl->popMatrix();

	glDepthFunc(GL_ALWAYS);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

}

void PageFlipOrig::pressEvent(const Vector2 & pstartPos, TransitionDirection pageFlipDir)
{
	transitionDirection_ = pageFlipDir;
	computeWH();
	Vector2 startPos = convCoords(pstartPos);
	isPressed_ = true;
	doPageChange_ = false;
	pressedTime_ = Global::currentTime();
	pressedPos_ = startPos;
	t_ = 0.0f;		
	tStep_ = 6.0f;	

	if (transitionDirection_ == TransitionDirectionRight || transitionDirection_ == TransitionDirectionDown)
	{
		startR_ = curR_ = Vector2(width_, height_/2.0f);
		startDir_ = curDir_ = Vector2(-1.0f, 0.0f);
		targetDir_ = pressDir_ = Vector2(-0.5f, 0.5f - startPos.y).normalize();
	}
	else
	{		
		startR_ = curR_ = Vector2(0.0f, height_/2.0f);
		startDir_ = curDir_ = Vector2(-1.0f, 0.0f);
		targetDir_ = pressDir_ = Vector2(-1.0f, 0.0f);
	}

	moveEvent(startPos);
}

Vector2 PageFlipOrig::convCoords(const Vector2 & pos)
{
	Vector2 ret = pos;
	
	if (transitionDirection_ == TransitionDirectionUp || transitionDirection_ == TransitionDirectionDown)
	{
		ret.x = pos.y;
		ret.y = 1.0f - pos.x;
	}

	return ret;
}

void PageFlipOrig::releaseEvent(const Vector2 & ppos)
{	
	Vector2 pos = convCoords(ppos);

	if (!isPressed_) return;

	isPressed_ = false;
	t_ = 0.0f;	
	tStep_ = 3.5f;
	startR_ = curR_;
	startDir_ = curDir_;	
	
	targetDir_ = curDir_;
	targetDir_.y = 0.0f;
	targetDir_ = targetDir_.normalize();

	float gestureDuration = Global::currentTime() - pressedTime_;
	bool isQuickFlick = gestureDuration < Global::flickTime();
	if (transitionDirection_ == TransitionDirectionRight || transitionDirection_ == TransitionDirectionDown)
	{
		if ((!isQuickFlick && pos.x > 0.5f) || (isQuickFlick && pos.x > pressedPos_.x))
			targetR_ = Vector2(width_, height_/2.0f);
		else
			targetR_ = Vector2(0.0f, height_/2.0f);
		
	}
	else
	{
		if ((!isQuickFlick && pos.x < 0.5f) || (isQuickFlick && pos.x < pressedPos_.x))
			targetR_ = Vector2(0.0f, height_/2.0f);
		else
			targetR_ = Vector2(width_, height_/2.0f);
	}
}

void PageFlipOrig::moveEvent(const Vector2 & ppos)
{	
	Vector2 pos = convCoords(ppos);

	if (isPressed_)
	{
		const float maxY = 4.0f;
		const float t0 = 0.0f;
		const float t1 = 0.1f;

		Vector2 dirVec;
			
		if (transitionDirection_ == TransitionDirectionRight || transitionDirection_ == TransitionDirectionDown)
		{
			dirVec = pos - Vector2(1.0f, pressedPos_.y);	
			if (dirVec.x > 0.0f) dirVec.x = 0.0f;
		}
		else
		{
			dirVec = Vector2(0.0f, pressedPos_.y) - pos;	
			if (dirVec.x > 0.0f) dirVec.x = 0.0f;
		}		

		float t = (fabs(dirVec.x) - t0)/(t1 - t0);
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;

		if (dirVec.x == 0.0f) dirVec = Vector2(-1.0f, 0.0f);

		float sx = fabs(dirVec.x);
		dirVec.x /= sx;
		dirVec.y /= sx;

		if (dirVec.y < -maxY) dirVec.y = -maxY;
		if (dirVec.y > maxY) dirVec.y = maxY;

		//dirVec.y *= t;
		dirVec = pressDir_ + t * (dirVec - pressDir_);

		dirVec = dirVec.normalize();

		needComputeVertex_ = true;
		targetDir_ = dirVec;				
		
		Ray ray = camera_.unproject(Vector2(2.0f * pos.x - 1.0f, 1.0f - 2.0 * pos.y));
		static Plane plane = Plane::fromPointNormal(Vector3(width_/2.0f, height_/2.0f, -rRadius_), Vector3(0.0f, 0.0f, -1.0f));
		Vector3 intPt;
		plane.intersect(&intPt, ray);		
		targetR_ = solveForRx2(dirVec, intPt.y, intPt.x);
		//targetR_ = pos;
		//targetR_.x *= 2.5f;		
	}
}

TransitionDirection PageFlipOrig::curTransitionDirection() const
{
	return transitionDirection_;
}


Vector2 PageFlipOrig::solveForRx2(const Vector2 & rollingDir, float height, float edgeX) const
{
	const float EPSILON = 0.0001f;

	float curEdgeX = 0.0f;
	float minGuessDist = 0.0f; 
	float maxGuessDist = sqrt(2.0f);
	float guessDist = 0.5f*(minGuessDist + maxGuessDist);
	Vector2 startPt(edgeX, height);
	Vector2 guessPt;

	for (int i = 0; i < 20; ++i)
	{
		guessPt = startPt + guessDist * rollingDir;
		curEdgeX = computeEdgeX2(guessPt, rollingDir, height);

		if (curEdgeX < edgeX - EPSILON)
		{
			maxGuessDist = guessDist;
			guessDist = 0.5f*(minGuessDist + guessDist);
		}
		else if (curEdgeX > edgeX + EPSILON)
		{
			minGuessDist = guessDist;
			guessDist = 0.5f*(maxGuessDist + guessDist);
		}
		else break;
	}
	
	return guessPt;	
}

float PageFlipOrig::computeEdgeX2(const Vector2 & pRollingPinPos, const Vector2 & pRollingPinDir, float height) const
{	
	Vector2 rollingPos(pRollingPinPos);
	Vector2 rollingDir(pRollingPinDir);
	
	Vector2 rAxis(rollingDir.y, -rollingDir.x);
	float t;
		
	t = (0.0f -rollingPos.y) / rAxis.y;
	Vector2 intPtTop = rollingPos + t*rAxis;
	if (intPtTop.x < 0.0f) intPtTop = Vector2(0.0f, 0.0f);

	t = (height_ -rollingPos.y) / rAxis.y;
	Vector2 intPtBottom = rollingPos + t*rAxis;
	if (intPtBottom.x < 0.0f) intPtBottom = Vector2(0.0f, height_);

	float f;
	f = min(intPtBottom.x, 1.0f) - 1.0f;
	f *= f;
	f *= f;
	float bottomRRadius = rRadius_ * (1 - f);
	f = min(intPtTop.x, 1.0f) - 1.0f;
	f *= f;
	f *= f;
	float topRRadius = rRadius_ * (1 - f);		

	rAxis = intPtBottom - intPtTop;
	rollingDir = Vector2(-rAxis.y, rAxis.x).normalize();
	rollingPos = intPtTop;
	
	//TODO: possibly make this more accurate
	float curRRadius = topRRadius + height * (bottomRRadius - topRRadius);
	float curHalfCircum = M_PI * curRRadius;	

	t = (height + curRRadius * rollingDir.y) / rAxis.y;

	float edgeX = (intPtTop + t * rAxis - curRRadius * rollingDir).x;	
	if (edgeX > width_) edgeX = width_;

	return edgeX;	
}

void PageFlipOrig::computeEdgeVec(const Vector2 & pRollingPos, const Vector2 & pRollingDir, Vector2 * edgeTopOut, Vector2 * edgeBottomOut) const
{	
	Vector2 rollingPos(pRollingPos);
	Vector2 rollingDir(pRollingDir);

	Vector2 rAxis(rollingDir.y, -rollingDir.x);
	float t;
		
	t = (0.0f -rollingPos.y) / rAxis.y;
	Vector2 intPtTop = rollingPos + t*rAxis;
	if (intPtTop.x < 0.0f) intPtTop = Vector2(0.0f, 0.0f);

	t = (height_ -rollingPos.y) / rAxis.y;
	Vector2 intPtBottom = rollingPos + t*rAxis;
	if (intPtBottom.x < 0.0f) intPtBottom = Vector2(0.0f, height_);

	float f;
	f = min(intPtBottom.x, 1.0f) - 1.0f;
	f *= f;
	f *= f;
	float bottomRRadius = rRadius_ * (1 - f);
	f = min(intPtTop.x, 1.0f) - 1.0f;
	f *= f;
	f *= f;
	float topRRadius = rRadius_ * (1 - f);		

	rAxis = intPtBottom - intPtTop;
	rollingDir = Vector2(-rAxis.y, rAxis.x).normalize();
	rollingPos = intPtTop;


	Vector3 intPtBottom3(intPtBottom.x, intPtBottom.y, -bottomRRadius);
	Vector3 intPtTop3(intPtTop.x, intPtTop.y, -topRRadius);
	Vector3 rollingDir3(rollingDir.x, rollingDir.y, 0.0f);

	Vector3 edgeBottom = intPtBottom3 - bottomRRadius * rollingDir3;
	Vector3 edgeTop = intPtTop3 - topRRadius * rollingDir3;
	
	edgeBottom = camera_.project(edgeBottom);
	edgeBottom.x = width_*0.5f*(edgeBottom.x + 1.0f);
	edgeBottom.y = height_*0.5f*(1.0f - edgeBottom.y);

	edgeTop = camera_.project(edgeTop);
	edgeTop.x = width_*0.5f*(edgeTop.x + 1.0f);
	edgeTop.y = height_*0.5f*(1.0f - edgeTop.y);


	Vector2 edgeTop2(edgeTop.x, edgeTop.y);
	Vector2 edgeBottom2(edgeBottom.x, edgeBottom.y);

	Vector2 edgeAxis = (edgeBottom2 - edgeTop2).normalize();
	computeIntPts(edgeTop2, edgeAxis, &edgeTop2, &edgeBottom2);

	*edgeTopOut = edgeTop2;
	*edgeBottomOut = edgeBottom2;
}

void PageFlipOrig::computeVerts(const Vector2 & pRollingPinPos, const Vector2 & pRollingPinDir)
{	
	//cout << pRollingPinPos.x << ", " << pRollingPinPos.y << " -- " << pRollingPinDir.x << ", " << pRollingPinDir.y << "\n";
	Vector2 rollingPos(pRollingPinPos);
	Vector2 rollingDir(pRollingPinDir);
	
	Vector2 rAxis(rollingDir.y, -rollingDir.x);
	float t;
		
	t = (0.0f -rollingPos.y) / rAxis.y;
	Vector2 intPtTop = rollingPos + t*rAxis;
	if (intPtTop.x < 0.0f) intPtTop = Vector2(0.0f, 0.0f);

	t = (height_ -rollingPos.y) / rAxis.y;
	Vector2 intPtBottom = rollingPos + t*rAxis;
	if (intPtBottom.x < 0.0f) intPtBottom = Vector2(0.0f, height_);

	float f;
	f = min(intPtBottom.x, 1.0f) - 1.0f;
	f *= f;
	f *= f;
	float bottomRRadius = rRadius_ * (1 - f);
	f = min(intPtTop.x, 1.0f) - 1.0f;
	f *= f;
	f *= f;
	float topRRadius = rRadius_ * (1 - f);	

	rAxis = intPtBottom - intPtTop;
	rollingDir = Vector2(-rAxis.y, rAxis.x).normalize();
	rollingPos = intPtTop;
	Vector3 rollingDir3(rollingDir.x, rollingDir.y, 0.0f);
	
	float axisLen = (intPtBottom - intPtTop).magnitude();
	rAxis = (1.0f/axisLen) * rAxis;

	makeMesh(rollingPos, rAxis, bottomRRadius, topRRadius);
	/*
	FILE * file = fopen("e:\\st\\paper.obj", "wb");
	for (int i = 0; i < (int)vertices_.size(); ++i)
	{
		fprintf(file, "v %f %f %f\n", vertices_[i].position.x, vertices_[i].position.y, vertices_[i].position.z);
	}

	for (int i = 0; i < (int)indices_.size(); i += 3)
	{
		fprintf(file, "f %d %d %d\n", indices_[i] + 1, indices_[i + 1] + 1, indices_[i + 2] + 1);
	}
	fclose(file);
	*/
	float maxCurlAmt = 0.0f;
	Vector2 origPos;
	for (int i = 0; i < (int)vertices_.size(); ++i)
	{
		origPos.x = vertices_[i].position.x;
		origPos.y = vertices_[i].position.y;

		Vector2 toRollingPinPosVec = rollingPos - origPos;
		float projDist = toRollingPinPosVec * rollingDir;
		Vector2 projVec = projDist * rollingDir;		
		float distFromRollingAxis = projDist;

		float curRRadius = topRRadius;
		if ((origPos - intPtTop) * rAxis > 0.0f)
		{
			float t = (origPos + projVec - intPtTop).magnitude() / axisLen;
			curRRadius = topRRadius + t * (bottomRRadius - topRRadius);
		}
		float curHalfCircum = M_PI * curRRadius;

		if (projDist > 0.0f)
		{
			float curlAmt = distFromRollingAxis / curHalfCircum;
			if (curlAmt > maxCurlAmt) maxCurlAmt = curlAmt;
			if (distFromRollingAxis <= curHalfCircum)
			{
				Vector3 p(origPos.x + projVec.x, origPos.y + projVec.y, -curRRadius);				
				float rollAngle = M_PI * curlAmt;

				float x = curRRadius * cos(rollAngle);
				float y = curRRadius * sin(rollAngle);

				vertices_[i].position = p + x*Vector3(0.0f, 0.0f, 1.0f) - y*rollingDir3;
			}
			else
			{
				Vector3 p(origPos.x + projVec.x, origPos.y + projVec.y, -2*curRRadius);
				vertices_[i].position = p + (distFromRollingAxis - curHalfCircum) * rollingDir3;
			}
		}
		else
		{
			vertices_[i].position.x = origPos.x;
			vertices_[i].position.y = origPos.y;
			vertices_[i].position.z = 0.0f;			
		}

		
	}
	
	computeShadowPolys();


	curCurlAmt_ = maxCurlAmt;
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*vertices_.size(), &vertices_[0]);	
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);	
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort)*indices_.size(), &indices_[0]);

	if (bottomShadeIndices_.size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, bottomShadeVertexBuffer_);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*bottomShadeVertices_.size(), &bottomShadeVertices_[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottomShadeIndexBuffer_);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort)*bottomShadeIndices_.size(), &bottomShadeIndices_[0]);
	}

	
	if (shadowIndices_.size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, shadowVertexBuffer_);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*shadowVertices_.size(), &shadowVertices_[0]);	
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shadowIndexBuffer_);	
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort)*shadowIndices_.size(), &shadowIndices_[0]);

	}

}





inline float sideTest(const Vector2 & intPt1, const Vector2 & intPt2, const Vector2 & pt)
{
	return (
		Vector3(intPt2.x - intPt1.x, intPt2.y - intPt1.y, 0.0f) ^ 
		Vector3(pt.x - intPt1.x, pt.y - intPt1.y, 0.0f)
		).z;
}

inline int getNextIndex(int i, int length)
{
	if (i < length - 1) return i + 1;
	else return 0;
}

void PageFlipOrig::cutPolyOnSide(const std::vector<Vector2> & origVerts, const Vector2 & intPt1, const Vector2 & intPt2, float side, std::vector<Vector2> * outVerts) const
{	
	int startIndex = -1;

	for (int i = 0; i < (int)origVerts.size(); ++i)
	{
		if (side * sideTest(intPt1, intPt2, origVerts[i]) >= 0.0f)
		{
			startIndex = i;
			break;
		}
	}

	int len = (int)origVerts.size();
	bool xDifferent = false, yDifferent = false;
	float lastX, lastY;
	for (int i = startIndex, j = 0; j < len; i = getNextIndex(i, len), ++j)
	{
		if (side * sideTest(intPt1, intPt2, origVerts[i]) >= 0.0f)	
		{
			outVerts->push_back(origVerts[i]);
			if (i != startIndex)
			{
				if (origVerts[i].x != lastX) xDifferent = true;
				if (origVerts[i].y != lastY) yDifferent = true;
			}

			lastX = origVerts[i].x;
			lastY = origVerts[i].y;
			
		}
	}

	if (!xDifferent || !yDifferent) outVerts->clear();
}

void PageFlipOrig::insertVert(const Vector2 & intPt, std::vector<Vector2> * porigVerts) const
{	
	int index = 0;	
	vector<Vector2> & origVerts = *porigVerts;
	while (1)
	{
		int nextIndex = getNextIndex(index, (int)origVerts.size());
		
		if (origVerts[index].x == origVerts[nextIndex].x && intPt.x == origVerts[index].x)
		{
			bool isInBetween = (origVerts[index].y - intPt.y) * (intPt.y - origVerts[nextIndex].y) > 0.0f;
			if (isInBetween)				
				origVerts.insert(origVerts.begin() + nextIndex, intPt);					
			break;				
		}
		else if (origVerts[index].y == origVerts[nextIndex].y && intPt.y == origVerts[index].y)
		{
			bool isInBetween = (origVerts[index].x - intPt.x) * (intPt.x - origVerts[nextIndex].x) > 0.0f;
			if (isInBetween)				
				origVerts.insert(origVerts.begin() + nextIndex, intPt);					
			break;				
		}

		index = nextIndex;
	}
}

void PageFlipOrig::cutPoly(const std::vector<Vector2> & origVerts, const Vector2 & intPt1, const Vector2 & intPt2, std::vector<Vector2> * outVerts1, std::vector<Vector2> * outVerts2) const
{	
	outVerts1->clear();
	outVerts2->clear();
	vector<Vector2> origVertsCopy = origVerts;
	insertVert(intPt1, &origVertsCopy);
	insertVert(intPt2, &origVertsCopy);
		
	cutPolyOnSide(origVertsCopy, intPt1, intPt2, 1, outVerts1);
	cutPolyOnSide(origVertsCopy, intPt1, intPt2, -1, outVerts2);
	
}

inline void doFltErr(Vector2 & intPt, const float & width, const  float & height)
{
	const float EPSILON = 0.00001f;
	if (fabs(intPt.x) < EPSILON) intPt.x = 0.0f;
	if (fabs(intPt.x - width) < EPSILON) intPt.x = width;
	if (fabs(intPt.y) < EPSILON) intPt.y = 0.0f;
	if (fabs(intPt.y - height) < EPSILON) intPt.y = height;
}

bool PageFlipOrig::computeIntPts(const Vector2 & r, const Vector2 & axis, Vector2 * intPt1Out, Vector2 * intPt2Out) const
{
	float t;
	Vector2 intPtMinusT, intPtPlusT;
	Vector2 intPt;	

	int intCounter = 0;
	float ts[4];
	Vector2 intPts[4];
	
	if (axis.y != 0.0f)
	{
		//intersect against top
		t = -r.y/axis.y;
		intPt = r + t * axis;
		doFltErr(intPt, width_, height_);
		if (intPt.x >= 0.0f && intPt.x <= width_)
		{
			ts[intCounter] = t;			
			intPts[intCounter++] = intPt;
		}

		//intersect against bottom
		t = (height_ - r.y)/axis.y;
		intPt = r + t * axis;
		doFltErr(intPt, width_, height_);
		if (intPt.x >= 0.0f && intPt.x <= width_)
		{
			ts[intCounter] = t;			
			intPts[intCounter++] = intPt;
		}
	}

	if (axis.x != 0.0f)
	{
		//intersect against left
		t = -r.x/axis.x;
		intPt = r + t * axis;
		doFltErr(intPt, width_, height_);
		if (intPt.y > 0.0f && intPt.y < height_)
		{
			ts[intCounter] = t;			
			intPts[intCounter++] = intPt;
		}

		//intersect against right
		t = (width_ - r.x)/axis.x;
		intPt = r + t * axis;		
		doFltErr(intPt, width_, height_);
		if (intPt.y > 0.0f && intPt.y < height_)
		{
			ts[intCounter] = t;			
			intPts[intCounter++] = intPt;
		}
	}

	if (intCounter < 2) return false;

	if (ts[0] > ts[1]) swap(intPts[0], intPts[1]);
	
	*intPt2Out = intPts[1];
	*intPt1Out = intPts[0];

	return true;
}


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

void PageFlipOrig::makeMesh(const Vector2 & r, const Vector2 & axis, float bottomRadius, float topRadius)
{
	static vector<Vector2> verts, verts1, verts2;
	Vector2 intPt1, intPt2;
	map<Vector2, int, Vector2Compare> vertIndices;
	map<Vector2, int, Vector2Compare>::iterator miter;

	verts.clear();
	verts1.clear();
	verts2.clear();

	verts.push_back(Vector2(0.0f, 0.0f));
	verts.push_back(Vector2(width_, 0.0f));
	verts.push_back(Vector2(width_, height_));
	verts.push_back(Vector2(0.0f, height_));

	vertices_.clear();
	indices_.clear();


	float bottomStep = (M_PI*bottomRadius)/(segments_ - 1);
	float topStep = (M_PI*topRadius)/(segments_ - 1);

	float topT = 0.0f;
	float bottomT = 0.0f;
	int curSeg = 0;

	Vector2 splitDir(axis.y, -axis.x);
	
	Vector2 startIntPt1, startIntPt2;
	computeIntPts(r, axis, &startIntPt1, &startIntPt2);

	static vector< pair<Vector2, Vector2> > intPts;

	intPts.clear();
	/*
	float bufferLen = 0.25f;
	int bufferSegs = 2;
	float bufferStep = bufferLen / bufferSegs;
	for(curSeg = bufferSegs; curSeg >= 1; curSeg--)
	{
		Vector2 rNew = startIntPt1 - curSeg*bufferStep*splitDir;
		Vector2 axisNew = startIntPt2 - curSeg*bufferStep*splitDir - rNew;
		if (!computeIntPts(rNew, axisNew, &intPt1, &intPt2)) break;
		intPts.push_back(make_pair(intPt1, intPt2));
	}

	*/
/*
	float l0 = startIntPt1.x;
	float l1 = startIntPt2.x;
	int numSeg = 5;
	for (int i = 0; i < numSeg; ++i)
	{
		float f = (float)i/numSeg;
		Vector2 rNew(f*l0, 0.0f);		
		Vector2 axisNew = Vector2(f*l1, height_) - rNew;
		if (!computeIntPts(rNew, axisNew, &intPt1, &intPt2)) break;
		intPts.push_back(make_pair(intPt1, intPt2));
	}
	*/
	for(curSeg = 0; curSeg < segments_; curSeg++)
	{
		Vector2 rNew = startIntPt1 + curSeg*topStep*splitDir;
		Vector2 axisNew = startIntPt2 + curSeg*bottomStep*splitDir - rNew;
		if (!computeIntPts(rNew, axisNew, &intPt1, &intPt2)) break;

		intPts.push_back(make_pair(intPt1, intPt2));
	}

#ifdef PER_VERTEX
	Vector2 endBufferIntPt1 = intPt1, endBufferIntPt2 = intPt2;
	float endBufferLen = 0.175f;
	int endBufferSegs = 2;
	float endBufferStep = endBufferLen / endBufferSegs;
	for(curSeg = 1; curSeg <= endBufferSegs; curSeg++)
	{
		Vector2 rNew = endBufferIntPt1 + curSeg*endBufferStep*splitDir;
		Vector2 axisNew = endBufferIntPt2 + curSeg*endBufferStep*splitDir - rNew;
		if (!computeIntPts(rNew, axisNew, &intPt1, &intPt2)) break;
		intPts.push_back(make_pair(intPt1, intPt2));
	}
#endif

	for(int i = 0; i < (int)intPts.size(); ++i)
	{
		if (verts.empty()) break;

		cutPoly(verts, intPts[i].first, intPts[i].second, &verts1, &verts2);

		for (int i = 0; i < (int)verts1.size(); ++i)
		{
			int index = (int)vertIndices.size();
			if (vertIndices.find(verts1[i]) == vertIndices.end())
				vertIndices[verts1[i]] = index;				
		}

		//triangulate

		for (int i = 1; i < (int)verts1.size() - 1; ++i)
		{

			indices_.push_back(vertIndices[verts1[0]]);
			indices_.push_back(vertIndices[verts1[i + 1]]);
			indices_.push_back(vertIndices[verts1[i]]);			
		}

		//keep processing towards verts2 side
		
		verts = verts2;
	}
	
	//triangulate remaining portion
	for (int i = 0; i < (int)verts.size(); ++i)
	{
		int index = (int)vertIndices.size();
		if (vertIndices.find(verts[i]) == vertIndices.end())
			vertIndices[verts[i]] = index;			
	}

	//triangulate

	for (int i = 1; i < (int)verts.size() - 1; ++i)
	{
		indices_.push_back(vertIndices[verts[0]]);
		indices_.push_back(vertIndices[verts[i + 1]]);
		indices_.push_back(vertIndices[verts[i]]);		
	}

	vertices_.resize(vertIndices.size());

	for (miter = vertIndices.begin(); miter != vertIndices.end(); ++miter)	
	{
		int index = (*miter).second;	
		vertices_[index].texCoord.x = (*miter).first.x/width_;
		vertices_[index].texCoord.y = (*miter).first.y/height_;
		vertices_[index].position.x =(*miter).first.x;
		vertices_[index].position.y = (*miter).first.y;
		vertices_[index].position.z = 0.0f;

	}

	//compute page curl edge data
	computeEdgeVec(curR_, curDir_, &curEdgeTop_, &curEdgeBottom_);

	curEdgeVec_ = (curEdgeBottom_ - curEdgeTop_).normalize();


	//compute the drop shadow mesh
	
	bottomShadeVertices_.clear();
	bottomShadeIndices_.clear();
	float bottomShadeDist = 0.2f * max(width_, 1.0f/width_);

	verts.clear();
	verts1.clear();
	verts2.clear();
	vertIndices.clear();
	verts.push_back(Vector2(0.0f, 0.0f));
	verts.push_back(Vector2(width_, 0.0f));
	verts.push_back(Vector2(width_, height_));
	verts.push_back(Vector2(0.0f, height_));

	splitDir = Vector2(curEdgeVec_.y, -curEdgeVec_.x);
	bool startInt = computeIntPts(r, axis, &startIntPt1, &startIntPt2);
	
	computeIntPts(curEdgeTop_, curEdgeVec_, &startIntPt1, &startIntPt2);
		
	float step = bottomShadeDist/(dropShadowSegments_ - 1);
	for(curSeg = 0; curSeg < dropShadowSegments_; curSeg++)
	{
		bool ranOut = false;
		if (verts.empty()) break;
		Vector2 rNew = startIntPt1 + curSeg*step*splitDir;
		Vector2 axisNew = startIntPt2 + curSeg*step*splitDir - rNew;
		if (!computeIntPts(rNew, axisNew, &intPt1, &intPt2)) 
		{
			ranOut = true;
			verts1 = verts2;
		}
		else
			cutPoly(verts, intPt1, intPt2, &verts1, &verts2);

		for (int i = 0; i < (int)verts1.size(); ++i)
		{
			int index = (int)vertIndices.size();
			if (vertIndices.find(verts1[i]) == vertIndices.end())
				vertIndices[verts1[i]] = index;				
		}

		//triangulate

		for (int i = 1; i < (int)verts1.size() - 1; ++i)
		{
			bottomShadeIndices_.push_back(vertIndices[verts1[0]]);
			bottomShadeIndices_.push_back(vertIndices[verts1[i + 1]]);
			bottomShadeIndices_.push_back(vertIndices[verts1[i]]);			
		}

		//keep processing towards verts2 side
			
		verts = verts2;
		if (ranOut) break;
	}

	bottomShadeVertices_.resize(vertIndices.size());

	for (miter = vertIndices.begin(); miter != vertIndices.end(); ++miter)	
	{
		int index = (*miter).second;	
		bottomShadeVertices_[index].texCoord.x = (*miter).first.x/width_;
		bottomShadeVertices_[index].texCoord.y = (*miter).first.y/height_;
		bottomShadeVertices_[index].position.x =(*miter).first.x;
		bottomShadeVertices_[index].position.y = (*miter).first.y;
		bottomShadeVertices_[index].position.z = 0.0001f;
	}
	
}


struct EdgeData
{
	unsigned short v1, v2;
	unsigned char numAdjTris;
	bool reversed;
	//float facing[2];

	EdgeData()
	{
		v1 = v2 = 0;
		numAdjTris = 0;
		reversed = false;
	}

	bool isBorderEdge() const
	{
		//return numAdjTris == 1 || facing[0] != facing[1];
		return numAdjTris == 1;
	}

	bool operator < (const EdgeData & rhs) const
	{
		if (v1 != rhs.v1) return v1 < rhs.v1;
		return v2 < rhs.v2;
	}
};
void PageFlipOrig::computeShadowPolys()
{
	set<EdgeData> edges;
	for (int i = 0; i < (int)indices_.size(); i += 3)
	{	
		/*
		Vector3 e1 = vertices_[indices_[i]].position - vertices_[indices_[i + 1]].position;
		Vector3 e2 = vertices_[indices_[i]].position - vertices_[indices_[i + 2]].position;
		Vector3 normal = (e1 ^ e2).normalize();
		float facing = normal * (shadowSrc_ - vertices_[indices_[i]].position);
		*/
		for (int k = 0; k < 3; ++k)
		{
			EdgeData newEdgeData;
			newEdgeData.v1 = indices_[i + k];
			newEdgeData.v2 = indices_[i + ((k == 2)?0:k + 1)];
			if (newEdgeData.v1 > newEdgeData.v2) 
			{
				swap(newEdgeData.v1, newEdgeData.v2);
				newEdgeData.reversed = true;
			}
			
			set<EdgeData>::iterator iter = edges.insert(newEdgeData).first;

			EdgeData & edgeData = const_cast<EdgeData &>(*iter);
			if (edgeData.numAdjTris < 2)
			{
				//edgeData.facing[edgeData.numAdjTris] = facing;
				edgeData.numAdjTris++;
			}
		}
	}

	set<EdgeData>::iterator iter;
	map<Vector3, int, Vector3Compare> shadowVertIndices;
	map<Vector3, int, Vector3Compare>::iterator miter;
		
	shadowIndices_.clear();
	shadowVertices_.clear();
	int shadowEdges = 0;

	for (iter = edges.begin(); iter != edges.end(); ++iter)
	{
		const EdgeData & edgeData = *iter;
		
		unsigned short v1 = edgeData.v1;
		unsigned short v2 = edgeData.v2;

		if (edgeData.reversed) swap(v1, v2);

		bool shadowEdge = false;
/*
		if (edgeData.numAdjTris == 2)
		{			
			if (edgeData.facing[0] * edgeData.facing[1] < 0.0f)
			{
				shadowEdge = true;
				shadowEdges++;
			}
		}
		else */if (edgeData.numAdjTris == 1)
		{
			if (vertices_[v1].position.z < 0.0f || vertices_[v2].position.z < 0.0f)			
				shadowEdge = true;			
		}

		if (shadowEdge)
		{
			Vector3 fromLight;
			float t;

			fromLight = vertices_[v2].position - shadowSrc_;			
			t = -shadowSrc_.z / fromLight.z;
			Vector3 v3Pos = shadowSrc_ + t * fromLight;
			//v3Pos = vertices_[v2].position +  Vector3(0.0f, 0.01f, 0.0f);

			fromLight = vertices_[v1].position - shadowSrc_;			
			t = -shadowSrc_.z / fromLight.z;
			Vector3 v4Pos = shadowSrc_ + t * fromLight;
			//v4Pos = vertices_[v1].position + Vector3(0.0f, 0.01f, 0.0f);

			int sv1 = (int)shadowVertIndices.size();
			miter = shadowVertIndices.find(vertices_[v1].position);
			if (miter == shadowVertIndices.end()) shadowVertIndices[vertices_[v1].position] = sv1;
			else sv1 = (*miter).second;

			int sv2 = (int)shadowVertIndices.size();
			miter = shadowVertIndices.find(vertices_[v2].position);
			if (miter == shadowVertIndices.end()) shadowVertIndices[vertices_[v2].position] = sv2;
			else sv2 = (*miter).second;

			int sv3 = (int)shadowVertIndices.size();
			miter = shadowVertIndices.find(v3Pos);
			if (miter == shadowVertIndices.end()) shadowVertIndices[v3Pos] = sv3;
			else sv3 = (*miter).second;

			int sv4 = (int)shadowVertIndices.size();
			miter = shadowVertIndices.find(v4Pos);
			if (miter == shadowVertIndices.end()) shadowVertIndices[v4Pos] = sv4;
			else sv4 = (*miter).second;

			if (sv1 != sv2 && sv1 != sv3 && sv2 != sv3)
			{
				shadowIndices_.push_back(sv1);
				shadowIndices_.push_back(sv2);
				shadowIndices_.push_back(sv3);
			}

			if (sv1 != sv3 && sv1 != sv4 && sv3 != sv4)
			{
				shadowIndices_.push_back(sv1);
				shadowIndices_.push_back(sv3);
				shadowIndices_.push_back(sv4);
			}
			
		}
		
	}


	shadowVertices_.resize(shadowVertIndices.size());
	
	for (miter = shadowVertIndices.begin(); miter != shadowVertIndices.end(); ++miter)	
	{
		int index = (*miter).second;
		shadowVertices_[index].texCoord.x = 0.0f;
		shadowVertices_[index].texCoord.y = 0.0f;
		shadowVertices_[index].position = (*miter).first;
	}

/*
	cout << "shadowEdges " << shadowEdges << "\n";
	cout << "shadow " << shadowVertices_.size() << " - " << shadowIndices_.size()/3 << " - " << shadowEdges << "\n";
	cout << "reg " << vertices_.size() << " - " << indices_.size()/3 << "\n";

	FILE * file = fopen("c:\\temp\\paper.obj", "wb");
	for (int i = 0; i < (int)vertices_.size(); ++i)
	{
		fprintf(file, "v %f %f %f\n", vertices_[i].position.x, vertices_[i].position.y, vertices_[i].position.z);
	}
	
	for (int i = 0; i < (int)indices_.size(); i += 3)
	{
		fprintf(file, "f %d %d %d\n", indices_[i] + 1, indices_[i + 1] + 1, indices_[i + 2] + 1);
	}
	fclose(file);

	file = fopen("c:\\temp\\shadow.obj", "wb");
	for (int i = 0; i < (int)shadowVertices_.size(); ++i)
	{
		fprintf(file, "v %f %f %f\n", shadowVertices_[i].position.x, shadowVertices_[i].position.y, shadowVertices_[i].position.z);
	}
	
	for (int i = 0; i < (int)shadowIndices_.size(); i += 3)
	{
		fprintf(file, "f %d %d %d\n", shadowIndices_[i] + 1, shadowIndices_[i + 1] + 1, shadowIndices_[i + 2] + 1);
	}

	fclose(file);
	*/
}

void PageFlipOrig::setPageDims(float w, float h)
{
	docW_ = w;
	docH_ = h;
}