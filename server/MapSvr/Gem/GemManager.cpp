#include "stdafx.h"
#include "GemManager.h"
#include "MsgDefine.h"
#include "..\Player.h"
#include "DBCDefine.h"
#include "..\Hero\HeroManager.h"
#include "..\ConfigManager.h"
#include "..\MapServer.h"


CGemManager::CGemManager()
{

}

CGemManager::~CGemManager()
{

}

// 初始化
void CGemManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pPlayerGemInfo = &pPlayer->GetRoleFullInfo().tSecondInfo.tGemInfo;
}

// 新建角色
void CGemManager::OnNewRole()
{
	InitDefaultOpenHole();
}

// 初始化默认开放的孔位
void CGemManager::InitDefaultOpenHole()
{
	// 已经初始化过了
	if (m_pPlayerGemInfo->dwHoleInitTime)
		return;

	// 处理默认开启的孔位
	WORD wMaxHole = 0;
	for (WORD i = 1; i <= MAX_GEM_HOLE_NUM; ++i)
	{
		const GemHole_Config *pHoleCfg = g_pCfgMgr->GetGemHole(i);
		if (pHoleCfg && pHoleCfg->wIsDefaultOpen && i > wMaxHole)
			wMaxHole = i;
	}

	for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		m_pPlayerGemInfo->arHoleOpenList[i] = wMaxHole;
	}

	m_pPlayerGemInfo->dwHoleInitTime = time(NULL);
}

// 检测是否已初始默认开放的孔位
bool CGemManager::IsInitDefaultOpenHole()
{
	for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		if (m_pPlayerGemInfo->arHoleOpenList[i])
			return true;
	}
	return false;
}

// 请求宝石列表
void CGemManager::OnNetGemList()
{
	SendGemDataList();
}

// 合成
void CGemManager::OnNetCompose(DWORD dwGemId, WORD wComposeNum)
{
	const Gem_Config *pGemCfg = g_pCfgMgr->GetGem(dwGemId);
	if (!pGemCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝石配置[GemId:%d]", __FUNCTION__, dwGemId);
		return;
	}

	// 检测是否可以合成
	if (!pGemCfg->dwNewGemFromCompose)
	{
		MAPLOG(ERROR_LEVEL, "[%s]此宝石不能合成[GemId:%d]", __FUNCTION__, dwGemId);
		return;
	}

	// 检测合成消耗的数量
	if (GetGemNum(dwGemId) < wComposeNum * pGemCfg->wComposeCostNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]宝石合成消耗的数量不足[GemId:%d]", __FUNCTION__, dwGemId);
		return;
	}

	// 删除合成消耗的宝石
	RemoveGem(dwGemId, wComposeNum * pGemCfg->wComposeCostNum);

	// 添加获得的新宝石
	AddGem(pGemCfg->dwNewGemFromCompose, wComposeNum);

	MSG_S2C_GEM_COMPOSE_RESP msg;
	msg.m_dwGemId = dwGemId;
	msg.m_wGemNum = wComposeNum;
	m_pPlayer->SendPacket(&msg);
}

// 镶嵌
void CGemManager::OnNetInlay(WORD wFormationSlot, GemInlay *pInlayList, WORD wInlayNum)
{
	// wFormationSlot 从0开始
	// wHole 从1开始

	if (!wInlayNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]镶嵌的宝石列表为空", __FUNCTION__);
		return;
	}

	const Formation *pFormation = m_pPlayer->GetHeroManager()->GetFormationByIndex(wFormationSlot);
	if (!pFormation)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到阵型数据[Formation:%d]", __FUNCTION__, wFormationSlot);
		return;
	}

	for (WORD i = 0; i < wInlayNum; ++i)
	{
		DWORD dwGemId = pInlayList[i].dwGemId;
		WORD wHole = pInlayList[i].wHole;

		// 取宝石配置
		const Gem_Config *pGemCfg = g_pCfgMgr->GetGem(dwGemId);
		if (!pGemCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到宝石配置[GemId:%d]", __FUNCTION__, dwGemId);
			continue;
		}

		// 检测孔位合法性
		if (!wHole || wHole > MAX_GEM_HOLE_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]宝石孔位不合法[Hole:%d]", __FUNCTION__, wHole);
			return;
		}

		// 检测宝石是否存在
		if (!GetGemNum(dwGemId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]镶嵌的宝石不存在[GemId:%d]", __FUNCTION__, dwGemId);
			continue;
		}

		// 检测是否开启
		if (!IsHoleOpen(wFormationSlot, wHole))
		{
			MAPLOG(ERROR_LEVEL, "[%s]宝石孔位还没开启[Formation:%d,Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
			continue;
		}

		DWORD dwOldGemId = pFormation->arGemList[wHole - 1];

		// 检测此同一个阵位上宝石类型数量限制
		const GemInlay_Config *pInlayCfg = g_pCfgMgr->GetGemInlay(pGemCfg->wType);
		if (!pInlayCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到宝石镶嵌配置[type:%d]", __FUNCTION__, pGemCfg->wType);
			continue;
		}
		WORD wSimilarGemNum = GetFormationSlotSimilarGemNum(wFormationSlot, pGemCfg->wType, dwOldGemId);
		if (wSimilarGemNum >= pInlayCfg->wInlayNumLimit)
		{
			MAPLOG(ERROR_LEVEL, "[%s]同一个阵位上同类宝石数量限制[type:%d]", __FUNCTION__, pGemCfg->wType);
			continue;
		}

		// todo::不知道是否需要消耗

		// 检测原孔位上是否有宝石,有则归还
		if (dwOldGemId)
			AddGem(dwOldGemId);

		// 删除准备镶嵌的宝石
		RemoveGem(dwGemId);

		// 镶嵌
		m_pPlayer->GetHeroManager()->SetGemHole(wFormationSlot, wHole-1, dwGemId);
	}

	// 更新属性
	m_pPlayer->UpdateFightingPower();

	m_pPlayer->GetHeroManager()->SendFormationList();

	MSG_S2C_GEM_INLAY_RESP msg;
	msg.m_wFormationSlot = wFormationSlot;
	msg.m_wGemInlayNum = wInlayNum;
	memcpy_s(msg.m_arGemInlayList, sizeof(msg.m_arGemInlayList), pInlayList, wInlayNum*sizeof(GemInlay));
	m_pPlayer->SendPacket(&msg);
}

// 卸下
void CGemManager::OnNetUnload(WORD wFormationSlot, WORD wHole)
{
	// wFormationSlot 从0开始
	// wHole 从1开始

 	if (wFormationSlot >= MAX_BATTLE_HERO_NUM)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]阵型槽位越界[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
 		return;
 	}

	if (!wHole || wHole > MAX_GEM_HOLE_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]宝石孔位不合法[Hole:%d]", __FUNCTION__, wHole);
		return;
	}

	// 检测是否已开启
	if (!IsHoleOpen(wFormationSlot, wHole))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]宝石孔位还没开启[FormationSlot:%d, Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
		return;
	}

	// 取阵型数据
	const Formation *pFormation = m_pPlayer->GetHeroManager()->GetFormationByIndex(wFormationSlot);
	if (!pFormation)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到阵型数据[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
		return;
	}

	// 检测孔位上的宝石为空
	DWORD dwGemId = pFormation->arGemList[wHole - 1];
	if (!dwGemId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]宝石孔位上没有镶嵌宝石[FormationSlot:%d,Hole:%d]", __FUNCTION__, wFormationSlot,wHole);
		return;
	}

	// 清空孔位的宝石
	m_pPlayer->GetHeroManager()->SetGemHole(wFormationSlot, wHole-1, 0);

	// 归还卸下的宝石
	AddGem(dwGemId);

	// 更新属性
	m_pPlayer->UpdateFightingPower();

	m_pPlayer->GetHeroManager()->SendFormationList();

	MSG_S2C_GEM_UNLOAD_RESP msg;
	msg.m_wFormationSlot = wFormationSlot;
	msg.m_wGemHole = wHole;
	m_pPlayer->SendPacket(&msg);
}

// 开启孔位
void CGemManager::OnNetHoleOpen(WORD wFormationSlot, WORD wHole)
{
	// wFormationSlot 从0开始
	// wHole 从1开始

	// 取配置
	const GemHole_Config *pHoleCfg = g_pCfgMgr->GetGemHole(wHole);
	if (!pHoleCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝石孔位配置[Hole:%d]", __FUNCTION__, wHole);
		return;
	}

	// 检测是否默认开启
	if (pHoleCfg->wIsDefaultOpen)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]宝石孔位已默认开启[FormationSlot:%d, Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
		return;
	}

	// 检测是否已开启
	if (IsHoleOpen(wFormationSlot, wHole))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]宝石孔位已开启[FormationSlot:%d, Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
		return;
	}

	// 检测是否越界开启
	WORD dwCurOpenHole = GetOpenHole(wFormationSlot);
	if (wHole > dwCurOpenHole + 1)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]越界开启宝石孔位[FormationSlot:%d, Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
		return;
	}

	// 检测等级
	if (m_pPlayer->GetLevel() < pHoleCfg->wNeedRoleLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]宝石开孔位等级不足[FormationSlot:%d, Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
		return;
	}

	// 检测消耗道具
	if (pHoleCfg->vCostItem.dwId && m_pPlayer->GetItemNum(pHoleCfg->vCostItem.dwId) < pHoleCfg->vCostItem.dwNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]宝石开孔消耗道具不足[FormationSlot:%d, Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
		return;
	}

	// 删除消耗的物品
	m_pPlayer->RemoveItem(pHoleCfg->vCostItem.dwId, pHoleCfg->vCostItem.dwNum, FROM_GEM_OPEN_HOLE);

	// 开启孔位
	OpenHole(wFormationSlot, wHole);

	MSG_S2C_GEM_HOLE_OPEN_RESP msg;
	msg.m_wFormationSlot = wFormationSlot;
	msg.m_wGemHole = wHole;
	m_pPlayer->SendPacket(&msg);
}

// 分解
void CGemManager::OnNetDecompose(DWORD dwGemId, DWORD dwGemNum)
{
	// 取配置
	const Gem_Config *pGemCfg = g_pCfgMgr->GetGem(dwGemId);
	if (!pGemCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝石配置[GemId:%d]", __FUNCTION__, dwGemId);
		return;
	}

	// 检测宝石的数量
	if (GetGemNum(dwGemId) < dwGemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]宝石数量不足[GemId:%d, GemNum:%d]", __FUNCTION__, dwGemId, dwGemNum);
		return;
	}

	RemoveGem(dwGemId, dwGemNum);

	m_pPlayer->ModifyProperty(ROLE_PRO_GEM_COIN, pGemCfg->nGemCoin * dwGemNum);

	MSG_S2C_GEM_DECOMPOSE_RESP msg;
	msg.m_dwGemId = dwGemId;
	msg.m_dwGemNum = dwGemNum;
	m_pPlayer->SendPacket(&msg);
}

// 添加宝石
bool CGemManager::AddGem(DWORD dwGemId, DWORD dwGemNum, bool bNotify, WORD wFromId)
{
	Gem *pGem = GetGem(dwGemId);
	if (pGem)
	{
		pGem->dwNum += dwGemNum;
	}
	else
	{
		if (m_pPlayerGemInfo->wGemNum >= MAX_GEM_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]宝石列表已满[num:%d]", __FUNCTION__, m_pPlayerGemInfo->wGemNum);
			return false;
		}

		Gem &tGem = m_pPlayerGemInfo->arGemList[m_pPlayerGemInfo->wGemNum++];
		tGem.dwId = dwGemId;
		tGem.dwNum = dwGemNum;
	}

	// 同步数据
	if (bNotify)
	{
		MSG_S2C_GEM_DATA_NOTIFY msg;
		msg.m_dwGemId = dwGemId;
		msg.m_dwGemNum = dwGemNum;
		msg.m_byFlag = 1;
		m_pPlayer->SendPacket(&msg);
	}

	// 记录获得
	g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), dwGemId, dwGemNum, true, wFromId);

	return true;
}

// 删除宝石
DWORD CGemManager::RemoveGem(DWORD dwGemId, DWORD dwGemNum, bool bNotify, WORD wFromId)
{
	Gem *pGem;
	WORD wRemoveNum = 0;
	for (WORD i = 0; i < m_pPlayerGemInfo->wGemNum; ++i)
	{
		pGem = &m_pPlayerGemInfo->arGemList[i];
		if (pGem->dwId == dwGemId)
		{
			// 记录获得
			g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), dwGemId, dwGemNum, false, wFromId);

			// 有足够的数量扣除
			if (pGem->dwNum > dwGemNum)
			{
				wRemoveNum = dwGemNum;
				pGem->dwNum -= dwGemNum;
			}
			// 数量不足,则从列表里拿掉
			else
			{
				wRemoveNum = pGem->dwNum;
				*pGem = m_pPlayerGemInfo->arGemList[--m_pPlayerGemInfo->wGemNum];
			}
			break;
		}
	}

	if (bNotify && wRemoveNum)
	{
		MSG_S2C_GEM_DATA_NOTIFY msg;
		msg.m_dwGemId = dwGemId;
		msg.m_dwGemNum = wRemoveNum;
		msg.m_byFlag = 0;
		m_pPlayer->SendPacket(&msg);
	}

	return wRemoveNum;
}

// 取宝石
Gem* CGemManager::GetGem(DWORD dwGemId)
{
	for (WORD i = 0; i < m_pPlayerGemInfo->wGemNum; ++i)
	{
		if (m_pPlayerGemInfo->arGemList[i].dwId == dwGemId)
			return &m_pPlayerGemInfo->arGemList[i];
	}
	return NULL;
}

// 取宝石数量
DWORD CGemManager::GetGemNum(DWORD dwGemId)
{
	Gem *pGem = GetGem(dwGemId);
	return pGem ? pGem->dwNum : 0;
}

// 检测孔位是否已开启
bool CGemManager::IsHoleOpen(WORD wFormationSlot, WORD wHole)
{
	if (wFormationSlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵型槽位越界[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
		return false;
	}

	// 如果小于等级当前的表示已开启了
	if (wHole <= m_pPlayerGemInfo->arHoleOpenList[wFormationSlot])
		return true;

	return false;
}

// 取阵型当前已开放的最大孔位
WORD CGemManager::GetOpenHole(WORD wFormationSlot)
{
	if (wFormationSlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵型槽位越界[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
		return 0;
	}

	return m_pPlayerGemInfo->arHoleOpenList[wFormationSlot];
}

// 开启孔位
void CGemManager::OpenHole(WORD wFormationSlot, WORD wHole)
{
	if (wFormationSlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵型槽位越界[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
		return;
	}
	m_pPlayerGemInfo->arHoleOpenList[wFormationSlot] = wHole;
}

// 取阵位上同类型宝石数量
WORD CGemManager::GetFormationSlotSimilarGemNum(WORD wFormationSlot, WORD wGemTeyp, DWORD dwExceptGemId)
{
	const Formation *pFormation = m_pPlayer->GetHeroManager()->GetFormationByIndex(wFormationSlot);
	if (!pFormation)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到阵型槽位上的数据[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
		return 0;
	}

	WORD wNum = 0;
	for (WORD i = 0; i < MAX_GEM_HOLE_NUM; ++i)
	{
		DWORD dwGemId = pFormation->arGemList[i];
		if (dwGemId && dwGemId != dwExceptGemId)
		{
			const Gem_Config *pGemCfg = g_pCfgMgr->GetGem(dwGemId);
			if (pGemCfg && pGemCfg->wType == wGemTeyp)
				wNum++;
		}
	}

	return wNum;
}

// 发送宝石列表
void CGemManager::SendGemDataList()
{
	MSG_S2C_GEM_LIST_RESP msg;
	msg.m_PlayerGemInfo = *m_pPlayerGemInfo;
	m_pPlayer->SendPacket(&msg);
}