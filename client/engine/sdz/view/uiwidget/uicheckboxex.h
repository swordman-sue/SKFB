#ifndef UI_CHECKBOX_H
#define UI_CHECKBOX_H

#include "zqcommon/typedef.h"
#include "ui/UICheckBox.h"

class ImageData;

class UICheckBoxEx : public cocos2d::ui::CheckBox
{
	DECLARE_CLASS_GUI_INFO
public:
	~UICheckBoxEx();
	bool InitWithImage(const std::string& backgroud_image_file, const std::string& cross_image_file);
	bool InitWithImage(const ImageData& background_image_data, const ImageData& cross_image_data);
	void OnSelectEvent(cocos2d::Ref *pSender, cocos2d::ui::CheckBox::EventType eventType);

public:
	static UICheckBoxEx* create();
	static UICheckBoxEx* CreateWithImage(const std::string& backgroud_image_file, const std::string& cross_image_file);
	static UICheckBoxEx* CreateWithImage(const ImageData& background_image_data, const ImageData& cross_image_data);

	//���ñ���ͼƬ
	void SetBackGroundImage(const std::string &image_file);
	void SetBackGroundImage(const ImageData &image_data);
	//������Ч״̬�µı���ͼƬ
	void SetBackGroundDisableImage(const std::string &image_file);
	void SetBackGroundDisableImage(const ImageData &image_data);
	//����ѡ��״̬�µı���ͼƬ
	void SetBackGroundSelectImage(const std::string &image_file);
	void SetBackGroundSelectImage(const ImageData &image_data);
	//���ù�ѡͼƬ
	void SetCrossImage(const std::string &image_file);
	void SetCrossImage(const ImageData &image_data);
	//������Ч״̬�µĹ�ѡͼƬ
	void SetCrossDisableImage(const std::string &image_file);
	void SetCrossDisableImage(const ImageData &image_data);

	//����ѡ��״̬
	void SetSelectedState(bool selected);
	//��ȡѡ��״̬
	bool GetSelectedState();
	void SetEventListener(GameType::UINT32 select_fun_id, GameType::UINT32 unselect_fun_id);

	void SetGray(bool grey);
	virtual std::string getDescription() const { return "UICheckBoxEx"; }

	virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;

protected:
	virtual cocos2d::ui::Widget* createCloneInstance();

private:
	UICheckBoxEx();

private:
	GameType::UINT32 m_select_fun_id;
	GameType::UINT32 m_unselect_fun_id;
};

#endif
