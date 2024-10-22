#include "stdafx.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Battle\BattleManager.h"
#include "AchievementDataDefine.h"
#include "VipManager.h"
#include "..\SystemNotice\SystemNotice.h"

CVipManager::CVipManager()
{

}

CVipManager::~CVipManager()
{

}

void CVipManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
}

// 添加VIP经验
void CVipManager::AddExp(DWORD dwExp)
{
	VipData &tVipData = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo.sVipData;
	tVipData.dwExp += dwExp;
	while (true)
	{
		const Vip_Config *pVipCfg = g_pCfgMgr->GetVip(tVipData.wLevel);
		if (!pVipCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到VIP配置数据![VipLevel:%d]", __FUNCTION__, tVipData.wLevel);
			break;
		}

		// 已达等级上限
		if (tVipData.wLevel >= g_pCfgMgr->GetMaxVipLevel())
			break;

		// 等级已达上限
// 		if (!pVipCfg->dwNeedExp)
// 			break;

		// 不足够升级
		if (tVipData.dwExp < pVipCfg->dwNeedExp)
			break;

		tVipData.wLevel++;

		m_pPlayer->OnUpdateAchievement(1);

		// 处理广播(广播需要到达指定VIP等级)
		DWORD nNoticeLevel = g_GetConfigValue(271);
		if (tVipData.wLevel >= nNoticeLevel)
		{
			DWORD dwNoticeId = g_pCfgMgr->GetVipNoticeId(tVipData.wLevel);
			CSystemNotice sys_notice(dwNoticeId, m_pPlayer->GetServerId());
			sys_notice.AddParam(m_pPlayer->GetRoleName());
			sys_notice.AddParam(tVipData.wLevel);
			sys_notice.SendMsg();
		}
	}

	m_pPlayer->NotifyProperty(ROLE_PRO_VIP_EXP, tVipData.dwExp);
	m_pPlayer->NotifyProperty(ROLE_PRO_VIP_LEVEL, tVipData.wLevel);
}

// 取VIP特权值
int CVipManager::GetValue(BYTE byType)
{
	WORD wVipLevel = m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL);
	const Vip_Config *pVipCfg = CConfigManager::Instance()->GetVip(wVipLevel);
	if (!pVipCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到VIP配置数据![VipLevel:%d]", __FUNCTION__, wVipLevel);
		return 0;
	}

	if (byType >= pVipCfg->vVipList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到VIP特权类型配置数据![VipType:%d]", __FUNCTION__, byType);
		return 0;
	}

	return pVipCfg->vVipList[byType];
}

