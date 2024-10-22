const char* ccModelBlend_frag = STRINGIFY(
\n#ifdef GL_ES\n
precision lowp float;
\n#endif\n
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform vec4 CC_blend_color;
void main()
{
	vec4 texColor = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
	gl_FragColor = texColor * CC_blend_color;
}
);