#pragma once

#include "../Common/Const.h"
#include "../ToolKit/ThreadBase.h"
#include "../ToolKit/LogFile.h"
#include <hash_map>
#include <map>
#include <deque>
#include "PlayerManager.h"
#include "KeyWodFilter/KeyWodFilter.h"
#include "ToolKit/IniFile.h"

class CGameLogic : public Singleton<CGameLogic>
{
public:
	CGameLogic();
	~CGameLogic();

	void Init(IniFile *pInifile);

	void ReloadInitFile(IniFile *pInifile);

	void Update();

	void OnNewDay();

	// ����ͳ��
public:
	// ȡ����AppId
	const char* GetReyunAppId() { return m_szReyunAppId; };

	// ȡ����ͳ�ƿ���
	bool IsOpenReyunStatistics() { return m_bIsOpenReyunStatistics; };

	// ����ͳ��
	void OnReyunStatistics(WORD wStatisticsType, DWORD dwChannelType, const char *pszAccount, WORD wServerId);

	// ����ͳ��
	void OnReyunStatistics(const char *pszPath, const char *pszContext);

	// ��ֵͳ��
	void OnRechargeStatistics(const RechargeCallbackData &callbackData,Player *pPlayer);

	// �ؿ���ʼͳ��
	void OnStageStartStatistics(Player *pPlayer, int nX, int nY);

	// �ؿ�����ͳ��
	void OnStageOverStatistics(Player *pPlayer, int nX, int nY, bool bIsPass);

	// ���ҽ���ͳ��
	void OnCurrencyStatistics(WORD wCurrencyType, int nCurrencyValue, const char *pszAccount, DWORD dwChannelType, WORD wServerId);

	// ��¼ͳ��
	void OnLoginStatistics(Player *pPlayer);

	// �ʺ�ע��
	void OnRegisterStatistics(DWORD dwChannelType, const char *pszAccount, WORD wServerId);

	// ��Ʒ����ͳ��
	void OnGoodsBuyStatistics(Player *pPlayer, WORD wShopType, WORD wGoodsId, WORD wBuyNum);


	// ��Ϸͳ��
public:
	// ������ɫͳ��
	void OnGameStatistics_CreateRole(DWORD dwRoleId, WORD wServerId);
 
	// ��¼ͳ��
	void OnGameStatistics_Login(Player *pPlayer, bool bIsNewDay);

	// ��ֵͳ��
	void OnGameStatistics_Recharge(Player *pPlayer, int nAmount, bool bIsTodayFirstPayment, bool bIsFristPayment);

	// �µ�һ��
	void OnGameStatistics_NewDay();

	// ��������ͳ��
	void OnGameStatistics_Online(WORD wServerId, WORD wOnlineNum);

	// MyCardͳ�����
public:
	// ��ֵ�ɹ��Ϸ�
	void OnMyCardRechargeSuccessReport(Player *pPlayer, int nAmount, const char *pszOrderNo);


	// �ͻ�����Ϣ���
public:
	// ��ӿͻ�����Ϣ
	void AddClientInfo(DWORD dwChannelType, const char *pszAccount, ClientInfo &tClientInfo);

	// ȡ�ͻ�����Ϣ
	const ClientInfo* GetClientInfo(DWORD dwChannelType, const char *pszAccount);

	// �����Ӫ��ļ��¼����
	void AddCampRecruitRecord(DWORD dwServerId,WORD wCamp, const CampRecruitRecord &tRecord);

	// ȡ��Ӫ��ļ��¼�б�
	const deque<CampRecruitRecord>* GetCampRecruitRecordList(DWORD dwServerId, WORD wCamp);

	// ����ͳ���������
private:
	char m_szReyunAppId[64];		// ����appid
	bool m_bIsOpenReyunStatistics;	// �Ƿ�������ͳ��

	map<string, ClientInfo> m_ClientInfoList;		// �ͻ�����Ϣ�б� channelid + account, data	

	// ��Ӫ��ļ��¼�б�
	map<DWORD, deque<CampRecruitRecord>> m_CampRecruitRecordList;
};


