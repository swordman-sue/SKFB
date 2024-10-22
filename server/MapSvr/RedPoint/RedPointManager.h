/**\file
��������
\author Kevin
\date 2017/1/6
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CRedPointManager
{
public:
	CRedPointManager();
	~CRedPointManager();

	void Init(Player *pPlayer);

public:
	// �������б�
	void OnNetRedPointList();

public:
	// ���֪ͨ
	void OnRedPointNotify(WORD wSystemId);

	// �Ƿ��к��
	bool IsHaveRedPoint(WORD wSystemId);

	// ��ӡ���״̬ for test
	void PrintRedPointStatus();

private:
	Player *m_pPlayer;
};