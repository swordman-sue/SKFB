const char* ccPositionTextureColor_blur_frag = STRINGIFY(
\n#ifdef GL_ES\n
precision lowp float;
\n#endif\n

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform vec2 u_pixel_offset;

void main()
{
	// use matrix instead of array for compatibility in glsl 100
	mat3 pixel_offsets_x = mat3(
		-u_pixel_offset.x, 0.0, u_pixel_offset.x, 
		-u_pixel_offset.x, 0.0, u_pixel_offset.x,
		-u_pixel_offset.x, 0.0, u_pixel_offset.x
	);
	mat3 pixel_offsets_y = mat3(
		u_pixel_offset.y, u_pixel_offset.y, u_pixel_offset.y,
		0.0, 0.0, 0.0,
		-u_pixel_offset.y, -u_pixel_offset.y, -u_pixel_offset.y
	);

	float kernelFactor = 0.0625;
	// use matrix instead of array for compatibility in glsl 100
	mat3 kernel = mat3(
		1.0, 2.0, 1.0,
		2.0, 4.0, 2.0,
		1.0, 2.0, 1.0  
	);

	float alpha;
	vec4 texColor;
	vec3 sampleColor = vec3(0.0, 0.0, 0.0);
	vec2 pixel_offset;
	for (int c = 0; c < 3; ++c)
	{
		for (int r = 0; r < 3; ++r)
		{
			pixel_offset.x = pixel_offsets_x[c][r];
			pixel_offset.y = pixel_offsets_y[c][r];
			texColor = texture2D(CC_Texture0, v_texCoord + pixel_offset);					
			if (c == 1 && r == 1) alpha = texColor.a;

			sampleColor += texColor.rgb * kernel[c][r];
		}
	}

	gl_FragColor = vec4(sampleColor * kernelFactor, alpha * v_fragmentColor.a);

	/*	
    vec2 pixel_offsets[9] = vec2[](
        vec2(-pixel_offset, pixel_offset),  // top-left
        vec2(0.0,    pixel_offset),			// top-center
        vec2(pixel_offset,  pixel_offset),  // top-right
        vec2(-pixel_offset, 0.0),			// center-left
        vec2(0.0,    0.0),					// center-center
        vec2(pixel_offset,  0.0),			// center-right
        vec2(-pixel_offset, -pixel_offset), // bottom-left
        vec2(0.0,    -pixel_offset),		// bottom-center
        vec2(pixel_offset,  -pixel_offset)  // bottom-right
    );

	float kernelFactor = 0.0625;
    float kernel[9] = float[](
		1.0, 2.0, 1.0,
		2.0, 4.0, 2.0,
		1.0, 2.0, 1.0  
    );

	float alpha;
	vec4 texColor;
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
		texColor = texture2D(CC_Texture0, v_texCoord + pixel_offsets[i]);
        sampleTex[i] = vec3(texColor);
		if (i == 4)
		{
			alpha = texColor.a;
		}
    }
    vec3 col;
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    gl_FragColor = vec4(col * kernelFactor, alpha * v_fragmentColor.a);
	*/
}
);