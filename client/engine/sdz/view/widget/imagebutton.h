#ifndef IMAGE_BUTTON_H
#define IMAGE_BUTTON_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include "view/struct.h"

class ImageData;
class SpriteEx;
class Scale9SpriteEx;

//��̬��ť
class ImageButton : public cocos2d::Menu
{
public:
	typedef void (Ref::*BUTTON_EVENT_FUNC)(float, float); 

public:
	~ImageButton();

	// ����һ���յİ�ť
	static ImageButton* Node();

	bool Init();
	bool InitWithImage(const std::string &image_file);
	bool InitWithImage(const ImageData& image_data);

	// 	ʹ��ͼƬ������ť
	// 	In: normal_image ������ʾ��ͼƬ
	//  In: down_image �°�ʱ��ʾ��ͼƬ
	//  In: disable_image ʧЧʱʹ�õ�ͼƬ
	static ImageButton* CreateWithImages(const std::string &image_file);
	static ImageButton* CreateWithImages(const ImageData&);
	static ImageButton* CreateWithImages(const std::string &normal_image, const std::string &down_image, const std::string &disable_image);
	static ImageButton* CreateWithImages(const ImageData &normal_image, const ImageData &down_image, const ImageData &disable_image);
	
	// 	�����°�״̬ʱ��ʾ��ͼƬ
	// 	In:image_file �°�ʱ��ʾ��ͼƬ·��
	void SetNormalImage(const std::string &image_file);
	void SetNormalImage(const ImageData& image_data);

	// 	�����°�״̬ʱ��ʾ��ͼƬ
	// 	In:image_file �°�ʱ��ʾ��ͼƬ·��
	void SetSelectedImage(const std::string &image_file);
	void SetSelectedImage(const ImageData& image_data);

	// 	����ʧЧ״̬ʱ��ʾ��ͼƬ
	// 	In:image_file ʧЧʱ��ʾ��ͼƬ·��
	void SetDisabledImage(const std::string &image_file);
	void SetDisabledImage(const ImageData &image_data);


	//���ð�ť�Ƿ�ʧЧ
	//In: able �Ƿ���Ч
	void SetIsEnabled(bool able);

	//  Lua�汾�Ļص�����
	void SetLuaTouchDown(ZQ::UINT32 down_call_fun){m_lua_down_fun_id = down_call_fun;}
	void SetLuaTouchMove(ZQ::UINT32 move_call_fun){m_lua_move_fun_id = move_call_fun;}
	void SetLuaTouchUp(ZQ::UINT32 up_call_fun){m_lua_up_fun_id = up_call_fun;}
	void SetLuaTouchCancel(ZQ::UINT32 cancel_call_fun){m_lua_cancel_fun_id = cancel_call_fun;}
	void SetLuaCallBack(ZQ::UINT32 down_call_fun, ZQ::UINT32 move_call_fun, ZQ::UINT32 up_call_fun, ZQ::UINT32 cancel_call_fun);

	//���ð�ť������
	void SetButtonText(const std::string &text, const std::string &font_name, float font_size);
	//���ð�ť���ֵ���ɫ
	void SetButtonTextColor(ZQ::UINT8, ZQ::UINT8, ZQ::UINT8);
	//���ð�ť�仯�����ֵ���ɫ
	void SetDisableButtonTextColor(ZQ::UINT8, ZQ::UINT8, ZQ::UINT8);

	//���ô���״̬ͼƬ����
	void SetTouchDownOverLap(bool is_overlap){m_touch_overlap = is_overlap;}

	//���������С
	void SetStretchSize(float, float);

	//����x��ת
	void SetFlipX(bool flip);
	//����y��ת
	void SetFlipY(bool flip);

	//��ȡ���
	float GetWidth();
	float GetHeight();

	//��ȡ����״̬�µ�ͼƬ�ؼ�
	Scale9SpriteEx* GetNormalImageNode();
	//��ȡ�°�״̬�µ�ͼƬ�ؼ�
	Scale9SpriteEx* GetSelectedImageNode();
	//��ȡ��״̬�µ�ͼƬ�ؼ�
	Scale9SpriteEx* GetDisableImageNode();

	//�����Զ�����
	void SetAutoGen(bool auto_gen){ m_auto_gen = auto_gen; }
	bool GetAutoGen(){ return m_auto_gen; }

protected:
	//�̳���CCMenu, �������¼�
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);

private:
	ImageButton();
	template<class T> void SetNormalImageTemplate(T&);
private:
	Scale9SpriteEx											*m_normal_sprite;			//��ť��Ƕ��ͼƬ����(����״̬)
	Scale9SpriteEx											*m_selected_sprite;			//��ť��Ƕ��ͼƬ���飨ѡ��״̬��
	Scale9SpriteEx											*m_disable_sprite;			//��ť��Ƕ��ͼƬ���飨ʧЧ״̬��
	cocos2d::MenuItemLabel								*m_menu_text_item;			//��ʾ���ֵİ�ť��
	cocos2d::MenuItemSprite								*m_menuitem_sprite;			//����˵���
	float												m_width, m_height;			//��ť���
	bool												m_enable;					//�Ƿ���Ч
	cocos2d::Color3B									m_text_color;				//��ť������ɫ
	cocos2d::Color3B									m_disable_text_color;		//��Ч��ť������ɫ
	bool												m_touch_overlap;			//�°�ʱ�Ƿ����
	float												m_stretch_width;			//�����С
	float												m_stretch_height;
	bool												m_auto_gen;					//�Ƿ��Զ����ɸ������Ҷ�ͼƬ
	bool												m_flip_x;
	bool												m_flip_y;					//��ת
public:
	//lua�ص�
	ZQ::UINT32											m_lua_down_fun_id;			//��ť�°���Ļص�����
	ZQ::UINT32											m_lua_move_fun_id;			//��ť�ƶ�ʱ�Ļص�����
	ZQ::UINT32											m_lua_up_fun_id;			//��ť��ס�ɿ���Ļص�����
	ZQ::UINT32											m_lua_cancel_fun_id;		//��ť��ס�ɿ���Ļص�����
};

#endif
