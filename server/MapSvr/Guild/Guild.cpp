#include "stdafx.h"
#include "Guild.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "GuildManager.h"
#include "..\PlayerManager.h"
#include "MsgDefine.h"
#include "..\MapServer.h"
#include "..\PlayerBaseDataManager.h"
#include "..\Rank\RankManager.h"
#include "..\Fuben\FubenManager.h"
#include "data_define.pb.h"
#include "..\MapSvrQuery.h"
#include "..\Shop\ShopManager.h"
#include "..\RewardCenter\RewardCenterManager.h"

CGuild::CGuild()
{
	m_tData.Init();
	m_dwCreateMonterTime = 0;
}

CGuild::~CGuild()
{

}

bool CGuild::Init(Player *pCreater, CGuildManager *pGuildMgr, WORD wIconId, const char *pszName)
{
	DWORD dwGuildId = pGuildMgr->GetFreeGuildId(pCreater->GetServerId());
	if (!dwGuildId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]获取工会ID为0", __FUNCTION__);
		return false;
	}

	m_dwCreateMonterTime = 0;
	m_tData.Init();
	m_pGuildMgr = pGuildMgr;
	m_tData.dwId = dwGuildId;
	m_tData.wIconId = wIconId;
	m_tData.wLevel = 1;
	m_tData.wIsFromHistoryChapter = true;
	m_tData.dwCreateTime = DWORD(time(NULL));
	strcpy_s(m_tData.szName, sizeof(m_tData.szName) - 1, pszName);
	memset(&m_tLastQYInfo, 0, sizeof(m_tLastQYInfo));

	GuildMember member;
	member.Init();
	member.byPosition = GUILD_POS_CAPTAIN;
	member.dwRoleId = pCreater->GetCharID();
	member.dwLastBattleTimesRecoverTime = time(NULL);
	m_dwSeverId = pCreater->GetServerId();
	if (!AddMember(&member))
	{
		MAPLOG(ERROR_LEVEL, "[%s]创建工会添加成员时失败![%s]", __FUNCTION__, pCreater->GetUTF8RoleName());
		return false;
	}

	return true;
}

void CGuild::OnNewDay()
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		GuildMember &tMember = m_tData.arMemberList[i];
		tMember.byQYType = 0;
		tMember.byTodayBattleTimes = 0;
		tMember.byQSRewardRecordNum = 0;
		tMember.dwTodayContribution = 0;
	}

	WORD wRemoveBegin = 0;
	WORD wRemoveEnd = 0;

	// 从历史最高记录开始
	if (m_tData.wIsFromHistoryChapter)
	{
		m_tData.wTodayStartChapter = m_tData.wHistoryChapterRecord;
		m_tData.wCurPassChapter = m_tData.wTodayStartChapter;

		wRemoveBegin = m_tData.wHistoryChapterRecord;
		wRemoveEnd = m_tData.wHistoryChapterRecord;

// 		map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(m_tData.wHistoryChapterRecord+1);
// 		if (iter != m_MapRewardRecordList.end())
// 			m_MapRewardRecordList.erase(iter);
	}
	else
	{
		// 至少要打到第二章才可以回退
		if (m_tData.wHistoryChapterRecord >= 2)
			m_tData.wTodayStartChapter = m_tData.wHistoryChapterRecord - 1;
		else
			m_tData.wTodayStartChapter = m_tData.wHistoryChapterRecord;

		m_tData.wCurPassChapter = m_tData.wTodayStartChapter;
		wRemoveBegin = m_tData.wTodayStartChapter;
		wRemoveEnd = m_tData.wHistoryChapterRecord;
	}

	// 删除宝箱领取记录
	m_MapRewardRecordList.clear();
// 	for (WORD i = wRemoveBegin; i <= wRemoveEnd; ++i)
// 	{
// 		map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(i+1);
// 		if (iter != m_MapRewardRecordList.end())
// 			m_MapRewardRecordList.erase(iter);
// 	}

	// 泉涌次数重置
	m_tData.byQYTimes = 0;
	m_tData.dwQuanShui = 0;

	// 每天默认是从历史记录章节开始
	// m_tData.wIsFromHistoryChapter = true;

	m_DamageRecordList.clear();

	SaveDataToDB();
}

void CGuild::OnNewWeek()
{
	m_tData.dwActive = 0;
}

// 存储数据到数据库
void CGuild::SaveDataToDB(bool isDelete)
{
	UINT64 lCurTime = GetTickCount64();

	PB::GuildInfo PBGuildInfo;
	
	// 基本数据
	::PB::GuildBaseData *pBaseData = PBGuildInfo.mutable_base_data();
	pBaseData->set_guild_id(m_tData.dwId);
	pBaseData->set_guild_name(m_tData.szName);
	pBaseData->set_icon_id(m_tData.wIconId);
	pBaseData->set_create_time(m_tData.dwCreateTime);
	pBaseData->set_level(m_tData.wLevel);
	pBaseData->set_exp(m_tData.dwExp);
	pBaseData->set_qs(m_tData.dwQuanShui);
	pBaseData->set_qy_times(m_tData.byQYTimes);
	pBaseData->set_notice(m_tData.szNotice);
	pBaseData->set_declaration(m_tData.szDeclaration);
	pBaseData->set_cur_pass_chapter(m_tData.wCurPassChapter);
	pBaseData->set_server_id(m_dwSeverId);
	pBaseData->set_history_chapter_record(m_tData.wHistoryChapterRecord);
	pBaseData->set_today_start_chapter(m_tData.wTodayStartChapter);
	pBaseData->set_is_from_history_chapter(m_tData.wIsFromHistoryChapter);
	pBaseData->set_active(m_tData.dwActive);

	// 成员列表
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		GuildMember &tMember = m_tData.arMemberList[i];
		::PB::GuildMember *pPBMember = PBGuildInfo.add_member_list();
		pPBMember->set_role_id(tMember.dwRoleId);
		pPBMember->set_position(tMember.byPosition);
		pPBMember->set_total_contribution(tMember.dwTotalContribution);
		pPBMember->set_cur_contribution(0);
		pPBMember->set_today_contribution(tMember.dwTodayContribution);
		pPBMember->set_last_logout_time(tMember.dwLastLogoutTime);
		pPBMember->set_qy_type(tMember.byQYType);
		pPBMember->set_battle_fb_times(tMember.byBattleTimes);
		pPBMember->set_buy_battle_fb_times(tMember.byBuyBattleTimes);
		pPBMember->set_battle_times_last_recover_time(tMember.dwLastBattleTimesRecoverTime);
		for (BYTE k = 0; k < tMember.byQSRewardRecordNum; ++k)
		{
			pPBMember->add_qs_reward_record_list(tMember.arQSRewardRecord[k]);
		}
// 		for (WORD k = 0; k < tMember.wTakeFBChapterRewardRecordNum; ++k)
// 		{
// 			pPBMember->add_take_chapter_reward_record(tMember.arTakeFBChapterRewardRecord[k]);
// 		}
	}

	// 工会事件列表
// 	for (size_t i = 0; i < m_EventList.size(); i++)
// 	{
// 		::PB::GuildEvent *pPBEvent = PBGuildInfo.add_event_list();
// 		GuildEvent &tEvent = m_EventList[i];
// 		pPBEvent->set_event_id(tEvent.wEventId);
// 		pPBEvent->set_target_name(tEvent.szTargetName);
// 		pPBEvent->set_value(tEvent.dwValue);
// 		pPBEvent->set_time(tEvent.dwTime);
// 	}
	for (list<GuildEvent>::iterator iter = m_EventList.begin(); iter != m_EventList.end(); ++iter)
	{
		::PB::GuildEvent *pPBEvent = PBGuildInfo.add_event_list();
		GuildEvent &tEvent = *iter;
		pPBEvent->set_event_id(tEvent.wEventId);
		pPBEvent->set_target_name(tEvent.szTargetName);
		pPBEvent->set_value(tEvent.dwValue);
		pPBEvent->set_time(tEvent.dwTime);
	}

	// 留言列表
	for (list<GuildLeaveMessage>::iterator iter = m_MessageList.begin(); iter != m_MessageList.end(); ++iter)
	{
		::PB::GuildLeaveMessage *pPBLeaveMessage = PBGuildInfo.add_leave_message_list();
		pPBLeaveMessage->set_time(iter->dwTime);
		pPBLeaveMessage->set_name(iter->szName);
		pPBLeaveMessage->set_position(iter->byPos);
		pPBLeaveMessage->set_text(iter->szText);
	}

	// 申请列表
	for (size_t i = 0; i < m_ApplyList.size(); i++)
	{
		PBGuildInfo.add_apply_list(m_ApplyList[i].dwRoleId);
	}

	// 技能列表
	for (size_t i = 0; i < m_SkillList.size(); i++)
	{
		::PB::SkillData *pPBSkillData = PBGuildInfo.add_skill_list();
		pPBSkillData->set_skill_id(m_SkillList[i].wSkillId);
		pPBSkillData->set_level(m_SkillList[i].wLevel);
	}

	// 限时商品列表
	for (size_t i = 0; i < m_vLimitGoodsList.size(); ++i)
	{
		::PB::ShopGoods *pPBGoods = PBGuildInfo.add_goods_list();
		pPBGoods->set_goods_id(m_vLimitGoodsList[i].wId);
		pPBGoods->set_goods_num(m_vLimitGoodsList[i].wNum);
	}

	// 聊天记录
	for (list<ChatRecord>::iterator iter = m_ChatRecordList.begin(); iter != m_ChatRecordList.end(); ++iter)
	{
		::PB::ChatRecord *pPBRecord = PBGuildInfo.add_chat_record_list();
		pPBRecord->set_role_id(iter->tSenderData.dwRoleID);
		pPBRecord->set_role_name(iter->tSenderData.szRoleName);
		pPBRecord->set_chat_text(iter->szChatText);
		pPBRecord->set_role_vip_level(iter->tSenderData.byVipLevel);
		pPBRecord->set_head_img_id(iter->tSenderData.dwHeadIcon);
		pPBRecord->set_time(iter->dwTime);
		pPBRecord->set_role_use_title_id(iter->tSenderData.dwCurUseNicknameId);
	}

	// 伤害记录列表
	for (map<DWORD, DWORD>::iterator iter = m_DamageRecordList.begin(); iter != m_DamageRecordList.end(); ++iter)
	{
		::PB::GuildMonsterDamageRecord *pPBRecord = PBGuildInfo.add_damage_record_list();
		pPBRecord->set_role_id(iter->first);
		pPBRecord->set_damage(iter->second);
	}

	// 关卡奖励领取记录
	for (map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.begin(); iter != m_MapRewardRecordList.end(); ++iter)
	{
		::PB::GuildMapRewardRecord  *pPBRewardRecord = PBGuildInfo.add_map_reward_record_list();
		pPBRewardRecord->set_chapter(iter->first);
		for (int i = 0; i < CAMP_NUM; ++i)
		{
			set<DWORD> *pTakePlayerList = &iter->second.arAlreadyGetPlayerList[i];
			vector<GuildFBBox> *pBoxList = &iter->second.vTakeBoxList[i];

			::PB::GuildCampBox *pPBCampBox = pPBRewardRecord->add_camp_box_list();
			pPBCampBox->set_camp(i);

			// 已经领取的玩家列表
			for (set<DWORD>::iterator iter = pTakePlayerList->begin(); iter != pTakePlayerList->end(); ++iter)
			{
				pPBCampBox->add_take_reward_player_list(*iter);
			}
			// 已领取的宝箱记录列表
			for (vector<GuildFBBox>::iterator iter = pBoxList->begin(); iter != pBoxList->end(); ++iter)
			{
				::PB::GuildFBBox *pPBBox = pPBCampBox->add_take_box_record_list();
				pPBBox->set_box_idx(iter->wBoxIdx);
				pPBBox->set_role_name(iter->szRoleName);
				pPBBox->set_box_reward_idx(iter->wRewardIdx);
			}
		}
	}

	// 阵营怪物列表
	for (int i = 0; i < CAMP_NUM; ++i)
	{
		vector<MonsterData> &vMonsterList = m_arMonsterList[i];
		::PB::GuildCampMonster *pPBCampMonster = PBGuildInfo.add_camp_monster_list();
		// 设置击杀者及阵营
		pPBCampMonster->set_camp(i);
		pPBCampMonster->set_killer_name(m_KillerList[i]);
		// 设置阵营怪物列表
		for (vector<MonsterData>::iterator iter = vMonsterList.begin(); iter != vMonsterList.end(); ++iter)
		{
			::PB::MonsterData *pPBMonster = pPBCampMonster->add_monster_list();
			pPBMonster->set_damage(iter->dwDamage);
			pPBMonster->set_idx(iter->byIndex);
			pPBMonster->set_total_hp(iter->dwTotalHP);
		}
	}

	// 创建怪物时间
	PBGuildInfo.set_create_monster_time(m_dwCreateMonterTime);

	string strSaveData("");
	if (!PBGuildInfo.AppendToString(&strSaveData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]系列化工会数据失败[GuildId:%d]", __FUNCTION__, GetGuildId());
		return;
	}
	if (strSaveData.size() > MAX_GUILD_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]工会存储数据过大[size:%d]", __FUNCTION__, strSaveData.size());
		return;
	}

	GuildData_Save* pQuery = GuildData_Save::ALLOC();
	pQuery->SetIndex(DB_GUILD_DATA_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_GUILD);
	pQuery->SetServerId(m_dwSeverId);
	pQuery->SetGuildName(GetGuildName());
	pQuery->SetKey(GetGuildId());
	pQuery->SetDataBuff(strSaveData.c_str(), strSaveData.size());
	pQuery->SetIsDelete(isDelete);
	g_MapServer.MapDBQuery(pQuery);

	DWORD dwCostTime = GetTickCount64() - lCurTime;
	if (dwCostTime){
		MAPLOG(ERROR_LEVEL, "[%s]系列化工会数据消息时间(%d)毫秒[GuildId:%d]!", __FUNCTION__, dwCostTime, GetGuildId());
	}
}

// 从数据库加载数据
void CGuild::LoadDataFromDB(CGuildManager *pGuildMgr, PB::GuildInfo *pPBGuildInfo)
{
	::PB::GuildBaseData *pPBBaseData = pPBGuildInfo->mutable_base_data();

	m_pGuildMgr = pGuildMgr;
	m_dwSeverId = pPBBaseData->server_id();
	
	// 工会基本数据
	m_tData.dwId = pPBBaseData->guild_id();
	strcpy_s(m_tData.szName, sizeof(m_tData.szName), pPBBaseData->guild_name().c_str());
	strcpy_s(m_tData.szNotice, sizeof(m_tData.szNotice), pPBBaseData->notice().c_str());
	strcpy_s(m_tData.szDeclaration, sizeof(m_tData.szDeclaration), pPBBaseData->declaration().c_str());
	m_tData.wIconId = pPBBaseData->icon_id();
	m_tData.dwCreateTime = pPBBaseData->create_time();
	m_tData.wLevel = pPBBaseData->level();
	m_tData.dwExp = pPBBaseData->exp();
	m_tData.dwQuanShui = pPBBaseData->qs();
	m_tData.byQYTimes = pPBBaseData->qy_times();
	m_tData.wCurPassChapter = pPBBaseData->cur_pass_chapter();
	m_tData.wHistoryChapterRecord = pPBBaseData->history_chapter_record();
	m_tData.wTodayStartChapter = pPBBaseData->today_start_chapter();
	m_tData.wIsFromHistoryChapter = pPBBaseData->is_from_history_chapter();
	m_tData.dwActive = pPBBaseData->active();

	// 工会成员
	m_tData.byMemberNum = 0;
	for (int i = 0; i < pPBGuildInfo->member_list_size(); ++i)
	{
		const PB::GuildMember &tPBMember = pPBGuildInfo->member_list(i);
		GuildMember &tMember = m_tData.arMemberList[m_tData.byMemberNum++];

		tMember.dwRoleId = tPBMember.role_id();
		tMember.byPosition = tPBMember.position();
		tMember.dwTotalContribution = tPBMember.total_contribution();
		//tMember.dwCurContribution = tPBMember.cur_contribution();
		tMember.dwTodayContribution = tPBMember.today_contribution();
		tMember.dwLastLogoutTime = tPBMember.last_logout_time();
		tMember.byQYType = tPBMember.qy_type();
		tMember.byBattleTimes = tPBMember.battle_fb_times();
		tMember.byBuyBattleTimes = tPBMember.buy_battle_fb_times();
		tMember.dwLastBattleTimesRecoverTime = tPBMember.battle_times_last_recover_time();
		tMember.byQSRewardRecordNum = 0;
		for (int k = 0; k < tPBMember.qs_reward_record_list_size(); ++k)
		{
			tMember.arQSRewardRecord[tMember.byQSRewardRecordNum++] = tPBMember.qs_reward_record_list(k);
		}
// 		tMember.wTakeFBChapterRewardRecordNum = 0;
// 		for (int k = 0; k < tPBMember.take_chapter_reward_record_size(); ++k)
// 		{
// 			tMember.arTakeFBChapterRewardRecord[tMember.wTakeFBChapterRewardRecordNum++] = tPBMember.take_chapter_reward_record(k);
// 		}


		m_pGuildMgr->AddFindData(tMember.dwRoleId, GetGuildId());
	}

	// 申请进入列表
	for (int i = 0; i < pPBGuildInfo->apply_list_size(); ++i)
	{
		GuildApplyData tApplyData;
		tApplyData.dwRoleId = pPBGuildInfo->apply_list(i);
		m_ApplyList.push_back(tApplyData);
	}

	// 工会事件列表
	for (int i = 0; i < pPBGuildInfo->event_list_size(); ++i)
	{
		const ::PB::GuildEvent &tPBEvent = pPBGuildInfo->event_list(i);
		GuildEvent tEvent;
		tEvent.wEventId = tPBEvent.event_id();
		tEvent.dwTime = tPBEvent.time();
		strcpy_s(tEvent.szTargetName, sizeof(tEvent.szTargetName) - 1, tPBEvent.target_name().c_str());
		tEvent.dwValue = tPBEvent.value();
		AddEvent(tEvent);
	}

	// 留言列表
	for (int i = 0; i < pPBGuildInfo->leave_message_list_size(); ++i)
	{
		const ::PB::GuildLeaveMessage &tPBLeaveMessage = pPBGuildInfo->leave_message_list(i);
		GuildLeaveMessage tLeaveMessage;
		tLeaveMessage.dwTime = tPBLeaveMessage.time();
		strcpy_s(tLeaveMessage.szName, sizeof(tLeaveMessage.szName) - 1, tPBLeaveMessage.name().c_str());
		tLeaveMessage.byPos = tPBLeaveMessage.position();
		//int nMessageLen = strlen(tPBLeaveMessage.text().c_str());
		strcpy_s(tLeaveMessage.szText, sizeof(tLeaveMessage.szText), tPBLeaveMessage.text().c_str());
		m_MessageList.push_back(tLeaveMessage);
	}

	// 技能列表
	for (int i = 0; i < pPBGuildInfo->skill_list_size(); ++i)
	{
		const ::PB::SkillData &tPBSkill = pPBGuildInfo->skill_list(i);
		SkillData tSkill;
		tSkill.wSkillId = tPBSkill.skill_id();
		tSkill.wLevel = tPBSkill.level();
		m_SkillList.push_back(tSkill);
	}

	// 限时商品列表
	for (int i = 0; i < pPBGuildInfo->goods_list_size(); ++i)
	{
		const ::PB::ShopGoods &tPBGoods = pPBGuildInfo->goods_list(i);
		Goods tGoods;
		tGoods.wId = tPBGoods.goods_id();
		tGoods.wNum = tPBGoods.goods_num();
		m_vLimitGoodsList.push_back(tGoods);
	}
	// todo::打补丁,防止上次刷新出来的商品木有存进去
	if (!m_vLimitGoodsList.size())
	{
		m_vLimitGoodsList = CGuildManager::Instance()->GetLimitGoodsList();
	}

	// 聊天记录
	m_ChatRecordList.clear();
	ChatRecord tChatRecord;
	for (int i = 0; i < pPBGuildInfo->chat_record_list_size(); ++i)
	{
		const ::PB::ChatRecord &tPBRecord = pPBGuildInfo->chat_record_list(i);
		tChatRecord.tSenderData.dwRoleID = tPBRecord.role_id();
		tChatRecord.tSenderData.dwHeadIcon = tPBRecord.head_img_id();
		tChatRecord.tSenderData.byVipLevel = tPBRecord.role_vip_level();
		tChatRecord.tSenderData.dwCurUseNicknameId = tPBRecord.role_use_title_id();
		strcpy_s(tChatRecord.tSenderData.szRoleName, sizeof(tChatRecord.tSenderData.szRoleName) - 1, tPBRecord.role_name().c_str());
		strcpy_s(tChatRecord.szChatText, sizeof(tChatRecord.szChatText), tPBRecord.chat_text().c_str());
		tChatRecord.dwTime = tPBRecord.time();
		tChatRecord.dwVoiceId = 0;
		tChatRecord.byChatType = CHAT_TYPE_GUILD;
		AddChatRecord(tChatRecord);
	}

	// 伤害记录列表
	m_DamageRecordList.clear();
	for (int i = 0; i < pPBGuildInfo->damage_record_list_size(); ++i)
	{
		const ::PB::GuildMonsterDamageRecord &pPBDamageRecord = pPBGuildInfo->damage_record_list(i);
		m_DamageRecordList.insert(make_pair(pPBDamageRecord.role_id(), pPBDamageRecord.damage()));
	}

	// 关卡奖励领取记录[chapter,reward]
	m_MapRewardRecordList.clear();
	for (int i = 0; i < pPBGuildInfo->map_reward_record_list_size(); ++i)
	{
		const ::PB::GuildMapRewardRecord &tPBRewardRecord = pPBGuildInfo->map_reward_record_list(i);
		GuildFBMapReward tMapReward;
		for (int k = 0; k < tPBRewardRecord.camp_box_list_size(); ++k)
		{
			const ::PB::GuildCampBox &tPBCampBox = tPBRewardRecord.camp_box_list(k);

			// 已领取的宝箱记录列表
			vector<GuildFBBox> &vBoxList = tMapReward.vTakeBoxList[tPBCampBox.camp()];
			for (size_t m = 0; m < tPBCampBox.take_box_record_list_size(); m++)
			{
				const ::PB::GuildFBBox &tPBBox = tPBCampBox.take_box_record_list(m);
				GuildFBBox tBox;
				tBox.wBoxIdx = tPBBox.box_idx();
				tBox.wRewardIdx = tPBBox.box_reward_idx();
				strcpy_s(tBox.szRoleName, sizeof(tBox.szRoleName) - 1, tPBBox.role_name().c_str());
				vBoxList.push_back(tBox);
			}

			// 已经领取的玩家列表
			set<DWORD> &vTakePlayerList = tMapReward.arAlreadyGetPlayerList[tPBCampBox.camp()];
			for (size_t m = 0; m < tPBCampBox.take_reward_player_list_size(); ++m)
			{
				vTakePlayerList.insert(tPBCampBox.take_reward_player_list(m));
			}
		}
		m_MapRewardRecordList.insert(make_pair(tPBRewardRecord.chapter(), tMapReward));
	}

	// 工会副本怪物列表
	for (int i = 0; i < pPBGuildInfo->camp_monster_list_size(); ++i)
	{
		const ::PB::GuildCampMonster &tPBCampMonster = pPBGuildInfo->camp_monster_list(i);
		// 击杀者
		m_KillerList[tPBCampMonster.camp()] = tPBCampMonster.killer_name();
		vector<MonsterData> &vMonsterList = m_arMonsterList[tPBCampMonster.camp()];
		for (int k = 0; k < tPBCampMonster.monster_list_size(); ++k)
		{
			const ::PB::MonsterData &tPBMonster = tPBCampMonster.monster_list(k);
			MonsterData tMonster;
			tMonster.byIndex = tPBMonster.idx();
			tMonster.dwDamage = tPBMonster.damage();
			tMonster.dwTotalHP = tPBMonster.total_hp();
			vMonsterList.push_back(tMonster);
		}
	}

	// 创建怪物时间
	m_dwCreateMonterTime = pPBGuildInfo->create_monster_time();

	MAPLOG(GUI_LEVEL, "成功加载工会[GuildId:%d]!", GetGuildId());

	// todo::从新刷新排行榜,暂时这么处理
	CRankManager::Instance()->AddGuildRankData(m_dwSeverId, GetGuildId(), GUILD_RANK_LEVEL, GetGuildLevel());
}

// 是否有红点
bool CGuild::IsHaveRedPoint(Player *pPlayer)
{
	const GuildMember *pMember = GetMemberData(pPlayer->GetRoleId());
	if (!pMember)
		return false;

	for (BYTE i = 0; i < GUILD_RED_POINT_MAX; ++i)
	{
		if (IsHaveRedPoint(pPlayer, pMember, i))
			return true;
	}

	return false;
}

// 是否有红点
bool CGuild::IsHaveRedPoint(Player *pPlayer, const GuildMember *pMember, BYTE byType)
{
	// 入会申请列表有新的申请显示红点
	if (GUILD_RED_POINT_APPLY == byType)
	{
		if (m_ApplyList.size() && pMember->byPosition != GUILD_POS_MEMBER)
			return true;
	}
	// 限时商店刷新后，限时页签，商店建筑显示红点，进入限时页签后红点消失
	else if (GUILD_RED_POINT_LIMIT_SHOP == byType)
	{
		if (pMember->bIsLimitShopRefresh)
			return true;
	}
	// 奖励商店解锁新的物品购买时，奖励页签，商店建筑显示红点，进入奖励页签红点消失
	else if (GUILD_RED_POINT_REWARD_SHOP == byType)
	{
		const vector<DWORD> &vGoodsList = g_pCfgMgr->GetGuildRewardGoodsIdList();
		for (size_t i = 0; i < vGoodsList.size(); ++i)
		{
			DWORD dwGoodsId = vGoodsList[i];
			const ShopGoodsData_Config *pGoodsCfg = g_pCfgMgr->GetShopGoods(SHOP_GUILD_AWARD, dwGoodsId);
			// 角色等级满足,工会等级满足
			if (pGoodsCfg && pPlayer->GetLevel() >= pGoodsCfg->wNeedRoleLevel && GetGuildLevel() >= pGoodsCfg->wNeedGuildLevel)
			{
				// 还没购买 或 还可以购买
				const ShopGoodsBuyRecord *pBuyRecord = pPlayer->GetShopManager()->GetGoodsBuyRecordEx(SHOP_GUILD_AWARD, dwGoodsId);
				if (!pBuyRecord || pBuyRecord->wBuyNum < pGoodsCfg->wBuyNumMax)
					return true;
			}
		}
	}
	// 军团有可剩余建设次数时，在军团建设建筑显示红点
	else if (GUILD_RED_POINT_QY == byType)
	{
		if (!pMember->byQYType)
			return true;
	}
	// 留言板内有新的留言，在留言按钮上显示红点，打开后红高点消失
	else if (GUILD_RED_POINT_LEAVE_MESSAGE == byType)
	{
		if (pMember->bHaveNewLeaveMessage)
			return true;
	}
	// 军团技能建筑中，当前剩余贡献值至少可学习/升级任意一次技能时，在军团技能建筑显示红点
	if (GUILD_RED_POINT_SKILL == byType)
	{
		// 还没到达开放等级
		const GuildBuilding_Config *pBuildingCfg = g_pCfgMgr->GetGuildBuilding(5);
		if (pBuildingCfg && GetGuildLevel() < pBuildingCfg->wNeedGuildLevel)
			return false;

		// 个人学习
		for (size_t i = 0; i < m_SkillList.size(); ++i)
		{
			WORD wSkillId = m_SkillList[i].wSkillId;
			WORD wSkillLevel = m_SkillList[i].wLevel;;
			const SkillData *pSkillData = pPlayer->GetGuildSkill(wSkillId);
			// 还没学习,或者可升级
			if (!pSkillData || pSkillData->wLevel < wSkillLevel)
			{
				const GuildSkill_Config *pSkillCfg = g_pCfgMgr->GetGuildSkill(wSkillId, pSkillData ? pSkillData->wLevel + 1 : 1);
				// 贡献值足够
				if (pSkillCfg && pPlayer->GetProperty(ROLE_PRO_GUILD_CONSTIBUTION) > pSkillCfg->GetCostType(ROLE_PRO_GUILD_CONSTIBUTION))
					return true;
			}
		}

		// 工会学习
		if (pMember->byPosition != GUILD_POS_MEMBER)
		{
			const map<DWORD, GuildSkill_Config> &mapSkillList = g_pCfgMgr->GetGuildSkillList();
			map<DWORD, GuildSkill_Config>::const_iterator iter = mapSkillList.begin();
			for (; iter != mapSkillList.end(); ++iter)
			{
				// 工会经验足够
				if (m_tData.dwExp >= iter->second.nCostGuildExp && m_tData.wLevel >= iter->second.wNeedGuildLevel)
				{
					const SkillData *pSkill = GetSkillData(iter->second.wSkillId); //pPlayer->GetGuildSkill(iter->second.wSkillId);
					// 还没学习,或者可升级
					if (!pSkill || pSkill->wLevel < iter->second.wLevel)
						return true;
				}
			}
		}
	}
	// 公会副本挑战
	else if (GUILD_RED_POINT_FB == byType)
	{
		// 还没到达开放等级
		const GuildBuilding_Config *pBuildingCfg = g_pCfgMgr->GetGuildBuilding(3);
		if (pBuildingCfg && GetGuildLevel() < pBuildingCfg->wNeedGuildLevel)
			return false;

		// 更新挑战次数
		GuildMember *pTemp = FindMemberData(pPlayer->GetRoleId());
		if (pTemp)
			UpdateBattleTimes(pTemp);

		// 开放时间
		if (IsFBOpenTime() && pMember->byBattleTimes < pMember->byBuyBattleTimes + g_GetConfigValue(133))
			return true;
	}
	// 公会章节奖励
	else if (GUILD_RED_POINT_CHAPTER_REWARD == byType)
	{
		PlayerGuildData &tPlayerGuildData = pPlayer->GetRoleFullInfo().tSecondInfo.tGuildData;

		// 如果已经领取完全部(大部分人的情况应该是这样的,减少消耗)
		if (m_tData.wCurPassChapter == tPlayerGuildData.wTakeChapterRewardRecordNum)
			return false;

		for (WORD i = 0; i <= m_tData.wCurPassChapter; ++i)
		{
			bool isTake = false;
			// 检测是否已领取
			for (WORD k = 0; k < tPlayerGuildData.wTakeChapterRewardRecordNum; ++k)
			{
				if (tPlayerGuildData.arTakeChapterRewardRecord[k] == i)
				{
					isTake = true;
					break;
				}
			}


			// 还没有领取过
			if (!isTake)
				return true;
		}
	}
	// 公会关卡奖励
	else if (GUILD_RED_POINT_MAP_REWARD == byType)
	{
		// 检测已经通过的章节是否有奖励可领取
		for (WORD i = m_tData.wTodayStartChapter + 1; i <= m_tData.wCurPassChapter; ++i)
		{
			map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(i);
			if (iter != m_MapRewardRecordList.end())
			{
				for (WORD k = 0; k < CAMP_NUM; ++k)
				{
					set<DWORD> &playerList = iter->second.arAlreadyGetPlayerList[k];
					// 只要有一个阵营的还没领取
					if (playerList.find(pPlayer->GetRoleId()) == playerList.end())
						return true;
				}
			}
			// 还没有人领取过
			else
			{
				return true;
			}
		}

		// 检测正在挑战的章节奖励是否可领取
		map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(m_tData.wCurPassChapter + 1);
		for (int i = CAMP_BEGIN; i <= CAMP_END; ++i)
		{
			// 此阵营的怪物已挂掉
			if (IsMonsterDead(i))
			{
				// 此章节奖励还没领取
				if (iter == m_MapRewardRecordList.end())
					return true;

				// 此阵营的奖励还没领取
				set<DWORD> &playerList = iter->second.arAlreadyGetPlayerList[i];
				if (playerList.find(pPlayer->GetRoleId()) == playerList.end())
					return true;
			}
		}
	}

	return false;
}

// 战斗开始
bool CGuild::OnBattleStart(Player *pPlayer, BYTE byCamp)
{
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的工会数据!", __FUNCTION__);
		return false;
	}

	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]挑战军团副本阵营出错![camp:%d]", __FUNCTION__, byCamp);
		return false;
	}

	// 挑战目标章节
	WORD wBattleTargetChapter = m_tData.wCurPassChapter + 1;

	const GuildFBChapter_Config *pChapterCfg = g_pCfgMgr->GetGuildFB(wBattleTargetChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到军团副本数据![chapter:%d]", __FUNCTION__, wBattleTargetChapter);
		return false;
	}

	if (pChapterCfg->vMapList.size() != CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]军团副本配置的关卡列表出错![map_size:%d]", __FUNCTION__, pChapterCfg->vMapList.size());
		return false;
	}

	// 是否开放时间
	if (!IsFBOpenTime())
	{
		MAPLOG(ERROR_LEVEL, "[%s]军团副本非开放时间!", __FUNCTION__);
		return false;
	}

	// 更新挑战次数
	UpdateBattleTimes(pMember);

	// 检测挑战次数
	if (pMember->byBattleTimes >= pMember->byBuyBattleTimes + g_GetConfigValue(133))
	{
		MAPLOG(ERROR_LEVEL, "[%s]挑战军团副本次数已达上限!", __FUNCTION__);
		return false;
	}

	// 检测是否已挂掉
	if (IsMonsterDead(byCamp))
	{
		MAPLOG(ERROR_LEVEL, "[%s]军团副本怪物已挂掉(Camp:%d)!", __FUNCTION__,byCamp);
		return false;
	}

	pMember->byBattleTimes++;
	pMember->byTodayBattleTimes++;

	vector<MonsterData> *pMonsterList = &m_arMonsterList[byCamp];

	// 添加怪物数据
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	for (size_t i = 0; i < pMonsterList->size(); ++i)
	{
		pFBMgr->AddMonsterData(pMonsterList->at(i));
	}

	pFBMgr->SetBattleInfo(wBattleTargetChapter, byCamp, pChapterCfg->vMapList[byCamp]);

	return true;
}

// 战斗结束
bool CGuild::OnBattleOver(Player *pPlayer, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	BYTE byCamp = pFBMgr->GetMapIdx();
	WORD wBattleChapter = pFBMgr->GetCurChapter();

	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]挑战军团副本阵营出错![camp:%d]", __FUNCTION__, byCamp);
		return false;
	}

	const GuildFBChapter_Config *pChapterCfg = g_pCfgMgr->GetGuildFB(wBattleChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到军团副本数据![chapter:%d]", __FUNCTION__, wBattleChapter);
		return false;
	}

	// 挑战奖励(军团贡献)
	AddContribution(pPlayer->GetRoleId(), pChapterCfg->nBattleContribution);
	pPlayer->ModifyProperty(ROLE_PRO_GUILD_CONSTIBUTION, pChapterCfg->nBattleContribution);

	DWORD dwCurHP = 0;
	bool bIsDeadBefore = IsMonsterDead(byCamp);

	// 防止挑战结束时,章节已经往别的章节走了(因为有可能同时有多个玩家在挑战)
	if (wBattleChapter == m_tData.wCurPassChapter + 1 && !bIsDeadBefore)
	{
		UpdateMonsterDamage(byCamp, pMonsterList, byMonsterNum);

		dwCurHP = GetMonsterCurHP(byCamp);

		// 击杀奖励
		if (IsMonsterDead(byCamp))
		{
			// 贡献
			AddContribution(pPlayer->GetRoleId(), pChapterCfg->nKillContribution);
			pPlayer->ModifyProperty(ROLE_PRO_GUILD_CONSTIBUTION, pChapterCfg->nKillContribution);

			// 经验
			AddExp(pChapterCfg->nKillExp);

			// 记录击杀者名称
			m_KillerList[byCamp] = pPlayer->GetRoleName();

			pFBMgr->SetIsMeKill(true);
		}

		// 是否所有的阵营的怪物都挂掉
		if (IsFBOpenTime() && IsAllCampMonsterDead())
		{
			// 进入下一章节
			m_tData.wCurPassChapter++; 

			// 记录历史记录
			m_tData.wHistoryChapterRecord = max(m_tData.wHistoryChapterRecord, m_tData.wCurPassChapter);

			CreateMonster(false);
		}
	}

	// 伤害排行
	DWORD dwTotalDamage = 0;
	for (BYTE i = 0; i < byMonsterNum; ++i)
	{
		dwTotalDamage += pMonsterList[i].dwDamage;
	}
	AddFBDamageRecord(pPlayer->GetRoleId(),dwTotalDamage);

	SaveDataToDB();

	// 如果之前已挂掉,则不需要广播
	if (!bIsDeadBefore)
	{
		MSG_S2C_GUILD_MONSNTER_DAMAGE_NOTIFY msg;
		msg.m_byCamp = byCamp;
		msg.m_wChapter = wBattleChapter;
		msg.m_dwCurTotalHP = dwCurHP;
		msg.m_dwDamage = dwTotalDamage;
		strcpy_s(msg.m_szAttackName, sizeof(msg.m_szAttackName) - 1, pPlayer->GetRoleName());
		SendMsgToAllMember(&msg);
	}

	return true;
}

// 发送工会副本数据
void CGuild::SendCopyData(Player *pPlayer)
{
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的工会数据!", __FUNCTION__);
		return;
	}

	// 如果是活动期间,且还没创建怪物
	if (IsFBOpenTime() && !IsTheSameDay(m_dwCreateMonterTime, time(NULL)))
	{
		CreateMonster(true);
	}

	// 更新挑战次数
	UpdateBattleTimes(pMember);

	DWORD dwTotalHP = 0;
	DWORD dwDamage = 0;
	for (int i = 0; i < CAMP_NUM; ++i)
	{
		vector<MonsterData> &vMonsterList = m_arMonsterList[i];
		for (size_t k = 0; k < vMonsterList.size(); ++k)
		{
			dwTotalHP += vMonsterList[k].dwTotalHP;
			dwDamage += vMonsterList[k].dwDamage;
		}
	}

	MSG_S2C_GUILD_FB_DATA_RESP msg;
	msg.m_dwTotalHP = dwTotalHP;
	msg.m_dwDamage = dwDamage;
	msg.m_FBData.byBattleTimes = pMember->byBattleTimes;
	msg.m_FBData.byBuyTimes = pMember->byBuyBattleTimes;
	msg.m_FBData.wCurPassChapter = m_tData.wCurPassChapter;
	msg.m_FBData.dwLastBattleTimesRecoverTime = pMember->dwLastBattleTimesRecoverTime;
	msg.m_FBData.wIsFromHistoryChapter = m_tData.wIsFromHistoryChapter;
	msg.m_FBData.wTodayStartChapter = m_tData.wTodayStartChapter;
	msg.m_FBData.vTakeChapterRewardRecord.clear();
	PlayerGuildData &tGuildData = pPlayer->GetRoleFullInfo().tSecondInfo.tGuildData;
	for (WORD i = 0; i < tGuildData.wTakeChapterRewardRecordNum; ++i)
	{
		msg.m_FBData.vTakeChapterRewardRecord.push_back(tGuildData.arTakeChapterRewardRecord[i]);
	}
	pPlayer->SendPacket(&msg);
}


// 领取副本章节奖励
void CGuild::OnNetCopyChapterReward(Player *pPlayer, WORD wChapter)
{
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的工会数据!", __FUNCTION__);
		return;
	}

	// 检测是否可领取
	if (/*wChapter < m_tData.wTodayStartChapter ||*/ wChapter > m_tData.wCurPassChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]此章节不可领取奖励[chapter:%d]!", __FUNCTION__, wChapter);
		return;
	}

	PlayerGuildData &tPlayerGuildData = pPlayer->GetRoleFullInfo().tSecondInfo.tGuildData;

	// 检测是否已领取
	for (WORD i = 0; i < tPlayerGuildData.wTakeChapterRewardRecordNum; ++i)
	{
		if (tPlayerGuildData.arTakeChapterRewardRecord[i] == wChapter)
		{
			MAPLOG(ERROR_LEVEL, "[%s]军团副本的章节奖励已领取过!", __FUNCTION__);
			return;
		}
	}

	// 检测列表是否已满
	if (tPlayerGuildData.wTakeChapterRewardRecordNum >= MAX_GUILD_FB_CHAPTER_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]军团副本章节奖励记录列表已满!", __FUNCTION__);
		return;
	}

	const GuildFBChapter_Config *pFBCfg = g_pCfgMgr->GetGuildFB(wChapter);
	if (!pFBCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到军团副本配置[chaper:%d]!", __FUNCTION__, wChapter);
		return;
	}

	// 发奖励
	pPlayer->AddObjectList(pFBCfg->vChapterRewardList, TRUE, FROM_GUILD_FB_CHAPTER_REWARD);

	// 记录已领取
	tPlayerGuildData.arTakeChapterRewardRecord[tPlayerGuildData.wTakeChapterRewardRecordNum++] = wChapter;

	// 主动同步数据
	SendCopyData(pPlayer);

	// 响应
	MSG_S2C_GUILD_FB_CHAPTER_REWARD_RESP msg;
	msg.m_wChapter = wChapter;
	pPlayer->SendPacket(&msg);

	SaveDataToDB();
}

// 领取副本关卡奖励
void CGuild::OnNetCopyMapReward(Player *pPlayer, WORD wChapter, BYTE byCamp, BYTE byBoxIdx)
{
	WORD wFromMsgId = C2S_GUILD_FB_MAP_REWARD_REQ;

	// 检测阵营
	if (byCamp >= CAMP_NUM)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]领取军团关卡奖励阵营越界[Camp:%d]!", __FUNCTION__, byCamp);
		return;
	}

	// 只能领取今天挑战过的
	if (wChapter <= m_tData.wTodayStartChapter)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]此章节不是今日挑战的不可领取[chapter:%d]!", __FUNCTION__, wChapter);
		return;
	}

	// 还没通关的章节不能领取
	if (wChapter > m_tData.wCurPassChapter + 1)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]领取的章节还木有通关[chapter:%d]!", __FUNCTION__, wChapter);
		return;
	}

	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (!pMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的工会数据!", __FUNCTION__);
		return;
	}

	// 检测阵营的怪物是否已死亡(必需死亡方可领取)
	if (m_tData.wCurPassChapter + 1 == wChapter && !IsMonsterDead(byCamp))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]军团副本阵营的怪物还木有挂掉,还不能领取[Camp:%d]!", __FUNCTION__, byCamp);
		return;
	}

	const GuildFBChapter_Config *pFBCfg = g_pCfgMgr->GetGuildFB(wChapter);
	if (!pFBCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到军团副本配置[chaper:%d]!", __FUNCTION__, wChapter);
		return;
	}

	// 取宝箱随机列表(todo::这个逻辑的有点消耗,日后抽空优化)
	vector<WORD> vRandBoxIdxList;
	map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(wChapter);
	if (iter != m_MapRewardRecordList.end())
	{
		vector<GuildFBBox> &vTakeBoxList = iter->second.vTakeBoxList[byCamp];
		for (size_t i = 0; i < pFBCfg->vMapRewardList.size(); ++i)
		{
			bool bIsTake = false;
			for (size_t k = 0; k < vTakeBoxList.size(); ++k)
			{
				if (i == vTakeBoxList[k].wBoxIdx)
				{
					bIsTake = true;
					break;
				}
			}

			// 只取还木有领取过的
			if (!bIsTake)
				vRandBoxIdxList.push_back(i);
		}
	}
	else
	{
		for (size_t i = 0; i < pFBCfg->vMapRewardList.size(); ++i)
		{
			vRandBoxIdxList.push_back(i);
		}
	}

	// 检测奖励列表是否已领取完
	if (!vRandBoxIdxList.size())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]军团关卡奖励列表已领取完[chaper:%d,camp:%d]!", __FUNCTION__, wChapter, byCamp);
		return;
	}
	
	if (iter != m_MapRewardRecordList.end())
	{
		// 检测个人是否已领取
		set<DWORD> &sTakePlayerList = iter->second.arAlreadyGetPlayerList[byCamp];
		if (sTakePlayerList.find(pPlayer->GetRoleId()) != sTakePlayerList.end())
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]已经领取过军团副本关卡奖励[chapter:%d,camp:%d]!", __FUNCTION__, wChapter, byCamp);
			return;
		}
	}

	// 检测宝箱索引
	bool bIsExist = false;
	for (size_t i = 0; i < vRandBoxIdxList.size(); ++i)
	{
		if (vRandBoxIdxList[i] == byBoxIdx)
		{
			bIsExist = true;
			break;
		}
	}
	if (!bIsExist)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]领取军团关卡奖励宝箱索引不存在[BoxIdx:%d]!", __FUNCTION__, byBoxIdx);
		return;
	}

	// 随机奖励
	WORD wRewardIdx = vRandBoxIdxList[g_Rand(0, vRandBoxIdxList.size()-1)];

	// 发奖励
	pPlayer->AddObjectData(&pFBCfg->vMapRewardList[wRewardIdx], TRUE, FROM_GUILD_FB_LEVEL_REWARD);

	// 添加奖励记录
	GuildFBBox box;
	box.wBoxIdx = byBoxIdx;
	box.wRewardIdx = wRewardIdx;
	strcpy_s(box.szRoleName, sizeof(box.szRoleName) - 1, pPlayer->GetRoleName());
	if (iter != m_MapRewardRecordList.end())
	{
		iter->second.vTakeBoxList[byCamp].push_back(box);
		iter->second.arAlreadyGetPlayerList[byCamp].insert(pPlayer->GetRoleId());
	}
	else
	{
		GuildFBMapReward tMapReward;
		tMapReward.vTakeBoxList[byCamp].push_back(box);
		tMapReward.arAlreadyGetPlayerList[byCamp].insert(pPlayer->GetRoleId());
		m_MapRewardRecordList.insert(make_pair(wChapter, tMapReward));
	}

	// 响应
	MSG_S2C_GUILD_FB_MAP_REWARD_RESP msg;
	msg.m_wChapter = wChapter;
	msg.m_byCamp = byCamp;
	msg.m_byBoxIdx = byBoxIdx;
	msg.m_byRewardIdx = wRewardIdx;
	pPlayer->SendPacket(&msg);

	SaveDataToDB();
}

// 请求关卡奖励记录
void CGuild::OnNetCopyMapRewardRecord(Player *pPlayer, WORD wChapter, BYTE byCamp)
{
	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵营参数越界[Camp:%d]!", __FUNCTION__, byCamp);
		return;
	}

	MSG_S2C_GUILD_FB_MAP_REWARD_RECORD_RESP msg;
	msg.m_wChapter = wChapter;
	msg.m_byCamp = byCamp;
	msg.m_wRecordNum = 0;

	// 计算关卡奖励状态
// 	for (int i = 0; i < CAMP_NUM; ++i)
// 	{
// 		BYTE byStatus = 0; // 默认未领取
// 		if (wChapter < m_tData.wCurPassChapter || IsMonsterDead(i))
// 		{
// 			byStatus = 1; // 通关后为可领取奖励
// 			map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(wChapter);
// 			if (iter != m_MapRewardRecordList.end())
// 			{
// 				set<DWORD> &tTakePlayerList = iter->second.arAlreadyGetPlayerList[i];
// 				if (tTakePlayerList.find(pPlayer->GetRoleId()) != tTakePlayerList.end())
// 				{
// 					byStatus = 2; // 已经领取过了
// 				}
// 			}
// 		}
// 		msg.m_RewardStatusList[i] = byStatus;
// 	}

	map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(wChapter);
	if (iter != m_MapRewardRecordList.end())
	{
		for (size_t i = 0; i < iter->second.vTakeBoxList[byCamp].size(); ++i)
		{
			msg.m_RecordList[msg.m_wRecordNum++] = iter->second.vTakeBoxList[byCamp][i];
		}
	}

	pPlayer->SendPacket(&msg);
}

// 请求成员副本战绩
void CGuild::OnNetCopyBattleRecord(Player *pPlayer)
{
	MSG_S2C_GUILD_FB_DAMAGE_RECORD_RESP msg;
	msg.m_wDamageNum = 0;

	map<DWORD, DWORD>::iterator iter = m_DamageRecordList.begin();
	for (; iter != m_DamageRecordList.end(); ++iter)
	{
		GuildMember *pMember = FindMemberData(iter->first);
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), iter->first, false);
		if (pBaseData && pMember)
		{
			GuildFBDamageData *pDamageData = &msg.m_DamageList[msg.m_wDamageNum++];
			memcpy_s(pDamageData, sizeof(PlayerCommonData), pBaseData, sizeof(PlayerCommonData));
			pDamageData->dwDamage = iter->second;
			pDamageData->byBattleTimes = pMember->byTodayBattleTimes;
		}
	}

	pPlayer->SendPacket(&msg);
}

// 购买副本挑战次数
void CGuild::OnNetBuyCopyBattleTimes(Player *pPlayer, BYTE byBuyTimes)
{
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的工会数据!", __FUNCTION__);
		return;
	}

	// 检测购买次数上限
	if (pMember->byBuyBattleTimes + byBuyTimes > pPlayer->GetVipValue(VIP_GUILD_FB_BUY_BATTLE_TIMES))
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买军团副本挑战次数已达上限[BuyTimes:%d]!",
			__FUNCTION__, pMember->byBuyBattleTimes);
		return;
	}

	// 检测购买消耗
	const Server_Config *pSvrCfg = g_pCfgMgr->GetCommonCfg(69);
	if (!pSvrCfg && !pSvrCfg->vValueList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买军团副本挑战次数消耗配置错误!",__FUNCTION__);
		return;
	}
	int nCostDiamond = 0;
	BYTE byBegin = pMember->byBuyBattleTimes;
	BYTE byEnd = byBegin + byBuyTimes;
	for (BYTE i = byBegin; i < byEnd; ++i)
	{
		// 如果次数超过配置的次数,则取最后一个
		int nIndex = min(i, pSvrCfg->vValueList.size()-1);
		nCostDiamond += pSvrCfg->vValueList[nIndex];
	}
	if (nCostDiamond > pPlayer->GetProperty(ROLE_PRO_DIAMOND))
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买军团副本挑战次数消耗元宝不足!", __FUNCTION__);
		return;
	}

	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_BUY_GUILD_COPY_BATTLE_TIMES);

	pMember->byBuyBattleTimes += byBuyTimes;

	SendCopyData(pPlayer);

	// 响应
	MSG_S2C_GUILD_FB_BATTLE_TIMES_BUY_RESP msg;
	msg.m_byBuyTimes = byBuyTimes;
	pPlayer->SendPacket(&msg);

	SaveDataToDB();
}

// 副本章节重置
void CGuild::OnNetFBChapterReset(Player *pPlayer, BYTE byIsHistoryRecord)
{
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]木有找到成员数据[role_id:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 普通成员木有权限
	if (GUILD_POS_MEMBER == pMember->byPosition)
	{
		MAPLOG(ERROR_LEVEL, "[%s]成员木有权限重置副本章节[role_id:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	m_tData.wIsFromHistoryChapter = byIsHistoryRecord;

	MSG_S2C_GUILD_FB_CHAPTER_RESET_RESP msg;
	msg.m_byIsHistoryRecord = byIsHistoryRecord;
	pPlayer->SendPacket(&msg);
}

// 请求章节关卡奖励状态
void CGuild::OnNetMyMapRewardRecord(Player *pPlayer)
{
	MSG_S2C_GUILD_MY_MAP_REWARD_RECORD_RESP msg;
	msg.m_wRecordNum = 0;

	// 已经通关的记录
	for (WORD i = m_tData.wTodayStartChapter+1; i <= m_tData.wCurPassChapter; ++i)
	{
		if (msg.m_wRecordNum >= MAX_GUILD_CHAPTER_NUM)
			break;

		MyGuildMapRewardRecord &record = msg.m_RewardRecordList[msg.m_wRecordNum++];
		memset(&record, 0, sizeof(record));
		record.wChapter = i;

		map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(i);
		if (iter != m_MapRewardRecordList.end())
		{
			for (WORD k = 0; k < CAMP_NUM; ++k)
			{
				set<DWORD> &playerList = iter->second.arAlreadyGetPlayerList[k];
				record.arRecordList[k] = playerList.find(pPlayer->GetRoleId()) != playerList.end() ? true : false;
			}
		}
	}

	// 正在挑战的记录
	if (msg.m_wRecordNum < MAX_GUILD_CHAPTER_NUM)
	{
		MyGuildMapRewardRecord &record = msg.m_RewardRecordList[msg.m_wRecordNum++];
		memset(&record, 0, sizeof(record));
		record.wChapter = m_tData.wCurPassChapter+1;
		map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(record.wChapter);
		if (iter != m_MapRewardRecordList.end())
		{
			for (WORD k = 0; k < CAMP_NUM; ++k)
			{
				set<DWORD> &playerList = iter->second.arAlreadyGetPlayerList[k];
				record.arRecordList[k] = playerList.find(pPlayer->GetRoleId()) != playerList.end() ? true : false;
			}
		}
	}

	pPlayer->SendPacket(&msg);
}

// 创建副本怪物
void CGuild::CreateMonster(bool bResetMemberData)
{
	m_DamageRecordList.clear();
	for (BYTE i = 0; i < CAMP_NUM; ++i)
	{
		m_arMonsterList[i].clear();
		m_KillerList[i].clear();
	}

	const GuildFBChapter_Config *pChapterCfg = g_pCfgMgr->GetGuildFB(m_tData.wCurPassChapter + 1);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到工会副本数据![%d]", __FUNCTION__, m_tData.wCurPassChapter + 1);
		return;
	}

	if (pChapterCfg->vMapList.size() != CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]军团副本关卡列表配置错误![map_size:%d]", __FUNCTION__, pChapterCfg->vMapList.size());
		return;
	}

	for (size_t i = 0; i < pChapterCfg->vMapList.size(); ++i)
	{
		DWORD dwMapId = pChapterCfg->vMapList[i];
		g_pCfgMgr->GetMapMonsterList(dwMapId, &m_arMonsterList[i]);
	}

	// 初始化成员挑战副本相关数据
	if (bResetMemberData)
	{
		DWORD dwCurTime = time(NULL);
		for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
		{
			m_tData.arMemberList[i].dwLastBattleTimesRecoverTime = dwCurTime;
			m_tData.arMemberList[i].byBattleTimes = 0;
			m_tData.arMemberList[i].byBuyBattleTimes = 0;
		}
	}

	m_dwCreateMonterTime = time(NULL);

	MAPLOG(GUI_LEVEL, "[%s]创建军团副本怪物成功![chapter:%d]", __FUNCTION__, m_tData.wCurPassChapter + 1);
}

// 是否怪物已死亡
bool CGuild::IsMonsterDead(BYTE byCamp)
{
	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵营越界![Camp:%d]", __FUNCTION__, byCamp);
		return true;
	}

	vector<MonsterData> *pCampMonsterList = &m_arMonsterList[byCamp];

	for (size_t i = 0; i < pCampMonsterList->size(); ++i)
	{
		if (pCampMonsterList->at(i).dwDamage < pCampMonsterList->at(i).dwTotalHP)
			return false;
	}

	return true;
}

// 是否所有阵营怪物都挂掉
bool CGuild::IsAllCampMonsterDead()
{
	for (BYTE i = 0; i < CAMP_NUM; ++i)
	{
		if (!IsMonsterDead(i))
			return false;
	}
	return true;
}

// 更新怪物伤害
void CGuild::UpdateMonsterDamage(BYTE byCamp, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵营越界![Camp:%d]", __FUNCTION__, byCamp);
		return;
	}

	vector<MonsterData> *pUpdateMonsterList = &m_arMonsterList[byCamp];

	for (BYTE i = 0; i < byMonsterNum; ++i)
	{
		vector<MonsterData>::iterator iter = pUpdateMonsterList->begin();
		for (; iter != pUpdateMonsterList->end(); ++iter)
		{
			if (pMonsterList[i].byIndex == iter->byIndex)
			{
				iter->dwDamage += pMonsterList[i].dwDamage;
				break;
			}
		}
	}
}

// 当前怪物HP
DWORD CGuild::GetMonsterCurHP(BYTE byCamp)
{
	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵营越界![Camp:%d]", __FUNCTION__, byCamp);
		return 0;
	}

	DWORD dwCurHP = 0;

	vector<MonsterData> *pMonsterList = &m_arMonsterList[byCamp];
	for (vector<MonsterData>::iterator iter = pMonsterList->begin(); iter != pMonsterList->end(); ++iter)
	{
		if (iter->dwTotalHP > iter->dwDamage)
			dwCurHP += (iter->dwTotalHP - iter->dwDamage);
	}

	return dwCurHP;
}

// 是否副本开放时间
bool CGuild::IsFBOpenTime()
{
	const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(132, true);
	if (pServerCfg && pServerCfg->vValueList.size() == 2)
	{
		int nBeginTime = pServerCfg->vValueList[0];
		int nEndTime = pServerCfg->vValueList[1];
		tm tmNow;
		GetTMTime(&tmNow, time(NULL));
		int nCurTime = tmNow.tm_hour * 100 + tmNow.tm_min;
		if (nCurTime >= nBeginTime && nCurTime <= nEndTime)
			return true;
	}
	return false;
}

// 添加副本伤害记录
void CGuild::AddFBDamageRecord(DWORD dwRoleId, DWORD dwDamage)
{
	map<DWORD, DWORD>::iterator iter = m_DamageRecordList.find(dwRoleId);
	if (iter != m_DamageRecordList.end())
	{
		iter->second = max(iter->second, dwDamage);
	}
	else
	{
		m_DamageRecordList.insert(make_pair(dwRoleId, dwDamage));
	}
}

// 取会长名称
const char* CGuild::GetCaptainName()
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		if (GUILD_POS_CAPTAIN == m_tData.arMemberList[i].byPosition)
		{
			const PlayerBaseData *pPBaseData = g_GetPlayerBaseData(m_dwSeverId, m_tData.arMemberList[i].dwRoleId, false);
			return pPBaseData ? pPBaseData->szRoleName : NULL;
		}
	}
	return NULL;
}

// 取会长ID
DWORD CGuild::GetCaptainId()
{
	const GuildMember *pMember =  GetCaptainData();
	return pMember ? pMember->dwRoleId : 0;
}

// 设置宣言
void CGuild::SetDeclaration(const char *pszDeclaration)
{ 
	strcpy_s(m_tData.szDeclaration, sizeof(m_tData.szDeclaration), pszDeclaration); 
};


// 设置公告
void CGuild::SetNotice(const char *pszNotice)
{ 
	strcpy_s(m_tData.szNotice, sizeof(m_tData.szNotice), pszNotice); 
};

// 添加经验
void CGuild::AddExp(DWORD dwExp)
{
	const Guild_Config *pGuildCfg = CConfigManager::Instance()->GetGuild(m_tData.wLevel);
	if (!pGuildCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到工会配置![level:%d]", __FUNCTION__, m_tData.wLevel);
		return;
	}

	m_tData.dwExp += dwExp;

	if (m_tData.dwExp >= pGuildCfg->dwNeedExp)
	{
		m_tData.dwExp -= pGuildCfg->dwNeedExp;
		m_tData.wLevel++;

		// 更新排行榜
		CRankManager::Instance()->AddGuildRankData(m_dwSeverId, GetGuildId(), GUILD_RANK_LEVEL, m_tData.wLevel);
	}
}

// 添加贡献值
void CGuild::AddContribution(DWORD dwRoleId, int nContribution)
{
	GuildMember *pMember = FindMemberData(dwRoleId);
	if (pMember)
	{
		pMember->dwTotalContribution += nContribution;
		pMember->dwTodayContribution += nContribution;

		// 1/100 算活跃
		AddActive(nContribution * 0.01);
	}
}


// 工会聊天
void CGuild::Chat(NetMsgBody *pMsg)
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(m_tData.arMemberList[i].dwRoleId);
		if (pPlayer)
		{
			pPlayer->SendPacket(pMsg);
		}
	}
}

// 添加成员
bool CGuild::AddMember(const GuildMember *pMember)
{
	if (m_tData.byMemberNum >= MAX_GUILD_MEMBER_NUM)
		return false;

	m_tData.arMemberList[m_tData.byMemberNum++] = *pMember;
	return true;
}

// 删除成员
bool CGuild::RemoveMember(DWORD dwRoleId)
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		if (m_tData.arMemberList[i].dwRoleId == dwRoleId)
		{
			m_tData.arMemberList[i] = m_tData.arMemberList[--m_tData.byMemberNum];
			return true;
		}
	}

	return false;
}

// 取成员数据
const GuildMember* CGuild::GetMemberData(DWORD dwRoleId)
{
	return FindMemberData(dwRoleId);
}

// 取会长信息
const GuildMember* CGuild::GetCaptainData()
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		if (m_tData.arMemberList[i].byPosition == GUILD_POS_CAPTAIN)
		{
			return &m_tData.arMemberList[i];
		}
	}

	return NULL;
}

// 填充工会数据
void CGuild::FillGuildData(GuildBaseData &tBaseData)
{
	memset(&tBaseData, 0, sizeof(tBaseData));
	tBaseData.dwId = m_tData.dwId;
	tBaseData.wIconId = m_tData.wIconId;
	tBaseData.wLevel = m_tData.wLevel;
	tBaseData.byMemberNum = GetMemberNum();
	tBaseData.byIsApply = false;
	tBaseData.byIsFull = IsMemberFull();
	if (GetCaptainName())
		strcpy_s(tBaseData.szCaptainName, sizeof(tBaseData.szCaptainName), GetCaptainName());
	strcpy_s(tBaseData.szGuildName, sizeof(tBaseData.szGuildName), m_tData.szName);
	strcpy_s(tBaseData.szDeclaration, sizeof(tBaseData.szDeclaration), m_tData.szDeclaration);
}

// 设置成员职位
void CGuild::SetMemberPos(DWORD dwRoleId, BYTE byPos)
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		if (m_tData.arMemberList[i].dwRoleId == dwRoleId)
		{
			m_tData.arMemberList[i].byPosition = byPos;
			break;
		}
	}
}

// 取成员职位
BYTE CGuild::GetMemberPos(DWORD dwRoleId)
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		if (m_tData.arMemberList[i].dwRoleId == dwRoleId)
			return m_tData.arMemberList[i].byPosition;
	}
	return GUILD_POS_INVALID;
}

// 取某个职位上的人数据
BYTE CGuild::GetMemberNumByPos(BYTE byPos)
{
	BYTE byMemberNum = 0;
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		if (m_tData.arMemberList[i].byPosition == byPos)
			byMemberNum++;
	}
	return byMemberNum;
}

// 取成员泉涌次数
BYTE CGuild::GetMemberQYType(DWORD dwRoleId)
{
	GuildMember *pMember = FindMemberData(dwRoleId);
	return pMember ? pMember->byQYType : 0;
}

// 添加成员泉涌次数
void CGuild::SetMemberQYType(DWORD dwRoleId, BYTE byType)
{
	GuildMember *pMember = FindMemberData(dwRoleId);
	if (pMember)
	{
		pMember->byQYType = byType;
	}
}

// 是否已领取了冰泉奖励
bool CGuild::IsGetBQReward(DWORD dwRoleId, WORD wQuanShui)
{
	GuildMember *pMember = FindMemberData(dwRoleId);
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到成员数据![RoleId:%d]", __FUNCTION__, dwRoleId);
		return false;
	}

	for (BYTE i = 0; i < pMember->byQSRewardRecordNum; ++i)
	{
		if (pMember->arQSRewardRecord[i] == wQuanShui)
			return true;
	}
	return false;
}

// 添加泉水奖励
void CGuild::AddQSRewardRecord(DWORD dwRoleId, WORD wQuanShui)
{
	GuildMember *pMember = FindMemberData(dwRoleId);
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到成员数据![RoleId:%d]", __FUNCTION__, dwRoleId);
		return;
	}

	if (pMember->byQSRewardRecordNum >= MAX_GUILD_QS_REWARD_RECORD)
	{
		MAPLOG(ERROR_LEVEL, "[%s]工会泉水奖励记录已满![RoleId:%d]", __FUNCTION__, dwRoleId);
		return;
	}

	pMember->arQSRewardRecord[pMember->byQSRewardRecordNum++] = wQuanShui;
}

// 取技能
const SkillData* CGuild::GetSkillData(WORD wSkillId)
{
	for (size_t i = 0; i < m_SkillList.size(); ++i)
	{
		if (m_SkillList[i].wSkillId == wSkillId)
			return &m_SkillList[i];
	}
	return NULL;
}

// 添加技能
void CGuild::AddSkillData(WORD wSkillId, WORD wSkillLevel)
{
	for (size_t i = 0; i < m_SkillList.size(); ++i)
	{
		if (m_SkillList[i].wSkillId == wSkillId)
		{
			m_SkillList[i].wLevel = wSkillLevel;
			return;
		}
	}

	SkillData tSkill;
	tSkill.wSkillId = wSkillId;
	tSkill.wLevel = wSkillLevel;
	m_SkillList.push_back(tSkill);
}

// 添加聊天记录
void CGuild::AddChatRecord(const ChatRecord &tRecord)
{
	m_ChatRecordList.push_back(tRecord);

	if (m_ChatRecordList.size() > MAX_CHAT_RECORD_COUNT)
		m_ChatRecordList.pop_front();
}

// 重置挑战次数 todo:: for test
void CGuild::ResetBattleTimes(Player *pPlayer)
{
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (pMember)
	{
		pMember->byBattleTimes = 0;
		pMember->byBuyBattleTimes = 0;
	}
	//SendCopyData(pPlayer);
}

void CGuild::ResetQYTimes(Player *pPlayer)
{
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (pMember)
	{
		pMember->byQYType = 0;
	}
}

// 给所有成员发奖励
void CGuild::SendRewardToAllMember(const RewardData &tRewardData)
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		const GuildMember &tMember = m_tData.arMemberList[i];
		DWORD dwServerId = PlayerBaseDataManager::Instance()->GetServerIdByRoleId(tMember.dwRoleId);
		CRewardCenterManager::AddReward(dwServerId, tMember.dwRoleId, tRewardData);
	}
}

// 发送工会数据
void CGuild::SendGuildData(Player *pPlayer)
{
	MSG_S2C_GUILD_DATE_RESP msg;
	memcpy_s(&msg.m_GuildData, sizeof(GuildData), &m_tData, sizeof(GuildData));
	msg.m_dwLastQYValue = 0;
	msg.m_byLastQYType = 0;
	memset(msg.m_szLastQYTargetName, 0, sizeof(msg.m_szLastQYTargetName));
	msg.m_byMemberNum = GetMemberNum();
	msg.m_byQYType = GetMemberQYType(pPlayer->GetRoleId());
	if (GetCaptainName())
		strcpy_s(msg.m_szCaptainName, sizeof(msg.m_szCaptainName) - 1, GetCaptainName());
	msg.m_byQSRewardRecordNum = 0;
	const GuildMember *pMember = GetMemberData(pPlayer->GetRoleId());
	if (pMember)
	{
		msg.m_byPosition = pMember->byPosition;
		msg.m_byQSRewardRecordNum = pMember->byQSRewardRecordNum;
		int nCopySize = pMember->byQSRewardRecordNum*sizeof(WORD);
		memcpy_s(msg.m_arQSRewardRecord, nCopySize, pMember->arQSRewardRecord, nCopySize);
	}
	
	// 检测是否当天有人泉水
	if (IsTheSameDay(m_tLastQYInfo.dwTime, time(NULL)))
	{
		msg.m_dwLastQYValue = m_tLastQYInfo.dwValue;
		msg.m_byLastQYType = m_tLastQYInfo.byQSType;
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_dwSeverId, m_tLastQYInfo.dwRoleId);
		if (pBaseData)
			strcpy_s(msg.m_szLastQYTargetName, sizeof(msg.m_szLastQYTargetName) - 1, pBaseData->szRoleName);
	}

	// 红点
	msg.m_wRedPointNum = 0;
	for (BYTE i = 0; i < GUILD_RED_POINT_MAX; ++i)
	{
		if (IsHaveRedPoint(pPlayer, pMember, i))
			msg.m_arRedPointList[msg.m_wRedPointNum++] = i;
	}

	pPlayer->SendPacket(&msg);
}

// 发送工会数据
void CGuild::SendGuildData(DWORD dwRoleId)
{
	Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId);
	if (pPlayer)
		SendGuildData(pPlayer);
}

// 发送成员列表
void CGuild::SendMemberList(Player *pPlayer)
{
	// todo::此处逻辑,日后必需要调整
	MSG_S2C_GUILD_MEMBER_LIST_RESP msg;
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		GuildMember &tMember = m_tData.arMemberList[i];
		ClientGuildMember &tClientMember = msg.m_GuildList[i];
		memcpy_s(&tClientMember, sizeof(GuildMember), &tMember, sizeof(GuildMember));
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), tMember.dwRoleId);
		if (pBaseData)
		{
			tClientMember.dwLastLogoutTime = pBaseData->dwOfflineTime;
			tClientMember.wLevel = pBaseData->wLevel;
			tClientMember.dwFightingPower = pBaseData->dwFightingPower;
			tClientMember.wHeadIconId = pBaseData->wHeadIcon;
			strcpy_s(tClientMember.szRoleName, sizeof(tClientMember.szRoleName)-1, pBaseData->szRoleName);
		}
	}
	msg.m_byMemberNum = m_tData.byMemberNum;
	pPlayer->SendPacket(&msg);
}

// 发送留言消息列表 
void CGuild::SendLeaveMsgList(Player *pPlayer)
{
	MSG_S2C_GUILD_LEAVEMSG_LIST_RESP msg;
	msg.m_byLeaveMsgNum = 0;
	list<GuildLeaveMessage>::iterator iter = m_MessageList.begin();
	for (; iter != m_MessageList.end(); ++iter)
	{
		if (msg.m_byLeaveMsgNum >= MAX_GUILD_LEAVEMSG_NUM)
			break;
		msg.m_LeaveMsgList[msg.m_byLeaveMsgNum++] = *iter;
	}
	pPlayer->SendPacket(&msg);
}

// 发送申请列表
void CGuild::SendApplyList(Player *pPlayer)
{
	MSG_S2C_GUILD_APPLY_LIST_RESP msg;
	msg.m_wApplyDataNum = 0;
	vector<GuildApplyData>::iterator iter = m_ApplyList.begin();
	for (; iter != m_ApplyList.end(); ++iter)
	{
		if (msg.m_wApplyDataNum >= MAX_GUILD_APPLY_NUM)
			break;

		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_dwSeverId, iter->dwRoleId);
		if (pBaseData)
			memcpy_s(&msg.m_ApplyDataList[msg.m_wApplyDataNum++], sizeof(PlayerCommonData), pBaseData, sizeof(PlayerCommonData));
	}
	pPlayer->SendPacket(&msg);
}

// 发送事件列表 
void CGuild::SendEventList(Player *pPlaye)
{
	MSG_S2C_GUILD_EVENT_LIST_RESP msg;
	msg.m_byEventNum = 0;
	for (list<GuildEvent>::iterator iter = m_EventList.begin(); iter != m_EventList.end(); ++iter)
	{
		if (msg.m_byEventNum >= MAX_GUILD_EVENT_NUM)
			break;
		msg.m_EventList[msg.m_byEventNum++] = *iter;
	}
	pPlaye->SendPacket(&msg);
}

// 解散
void CGuild::Dismiss()
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		DWORD dwRoleId = m_tData.arMemberList[i].dwRoleId;
		m_pGuildMgr->RemoveMember(dwRoleId);
		Player * pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId);
		if (pPlayer)
		{
			// todo::通知玩家,工会解散
		}
	}
}

// 工会成员是否已满
bool CGuild::IsMemberFull()
{
	const Guild_Config *pGuildCfg = CConfigManager::Instance()->GetGuild(m_tData.wLevel);
	if (!pGuildCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到工会配置![level:%d]", __FUNCTION__, m_tData.wLevel);
		return true;
	}
	return m_tData.byMemberNum >= pGuildCfg->wMaxMember ? true : false;
}

// 添加申请玩家数据
void CGuild::AddApplyData(Player *pPlayer)
{
	GuildApplyData applydata;
	applydata.dwRoleId = pPlayer->GetCharID();
	m_ApplyList.push_back(applydata);
}

// 删除申请玩家数据
void CGuild::RemoveApplyData(DWORD dwRoleId)
{
	vector<GuildApplyData>::iterator iter = m_ApplyList.begin();
	for (; iter != m_ApplyList.end(); ++iter)
	{
		if (iter->dwRoleId == dwRoleId)
		{
			m_ApplyList.erase(iter);
			break;
		}
	}
}

// 取申请玩家数据
GuildApplyData* CGuild::GetApplyData(DWORD dwRoleId)
{
	vector<GuildApplyData>::iterator iter = m_ApplyList.begin();
	for (; iter != m_ApplyList.end(); ++iter)
	{
		if (iter->dwRoleId == dwRoleId)
		{
			return (GuildApplyData*)&*iter;
		}
	}

	return NULL;
}

// 检测是否在申请列表
bool CGuild::IsInApplyList(DWORD dwRoleId)
{
	return GetApplyData(dwRoleId) ? true : false;
}

// 添加留言消息
void CGuild::AddLeaveMessage(const GuildLeaveMessage &sLeaveMsg)
{
	m_MessageList.push_front(sLeaveMsg);

	if (m_MessageList.size() > MAX_GUILD_LEAVEMESSAGE_NUM)
	{
		m_MessageList.pop_back();
	}

	// 更新新留言状态
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		m_tData.arMemberList[i].bHaveNewLeaveMessage = true;
	}
}

// 取商品购买记录
// const ShopGoodsBuyRecord* CGuild::GetGoodsBuyRecord(WORD wGoodsId)
// {
// 	map<WORD, ShopGoodsBuyRecord>::iterator iter = m_GoodsBuyRecordList.find(wGoodsId);
// 	if (iter != m_GoodsBuyRecordList.end())
// 	{
// 		return &iter->second;
// 	}
// 
// 	return NULL;
// }

// 添加商品购买记录
// bool CGuild::AddGoodsBuyRecord(WORD wGoodsId, WORD wGoodsNum)
// {
// 	map<WORD, ShopGoodsBuyRecord>::iterator iter = m_GoodsBuyRecordList.find(wGoodsId);
// 	if (iter != m_GoodsBuyRecordList.end())
// 	{
// 		iter->second.wBuyNum += wGoodsNum;
// 	}
// 	else
// 	{
// 		ShopGoodsBuyRecord sRecord;
// 		sRecord.wGoodsId = wGoodsId;
// 		sRecord.wBuyNum = wGoodsNum;
// 		m_GoodsBuyRecordList.insert(std::make_pair(wGoodsId, sRecord));
// 	}
// 	return true;
// }

// 刷新限时商品 
void CGuild::UpdateLimitGoods(vector<Goods> &vGoodsList)
{
	m_vLimitGoodsList = vGoodsList;

	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		m_tData.arMemberList[i].bIsLimitShopRefresh = true;
	}
}

// 添加事件
void CGuild::AddEvent(const GuildEvent &tEvent)
{
	m_EventList.push_back(tEvent);

	if (m_EventList.size() > MAX_GM_EVENT_NUM)
		m_EventList.pop_front();
}

// 添加事件
void CGuild::AddEvent(WORD wEventId, const char *pszTargetName, DWORD dwValue)
{
	GuildEvent tEvent;
	tEvent.wEventId = wEventId;
	tEvent.dwTime = time(NULL);
	tEvent.dwValue = dwValue;
	strcpy_s(tEvent.szTargetName, sizeof(tEvent.szTargetName) - 1, pszTargetName);
	AddEvent(tEvent);
}

// 设置最近泉水信息
void CGuild::SetLastQSInfo(DWORD dwRoleId, DWORD dwValue, BYTE byType)
{
	m_tLastQYInfo.byQSType = byType;
	m_tLastQYInfo.dwValue = dwValue;
	m_tLastQYInfo.dwRoleId = dwRoleId;
	m_tLastQYInfo.dwTime = time(NULL);
}

// 取限时商品
const Goods* CGuild::GetLimitGoods(WORD wGoodsId)
{
	for (size_t i = 0; i < m_vLimitGoodsList.size(); ++i)
	{
		if (m_vLimitGoodsList[i].wId == wGoodsId)
			return &m_vLimitGoodsList[i];
	}
	return NULL;
}

// 添加限时商品购买数量
void CGuild::AddLimitGoodsBuyNum(WORD wGoodsId, WORD wBuyNum)
{
	// 只有工会刷出的商品才能购买,所以,如果没有则不需要插入
	for (size_t i = 0; i < m_vLimitGoodsList.size(); ++i)
	{
		if (m_vLimitGoodsList[i].wId == wGoodsId)
		{
			m_vLimitGoodsList[i].wNum += wBuyNum;
			break;
		}
	}
}

bool RolePowerPred(RolePower &data1, RolePower &data2)
{
	if (data1.dwPower > data2.dwPower)
		return true;

	if (data1.dwPower == data2.dwPower && data1.dwRoleId < data2.dwRoleId)
		return true;

	return false;
}

// 取成员战力排行列表
void CGuild::GetMemeberPowerRankList(vector<RolePower> &vPlayerList)
{
	RolePower tRolePower;
	for (BYTE i = 0; i < m_tData.byMemberNum; i++)
	{
		const GuildMember &tMember = m_tData.arMemberList[i];
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(tMember.dwRoleId, false);
		if (pBaseData)
		{
			tRolePower.dwRoleId = tMember.dwRoleId;
			tRolePower.dwPower = pBaseData->dwFightingPower;
			vPlayerList.push_back(tRolePower);
			//MAPLOG(SPECAIL_LEVEL, "工会成员[GuildId:%d,RoleId:%d]!", GetGuildId(), tMember.dwRoleId);
		}
	}

	std::sort(vPlayerList.begin(), vPlayerList.end(), RolePowerPred);
}


// 发送限时商品购买记录
// void CGuild::SendLimitGoodsBuyRecord(Player *pPlayer)
// {
// 	MSG_S2C_SHOP_GOODS_BUY_RECORD_RESP msg;
// 	msg.m_byType = SHOP_GUILD_LIMIT;
// 	msg.m_byBuyRecordNum = 0;
// 	map<WORD, ShopGoodsBuyRecord>::iterator iter = m_GoodsBuyRecordList.begin();
// 	for (; iter != m_GoodsBuyRecordList.end(); ++iter)
// 	{
// 		if (msg.m_byBuyRecordNum >= MAX_GOOGS_BUY_RECORD_NUM)
// 			break;
// 		msg.m_arBuyRecord[msg.m_byBuyRecordNum++] = iter->second;
// 	}
// 	pPlayer->SendPacket(&msg);
// }

// 发送限时商品列表
void CGuild::SendLimitGoodsList(Player *pPlayer)
{
	MSG_S2C_SHOP_LIMIT_GOODS_LIST_RESP msg;
	msg.m_byType = SHOP_GUILD_LIMIT;
	msg.m_dwRefreshTime = m_pGuildMgr->GetLimitGoodsRefreshTime();
	msg.m_byGoodsNum = 0;
	vector<Goods>::iterator iter = m_vLimitGoodsList.begin();
	for (; iter != m_vLimitGoodsList.end(); ++iter)
	{
		msg.m_arGoodsList[msg.m_byGoodsNum++] = *iter;
	}
	pPlayer->SendPacket(&msg);

	// for test
	//MAPLOG(ERROR_LEVEL, "[%s]工会限时商品列表数量[num:%d]!", __FUNCTION__, msg.m_byGoodsNum);

	// todo::暂时这样处理
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (pMember)
		pMember->bIsLimitShopRefresh = false;
}

// 发送技能列表
void CGuild::SendSkillList(Player *pPlayer)
{
	MSG_S2C_GUILD_SKILL_LIST_RESP msg;
	msg.m_bySelfSkillNum = 0;
	msg.m_byGuildSkillNum = 0;

	// 工会技能
	for (size_t i = 0; i < m_SkillList.size(); ++i)
	{
		if (msg.m_byGuildSkillNum >= MAX_GUILD_SKILL_NUM)
			break;
		msg.m_GuildSkillList[msg.m_byGuildSkillNum++] = m_SkillList[i];
	}

	// 个人技能
	const PlayerGuildSkillData &tPlayerGuildSkill = pPlayer->GetRoleFullInfo().tGuildSkillData;
	for (WORD i = 0; i < tPlayerGuildSkill.wSkillNum; ++i)
	{
		if (msg.m_bySelfSkillNum >= MAX_GUILD_SKILL_NUM)
			break;
		msg.m_SelfSkillList[msg.m_bySelfSkillNum++] = tPlayerGuildSkill.arSkillList[i];
	}

	pPlayer->SendPacket(&msg);
}

// 发送聊天记录
void CGuild::SendChatRecordList(Player *pPlayer)
{
	MSG_S2C_CHAT_RECORD_LIST_RESP msg;
	msg.m_byChatType = CHAT_TYPE_GUILD;
	msg.m_wRecordNum = 0;
	for (list<ChatRecord>::iterator iter = m_ChatRecordList.begin(); iter != m_ChatRecordList.end(); ++iter)
	{
		if (msg.m_wRecordNum >= MAX_CHAT_RECORD_COUNT)
			break;

		msg.m_arRecordList[msg.m_wRecordNum++] = *iter;
	}
	pPlayer->SendPacket(&msg);
}

// 发送怪物数据
void CGuild::SendMonsterData(Player *pPlayer)
{
	MSG_S2C_GUILD_MONSTER_DATA_RESP msg;
	msg.m_wCampNum = 0;
	for (int i = CAMP_BEGIN; i <= CAMP_END; ++i)
	{
		msg.m_wCampNum++;
		msg.m_MonsterList[i].clear();
		msg.m_KillerList[i] = m_KillerList[i];
		vector<MonsterData> &vMonsterList = m_arMonsterList[i];
		for (int k = 0; k < vMonsterList.size(); ++k)
		{
			msg.m_MonsterList[i].push_back(vMonsterList[k]);
		}

		// 计算关卡奖励状态
// 		BYTE byStatus = 0; // 默认未领取
// 		if (IsMonsterDead(i))
// 		{
// 			byStatus = 1; // 通关后为可领取奖励
// 			map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(m_tData.wCurPassChapter+1);
// 			if (iter != m_MapRewardRecordList.end())
// 			{
// 				set<DWORD> &tTakePlayerList = iter->second.arAlreadyGetPlayerList[i];
// 				if (tTakePlayerList.find(pPlayer->GetRoleId()) != tTakePlayerList.end())
// 				{
// 					byStatus = 2; // 已经领取过了
// 				}
// 			}
// 		}
// 		msg.m_RewardStatusList[i] = byStatus;
	}
	pPlayer->SendPacket(&msg);
}

// 发送消息给所有成员
void CGuild::SendMsgToAllMember(NetMsgBody * pMsg)
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(m_tData.arMemberList[i].dwRoleId);
		if (pPlayer)
			pPlayer->SendPacket(pMsg);
	}
}

// 广播工会技能数据
void CGuild::SendSkillDataToAll()
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(m_tData.arMemberList[i].dwRoleId);
		if (pPlayer)
			SendSkillList(pPlayer);
	}
}

// 查找成员数据
GuildMember* CGuild::FindMemberData(DWORD dwRoleId)
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		if (m_tData.arMemberList[i].dwRoleId == dwRoleId)
		{
			return &m_tData.arMemberList[i];
		}
	}
	return NULL;
}

// 更新挑战次数
void CGuild::UpdateBattleTimes(GuildMember *pMember)
{
	// 非活动时间不需要恢复
	if (!IsFBOpenTime())
		return;

	// 挑战次数是满的
	if (!pMember->byBattleTimes)
		return;

	int nRecoveryTimes = DoRecoveryTimes(pMember->dwLastBattleTimesRecoverTime, time(NULL), g_GetConfigValue(134) * 60);
	if (nRecoveryTimes)
	{
		int nDailyBattleTimes = g_GetConfigValue(133);
		// 如果恢复次数>=默认次数 或 >= 已挑战的次数
		if (nRecoveryTimes >= nDailyBattleTimes || nRecoveryTimes >= pMember->byBattleTimes)
		{
			pMember->byBattleTimes = 0;
		}
		else
		{
			pMember->byBattleTimes -= nRecoveryTimes;
		}
	}
}