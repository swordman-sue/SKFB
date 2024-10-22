#ifndef CC_IMAGE9_LABEL_H
#define CC_IMAGE9_LABEL_H

#include "ui/UIScale9Sprite.h"
#include "zqcommon/typedef.h"
class ImageData;

class Scale9SpriteEx : public cocos2d::ui::Scale9Sprite
{
public:
	Scale9SpriteEx();
	virtual ~Scale9SpriteEx();
	static Scale9SpriteEx* Node();

	//创建九宫格控件
	static Scale9SpriteEx* CreateWithImageFile(const std::string &image_file);
	static Scale9SpriteEx* CreateWithImageFile(const ImageData& image_data);

public:
	//初始化九宫格控件
	bool InitWithImageFile(const std::string &image_file);
	bool InitWithImageFile(const ImageData& image_data);

	//设置九宫格图片
	bool SetImage(const std::string &image_file);
	bool SetImage(const ImageData& image_data);

	//设置拉伸大小
	void SetStretchSize(float, float);
	void SetContentWidth(float);
	void SetContentHeight(float);
	void SetContentSize(float, float);

	//设置颜色
	void SetColor(GLubyte red, GLubyte green, GLubyte blue);

	//获取宽高
	float GetWidth(){ return m_stretch_size.width; }
	float GetHeight(){ return  m_stretch_size.height; }
	float GetOriginWidth() { return m_origin_size.width; }
	float GetOriginHeight() { return m_origin_size.height; }

	//设置九宫格属性
	void SetInnerRect(const cocos2d::Rect& rc_inner);
	void SetInnerRect(float x, float y, float width, float height);
	cocos2d::Rect GetInnerRect() const { return m_rect_inner; }

	//设置高亮
	void SetHighLight(bool highlight);
	void SetBright(float);
	float GetBright(){ return m_highlight_value; }

	//设置灰度值
	void SetGray(bool grey);

	void SetFlipX(bool flip_x);
	void SetFlipY(bool flip_y);
	bool IsFlipX(){ return m_flip_x; }
	bool IsFlipY(){ return m_flip_y; }

	virtual void onDraw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

protected:
	cocos2d::Rect m_rect_inner;
	cocos2d::Size m_stretch_size;
	cocos2d::Size m_origin_size;
	cocos2d::Rect m_texture_rect;

	bool m_highlight_enable;
	bool m_grey_enable;
	cocos2d::GLProgramState *m_shader_program;
	float m_highlight_value;

	bool m_flip_x;
	bool m_flip_y;
};

#endif