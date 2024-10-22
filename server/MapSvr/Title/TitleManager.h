/**\file
�ƺŹ�����
\author Kevin
\date 2017/01/14
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CTitleManager
{
public:
	CTitleManager();
	~CTitleManager();

	void Init(Player *pPlayer);

public:
	// �����ɫ�ƺ���Ϣ
	void OnNetRoleTitleInfo();

	// ʹ�óƺ�
	void OnNetUseTitle(int nTitleId);

public:

	// ��ӳƺ�
	static void AddTitleEx(DWORD dwServerId, DWORD dwRoleId, int nTitleId);

	// ���ͽ�ɫ�ƺ���Ϣ
	void SendRoleTitleInfo();

	// ��ӳƺ�
	void AddTitle(int nTitleId, DWORD dwAddTime, bool bNotice = true);

	// �Ƴ��ƺ�
	void RemoveTitle(int nTitleId);

	// ȡ�ƺ�
	const TitleData* GetTitle(int nTitleId);

	// ɾ�����ڳƺ�

	void DeleteTimeOutTitle();

	// ȡ��ǰʹ�õĳƺ�ID
	DWORD GetCurUseTitleId() { return m_pTitleData->nCurUseTitleId; };

	// ȡ��ǰʹ�õ��ǳ�ID
	DWORD GetCurUseNicknameId() { return m_pTitleData->nCurUseNicknameId; };
private:
	Player *m_pPlayer;
	PlayerTitleInfo *m_pTitleData;
};