const char* ccModelBlend_grey_frag = STRINGIFY(
\n#ifdef GL_ES\n
precision lowp float;
\n#endif\n
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform vec4 CC_blend_color;
void main()
{
	vec4 texColor = vec4(0.25, 0.25, 0.25, 1) * texture2D(CC_Texture0, v_texCoord);
	texColor.a = texColor.a * CC_blend_color.a;
	gl_FragColor = texColor;
}
);