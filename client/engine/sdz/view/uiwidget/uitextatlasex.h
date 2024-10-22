#ifndef UI_LABELATLASEX_H
#define UI_LABELATLASEX_H

#include "2d/CCSprite.h"
#include "ui/UIWidget.h"
#include "ui/UITextAtlas.h"
#include "view/uiwidget/uiimageviewex.h"

class UITextAtlasEx : public cocos2d::ui::TextAtlas
{
	DECLARE_CLASS_GUI_INFO
public:
	UITextAtlasEx();
	~UITextAtlasEx();

public:
	static UITextAtlasEx* create();
	static UITextAtlasEx* CreateWithImage(const char* number_value, const char* file_name, int texType = 0);
	static UITextAtlasEx* CreateWithImage(const char* number_value, const char* file_name, int item_width, int item_height, int texType = 0);
	
	void InitWithImage(const char* number_value, const char* file_name, int item_width, int item_height, int texType = 0);

	void setProperty(const std::string& stringValue, const std::string& charMapFile, int itemWidth, int itemHeight, const std::string& startCharMap);
	void SetProperty(const std::string& str_value, const std::string& image_file, int item_width, int item_height, const std::string& start_char);
	void SetString(const std::string& value);	// 修复Lua无法正常调用SetString方法的问题 edit by JiaMiao 2016-1-4
	const char* GetString();

	virtual void setOpacity(GLubyte opacity);
	virtual std::string getDescription() const { return "UILabelAtlasEx"; }
protected:
	virtual cocos2d::ui::Widget* createCloneInstance();
	// 修复UILabelAtlasEx克隆出错的问题 add by JiaMiao 2016-1-4
	virtual void copySpecialProperties(Widget* model) override;
	virtual void copyClonedWidgetChildren(Widget* model);

private:
	Widget::TextureResType m_tex_type;
	std::string m_number_value;
	int m_item_width;
	int m_item_height;
	// 新版本的imageview不支持，修改为sprite add by chuxi 2016-3-25
	//std::vector<UIImageViewEx*> m_num_vec;
	std::vector<cocos2d::Sprite*> m_num_vec;
	cocos2d::Rect m_rect;
	std::string m_num_file;
	std::string m_start_charMap;

};

#endif
