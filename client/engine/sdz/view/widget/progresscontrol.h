#ifndef PROGRESS_CONTROL_H
#define PROGRESS_CONTROL_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include "view/widget/spriteex.h"
class ImageData;

//�������ؼ�
class ProgressControl : public cocos2d::Layer
{
public:
	~ProgressControl();

	//  ����һ���յĽ�����
	static ProgressControl* Node();

	// 	ʹ��ͼƬ����������
	// 	In:loading_image_file ��ȡʱ��ʾ��ͼƬ
	// 	In:background_image_file �������ı���ͼƬ
	// 	Return ����ָ���Ѵ�����ProgressControl��ָ��
	static ProgressControl* CreateWithImage(const std::string &loading_image_file, const std::string &background_image_file);
	static ProgressControl* CreateWithImage(const ImageData &loading_image, const ImageData &background_image);

	// 	ʹ��ͼƬ��ʼ��������
	// 	In:loading_image_file ��ȡʱ��ʾ��ͼƬ
	// 	In:background_image_file �������ı���ͼƬ
	bool InitWithImage(const std::string &loading_image_file, const std::string &background_image_file);
	bool InitWithImage(const ImageData &loading_image, const ImageData &background_image);

	//���ý�����ͼƬ
	bool SetProgressImage(const std::string &loading_image_file);
	bool SetProgressImage(const ImageData &loading_image);

	//���ý���������ͼƬ
	bool SetBkImage(const std::string &background_image_file);
	bool SetBkImage(const ImageData &background_image);

	// 	���ý�����������
	// 	In:max ������������ Ĭ��Ϊ100.0
	inline void SetProgressMax(float max){
		m_max_volume = max;
		SetCurrentProgress();
	}

	// 	���ӽ���������
	// 	In:volume ���ӵĽ�����
	// 	return: ��������֮ǰ�Ľ�����
	float AddProgress(float volume);

	// 	���ý���������
	// 	In:volume ���õĽ���
	void SetProgress(float volume);

	// 	��ȡ��ǰ����
	// 	return����ǰ����
	inline float GetProgress(){return m_current_volume;}
	inline float GetProgressMax(){ return m_max_volume; }

	//������ת
	void SetFlip(bool flip);

	virtual void onEnter();
	virtual void onExit();
protected:
	//���º�����ÿ֡����һ��
	virtual void update(float dt);

protected:
	ProgressControl();
	void SetCurrentProgress();

private:
	float					m_current_volume;				//��ǰ������
	float					m_max_volume;					//�ܽ�����
	float					m_dx, m_dy;						//λ��ƫ��
	float					m_width, m_height;				//�ؼ����
	SpriteEx				*m_loading_sprite;				//������
	SpriteEx				*m_background_sprite;			//����������
	float					m_flip;							//�Ƿ�ת����
};

#endif
