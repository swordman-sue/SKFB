#ifndef _COLOR_TYPE_H_
#define _COLOR_TYPE_H_

#include "cocos2d.h"

cocos2d::Color4B ARGBToColor4B(unsigned int nCol);

unsigned int Color4BToARGB(const cocos2d::Color4B& col4B);

cocos2d::Color3B ARGBToColor3B(unsigned int nCol);

unsigned int Color3BToARGB(const cocos2d::Color3B& col4B);

cocos2d::Color4B Color4BMultARGB(const cocos2d::Color4B& col4B, unsigned int nCol);

cocos2d::Color4B Color4BPremultiAlpha(const cocos2d::Color4B& col4B);

struct Color4BRect
{
	Color4BRect()
	{

	}

	Color4BRect(unsigned int nCol)
	{
		colTopLeft = colTopRight= colBottomLeft= colBottomRight = ARGBToColor4B(nCol);
	}

	Color4BRect(unsigned int nColTL, unsigned int nColTR, unsigned int nColBL, unsigned int nColBR)
	{
		colTopLeft = ARGBToColor4B(nColTL);
		colTopRight = ARGBToColor4B(nColTR);
		colBottomLeft = ARGBToColor4B(nColBL);
		colBottomRight = ARGBToColor4B(nColBR);
	}


	Color4BRect(const cocos2d::Color4B& colTL, const cocos2d::Color4B& colTR, const cocos2d::Color4B& colBL, const cocos2d::Color4B& colBR):
		colTopLeft(colTL),
		colTopRight(colTR),
		colBottomLeft(colBL),
		colBottomRight(colBR)
	{
	}

	void SetOpacity(GLubyte nOpacity)
	{
		colTopLeft.a = nOpacity;
		colTopRight.a = nOpacity;
		colBottomLeft.a = nOpacity;
		colBottomRight.a = nOpacity;
	}

	static Color4BRect FromColor4B(const cocos2d::Color4B& col4B)
	{
		return Color4BRect(Color4BToARGB(col4B));
	}

	cocos2d::Color4B colTopLeft;
	cocos2d::Color4B colTopRight;
	cocos2d::Color4B colBottomLeft;
	cocos2d::Color4B colBottomRight;
} ;

#endif