const char* ccModelBlend_NotColor_vert = STRINGIFY(
attribute vec4 a_position;
attribute vec2 a_texCoord;
uniform   mat2 uv_rotate;
uniform   vec2 uv_offset;
uniform   vec2 uv_scale;

\n#ifdef GL_ES\n
varying mediump vec2 v_texCoord;
\n#else\n
varying vec2 v_texCoord;
\n#endif\n

void main()
{
	gl_Position = CC_MVPMatrix * a_position;
	v_texCoord = a_texCoord - vec2(0.5, 0.5);
	v_texCoord = uv_rotate * v_texCoord;
	v_texCoord = v_texCoord + vec2(0.5, 0.5);
	v_texCoord = (v_texCoord + uv_offset) * uv_scale;
}
);
