#ifndef CC_IMAGE_LABEL_H
#define CC_IMAGE_LABEL_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include "math/Mat4.h"

class ImageData;

namespace cocos2d
{
	class Renderer;
}

//ͼƬ��ʾ
class SpriteEx : public cocos2d::Sprite
{
public:
	static SpriteEx* Node();

	// 	ʹ��ͼƬ������ǩ�ؼ�
	// 	In : image_file ͼƬ�ļ�·��
	static SpriteEx* CreateWithImageFile(const std::string &image_file);
	static SpriteEx* CreateWithImageFile(const ImageData& image_data);

	//����Ϊpublic�Թ�GraphicRenderʹ��
	virtual bool InitWithTexture(cocos2d::Texture2D *texture);

	//	������ʾ��ͼƬ
	//	In: image_file �µ�ͼƬ��·��
	bool SetImage(const std::string &image_file);
	bool SetImage(const ImageData& image_data);

	// 	ʹ��ͼƬ��ʼ����ǩ�ؼ�
	// 	In : image_file ͼƬ�ļ�·��
	bool InitWithImageFile(const std::string &image_file);
	bool InitWithImageFile(const ImageData& image_data);

	//����������ɫ
	void SetColor(GLubyte red, GLubyte green, GLubyte blue);
	
	//�Զ����쵽width, height
	void SetStretchSize(ZQ::UINT32 width, ZQ::UINT32 height);
		
	//��ȡ���
	float GetWidth();
	float GetHeight();

	//���ø���
	void SetHighLight(bool highlight);
	void SetBright(float value);
	float GetBright();

	//���ûҶ�ֵ
	void SetGray(bool grey);

	//����ģ��
	void SetBlur(bool var);

	//������
	void SetFeather(bool var);

	/*����������
		0:������ĳ��Ϊ���ģ����������Բ����״����
		1:������ĳ��Ϊ���ģ�������Բ����״���𻯴�(�����������������������������ִ�����㷨)
	*/
	void SetFeatherType(int var) { m_feather_type = var; }

	/*�����𻯽���
		[0.0f, 1.0f]��Ĭ��Ϊ(0.5f, 0.5f)��������������ϵ
	*/
	void SetFeatherFocus(float u, float v);

	/*�����𻯽���ϵ��
		(0.0f, ������)��Ĭ��Ϊ1.0f��Խ��Խ�۽�
	*/
	void SetFeatherFocusFactor(float var);

	/*������˥����ֵ(��������Ϊ1ʱ��Ч)
	*/
	void SetFeatherAttenuationThreshold(float var);

	//���÷�ת
	void SetFlipX(bool flip_x);
	void SetFlipY(bool flip_y);

	virtual std::string getDescription() const { return "SpriteEx"; }

	virtual void onDraw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

protected:
	SpriteEx();

protected:
	bool m_highlight_enable;
	float m_highlight_value;

	bool m_grey_enable;

	bool m_blur_enable;

	bool m_feather_enable;
	int m_feather_type;
	float m_feather_focus_factor;
	cocos2d::Vec2 m_feather_focus;
	float m_feather_attenuation_threshold;

	cocos2d::GLProgram *m_shader_program;
};

#endif
