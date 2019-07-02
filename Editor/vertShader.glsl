attribute vec4 position;
attribute vec2 texCoord;
uniform mat4 modelViewProjectionMatrix;

varying vec2 vTexCoord;

void main()
{
	gl_Position = modelViewProjectionMatrix*position;
	vTexCoord = texCoord;
}