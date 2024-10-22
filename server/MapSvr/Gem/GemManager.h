/**\file
物品管理器
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

	// 初始化
	void Init(Player *pPlayer);

	// 新建角色
	void OnNewRole();

	// 初始化默认开放的孔位
	void InitDefaultOpenHole();

	// 检测是否已初始默认开放的孔位
	bool IsInitDefaultOpenHole();

public:
	// 请求宝石列表
	void OnNetGemList();

	// 合成
	void OnNetCompose(DWORD dwGemId, WORD wComposeNum);
	
	// 镶嵌
	void OnNetInlay(WORD wFormationSlot, GemInlay *pInlayList, WORD wInlayNum);

	// 卸下
	void OnNetUnload(WORD wFormationSlot, WORD wHole);

	// 孔位开启
	void OnNetHoleOpen(WORD wFormationSlot, WORD wHole);

	// 分解
	void OnNetDecompose(DWORD dwGemId, DWORD dwGemNum);
public:
	// 添加宝石
	bool AddGem(DWORD dwGemId, DWORD dwGemNum = 1, bool bNotify = true, WORD wFromId = 0);

	// 删除宝石
	DWORD RemoveGem(DWORD dwGemId, DWORD dwGemNum = 1, bool bNotify = true, WORD wFromId = 0);

	// 取宝石
	Gem* GetGem(DWORD dwGemId);

	// 取宝石数量
	DWORD GetGemNum(DWORD dwGemId);

	// 检测孔位是否已开启
	bool IsHoleOpen(WORD wFormationSlot, WORD wHole);

	// 取当前开放的孔位(最大的)
	WORD GetOpenHole(WORD wFormationSlot);

	// 开启孔位
	void OpenHole(WORD wFormationSlot, WORD wHole);

	// 取阵位上同类型宝石数量
	WORD GetFormationSlotSimilarGemNum(WORD wFormationSlot, WORD wGemTeyp, DWORD dwExceptGemId);

protected:
	
	// 发送宝石列表
	void SendGemDataList();

private:
	Player *m_pPlayer;
	PlayerGemInfo *m_pPlayerGemInfo;
};