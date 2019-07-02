#include "stdafx.h"
#include "PageFlip.h"
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






const char gWriteDepthShaderSrc[] =
"\n\
precision highp float;\n\
varying vec4 vPosition;\n\
\n\
vec4 pack(float depth)\n\
{\n\
    const vec4 bitSh = vec4(256.0 * 256.0 * 256.0, 256.0 * 256.0, 256.0, 1.0);\n\
    const vec4 bitMsk = vec4(0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);\n\
    vec4 comp = fract(depth * bitSh);\n\
    comp -= comp.xxyz * bitMsk;\n\
    return comp;\n\
}\n\
void main()\n\
{\n\
	gl_FragColor = pack(vPosition.z / vPosition.w);\n\
}\n\
";

const char gWriteDepthVertShaderSrc[] =
"\
attribute vec4 position;\
uniform mat4 modelViewProjectionMatrix;\n\
varying vec4 vPosition;\n\
\n\
void main()\n\
{\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	vPosition = gl_Position;\n\
}\n\
";

const char gDepthShaderSrc[] =
"\n\
precision highp float;\n\
uniform mat4 lightMVP;\n\
varying vec4 vPosition;\n\
uniform sampler2D tex0;\n\
\n\
\n\
float unpack(vec4 packedZValue)\n\
{\n\
    const vec4 bitShifts = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);\n\
    return dot(packedZValue , bitShifts);\n\
}\n\
void main()\n\
{\
	vec4 camCoord = lightMVP*vPosition;\n\
	camCoord.x = 0.5*(1.0 + camCoord.x / camCoord.w);\n\
	camCoord.y = 0.5*(1.0 + camCoord.y / camCoord.w);\n\
	camCoord.z /= camCoord.w;\n\
	float depthZ = unpack(texture2D(tex0, camCoord.xy));\n\
	if (depthZ < camCoord.z)\n\
		gl_FragColor = vec4(0.6, 0.6, 0.6, 1.0);\n\
	else\n\
		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n\
}\n\
";

const char gDepthVertShaderSrc[] =
"\
attribute vec4 position;\n\
uniform mat4 modelViewProjectionMatrix;\n\
varying vec4 vPosition;\n\
\n\
void main()\n\
{\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	vPosition = position;\n\
}\n\
";

//#define PER_VERTEX 1

const char gFrontShaderSrc[] =
"precision mediump float;\n\
varying vec2 vTexCoord;\n\
varying vec3 vNormal;\n\
uniform sampler2D tex0;\n\
uniform float lightT;\n\
const vec3 lightVec = normalize(vec3(-0.4, 0.0, -1.0));\n\
\n\
void main()\n\
{\
	gl_FragColor = texture2D(tex0, vTexCoord.st);\n\
	float dotVal = dot(vNormal, lightVec);\n\
	float lighting = 0.2 + 0.8*dotVal;\n\
	lighting = mix(lighting, 1.0, lightT);\n\
	gl_FragColor.rgb *= lighting;\n\
}\n\
";

const char gFrontShaderSrcV[] =
"precision mediump float;\n\
varying vec2 vTexCoord;\n\
varying vec3 vNormal;\n\
uniform sampler2D tex0;\n\
uniform float lightT;\n\
const vec3 lightVec = normalize(vec3(-0.4, 0.0, -1.0));\n\
\n\
void main()\n\
{\
	gl_FragColor = texture2D(tex0, vec2(1.0 - vTexCoord.t, vTexCoord.s));\n\
	float dotVal = dot(vNormal, lightVec);\n\
	float lighting = 0.2 + 0.8*dotVal;\n\
	lighting = mix(lighting, 1.0, lightT);\n\
	gl_FragColor.rgb *= lighting;\n\
}\n\
";

//clamp(dot(vNormal, vec3(0.0, 0.0, -1.0)), 0.0, 1.0);\n\

const char gFrontVertShaderSrc[] =
"\
attribute vec4 position;\
attribute vec2 texCoord;\
attribute vec3 normal;\
uniform mat4 modelViewProjectionMatrix;\n\
uniform float lightT;\n\
varying vec2 vTexCoord;\n\
varying vec3 vNormal;\n\
void main()\n\
{\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	vTexCoord = texCoord;\n\
	vNormal = normal;\n\
}\
";


const char gBackShaderSrc[] =
"precision mediump float;\n\
varying vec2 vTexCoord;\n\
uniform sampler2D tex0;\n\
\n\
void main()\n\
{\
	float alpha = texture2D(tex0, vTexCoord.st).r;\n\
	gl_FragColor.rgba = vec4(0.0, 0.0, 0.0, 1.0 - alpha);\n\
}\n\
";

const char gBackVertShaderSrc[] =
"\
attribute vec4 position;\
attribute vec2 texCoord;\
uniform mat4 modelViewProjectionMatrix;\n\
varying vec2 vTexCoord;\n\
\n\
void main()\n\
{\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	vTexCoord = texCoord;\n\
}\n\
";

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

///////////////////////////////////////////////////////////////////////////////

struct InterpPt
{
	float x;
	float y;
	bool smooth;

	InterpPt(float x, float y, bool smooth) 
	{
		this->x = x;
		this->y = y;
		this->smooth = smooth;
	}

	bool operator < (const InterpPt & rhs) const
	{
		return x < rhs.x;
	}
};

class Interpolator
{
public:
	Interpolator(float start, float end);
	Interpolator(float startY, bool smoothStart, float endY, bool smoothEnd);
	void addInterpPt(float x, float y, bool smooth = false);
	float evaluate(float x) const;

private:
	std::vector<InterpPt> interpPts_;
	
	int curIndex_;
	bool smooth_;
};

Interpolator::Interpolator(float startY, bool smoothStart, float endY, bool smoothEnd)
{
	interpPts_.push_back(InterpPt(0.0f, startY, smoothStart));
	interpPts_.push_back(InterpPt(1.0f, endY, smoothEnd));
	sort(interpPts_.begin(), interpPts_.end());
	curIndex_ = 0;
	smooth_ = false;
}

Interpolator::Interpolator(float startY, float endY)
{
	interpPts_.push_back(InterpPt(0.0f, startY, false));
	interpPts_.push_back(InterpPt(1.0f, endY, false));
	sort(interpPts_.begin(), interpPts_.end());
	curIndex_ = 0;
	smooth_ = false;
}

void Interpolator::addInterpPt(float x, float y, bool smooth)
{
	interpPts_.push_back(InterpPt(x, y, smooth));
	sort(interpPts_.begin(), interpPts_.end());
	curIndex_ = 0;
}

float toSmoothBoth(float t)
{
	if (t > 1.0f) return 1.0f;
	else if (t > 0.5f)
		return 0.5f - 2.0f * (t - 0.5f) * (t - 1.5f);
	else if (t > 0.0f)
		return 2.0f*t*t;
	else
		return 0.0f;
}

float toSmoothStart(float t)
{
	if (t > 1.0f) return 1.0f;
	if (t < 0.0f) return 0.0f;
	return t*t;
}

float toSmoothEnd(float t)
{
	if (t > 1.0f) return 1.0f;
	if (t < 0.0f) return 0.0f;
	return 1.0f -(t - 1)*(t - 1);
}

float Interpolator::evaluate(float x) const
{	
	if (x < 0.0f) x = 0.0f;
	if (x > 1.0f) x = 1.0f;
	int i = 0;
	for (; i < (int)interpPts_.size(); ++i)
	{
		if (interpPts_[i].x > x) break;
	}	

	if (i >= (int)interpPts_.size()) i = (int)interpPts_.size() - 1;

	int startI = i - 1;
	int endI = i;

	float t = x - interpPts_[startI].x;
	float range = interpPts_[endI].x - interpPts_[startI].x;

	t = t/range;
	
	if (interpPts_[startI].smooth && interpPts_[endI].smooth)
		t = toSmoothBoth(t);
	
	if (interpPts_[startI].smooth)
	{
		if (interpPts_[endI].smooth)
			t = toSmoothBoth(t);
		else
			t = toSmoothStart(t);
	}
	else
	{
		if (interpPts_[endI].smooth)
			t = toSmoothEnd(t);		
	}

	return interpPts_[startI].y + t*(interpPts_[endI].y - interpPts_[startI].y);
}


///////////////////////////////////////////////////////////////////////////////



void gfxLog(const std::string & str);

PageFlip::PageFlip()
{
	camera_.setNearPlane(0.001f);
	camera_.setFarPlane(100.0f);

	//camera_.SetTo2DArea(0.5f, 0.5f, 0.5f, 1.25f, 1.0f, 1.0f, 40.0f, 1.0f);
	//camera_.SetTo2DArea(0.5f, 0.5f, -0.25f, 1.4f, 1.0f, 1.0f, 40.0f, 1.0f);
	camera_.SetTo2DArea(0.5f, 0.5f, -0.25f, 1.25f, 1.0f, 1.0f, 40.0f, 1.0f);
			
	//debug
	/*
	camera_.setLookAt(Vector3(0.5f, 0.5f, 0.0f));
	camera_.setEye(Vector3(0.5f, 2.5f, -1.5f));	
	camera_.setUp(Vector3(0.0f, 0.0f, -1.0f));
*/
	//end debug
	
	
	
	lightCam_.setNearPlane(0.01f);
	lightCam_.setFarPlane(100.0f);

	Vector3 target = Vector3(0.5f, 0.5f, 0.0f);	
	Vector3 dirVec = (Vector3(-1.0f, -0.5f, -1.0f) - target).normalize();
	lightCam_.setLookAt(target);
	lightCam_.setEye(target + 1.2f*dirVec);	
	lightCam_.setUp(Vector3(0.0f, 0.0f, -1.0f));
	lightCam_.setAspectRatio(2.0f);


	//camera_ = lightCam_;
	
	
	cameraMatrix_ = camera_.GetMatrix();
	

	
	indexBuffer_ = vertexBuffer_ = 0;
	blankTexture_ = 0;

	programDepth_ = programWriteDepth_ = programVBlur_ = programHBlur_ = programBack_ = programFront_ = programFrontV_ = 0;

	segments_ = 15;

	gl_ = NULL;
	prevTime_ = 0.0f;
	isPressed_ = false;

	t_ = 1.0f;
	targetR_ = startR_ = curR_ = Vector2(1.0f, 0.5f);
	targetDir_ = startDir_ = curDir_ = Vector2(-1.0f, 0.0f);
	needComputeVertex_ = true;

	transitionDirection_ = TransitionDirectionNone;

	doPageChange_ = false;
	texture_ = NULL;

	texRenderer1_ = new TextureRenderer();
	renderTex1_ = new Texture();

	texRenderer2_ = new TextureRenderer();
	renderTex2_ = new Texture();

	shadowRenderer_ = new TextureRenderer();
	shadowTex_ = new Texture();

	tStep_ = 7.0f;
}

PageFlip::~PageFlip()
{
	delete texRenderer1_;
	delete renderTex1_;

	delete texRenderer2_;
	delete renderTex2_;

	delete shadowRenderer_;
	delete shadowTex_;
}

void PageFlip::animate(TransitionDirection dir)
{
	transitionDirection_ = dir;

	if (transitionDirection_ == TransitionDirectionRight || transitionDirection_ == TransitionDirectionDown)
	{
		startR_ = curR_ = Vector2(1.0f, 0.5f);
		startDir_ = curDir_ = Vector2(-1.0f, 0.0f);
		targetDir_ = pressDir_ = Vector2(-0.5f, 0.5f).normalize();		
		targetR_ = Vector2(0.0f, 0.5f);
	}
	else
	{		
		startR_ = curR_ = Vector2(0.0f, 0.5f);
		startDir_ = curDir_ = Vector2(-1.0f, 0.0f);
		targetDir_ = pressDir_ = Vector2(-1.0f, 0.0f);
		targetR_ = Vector2(1.0f, 0.5f);
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

void PageFlip::reset()
{
	transitionDirection_ = TransitionDirectionNone;
	if (t_ < 1.0f)
	{
		targetR_ = startR_;
		targetDir_ = startDir_;
		t_ = 1.0f;
	}	
}


bool PageFlip::update()
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
				if ((transitionDirection_ == TransitionDirectionLeft || transitionDirection_ == TransitionDirectionUp) && targetR_.x == 1.0f)
					doPageChange_ = true;

				if ((transitionDirection_ == TransitionDirectionRight || transitionDirection_ == TransitionDirectionDown) && targetR_.x <= 0.0f)
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
	glBindAttribLocation(program, AttribNormal, "normal");
	linkProgram(program, &error);	
	glDeleteShader(fragmentShader);
	glDeleteShader(vertShader);	
	
	return program;
}

#include "GfxShaders.h"

void PageFlip::init(GfxRenderer * gl)
{
	uninit();
	gl_ = gl;

	GLint texLoc;



	programFront_ = setupShader(gFrontShaderSrc, gFrontVertShaderSrc, "Front");
	glUseProgram(programFront_);
	locFrontMVPMatrix_ = 
		glGetUniformLocation(programFront_, "modelViewProjectionMatrix");
	locLightT_ = glGetUniformLocation(programFront_, "lightT");
	texLoc = glGetUniformLocation(programFront_, "tex0");
	glUniform1i(texLoc, 0);
	texLoc = glGetUniformLocation(programFront_, "tex1");
	glUniform1i(texLoc, 1);

	programFrontV_ = setupShader(gFrontShaderSrcV, gFrontVertShaderSrc, "Front");
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

	programDepth_ = setupShader(gDepthShaderSrc, gDepthVertShaderSrc, "Depth");
	glUseProgram(programDepth_);
	locDepthLightMVP_ = 
		glGetUniformLocation(programDepth_, "lightMVP");
	locDepthMVP_ = 
		glGetUniformLocation(programDepth_, "modelViewProjectionMatrix");

	programWriteDepth_ = setupShader(gWriteDepthShaderSrc, gWriteDepthVertShaderSrc, "WriteDepth");
	glUseProgram(programWriteDepth_);
	locWriteDepthMVPMatrix_ = 
		glGetUniformLocation(programWriteDepth_, "modelViewProjectionMatrix");
	///////////////////////////
	
	
	int maxVerts = 2*(segments_)*2 + 4;

	glGenBuffers(1, &vertexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*maxVerts, 0, GL_DYNAMIC_DRAW);
	
	int maxTris = 2*(segments_)*2 + 2;
	glGenBuffers(1, &indexBuffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*maxTris*3, 0, GL_DYNAMIC_DRAW);


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

	float shadowMapDim = 256.0f;
	shadowTex_->init(gl, shadowMapDim , shadowMapDim, Texture::UncompressedRgba32, 0);
	shadowRenderer_->init(gl, shadowMapDim , shadowMapDim, true);
	shadowRenderer_->setTexture(shadowTex_);
}

void PageFlip::uninit()
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

	if (programDepth_) 
	{
		glDeleteProgram(programDepth_);
		programDepth_ = 0;
	}

	if (programWriteDepth_) 
	{
		glDeleteProgram(programWriteDepth_);
		programWriteDepth_ = 0;
	}


	renderTex1_->uninit();
	texRenderer1_->uninit();

	renderTex2_->uninit();
	texRenderer2_->uninit();

	shadowTex_->uninit();
	shadowRenderer_->uninit();

	gl_ = 0;
}

void PageFlip::setTexture(Texture * texture)
{
	if (texture)
		texture_ = texture->glTexture();
	else
		texture_ = 0;
}

void PageFlip::setGlTexture(GLuint texture)
{
	texture_ = texture;
}

bool PageFlip::doPageChange() const
{
	return doPageChange_;
}


void PageFlip::draw(GfxRenderer * gl)
{
	GLuint useTexture = texture_;
	if (!useTexture) useTexture = blankTexture_;
	

	Matrix prevCamMatrix = gl->cameraMatrix();

	GLuint programFront = programFront_;
	GLuint programBack = programBack_;

	if (transitionDirection_ == TransitionDirectionUp || transitionDirection_ == TransitionDirectionDown)
	{
		gl->setCameraMatrix(Matrix::Rotate(M_PI/2.0f, 0.0f, 0.0f, -1.0f) * cameraMatrix_);
		programFront = programFrontV_;		
	}
	else
	{
		gl->setCameraMatrix(cameraMatrix_);
	}


	gl->pushMatrix();
	gl->loadMatrix(Matrix::Identity());

	gl->clearZBuffer();	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

/*
	gl->useColorProgram();
	gl->setColorProgramColor(1, 1, 1, 1);		
	gl->drawRect(0.0f, 0.0f, 1.0f, 1.0f);*/

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glEnableVertexAttribArray(AttribPosition);
	glVertexAttribPointer(
		AttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0);
	glEnableVertexAttribArray(AttribTexCoord);
	glVertexAttribPointer(
		AttribTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0 + sizeof(Vector3));
	glEnableVertexAttribArray(AttribNormal);
	glVertexAttribPointer(
		AttribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0 + sizeof(Vector3) + sizeof(Vector2));

		
	shadowRenderer_->beginRenderToTexture(gl);
				
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gl->useCustomProgram(programWriteDepth_, locWriteDepthMVPMatrix_);
	gl->setCameraMatrix(lightCam_.GetMatrix());
	gl->applyCurrentShaderMatrix();
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, 0);
		
	shadowRenderer_->endRenderToTexture(gl);

	texRenderer2_->beginRenderToTexture(gl);	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	gl->useCustomProgram(programDepth_, locDepthMVP_);
	gl->setCameraMatrix(cameraMatrix_);
	glUniformMatrix4fv(locDepthLightMVP_, 1, GL_FALSE, lightCam_.GetMatrix().vals());
	gl->use(shadowTex_->glTexture());	
	gl->drawRect(0.0f, 1.0f, 1.0f, -1.0f);

	texRenderer2_->endRenderToTexture(gl);

	//blur
	
	glDisable(GL_DEPTH_TEST);
	for (int i = 0; i < 2; ++i)
	{
		texRenderer1_->beginRenderToTexture(gl);	
		gl->setCameraMatrix(cameraMatrix_);		
		gl->useCustomProgram(programVBlur_, locVBlurMVPMatrix_);
		glUniform1f(locVBlurSize_, 1.0f/renderTexDim_);	
		gl->use(renderTex2_);
		gl->drawRect(0.0f, 0.0f, 1.0f, 1.0f);
		texRenderer1_->endRenderToTexture(gl);


		texRenderer2_->beginRenderToTexture(gl);	
		gl->setCameraMatrix(cameraMatrix_);	
		gl->useCustomProgram(programHBlur_, locHBlurMVPMatrix_);
		glUniform1f(locHBlurSize_, 1.0f/renderTexDim_);
		gl->use(renderTex1_);
		gl->drawRect(0.0f, 0.0f, 1.0f, 1.0f);
		texRenderer2_->endRenderToTexture(gl);
	}
	
	//end draw shadows to texture


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glEnableVertexAttribArray(AttribPosition);
	glVertexAttribPointer(
		AttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0);
	glEnableVertexAttribArray(AttribTexCoord);
	glVertexAttribPointer(
		AttribTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0 + sizeof(Vector3));
	glEnableVertexAttribArray(AttribNormal);
	glVertexAttribPointer(
		AttribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0 + sizeof(Vector3) + sizeof(Vector2));

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	gl->useCustomProgram(programFront, locFrontMVPMatrix_);	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, useTexture);
	glUniform1f(locLightT_, lightT_);
	gl->applyCurrentShaderMatrix();
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, 0);

	/*
	glDepthFunc(GL_LEQUAL);
	glPolygonOffset(-1, 0);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	gl->useColorProgram();
	gl->setColorProgramColor(1.0, 1.0, 1.0, 1.0);
	gl->applyCurrentShaderMatrix();
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, 0);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glPolygonOffset(0, 0);*/
	
	/*
	gl->useColorProgram();
	gl->setColorProgramColor(1.0f, 0.0f, 0.0f, 1.0f);
	*/
	
	gl->useCustomProgram(programBack, locBackMVPMatrix_);	
	gl->use(renderTex2_->glTexture());		
	gl->drawRect(0.0f, 0.0f, 1.0f, 1.0f);


	
	gl->setCameraMatrix(prevCamMatrix);
	gl->popMatrix();

	glDepthFunc(GL_ALWAYS);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

}

void PageFlip::pressEvent(const Vector2 & pstartPos, TransitionDirection pageFlipDir)
{
	transitionDirection_ = pageFlipDir;
	Vector2 startPos = convCoords(pstartPos);
	isPressed_ = true;
	doPageChange_ = false;
	pressedTime_ = Global::currentTime();
	pressedPos_ = startPos;
	t_ = 0.0f;		
	tStep_ = 6.0f;

	if (transitionDirection_ == TransitionDirectionRight || transitionDirection_ == TransitionDirectionDown)
	{
		startR_ = curR_ = Vector2(1.0f, 0.5f);
		startDir_ = curDir_ = Vector2(-1.0f, 0.0f);
		targetDir_ = pressDir_ = Vector2(-0.5f, 0.5f - startPos.y).normalize();
	}
	else
	{		
		startR_ = curR_ = Vector2(0.0f, 0.5f);
		startDir_ = curDir_ = Vector2(-1.0f, 0.0f);
		targetDir_ = pressDir_ = Vector2(-1.0f, 0.0f);
	}

	moveEvent(startPos);
}

Vector2 PageFlip::convCoords(const Vector2 & pos)
{
	Vector2 ret = pos;
	if (transitionDirection_ == TransitionDirectionUp || transitionDirection_ == TransitionDirectionDown)
	{
		ret.x = pos.y;
		ret.y = 1.0f - pos.x;
	}

	return ret;
}

void PageFlip::releaseEvent(const Vector2 & ppos)
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
	if (transitionDirection_ == TransitionDirectionRight)
	{
		if ((!isQuickFlick && pos.x > 0.5f) || (isQuickFlick && pos.x > pressedPos_.x))
			targetR_ = Vector2(1.0f, 0.5f);
		else
			targetR_ = Vector2(-0.1f, 0.5f);
		
	}
	else
	{
		if ((!isQuickFlick && pos.x < 0.5f) || (isQuickFlick && pos.x < pressedPos_.x))
			targetR_ = Vector2(-0.1f, 0.5f);
		else
			targetR_ = Vector2(1.0f, 0.5f);
	}
}

void PageFlip::moveEvent(const Vector2 & ppos)
{
	Vector2 pos = convCoords(ppos);

	if (isPressed_)
	{
		const float maxY = 4.0f;
		const float t0 = 0.0f;
		const float t1 = 0.1f;

		Vector2 dirVec;
		
		dirVec = pos - Vector2(1.0f, pressedPos_.y);
		dirVec = Vector2(-0.5f + 0.5f*(pos.x-1.0f), pos.y - pressedPos_.y);
		if (dirVec.x > 0.0f) dirVec.x = 0.0f;
	

		float t = (fabs(dirVec.x) - t0)/(t1 - t0);
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;

		if (dirVec.x == 0.0f) dirVec = Vector2(-1.0f, 0.0f);

		float sx = fabs(dirVec.x);
		dirVec.x /= sx;
		dirVec.y /= sx;

		if (dirVec.y < -maxY) dirVec.y = -maxY;
		if (dirVec.y > maxY) dirVec.y = maxY;

		dirVec = pressDir_ + t * (dirVec - pressDir_);


		needComputeVertex_ = true;
			
		/*
		t_ = 0.0f;
		tStep_ = 20.0f;
		startR_ = curR_;
		startDir_ = curDir_;*/
		targetR_ = Vector2(pos.x, 0.5f);
		targetDir_ = dirVec;
	}
}

TransitionDirection PageFlip::curTransitionDirection() const
{
	//return TransitionDirectionLeft;
	return transitionDirection_;
}





float calcRightB(float t, float c1, float r1, float s1, float c2, float r2, float * h)
{
	float pageAngle = t;
	float t1 = s1 / r1;
	Vector2 cylinder1Pos(0.0f, 0.5f);
	cylinder1Pos.x = c1;
	Vector2 cylinder1Dir(-1.0f, 0.0f); //must be unit
	Vector2 rAxis(0.0f, 1.0f);
	float cylinder1Angle = t1;
	float cylinder2Pos = c2;	
	float cylinder2Rad = r2;	
	float cylinder2Angle = pageAngle - cylinder1Angle;

	Vector3 cylinder1Dir3(cylinder1Dir.x, cylinder1Dir.y, 0.0f);
	float axisLen = 1.0f;
	
	float cylinder2Circ = fabs(cylinder2Angle * cylinder2Rad);

	//rotate via page angle about the y axis
	float cp = cos(pageAngle);
	float sp = sin(pageAngle);

	float sc = sin(cylinder1Angle);
	float cc = cos(cylinder1Angle);

	float restAngle = cylinder1Angle + cylinder2Angle;
	Vector3 restDir = sin(restAngle) * Vector3(0.0f, 0.0f, 1.0f) - cos(restAngle) * cylinder1Dir3;

	Vector3 dirVec = sc * Vector3(0.0f, 0.0f, 1.0f) - cc * cylinder1Dir3;

	Vector3 upVec = -cc * Vector3(0.0f, 0.0f, 1.0f) - sc * cylinder1Dir3; 
	if (cylinder2Angle > 0.0f) 
		upVec = cc * Vector3(0.0f, 0.0f, 1.0f) + sc * cylinder1Dir3; 

	Vector2 origPos(1.0f, 0.5f);
	Vector3 position;
	
	Vector2 v1 = cylinder1Pos - origPos;
	float projDist = v1 * cylinder1Dir;
	Vector2 projVec = projDist * cylinder1Dir;		
	float distFromCylinder1 = projDist;

	float curRRadius = r1;
	float cylinder1Circ = cylinder1Angle * curRRadius;

	if (distFromCylinder1 < 0.0f)
	{
		//before first cylinder
		position.x = origPos.x;
		position.y = origPos.y;
		position.z = 0.0f;
	}	
	else
	if (distFromCylinder1 < cylinder1Circ)
	{
		//first cylinder
		Vector3 p(origPos.x + projVec.x, origPos.y + projVec.y, curRRadius);				
		float angle = (distFromCylinder1 / cylinder1Circ) * cylinder1Angle;
		float x = curRRadius * cos(angle);
		float y = curRRadius * sin(angle);

		position = p + x*Vector3(0.0f, 0.0f, -1.0f) - y*cylinder1Dir3;		
	}
	else if (distFromCylinder1 < cylinder1Circ + cylinder2Pos)
	{
		//between first and second cylinder
		
		Vector3 dirVec = sc * Vector3(0.0f, 0.0f, 1.0f) - cc * cylinder1Dir3;
		
		Vector3 startPt(
			origPos.x + projVec.x - curRRadius * sc * cylinder1Dir.x,
			origPos.y + projVec.y - curRRadius * sc * cylinder1Dir.y,
			curRRadius - curRRadius * cc
			);

		position = startPt + (distFromCylinder1 - cylinder1Circ) * dirVec;

	}
	else if (distFromCylinder1 < cylinder1Circ + cylinder2Pos + cylinder2Circ)
	{
		//second cylinder
		Vector3 startPt(
			origPos.x + projVec.x - curRRadius * sc * cylinder1Dir.x,
			origPos.y + projVec.y - curRRadius * sc * cylinder1Dir.y,
			curRRadius - curRRadius * cc
			);

		Vector3 startPt2 = startPt + cylinder2Pos * dirVec;

		Vector3 p = startPt2 + cylinder2Rad * upVec;

		float percent = (distFromCylinder1 - cylinder1Circ - cylinder2Pos) / cylinder2Circ;
		float sc2 = sin(fabs(percent * cylinder2Angle));
		float cc2 = cos(fabs(percent * cylinder2Angle));

		position = p - cylinder2Rad * cc2 * upVec + cylinder2Rad * sc2 * dirVec;
	}
	else
	{
		Vector3 startPt(
			origPos.x + projVec.x - curRRadius * sc * cylinder1Dir.x,
			origPos.y + projVec.y - curRRadius * sc * cylinder1Dir.y,
			curRRadius - curRRadius * cc
			);

		Vector3 startPt2 = startPt + cylinder2Pos * dirVec;

		Vector3 p = startPt2 + cylinder2Rad * upVec;

		float sc2 = sin(fabs(cylinder2Angle));
		float cc2 = cos(fabs(cylinder2Angle));

		Vector3 startPt3 = p - cylinder2Rad * cc2 * upVec + cylinder2Rad * sc2 * dirVec;

		float restDist = distFromCylinder1 - cylinder1Circ - cylinder2Pos - cylinder2Circ;			

		position = startPt3 + restDist * restDir;			
	}

	*h = -position.x * sp + cp * position.z;
	return position.x * cp + sp * position.z;
}


float rCompare(float x, float y)
{
	return x > y;
}


float solveC1GivenX(float R, float initGuess, float t, float r1, float s1, float c2, float r2)
{
	const float EPSILON = 0.0001f;


	static vector<float> initX;

	initX.clear();
	initX.push_back(0.0f);
	initX.push_back(initGuess);
	initX.push_back(1.0f);

	sort(initX.begin(), initX.end());

	float a, b, fa, fb;
	float prevX = 0.0f;
	float prevF = 0.0f;
	float minF = FLT_MAX;
	float minX = 0.0f;

	bool oppRootsFound = false;
	float h;

	for (int i = 0; i < (int)initX.size(); ++i)
	{
		float x = initX[i];		
		float f = calcRightB(t, x, r1, s1, c2, r2, &h) - R;

		if (fabs(f) < fabs(minF))
		{
			minF = f;
			minX = x;
		}

		if (i > 0)
		{
			if (prevF * f < 0.0f) 
			{
				oppRootsFound = true;

				a = prevX;
				fa = prevF;

				b = x;
				fb = f;
				break;
			}
		}
		prevX = x;
		prevF = f;
	}
	
	if (!oppRootsFound)
	{
		//cout <<"c1 solve failed\n";
		return minX;
	}
	

	float c;
	float fc;
	//bisection method for root finding
	int i;
	for (i = 0; i < 30; ++i)
	{
		c = (a + b) / 2;

		fc = calcRightB(t, c, r1, s1, c2, r2, &h) - R;

		if (fabs(fc) < EPSILON) break;

		if (fc * fa > 0.0f)
		{
			a = c;
			fa = fc;
		}
		else
		{
			b = c;
			fb = fc;
		}
	}

	//cout << "solveC1 iterations " << i << "\n";
	return c;
}


float solveR1GivenH(float H, float initGuess, float t, float c1, float s1, float c2, float r2)
{
	//cout << "Solve " << t << " - " << R << "\n";
	const float EPSILON = 0.0001f;


	static vector<float> initX;

	initX.clear();
	initX.push_back(s1 / (6 * M_PI / 4));
	initX.push_back(s1 / M_PI);
	initX.push_back(initGuess);
	initX.push_back(s1 / (3 * M_PI / 4));
	initX.push_back(s1 / (M_PI / 2));
	initX.push_back(s1 / (M_PI / 4));
	initX.push_back(s1 / (0.001f));

	if (t > 3*M_PI/4)
		sort(initX.begin(), initX.end());
	else
		sort(initX.begin(), initX.end(), rCompare);

/*
	vector<pair<float, float>> vals;
	for (int i = 0; i <= 100; ++i)
	{
		float x = initX.front() + ((float)i/100)(initX.back() - initX.front());
		calcRightB(t, c1, x, s1, c2, r2, &h);
		float f = h - H;
		vals.push_back(make_pair(x, f));
	}
*/

	float a, b, fa, fb;
	float prevX = 0.0f;
	float prevF = 0.0f;
	float minF = FLT_MAX;
	float minX = 0.0f;

	float h;

	bool oppRootsFound = false;

	for (int i = 0; i < (int)initX.size(); ++i)
	{
		float x = initX[i];
		
		calcRightB(t, c1, x, s1, c2, r2, &h);
		float f = h - H;
		if (fabs(f) < fabs(minF))
		{
			minF = f;
			minX = x;
		}

		if (i > 0)
		{
			if (prevF * f < 0.0f) 
			{
				oppRootsFound = true;

				a = prevX;
				fa = prevF;

				b = x;
				fb = f;
				break;
			}
		
		}

		prevX = x;
		prevF = f;
	}
	
	if (!oppRootsFound)
	{
		//cout << "r1 solve given H failed\n";
		return minX;
	}
	

	float c;
	float fc;
	//bisection method for root finding
	int i;
	for (i = 0; i < 30; ++i)
	{
		c = (a + b) / 2;

		calcRightB(t, c1, c, s1, c2, r2, &h);
		fc = h - H;

		if (fabs(fc) < EPSILON) break;

		if (fc * fa > 0.0f)
		{
			a = c;
			fa = fc;
		}
		else
		{
			b = c;
			fb = fc;
		}
	}

	//cout << "solveR1 iterations " << i << "\n";;
	return c;
}

void PageFlip::computeVerts(const Vector2 & pRollingPinPos, const Vector2 & pRollingPinDir)
{

	Vector2 rollingPos(pRollingPinPos);
	Vector2 rollingDir(pRollingPinDir);

	float t = 1 - rollingPos.x;
	t /= 2.0f;
	if (t < 0.001f) t = 0.001f;
	
	lightT_ = max(0.0f, 1.0f - (t/0.1f));

	Vector2 cylinder1Pos(0.0f, 0.5f);
	Vector2 cylinder1Dir(rollingDir.x, 0.5f*rollingDir.y);
	cylinder1Dir = cylinder1Dir.normalize();

	/*
	t = 0.19f;
	cylinder1Dir = Vector2(-0.9f, -0.3f).normalize();
	cout << "t " << t << "\n";
	cout << "cylinder " << cylinder1Dir.x << ", " << cylinder1Dir.y << "\n";
	*/
	//

	Interpolator cInterp(0.0f, 0.15f);	
	cInterp.addInterpPt(0.5f, 0.15f, true);
	float c1 = cInterp.evaluate(t);

	Interpolator sInterp(0.6f, 0.25f);
	sInterp.addInterpPt(0.5f, 0.25f, true);	
	float s1 = sInterp.evaluate(t);

	Interpolator xInterp(1.0f, -1.0f);
	xInterp.addInterpPt(0.5f, 0.0f);
	float x = xInterp.evaluate(t);	

	Interpolator hInterp(0.0f, -0.15f);
	hInterp.addInterpPt(0.5f, -0.15f);
	float h = hInterp.evaluate(t);
	
	float pageAngle;
	float pA = 0.4f;
	float pS = -M_PI / (pA * pA);
	if (t > pA) pageAngle = M_PI;
	else pageAngle = pS*t*(t-2*pA);
		
	float r1;
	float t1;
	float c2 = 0.2f;
	float r2 = 0.5f;
	

	float origH;

	static float prevR1 = 0.0f;	
	float rx = x;
	float rc1 = c1;
	float rPageAngle = pageAngle;
	
	r1 = solveR1GivenH(h, 10.0f, rPageAngle, c1, s1, c2, r2);	
	t1 = s1 / r1;


	float correctedC1 = solveC1GivenX(x, c1, pageAngle, r1, s1, c2, r2);
	float angle0 = 0.0f;
	float cT = max(0.0f, min(1.0f, (pageAngle - angle0) / ((float)M_PI - angle0)));
	cT*=cT;
	c1 = c1 + cT*(correctedC1 - c1);

	
	cylinder1Pos.x = c1;
	float cylinder1Angle = t1;
	float cylinder1Rad = r1;
	float cylinder2Pos = c2;	
	float cylinder2Rad = r2;	
	float cylinder2Angle = pageAngle - cylinder1Angle;


	Vector2 rAxis(cylinder1Dir.y, -cylinder1Dir.x);
	t = (0.0f -cylinder1Pos.y) / rAxis.y;
	Vector2 intPtTop = cylinder1Pos + t*rAxis;
	if (intPtTop.x < 0.0f) intPtTop.x = 0.0f;
	if (intPtTop.x > 0.5f) intPtTop.x = 0.5f;	

	t = (1.0f -cylinder1Pos.y) / rAxis.y;
	Vector2 intPtBottom = cylinder1Pos + t*rAxis;
	if (intPtBottom.x < 0.0f) intPtBottom.x = 0.0f;
	if (intPtBottom.x > 0.5f) intPtBottom.x = 0.5f;
	
	rAxis = intPtBottom - intPtTop;
	cylinder1Dir = Vector2(-rAxis.y, rAxis.x).normalize();
	Vector3 cylinder1Dir3(cylinder1Dir.x, cylinder1Dir.y, 0.0f);
	float axisLen = (intPtBottom - intPtTop).magnitude();
	rAxis = (1.0f/axisLen) * rAxis;
	cylinder1Pos = 0.5f*(intPtBottom + intPtTop);

	float cylinder2Circ = fabs(cylinder2Angle * cylinder2Rad);

	//rotate via page angle about the y axis
	float cp = cos(pageAngle);
	float sp = sin(pageAngle);


	float sc = sin(cylinder1Angle);
	float cc = cos(cylinder1Angle);

	float restAngle = cylinder1Angle + cylinder2Angle;
	Vector3 restDir = sin(restAngle) * Vector3(0.0f, 0.0f, 1.0f) - cos(restAngle) * cylinder1Dir3;
	Vector3 restUp = - cos(restAngle) * Vector3(0.0f, 0.0f, 1.0f)  -sin(restAngle) * cylinder1Dir3;

	Vector3 dirVec = sc * Vector3(0.0f, 0.0f, 1.0f) - cc * cylinder1Dir3;
	Vector3 upVec = -cc * Vector3(0.0f, 0.0f, 1.0f) - sc * cylinder1Dir3; 

	if (cylinder2Angle > 0.0f) 
		upVec = cc * Vector3(0.0f, 0.0f, 1.0f) + sc * cylinder1Dir3; 


	makeMesh(cylinder1Pos, rAxis, c1, r1, t1, c2, r2, cylinder2Angle);

	Vector2 origPos;
	Vector3 pos, normal;
	for (int i = 0; i < (int)vertices_.size(); ++i)
	{
		origPos = vertices_[i].texCoord;

		Vector2 v1 = cylinder1Pos - origPos;
		float projDist = v1 * cylinder1Dir;
		Vector2 projVec = projDist * cylinder1Dir;		
		float distFromCylinder1 = projDist;

		float curRRadius = cylinder1Rad;		

		float cylinder1Circ = 		
			cylinder1Angle * curRRadius;
	
		int flag = 0;
		if (distFromCylinder1 < 0.0f)
		{
			//before first cylinder
			pos.x = origPos.x;
			pos.y = origPos.y;
			pos.z = 0.0f;

			normal.x = 0.0f;
			normal.y = 0.0f;
			normal.z = -1.0f;
		}	
		else if (distFromCylinder1 < cylinder1Circ)
		{
			//first cylinder
			Vector3 p(origPos.x + projVec.x, origPos.y + projVec.y, curRRadius);				
			float angle = (distFromCylinder1 / cylinder1Circ) * cylinder1Angle;
			float x = curRRadius * cos(angle);
			float y = curRRadius * sin(angle);

			pos = p + x*Vector3(0.0f, 0.0f, -1.0f) - y*cylinder1Dir3;	

			normal = (pos - p).normalize();
		}
		else if (distFromCylinder1 < cylinder1Circ + cylinder2Pos)
		{
			//between first and second cylinder
						
			Vector3 startPt(
				origPos.x + projVec.x - curRRadius * sc * cylinder1Dir.x,
				origPos.y + projVec.y - curRRadius * sc * cylinder1Dir.y,
				curRRadius - curRRadius * cc
				);

			pos = startPt + (distFromCylinder1 - cylinder1Circ) * dirVec;
			normal = upVec;

			//cout << "Normal " << normal.x << ", " << normal.y << ", " << normal.z << "\n";

		}
		else if (distFromCylinder1 < cylinder1Circ + cylinder2Pos + cylinder2Circ)
		{
			//second cylinder
			Vector3 startPt(
				origPos.x + projVec.x - curRRadius * sc * cylinder1Dir.x,
				origPos.y + projVec.y - curRRadius * sc * cylinder1Dir.y,
				curRRadius - curRRadius * cc
				);

			Vector3 startPt2 = startPt + cylinder2Pos * dirVec;

			Vector3 p = startPt2 + cylinder2Rad * upVec;

			float percent = (distFromCylinder1 - cylinder1Circ - cylinder2Pos) / cylinder2Circ;
			float sc2 = sin(fabs(percent * cylinder2Angle));
			float cc2 = cos(fabs(percent * cylinder2Angle));

			pos = p - cylinder2Rad * cc2 * upVec + cylinder2Rad * sc2 * dirVec;
			normal = (p - pos).normalize();
/*
			normal = Vector3(1.0f, 1.0f, 0.0f);
			flag = 1;*/
		}
		else
		{
			Vector3 startPt(
				origPos.x + projVec.x - curRRadius * sc * cylinder1Dir.x,
				origPos.y + projVec.y - curRRadius * sc * cylinder1Dir.y,
				curRRadius - curRRadius * cc
				);

			Vector3 startPt2 = startPt + cylinder2Pos * dirVec;

			Vector3 p = startPt2 + cylinder2Rad * upVec;

			float sc2 = sin(fabs(cylinder2Angle));
			float cc2 = cos(fabs(cylinder2Angle));

			Vector3 startPt3 = p - cylinder2Rad * cc2 * upVec + cylinder2Rad * sc2 * dirVec;

			float restDist = distFromCylinder1 - cylinder1Circ - cylinder2Pos - cylinder2Circ;			

			pos = startPt3 + restDist * restDir;	

			normal = restUp;
		}

		vertices_[i].position.x = pos.x * cp + sp * pos.z;
		vertices_[i].position.y = pos.y;
		vertices_[i].position.z = -pos.x * sp + cp * pos.z;		

		vertices_[i].normal.x = normal.x * cp + sp * normal.z;
		vertices_[i].normal.y = normal.y;
		vertices_[i].normal.z = -normal.x * sp + cp * normal.z;

		if (vertices_[i].position.z > 0.0f) vertices_[i].position.z = 0.0f;

	}
	

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*vertices_.size(), &vertices_[0]);	
	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);	
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort)*indices_.size(), &indices_[0]);


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

void PageFlip::cutPolyOnSide(const std::vector<Vector2> & origVerts, const Vector2 & intPt1, const Vector2 & intPt2, float side, std::vector<Vector2> * outVerts) const
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

void PageFlip::insertVert(const Vector2 & intPt, std::vector<Vector2> * porigVerts) const
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

void PageFlip::cutPoly(const std::vector<Vector2> & origVerts, const Vector2 & intPt1, const Vector2 & intPt2, std::vector<Vector2> * outVerts1, std::vector<Vector2> * outVerts2) const
{	
	outVerts1->clear();
	outVerts2->clear();
	vector<Vector2> origVertsCopy = origVerts;
	insertVert(intPt1, &origVertsCopy);
	insertVert(intPt2, &origVertsCopy);
		
	cutPolyOnSide(origVertsCopy, intPt1, intPt2, 1, outVerts1);
	cutPolyOnSide(origVertsCopy, intPt1, intPt2, -1, outVerts2);
	
}

inline void doFltErr(Vector2 & intPt)
{
	const float EPSILON = 0.001f;
	if (fabs(intPt.x) < EPSILON) intPt.x = 0.0f;
	if (fabs(intPt.x - 1.0f) < EPSILON) intPt.x = 1.0f;
	if (fabs(intPt.y) < EPSILON) intPt.y = 0.0f;
	if (fabs(intPt.y - 1.0f) < EPSILON) intPt.y = 1.0f;
}

bool PageFlip::computeIntPts(const Vector2 & r, const Vector2 & axis, Vector2 * intPt1Out, Vector2 * intPt2Out) const
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
		doFltErr(intPt);
		if (intPt.x >= 0.0f && intPt.x <= 1.0f)
		{
			ts[intCounter] = t;			
			intPts[intCounter++] = intPt;
		}

		//intersect against bottom
		t = (1.0f - r.y)/axis.y;
		intPt = r + t * axis;
		doFltErr(intPt);
		if (intPt.x >= 0.0f && intPt.x <= 1.0f)
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
		doFltErr(intPt);
		if (intPt.y > 0.0f && intPt.y < 1.0f)
		{
			ts[intCounter] = t;			
			intPts[intCounter++] = intPt;
		}

		//intersect against right
		t = (1.0f - r.x)/axis.x;
		intPt = r + t * axis;		
		doFltErr(intPt);
		if (intPt.y > 0.0f && intPt.y < 1.0f)
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




void PageFlip::makeMesh(const Vector2 & r, const Vector2 & axis, float c1, float r1, float t1, float c2, float r2, float t2)
{
	float fudgeCutOff = 0;
	

	static vector<Vector2> verts, verts1, verts2;
	Vector2 intPt1, intPt2;
	map<Vector2, int, Vector2Compare> vertIndices;
	map<Vector2, int, Vector2Compare>::iterator miter;

	verts.clear();
	verts1.clear();
	verts2.clear();

	verts.push_back(Vector2(0.0f, 0.0f));
	verts.push_back(Vector2(1.0f, 0.0f));
	verts.push_back(Vector2(1.0f, 1.0f));
	verts.push_back(Vector2(0.0f, 1.0f));

	vertices_.clear();
	indices_.clear();

	float minStep = 0.02f;

	int segments1 = floor(fabs(t1 * r1) / minStep);	
	if (segments1 < 2) segments1 = 2;
	if (segments1 > segments_) segments1 = segments_;	
	float step1 = fabs(t1*r1)/(segments1 - 1);

	int segments2 = floor(fabs(t2 * r2) / minStep);	
	if (segments2 < 2) segments2 = 2;
	if (segments2 > segments_) segments2 = segments_;	
	float step2 = fabs(t2*r2)/(segments2 - 1);
	

	int curSeg = 0;

	Vector2 splitDir(axis.y, -axis.x);
	
	Vector2 startIntPt1, startIntPt2;

	static vector< pair<Vector2, Vector2> > intPts;
	intPts.clear();

	computeIntPts(r, axis, &startIntPt1, &startIntPt2);

	for(curSeg = 0; curSeg < segments1; curSeg++)
	{
		Vector2 rNew = startIntPt1 + curSeg*step1*splitDir;
		Vector2 axisNew = startIntPt2 + curSeg*step1*splitDir - rNew;
		if (!computeIntPts(rNew, axisNew, &intPt1, &intPt2)) break;

		intPts.push_back(make_pair(intPt1, intPt2));
	}

	Vector2 cyl2Pos = r + (r1*t1 + c2) * splitDir;

	computeIntPts(cyl2Pos, axis, &startIntPt1, &startIntPt2);
	for(curSeg = 0; curSeg < segments2; curSeg++)
	{
		Vector2 rNew = startIntPt1 + curSeg*step2*splitDir;
		Vector2 axisNew = startIntPt2 + curSeg*step2*splitDir - rNew;
		if (!computeIntPts(rNew, axisNew, &intPt1, &intPt2)) break;

		intPts.push_back(make_pair(intPt1, intPt2));
	}

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
		vertices_[index].texCoord = (*miter).first;
		vertices_[index].position.x = vertices_[index].texCoord.x;
		vertices_[index].position.y = vertices_[index].texCoord.y;
		vertices_[index].position.z = 0.0f;

	}

}



/*
	FILE * file = fopen("c:\\temp\\paper2.obj", "wb");
	for (int i = 0; i < (int)vertices_.size(); ++i)
	{
		fprintf(file, "v %f %f %f\n", vertices_[i].position.x, vertices_[i].position.y, vertices_[i].position.z);
	}
	
	for (int i = 0; i < (int)indices_.size(); i += 3)
	{
		fprintf(file, "f %d %d %d\n", indices_[i] + 1, indices_[i + 1] + 1, indices_[i + 2] + 1);
	}
	fclose(file);

	file = fopen("c:\\temp\\shadow2.obj", "wb");
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
