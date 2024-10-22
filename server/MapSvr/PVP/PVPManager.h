/**\file
PVP������
\author Kevin
\date 2017/6/19
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include <deque>
class Player;


class CPVPManager : public Singleton<CPVPManager>
{
public:
	CPVPManager();
	~CPVPManager();

	void Init();
	void Release();

	void Update();

	// �ǳ�
	void OnLogout(Player *pPlayer);

public:
	// �������PVP����
	void OnNetPlayerPVPData(Player *pPlayer);

	// ����Ŀ��
	void OnNetSearchTarget(Player *pPlayer, DWORD dwSpiritSkillId);

	// ȡ��ս��
	void OnNetCancelSearchTarget(Player *pPlayer);

	// ׼�����
	void OnNetReadyFinish(Player *pPlayer);

	// ʹ�ü���
	void OnNetUseSkill(Player *pPlayer, DWORD dwObjId, DWORD dwSkillId, WORD wX, WORD wY);

	// Ѫ���仯
	void OnNetHPChange(Player *pPlayer, DWORD dwSrcObjId, DWORD dwTgtObjId, int nChangeHP, int nDamageType, DWORD dwPower, const char *pSign);

	// ����buff
	void OnNetCreateBuff(Player *pPlayer, DWORD dwBuffId, DWORD dwSrcObjId, DWORD dwTgtObjId, BYTE byIsFromSpriteBless);
	
	// ս�����(ֻ�д�����˲Żᷢ��) ʱ�䵽��ƽ���ɷ��������ж�
	void OnNetBattleResult(Player *pPlayer, BYTE byResult);

	// ��ȡ��ս����
	void OnNetTakeBattleReward(Player *pPlayer, BYTE byType, BYTE *pTimesList, WORD wListNum);

public:
	// ׼������ս��(���)
	bool ReadyToBattleWithPlayer(DWORD dwRoleId, DWORD dwTargetId, DWORD dwSpiritSkillId);

	// ׼������ս��(������)
	bool ReadyToBattleWithRobot(DWORD dwRoleId);

	// ȡս������ݵ�����
	BYTE GetBattlefieldActType(DWORD dwRoleId);

	// ȡս������ݵ�����
	BYTE GetBattlefieldActType(const PVPBattlefieldData *pBattleData, DWORD dwRoleId);

	// Ӣ���Ƿ�������
	bool IsHeroDead(const PVPBattlefieldData *pBattleData, DWORD dwRoleId, DWORD dwHeroInfoId);

	// �Ƿ����е�Ӣ�۶��ҵ�
	bool IsAllHeroDead(const PVPBattlefieldData *pBattleData, BYTE byActType);

	// ͨ��ս��ɸѡĿ��
	DWORD SelectTargetByFightingPower(DWORD dwFightingPower);

	// ɾ���ȴ���ս�Ľ�ɫ����
	void RemoveWaitBattleRoleData(DWORD dwRoleId);

	// ����Ӣ��HP
	int UpateHeroHP(PVPBattlefieldData *pBattleData, BYTE byActType, DWORD dwHeroInfoId, int nChangeHP);

	// ȡս��Ŀ��ID
	DWORD GetTargetId(const PVPBattlefieldData *pBattleData, DWORD dwRoleId);

	// ɾ��ս������
	void RemoveBattlefieldData(DWORD dwRoleId);

	// �Ƿ�����
	bool IsActivityOpen();

	// �Ƿ��к��
	bool IsHaveRedPoint(Player *pPlayer);

	// �������н���
	void DoRankAward();

	// �������гƺ�
	void DoRankTitle();

	// �޸Ļ���(GMʹ��)
	void ModifyScoreForGM(Player *pPlayer, DWORD dwScore, WORD wWeekWinTimes);

protected:
	// ���ҵȴ���ս�Ľ�ɫ����
	PVPWaitBattleRoleData* FindWaitBattleRoleData(DWORD dwRoleId);

	// ����ս������
	PVPBattlefieldData* FindBattlefieldData(DWORD dwRoleId);

	// ȡ��ɫս������
	PVPRoleBattleData* GetRoleBattleData(PVPBattlefieldData *pBattleData, DWORD dwRoleId);

	// ȡ��ɫս������
	PVPRoleBattleData* GetRoleBattleData(DWORD dwRoleId);

	// ȡ��ɫս������
	PVPRoleBattleData* GetRoleBattleData(PVPBattlefieldData *pBattleData, BYTE byActType);

	// ���ս������
	void AddBattlefieldData(const PVPBattlefieldData &tBattleData);

	// �����ʤ����
	DWORD CalculateWinScore(DWORD dwTargetFightingPower);

	// ����ʧ�ܻ���
	DWORD CalculateLoseScore(DWORD dwWinScore);

	// ȡƽ�ֻ���
	DWORD CalculateDrawScore(DWORD dwWinScore);

	// �������
	DWORD CalculateScore(BYTE byResult, DWORD dwTargetFightingPower);

	// �������PVP����
	void SendPlayerPVPData(Player *pPlayer);

	// ����ս����ʼ
	void DoBattleStart(Player *pPlayer, Player *pTarget);

	void DoBattleOverWithPlayer(Player *pSrc, Player *pTgt, DWORD dwWinnerId);

	// ����ս������(������)
	void DoBattleOverWithRobot(Player *pPlayer, DWORD dwTargetFightinPower, BYTE byResult);

	// ��ӻ���
	void AddScore(Player *pPlayer, DWORD dwScore, BYTE byWinner = false);

	// ȡ����
	WORD GetRank(DWORD dwRoleId);
private:

	// PVP�ȴ���ս�Ľ�ɫ�����б�
	deque<PVPWaitBattleRoleData> m_WaitBattleRoleDataList;

	// ս���б�
	vector<PVPBattlefieldData> m_BattlefieldList;

	// �ȼ���սʱ������
	DWORD m_dwWaitBattleTimeMax;

	// һ��ս��������ʱ��
	DWORD m_dwBattleLimitTime;

	// ����������ʱ��
	CDateTimers m_RankRewardTimer;

	// �����ƺż�ʱ��
	CDateTimers m_RankTitleTimer;
};