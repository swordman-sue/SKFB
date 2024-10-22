#ifndef CC_FONT_LABEL_H
#define CC_FONT_LABEL_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"

//������ʾ�ؼ�
class FontLabel : public cocos2d::LabelTTF
{
public:
	static const ZQ::UINT32 EFFECT_NONE = 0;
	static const ZQ::UINT32 EFFECT_SHADOW = 0x10;
	static const ZQ::UINT32 EFFECT_OUTLINE = 0x20;

	static FontLabel* Node();

	// 	ʹ�����ִ�����ǩ�ؼ�
	// 	In: text ��ǩ����
	//	In: font_name ����
	//	In: font_size �����С
	// 	return : ָ�򴴽���Label����
	static FontLabel* CreateWithString(const std::string &text, const std::string &font_name, float font_size);
	static FontLabel* CreateWithString(const std::string &text, const std::string &font_name, float font_size, float area_width, float area_height);

	//����������ɫ
	void SetColor(GLubyte red, GLubyte green, GLubyte blue);

	//	����͸����
	void SetOpacity(GLubyte var);
	
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

	// 	ʹ�����ֳ�ʼ����ǩ�ؼ�
	// 	In: text ��ǩ����
	// 	In: font_name ����
	// 	In: font_size �����С
	bool InitWithString(const std::string &text, const std::string &font_name, float font_size);
	bool InitWithString(const std::string &text, const std::string &font_name, float font_size, float area_width, float area_height);

	// �������Ч��
	void SetStroke(bool stroke);
	// ���������ɫֵ��͸��ֵ
	void SetStrokeColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);

	//������ӰЧ��
	void SetShadow(bool shadow);
	// ������Ӱ��ɫֵ��͸��ֵ
	void SetShadowColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);

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

	//������ߺ���Ӱѡ��
	void SetShadowOpt(bool stroke, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);
	void SetOutLineOpt(bool outline, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);
	void SetShadowWidth(float width);
	void SetOutlineWidth(float width);
	//��ȡ���
	float GetWidth();
	float GetHeight();

	void SetFlipX(bool is_flipx);
	void SetFlipY(bool is_flipy);
	bool IsFlipX();
	bool IsFlipY();
	void SetSwallow(bool swallow){};

	virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags) override;
protected:
	FontLabel();

private:
	ZQ::UINT32	m_line_width;			//ÿ�п��
	ZQ::UINT32	m_height_max;			//���߶�
	std::string	m_text;					//�ַ���
	bool m_is_flipx;					//X��ת
	bool m_is_flipy;					//y��ת
	ZQ::UINT32	m_effect;				//labelЧ��
	cocos2d::Color4B m_shadow_color;	//��Ӱ��ɫ
	cocos2d::Color4B m_outline_color;	//�����ɫ
	float m_shadow_width;
	float m_outline_width;
};

#endif
