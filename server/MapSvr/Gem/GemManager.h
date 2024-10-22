/**\file
��Ʒ������
\author Kevin
\date 2016/7/5
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CGemManager
{
public:
	CGemManager();
	~CGemManager();

	// ��ʼ��
	void Init(Player *pPlayer);

	// �½���ɫ
	void OnNewRole();

	// ��ʼ��Ĭ�Ͽ��ŵĿ�λ
	void InitDefaultOpenHole();

	// ����Ƿ��ѳ�ʼĬ�Ͽ��ŵĿ�λ
	bool IsInitDefaultOpenHole();

public:
	// ����ʯ�б�
	void OnNetGemList();

	// �ϳ�
	void OnNetCompose(DWORD dwGemId, WORD wComposeNum);
	
	// ��Ƕ
	void OnNetInlay(WORD wFormationSlot, GemInlay *pInlayList, WORD wInlayNum);

	// ж��
	void OnNetUnload(WORD wFormationSlot, WORD wHole);

	// ��λ����
	void OnNetHoleOpen(WORD wFormationSlot, WORD wHole);

	// �ֽ�
	void OnNetDecompose(DWORD dwGemId, DWORD dwGemNum);
public:
	// ��ӱ�ʯ
	bool AddGem(DWORD dwGemId, DWORD dwGemNum = 1, bool bNotify = true, WORD wFromId = 0);

	// ɾ����ʯ
	DWORD RemoveGem(DWORD dwGemId, DWORD dwGemNum = 1, bool bNotify = true, WORD wFromId = 0);

	// ȡ��ʯ
	Gem* GetGem(DWORD dwGemId);

	// ȡ��ʯ����
	DWORD GetGemNum(DWORD dwGemId);

	// ����λ�Ƿ��ѿ���
	bool IsHoleOpen(WORD wFormationSlot, WORD wHole);

	// ȡ��ǰ���ŵĿ�λ(����)
	WORD GetOpenHole(WORD wFormationSlot);

	// ������λ
	void OpenHole(WORD wFormationSlot, WORD wHole);

	// ȡ��λ��ͬ���ͱ�ʯ����
	WORD GetFormationSlotSimilarGemNum(WORD wFormationSlot, WORD wGemTeyp, DWORD dwExceptGemId);

protected:
	
	// ���ͱ�ʯ�б�
	void SendGemDataList();

private:
	Player *m_pPlayer;
	PlayerGemInfo *m_pPlayerGemInfo;
};