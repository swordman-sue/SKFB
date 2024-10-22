#ifndef IMAGE_BUTTON_H
#define IMAGE_BUTTON_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include "view/struct.h"

class ImageData;
class SpriteEx;
class Scale9SpriteEx;

//动态按钮
class ImageButton : public cocos2d::Menu
{
public:
	typedef void (Ref::*BUTTON_EVENT_FUNC)(float, float); 

public:
	~ImageButton();

	// 创建一个空的按钮
	static ImageButton* Node();

	bool Init();
	bool InitWithImage(const std::string &image_file);
	bool InitWithImage(const ImageData& image_data);

	// 	使用图片创建按钮
	// 	In: normal_image 正常显示的图片
	//  In: down_image 下按时显示的图片
	//  In: disable_image 失效时使用的图片
	static ImageButton* CreateWithImages(const std::string &image_file);
	static ImageButton* CreateWithImages(const ImageData&);
	static ImageButton* CreateWithImages(const std::string &normal_image, const std::string &down_image, const std::string &disable_image);
	static ImageButton* CreateWithImages(const ImageData &normal_image, const ImageData &down_image, const ImageData &disable_image);
	
	// 	设置下按状态时显示的图片
	// 	In:image_file 下按时显示的图片路径
	void SetNormalImage(const std::string &image_file);
	void SetNormalImage(const ImageData& image_data);

	// 	设置下按状态时显示的图片
	// 	In:image_file 下按时显示的图片路径
	void SetSelectedImage(const std::string &image_file);
	void SetSelectedImage(const ImageData& image_data);

	// 	设置失效状态时显示的图片
	// 	In:image_file 失效时显示的图片路径
	void SetDisabledImage(const std::string &image_file);
	void SetDisabledImage(const ImageData &image_data);


	//设置按钮是否失效
	//In: able 是否生效
	void SetIsEnabled(bool able);

	//  Lua版本的回调函数
	void SetLuaTouchDown(ZQ::UINT32 down_call_fun){m_lua_down_fun_id = down_call_fun;}
	void SetLuaTouchMove(ZQ::UINT32 move_call_fun){m_lua_move_fun_id = move_call_fun;}
	void SetLuaTouchUp(ZQ::UINT32 up_call_fun){m_lua_up_fun_id = up_call_fun;}
	void SetLuaTouchCancel(ZQ::UINT32 cancel_call_fun){m_lua_cancel_fun_id = cancel_call_fun;}
	void SetLuaCallBack(ZQ::UINT32 down_call_fun, ZQ::UINT32 move_call_fun, ZQ::UINT32 up_call_fun, ZQ::UINT32 cancel_call_fun);

	//设置按钮的文字
	void SetButtonText(const std::string &text, const std::string &font_name, float font_size);
	//设置按钮文字的颜色
	void SetButtonTextColor(ZQ::UINT8, ZQ::UINT8, ZQ::UINT8);
	//设置按钮变化后文字的颜色
	void SetDisableButtonTextColor(ZQ::UINT8, ZQ::UINT8, ZQ::UINT8);

	//设置触摸状态图片叠加
	void SetTouchDownOverLap(bool is_overlap){m_touch_overlap = is_overlap;}

	//设置拉伸大小
	void SetStretchSize(float, float);

	//设置x翻转
	void SetFlipX(bool flip);
	//设置y翻转
	void SetFlipY(bool flip);

	//获取宽高
	float GetWidth();
	float GetHeight();

	//获取正常状态下的图片控件
	Scale9SpriteEx* GetNormalImageNode();
	//获取下按状态下的图片控件
	Scale9SpriteEx* GetSelectedImageNode();
	//获取无状态下的图片控件
	Scale9SpriteEx* GetDisableImageNode();

	//设置自动生成
	void SetAutoGen(bool auto_gen){ m_auto_gen = auto_gen; }
	bool GetAutoGen(){ return m_auto_gen; }

protected:
	//继承自CCMenu, 处理触摸事件
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);

private:
	ImageButton();
	template<class T> void SetNormalImageTemplate(T&);
private:
	Scale9SpriteEx											*m_normal_sprite;			//按钮内嵌的图片精灵(正常状态)
	Scale9SpriteEx											*m_selected_sprite;			//按钮内嵌的图片精灵（选中状态）
	Scale9SpriteEx											*m_disable_sprite;			//按钮内嵌的图片精灵（失效状态）
	cocos2d::MenuItemLabel								*m_menu_text_item;			//显示文字的按钮项
	cocos2d::MenuItemSprite								*m_menuitem_sprite;			//精灵菜单项
	float												m_width, m_height;			//按钮宽高
	bool												m_enable;					//是否生效
	cocos2d::Color3B									m_text_color;				//按钮文字颜色
	cocos2d::Color3B									m_disable_text_color;		//无效按钮文字颜色
	bool												m_touch_overlap;			//下按时是否叠加
	float												m_stretch_width;			//拉伸大小
	float												m_stretch_height;
	bool												m_auto_gen;					//是否自动生成高亮跟灰度图片
	bool												m_flip_x;
	bool												m_flip_y;					//翻转
public:
	//lua回调
	ZQ::UINT32											m_lua_down_fun_id;			//按钮下按后的回调函数
	ZQ::UINT32											m_lua_move_fun_id;			//按钮移动时的回调函数
	ZQ::UINT32											m_lua_up_fun_id;			//按钮按住松开后的回调函数
	ZQ::UINT32											m_lua_cancel_fun_id;		//按钮按住松开后的回调函数
};

#endif
