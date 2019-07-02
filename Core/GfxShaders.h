#pragma once
#ifdef DX

//---------------------------------------STATE_LOADING----------------------------------------

struct loading_vs_input
{
   float4 position_ : POSITION;
   float2 texcoord_ : TEXCOORD0;
};

struct loading_vs_output
{
   float4 position_ : POSITION;
   float2 texcoord_ : TEXCOORD0;
};

float4x4 MVPMatrix;   

loading_vs_output vs_main( loading_vs_input Input )
{
   loading_vs_output Output;

   Output.position_ = mul(Input.position_, MVPMatrix);

   Output.position_.x = Output.position_.x/Output.position_.w;
   Output.position_.y = Output.position_.y/Output.position_.w;

   Output.texcoord_ = Input.texcoord_;
   
   return Output;
}

struct loading_ps_input
{
	float4 position_ : POSITION;
	float2 texcoord_ : TEXCOORD0;
	float4 color_    : COLOR0;
};

loading_ps_output ps_main( loading_ps_input Input )
{
}

//---------------------------------------STATE_LOADING----------------------------------------

#else	//openGL 2.0


class Material;


const char gLoadingVertShaderSrc[] =
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
	vPosition.x = gl_Position.x/gl_Position.w;\
	vPosition.y = gl_Position.y/gl_Position.w;\
}\
";

const char gLoadingVertShaderSrcMask[] =
"\
attribute vec4 position;\n\
attribute vec2 texCoord;\n\
uniform mat4 modelViewProjectionMatrix;\n\
\n\
varying vec2 vTexCoord;\n\
varying vec2 vPosition;\n\
varying vec3 vMaskTexCoord;\n\
\n\
void main()\n\
{\n\
	\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	vTexCoord = texCoord;\n\
	vPosition.x = gl_Position.x/gl_Position.w;\n\
	vPosition.y = gl_Position.y/gl_Position.w;\n\
	vMaskTexCoord = gl_Position.xyw;\n\
}\n\
";

const char gVertShader[] = 
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

const char gVertShaderMask[] = 
"\n\
attribute vec4 position;\n\
attribute vec2 texCoord;\n\
uniform mat4 modelViewProjectionMatrix;\n\
\n\
varying vec2 vTexCoord;\n\
varying vec3 vMaskTexCoord;\n\
\n\
void main()\n\
{\n\
	\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	\n\
	vTexCoord = texCoord;\n\
	vMaskTexCoord = gl_Position.xyw;\n\
}\n\
";

const char gVertBlurShader[] = 
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

const char gVertBlurShaderMask[] = 
	"\n\
	attribute vec4 position;\n\
	attribute vec2 texCoord;\n\
	uniform mat4 modelViewProjectionMatrix;\n\
	\n\
	varying vec2 vTexCoord;\n\
	varying vec3 vMaskTexCoord;\n\
	\n\
	void main()\n\
	{\n\
	\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	\n\
	vec2 Pos;\n\
	Pos = sign(gl_Vertex.xy);\n\
	gl_Position = vec4(Pos, 0.0, 1.0);\n\
	vTexCoord = Pos * 0.5 + 0.5;\n\
	vMaskTexCoord = gl_Position.xyw;\n\
	}\n\
	";

const char gGradationVertShader[] = 
	"\n\
	attribute vec4 position;\n\
	attribute vec2 texCoord;\n\
	uniform mat4 modelViewProjectionMatrix;\n\
	uniform vec4 ColorTopLeft;\n\
	uniform vec4 ColorBottomLeft;\n\
	uniform vec4 ColorTopRight;\n\
	uniform vec4 ColorBottomRight;\n\
	uniform vec4 colorStop;\n\
	uniform vec3 stopPosition;\n\
	\n\
	varying vec2 vTexCoord;\n\
	varying vec4 color;\n\
	\n\
	void main()\n\
	{\n\
	\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	\n\
	if (gl_Position.x == 1.0)\n\
	{\n\
	if (gl_Position.y == 1.0)\n\
	color = ColorTopRight;\n\
	else\n\
	color = ColorBottomRight;\n\
	}\n\
	else\n\
	{\n\
	if (gl_Position.y == 1.0)\n\
	color = ColorTopLeft;\n\
	else\n\
	color = ColorBottomLeft;\n\
	}\n\
	if (gl_Position.x == stopPosition.x && gl_Position.y == stopPosition.y)\n\
	{\n\
		color = colorStop;\n\
	}\n\
	\n\
	vTexCoord = texCoord;\n\
	}\n\
	";

const char gGradationVertShaderMask[] = 
	"\n\
	attribute vec4 position;\n\
	attribute vec2 texCoord;\n\
	uniform mat4 modelViewProjectionMatrix;\n\
	uniform vec4 ColorTopLeft;\n\
	uniform vec4 ColorBottomLeft;\n\
	uniform vec4 ColorTopRight;\n\
	uniform vec4 ColorBottomRight;\n\
	uniform vec4 colorStop;\n\
	uniform vec3 stopPosition;\n\
	\n\
	varying vec4 color;\n\
	varying vec2 vTexCoord;\n\
	varying vec3 vMaskTexCoord;\n\
	\n\
	void main()\n\
	{\n\
	\n\
	gl_Position = modelViewProjectionMatrix*position;\n\
	\n\
	if (gl_Position.x == 1.0)\n\
	{\n\
	if (gl_Position.y == 1.0)\n\
	color = ColorTopRight;\n\
	else\n\
	color = ColorBottomRight;\n\
	}\n\
	else\n\
	{\n\
	if (gl_Position.y == 1.0)\n\
	color = ColorTopLeft;\n\
	else\n\
	color = ColorBottomLeft;\n\
	}\n\
	if (gl_Position.x == stopPosition.x && gl_Position.y == stopPosition.y)\n\
	{\n\
		color = colorStop;\n\
	}\n\
	\n\
	\n\
	vTexCoord = texCoord;\n\
	vMaskTexCoord = gl_Position.xyw;\n\
	}\n\
	";
#endif	//openGL 2.0 end


struct GfxShaderParams
{
	bool isMobile;
	bool withMask;

	GfxShaderParams()
	{
		isMobile = false;
		withMask = false;
	}

	bool operator == (const GfxShaderParams & rhs) const
	{
		return isMobile == rhs.isMobile && withMask == rhs.withMask;
	}

	bool operator != (const GfxShaderParams & rhs) const
	{
		return isMobile != rhs.isMobile || withMask != rhs.withMask;		
	}

	bool operator < (const GfxShaderParams & rhs) const
	{
		if (isMobile != rhs.isMobile) return isMobile < rhs.isMobile;
		if (withMask != rhs.withMask) return withMask < rhs.withMask;
		return false;
	}


};

struct GfxShaderData
{	
	GLuint program;
	GLuint locMvpMatrix;

	GfxShaderData() {program = 0;}
};


typedef boost::shared_ptr<GfxShaderData> GfxShaderDataSPtr;
typedef std::map<GfxShaderParams, GfxShaderDataSPtr> GfxShaderDataMap;
typedef std::pair<GfxShaderParams, GfxShaderDataSPtr> GfxShaderDataMapPair;
class GfxShader
{
public:
	GfxShader() {curShaderData_ = NULL;}

	virtual void setCurParams(const GfxShaderParams & params);

	GLuint glProgram() {return curShaderData_->program;}
	GLint locMvpMatrix() {return curShaderData_->locMvpMatrix;}

	GfxShaderData * data() const {return curShaderData_;}

	virtual ~GfxShader()
	{
		uninit();
	}
	virtual void init(const GfxShaderParams & params) = 0;
	virtual void uninit(const GfxShaderParams & params);
	virtual void uninit();

	virtual void setMaskTexture(Texture * maskTexture) = 0;
	
protected:	
	GLint init(const char * name, const char * vertProgram, const char * fragProgram,	
		int attribCount, const GLuint * attribIndices, const char ** attribNames);


	GfxShaderData * curShaderData_;
	GfxShaderDataMap shaderData_;
};

///////////////////////////////////////////////////////////////////////////////
struct GfxTextureShaderData : public GfxShaderData
{	
	GLint locOpacity;
	GLint locFS, locFT;
};

class GfxTextureProgram : public GfxShader
{
public:
	virtual void init(const GfxShaderParams & params);
	virtual void setMaskTexture(Texture * maskTexture);

	GfxTextureShaderData * data() const {return (GfxTextureShaderData *)curShaderData_;}	
private:
	
};
///////////////////////////////////////////////////////////////////////////////
struct GfxTextureAlphaShaderData : public GfxShaderData
{	
	GLint locOpacity;
	GLint locFS, locFT;
};
class GfxTextureAlphaProgram : public GfxShader
{
public:
	virtual void init(const GfxShaderParams & params);
	virtual void setMaskTexture(Texture * maskTexture);
	
	GfxTextureAlphaShaderData * data() const {return (GfxTextureAlphaShaderData *)curShaderData_;}	
private:
};

///////////////////////////////////////////////////////////////////////////////
struct GfxColorShaderData : public GfxShaderData
{	
	GLint locColor;
};
class GfxColorProgram : public GfxShader
{
public:
	virtual void init(const GfxShaderParams & params);
	virtual void setMaskTexture(Texture * maskTexture);
	GfxColorShaderData * data() const {return (GfxColorShaderData *)curShaderData_;}	
private:
};

///////////////////////////////////////////////////////////////////////////////
struct GfxHorizontalGaussianShaderData : public GfxShaderData
{	
	GLint locColor;
	GLfloat invWidth;
};
class GfxHorizontalGaussianProgram : public GfxShader
{
public:
	virtual void init(const GfxShaderParams & params);
	virtual void setMaskTexture(Texture * maskTexture);
	GfxHorizontalGaussianShaderData * data() const {return (GfxHorizontalGaussianShaderData *)curShaderData_;}	
private:
};

///////////////////////////////////////////////////////////////////////////////
struct GfxVerticalGaussianShaderData : public GfxShaderData
{	
	GLint locColor;
	GLfloat invHeight;
};
class GfxVerticalGaussianProgram : public GfxShader
{
public:
	virtual void init(const GfxShaderParams & params);
	virtual void setMaskTexture(Texture * maskTexture);
	GfxVerticalGaussianShaderData * data() const {return (GfxVerticalGaussianShaderData *)curShaderData_;}	
private:
};

///////////////////////////////////////////////////////////////////////////////
struct GfxGradationShaderData : public GfxShaderData
{	
	GLint locColorTopLeft;
	GLint locColorBottomLeft;
	GLint locColorTopRight;
	GLint locColorBottomRight;
	GLint locColorStop;
	GLint locStopPosition;
	
};
class GfxGradationProgram : public GfxShader
{
public:
	virtual void init(const GfxShaderParams & params);
	virtual void setMaskTexture(Texture * maskTexture);
	GfxGradationShaderData * data() const {return (GfxGradationShaderData *)curShaderData_;}	
private:
};

///////////////////////////////////////////////////////////////////////////////
struct GfxTextShaderData : public GfxShaderData
{	
	GLint locColor;
	GLint locOpacity;
};
class GfxTextProgram : public GfxShader
{
public:
	virtual void init(const GfxShaderParams & params);
	virtual void setMaskTexture(Texture * maskTexture);	
	GfxTextShaderData * data() const {return (GfxTextShaderData *)curShaderData_;}

};

///////////////////////////////////////////////////////////////////////////////

struct GfxConvolutionShaderData : public GfxShaderData
{	
	GLfloat locOpacity;
	GLint locConvType;
	GLfloat locTexSize;
};
class GfxConvolutionProgram : public GfxShader
{
public:
	virtual void init(const GfxShaderParams & params);
	virtual void setMaskTexture(Texture * maskTexture);	
	GfxConvolutionShaderData * data() const {return (GfxConvolutionShaderData *)curShaderData_;}

};

struct GfxHBlurShaderData : public GfxShaderData
{	
	GLfloat locOpacity;
	GLfloat locShadow;
};
class GfxHBlurProgram : public GfxShader
{
public:
	virtual void init(const GfxShaderParams & params);
	virtual void setMaskTexture(Texture * maskTexture);	
	GfxHBlurShaderData * data() const {return (GfxHBlurShaderData *)curShaderData_;}

};


struct GfxVBlurShaderData : public GfxShaderData
{	
	GLfloat locOpacity;
	GLfloat locShadow;
};
class GfxVBlurProgram : public GfxShader
{
public:
	virtual void init(const GfxShaderParams & params);
	virtual void setMaskTexture(Texture * maskTexture);	
	GfxVBlurShaderData * data() const {return (GfxVBlurShaderData *)curShaderData_;}

};

///////////////////////////////////////////////////////////////////////////////

struct GfxLoadingShaderData : public GfxShaderData
{	
	GLint locDim, locAngle;
};
class GfxLoadingProgram : public GfxShader
{
public:
	virtual void init(const GfxShaderParams & params);
	virtual void setMaskTexture(Texture * maskTexture);	
	GfxLoadingShaderData * data() const {return (GfxLoadingShaderData *)curShaderData_;}
private:
	
};

///////////////////////////////////////////////////////////////////////////////
struct GfxMeshShaderData : public GfxShaderData
{	
	GLint 
		locOpacity, locVPMatrix, 
		locMMatrix, locSpecularPower,
		locAmbient, locLightDirs, locCamDir, 
		locLightIntensities, locSpecularIntensities;
};

struct LightParams
{
	bool hasSpecular;

	LightParams()
	{
		this->hasSpecular = false;
	}

	bool operator != (const LightParams & rhs) const
	{
		return hasSpecular != rhs.hasSpecular;
	}

	bool operator < (const LightParams & rhs) const
	{
		return hasSpecular < rhs.hasSpecular;
	}
};

struct MaterialParams
{
	bool hasNormalMap;
	bool hasSpecular;
	bool hasSeparateAlpha;

	MaterialParams()
	{
		this->hasNormalMap = false;
		this->hasSpecular = false;
		this->hasSeparateAlpha = false;
	}

	bool operator != (const MaterialParams & rhs) const
	{
		return hasNormalMap != rhs.hasNormalMap || hasSpecular != rhs.hasSpecular || hasSeparateAlpha != rhs.hasSeparateAlpha;
	}

	bool operator < (const MaterialParams & rhs) const
	{
		if (hasNormalMap != rhs.hasNormalMap) 
			return hasNormalMap < rhs.hasNormalMap;
		
		if (hasSpecular != rhs.hasSpecular)
			return hasSpecular < rhs.hasSpecular;

		return hasSeparateAlpha < rhs.hasSeparateAlpha;
	}
};

struct MeshParams
{
	GfxShaderParams shaderParams;
	std::vector<LightParams> lightParams;
	MaterialParams materialParams;	

	bool operator < (const MeshParams & rhs) const
	{
		if (shaderParams != rhs.shaderParams)
			return shaderParams < rhs.shaderParams;

		if (lightParams.size() != rhs.lightParams.size())
			return lightParams.size() < rhs.lightParams.size();

		for (int i = 0; i < (int)lightParams.size(); ++i)
		{
			if (lightParams[i] != rhs.lightParams[i])
				return lightParams[i] < rhs.lightParams[i];
		}

		return materialParams < rhs.materialParams;
	}
};

typedef boost::shared_ptr<GfxMeshShaderData> GfxMeshShaderDataSPtr;

typedef std::map<MeshParams, GfxMeshShaderDataSPtr> MeshShaderData;
typedef std::pair<MeshParams, GfxMeshShaderDataSPtr> MeshShaderDataPair;

class GfxMeshProgram : public GfxShader
{
public:
	GfxMeshProgram();
	virtual void init(const GfxShaderParams & params);
	void init(const MeshParams & meshParams);
	virtual void uninit(const GfxShaderParams & params);
	void uninit(const MeshParams & meshParams);
	virtual void uninit();
	virtual void setMaskTexture(Texture * maskTexture);	
	
	virtual void setCurParams(const GfxShaderParams & params);
	void setCurParams(const MeshParams & meshParams);

	void setMeshProgramParams(const MaterialParams & params);
	GfxMeshShaderData * data() const {return (GfxMeshShaderData *)curShaderData_;}


	bool initLights(const Vector3 & camDir, float ambient, const std::vector<LightObject *> & lights);

	void setCurMaterial(float opacity, const Material * material);

private:
	
	void assignCurLightParams(GfxMeshShaderData * data);
private:
	GfxShaderParams shaderParams_;
	std::vector<LightParams> lightParams_;
	float lightAmbient_;
	std::vector<GLfloat> lightDirs_, lightIntensities_, specularIntensities_;
	Vector3 camDir_;
	
	MeshShaderData shaderData_;
};


///////////////////////////////////////////////////////////////////////////////
///needs to be copyable
class GfxSingleShader : public GfxShader
{
public:
	GfxSingleShader(GLuint program, GLuint locMvpMatrix);
	virtual void setCurParams(const GfxShaderParams & params) {}

	const GfxShaderData * data() const {return &basicData_;}
	virtual void init(const GfxShaderParams & params) {}	
	virtual void setMaskTexture(Texture * maskTexture) {}

	virtual void uninit(const GfxShaderParams & params) {}
	virtual void uninit() {}
private:

	GfxShaderData basicData_;
};
