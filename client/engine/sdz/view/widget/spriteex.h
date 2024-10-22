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

//图片显示
class SpriteEx : public cocos2d::Sprite
{
public:
	static SpriteEx* Node();

	// 	使用图片创建标签控件
	// 	In : image_file 图片文件路径
	static SpriteEx* CreateWithImageFile(const std::string &image_file);
	static SpriteEx* CreateWithImageFile(const ImageData& image_data);

	//声明为public以供GraphicRender使用
	virtual bool InitWithTexture(cocos2d::Texture2D *texture);

	//	重置显示的图片
	//	In: image_file 新的图片的路径
	bool SetImage(const std::string &image_file);
	bool SetImage(const ImageData& image_data);

	// 	使用图片初始化标签控件
	// 	In : image_file 图片文件路径
	bool InitWithImageFile(const std::string &image_file);
	bool InitWithImageFile(const ImageData& image_data);

	//设置文字颜色
	void SetColor(GLubyte red, GLubyte green, GLubyte blue);
	
	//自动拉伸到width, height
	void SetStretchSize(ZQ::UINT32 width, ZQ::UINT32 height);
		
	//获取宽高
	float GetWidth();
	float GetHeight();

	//设置高亮
	void SetHighLight(bool highlight);
	void SetBright(float value);
	float GetBright();

	//设置灰度值
	void SetGray(bool grey);

	//设置模糊
	void SetBlur(bool var);

	//设置羽化
	void SetFeather(bool var);

	/*设置羽化类型
		0:以纹理某点为中心，对其进行正圆辐射状的羽化
		1:以纹理某点为中心，生成正圆辐射状的羽化带(不进行纹理采样，仅利用纹理坐标执行羽化算法)
	*/
	void SetFeatherType(int var) { m_feather_type = var; }

	/*设置羽化焦点
		[0.0f, 1.0f]，默认为(0.5f, 0.5f)，处于纹理坐标系
	*/
	void SetFeatherFocus(float u, float v);

	/*设置羽化焦点系数
		(0.0f, 正无穷)，默认为1.0f，越大越聚焦
	*/
	void SetFeatherFocusFactor(float var);

	/*设置羽化衰减阈值(仅当类型为1时生效)
	*/
	void SetFeatherAttenuationThreshold(float var);

	//设置翻转
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
