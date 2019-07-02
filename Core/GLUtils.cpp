#include "stdafx.h"
#include "GLUtils.h"
#include "Global.h"
#include "GfxRenderer.h"
using namespace std;

GLuint loadShader(GLenum type, const char * shaderSrc, std::string * errorMsg)
{	
	if (errorMsg) *errorMsg = "";
	GLuint shader;
	GLint compiled;
	// Create the shader object
	shader = glCreateShader(type);
	if(shader == 0)
	return 0;
	// Load the shader source
	glShaderSource(shader, 1, &shaderSrc, NULL);

	// Compile the shader
	glCompileShader(shader);
	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if(!compiled)
	{
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 1)
		{
			char* infoLog = (char *)malloc(sizeof(char) * infoLen);
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			if (errorMsg) *errorMsg = infoLog;
			free(infoLog);
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

bool linkProgram(GLuint programObject, std::string * errorMsg)
{
	GLint linked;
	// Link the program
	glLinkProgram(programObject);
	// Check the link status
	glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
	if(!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 1)
		{
			char* infoLog = (char *)malloc(sizeof(char) * infoLen);
			glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
			stringstream ss;
			ss << "Error compiling shader:\n" << infoLog << "\n";
			if (errorMsg) *errorMsg = ss.str();
			free(infoLog);
		}
		glDeleteProgram(programObject);
		return false;
	}

	return true;
}


