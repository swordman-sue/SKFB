const char* ccModelBlend_AddMode_frag = STRINGIFY(
\n#ifdef GL_ES\n
precision lowp float;
\n#endif\n
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform vec4 CC_blend_color;
uniform float cc_mansual_alpha;
void main()
{
	vec4 texColor = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
	texColor = texColor * CC_blend_color;
	gl_FragColor.rgb = texColor.a * texColor.rgb;
	gl_FragColor.a = texColor.a * cc_mansual_alpha;
}
);