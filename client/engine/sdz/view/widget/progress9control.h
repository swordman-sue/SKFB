#ifndef PROGRESS9_CONTROL_H
#define PROGRESS9_CONTROL_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include "view/widget/scale9spriteex.h"

class ImageData;

namespace cocos2d
{
	class DrawNode;
	class ClippingNode;
}

//�������ؼ�
class Progress9Control : public cocos2d::Layer
{
public:
	~Progress9Control();

	//����һ���յĽ�����
	static Progress9Control* Node();

	//ʹ��ͼƬ����������
	//In:loading_image_file ��ȡʱ��ʾ��ͼƬ
	//In:background_image_file �������ı���ͼƬ
	//Return ����ָ���Ѵ�����ProgressControl��ָ��
	static Progress9Control* CreateWithImage(const std::string &loading_image_file);
	static Progress9Control* CreateWithImage(const ImageData &loading_image);

	//���ý�����ͼƬ
	bool SetProgressImage(const std::string &loading_image_file);
	bool SetProgressImage(const ImageData &loading_image);

	//���ý�����������
	//In:max ������������ Ĭ��Ϊ100.0
	inline void SetProgressMax(float max) 
	{ 
		m_max_volume = max; 
	}

	//���ý���������
	//In:volume ���õĽ���
	inline void SetProgress(float volume)
	{
		if (volume != m_current_volume)
		{
			m_bVolumeDirty = true;
			m_current_volume = volume;
		}
	}

	//���ӽ���������
	//In:volume ���ӵĽ�����
	//return: ��������֮ǰ�Ľ�����
	float AddProgress(float volume);

	//��ȡ��ǰ����
	//return����ǰ����
	inline float GetProgress() { return m_current_volume; }

protected:
	//���º�����ÿ֡����һ��
	virtual void update(float dt);

protected:
	Progress9Control();

private:
	float					m_current_volume;				//��ǰ������
	float					m_max_volume;					//�ܽ�����
	Scale9SpriteEx*			m_loading_sprite;				//������

	//���ģ��ü�������ȱ仯��ģʽ add by swordman sue 2016-11-5
	bool					m_bVolumeDirty;					//������λ
	cocos2d::DrawNode*		m_pClippingStencil;				//ģ��ڵ�
	cocos2d::ClippingNode*	m_pClippingNode;
};

#endif
