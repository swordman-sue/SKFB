#include "graphiclabel.h"
using namespace cocos2d;

#define CIRCLE_SEGEMENT 64

void GraphicLabel::GraphicLabelRect::Draw(GLubyte capacity)
{	
	DrawPrimitives::drawSolidRect(cocos2d::Vec2(x, y), cocos2d::Vec2(x + width, y + height),
		cocos2d::Color4F((float)(r * capacity) / 65025, (float)(g * capacity) / 65025, (float)(b * capacity) / 65025, (float)(a * capacity) / 65025));
}

void GraphicLabel::GraphicLabelCircle::Draw(GLubyte capacity)
{
	DrawPrimitives::setDrawColor4F((float)(r * capacity) / 65025, (float)(g * capacity) / 65025, (float)(b * capacity) / 65025, (float)(a * capacity) / 65025);
	DrawPrimitives::drawSolidCircle(cocos2d::Vec2(x, y), radius, CC_DEGREES_TO_RADIANS(CIRCLE_SEGEMENT), CIRCLE_SEGEMENT);
}

void GraphicLabel::GraphicLabelTriangle::Draw(GLubyte capacity)
{
	DrawPrimitives::drawSolidPoly(pt, sizeof(pt) / sizeof(pt[0]), cocos2d::Color4F((float)(r * capacity) / 65025, (float)(g * capacity) / 65025, (float)(b * capacity) / 65025, (float)(a * capacity) / 65025));
}

void GraphicLabel::GraphicLabelLine::Draw(GLubyte capacity)
{	
	glLineWidth(linewidth*CC_CONTENT_SCALE_FACTOR());
	DrawPrimitives::setDrawColor4F((float)(r * capacity) / 65025, (float)(g * capacity) / 65025, (float)(b * capacity) / 65025, (float)(a * capacity) / 65025);
	DrawPrimitives::drawLine(pt[0], pt[1]);
	glLineWidth(1 * CC_CONTENT_SCALE_FACTOR());
}

void GraphicLabel::GraphicLabelEmptyRect::Draw(GLubyte capacity)
{
	glLineWidth(linewidth*CC_CONTENT_SCALE_FACTOR());
	DrawPrimitives::setDrawColor4F((float)(r * capacity) / 65025, (float)(g * capacity) / 65025, (float)(b * capacity) / 65025, (float)(a * capacity) / 65025);
	DrawPrimitives::drawRect(cocos2d::Vec2(x, y), cocos2d::Vec2(x + width, y + height));	// 修复升级3.2后无法绘制图形的Bug edit by jiamiao 2015-2-4
	glLineWidth(1 * CC_CONTENT_SCALE_FACTOR());
}

void GraphicLabel::GraphicLabelEmptyCircle::Draw(GLubyte capacity)
{	
	glLineWidth(linewidth*CC_CONTENT_SCALE_FACTOR());
	DrawPrimitives::setDrawColor4F((float)(r * capacity) / 65025, (float)(g * capacity) / 65025, (float)(b * capacity) / 65025, (float)(a * capacity) / 65025);
	DrawPrimitives::drawCircle(cocos2d::Vec2(x, y), radius, CC_DEGREES_TO_RADIANS(CIRCLE_SEGEMENT), CIRCLE_SEGEMENT, false);
	glLineWidth(1 * CC_CONTENT_SCALE_FACTOR());
}

GraphicLabel* GraphicLabel::Node()
{
	GraphicLabel *graphic_label = new GraphicLabel();
	graphic_label->autorelease();

	return graphic_label;
}

//绘制实心矩形
void GraphicLabel::DrawSolidRect(float x, float y, float width, float height, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	GraphicLabelRect* rect_draw = new GraphicLabelRect();
	rect_draw->x = x;
	rect_draw->y = y;
	rect_draw->width = width;
	rect_draw->height = height;
	rect_draw->r = red;
	rect_draw->g = green;
	rect_draw->b = blue;
	rect_draw->a = alpha;
	m_draw_list.push_back(rect_draw);
}

//绘制实心圆形
void GraphicLabel::DrawSolidCircle(float x, float y, float radius, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	GraphicLabelCircle* circle_draw = new GraphicLabelCircle();
	circle_draw->x = x;
	circle_draw->y = y;
	circle_draw->radius = radius;
	circle_draw->r = red;
	circle_draw->g = green;
	circle_draw->b = blue;
	circle_draw->a = alpha;
	m_draw_list.push_back(circle_draw);
}

//绘制实心三角形
void GraphicLabel::DrawSolidTriangle(const COCOPoint& pt1, const COCOPoint& pt2, const COCOPoint& pt3, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	GraphicLabelTriangle *triangle_draw = new GraphicLabelTriangle();
	triangle_draw->pt[0] = pt1;
	triangle_draw->pt[1] = pt2;
	triangle_draw->pt[2] = pt3;
	triangle_draw->r = red;
	triangle_draw->g = green;
	triangle_draw->b = blue;
	triangle_draw->a = alpha;
	m_draw_list.push_back(triangle_draw);
}
//绘制线段
void GraphicLabel::DrawLine(const COCOPoint& pt1, const COCOPoint& pt2, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha, float linewidth)
{
	GraphicLabelLine *line_draw = new GraphicLabelLine();
	line_draw->pt[0] = pt1;
	line_draw->pt[1] = pt2;
	line_draw->r = red;
	line_draw->g = green;
	line_draw->b = blue;
	line_draw->a = alpha;
	line_draw->linewidth = linewidth;
	m_draw_list.push_back(line_draw);
}
//绘制空心矩形
void GraphicLabel::DrawRect(float x, float y, float width, float height, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha, float linewidth)
{
	GraphicLabelEmptyRect* rect_draw = new GraphicLabelEmptyRect();
	rect_draw->x = x;
	rect_draw->y = y;
	rect_draw->width = width;
	rect_draw->height = height;
	rect_draw->r = red;
	rect_draw->g = green;
	rect_draw->b = blue;
	rect_draw->a = alpha;
	rect_draw->linewidth = linewidth;
	m_draw_list.push_back(rect_draw);

}
//绘制空心圆
void GraphicLabel::DrawCircle(float x, float y, float radius, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha, float linewidth)
{
	GraphicLabelEmptyCircle* circle_draw = new GraphicLabelEmptyCircle();
	circle_draw->x = x;
	circle_draw->y = y;
	circle_draw->radius = radius;
	circle_draw->r = red;
	circle_draw->g = green;
	circle_draw->b = blue;
	circle_draw->a = alpha;
	circle_draw->linewidth = linewidth;
	m_draw_list.push_back(circle_draw);
}

void GraphicLabel::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags)
{
	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(GraphicLabel::onDraw, this, transform, flags);
	renderer->addCommand(&_customCommand);
}

void GraphicLabel::onDraw(const Mat4 &transform, uint32_t flags)
{
	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

	std::vector<GraphicLabelBase*>::iterator iter = m_draw_list.begin();

	while (iter != m_draw_list.end())
	{
		(*iter)->Draw(m_opacity);
		++iter;
	}

	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

//设置对位点
void GraphicLabel::SetAnchorPoint(float x, float y)
{
	cocos2d::Node::setAnchorPoint(cocos2d::Vec2(x, y));
}

void GraphicLabel::SetVisible(bool is_visible)
{
	cocos2d::Node::setVisible(is_visible);
}

//  设置缩放倍数
void GraphicLabel::SetScaleFactor(float factor)
{
	cocos2d::Node::setScale(factor);
}

void GraphicLabel::SetPosition(float x, float y)
{
	cocos2d::Node::setPosition(x, y);
}

void GraphicLabel::Clear()
{
	std::vector<GraphicLabelBase*>::iterator iter = m_draw_list.begin();

	while(iter != m_draw_list.end())
	{
		delete *iter++;
	}

	m_draw_list.clear();
}

GraphicLabel::GraphicLabel()
	:m_opacity(255), m_color(ccc3(0, 0, 0)), m_rgb_modify(false)
{}

GraphicLabel::~GraphicLabel()
{
	Clear();
}
