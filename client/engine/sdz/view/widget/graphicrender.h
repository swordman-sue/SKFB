#ifndef GRAPHIC_RENDER_H
#define GRAPHIC_RENDER_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"

class SpriteEx;

class GraphicRender : public cocos2d::RenderTexture
{
public:
	static GraphicRender* Create(int w, int h);
	//image_name 截屏文件的文件名， 截图后返回完整的文件路径
	static std::string CaptureScreen(std::string image_name, ZQ::UINT32 call_back_func_id);

public:
	//绑定写缓存区，调用此函数后，任何绘制的节点将在本缓存区上
	void Begin()
	{
		RenderTexture::begin();
	}

	void BeginWithClear(float r, float g, float b, float a)
	{
		RenderTexture::beginWithClear(r, g, b, a);
	}

	//取消绑定写缓存区
	void End()
	{
		RenderTexture::end();
	}

	//是否绘制自身节点内容
	void SetAutoDraw(bool auto_draw)
	{
		RenderTexture::setAutoDraw(auto_draw);
	}

	bool IsAutoDraw(){ return RenderTexture::isAutoDraw(); }

	//将绘制区内容保存到文件
	bool SaveToFile(std::string image_path);

public:
	SpriteEx* GetImageLabel();

private:
	static void afterCaptured(bool succeed, const std::string& outputFile);

private:
	static unsigned int m_call_back_func_id;
};

#endif