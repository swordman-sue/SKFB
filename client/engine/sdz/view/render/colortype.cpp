#include "ColorType.h"

USING_NS_CC;

Color4B ARGBToColor4B(unsigned int nCol)
{
	Color4B col4B;
	col4B.a = (nCol & 0xff000000) >> 24;
	col4B.r = (nCol & 0x00ff0000) >> 16;
	col4B.g = (nCol & 0x0000ff00) >> 8;
	col4B.b = nCol & 0x000000ff;
	return col4B;
}

unsigned int Color4BToARGB(const Color4B& col4B)
{
	unsigned int nCol = 0;
	nCol |= (col4B.a << 24);
	nCol |= (col4B.r << 16);
	nCol |= (col4B.g << 8);
	nCol |= col4B.b;
	return nCol;
}

Color3B ARGBToColor3B(unsigned int nCol)
{
	Color3B col3B;
	col3B.r = (nCol & 0x00ff0000) >> 16;
	col3B.g = (nCol & 0x0000ff00) >> 8;
	col3B.b = nCol & 0x000000ff;
	return col3B;
}

unsigned int Color3BToARGB(const Color3B& col3B)
{
	unsigned int nCol = 0;
	nCol |= 0xff000000;
	nCol |= (col3B.r << 16);
	nCol |= (col3B.g << 8);
	nCol |= col3B.b;
	return nCol;
}

Color4B Color4BMultARGB(const Color4B& col4B, unsigned int nCol)
{
	Color4B ret;
	ret.a = GLubyte((float)col4B.a / 255 * ((nCol & 0xff000000) >> 24));
	ret.r = GLubyte((float)col4B.r / 255 * ((nCol & 0x00ff0000) >> 16));
	ret.g = GLubyte((float)col4B.g / 255 * ((nCol & 0x0000ff00) >> 8));
	ret.b = GLubyte((float)col4B.b / 255 * (nCol & 0x000000ff));
	return ret;
}

Color4B Color4BPremultiAlpha(const Color4B& col4B)
{
	Color4B finalCol4B = col4B;
	if(finalCol4B.a < 255)
	{
		finalCol4B.r = (GLubyte)(finalCol4B.r * (col4B.a / 255.0f));
		finalCol4B.g = (GLubyte)(finalCol4B.g * (col4B.a / 255.0f));
		finalCol4B.b = (GLubyte)(finalCol4B.b * (col4B.a / 255.0f));
	}
	return finalCol4B;
}