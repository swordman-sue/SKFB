const char* ccModelBlend_NotColor_frag = STRINGIFY(
\n#ifdef GL_ES\n
precision lowp float;
\n#endif\n

varying vec2 v_texCoord;
uniform vec4 CC_blend_color;

void main()
{
	gl_FragColor = CC_blend_color * texture2D(CC_Texture0, v_texCoord);
}
);