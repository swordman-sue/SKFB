const char* ccPositionTextureColor_bright = STRINGIFY(
\n#ifdef GL_ES\n
precision lowp float;
\n#endif\n

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform float bright_value;

void main()
{
vec4 texture_color = texture2D(CC_Texture0, v_texCoord);
float alpha = texture_color.a;
gl_FragColor = vec4(texture_color.r * bright_value, texture_color.g * bright_value, texture_color.b * bright_value, alpha);
}
);