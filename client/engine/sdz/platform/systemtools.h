#ifndef SDK_INFO_TOOLS_H_
#define SDK_INFO_TOOLS_H_

#include "base/ccUTF8.h"
#include "zqcommon/typedef.h"

#include <string>

//Sdk��Ϣ��ȡ
class SystemTools
{
public:
	//��������״̬ (0 �ޣ�1 WIFI��2 2G���磬3 3G���磬4 4G���磬5 ����)
	enum NETWORKSTATE
	{
		NETWORK_NONE = 0,		//����������
		NETWORK_WIFI,			//wifi����
		NETWORK_MOBILE_2G,		//�ƶ�����2G
		NETWORK_MOBILE_3G,		//�ƶ�����3G
		NETWORK_MOBILE_4G,		//�ƶ�����4G
		NETWORK_UNKNOW			//δ֪����
	};

	// ��ȡϵͳ����
	static const std::string GetSystemType()
	{
#if defined(WP8)
		return "wp8";
#elif defined(_WIN32)
		return "win32";
#elif defined(__APPLE__)
		return "apple";
#elif defined(__ANDROID__)
		return "android";
#endif
		return "";
	}

	// ����״̬�仯�ص�
	typedef void (*NETWORK_STATE_CHANGED_CALLBACK) (void*);
	static ZQ::UINT32 network_change_notify_fun_id;
	static NETWORK_STATE_CHANGED_CALLBACK network_change_notify_fun;
	static void* network_change_notify_fun_data;

    static ZQ::UINT32 device_back_pressed_fun_id;

    // ��ȡ�ڴ��С
	static ZQ::UINT32 GetMemorySize();

    // ��ȡӦ����
	static std::string GetProductName();
	// ��ȡӦ��·��
	static const std::string GetAppPath();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // ��ȡIOSϵͳ�汾
    static float GetIOSVersion();
#endif
    
	// ��ȡ����·��
	static const std::string GetAppPackPath();
	// ��ȡӦ�����ƣ���Ϸ���ƣ�
	static std::string GetAppName();
	// �Ƿ�����λ����
	static bool IsOpenLocationService();
	// ��ȡ����״̬   0.����������  1.wifi���� 2.�ƶ���·���� 3.δ֪����
	static NETWORKSTATE GetNetWorkState();
	// �����Ƿ���Ч
	static bool IsNetworkAvailable();
	// �������״̬
	static void CheckNetWork(const char* content, const char* positive_text, const char* negative_text);
	// ��ȡ�豸ID
	static std::string GetDeviceId();
	// ��ȡIMSI��
	static std::string GetImsi();
	// ��ȡ������
	static std::string GetPackageName();
	// ��ȡ�汾��(1.0.0)
	static std::string GetAppVersion();
	// ��ȡ�汾��(100)
	static std::string GetAppVersionCode();
	// ��ȡ����
	static std::string GetDeviceModel();
	// ��ȡMAC
	static std::string GetMacAddress();
	// ��ȡIP
	static std::string GetIPAddress();

	// �Ƿ���ƻ����İ�
	static bool IsApptorePackage();

	// �Ƿ�֧���԰�װ
	static bool IsSupportAppAutoInstall();
	// ��ȡifa(ios6+)
	static std::string GetIdentifierForAdvertisers();
	// ��ȡifv
	static std::string GetIdentifierForVendor();
	// ��ȡ��չ�ļ�·��(googleplay)
	static std::string GetApkExpansionPath();
	// �ж�ĳ�������İ�װ���Ƿ�װ
	static bool IsAppInstalled(std::string packageName);
	// ��ȡ��־Ŀ¼·��
	static std::string GetLogPath();

	// ��������״̬�仯�ص�
	static void SetNetworkStateChangeNotifyFunc(ZQ::UINT32 fun_id){ network_change_notify_fun_id = fun_id; }
	static void SetNetworkStateChangeNotifyFuncEx(NETWORK_STATE_CHANGED_CALLBACK func, void* func_data)
	{ 
		network_change_notify_fun = func; 
		network_change_notify_fun_data = func_data;
	}
	// ����״̬�仯֪ͨ
	static void NetworkStateChangeNotify();

	// ��ȡ���Ҵ���
	static std::string GetCountryCode();
	// ��ȡ���Ҵ���
	static std::string GetCurrencyCode();

	// ��ά���ı��ӻ�����ת��Ϊ��չ��
	static std::string ConvertUygurToEx(const std::string& orign);

	// ��ά���ı��ӻ�����ת��Ϊ��չ����ͨ��Fribidi������������
	static std::string ConvertUygur(const std::string& orign);
};

#endif
