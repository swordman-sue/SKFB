const char* ccParticleAddBlend_vert = STRINGIFY(

\n#ifdef GL_ES\n
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
\n#else\n
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
\n#endif\n

attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

void main()
{
	gl_Position = a_position;
	v_fragmentColor = a_color;
	v_texCoord = a_texCoord;
}
);