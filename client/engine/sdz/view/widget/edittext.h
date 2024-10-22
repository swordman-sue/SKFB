#ifndef EDIT_TEXT_H
#define EDIT_TEXT_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include <string>

//�༭�ؼ�
class EditText : public cocos2d::TextFieldTTF, public cocos2d::TextFieldDelegate
{
public:

	static ZQ::UINT32 current_edittext_fun_id;

	//����һ���յı༭��ؼ�
	static EditText* Node();

	//����һ���༭��ؼ�
	static EditText* CreateEditText(const std::string &holder_text, float width, float height, const std::string &font_name, float font_size);
	//��ʼ���༭��
	bool InitEditText(const std::string &holder_text, float width, float height, const std::string &font_name, float font_size);
	//���ñ༭����ʾ����
	void SetHolderText(const std::string &holder_text);
	//���ñ༭����ı�
	void SetText(const std::string &text);
	//���ñ༭����ʾ��������
	void SetFontName(const std::string &font_name);
	//���ñ༭����ʾ���ִ�С
	void SetFontSize(float font_size);
	//���ر༭����ı�
	const char* GetText(){return m_text.c_str();}
	//����Ԥ����������ɫ
	void SetPlaceHolderColor(GLubyte red, GLubyte green, GLubyte blue);
	//����������ɫ
	void SetColor(GLubyte red, GLubyte green, GLubyte blue);
	//����͸����
	void SetOpacity(GLubyte alpha);
	//���ÿؼ���λ��
	//In: x, y �ؼ�������
	virtual void setPosition(const cocos2d::Vec2& pos);
	void SetPosition(float x, float y);
	//�������ֿ�Ĵ�С
	void SetContentSize(float width, float height);
	//���ó�����ģʽ
	void SetPasswordMode(bool is_password){m_is_password = is_password;}
	//������󳤶�
	void SetMaxLength(ZQ::UINT32 len){m_text_max = len;}
	//������ʾ����󳤶�
	void SetMaxShowLength(float show_length){m_show_length = show_length;}
	//���ö�λ��
	void SetAnchorPoint(float x, float y);
	//���ù����ַ�
	void SetFilterWords(const std::string& words){m_filter_str = words;};
	//������������
	void SetExpand(ZQ::UINT32 width, ZQ::UINT32 height);

	//���̵�����Ϣ��Lua�ص�
	void SetKeyBoardWillShowCallBack(ZQ::UINT32 call_fun_id){m_keyboard_willshow_id = call_fun_id;}
	void SetKeyBoardDidShowCallBack(ZQ::UINT32 call_fun_id){m_keyboard_didshow_id = call_fun_id;}
	void SetKeyBoardWillHideCallBack(ZQ::UINT32 call_fun_id){m_keyboard_willhide_id = call_fun_id;}
	void SetKeyBoardDidHideCallBack(ZQ::UINT32 call_fun_id){m_keyboard_didhide_id = call_fun_id;}	
	
	//�̳���CCTextFieldDelegate
	virtual bool onTextFieldInsertText(TextFieldTTF * sender, const char * text, size_t nLen);
	virtual bool onTextFieldDeleteBackward(TextFieldTTF * sender, const char * delText, size_t nLen);
	//�̳���CCIMEDelegate���麯��,��ȡ����ʽ���̵���Ϣ
	virtual void keyboardWillShow(cocos2d::IMEKeyboardNotificationInfo& info);
	virtual void keyboardDidShow(cocos2d::IMEKeyboardNotificationInfo& info);
	virtual void keyboardWillHide(cocos2d::IMEKeyboardNotificationInfo& info);
	virtual void keyboardDidHide(cocos2d::IMEKeyboardNotificationInfo& info);
	
	virtual void onEnter();
	virtual void onExit();
	~EditText();

	virtual bool attachWithIME();
	virtual bool detachWithIME();

	virtual void visit(cocos2d::Renderer* renderer, const cocos2d::Mat4 &transform, uint32_t flags);

	//�������ֱ��ʱ��֪ͨ
	void SetTextChangedCallBack(ZQ::UINT32 call_fun_id){m_text_changed_id = call_fun_id;}

	void SetCursorSprite(cocos2d::Sprite *cursor_sprite);
	cocos2d::Rect GetTouchRect();

public:
	static bool is_keyboard_show;
	static int	keyboard_lock_count;

protected:
	EditText();
	//ת�������봮
	std::string TranslateToPassword(const std::string &str);
	//���ù�꾫���λ��
	void ResetCursorPosition();
	//�����������
	void ResetVisibleSection();

	void onBeforeVisitScissor();
	void onAfterVisitScissor();

	cocos2d::Size		m_content_size;				//�༭�������С
	std::string			m_text;						//�༭������
	std::string			m_password;					//���봮
	ZQ::UINT32			m_text_max;					//���ֳ�������
	bool				m_is_password;				//�Ƿ�������ģʽ
	cocos2d::Sprite		*m_cursor_sprite;			//��꾫��
	bool				m_capture;					//�Ƿ���������
	std::string			m_filter_str;				//�����ַ���
	float				m_pos_x, m_pos_y;			//λ��
	float				m_offset_x, m_offset_y;		//ƫ����
	float				m_show_length;				//��ʾ����
	std::string			m_holder_text;				//�༭��Ĭ������
	ZQ::UINT32			m_touch_width_ex;			//���Ӵ������
	ZQ::UINT32			m_touch_height_ex;			//���Ӵ����߶�

	ZQ::UINT32			m_keyboard_willshow_id;		
	ZQ::UINT32			m_keyboard_didshow_id;
	ZQ::UINT32			m_keyboard_willhide_id;
	ZQ::UINT32			m_keyboard_didhide_id;
	ZQ::UINT32			m_text_changed_id;

	GLint m_scissor_section[4];
	GLboolean m_gl_is_scissor;
	cocos2d::CustomCommand _beforeVisitCmdScissor;
	cocos2d::CustomCommand _afterVisitCmdScissor;

private:
	enum class FontType
	{
		SYSTEM,
		TTF
	};
	FontType _fontType;
};

#endif
