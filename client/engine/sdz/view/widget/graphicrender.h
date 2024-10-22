#ifndef GRAPHIC_RENDER_H
#define GRAPHIC_RENDER_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"

class SpriteEx;

class GraphicRender : public cocos2d::RenderTexture
{
public:
	static GraphicRender* Create(int w, int h);
	//image_name �����ļ����ļ����� ��ͼ�󷵻��������ļ�·��
	static std::string CaptureScreen(std::string image_name, ZQ::UINT32 call_back_func_id);

public:
	//��д�����������ô˺������κλ��ƵĽڵ㽫�ڱ���������
	void Begin()
	{
		RenderTexture::begin();
	}

	void BeginWithClear(float r, float g, float b, float a)
	{
		RenderTexture::beginWithClear(r, g, b, a);
	}

	//ȡ����д������
	void End()
	{
		RenderTexture::end();
	}

	//�Ƿ��������ڵ�����
	void SetAutoDraw(bool auto_draw)
	{
		RenderTexture::setAutoDraw(auto_draw);
	}

	bool IsAutoDraw(){ return RenderTexture::isAutoDraw(); }

	//�����������ݱ��浽�ļ�
	bool SaveToFile(std::string image_path);

public:
	SpriteEx* GetImageLabel();

private:
	static void afterCaptured(bool succeed, const std::string& outputFile);

private:
	static unsigned int m_call_back_func_id;
};

#endif