#ifndef UI_LOADINGBAREX_H
#define UI_LOADINGBAREX_H

#include "zqcommon/typedef.h"
#include "ui/UILoadingBar.h"
class ImageData;

class UILoadingBarEx: public cocos2d::ui::LoadingBar
{
	DECLARE_CLASS_GUI_INFO
public:
	~UILoadingBarEx();

public:
	static UILoadingBarEx* create();
	static UILoadingBarEx* CreateWithImage(const std::string& image_file);
	static UILoadingBarEx* CreateWithImage(const ImageData& image_data);

	//����ͼƬ
	void SetImage(const std::string &image_file);
	void SetImage(const ImageData& image_data);
	//���ý���
	void SetPercent(int percent);
	//��ȡ����
	int GetPercent();
	//������ʾ�����ٽ���
	void SetShowMinPercent(GameType::INT32 min_show_progress){m_min_show_progress = m_min_show_progress;}
	//�����Ƿ��ǾŹ���ؼ�
	void SetScale9Enabled(bool enabled);
	//���þŹ�������
	void SetCapInsets(float x, float y, float w, float h);
	//���ý������ķ���
	void SetDirection(int loadingbar_dir);

	virtual std::string getDescription() const { return "UILoadingBarEx"; }
protected:
	virtual cocos2d::ui::Widget* createCloneInstance();

private:
		UILoadingBarEx();

private:
	GameType::INT32 m_min_show_progress;
};

#endif
