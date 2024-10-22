/* 
 *	ң������,���ڹ���ɽ�����ң����֧�ֿؼ�
 *��create by liwei
 */

#ifndef TELE_CONTROLLER_H
#define TELE_CONTROLLER_H
#include "cocos2d.h"
#include "keypad_dispatcher/CCKeypadDispatcher.h"
//#include "zqcommon/typedef.h"

class TeleController : public cocos2d::CCNode
{
public:

	// ����������ң����
	enum TELE_CONTROLLER_KEY_TYPE
	{
		TELE_KEY_UNKNOWN = 0,		// δ֪����
		TELE_KEY_9,					// 9��ң����(Ҳ�����ӵ�����֧��)
		TELE_KEY_39,				// 39��ң����(�����ּ�����������)
		TELE_KEY_SMART,				// ����ң����(�д����塢�������롢������)
	};

	enum TeleControllerKeyType
	{
		TELE_CONTROLLER_KEY_ENTER = cocos2d::kTypeEnterClicked,
		TELE_CONTROLLER_KEY_LEFT = cocos2d::kTypeLeftArrowClicked,
		TELE_CONTROLLER_KEY_UP,
		TELE_CONTROLLER_KEY_RIGHT,
		TELE_CONTROLLER_KEY_DOWN,
	};

    // ����ң����������
    static TeleController * CreateTeleController();
    
    bool init();

	void Start();

	void Stop();

	void Pause();

	void Resume();

	void Destory();

	// �������пɽ�����ң����֧�ֿؼ�
	void Visit();
	// ��ʾ���пɽ�����ң����֧�ֿؼ��ڳ����е�λ��
	void KeyCoordinates();
    
protected:
    virtual ~TeleController();

private:
	TeleController();
};

#endif
