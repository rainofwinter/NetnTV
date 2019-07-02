#ifndef GLUtils_h__
#define GLUtils_h__

using namespace std;


GLuint loadShader(GLenum type, const char * shaderSrc, std::string * errorMsg = 0);
bool linkProgram(GLuint programObject, std::string * errorMsg = 0);


#endif // GLUtils_h__