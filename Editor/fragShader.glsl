//precision mediump float;
varying vec2 vTexCoord;

uniform sampler2D tex0;
uniform float opacity;
void main()
{
	gl_FragColor = texture2D(tex0, vTexCoord.st);
	gl_FragColor.a *= opacity;
}