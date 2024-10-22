/* 
 *	遥控器类,用于管理可交互的遥控器支持控件
 *　create by liwei
 */

#ifndef TELE_CONTROLLER_H
#define TELE_CONTROLLER_H
#include "cocos2d.h"
#include "keypad_dispatcher/CCKeypadDispatcher.h"
//#include "zqcommon/typedef.h"

class TeleController : public cocos2d::CCNode
{
public:

	// 按按键分类遥控器
	enum TELE_CONTROLLER_KEY_TYPE
	{
		TELE_KEY_UNKNOWN = 0,		// 未知类型
		TELE_KEY_9,					// 9键遥控器(也被乐视电视所支持)
		TELE_KEY_39,				// 39键遥控器(有数字键能输入数字)
		TELE_KEY_SMART,				// 智能遥控器(有触摸板、语音输入、空鼠功能)
	};

	enum TeleControllerKeyType
	{
		TELE_CONTROLLER_KEY_ENTER = cocos2d::kTypeEnterClicked,
		TELE_CONTROLLER_KEY_LEFT = cocos2d::kTypeLeftArrowClicked,
		TELE_CONTROLLER_KEY_UP,
		TELE_CONTROLLER_KEY_RIGHT,
		TELE_CONTROLLER_KEY_DOWN,
	};

    // 创建遥控器管理器
    static TeleController * CreateTeleController();
    
    bool init();

	void Start();

	void Stop();

	void Pause();

	void Resume();

	void Destory();

	// 遍历所有可交互的遥控器支持控件
	void Visit();
	// 显示所有可交互的遥控器支持控件在场景中的位置
	void KeyCoordinates();
    
protected:
    virtual ~TeleController();

private:
	TeleController();
};

#endif
