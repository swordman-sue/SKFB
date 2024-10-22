
/**\file
�Ѿ�������
\author Kevin
\date 2016/9/7
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "RebelDataDefine.h"

class Player;

typedef std::map<DWORD, RebelData> REBEL_MAP;
typedef std::map<DWORD, vector<DWORD>> REBEL_SHARE_MAP;
typedef std::map<DWORD, DWORD> REBEL_DISCOVER_MAP;

class CRebelManager : public Singleton<CRebelManager>
{
public:
	CRebelManager();
	~CRebelManager();

	void Init();

	void Update();

public:

	// �����Ѿ����� 
	void OnRebelDataReq(Player *pPlayer);

	// ��ȡ�Ѿ���ѫ����
	void OnRebelAwardReq(Player *pPlayer, WORD *pAwardList, BYTE byAwardNum);

	// ����ѫ������¼
	void OnNetAwardRecordList(Player *pPlayer);

	// �����Ѿ�
	void OnNetRebelShare(Player *pPlayer);

	// ��ʼս��
	bool OnBattleStart(Player *pPlayer, DWORD dwRebelId, WORD wAttackType);

	// ս������
	bool OnBattleOver(Player *pPlayer, DWORD dwRebelId, WORD wAttackType, MonsterData *pMonsterList, BYTE byMonsterNum);

public:

	// �����Ѿ�
	DWORD CreateRebel(Player *pPlayer);

	// ɾ���Ѿ�����
	void RemoveRebelData(DWORD dwRebelId);

	// ����Ѿ�����
	void AddRebelData(const RebelData &tRebelData);
	
	// ��ӷ�������
	void AddShareData(DWORD dwShareTargetId, DWORD dwRebelId);

	// ɾ����������
	void RemoveShareData(DWORD dwShareTargetId, DWORD dwRebelId);

	// ȡ�Լ����ֵ��Ѿ�ID
	DWORD GetSelfRebelId(DWORD dwRoleId);

	// �����Ѿ�����
	void SendRebelData(Player *pPlayer);

	// �Ƿ��к��
	bool IsHaveRedPoint(Player *pPlayer);
protected:

	// ȡ�Ѿ�����
	RebelData* GetRebelData(DWORD dwRebelId);

	// ȡ���˷�����Լ����Ѿ��б�
	vector<DWORD>* GetShareRebelList(DWORD dwRoleId);

	// ȡ�Ѿ��ؿ������б�(�ڲ�ʹ�õĽӿ�)
	//bool GetRebelMonsterList(WORD wRebelLevel, vector<MonsterData>* pMonsterList);

	// �Ƿ����ļ���
	bool IsCostHalveActivity();

	// �Ƿ��������
	bool IsDoubleAwardActivity();

	// �����Ѿ��˺�
	void UpdateRebelDamage(DWORD dwRebelId, MonsterData *pMonsterList, BYTE byMonsterNum);

private:
	// �Ѿ��б�
	REBEL_MAP m_RebelList;

	// ����յ����Ѿ������б�
	REBEL_SHARE_MAP m_RebelShareList;

	// ��ҷ��ֵ��Ѿ��б�
	REBEL_DISCOVER_MAP m_RebelDiscoverList;

	// �����Ѿ���������
	int m_nRebelShareMax;

	// ϵ��ID
	DWORD m_dwSeriesId;

	UINT64 m_lProcessTime;
};