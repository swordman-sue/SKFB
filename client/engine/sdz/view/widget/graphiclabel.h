#ifndef GRAPHIC_LABEL_H
#define GRAPHIC_LABEL_H

#include "cocos2d.h"
#include "view/struct.h"
#include <vector>

//������ڵ�
class GraphicLabel : public cocos2d::Node
{
public:
	typedef enum
	{
		GL_NODEFINE = 1,		//������			
		GL_SOLID_RECT,			//ʵ�ľ���
		GL_SOLID_CIRCLE,		//ʵ��ԲȦ
		GL_SOLID_TRIANGLE,		//ʵ��������
		GL_NORMAL_LINE,			//��
		GL_RECT,				//����
		GL_CIRCLE,				//Բ��

	}DrawType;

	//���ƽڵ�Ļ�������
	class GraphicLabelBase
	{
	public:
		virtual void Draw(GLubyte capacity) = 0;
		DrawType type;		//��������
	};

	//ʵ�ľ���
	class GraphicLabelRect:public GraphicLabelBase
	{
	public:
		GraphicLabelRect(){type = GL_SOLID_RECT;}
		virtual void Draw(GLubyte capacity);

		float x, y, width, height;
		GLubyte r, g, b, a;
	};

	//ʵ��Բ��
	class GraphicLabelCircle:public GraphicLabelBase
	{
	public:
		GraphicLabelCircle(){type = GL_SOLID_CIRCLE;}
		virtual void Draw(GLubyte capacity);
		
		float x, y, radius;
		GLubyte r, g, b, a;
	};

	//ʵ��������
	class GraphicLabelTriangle:public GraphicLabelBase
	{
	public:
		GraphicLabelTriangle(){type = GL_SOLID_TRIANGLE;}
		virtual void Draw(GLubyte capacity);
		
		COCOPoint pt[3];
		GLubyte r, g, b, a;
	};

	//�߶�
	class GraphicLabelLine:public GraphicLabelBase
	{
	public:
		GraphicLabelLine(){type = GL_NORMAL_LINE;}
		virtual void Draw(GLubyte capacity);

		COCOPoint pt[2];
		float linewidth;
		GLubyte r, g, b, a;
	};

	//���ľ���
	class GraphicLabelEmptyRect:public GraphicLabelBase
	{
	public:
		GraphicLabelEmptyRect(){type = GL_RECT;}
		virtual void Draw(GLubyte capacity);

		float x, y, width, height, linewidth;
		GLubyte r, g, b, a;
	};

	//����Բ��
	class GraphicLabelEmptyCircle:public GraphicLabelBase
	{
	public:
		GraphicLabelEmptyCircle(){type = GL_CIRCLE;}
		virtual void Draw(GLubyte capacity);
		
		float x, y, radius, linewidth;
		GLubyte r, g, b, a;
	};


	//�������ƽڵ���
	static GraphicLabel* Node();

	//����ʵ�ľ���
	void DrawSolidRect(float x, float y, float width, float height, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);

	//����ʵ��Բ��
	void DrawSolidCircle(float x, float y, float radius, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);

	//����ʵ��������
	void DrawSolidTriangle(const COCOPoint& pt1, const COCOPoint& pt2, const COCOPoint& pt3, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);

	//�����߶�
	void DrawLine(const COCOPoint& pt1, const COCOPoint& pt2, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha, float linewidth);
	
	//���ƿ��ľ���
	void DrawRect(float x, float y, float width, float height, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha, float linewidth);

	//���ƿ���Բ��
	void DrawCircle(float x, float y, float radius, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha, float linewidth);
	
	//������л��ƵĽڵ�
	void Clear();

	//���ÿؼ���λ��
	//In: x, y �ؼ�������
	void SetPosition(float x, float y);

	//	���ö�λ��
	void SetAnchorPoint(float x, float y);

	//	�����Ƿ�ɼ�
	void SetVisible(bool is_visible);

	//  �������ű���
	void SetScaleFactor(float factor);

	//	����CCNode�Ļ��ƺ���,
	virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
	
	//��ȡ͸���ȣ��̳���CCRGBAProtocol
	virtual GLubyte getOpacity() const { return m_opacity; }

	//����͸���ȣ��̳���CCRGBAProtocol
    virtual void setOpacity(GLubyte opacity){m_opacity = opacity;}

	//����Ĭ����ɫ���̳���CCRGBAProtocol
	virtual void setColor(const cocos2d::Color3B& color){m_color = color;}
	
	//��ȡĬ����ɫ���̳���CCRGBAProtocol
	virtual const cocos2d::Color3B& getColor(){return m_color;}

	//�����Ƿ�����ɫ���̳���CCRGBAProtocol
	virtual void setOpacityModifyRGB(bool is_modify){m_rgb_modify = is_modify;}
	
	//��ȡ�Ƿ�����ɫ���̳���CCRGBAProtocol
	virtual bool isOpacityModifyRGB(){return m_rgb_modify;}
	/**
	//��ȡ��ʾ��ɫ�� �̳���CCRGBAProtocol
	virtual const cocos2d::ccColor3B& getDisplayedColor() { return getColor(); }

	//��ȡ��ʾ͸���ȣ��̳���CCRGBAProtocol
	virtual GLubyte getDisplayedOpacity() { return getOpacity(); }

	// ��ȡ�Ƿ����ɫ������child���̳���CCRGBAProtocol
	virtual bool isCascadeColorEnabled(){return false;}
	// �����Ƿ����ɫ������child���̳���CCRGBAProtocol
	virtual void setCascadeColorEnabled(bool cascadeColorEnabled){}
	// ѭ��������ʾ��ɫ�ķ������̳���CCRGBAProtocol
	virtual void updateDisplayedColor(const cocos2d::ccColor3B& color){m_color = color;}
	// ��ȡ�Ƿ��͸���ȴ�����child���̳���CCRGBAProtocol
	virtual bool isCascadeOpacityEnabled(){return false;}
	// �����Ƿ��͸���ȴ�����child���̳���CCRGBAProtocol
	virtual void setCascadeOpacityEnabled(bool cascadeOpacityEnabled){}
	// ѭ��������ʾ͸���ȵķ������̳���CCRGBAProtocol
	virtual void updateDisplayedOpacity(GLubyte opacity){m_opacity = opacity;}
	**/
	~GraphicLabel();

protected:
	void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
	cocos2d::CustomCommand _customCommand;

private:
	GraphicLabel();

	GLubyte							m_opacity;			//͸����
	cocos2d::Color3B				m_color;			//�����ɫ
	bool							m_rgb_modify;		//�Ƿ���ɫ����͸����
	std::vector<GraphicLabelBase*>	m_draw_list;		//�����б�
};

#endif