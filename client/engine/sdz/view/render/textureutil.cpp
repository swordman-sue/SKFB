#include "textureutil.h"
#include "batchrenderer.h"

USING_NS_CC;

Texture2D* createEmptyTexture(int w, int h)
{
	Texture2D* pTex = new Texture2D();

	int total = w * h;
	unsigned int* data = new unsigned int[total];
	memset(data, 0, w * h * sizeof(unsigned int));
	
	pTex->initWithData(data, total, Texture2D::PixelFormat::RGBA8888, w, h, Size((float)w, (float)h));
	delete[] data;
	return pTex;
}

Texture2D* createARGB8Texture(int w, int h, unsigned int col)
{
	Texture2D* pTex = new Texture2D();

	int total = w * h;
	unsigned int* data = new unsigned int[total];
	for(int i = 0; i < total; ++i)
	{
		data[i] = col;
	}

	pTex->initWithData(data, total , Texture2D::PixelFormat::RGBA8888, w, h, Size((float)w, (float)h));
	delete[] data;
	return pTex;
}

void setSubTexture(Texture2D* pTex, int x, int y, int w, int h, void* data, Texture2D::PixelFormat format)
{
	GL::bindTexture2D(pTex->getName());

	const Texture2D::PixelFormatInfoMap& infoMap = Texture2D::getPixelFormatInfoMap();
	Texture2D::PixelFormatInfo formatInfo = infoMap.at(format);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, formatInfo.format, formatInfo.type, data);

	GL::bindTexture2D((GLuint)0);
}

void setSubTexture(Texture2D* pTex, int x, int y, Image* pImage)
{
	setSubTexture(pTex, x, y, pImage->getWidth(), pImage->getHeight(), (void*)(pImage->getData()));
}

void setSubTexture(Texture2D* pTex, int x, int y, const char *text, const char *fontName, float fontSize,
				   int& w, int& h)
{
	auto textDef = FontDefinition();
	auto contentScaleFactor = CC_CONTENT_SCALE_FACTOR();
	textDef._fontName = fontName;
	textDef._fontSize = fontSize * contentScaleFactor;
	textDef._dimensions.width = 0 * contentScaleFactor;
	textDef._dimensions.height = 0 * contentScaleFactor;
	textDef._stroke._strokeSize = 0 * contentScaleFactor;
	textDef._shadow._shadowEnabled = true;

	int imageWidth;
	int imageHeight;
	bool hasPremultipliedAlpha;
	Data outData = Device::getTextureDataForText(text, textDef, Device::TextAlign::TOP_LEFT, imageWidth, imageHeight, hasPremultipliedAlpha);
	if (outData.isNull())
	{
		return;
	}
	setSubTexture(pTex, x, y, imageWidth, imageHeight, outData.getBytes());
}

void updateColor(V3F_C4B_T2F_Quad* pQuad, const Color4B& color4)
{
	pQuad->bl.colors = color4;
	pQuad->br.colors = color4;
	pQuad->tl.colors = color4;
	pQuad->tr.colors = color4;
}

void updateColor(V3F_C4B_T2F_Quad* pQuad, const Color4BRect& colRect)
{
	pQuad->bl.colors = colRect.colBottomLeft;
	pQuad->br.colors = colRect.colBottomRight;
	pQuad->tl.colors = colRect.colTopLeft;
	pQuad->tr.colors = colRect.colTopRight;
}

void updateVertex(V3F_C4B_T2F_Quad* pQuad, float x1, float y1, float x2, float y2)
{
	pQuad->bl.vertices = Vec3( x1, y1, 0 );
	pQuad->br.vertices = Vec3( x2, y1, 0 );
	pQuad->tl.vertices = Vec3( x1, y2, 0 );
	pQuad->tr.vertices = Vec3( x2, y2, 0 );
}

void updateTexCoord(V3F_C4B_T2F_Quad* pQuad, float left, float top, float right, float bottom)
{
	pQuad->bl.texCoords.u = left;
	pQuad->bl.texCoords.v = bottom;
	pQuad->br.texCoords.u = right;
	pQuad->br.texCoords.v = bottom;
	pQuad->tl.texCoords.u = left;
	pQuad->tl.texCoords.v = top;
	pQuad->tr.texCoords.u = right;
	pQuad->tr.texCoords.v = top;
}

void updateQuad(V3F_C4B_T2F_Quad* pQuad, Texture2D* pTex, const Rect& rcTex, const Rect& rcDest, const Color4B& color)
{
	updateVertex(pQuad, rcDest.getMinX(), rcDest.getMinY(), rcDest.getMaxX(), rcDest.getMaxY());
	updateColor(pQuad, color);
	updateTexCoord(pQuad, rcTex.getMinX() / pTex->getPixelsWide(), 
		rcTex.getMinY() / pTex->getPixelsHigh(),
		rcTex.getMaxX() / pTex->getPixelsWide(), 
		rcTex.getMaxY() / pTex->getPixelsHigh());
}

void updateQuad(V3F_C4B_T2F_Quad* pQuad, Texture2D* pTex, const Rect& rcTex, const Rect& rcDest, const Color4BRect& colRect)
{
	updateVertex(pQuad, rcDest.getMinX(), rcDest.getMinY(), rcDest.getMaxX(), rcDest.getMaxY());
	updateColor(pQuad, colRect);
	updateTexCoord(pQuad, rcTex.getMinX() / pTex->getPixelsWide(), 
		rcTex.getMinY() / pTex->getPixelsHigh(),
		rcTex.getMaxX() / pTex->getPixelsWide(), 
		rcTex.getMaxY() / pTex->getPixelsHigh());
}

void drawTexture(Texture2D* pTex, const Rect& rcTex, const Rect& rcDest, const Color4B& color)
{
	if(pTex == NULL)
	{
		return;
	}

	BatchRenderer* pRenderer = BatchRendererManager::sharedRendererManager()->getRenderer();
	if(pRenderer)
	{
		pRenderer->insertTexQuad(pTex, rcTex, rcDest, color);
		return;
	}

	V3F_C4B_T2F_Quad quad;

#define kQuadSize sizeof(quad.bl)

	updateVertex(&quad, rcDest.getMinX(), rcDest.getMinY(), rcDest.getMaxX(), rcDest.getMaxY());
	updateColor(&quad, color);
	updateTexCoord(&quad, rcTex.getMinX() / pTex->getPixelsWide(), 
		rcTex.getMinY() / pTex->getPixelsHigh(),
		rcTex.getMaxX() / pTex->getPixelsWide(), 
		rcTex.getMaxY() / pTex->getPixelsHigh());

	GL::bindTexture2D( pTex->getName() );

	GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX );

	//vertex
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(&quad.tl.vertices));

	// texCoods
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(&quad.tl.texCoords));

	// color
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(&quad.tl.colors));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	CHECK_GL_ERROR_DEBUG();
}

void drawTexture(Texture2D* pTex, const Rect& rcTex, const Rect& rcDest, const Color4B& color, const cocos2d::Mat4& transform, bool need_transform)
{
	if (pTex == NULL)
	{
		return;
	}

	BatchRenderer* pRenderer = BatchRendererManager::sharedRendererManager()->getRenderer();
	if (pRenderer)
	{
		pRenderer->insertTexQuad(pTex, rcTex, rcDest, color);
		return;
	}

	V3F_C4B_T2F_Quad quad;

#define kQuadSize sizeof(quad.bl)

	updateVertex(&quad, rcDest.getMinX(), rcDest.getMinY(), rcDest.getMaxX(), rcDest.getMaxY());
	updateColor(&quad, color);
	updateTexCoord(&quad, rcTex.getMinX() / pTex->getPixelsWide(),
		rcTex.getMinY() / pTex->getPixelsHigh(),
		rcTex.getMaxX() / pTex->getPixelsWide(),
		rcTex.getMaxY() / pTex->getPixelsHigh());

	if (need_transform)
	{
		transform.transformPoint(&quad.bl.vertices);
		transform.transformPoint(&quad.br.vertices);
		transform.transformPoint(&quad.tl.vertices);
		transform.transformPoint(&quad.tr.vertices);
	}

	GL::bindTexture2D(pTex->getName());

	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);

	//vertex
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(&quad.tl.vertices));

	// texCoods
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(&quad.tl.texCoords));

	// color
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(&quad.tl.colors));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	CHECK_GL_ERROR_DEBUG();
}

void drawTexture(Texture2D* pTex, const Rect& rcTex, const Rect& rcDest, const Color4BRect& colRect)
{
	if(pTex == NULL)
	{
		return;
	}
	V3F_C4B_T2F_Quad quad;

	BatchRenderer* pRenderer = BatchRendererManager::sharedRendererManager()->getRenderer();
	if(pRenderer)
	{
		pRenderer->insertTexQuad(pTex, rcTex, rcDest, colRect);
		return;
	}

#define kQuadSize sizeof(quad.bl)
	updateVertex(&quad, rcDest.getMinX(), rcDest.getMinY(), rcDest.getMaxX(), rcDest.getMaxY());
	updateColor(&quad, colRect);
	updateTexCoord(&quad, rcTex.getMinX() / pTex->getPixelsWide(), 
		rcTex.getMinY() / pTex->getPixelsHigh(),
		rcTex.getMaxX() / pTex->getPixelsWide(), 
		rcTex.getMaxY() / pTex->getPixelsHigh());

	GL::bindTexture2D( pTex->getName() );

	GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX );

	//vertex
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(&quad.tl.vertices));

	// texCoods
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(&quad.tl.texCoords));

	// color
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(&quad.tl.colors));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	CHECK_GL_ERROR_DEBUG();
}

static Rect CreateSafeRect(float x, float y, float w, float h)
{
	return Rect(x, y, w > 0.0f ? w : 0.0f, h > 0.0f ? h : 0.0f);
}

void drawScale9Texture(Texture2D* pTex, const Rect& rcTex, const Rect& rcDest, const Rect& rcInner, const Color4B& color, const cocos2d::Mat4& transform, bool need_transform)
{
	float fTexW = rcTex.size.width;
	float fTexH = rcTex.size.height;

	float inner_w = fTexW * rcInner.size.width;
	float inner_h = fTexH * rcInner.size.height;

	/*
	NOTE:
	CC_CONTENT_SCALE_FACTOR不为1时特别复杂，
	不管是逻辑脚本的纹理坐标还是底层引擎，
	建议SceneManager中不要将其设置为1以外的值。
	*/
	float inner_l = fTexW * rcInner.origin.x / CC_CONTENT_SCALE_FACTOR();
	float inner_t = fTexH * rcInner.origin.y / CC_CONTENT_SCALE_FACTOR();
	float inner_r = fTexW * (1 - rcInner.getMaxX()) / CC_CONTENT_SCALE_FACTOR();
	float inner_b = fTexH * (1 - rcInner.getMaxY()) / CC_CONTENT_SCALE_FACTOR();

	//计算9宫格显示区域
	Rect rcLeftTop, rcRightTop, rcLeftBottom, rcRightBottom;
	Rect rcLeft, rcTop, rcRight, rcBottom;
	Rect rcCenter;

	rcLeftTop = CreateSafeRect(rcDest.getMinX(), rcDest.getMaxY() - inner_t, inner_l, inner_t);
	rcRightTop = CreateSafeRect(rcDest.getMaxX() - inner_r, rcDest.getMaxY() - inner_t, inner_r, inner_t);
	rcLeftBottom = CreateSafeRect(rcDest.getMinX(), rcDest.getMinY(), inner_l, inner_b);
	rcRightBottom = CreateSafeRect(rcDest.getMaxX() - inner_r, rcDest.getMinY(), inner_r, inner_b);

	rcLeft = CreateSafeRect(rcDest.getMinX(), rcDest.getMinY() + inner_b, inner_l, rcDest.size.height - inner_t - inner_b);
	rcRight = CreateSafeRect(rcDest.getMaxX() - inner_r, rcDest.getMinY() + inner_b, inner_r, rcDest.size.height - inner_t - inner_b);
	rcTop = CreateSafeRect(rcDest.getMinX() + inner_l, rcDest.getMaxY() - inner_t, rcDest.size.width - inner_l - inner_r, inner_t);
	rcBottom = CreateSafeRect(rcDest.getMinX() + inner_l, rcDest.getMinY(), rcDest.size.width - inner_l - inner_r, inner_b);

	rcCenter = CreateSafeRect(rcDest.getMinX() + inner_l, rcDest.getMinY() + inner_b, rcDest.size.width - inner_l - inner_r, rcDest.size.height - inner_t - inner_b);

	/*
	NOTE:
	CC_CONTENT_SCALE_FACTOR不为1时特别复杂，
	不管是逻辑脚本的纹理坐标还是底层引擎，
	建议SceneManager中不要将其设置为1以外的值。
	*/
	inner_l *= CC_CONTENT_SCALE_FACTOR();
	inner_t *= CC_CONTENT_SCALE_FACTOR();
	inner_r *= CC_CONTENT_SCALE_FACTOR();
	inner_b *= CC_CONTENT_SCALE_FACTOR();

	//计算9宫格纹理坐标
	Rect rcTexLT, rcTexRT, rcTexLB, rcTexRB, rcTexL, rcTexT, rcTexR, rcTexB, rcTexC;
	
	rcTexLT = CreateSafeRect(rcTex.getMinX(), rcTex.getMinY(), inner_l, inner_t);
	rcTexRT = CreateSafeRect(rcTex.getMaxX() - inner_r, rcTex.getMinY(), inner_r, inner_t);
	rcTexLB = CreateSafeRect(rcTex.getMinX(), rcTex.getMaxY() - inner_b, inner_l, inner_b);
	rcTexRB = CreateSafeRect(rcTex.getMaxX() - inner_r, rcTex.getMaxY() - inner_b, inner_r, inner_b);

	rcTexL = CreateSafeRect(rcTex.getMinX(), rcTex.getMinY() + inner_t, inner_l, inner_h);
	rcTexR = CreateSafeRect(rcTex.getMaxX() - inner_r, rcTex.getMinY() + inner_t, inner_r, inner_h);
	rcTexT = CreateSafeRect(rcTex.getMinX() + inner_l, rcTex.getMinY(), inner_w, inner_t);
	rcTexB = CreateSafeRect(rcTex.getMinX() + inner_l, rcTex.getMaxY() - inner_b, inner_w, inner_b);

	rcTexC = CreateSafeRect(rcTex.getMinX() + inner_l, rcTex.getMinY() + inner_t, inner_w, inner_h);

	//渲染9宫格
	drawTexture(pTex, rcTexLT, rcLeftTop, color, transform, need_transform);
	drawTexture(pTex, rcTexRT, rcRightTop, color, transform, need_transform);
	drawTexture(pTex, rcTexLB, rcLeftBottom, color, transform, need_transform);
	drawTexture(pTex, rcTexRB, rcRightBottom, color, transform, need_transform);

	drawTexture(pTex, rcTexL, rcLeft, color, transform, need_transform);
	drawTexture(pTex, rcTexT, rcTop, color, transform, need_transform);
	drawTexture(pTex, rcTexR, rcRight, color, transform, need_transform);
	drawTexture(pTex, rcTexB, rcBottom, color, transform, need_transform);

	drawTexture(pTex, rcTexC, rcCenter, color, transform, need_transform);
}

void drawTexQuads(Texture2D* pTex, V3F_C4B_T2F_Quad* pQuads, int count)
{
	unsigned int stride = sizeof(pQuads->bl);

	GL::bindTexture2D( pTex->getName() );

	GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX );

	//vertex
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, stride, (void*)(&((*pQuads).tl.vertices)));

	// texCoods
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, stride, (void*)(&((*pQuads).tl.texCoords)));

	// color
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (void*)(&((*pQuads).tl.colors)));

	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 * count);
	glDrawElements(GL_TRIANGLES, 6 * count, GL_UNSIGNED_SHORT, BatchRendererManager::sharedRendererManager()->getIndices());

	CHECK_GL_ERROR_DEBUG();
}

void drawTriangle(Texture2D* pTex, const Vec2* ptTex, const Vec2* ptDest, const Color4B& color, unsigned int count)
{
#define TRI_FAN_VERTEX_MAX	100

	static V3F_C4B_T2F vertex[TRI_FAN_VERTEX_MAX];

	if(count > TRI_FAN_VERTEX_MAX)
		count = TRI_FAN_VERTEX_MAX;

	for(unsigned int i = 0; i < count; ++i)
	{
		vertex[i].colors = color;
		vertex[i].vertices = Vec3(ptDest[i].x, ptDest[i].y, 0);
		vertex[i].texCoords = Tex2F(ptTex[i].x / pTex->getPixelsWide(), ptTex[i].y / pTex->getPixelsHigh());
	}

	unsigned int stride = sizeof(vertex[0]);

	GL::bindTexture2D( pTex->getName() );

	GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX );

	//vertex
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, stride, (void*)(&vertex[0].vertices));

	// texCoods
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, stride, (void*)(&vertex[0].texCoords));

	// color
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (void*)(&vertex[0].colors));

	glDrawArrays(GL_TRIANGLE_FAN, 0, count);

	CHECK_GL_ERROR_DEBUG();
}