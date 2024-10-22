#include "cocos2d.h"
#include "graphicrender.h"
#include "view/widget/spriteex.h"
#include "filesystem/filesystemex.h"
#include "filesystem/filehelper.h"
#include "zqcommon/logutil.h"
#include "luaengine/luaengine.h"

USING_NS_CC;

unsigned int GraphicRender::m_call_back_func_id = 0;

GraphicRender* GraphicRender::Create(int w, int h)
{
	GraphicRender *graphic_render = new GraphicRender();

	if(graphic_render && graphic_render->initWithWidthAndHeight(w, h, kCCTexture2DPixelFormat_RGBA8888, GL_DEPTH24_STENCIL8))
	{
		graphic_render->autorelease();
		return graphic_render;
	}
	CC_SAFE_DELETE(graphic_render);
	return NULL;
}

void GraphicRender::afterCaptured(bool succeed, const std::string& outputFile)
{
	if (!succeed)
	{
		LogUtil::LogError("CaptureScreen is failure!");
	}

	FileHelper::ChmodFile(outputFile);
	LuaEngine::GetInstance()->CallFunction(m_call_back_func_id, "bs", succeed, outputFile.c_str());
}

std::string GraphicRender::CaptureScreen(std::string image_path, ZQ::UINT32 call_back_func_id)
{
	m_call_back_func_id = call_back_func_id;
	std::string full_image_path = FileSystemEx::GetInstance()->GetTmpPath() + image_path;
	utils::captureScreen(GraphicRender::afterCaptured, full_image_path);

	return full_image_path;
}

SpriteEx* GraphicRender::GetImageLabel()
{
	SpriteEx *image_label = SpriteEx::Node();
	if(image_label && image_label->InitWithTexture(getSprite()->getTexture()))
	{
		return image_label;
	}
	CC_SAFE_DELETE(image_label);
	return NULL;
}

//将绘制区内容保存到文件
bool GraphicRender::SaveToFile(std::string image_path)
{
	std::string full_image_path = FileSystemEx::GetInstance()->GetTmpPath() + image_path;
	if(!CCRenderTexture::saveToFile(full_image_path.c_str()))
	{
		LogUtil::LogError("[GraphicRender::SaveToFile]:Save Failed %s", image_path.c_str());
		return false;
	}
	
	return true;
}