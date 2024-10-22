const char* ccModelBlend_NotColor_AddMode_frag = STRINGIFY(
\n#ifdef GL_ES\n
precision lowp float;
\n#endif\n

varying vec2 v_texCoord;
uniform vec4 CC_blend_color;
uniform float cc_mansual_alpha;

void main()
{
	vec4 texColor = CC_blend_color * texture2D(CC_Texture0, v_texCoord);
	gl_FragColor.rgb = texColor.a * texColor.rgb;
	gl_FragColor.a = cc_mansual_alpha * texColor.a;
}
);