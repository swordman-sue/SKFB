#pragma once
#include "Character.h"
#include "../Common/DataDefine.h"
#include "../ToolKit/Timer.h"
#include "../Common/DBCDefine.h"
#include "../Common/MsgDefine.h"
#include "common_msg.pb.h"
#include "header.pb.h"

class AccountInfo;
class ServerSession;
class NetMsgBody;
class CItemManager;
class CTreasureManager;
class CHeroManager;
class CEquipmentManager;
class CFubenManager;
class CSpriteManager;
class CShopManager;
class EmailManager;
class CFriendManager;
class CRecyclingManager;
class CDuobaoManager;
class CAchievementManager;
class CDailyMissionManager;
class CVipManager;
class CLogicManager;
class CPropertyManager;
class CSevenDayManager;
class CZhanBuManager;
class CTitleManager;
class CActivityManager;
class CRedPointManager;
class CRewardCenterManager;
class CRechargeManager;
class CEndlessLandManager;
class CGemManager;
class CWorldLevelTaskMananger;
class CHangUpDropMgr;


//�����
class Player : public Character
{
public:
	Player();
	~Player();

	BOOL				Init();
	VOID				Release();
	VOID				Update(DWORD dwTick);
	void			    UpdateOnlineTime( DWORD dwTick );
	VOID				Create( ServerSession* pSession, char* pszAccID, DWORD dwCharID, DWORD dwGuid, AccountInfo* pAccountInfo );

	//�ʺ����
	DWORD				GetGuid() const { return m_dwGuid; }
	VOID				SetGuid(DWORD dwGuid) { m_dwGuid = dwGuid; }
	WORD				GetServerId();
	WORD				GetInitServerId();
	DWORD				GetCharID() const { return m_tRoleFullInfo.tRoleBaseInfo.dwRoleId; }
	DWORD				GetRoleId() { return m_tRoleFullInfo.tRoleBaseInfo.dwRoleId; };
	AccountInfo*		GetAccountInfo() { return m_pAccountInfo; }
	const char*			GetAccID() { return m_tRoleFullInfo.tRoleBaseInfo.szAccount; }
	VOID				SetRoleName(const char* pszRoleName);
	const char*			GetUTF8RoleName() { return m_szUTF8RoleName; };
	const char*			GetRoleName() { return m_tRoleFullInfo.tRoleBaseInfo.szRoleName; };
	virtual USHORT		GetLevel() const { return m_tRoleFullInfo.tRoleBaseInfo.wLevel; }
	VOID				SetLevel(WORD wLevel);
	WORD				GetDailyInitLevel() { return m_tRoleFullInfo.tRoleBaseInfo.wDailyInitLevel; };
	DWORD				GetLoginTimes()	{ return m_tRoleFullInfo.tRoleBaseInfo.dwLoginTimes; };
	VOID				SetLastHeartBeatTime(DWORD dwLastHeartBeatTime) { m_dwLastHeartBeatTime = dwLastHeartBeatTime; };
	DWORD				GetLastHeartBeatTime() { return m_dwLastHeartBeatTime; };
	void				SetHeartBeatFastTimes(WORD wHeartBeatFastTimes) { m_wHeartBeatFastTimes = wHeartBeatFastTimes; };
	WORD				GetHeartBeatFastTimes() { return m_wHeartBeatFastTimes; };
	void				SetNetworkSeriesId(DWORD dwNetworkSeriesId) { m_dwNetworkSeriesId = dwNetworkSeriesId; };
	DWORD				GetNetworkSeriesId() { return m_dwNetworkSeriesId; };
	const MapPos&		GetMapPos(){ return m_tMapPos; };
	VOID				SetMapPos(int nX, int nY);
	void				AddRechargeAmount(DWORD dwRechargeAmount);
	DWORD				GetTeamId() { return m_dwTeamId; };
	void				SetTeamId(DWORD dwTeamId) { m_dwTeamId = dwTeamId; };
	WORD				GetTeamDungeonId() { return m_wTeamDungeonId; };
	void				SetTeamDungeonId(WORD wTeamDungeonId) { m_wTeamDungeonId = wTeamDungeonId; };
	WORD				GetRoleCreateDays();
	DWORD				GetRoleCreateTime() { return m_tRoleFullInfo.tRoleBaseInfo.dwRoleCreateTime; };
	WORD				GetRoleLoginDays() { return m_tRoleFullInfo.tRoleBaseInfo.wLoginDays; };
	DWORD				GetLastChatTime() { return m_dwLastChatTime; };
	void				SetLastChatTime(DWORD dwLastChatTime) { m_dwLastChatTime = dwLastChatTime; };
	DWORD				GetLastLogoutTime() { return m_tRoleFullInfo.tRoleBaseInfo.dwLastLogoutTime; };
	DWORD				GetLastLoginTime() { return m_tRoleFullInfo.tRoleBaseInfo.dwLastLoginTime; };
	DWORD				GetChannelType();
	bool				IsHaveSpriteTalentSkillReward() { return m_bIsHaveSpriteTalentSkillReward; };
	void				SetIsHaveSpriteTalentSkillReward(bool bIsHave) { m_bIsHaveSpriteTalentSkillReward = bIsHave; };
	WORD				GetVipGMLevel();
	
	
	//�������
	VOID				SetRoleFullInfo(RoleFullInfo& tCharFullInfo);
	RoleFullInfo&		GetRoleFullInfo() { return m_tRoleFullInfo; }

	// ����������
	void				FillCommonData(PlayerCommonData *pBaseData);

	// ����������
	void				FillBaseData(PlayerBaseData *pBaseData);

	// �������Ӣ������
	void				FillToBattleHeroData(vector<ToBattleHeroData> &vHeroList);

	void				ModifyPropertyList(const vector<Property> &vProList, BOOL bNotice = TRUE, WORD wFromId = 0);
	void				ModifyProperty(WORD wPropertyType, INT32 nValue = 1, BOOL bNotice = TRUE, WORD wFromId = 0, BOOL bSaveLog = TRUE);
	void				SetProperty(WORD wPropertyType, INT32 nValue, BOOL bNotice = TRUE);
	INT32				GetProperty(WORD wPropertyType);
	void				NotifyProperty(WORD wPropertyType, INT32 nValue);
	DWORD				GetFightingPower(bool bIsUpdate = false);

	bool				AddObject(DWORD dwObjectId, DWORD dwOjbectNum = 1, BOOL bNotice = TRUE, WORD wFromId = 0);
	bool				AddObjectEx(BYTE byType, DWORD dwObjectId, DWORD dwOjbectNum = 1, BOOL bNotice = TRUE, WORD wFromId = 0);
	bool				AddObjectData(const Item *pItem, BOOL bNotice = TRUE, WORD wFromId = 0);
	bool				AddObjectList(const vector<Item> &vItemList, BOOL bNotice = TRUE, WORD wFromId = 0);

	// �洢������־
	void				SaveCurrencyLog(WORD wPropertyType, INT32 nValue, WORD wFromId);




	// ��Ӿ���
	void				AddExp(DWORD dwExp);

	//�¼�����	
	VOID				OnLoad(RoleFullInfo& sRoleInfo);
	VOID				OnSave(SaveDBType enSaveDBType);
	VOID				OnLogin();
	VOID				OnLogout();
	VOID				OnLevelUp();

	// �µ�һ��(ÿ��0��)
	VOID				OnNewDay();

	// �µ�һ��(ÿ��һ0��)
	VOID				OnNewWeek();

	// �µ�һ��(ÿ����1��0��)
	void				OnNewMonth();

	// ������ʱ����
	VOID				DoTempData(bool isSave = true);

	// �Ƿ��½���ɫ
	BOOL				IsNewRole();
	VOID				OnNewRole();
	BOOL				IsTodayCreateRole();

	//��Ϣ���
	BOOL				SendPacket(NetMsgBody * pMsg);
	VOID				SendRoleBaseInfo();
	VOID				SendErrorMsg(WORD wErrorCode, WORD wFromMsgId = 0);

 	ERROR_CODE			HandleMissionScript(const char* pszScriptName, const char* pszScriptEventName, INT nEventPara = 0);

	//����ű��ýӿ�
	BOOL				IsAcceptMission(DWORD dwMissionID);
	VOID				TriggerMission(DWORD dwMissionID);

	// ��ɫ��Ϣ����
	void				RoleInfoUpdate(bool isSave = true);

////// ģ���߼��ӿ�
	// ���³ɾ�
	void				OnUpdateAchievement(WORD wAchievementId, int nValue = 1, bool bAdd = true);

	// ȡVIP��Ȩֵ
	int					GetVipValue(BYTE byType);

	// �����ճ�����
	void				OnUpdateDailyMission(WORD wMissionType, int nValue = 1, bool bAdd = true);

	// ȡ��Ʒ����
	DWORD				GetItemNum(DWORD dwItemId);

	// ɾ����Ʒ
	void				RemoveItem(DWORD dwItemId, DWORD dwItemNum = 1, WORD wFromId = 0);

	// ɾ����Ʒ
	void				RemoveItem(const Item &tItem, WORD wFromId);

	// ����ս��
	void				UpdateFightingPower();

	// ȡ���Ἴ��
	const SkillData*	GetGuildSkill(WORD wSkillId);

	// ��ӹ��Ἴ��
	void				AddGuildSkill(WORD wSkillId, WORD wSkillLevel);

	// ��乤�Ἴ���б�
	void				FillGuildSkillList(vector<SkillData> &vSkillList);

	// ��ѯĿ����ϸ��Ϣ�ص�
	void				SelectTargetDetailsCallBack(BYTE byRequestType, const RoleFullInfo *pTargetFullInfo);

	// ����������
	bool				CheckMoneyCost(const vector<CostData> &vCostList, WORD wNum = 1);

	// �����Ʒ����
	bool				CheckItemCost(const vector<Item> &vCostItemList, WORD wNum = 1);

	// �����������
	void				DoMoneyCost(const vector<CostData> &vCostList, WORD wNum = 1, WORD wFromId = 0);

	// ȡ��ǰʹ�õĳƺ�Id
	int					GetCurUseTitleId();

	// ���һ���Ա�־
	BOOL CheckOnceFlag(int nFlagIdx);

	// ����һ���Ա�־
	VOID SetOnceFlag(int nFlagIdx);

	// ���һ���Ա�־
	VOID ClearOnceFlag(int nFlagIdx);

	// ����7��Ŀ������
	void OnUpdateSevenDayTarget(WORD wLogicType);

	// ����7��Ŀ������
	void OnUpdateSevenDayTarget(WORD wLogicType, DWORD dwX, bool bAdd);

	// ���ջ�ԾĿ���¼�
	void OnFestivalActiveTarget(WORD wActivityType, WORD wLogicType, DWORD dwX, bool bAdd);

	// �洢��Ʒ��ʧ��¼
	void SaveItemLoseRecord(DWORD dwItemId, DWORD dwItemNum = 1);

	// ��ס�ʺ�
	void LockAccount();

	// �������������¼�
	void OnUpdateWorldTaskEvent(WORD wTaskType, int nContext = 1);

	// ��Ⲣ�������ʥ��������
	void CheckAndResetChristmasData();


public:

	EmailManager*			GetEmailManager()		{ return m_pEmailManager; }
	CItemManager*			GetItemManager()		{ return m_pItemMgr; };
	CHeroManager*			GetHeroManager()		{ return m_pHeroMgr; };
	CEquipmentManager*		GetEquipmentManager()	{ return m_pEquipmentMgr; };
	CTreasureManager*		GetTreasureManager()	{ return m_pTreasureMgr; };
	CFubenManager*			GetFubenManager()		{ return m_pFubenMgr; };
	CSpriteManager*			GetSpiritManager()		{ return m_pSpiritMgr; };
	CShopManager*			GetShopManager()		{ return m_pShopMgr; };
	CFriendManager*			GetFriendManager()		{ return m_pFriendMgr; };
	CRecyclingManager*		GetRecyclingManager()	{ return m_pRecyclingMgr; };
	CDuobaoManager*			GetDuobaoManager()		{ return m_pDuobaoMgr; };
	CAchievementManager*	GetAchievementManager() { return m_pAchievementMgr; };
	CDailyMissionManager*	GetDailyMissionManager(){ return m_pDailyMissionMgr; };
	CVipManager*			GetVipManager()			{ return m_pVipMgr; };
	CLogicManager*			GetLogicManager()		{ return m_pLogicMgr; };
	CPropertyManager*		GetPropertyManager()	{ return m_pPropertyManager; };
	CSevenDayManager*		GetSevenDayManager()	{ return m_pSevenDayManager; };
	CZhanBuManager*			GetZhanBuManager()		{ return m_pZhanBuManager; };
	CTitleManager*			GetTitleManager()		{ return m_pTitleManager; };
	CActivityManager*		GetActivityManager()	{ return m_pActivityManager; };
	CRedPointManager*		GetRedPointManager()	{ return m_pRedPointManager; };
	CRewardCenterManager*	GetRewardCenterManager(){ return m_pRewardManager; };
	CRechargeManager*		GetRechargeManager()	{ return m_pRechargeManager; };
	CEndlessLandManager*	GetEndlessLandManager() { return m_pEndlessLandManager; };
	CGemManager*			GetGemManager()			{ return m_pGemManager; };
	CWorldLevelTaskMananger*GetWorldLevelTaskManager(){ return m_pWorldLevelTaskMgr; };
	CHangUpDropMgr*			GetHangUpDropManager()	{ return m_pHangUpDropMgr; };
	

	// ���Խӿ�
public:
	void PrintPVPData(bool isLogin);


private:
	BOOL				IsNewDay();
	BOOL				IsNewWeek();
	BOOL				IsNewMonth();

private:
	char				m_szAccID[MAX_ACCOUNT_LEN];				//����˺�ID
	DWORD				m_dwGuid;								//��ұ�ʶID(��ʶGateSvr�϶�Ӧ�����)
	AccountInfo*		m_pAccountInfo;							//����˺���Ϣ
	RoleFullInfo		m_tRoleFullInfo;						//�����������Ϸ��Ϣ
	Timers				m_cSaveDBTimers;						//DB�洢��ʱ��

	EmailManager*			m_pEmailManager;                     	//�ʼ�������
	CItemManager*			m_pItemMgr;								//��Ʒ������
	CHeroManager*			m_pHeroMgr;								//Ӣ�۹�����
	CTreasureManager*		m_pTreasureMgr;							//���������
	CEquipmentManager*		m_pEquipmentMgr;						//װ��������
	CFubenManager*			m_pFubenMgr;							//����������
	CSpriteManager*			m_pSpiritMgr;							//���������
	CShopManager*			m_pShopMgr;								//�̵������
	CFriendManager*			m_pFriendMgr;							//���ѹ�����
	CRecyclingManager*		m_pRecyclingMgr;						//���չ�����
	CDuobaoManager*			m_pDuobaoMgr;							//�ᱦ������
	CAchievementManager*	m_pAchievementMgr;						//�ɾ͹�����
	CDailyMissionManager*	m_pDailyMissionMgr;						//���������
	CVipManager*			m_pVipMgr;								//VIP������
	CLogicManager*			m_pLogicMgr;							//С�����߼�������
	CPropertyManager*		m_pPropertyManager;						//���Թ�����
	CSevenDayManager*		m_pSevenDayManager;						//7�������
	CZhanBuManager*			m_pZhanBuManager;						//ռ��������
	CTitleManager*			m_pTitleManager;						//�ƺŹ�����
	CActivityManager*		m_pActivityManager;						//�������
	CRedPointManager*		m_pRedPointManager;						//��������
	CRewardCenterManager*	m_pRewardManager;						//����������
	CRechargeManager*		m_pRechargeManager;						//��ֵ������
	CEndlessLandManager*	m_pEndlessLandManager;					//�޾�֮�ع�����
	CGemManager*			m_pGemManager;							//��ʯ������
	CWorldLevelTaskMananger*m_pWorldLevelTaskMgr;					//����ȼ����������
	CHangUpDropMgr*			m_pHangUpDropMgr;						//�һ����������


	//Timers				m_StaTimes;								//�����ָ���ʱ��
	char				m_szUTF8RoleName[MAX_ROLE_LEN+1];			//��ɫ��
	DWORD				m_dwLastHeartBeatTime;						//���һ������ʱ��
	WORD				m_wHeartBeatFastTimes;						//�����������
	MapPos				m_tMapPos;									//��ͼ���� 
	DWORD				m_dwNetworkSeriesId;						//����ϵ��ID
	DWORD				m_dwTeamId;									//����ID
	WORD				m_wTeamDungeonId;							//��Ӹ���ID
	DWORD				m_dwLastChatTime;							//���һ������ʱ��
	bool				m_bIsHaveSpriteTalentSkillReward;			//�Ƿ��о����츳���ܽ���

	int	m_arProList[MAX_BATTLE_HERO_NUM][HERO_PRO_MAX];			// ���������б�

	ServerSession*		m_pSession;								//���ط������Ự����
};
