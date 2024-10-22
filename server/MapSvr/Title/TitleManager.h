/**\file
称号管理器
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
	// 请求角色称号信息
	void OnNetRoleTitleInfo();

	// 使用称号
	void OnNetUseTitle(int nTitleId);

public:

	// 添加称号
	static void AddTitleEx(DWORD dwServerId, DWORD dwRoleId, int nTitleId);

	// 发送角色称号信息
	void SendRoleTitleInfo();

	// 添加称号
	void AddTitle(int nTitleId, DWORD dwAddTime, bool bNotice = true);

	// 移除称号
	void RemoveTitle(int nTitleId);

	// 取称号
	const TitleData* GetTitle(int nTitleId);

	// 删除过期称号

	void DeleteTimeOutTitle();

	// 取当前使用的称号ID
	DWORD GetCurUseTitleId() { return m_pTitleData->nCurUseTitleId; };

	// 取当前使用的昵称ID
	DWORD GetCurUseNicknameId() { return m_pTitleData->nCurUseNicknameId; };
private:
	Player *m_pPlayer;
	PlayerTitleInfo *m_pTitleData;
};