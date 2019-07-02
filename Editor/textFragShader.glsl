//precision mediump float;
uniform vec4 Color;
varying vec2 vTexCoord;

uniform sampler2D tex0;
uniform float opacity;

void main()
{
	gl_FragColor = Color;
	gl_FragColor.a *= texture2D(tex0, vTexCoord.st).a;
	gl_FragColor.a *= opacity;
}