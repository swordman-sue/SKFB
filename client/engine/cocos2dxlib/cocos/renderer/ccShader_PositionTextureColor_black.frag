 const char* ccPositionTextureColor_black = STRINGIFY(
\n#ifdef GL_ES\n
precision lowp float;
\n#endif\n

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()
{
	gl_FragColor = vec4(0.25, 0.25, 0.25, 1) * texture2D(CC_Texture0, v_texCoord);
}
);