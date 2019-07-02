#include "stdafx.h"
#include "GfxRenderer.h"
#include "GLUtils.h"
#include "GfxShaders.h"
#include "Texture.h"
#include "Material.h"
#include "LightObject.h"
#include <boost/lexical_cast.hpp>

void gfxLog(const std::string & str);

GLint GfxShader::init(const char * name, const char * vertProgram, const char * fragProgram,
	int attribCount, const GLuint * attribIndices, const char ** attribNames)
{	
	GLuint vertexShader, fragmentShader;
	GLuint program = 0;
	std::string errorMsg;
	//---------------------------
	program = glCreateProgram();
	vertexShader = loadShader(GL_VERTEX_SHADER, vertProgram, &errorMsg);
	if (!errorMsg.empty()) gfxLog(string("Error loading vertex shader: ") + name + ": " + errorMsg);	
	errorMsg.clear();
	fragmentShader = loadShader(GL_FRAGMENT_SHADER, 
		fragProgram, 
		&errorMsg);
	if (!errorMsg.empty()) gfxLog(string("Error loading fragment shader: ") + name + ": " + errorMsg);	
	glAttachShader(program, vertexShader);   
	glAttachShader(program, fragmentShader);  	
	errorMsg.clear();

	for (int i = 0; i < attribCount; ++i)
	{
		glBindAttribLocation(program, attribIndices[i], attribNames[i]);
	}


	linkProgram(program, &errorMsg);	
	if (!errorMsg.empty()) gfxLog(string("Error linking shader program: ") + name + ": " + errorMsg);	
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glUseProgram(program);  

	return program;
}

void GfxShader::setCurParams(const GfxShaderParams & params)
{
	GfxShaderDataMap::iterator itr = shaderData_.find(params);
	if (itr == shaderData_.end()) init(params);	
	
	itr = shaderData_.find(params);
	assert(itr != shaderData_.end());

	curShaderData_ = (*itr).second.get();	
}

void GfxShader::uninit()
{
	BOOST_FOREACH(GfxShaderDataMapPair pair, shaderData_)
	{
		glDeleteProgram(pair.second->program);
	}

	shaderData_.clear();
	curShaderData_ = NULL;
}

void GfxShader::uninit(const GfxShaderParams & params)
{	
	if (shaderData_.find(params) != shaderData_.end())
	{
		GfxShaderData * data = shaderData_[params].get();
		glDeleteProgram(data->program);

		if (curShaderData_ == data)
		{
			curShaderData_ = NULL;
		}
		shaderData_.erase(params);
	}	
}

GfxSingleShader::GfxSingleShader(GLuint program, GLuint locMvpMatrix)
{
	basicData_.program = program;
	basicData_.locMvpMatrix = locMvpMatrix;
	curShaderData_ = &basicData_;
}

///////////////////////////////////////////////////////////////////////////////

void GfxTextureProgram::init(const GfxShaderParams & params)
{
	if (shaderData_.find(params) != shaderData_.end()) return;
	bool isMobile = params.isMobile, withMask = params.withMask;
	GfxTextureShaderData data;

	std::string ret;
	if (isMobile) ret += "\
precision mediump float;\n\
";

	ret += "\
varying vec2 vTexCoord;\n\
\n\
uniform float fs, ft;\n\
uniform sampler2D tex0;\n\
uniform float opacity;\n";

	if (withMask) ret += "\
varying vec3 vMaskTexCoord;\n\
uniform sampler2D tex1;\n";

	ret += "\
void main()\n\
{\n\
	vec2 texCoords = vec2(vTexCoord.s * fs, vTexCoord.t * ft);\n\
	gl_FragColor = texture2D(tex0, texCoords);\n\
	gl_FragColor.a *= opacity;\n";

	if (withMask) ret += "\
	gl_FragColor.a *= (1.0 - texture2D(tex1, \
		vec2(0.5*(vMaskTexCoord.x/vMaskTexCoord.z + 1.0), \
		0.5*(vMaskTexCoord.y/vMaskTexCoord.z + 1.0))).a);\n";

	ret += "\
}\n";		

	const char * vert = gVertShader;
	if (withMask) vert = gVertShaderMask;


	GLuint attribIndices[] = {AttribPosition, AttribTexCoord};
	const char * attribNames[] = {"position", "texCoord"};

	data.program = GfxShader::init("Texture Shader", vert, ret.c_str(),
		2, attribIndices, attribNames);

	GLint texLoc;
	
	texLoc = glGetUniformLocation(data.program, "tex0");
	glUniform1i(texLoc, 0);

	if (withMask)
	{
		texLoc = glGetUniformLocation(data.program, "tex1");
		glUniform1i(texLoc, 1);
	}
	
	data.locOpacity = glGetUniformLocation(data.program, "opacity");
	data.locMvpMatrix = 
		glGetUniformLocation(data.program, "modelViewProjectionMatrix");
	glUniform1f(data.locOpacity, 1);

	data.locFS = glGetUniformLocation(data.program, "fs");
	data.locFT = glGetUniformLocation(data.program, "ft");	

	GfxShaderDataSPtr dataSPtr(new GfxTextureShaderData(data));
	shaderData_[params] = dataSPtr;
	if (!curShaderData_) curShaderData_ = dataSPtr.get();
}

void GfxTextureProgram::setMaskTexture(Texture * maskTexture)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, maskTexture->glTexture());
}

///////////////////////////////////////////////////////////////////////////////

void GfxTextureAlphaProgram::init(const GfxShaderParams & params)
{
	if (shaderData_.find(params) != shaderData_.end()) return;
	bool isMobile = params.isMobile, withMask = params.withMask;
	GfxTextureAlphaShaderData data;

	std::string ret;

	if (isMobile) ret += "\
precision mediump float;\n\
";
	ret += "\
varying vec2 vTexCoord;\n\
\n\
uniform float fs, ft;\n\
uniform sampler2D tex0;\n\
uniform sampler2D tex1;\n\
uniform float opacity;\n";

	if (withMask) ret += "\
varying vec3 vMaskTexCoord;\n\
uniform sampler2D tex2;\n";

	ret += "\
void main()\n\
{\n\
	vec2 texCoords = vec2(vTexCoord.s * fs, vTexCoord.t * ft);\n\
	gl_FragColor = texture2D(tex0, texCoords);\n\
	gl_FragColor.a = texture2D(tex1, texCoords).r;\n\
	gl_FragColor.a *= opacity;\n";

	if (withMask) ret += "\
	gl_FragColor.a *= (1.0 - texture2D(tex2, \
		vec2(0.5*(vMaskTexCoord.x/vMaskTexCoord.z + 1.0), \
		0.5*(vMaskTexCoord.y/vMaskTexCoord.z + 1.0))).a);\n";

	ret += "\
}\n";

	const char * vert = gVertShader;
	if (withMask) vert = gVertShaderMask;

	GLuint attribIndices[] = {AttribPosition, AttribTexCoord};
	const char * attribNames[] = {"position", "texCoord"};

	data.program = GfxShader::init("Texture With Alpha Shader", vert, ret.c_str(),
		2, attribIndices, attribNames);

	GLint texLoc;

	texLoc = glGetUniformLocation(data.program, "tex0");
	glUniform1i(texLoc, 0);
	
	texLoc = glGetUniformLocation(data.program, "tex1");
	glUniform1i(texLoc, 1);

	if (withMask)
	{
		texLoc = glGetUniformLocation(data.program, "tex2");
		glUniform1i(texLoc, 2);
	}

	data.locOpacity = glGetUniformLocation(data.program, "opacity");
	data.locMvpMatrix = 
		glGetUniformLocation(data.program, "modelViewProjectionMatrix");
	glUniform1f(data.locOpacity, 1);

	data.locFS = glGetUniformLocation(data.program, "fs");
	data.locFT = glGetUniformLocation(data.program, "ft");	
	
	GfxShaderDataSPtr dataSPtr(new GfxTextureAlphaShaderData(data));
	shaderData_[params] = dataSPtr;
	if (!curShaderData_) curShaderData_ = dataSPtr.get();
}

void GfxTextureAlphaProgram::setMaskTexture(Texture * maskTexture)
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, maskTexture->glTexture());
}

///////////////////////////////////////////////////////////////////////////////
void GfxColorProgram::init(const GfxShaderParams & params)
{
	if (shaderData_.find(params) != shaderData_.end()) return;
	bool isMobile = params.isMobile, withMask = params.withMask;
	GfxColorShaderData data;

	std::string ret;

	if (isMobile) ret += "\
precision mediump float;\n";
	if (withMask) ret += "\
varying vec3 vMaskTexCoord;\n\
uniform sampler2D tex0;\n";
	ret += "\
uniform vec4 Color;\n\
void main()\n\
{\n\
	gl_FragColor = Color;\n";

	if (withMask) ret += "\
	gl_FragColor.a *= (1.0 - texture2D(tex0, \
		vec2(0.5*(vMaskTexCoord.x/vMaskTexCoord.z + 1.0), \
		0.5*(vMaskTexCoord.y/vMaskTexCoord.z + 1.0))).a);\n";

	ret += "\
}\n";
	const char * vert = gVertShader;
	if (withMask) vert = gVertShaderMask;
	
	GLuint attribIndices[] = {AttribPosition, AttribTexCoord};
	const char * attribNames[] = {"position", "texCoord"};
	
	data.program = GfxShader::init("Color Shader", vert, ret.c_str(),
		2, attribIndices, attribNames);
	
	if (withMask)
	{
		GLint texLoc = glGetUniformLocation(data.program, "tex0");
		glUniform1i(texLoc, 0);
	}

	data.locColor = glGetUniformLocation(data.program, "Color");
	data.locMvpMatrix = 
		glGetUniformLocation(data.program, "modelViewProjectionMatrix");

	GfxShaderDataSPtr dataSPtr(new GfxColorShaderData(data));
	shaderData_[params] = dataSPtr;
	if (!curShaderData_) curShaderData_ = dataSPtr.get();
}

void GfxColorProgram::setMaskTexture(Texture * maskTexture)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, maskTexture->glTexture());
}
///////////////////////////////////////////////////////////////////////////////
void GfxHorizontalGaussianProgram::init(const GfxShaderParams & params)
{
	if (shaderData_.find(params) != shaderData_.end()) return;
	bool isMobile = params.isMobile, withMask = params.withMask;
	GfxHorizontalGaussianShaderData data;

	std::string ret;

	if (isMobile) ret += "\
precision mediump float;\n\
";

	ret += "\
varying vec2 vTexCoord;\n\
\n\
uniform sampler2D tex0;\n\
uniform float invWidth;\n";

	if (withMask) ret += "\
varying vec3 vMaskTexCoord;\n\
uniform sampler2D tex1;\n";

	ret += "\
void main()\n\
{\n\
	gl_FragColor = \n\
		0.388 * texture2D(tex0, vTexCoord) +\
		0.245 * texture2D(tex0, vTexCoord + vec2(-invWidth, 0)) +\
		0.245 * texture2D(tex0, vTexCoord + vec2(invWidth, 0)) +\
		0.061 * texture2D(tex0, vTexCoord + vec2(-2.0*invWidth, 0)) +\
		0.061 * texture2D(tex0, vTexCoord + vec2(2.0*invWidth, 0));\n";

	if (withMask) ret += "\
	gl_FragColor.a *= (1.0 - texture2D(tex1, \
		vec2(0.5*(vMaskTexCoord.x/vMaskTexCoord.z + 1.0), \
		0.5*(vMaskTexCoord.y/vMaskTexCoord.z + 1.0))).a);\n";

	ret += "\
}\n";		

	const char * vert = gVertShader;
	if (withMask) vert = gVertShaderMask;
	
	GLuint attribIndices[] = {AttribPosition, AttribTexCoord};
	const char * attribNames[] = {"position", "texCoord"};
	
	data.program = GfxShader::init("HorizontalGaussian Shader", vert, ret.c_str(),
		2, attribIndices, attribNames);
	
	GLint texLoc;
	
	texLoc = glGetUniformLocation(data.program, "tex0");
	glUniform1i(texLoc, 0);

	if (withMask)
	{
		texLoc = glGetUniformLocation(data.program, "tex1");
		glUniform1i(texLoc, 1);
	}
	
	data.locMvpMatrix = 
		glGetUniformLocation(data.program, "modelViewProjectionMatrix");

	data.invWidth = glGetUniformLocation(data.program, "invWidth");

	GfxShaderDataSPtr dataSPtr(new GfxHorizontalGaussianShaderData(data));
	shaderData_[params] = dataSPtr;
	if (!curShaderData_) curShaderData_ = dataSPtr.get();
}

void GfxHorizontalGaussianProgram::setMaskTexture(Texture * maskTexture)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, maskTexture->glTexture());
}

///////////////////////////////////////////////////////////////////////////////
void GfxVerticalGaussianProgram::init(const GfxShaderParams & params)
{
	if (shaderData_.find(params) != shaderData_.end()) return;
	bool isMobile = params.isMobile, withMask = params.withMask;
	GfxVerticalGaussianShaderData data;

	std::string ret;

	if (isMobile) ret += "\
precision mediump float;\n\
";

	ret += "\
varying vec2 vTexCoord;\n\
\n\
uniform sampler2D tex0;\n\
uniform float invHeight;\n";

	if (withMask) ret += "\
varying vec3 vMaskTexCoord;\n\
uniform sampler2D tex1;\n";

	ret += "\
void main()\n\
{\n\
	gl_FragColor = \n\
		0.388 * texture2D(tex0, vTexCoord) +\
		0.245 * texture2D(tex0, vTexCoord + vec2(0, -invHeight)) +\
		0.245 * texture2D(tex0, vTexCoord + vec2(0, invHeight)) +\
		0.061 * texture2D(tex0, vTexCoord + vec2(0, -2.0*invHeight)) +\
		0.061 * texture2D(tex0, vTexCoord + vec2(0, 2.0*invHeight));\n";

	if (withMask) ret += "\
	gl_FragColor.a *= (1.0 - texture2D(tex1, \
		vec2(0.5*(vMaskTexCoord.x/vMaskTexCoord.z + 1.0), \
		0.5*(vMaskTexCoord.y/vMaskTexCoord.z + 1.0))).a);\n";

	ret += "\
}\n";		

	const char * vert = gVertShader;
	if (withMask) vert = gVertShaderMask;
	
	GLuint attribIndices[] = {AttribPosition, AttribTexCoord};
	const char * attribNames[] = {"position", "texCoord"};
	
	data.program = GfxShader::init("VerticalGaussian Shader", vert, ret.c_str(),
		2, attribIndices, attribNames);
	
	GLint texLoc;
	
	texLoc = glGetUniformLocation(data.program, "tex0");
	glUniform1i(texLoc, 0);

	if (withMask)
	{
		texLoc = glGetUniformLocation(data.program, "tex1");
		glUniform1i(texLoc, 1);
	}
	
	data.locMvpMatrix = 
		glGetUniformLocation(data.program, "modelViewProjectionMatrix");

	data.invHeight = glGetUniformLocation(data.program, "invHeight");

	GfxShaderDataSPtr dataSPtr(new GfxVerticalGaussianShaderData(data));
	shaderData_[params] = dataSPtr;
	if (!curShaderData_) curShaderData_ = dataSPtr.get();
}

void GfxVerticalGaussianProgram::setMaskTexture(Texture * maskTexture)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, maskTexture->glTexture());
}


///////////////////////////////////////////////////////////////////////////////
void GfxGradationProgram::init(const GfxShaderParams & params)
{
	if (shaderData_.find(params) != shaderData_.end()) return;
	bool isMobile = params.isMobile, withMask = params.withMask;
	GfxGradationShaderData data;

	std::string ret;

	if (isMobile) ret += "\
						 precision mediump float;\n";
	if (withMask) ret += "\
						 varying vec3 vMaskTexCoord;\n\
						 uniform sampler2D tex0;\n";
	ret += "\
		   varying vec4 color;\n\
		   void main()\n\
		   {\n\
		   gl_FragColor = color;\n";

	if (withMask) ret += "\
						 gl_FragColor.a *= (1.0 - texture2D(tex0, \
						 vec2(0.5*(vMaskTexCoord.x/vMaskTexCoord.z + 1.0), \
						 0.5*(vMaskTexCoord.y/vMaskTexCoord.z + 1.0))).a);\n";

	ret += "\
		   }\n";
	const char * vert = gGradationVertShader;
	if (withMask) vert = gGradationVertShaderMask;

	GLuint attribIndices[] = {AttribPosition, AttribTexCoord};
	const char * attribNames[] = {"position", "texCoord"};

	data.program = GfxShader::init("Gradation Shader", vert, ret.c_str(),
		2, attribIndices, attribNames);

	if (withMask)
	{
		GLint texLoc = glGetUniformLocation(data.program, "tex0");
		glUniform1i(texLoc, 0);
	}

	data.locColorTopLeft = glGetUniformLocation(data.program, "ColorTopLeft");
	data.locColorBottomLeft = glGetUniformLocation(data.program, "ColorBottomLeft");
	data.locColorTopRight = glGetUniformLocation(data.program, "ColorTopRight");
	data.locColorBottomRight = glGetUniformLocation(data.program, "ColorBottomRight");
	data.locColorStop = glGetUniformLocation(data.program, "colorStop");
	data.locStopPosition = glGetUniformLocation(data.program, "stopPosition");
	data.locMvpMatrix = 
		glGetUniformLocation(data.program, "modelViewProjectionMatrix");

	GfxShaderDataSPtr dataSPtr(new GfxGradationShaderData(data));
	shaderData_[params] = dataSPtr;
	if (!curShaderData_) curShaderData_ = dataSPtr.get();
}

void GfxGradationProgram::setMaskTexture(Texture * maskTexture)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, maskTexture->glTexture());
}

///////////////////////////////////////////////////////////////////////////////


void GfxTextProgram::init(const GfxShaderParams & params)
{
	if (shaderData_.find(params) != shaderData_.end()) return;
	bool isMobile = params.isMobile, withMask = params.withMask;
	GfxTextShaderData data;

	std::string ret;

	if (isMobile) ret += "\
precision mediump float;\n\
";
	ret += "\
uniform vec4 Color;\n\
varying vec2 vTexCoord;\n\
\n\
uniform sampler2D tex0;\n\
uniform float opacity;\n\
\n";

	if (withMask) ret += "\
varying vec3 vMaskTexCoord;\n\
uniform sampler2D tex1;\n";

	ret += "\
		   vec4 get_pixel(in float dx, in float dy) {\n\
		   return texture2D(tex0, vTexCoord + vec2(dx, dy));\n\
		   }\n\
		   ";

	ret += "\
void main()\n\
{\n\
	gl_FragColor = Color;\n\
	float alpha = get_pixel(0.0, 0.0)[3];\n\
	\
	//if(alpha > 0.1 && alpha < 1.0){\n\
	//	alpha += 0.1;\n\
	//}\n\
	//else if(alpha <= 0.1){\n\
	//	alpha = 0.0;\n\
	//}\n\
	\
	gl_FragColor.a = alpha * opacity;\n\
	\n";

	if (withMask) 
		ret += "\
	gl_FragColor.a *= (1.0 - texture2D(tex1, \
		vec2(0.5*(vMaskTexCoord.x/vMaskTexCoord.z + 1.0), \
		0.5*(vMaskTexCoord.y/vMaskTexCoord.z + 1.0))).a);\n";

	ret += "\
}\n";
	const char * vert = gVertShader;
	if (withMask) vert = gVertShaderMask;

	GLuint attribIndices[] = {AttribPosition, AttribTexCoord};
	const char * attribNames[] = {"position", "texCoord"};

	data.program = GfxShader::init("Text Shader", vert, ret.c_str(),
		2, attribIndices, attribNames);

	GLint texLoc;
	texLoc = glGetUniformLocation(data.program, "tex0");
	glUniform1i(texLoc, 0);
	if (withMask)
	{
		texLoc = glGetUniformLocation(data.program, "tex1");
		glUniform1i(texLoc, 1);
	}

	data.locOpacity = glGetUniformLocation(data.program, "opacity");
	data.locColor = glGetUniformLocation(data.program, "Color");
	data.locMvpMatrix = 
		glGetUniformLocation(data.program, "modelViewProjectionMatrix");

	GfxShaderDataSPtr dataSPtr(new GfxTextShaderData(data));
	shaderData_[params] = dataSPtr;
	if (!curShaderData_) curShaderData_ = dataSPtr.get();
}

void GfxTextProgram::setMaskTexture(Texture * maskTexture)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, maskTexture->glTexture());
}
///////////////////////////////////////////////////////////////////////////////

void GfxConvolutionProgram::init(const GfxShaderParams & params)
{
	if (shaderData_.find(params) != shaderData_.end()) return;
	bool isMobile = params.isMobile, withMask = params.withMask;
	GfxConvolutionShaderData data;

	std::string ret;

	if (isMobile) ret += "\
						 precision mediump float;\n\
						 ";
	ret += "\
		   uniform vec4 Color;\n\
		   varying vec2 vTexCoord;\n\
		   \n\
		   uniform sampler2D tex0;\n\
		   uniform float opacity;\n\
		   uniform float texSize;\n\
		   uniform int convtype;\n\
		   \n";
	

	if (withMask) ret += "\
						 varying vec3 vMaskTexCoord;\n\
						 uniform sampler2D tex1;\n";
	
	ret += "\
		    vec4 get_pixel(in float dx, in float dy) {\n\
				return texture2D(tex0, vTexCoord + vec2(dx, dy));\n\
			}\n\
			\
			float Convolve(in float[9] kernel, in float[9] matrix,in float denom, in float offset) {\n\
				float res = 0.0;\n\
				for (int i=0; i<9; i++) {\n\
					res += kernel[i]*matrix[i];\n\
				}\n\
				return clamp(res/denom + offset,0.0,1.0);\n\
			}\n\
			\
			float Convolvex(in float[9] kernel, in float[9] matrix,in float denom, in float offset) {\n\
			float res = 0.0;\n\
			for (int i=0; i<9; i++) {\n\
			res += kernel[i]*matrix[i];\n\
			}\n\
			return res/denom;\n\
			}\n\
			\
			float[9] GetData(in int channel) {\n\
				float dxtex = 1.0 / texSize;\n\
				float dytex = 1.0 / texSize;\n\
				float[9] mat;\n\
				int k = -1;\n\
				for (int i=-1; i<2; i++) {\n\
					for(int j=-1; j<2; j++) {\n\
						k++;\n\
						mat[k] = get_pixel(float(i)*dxtex,\n\
							float(j)*dytex)[channel];\n\
					}\n\
				}\n\
				return mat;\n\
			}\n\
			\
			float[9] GetMean(in float[9] matr, in float[9] matg, in float[9] matb) {\n\
				float[9] mat;\n\
				for (int i=0; i<9; i++) {\n\
					mat[i] = (matr[i]+matg[i]+matb[i])/3.;\n\
				}\n\
				return mat;\n\
			}\n";

	ret += "\
		   void main()\n\
		   {\n\
		   float[9] kerEmboss = float[] (2.,0.,0.,\n\
		   0., -1., 0.,\n\
		   0., 0., -1.);\n\
		   float[9] kerSharpness = float[] (-1.,-1.,-1.,\n\
		   -1., 9., -1.,\n\
		   -1., -1., -1.);\n\
		   float[9] kerGausBlur = float[]  (1.,2.,1.,\n\
		   2., 4., 2.,\n\
		   1., 2., 1.);\n\
		   float[9] kerEdgeDetect = float[] (-1./8.,-1./8.,-1./8.,\n\
		   -1./8., 1., -1./8.,\n\
		   -1./8., -1./8., -1./8.);\n\
		   float matr[9] = GetData(0);\n\
		   float matg[9] = GetData(1);\n\
		   float matb[9] = GetData(2);\n\
		   \
		   if(convtype == 0){\n\
			   float alpha[9] = GetData(3);\n\
			   gl_FragColor = vec4(Convolve(kerSharpness,matr,1.,0.),\n\
			   Convolve(kerSharpness,matg,1.,0.),\n\
			   Convolve(kerSharpness,matb,1.,0.),Convolve(kerSharpness,alpha,1.,0.)*opacity);\n\
		   }\n\
		   else if(convtype == 1){\n\
			   float alpha[9] = GetData(3);\n\
			   gl_FragColor = vec4(Convolvex(kerGausBlur,matr,16.,0.),\n\
			   Convolvex(kerGausBlur,matg,16.,0.),\n\
			   Convolvex(kerGausBlur,matb,16.,0.),Convolvex(kerGausBlur,alpha,16.,0.)*opacity);\n\
		   }\n\
		   else if(convtype == 2){\n\
		       float mata[9] = GetMean(matr,matg,matb);\n\
			   gl_FragColor = vec4(Convolve(kerEdgeDetect,mata,0.1,0.),\n\
			   Convolve(kerEdgeDetect,mata,0.1,0.),\n\
			   Convolve(kerEdgeDetect,mata,0.1,0.),1.0);\n\
		   }\n\
		   else if(convtype == 3){\n\
		       float mata[9] = GetMean(matr,matg,matb);\n\
			   gl_FragColor = vec4(Convolve(kerEmboss,mata,1.,1./2.),\n\
			   Convolve(kerEmboss,mata,1.,1./2.),\n\
			   Convolve(kerEmboss,mata,1.,1./2.),1.0);\n\
		   }\n\
		   else if(convtype == 4){\n\
			 gl_FragColor.r = 1.0;\n\
			 gl_FragColor.g = 0.0;\n\
			 gl_FragColor.b = 0.0;\n\
		   }\n\
		   \n";

	if (withMask) 
		ret += "\
			   gl_FragColor.a *= (1.0 - texture2D(tex1, \
			   vec2(0.5*(vMaskTexCoord.x/vMaskTexCoord.z + 1.0), \
			   0.5*(vMaskTexCoord.y/vMaskTexCoord.z + 1.0))).a);\n";

	ret += "\
		   }\n";

	const char * vert = gVertShader;

	if (withMask) 
		vert = gVertShaderMask;

	GLuint attribIndices[] = {AttribPosition, AttribTexCoord};
	const char * attribNames[] = {"position", "texCoord"};

	data.program = GfxShader::init("Convolutiuon Shader", vert, ret.c_str(),
		2, attribIndices, attribNames);

	GLint texLoc;
	texLoc = glGetUniformLocation(data.program, "tex0");
	glUniform1i(texLoc, 0);
	if (withMask)
	{
		texLoc = glGetUniformLocation(data.program, "tex1");
		glUniform1i(texLoc, 1);
	}

	data.locOpacity = glGetUniformLocation(data.program, "opacity");
	data.locConvType = glGetUniformLocation(data.program, "convtype");
	data.locTexSize = glGetUniformLocation(data.program, "texSize");
	data.locMvpMatrix = 
		glGetUniformLocation(data.program, "modelViewProjectionMatrix");

	GfxShaderDataSPtr dataSPtr(new GfxConvolutionShaderData(data));
	shaderData_[params] = dataSPtr;
	if (!curShaderData_) curShaderData_ = dataSPtr.get();
}

void GfxConvolutionProgram::setMaskTexture(Texture * maskTexture)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, maskTexture->glTexture());
}
///////////////////////////////////////////////////////////////////////////////

void GfxHBlurProgram::init(const GfxShaderParams & params)
{
	if (shaderData_.find(params) != shaderData_.end()) return;
	bool isMobile = params.isMobile, withMask = params.withMask;
	GfxHBlurShaderData data;

	std::string ret;

	if (isMobile) ret += "\
						 precision mediump float;\n\
						 ";
	ret += "\
		   uniform vec4 Color;\n\
		   varying vec2 vTexCoord;\n\
		   \n\
		   uniform sampler2D tex0;\n\
		   uniform float opacity;\n\
		   uniform float blurSize;\n\
		   \n";


	if (withMask) ret += "\
			varying vec3 vMaskTexCoord;\n\
			uniform sampler2D tex1;\n";

	ret += "\
		   void main()\n\
		   {\n\
			gl_FragColor = \n\
			0.388 * texture2D(tex0, vTexCoord) +\
			0.245 * texture2D(tex0, vTexCoord + vec2(-blurSize, 0)) +\
			0.245 * texture2D(tex0, vTexCoord + vec2(blurSize, 0)) +\
			0.061 * texture2D(tex0, vTexCoord + vec2(-2.0*blurSize, 0)) +\
			0.061 * texture2D(tex0, vTexCoord + vec2(2.0*blurSize, 0));\n\
			gl_FragColor.a *= opacity;\n";


	if (withMask) 
		ret += "\
			   gl_FragColor.a *= (1.0 - texture2D(tex1, \
			   vec2(0.5*(vMaskTexCoord.x/vMaskTexCoord.z + 1.0), \
			   0.5*(vMaskTexCoord.y/vMaskTexCoord.z + 1.0))).a);\n";

	ret += "\
		   }\n";
	
	const char * vert = gVertBlurShader;
	
	if (withMask) 
		vert = gVertBlurShaderMask;

	GLuint attribIndices[] = {AttribPosition, AttribTexCoord};
	const char * attribNames[] = {"position", "texCoord"};

	data.program = GfxShader::init("HBlur Shader", vert, ret.c_str(),
		2, attribIndices, attribNames);

	GLint texLoc;
	texLoc = glGetUniformLocation(data.program, "tex0");
	glUniform1i(texLoc, 0);
	if (withMask)
	{
		texLoc = glGetUniformLocation(data.program, "tex1");
		glUniform1i(texLoc, 1);
	}

	data.locOpacity = glGetUniformLocation(data.program, "opacity");
	data.locShadow = glGetUniformLocation(data.program, "blurSize");
	data.locMvpMatrix = 
		glGetUniformLocation(data.program, "modelViewProjectionMatrix");

	GfxShaderDataSPtr dataSPtr(new GfxHBlurShaderData(data));
	shaderData_[params] = dataSPtr;
	if (!curShaderData_) curShaderData_ = dataSPtr.get();
}

void GfxHBlurProgram::setMaskTexture(Texture * maskTexture)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, maskTexture->glTexture());
}
///////////////////////////////////////////////////////////////////////////////

void GfxVBlurProgram::init(const GfxShaderParams & params)
{
	if (shaderData_.find(params) != shaderData_.end()) return;
	bool isMobile = params.isMobile, withMask = params.withMask;
	GfxVBlurShaderData data;

	std::string ret;

	if (isMobile) ret += "\
						 precision mediump float;\n\
						 ";
	ret += "\
		   uniform vec4 Color;\n\
		   varying vec2 vTexCoord;\n\
		   \n\
		   uniform sampler2D tex0;\n\
		   uniform float opacity;\n\
		   uniform float blurSize;\n\
		   \n";

	if (withMask) ret += "\
						 varying vec3 vMaskTexCoord;\n\
						 uniform sampler2D tex1;\n";

	ret += "\
		   void main()\n\
		   {\n\
			gl_FragColor = \n\
			0.388 * texture2D(tex0, vTexCoord) +\
			0.245 * texture2D(tex0, vTexCoord + vec2(0, -blurSize)) +\
			0.245 * texture2D(tex0, vTexCoord + vec2(0, blurSize)) +\
			0.061 * texture2D(tex0, vTexCoord + vec2(0, -2.0*blurSize)) +\
			0.061 * texture2D(tex0, vTexCoord + vec2(0, 2.0*blurSize));\n\
			gl_FragColor.a *= opacity;\n";

	if (withMask) 
		ret += "\
			   gl_FragColor.a *= (1.0 - texture2D(tex1, \
			   vec2(0.5*(vMaskTexCoord.x/vMaskTexCoord.z + 1.0), \
			   0.5*(vMaskTexCoord.y/vMaskTexCoord.z + 1.0))).a);\n";

	ret += "\
		   }\n";

	const char * vert = gVertBlurShader;

	if (withMask) 
		vert = gVertBlurShaderMask;

	GLuint attribIndices[] = {AttribPosition, AttribTexCoord};
	const char * attribNames[] = {"position", "texCoord"};

	data.program = GfxShader::init("VBlur Shader", vert, ret.c_str(),
		2, attribIndices, attribNames);

	GLint texLoc;
	texLoc = glGetUniformLocation(data.program, "tex0");
	glUniform1i(texLoc, 0);
	if (withMask)
	{
		texLoc = glGetUniformLocation(data.program, "tex1");
		glUniform1i(texLoc, 1);
	}

	data.locOpacity = glGetUniformLocation(data.program, "opacity");
	data.locShadow = glGetUniformLocation(data.program, "blurSize");
	data.locMvpMatrix = 
		glGetUniformLocation(data.program, "modelViewProjectionMatrix");

	GfxShaderDataSPtr dataSPtr(new GfxVBlurShaderData(data));
	shaderData_[params] = dataSPtr;
	if (!curShaderData_) curShaderData_ = dataSPtr.get();
}

void GfxVBlurProgram::setMaskTexture(Texture * maskTexture)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, maskTexture->glTexture());
}

///////////////////////////////////////////////////////////////////////////////

void GfxLoadingProgram::init(const GfxShaderParams & params)
{
	if (shaderData_.find(params) != shaderData_.end()) return;
	bool isMobile = params.isMobile, withMask = params.withMask;
	GfxLoadingShaderData data;

	std::string ret;

	if (isMobile) ret += "\
precision mediump float;\n\
";

	ret += "\
uniform vec4 Color;\n\
varying vec2 vTexCoord;\n\
varying vec2 vPosition;\n\
uniform float dim;\n\
uniform float angleOffset;\n";

	if (withMask) ret += "\
varying vec3 vMaskTexCoord;\n\
uniform sampler2D tex0;\n";

	ret += "\
void main()\n\
{\
float dist = sqrt(vPosition.x*vPosition.x + vPosition.y*vPosition.y); \n\
float pixDim = 2.0 * 1.0 / dim; \n\
float radius = 0.9;	\n\
float innerRadiusW = 0.375; \n\
float innerCirc = innerRadiusW*6.2831853;\n\
float barRadius = 0.018 * innerCirc;\n\
float alpha = clamp(1.0 - (dist - radius + barRadius) / pixDim, 0.0, 1.0);\n\
float hAlpha = 1.0 - clamp(1.0 - (innerRadiusW + barRadius - dist) / pixDim, 0.0, 1.0);\n\
alpha -= hAlpha;\n\
float angle = 2.0 * atan(vPosition.y/(dist + vPosition.x)) + 3.14159265;\n\
float val = 1.0 - mod(angle - angleOffset, 6.2831853)/6.2831853;\n\
//0.523598775 is the barAngleSpacing\n\
float barAngle = 0.523598775 * floor(angle / 0.523598775 + 0.5);\n\
float cRadius = dist * sin(abs(angle - barAngle));\n\
float minCRadius = cRadius;\n\
minCRadius += 999.0*(1.0 - alpha);\n\
hAlpha = clamp(1.0 - (cRadius - barRadius)/pixDim, 0.0, 1.0);\n\
hAlpha *= alpha;\n\
float cosBarAngle = cos(barAngle);\n\
float sinBarAngle = sin(barAngle);\n\
float dx = -cosBarAngle*(innerRadiusW + barRadius);\n\
float dy = -sinBarAngle*(innerRadiusW + barRadius);\n\
dx = vPosition.x - dx;\n\
dy = vPosition.y - dy;\n\
cRadius = sqrt(dx*dx + dy*dy);\n\
minCRadius = min(cRadius, minCRadius);\n\
float bAlpha = clamp(1.0 - (cRadius - barRadius)/pixDim, 0.0, 1.0);\n\
dx = -cosBarAngle*(radius - barRadius);\n\
dy = -sinBarAngle*(radius - barRadius);\n\
dx = vPosition.x - dx;\n\
dy = vPosition.y - dy;\n\
cRadius = sqrt(dx*dx + dy*dy);\n\
minCRadius = min(cRadius, minCRadius);\n\
float tAlpha = clamp(1.0 - (cRadius - barRadius)/pixDim, 0.0, 1.0);\n\
alpha = max(max(hAlpha, bAlpha), tAlpha);\n\
//val = mix(val-0.25, val+0.2, (dist - innerRadiusW)/(radius - innerRadiusW));\n\
val = mix( 1.0 - val, val, alpha);\n\
alpha = clamp(1.0 - (minCRadius - barRadius)/(0.075 + barRadius), 0.0, 1.0);\n\
gl_FragColor = vec4(val, val, val, alpha);\n";

	if (withMask) ret += "\
gl_FragColor.a *= (1.0 - texture2D(tex0, \
	vec2(0.5*(vMaskTexCoord.x/vMaskTexCoord.z + 1.0), \
	0.5*(vMaskTexCoord.y/vMaskTexCoord.z + 1.0))).a);\n";
		
	ret += "\
}\n";
	const char * vert = gLoadingVertShaderSrc;
	if (withMask) vert = gLoadingVertShaderSrcMask;

	GLuint attribIndices[] = {AttribPosition, AttribTexCoord};
	const char * attribNames[] = {"position", "texCoord"};

	data.program = GfxShader::init("Loading Shader", vert, ret.c_str(), 2,
		attribIndices, attribNames);

	if (withMask)
	{
		GLint texLoc = glGetUniformLocation(data.program, "tex0");
		glUniform1i(texLoc, 0);
	}

	data.locMvpMatrix = 
		glGetUniformLocation(data.program, "modelViewProjectionMatrix");
	data.locDim = glGetUniformLocation(data.program, "dim");
	data.locAngle = glGetUniformLocation(data.program, "angleOffset");

	GfxShaderDataSPtr dataSPtr(new GfxLoadingShaderData(data));
	shaderData_[params] = dataSPtr;
	if (!curShaderData_) curShaderData_ = dataSPtr.get();
}

void GfxLoadingProgram::setMaskTexture(Texture * maskTexture)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, maskTexture->glTexture());
}
///////////////////////////////////////////////////////////////////////////////
GfxMeshProgram::GfxMeshProgram()
{
	lightAmbient_ = 0.0f;
	lightParams_.push_back(LightParams());

	lightDirs_.resize(3);
	lightIntensities_.resize(1);
	specularIntensities_.resize(1);
}

void GfxMeshProgram::init(const GfxShaderParams & params)
{
	MeshParams meshParam;
	meshParam.shaderParams = params;
	init(meshParam);
}

void GfxMeshProgram::init(const MeshParams & meshParams)
{
	MeshShaderData::iterator itr1 = shaderData_.find(meshParams);
	if (itr1 != shaderData_.end()) return;
	
	shaderParams_ = meshParams.shaderParams;
	bool isMobile = meshParams.shaderParams.isMobile, withMask = meshParams.shaderParams.withMask;
	GfxMeshShaderData data;
	
	bool existsLightWSpecular = false;
	for (int i = 0; i < (int)meshParams.lightParams.size(); ++i)
	{
		existsLightWSpecular |= meshParams.lightParams[i].hasSpecular;
	}
	std::string ret;
	std::string vertShader;
	vertShader += "\
attribute vec3 position;\n\
attribute vec2 texCoord;\n\
attribute vec3 normal;\n\
attribute vec3 tangent;\n\
attribute float binormalFactor;\n\
uniform mat4 viewProjectionMatrix;\n\
uniform mat4 modelMatrix;\n\
\n\
varying vec2 vTexCoord;\n\
varying vec3 vNormal;\n";

	if (meshParams.materialParams.hasNormalMap)
	{
		vertShader += "\
varying vec3 vTangent;\n\
varying vec3 vBinormal;\n";
	}

	if (withMask) vertShader += "\
varying vec3 vMaskTexCoord;\n";

	vertShader += "\
\n\
void main()\n\
{\n\
	gl_Position = viewProjectionMatrix*modelMatrix*vec4(position.xyz, 1);\n\
	vTexCoord.s = texCoord.s;\n\
	vTexCoord.t = 1.0 - texCoord.t;\n\
	vNormal = normalize(mat3(modelMatrix) * normal);\n";

	if (meshParams.materialParams.hasNormalMap)
	{
		vertShader += "\
	vTangent = normalize(mat3(modelMatrix) * tangent);\n\
	vBinormal = normalize(binormalFactor * cross(vNormal, vTangent));\n";
	}

	if (withMask) vertShader += "\
	vMaskTexCoord = gl_Position.xyw;\n";

	vertShader += "\
}\n";
	
	if (isMobile) ret += "\
precision mediump float;\n\
";

	int numDirLights = (int)lightParams_.size();
	std::string strNumLights = boost::lexical_cast<string>(numDirLights);
	ret += "\
#define NUM_DIR_LIGHTS " + strNumLights + "\n\
#define MIPMAP_BIAS -0.75\n\
uniform float lightIntensities[NUM_DIR_LIGHTS];\n\
uniform vec3 lightDirs[NUM_DIR_LIGHTS];\n\
uniform float specularIntensities[NUM_DIR_LIGHTS];\n\
uniform float ambient;\n\
uniform float opacity;\n\
uniform vec3 camDir;\n\
varying vec2 vTexCoord;\n\
varying vec3 vNormal;\n";

	if (meshParams.materialParams.hasNormalMap) ret += "\
varying vec3 vTangent;\n\
varying vec3 vBinormal;\n";

	ret += "\
uniform sampler2D diffuseSampler;	//0\n";

	if (meshParams.materialParams.hasSeparateAlpha) ret += "\
uniform sampler2D alphaSampler;	//1\n";

	if (meshParams.materialParams.hasNormalMap) ret += "\
uniform sampler2D normalSampler;	//2\n";

	if (meshParams.materialParams.hasSpecular && existsLightWSpecular) ret += "\
uniform sampler2D specularSampler;	//3\n\
uniform float specularPower;\n";

	if (withMask) ret += "\
varying vec3 vMaskTexCoord;\n\
uniform sampler2D tex3;		//4\n";
	
	ret += "\
vec4 doLighting()\n\
{\n\
	vec4 shaderDiffuseColor;\n";
	
	if (meshParams.materialParams.hasSeparateAlpha) ret += "\
	shaderDiffuseColor.rgb = texture2D(diffuseSampler, vTexCoord.st, MIPMAP_BIAS).rgb;\n\
	shaderDiffuseColor.a = texture2D(alphaSampler, vTexCoord.st, MIPMAP_BIAS).r;\n";
	else ret += "\
	shaderDiffuseColor = texture2D(diffuseSampler, vTexCoord.st, MIPMAP_BIAS);\n";

	if (meshParams.materialParams.hasNormalMap)
	{
		ret += "\
	vec3 normalFromMap = texture2D(normalSampler, vTexCoord.st, MIPMAP_BIAS).rgb;\n\
	//For some strange reason the line below doesn't work on Motorola XOOM (the 2.0 * part), so use the two lines below that one instead\n\
	//normalFromMap = 2.0*normalFromMap - 1.0;\n\
	\n\
	normalFromMap -= 0.5;\n\
	normalFromMap += normalFromMap;\n\
	\n\
	vec3 normal = normalFromMap.x * vTangent + normalFromMap.y * vBinormal + normalFromMap.z * vNormal;\n\
	normal = normalize(normal);\n";	
	}
	else
	{
		ret += "\
	vec3 normal = normalize(vNormal);\n";
	}

	if (meshParams.materialParams.hasSpecular && existsLightWSpecular)
	{
		ret += "\
	vec3 specularFromMap = texture2D(specularSampler, vTexCoord.st, MIPMAP_BIAS).rgb;\n\
	vec3 r;\n\
	float rDotV;\n";
	}

	ret += "\
	float lighting = ambient;\n\
	vec4 color = vec4(0.0, 0.0, 0.0, shaderDiffuseColor.a * opacity);\n";

	
	for (int i = 0; i < numDirLights; ++i)
	{	
		std::string strI = boost::lexical_cast<string>(i);
		ret += "\
	lighting += lightIntensities["+strI+"] * clamp(dot(normal, -lightDirs["+strI+"]), 0.0, 1.0);\n";

		if (meshParams.materialParams.hasSpecular && lightParams_[i].hasSpecular)
		{
			ret += "\
	r = reflect(lightDirs["+strI+"], normal);\n\
	rDotV = clamp(dot(r, -camDir), 0.0, 1.0);\n\
	color.rgb += specularIntensities["+strI+"] * specularFromMap * pow(rDotV, specularPower);\n";
		}
	}

	

	ret += "\
	color.rgb += shaderDiffuseColor.rgb*lighting;\n\
	return color;\n\
}\n\
void main()\n\
{\n\
	gl_FragColor = doLighting();\n";

	if (withMask) ret += "\
	gl_FragColor.a *= (1.0 - texture2D(tex3, \
		vec2(0.5*(vMaskTexCoord.x/vMaskTexCoord.z + 1.0), \
		0.5*(vMaskTexCoord.y/vMaskTexCoord.z + 1.0))).a);\n";

	ret += "\
}\n";


	GLuint attribIndices[] = {AttribPosition, AttribTexCoord, AttribNormal, AttribTangent, AttribBinormalFactor};
	const char * attribNames[] = {"position", "texCoord", "normal", "tangent", "binormalFactor"};
	
	data.program = GfxShader::init("Mesh Shader", vertShader.c_str(), ret.c_str(),
		5, attribIndices, attribNames);	


	GLint texLoc;
	texLoc = glGetUniformLocation(data.program, "diffuseSampler");
	glUniform1i(texLoc, 0);
	texLoc = glGetUniformLocation(data.program, "alphaSampler");
	glUniform1i(texLoc, 1);
	texLoc = glGetUniformLocation(data.program, "normalSampler");
	glUniform1i(texLoc, 2);
	texLoc = glGetUniformLocation(data.program, "specularSampler");
	glUniform1i(texLoc, 3);
	if (withMask)
	{
		texLoc = glGetUniformLocation(data.program, "tex3");
		glUniform1i(texLoc, 4);
	}
	data.locOpacity = glGetUniformLocation(data.program, "opacity");
	data.locSpecularPower = glGetUniformLocation(data.program, "specularPower");
	data.locAmbient = glGetUniformLocation(data.program, "ambient");
	glUniform1f(data.locAmbient, 0.1f);
	data.locLightDirs = glGetUniformLocation(data.program, "lightDirs");
	data.locLightIntensities = glGetUniformLocation(data.program, "lightIntensities");
	data.locSpecularIntensities = glGetUniformLocation(data.program, "specularIntensities");
	data.locCamDir = glGetUniformLocation(data.program, "camDir");
	data.locVPMatrix = 
		glGetUniformLocation(data.program, "viewProjectionMatrix");
	data.locMMatrix = 
		glGetUniformLocation(data.program, "modelMatrix");
	glUniform1f(data.locOpacity, 1);
	

	GfxMeshShaderDataSPtr dataSPtr(new GfxMeshShaderData(data));
	shaderData_[meshParams] = dataSPtr;
	if (!curShaderData_) 
	{
		curShaderData_ = dataSPtr.get();
	}
}

void GfxMeshProgram::uninit()
{
	BOOST_FOREACH(const MeshShaderDataPair & p, shaderData_)
	{
		glDeleteProgram(p.second->program);				
	}
	curShaderData_ = NULL;
	shaderData_.clear();
}

void GfxMeshProgram::uninit(const GfxShaderParams & params)
{	
	MeshShaderData::iterator itr;
	for (itr = shaderData_.begin(); itr != shaderData_.end();)
	{
		const MeshParams & meshParams = (*itr).first;
		if (meshParams.shaderParams == params)
		{
			GfxMeshShaderData * data = (*itr).second.get();
			glDeleteProgram(data->program);
			if (curShaderData_ == (GfxShaderData *)data)
			{
				curShaderData_ = NULL;
			}
			shaderData_.erase(itr++);
		}
		else
			++itr;
	}	
}

void GfxMeshProgram::uninit(const MeshParams & meshParams)
{		
	if (shaderData_.find(meshParams) != shaderData_.end())
	{
		GfxMeshShaderData * data = shaderData_[meshParams].get();
		glDeleteProgram(data->program);

		if (curShaderData_ == data)
		{
			curShaderData_ = NULL;
		}
		shaderData_.erase(meshParams);
	}	
}

void GfxMeshProgram::setCurParams(const GfxShaderParams & params)
{
	MeshParams meshParams;
	meshParams.shaderParams = params;
	setCurParams(meshParams);
}

void GfxMeshProgram::setCurParams(const MeshParams & meshParams)
{	
	MeshShaderData::iterator itr = shaderData_.find(meshParams);
	if (itr == shaderData_.end()) 
	{
		init(meshParams);	
	}

	itr = shaderData_.find(meshParams);
	assert(itr != shaderData_.end());

	curShaderData_ = (*itr).second.get();
}

bool GfxMeshProgram::initLights(const Vector3 & camDir, float lightAmbient, const std::vector<LightObject *> & lights)
{
	bool lightsChanged = false;

	if (lightParams_.size() != lights.size())
	{
		lightParams_.resize(lights.size());
		lightsChanged = true;
	}
	

	for (int i = 0; i < (int)lights.size(); ++i)
	{
		if (lightParams_[i].hasSpecular != lights[i]->specularIntensity() > 0.0f)
		{
			lightParams_[i].hasSpecular = !lightParams_[i].hasSpecular;
			lightsChanged = true;
		}
	}

	lightAmbient_ = lightAmbient;
	camDir_ = camDir;
	int numLights = (int)lightParams_.size();

	lightDirs_.resize(3*numLights);
	lightIntensities_.resize(numLights);
	specularIntensities_.resize(numLights);


	for (int i = 0; i < numLights; ++i)
	{
		lightDirs_[3*i] = lights[i]->lightDirection().x;
		lightDirs_[3*i + 1] = lights[i]->lightDirection().y;
		lightDirs_[3*i + 2] = lights[i]->lightDirection().z;
		lightIntensities_[i] = lights[i]->intensity();
		specularIntensities_[i] = lights[i]->specularIntensity();
	}

	return lightsChanged;	
}

void GfxMeshProgram::assignCurLightParams(GfxMeshShaderData * data)
{
	glUseProgram(data->program);
	glUniform3f(data->locCamDir, camDir_.x, camDir_.y, camDir_.z);
	glUniform1f(data->locAmbient, lightAmbient_);	
	glUniform3fv(data->locLightDirs, lightParams_.size(), &lightDirs_[0]);	
	glUniform1fv(data->locLightIntensities, lightParams_.size(), &lightIntensities_[0]);
	glUniform1fv(data->locSpecularIntensities, lightParams_.size(), &specularIntensities_[0]);
}

void GfxMeshProgram::setMaskTexture(Texture * maskTexture)
{
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, maskTexture->glTexture());
}

void GfxMeshProgram::setCurMaterial(float opacity, const Material * material)
{
	MaterialParams matParams;
	matParams.hasNormalMap = material->normalMap() != NULL;
	matParams.hasSpecular = material->specularMap() != NULL && material->specularPower() > 1.0f;
	if (material->diffuseMap())			
		matParams.hasSeparateAlpha = material->diffuseMap()->needSeparateAlpha();
	
	MeshParams meshParams;
	meshParams.materialParams = matParams;

	meshParams.lightParams = lightParams_;
	meshParams.shaderParams = shaderParams_;
	setCurParams(meshParams);
	assignCurLightParams(data());
	
	GfxMeshShaderData * d = data();
	glUseProgram(d->program);
	glUniform1f(d->locOpacity, opacity);	
	glUniform1f(d->locSpecularPower, material->specularPower());
}