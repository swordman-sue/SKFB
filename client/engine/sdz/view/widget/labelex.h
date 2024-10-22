#ifndef CC_FONT_LABEL_H
#define CC_FONT_LABEL_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"

//������ʾ�ؼ�
class LabelEx : public cocos2d::Label
{
public:
	static LabelEx* Node();

	// 	ʹ�����ִ�����ǩ�ؼ�
	// 	In: text ��ǩ����
	//	In: font_name ����
	//	In: font_size �����С
	// 	return : ָ�򴴽���Label����
	static LabelEx* CreateWithString(const std::string &text, const std::string &font_name, float font_size);
	static LabelEx* CreateWithString(const std::string &text, const std::string &font_name, float font_size, float area_width, float area_height);

	void InitWithString(const std::string &text, const std::string &font_name, float font_size, float area_width, float area_height);

	//����������ɫ
	void SetColor(GLubyte red, GLubyte green, GLubyte blue);

	// 	�����ַ���
	// 	In: text ��ǩ����
	// 	In: font_name ����
	// 	In: font_size �����С	
	void SetString(const std::string &text);

	void SetFontName(const std::string &font_name);
	void SetFontSize(float font_size);

	void SetPosition(float x, float y);

	// 	��ȡ�ַ���
	// 	return: ��ǰ�ؼ����ַ���
	const char* GetString();

	// �������Ч��
	void SetStroke(bool stroke);
	// ������߿��
	void SetStrokeWidth(float width);
	// ���������ɫֵ��͸��ֵ
	void SetStrokeColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);
	// ����ȫ�����ѡ��
	static void SetStrokeOpt(bool stroke, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);

	void SetStrokeColorEx(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha, ZQ::UINT8 stroke_width);
    static void SetStrokeOptEx(bool stroke, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha, ZQ::UINT8 stroke_width);

	//������ӰЧ��
	void SetShadow(bool shadow);
	//������Ӱ��ȣ�����ʵ�֣�
	//void SetShadowWidth(float width){};
	// ������Ӱ��ɫֵ��͸��ֵ
	void SetShadowColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);
	//����ȫ����Ӱѡ��
	static void SetShadowOpt(bool stroke, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);

	// ����ÿ����󳤶�
	void SetLineWidth(ZQ::UINT32 width);
	// �������߶�
	void SetHeightMax(ZQ::UINT32 height);
	// ��������Ⱥ͸߶�
	void SetTextWidthAndHeight(ZQ::UINT32 width, ZQ::UINT32 height);

	//����ˮƽ���뷽ʽ
	void SetTextHorizontalAlign(int align);

	//���ô�ֱ���뷽ʽ
	void SetTextVertiacalAlign(int align);
	
	//��ȡ���
	float GetWidth() const;
	float GetHeight() const;

	//���÷�ת
	void SetFlipX(bool is_flipx);
	void SetFlipY(bool is_flipy);
	bool IsFlipX() const;
	bool IsFlipY() const;

	float GetContentWidth() const;
	float GetContentHeight() const;
protected:
	LabelEx();

public:
	static bool s_outline_enable;
	static cocos2d::Color4B s_outline_color;
    static ZQ::UINT8 s_outline_width;

	static bool s_shadow_enable;
	static cocos2d::Color4B s_shadow_color;

private:
	ZQ::UINT32	_line_width;			//ÿ�п��
	ZQ::UINT32	_height_max;			//���߶�

	bool _is_flipx;					//X��ת
	bool _is_flipy;					//y��ת

	cocos2d::Color4B _ex_outline_color;
	ZQ::UINT8 _outline_width;
	cocos2d::Color4B _ex_shadow_color;
};

#endif
