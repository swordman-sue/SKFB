const char* ccPositionTextureColor_feather_frag = STRINGIFY(
\n#ifdef GL_ES\n
precision mediump float;
\n#endif\n

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform int u_type;
uniform float u_ratio;
uniform float u_attenuation_threshold;
uniform vec2 u_texSize;
uniform vec2 u_focusCenter;
uniform float u_focusFactor;
uniform float u_focusRadiusSq;

void main()
{
	float dx = u_focusCenter.x - v_texCoord.x * u_texSize.x;
	float dy = u_focusCenter.y - v_texCoord.y * u_texSize.y;
	float dstsq = pow(dx, 2.0) + pow(dy, 2.0);
	float v = dstsq / u_focusRadiusSq * u_focusFactor;

	float r; float g; float b; float a;
	if (u_type == 0)
	{
		//以纹理某点为中心，对其进行正圆辐射状的羽化
		vec4 srcColor = texture2D(CC_Texture0, v_texCoord);   
		r = clamp(srcColor.r + v * v_fragmentColor.r, 0.0, 1.0);
		g = clamp(srcColor.g + v * v_fragmentColor.g, 0.0, 1.0);
		b = clamp(srcColor.b + v * v_fragmentColor.b, 0.0, 1.0);
		a = srcColor.a * v_fragmentColor.a;
	}
	else if (u_type == 1)
	{
		//以纹理某点为中心，生成正圆辐射状的羽化色带(不进行纹理采样，仅利用纹理坐标执行羽化算法)
		r = clamp(v * v_fragmentColor.r, 0.0, 1.0);
		g = clamp(v * v_fragmentColor.g, 0.0, 1.0);
		b = clamp(v * v_fragmentColor.b, 0.0, 1.0);
		a = clamp(v * v_fragmentColor.a, 0.0, v_fragmentColor.a);

		float attenuation_threshold = clamp(u_attenuation_threshold, 0.0, 0.9999);
		if (dstsq >= u_focusRadiusSq)
		{
			a = 0.0;
		}
		else if (dstsq >= u_focusRadiusSq * attenuation_threshold)
		{
			a *= (u_focusRadiusSq - dstsq) / (u_focusRadiusSq * (1.0 - attenuation_threshold));		
		}
	}
	else
	{
		vec4 srcColor = texture2D(CC_Texture0, v_texCoord);   
		r = srcColor.r * v_fragmentColor.r;
		g = srcColor.g * v_fragmentColor.g;
		b = srcColor.b * v_fragmentColor.b;
		a = srcColor.a * v_fragmentColor.a;
	}

	gl_FragColor = vec4(r, g, b, a);
}
);
