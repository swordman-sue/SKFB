#ifndef GRAPHIC_LABEL_H
#define GRAPHIC_LABEL_H

#include "cocos2d.h"
#include "view/struct.h"
#include <vector>

//绘制类节点
class GraphicLabel : public cocos2d::Node
{
public:
	typedef enum
	{
		GL_NODEFINE = 1,		//不绘制			
		GL_SOLID_RECT,			//实心矩形
		GL_SOLID_CIRCLE,		//实心圆圈
		GL_SOLID_TRIANGLE,		//实心三角形
		GL_NORMAL_LINE,			//线
		GL_RECT,				//矩形
		GL_CIRCLE,				//圆形

	}DrawType;

	//绘制节点的基本类型
	class GraphicLabelBase
	{
	public:
		virtual void Draw(GLubyte capacity) = 0;
		DrawType type;		//绘制类型
	};

	//实心矩形
	class GraphicLabelRect:public GraphicLabelBase
	{
	public:
		GraphicLabelRect(){type = GL_SOLID_RECT;}
		virtual void Draw(GLubyte capacity);

		float x, y, width, height;
		GLubyte r, g, b, a;
	};

	//实心圆形
	class GraphicLabelCircle:public GraphicLabelBase
	{
	public:
		GraphicLabelCircle(){type = GL_SOLID_CIRCLE;}
		virtual void Draw(GLubyte capacity);
		
		float x, y, radius;
		GLubyte r, g, b, a;
	};

	//实心三角形
	class GraphicLabelTriangle:public GraphicLabelBase
	{
	public:
		GraphicLabelTriangle(){type = GL_SOLID_TRIANGLE;}
		virtual void Draw(GLubyte capacity);
		
		COCOPoint pt[3];
		GLubyte r, g, b, a;
	};

	//线段
	class GraphicLabelLine:public GraphicLabelBase
	{
	public:
		GraphicLabelLine(){type = GL_NORMAL_LINE;}
		virtual void Draw(GLubyte capacity);

		COCOPoint pt[2];
		float linewidth;
		GLubyte r, g, b, a;
	};

	//空心矩形
	class GraphicLabelEmptyRect:public GraphicLabelBase
	{
	public:
		GraphicLabelEmptyRect(){type = GL_RECT;}
		virtual void Draw(GLubyte capacity);

		float x, y, width, height, linewidth;
		GLubyte r, g, b, a;
	};

	//空心圆形
	class GraphicLabelEmptyCircle:public GraphicLabelBase
	{
	public:
		GraphicLabelEmptyCircle(){type = GL_CIRCLE;}
		virtual void Draw(GLubyte capacity);
		
		float x, y, radius, linewidth;
		GLubyte r, g, b, a;
	};


	//创建绘制节点类
	static GraphicLabel* Node();

	//绘制实心矩形
	void DrawSolidRect(float x, float y, float width, float height, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);

	//绘制实心圆形
	void DrawSolidCircle(float x, float y, float radius, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);

	//绘制实心三角形
	void DrawSolidTriangle(const COCOPoint& pt1, const COCOPoint& pt2, const COCOPoint& pt3, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);

	//绘制线段
	void DrawLine(const COCOPoint& pt1, const COCOPoint& pt2, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha, float linewidth);
	
	//绘制空心矩形
	void DrawRect(float x, float y, float width, float height, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha, float linewidth);

	//绘制空心圆形
	void DrawCircle(float x, float y, float radius, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha, float linewidth);
	
	//清除所有绘制的节点
	void Clear();

	//设置控件的位置
	//In: x, y 控件的坐标
	void SetPosition(float x, float y);

	//	设置对位点
	void SetAnchorPoint(float x, float y);

	//	设置是否可见
	void SetVisible(bool is_visible);

	//  设置缩放倍数
	void SetScaleFactor(float factor);

	//	重载CCNode的绘制函数,
	virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
	
	//获取透明度，继承自CCRGBAProtocol
	virtual GLubyte getOpacity() const { return m_opacity; }

	//设置透明度，继承自CCRGBAProtocol
    virtual void setOpacity(GLubyte opacity){m_opacity = opacity;}

	//设置默认颜色，继承自CCRGBAProtocol
	virtual void setColor(const cocos2d::Color3B& color){m_color = color;}
	
	//获取默认颜色，继承自CCRGBAProtocol
	virtual const cocos2d::Color3B& getColor(){return m_color;}

	//设置是否混乘颜色，继承自CCRGBAProtocol
	virtual void setOpacityModifyRGB(bool is_modify){m_rgb_modify = is_modify;}
	
	//获取是否混乘颜色，继承自CCRGBAProtocol
	virtual bool isOpacityModifyRGB(){return m_rgb_modify;}
	/**
	//获取显示颜色， 继承自CCRGBAProtocol
	virtual const cocos2d::ccColor3B& getDisplayedColor() { return getColor(); }

	//获取显示透明度，继承自CCRGBAProtocol
	virtual GLubyte getDisplayedOpacity() { return getOpacity(); }

	// 获取是否把颜色传播到child，继承自CCRGBAProtocol
	virtual bool isCascadeColorEnabled(){return false;}
	// 设置是否把颜色传播到child，继承自CCRGBAProtocol
	virtual void setCascadeColorEnabled(bool cascadeColorEnabled){}
	// 循环更新显示颜色的方法，继承自CCRGBAProtocol
	virtual void updateDisplayedColor(const cocos2d::ccColor3B& color){m_color = color;}
	// 获取是否把透明度传播到child，继承自CCRGBAProtocol
	virtual bool isCascadeOpacityEnabled(){return false;}
	// 设置是否把透明度传播到child，继承自CCRGBAProtocol
	virtual void setCascadeOpacityEnabled(bool cascadeOpacityEnabled){}
	// 循环更新显示透明度的方法，继承自CCRGBAProtocol
	virtual void updateDisplayedOpacity(GLubyte opacity){m_opacity = opacity;}
	**/
	~GraphicLabel();

protected:
	void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
	cocos2d::CustomCommand _customCommand;

private:
	GraphicLabel();

	GLubyte							m_opacity;			//透明度
	cocos2d::Color3B				m_color;			//混合颜色
	bool							m_rgb_modify;		//是否将颜色乘于透明度
	std::vector<GraphicLabelBase*>	m_draw_list;		//绘制列表
};

#endif