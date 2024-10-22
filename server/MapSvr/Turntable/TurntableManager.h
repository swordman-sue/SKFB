/**\file
ת�̹�����
\author Kevin
\date 2018/3/20
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include <deque>
class Player;

// ת������
struct TurntableData : public TurntableBaseData
{
	deque<TurntableLotteryRecord> mLotteryRecordList; // �齱��¼�б�
};

class CTurntableManager : public Singleton<CTurntableManager>
{
public:
	CTurntableManager();
	~CTurntableManager();

	void Init();

	void Update();

	// ��������
	void SendDataToDB();

	// �洢����
	void LoadDataFromDB(const char *pData, int size);

	// ������������
	void LoadDataRequest();

public:

	// ����ת������
	void OnNetTurntableData(Player *pPlayer);

	// ת�̳齱����
	void OnNetTurntableLottery(Player *pPlayer, WORD wLotteryType, BYTE byIsCostItem);


protected:
	// ����ת������
	void SendTurntableData(Player *pPlayer);

	// ��ӳ齱��¼
	void AddLotteryRecord(DWORD dwServerId, const TurntableLotteryRecord &tLotteryRecrod);

	// ���������н���
	void DoRankReward(DWORD dwServerId, WORD wDataId);

	// ȡת������
	TurntableData* GetTurntableData(DWORD dwServerId);

private:

	bool m_bIsUpdate;
	bool m_bLoadDataFromDB;
	DWORD m_dwLastProccessTimer;

	// ת�������б�[server_id, data_list]
	map<DWORD, TurntableData> m_TurntableDataList; 
};
