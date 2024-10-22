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
		MAPLOG(ERROR_LEVEL, "[%s]��ȡ����IDΪ0", __FUNCTION__);
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
		MAPLOG(ERROR_LEVEL, "[%s]����������ӳ�Աʱʧ��![%s]", __FUNCTION__, pCreater->GetUTF8RoleName());
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

	// ����ʷ��߼�¼��ʼ
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
		// ����Ҫ�򵽵ڶ��²ſ��Ի���
		if (m_tData.wHistoryChapterRecord >= 2)
			m_tData.wTodayStartChapter = m_tData.wHistoryChapterRecord - 1;
		else
			m_tData.wTodayStartChapter = m_tData.wHistoryChapterRecord;

		m_tData.wCurPassChapter = m_tData.wTodayStartChapter;
		wRemoveBegin = m_tData.wTodayStartChapter;
		wRemoveEnd = m_tData.wHistoryChapterRecord;
	}

	// ɾ��������ȡ��¼
	m_MapRewardRecordList.clear();
// 	for (WORD i = wRemoveBegin; i <= wRemoveEnd; ++i)
// 	{
// 		map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(i+1);
// 		if (iter != m_MapRewardRecordList.end())
// 			m_MapRewardRecordList.erase(iter);
// 	}

	// Ȫӿ��������
	m_tData.byQYTimes = 0;
	m_tData.dwQuanShui = 0;

	// ÿ��Ĭ���Ǵ���ʷ��¼�½ڿ�ʼ
	// m_tData.wIsFromHistoryChapter = true;

	m_DamageRecordList.clear();

	SaveDataToDB();
}

void CGuild::OnNewWeek()
{
	m_tData.dwActive = 0;
}

// �洢���ݵ����ݿ�
void CGuild::SaveDataToDB(bool isDelete)
{
	UINT64 lCurTime = GetTickCount64();

	PB::GuildInfo PBGuildInfo;
	
	// ��������
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

	// ��Ա�б�
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

	// �����¼��б�
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

	// �����б�
	for (list<GuildLeaveMessage>::iterator iter = m_MessageList.begin(); iter != m_MessageList.end(); ++iter)
	{
		::PB::GuildLeaveMessage *pPBLeaveMessage = PBGuildInfo.add_leave_message_list();
		pPBLeaveMessage->set_time(iter->dwTime);
		pPBLeaveMessage->set_name(iter->szName);
		pPBLeaveMessage->set_position(iter->byPos);
		pPBLeaveMessage->set_text(iter->szText);
	}

	// �����б�
	for (size_t i = 0; i < m_ApplyList.size(); i++)
	{
		PBGuildInfo.add_apply_list(m_ApplyList[i].dwRoleId);
	}

	// �����б�
	for (size_t i = 0; i < m_SkillList.size(); i++)
	{
		::PB::SkillData *pPBSkillData = PBGuildInfo.add_skill_list();
		pPBSkillData->set_skill_id(m_SkillList[i].wSkillId);
		pPBSkillData->set_level(m_SkillList[i].wLevel);
	}

	// ��ʱ��Ʒ�б�
	for (size_t i = 0; i < m_vLimitGoodsList.size(); ++i)
	{
		::PB::ShopGoods *pPBGoods = PBGuildInfo.add_goods_list();
		pPBGoods->set_goods_id(m_vLimitGoodsList[i].wId);
		pPBGoods->set_goods_num(m_vLimitGoodsList[i].wNum);
	}

	// �����¼
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

	// �˺���¼�б�
	for (map<DWORD, DWORD>::iterator iter = m_DamageRecordList.begin(); iter != m_DamageRecordList.end(); ++iter)
	{
		::PB::GuildMonsterDamageRecord *pPBRecord = PBGuildInfo.add_damage_record_list();
		pPBRecord->set_role_id(iter->first);
		pPBRecord->set_damage(iter->second);
	}

	// �ؿ�������ȡ��¼
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

			// �Ѿ���ȡ������б�
			for (set<DWORD>::iterator iter = pTakePlayerList->begin(); iter != pTakePlayerList->end(); ++iter)
			{
				pPBCampBox->add_take_reward_player_list(*iter);
			}
			// ����ȡ�ı����¼�б�
			for (vector<GuildFBBox>::iterator iter = pBoxList->begin(); iter != pBoxList->end(); ++iter)
			{
				::PB::GuildFBBox *pPBBox = pPBCampBox->add_take_box_record_list();
				pPBBox->set_box_idx(iter->wBoxIdx);
				pPBBox->set_role_name(iter->szRoleName);
				pPBBox->set_box_reward_idx(iter->wRewardIdx);
			}
		}
	}

	// ��Ӫ�����б�
	for (int i = 0; i < CAMP_NUM; ++i)
	{
		vector<MonsterData> &vMonsterList = m_arMonsterList[i];
		::PB::GuildCampMonster *pPBCampMonster = PBGuildInfo.add_camp_monster_list();
		// ���û�ɱ�߼���Ӫ
		pPBCampMonster->set_camp(i);
		pPBCampMonster->set_killer_name(m_KillerList[i]);
		// ������Ӫ�����б�
		for (vector<MonsterData>::iterator iter = vMonsterList.begin(); iter != vMonsterList.end(); ++iter)
		{
			::PB::MonsterData *pPBMonster = pPBCampMonster->add_monster_list();
			pPBMonster->set_damage(iter->dwDamage);
			pPBMonster->set_idx(iter->byIndex);
			pPBMonster->set_total_hp(iter->dwTotalHP);
		}
	}

	// ��������ʱ��
	PBGuildInfo.set_create_monster_time(m_dwCreateMonterTime);

	string strSaveData("");
	if (!PBGuildInfo.AppendToString(&strSaveData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]ϵ�л���������ʧ��[GuildId:%d]", __FUNCTION__, GetGuildId());
		return;
	}
	if (strSaveData.size() > MAX_GUILD_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����洢���ݹ���[size:%d]", __FUNCTION__, strSaveData.size());
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
		MAPLOG(ERROR_LEVEL, "[%s]ϵ�л�����������Ϣʱ��(%d)����[GuildId:%d]!", __FUNCTION__, dwCostTime, GetGuildId());
	}
}

// �����ݿ��������
void CGuild::LoadDataFromDB(CGuildManager *pGuildMgr, PB::GuildInfo *pPBGuildInfo)
{
	::PB::GuildBaseData *pPBBaseData = pPBGuildInfo->mutable_base_data();

	m_pGuildMgr = pGuildMgr;
	m_dwSeverId = pPBBaseData->server_id();
	
	// �����������
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

	// �����Ա
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

	// ��������б�
	for (int i = 0; i < pPBGuildInfo->apply_list_size(); ++i)
	{
		GuildApplyData tApplyData;
		tApplyData.dwRoleId = pPBGuildInfo->apply_list(i);
		m_ApplyList.push_back(tApplyData);
	}

	// �����¼��б�
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

	// �����б�
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

	// �����б�
	for (int i = 0; i < pPBGuildInfo->skill_list_size(); ++i)
	{
		const ::PB::SkillData &tPBSkill = pPBGuildInfo->skill_list(i);
		SkillData tSkill;
		tSkill.wSkillId = tPBSkill.skill_id();
		tSkill.wLevel = tPBSkill.level();
		m_SkillList.push_back(tSkill);
	}

	// ��ʱ��Ʒ�б�
	for (int i = 0; i < pPBGuildInfo->goods_list_size(); ++i)
	{
		const ::PB::ShopGoods &tPBGoods = pPBGuildInfo->goods_list(i);
		Goods tGoods;
		tGoods.wId = tPBGoods.goods_id();
		tGoods.wNum = tPBGoods.goods_num();
		m_vLimitGoodsList.push_back(tGoods);
	}
	// todo::�򲹶�,��ֹ�ϴ�ˢ�³�������Ʒľ�д��ȥ
	if (!m_vLimitGoodsList.size())
	{
		m_vLimitGoodsList = CGuildManager::Instance()->GetLimitGoodsList();
	}

	// �����¼
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

	// �˺���¼�б�
	m_DamageRecordList.clear();
	for (int i = 0; i < pPBGuildInfo->damage_record_list_size(); ++i)
	{
		const ::PB::GuildMonsterDamageRecord &pPBDamageRecord = pPBGuildInfo->damage_record_list(i);
		m_DamageRecordList.insert(make_pair(pPBDamageRecord.role_id(), pPBDamageRecord.damage()));
	}

	// �ؿ�������ȡ��¼[chapter,reward]
	m_MapRewardRecordList.clear();
	for (int i = 0; i < pPBGuildInfo->map_reward_record_list_size(); ++i)
	{
		const ::PB::GuildMapRewardRecord &tPBRewardRecord = pPBGuildInfo->map_reward_record_list(i);
		GuildFBMapReward tMapReward;
		for (int k = 0; k < tPBRewardRecord.camp_box_list_size(); ++k)
		{
			const ::PB::GuildCampBox &tPBCampBox = tPBRewardRecord.camp_box_list(k);

			// ����ȡ�ı����¼�б�
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

			// �Ѿ���ȡ������б�
			set<DWORD> &vTakePlayerList = tMapReward.arAlreadyGetPlayerList[tPBCampBox.camp()];
			for (size_t m = 0; m < tPBCampBox.take_reward_player_list_size(); ++m)
			{
				vTakePlayerList.insert(tPBCampBox.take_reward_player_list(m));
			}
		}
		m_MapRewardRecordList.insert(make_pair(tPBRewardRecord.chapter(), tMapReward));
	}

	// ���ḱ�������б�
	for (int i = 0; i < pPBGuildInfo->camp_monster_list_size(); ++i)
	{
		const ::PB::GuildCampMonster &tPBCampMonster = pPBGuildInfo->camp_monster_list(i);
		// ��ɱ��
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

	// ��������ʱ��
	m_dwCreateMonterTime = pPBGuildInfo->create_monster_time();

	MAPLOG(GUI_LEVEL, "�ɹ����ع���[GuildId:%d]!", GetGuildId());

	// todo::����ˢ�����а�,��ʱ��ô����
	CRankManager::Instance()->AddGuildRankData(m_dwSeverId, GetGuildId(), GUILD_RANK_LEVEL, GetGuildLevel());
}

// �Ƿ��к��
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

// �Ƿ��к��
bool CGuild::IsHaveRedPoint(Player *pPlayer, const GuildMember *pMember, BYTE byType)
{
	// ��������б����µ�������ʾ���
	if (GUILD_RED_POINT_APPLY == byType)
	{
		if (m_ApplyList.size() && pMember->byPosition != GUILD_POS_MEMBER)
			return true;
	}
	// ��ʱ�̵�ˢ�º���ʱҳǩ���̵꽨����ʾ��㣬������ʱҳǩ������ʧ
	else if (GUILD_RED_POINT_LIMIT_SHOP == byType)
	{
		if (pMember->bIsLimitShopRefresh)
			return true;
	}
	// �����̵�����µ���Ʒ����ʱ������ҳǩ���̵꽨����ʾ��㣬���뽱��ҳǩ�����ʧ
	else if (GUILD_RED_POINT_REWARD_SHOP == byType)
	{
		const vector<DWORD> &vGoodsList = g_pCfgMgr->GetGuildRewardGoodsIdList();
		for (size_t i = 0; i < vGoodsList.size(); ++i)
		{
			DWORD dwGoodsId = vGoodsList[i];
			const ShopGoodsData_Config *pGoodsCfg = g_pCfgMgr->GetShopGoods(SHOP_GUILD_AWARD, dwGoodsId);
			// ��ɫ�ȼ�����,����ȼ�����
			if (pGoodsCfg && pPlayer->GetLevel() >= pGoodsCfg->wNeedRoleLevel && GetGuildLevel() >= pGoodsCfg->wNeedGuildLevel)
			{
				// ��û���� �� �����Թ���
				const ShopGoodsBuyRecord *pBuyRecord = pPlayer->GetShopManager()->GetGoodsBuyRecordEx(SHOP_GUILD_AWARD, dwGoodsId);
				if (!pBuyRecord || pBuyRecord->wBuyNum < pGoodsCfg->wBuyNumMax)
					return true;
			}
		}
	}
	// �����п�ʣ�ཨ�����ʱ���ھ��Ž��轨����ʾ���
	else if (GUILD_RED_POINT_QY == byType)
	{
		if (!pMember->byQYType)
			return true;
	}
	// ���԰������µ����ԣ������԰�ť����ʾ��㣬�򿪺��ߵ���ʧ
	else if (GUILD_RED_POINT_LEAVE_MESSAGE == byType)
	{
		if (pMember->bHaveNewLeaveMessage)
			return true;
	}
	// ���ż��ܽ����У���ǰʣ�๱��ֵ���ٿ�ѧϰ/��������һ�μ���ʱ���ھ��ż��ܽ�����ʾ���
	if (GUILD_RED_POINT_SKILL == byType)
	{
		// ��û���￪�ŵȼ�
		const GuildBuilding_Config *pBuildingCfg = g_pCfgMgr->GetGuildBuilding(5);
		if (pBuildingCfg && GetGuildLevel() < pBuildingCfg->wNeedGuildLevel)
			return false;

		// ����ѧϰ
		for (size_t i = 0; i < m_SkillList.size(); ++i)
		{
			WORD wSkillId = m_SkillList[i].wSkillId;
			WORD wSkillLevel = m_SkillList[i].wLevel;;
			const SkillData *pSkillData = pPlayer->GetGuildSkill(wSkillId);
			// ��ûѧϰ,���߿�����
			if (!pSkillData || pSkillData->wLevel < wSkillLevel)
			{
				const GuildSkill_Config *pSkillCfg = g_pCfgMgr->GetGuildSkill(wSkillId, pSkillData ? pSkillData->wLevel + 1 : 1);
				// ����ֵ�㹻
				if (pSkillCfg && pPlayer->GetProperty(ROLE_PRO_GUILD_CONSTIBUTION) > pSkillCfg->GetCostType(ROLE_PRO_GUILD_CONSTIBUTION))
					return true;
			}
		}

		// ����ѧϰ
		if (pMember->byPosition != GUILD_POS_MEMBER)
		{
			const map<DWORD, GuildSkill_Config> &mapSkillList = g_pCfgMgr->GetGuildSkillList();
			map<DWORD, GuildSkill_Config>::const_iterator iter = mapSkillList.begin();
			for (; iter != mapSkillList.end(); ++iter)
			{
				// ���ᾭ���㹻
				if (m_tData.dwExp >= iter->second.nCostGuildExp && m_tData.wLevel >= iter->second.wNeedGuildLevel)
				{
					const SkillData *pSkill = GetSkillData(iter->second.wSkillId); //pPlayer->GetGuildSkill(iter->second.wSkillId);
					// ��ûѧϰ,���߿�����
					if (!pSkill || pSkill->wLevel < iter->second.wLevel)
						return true;
				}
			}
		}
	}
	// ���ḱ����ս
	else if (GUILD_RED_POINT_FB == byType)
	{
		// ��û���￪�ŵȼ�
		const GuildBuilding_Config *pBuildingCfg = g_pCfgMgr->GetGuildBuilding(3);
		if (pBuildingCfg && GetGuildLevel() < pBuildingCfg->wNeedGuildLevel)
			return false;

		// ������ս����
		GuildMember *pTemp = FindMemberData(pPlayer->GetRoleId());
		if (pTemp)
			UpdateBattleTimes(pTemp);

		// ����ʱ��
		if (IsFBOpenTime() && pMember->byBattleTimes < pMember->byBuyBattleTimes + g_GetConfigValue(133))
			return true;
	}
	// �����½ڽ���
	else if (GUILD_RED_POINT_CHAPTER_REWARD == byType)
	{
		PlayerGuildData &tPlayerGuildData = pPlayer->GetRoleFullInfo().tSecondInfo.tGuildData;

		// ����Ѿ���ȡ��ȫ��(�󲿷��˵����Ӧ����������,��������)
		if (m_tData.wCurPassChapter == tPlayerGuildData.wTakeChapterRewardRecordNum)
			return false;

		for (WORD i = 0; i <= m_tData.wCurPassChapter; ++i)
		{
			bool isTake = false;
			// ����Ƿ�����ȡ
			for (WORD k = 0; k < tPlayerGuildData.wTakeChapterRewardRecordNum; ++k)
			{
				if (tPlayerGuildData.arTakeChapterRewardRecord[k] == i)
				{
					isTake = true;
					break;
				}
			}


			// ��û����ȡ��
			if (!isTake)
				return true;
		}
	}
	// ����ؿ�����
	else if (GUILD_RED_POINT_MAP_REWARD == byType)
	{
		// ����Ѿ�ͨ�����½��Ƿ��н�������ȡ
		for (WORD i = m_tData.wTodayStartChapter + 1; i <= m_tData.wCurPassChapter; ++i)
		{
			map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(i);
			if (iter != m_MapRewardRecordList.end())
			{
				for (WORD k = 0; k < CAMP_NUM; ++k)
				{
					set<DWORD> &playerList = iter->second.arAlreadyGetPlayerList[k];
					// ֻҪ��һ����Ӫ�Ļ�û��ȡ
					if (playerList.find(pPlayer->GetRoleId()) == playerList.end())
						return true;
				}
			}
			// ��û������ȡ��
			else
			{
				return true;
			}
		}

		// ���������ս���½ڽ����Ƿ����ȡ
		map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(m_tData.wCurPassChapter + 1);
		for (int i = CAMP_BEGIN; i <= CAMP_END; ++i)
		{
			// ����Ӫ�Ĺ����ѹҵ�
			if (IsMonsterDead(i))
			{
				// ���½ڽ�����û��ȡ
				if (iter == m_MapRewardRecordList.end())
					return true;

				// ����Ӫ�Ľ�����û��ȡ
				set<DWORD> &playerList = iter->second.arAlreadyGetPlayerList[i];
				if (playerList.find(pPlayer->GetRoleId()) == playerList.end())
					return true;
			}
		}
	}

	return false;
}

// ս����ʼ
bool CGuild::OnBattleStart(Player *pPlayer, BYTE byCamp)
{
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵĹ�������!", __FUNCTION__);
		return false;
	}

	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ս���Ÿ�����Ӫ����![camp:%d]", __FUNCTION__, byCamp);
		return false;
	}

	// ��սĿ���½�
	WORD wBattleTargetChapter = m_tData.wCurPassChapter + 1;

	const GuildFBChapter_Config *pChapterCfg = g_pCfgMgr->GetGuildFB(wBattleTargetChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Ÿ�������![chapter:%d]", __FUNCTION__, wBattleTargetChapter);
		return false;
	}

	if (pChapterCfg->vMapList.size() != CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���Ÿ������õĹؿ��б����![map_size:%d]", __FUNCTION__, pChapterCfg->vMapList.size());
		return false;
	}

	// �Ƿ񿪷�ʱ��
	if (!IsFBOpenTime())
	{
		MAPLOG(ERROR_LEVEL, "[%s]���Ÿ����ǿ���ʱ��!", __FUNCTION__);
		return false;
	}

	// ������ս����
	UpdateBattleTimes(pMember);

	// �����ս����
	if (pMember->byBattleTimes >= pMember->byBuyBattleTimes + g_GetConfigValue(133))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ս���Ÿ��������Ѵ�����!", __FUNCTION__);
		return false;
	}

	// ����Ƿ��ѹҵ�
	if (IsMonsterDead(byCamp))
	{
		MAPLOG(ERROR_LEVEL, "[%s]���Ÿ��������ѹҵ�(Camp:%d)!", __FUNCTION__,byCamp);
		return false;
	}

	pMember->byBattleTimes++;
	pMember->byTodayBattleTimes++;

	vector<MonsterData> *pMonsterList = &m_arMonsterList[byCamp];

	// ��ӹ�������
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	for (size_t i = 0; i < pMonsterList->size(); ++i)
	{
		pFBMgr->AddMonsterData(pMonsterList->at(i));
	}

	pFBMgr->SetBattleInfo(wBattleTargetChapter, byCamp, pChapterCfg->vMapList[byCamp]);

	return true;
}

// ս������
bool CGuild::OnBattleOver(Player *pPlayer, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	BYTE byCamp = pFBMgr->GetMapIdx();
	WORD wBattleChapter = pFBMgr->GetCurChapter();

	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ս���Ÿ�����Ӫ����![camp:%d]", __FUNCTION__, byCamp);
		return false;
	}

	const GuildFBChapter_Config *pChapterCfg = g_pCfgMgr->GetGuildFB(wBattleChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Ÿ�������![chapter:%d]", __FUNCTION__, wBattleChapter);
		return false;
	}

	// ��ս����(���Ź���)
	AddContribution(pPlayer->GetRoleId(), pChapterCfg->nBattleContribution);
	pPlayer->ModifyProperty(ROLE_PRO_GUILD_CONSTIBUTION, pChapterCfg->nBattleContribution);

	DWORD dwCurHP = 0;
	bool bIsDeadBefore = IsMonsterDead(byCamp);

	// ��ֹ��ս����ʱ,�½��Ѿ�������½�����(��Ϊ�п���ͬʱ�ж���������ս)
	if (wBattleChapter == m_tData.wCurPassChapter + 1 && !bIsDeadBefore)
	{
		UpdateMonsterDamage(byCamp, pMonsterList, byMonsterNum);

		dwCurHP = GetMonsterCurHP(byCamp);

		// ��ɱ����
		if (IsMonsterDead(byCamp))
		{
			// ����
			AddContribution(pPlayer->GetRoleId(), pChapterCfg->nKillContribution);
			pPlayer->ModifyProperty(ROLE_PRO_GUILD_CONSTIBUTION, pChapterCfg->nKillContribution);

			// ����
			AddExp(pChapterCfg->nKillExp);

			// ��¼��ɱ������
			m_KillerList[byCamp] = pPlayer->GetRoleName();

			pFBMgr->SetIsMeKill(true);
		}

		// �Ƿ����е���Ӫ�Ĺ��ﶼ�ҵ�
		if (IsFBOpenTime() && IsAllCampMonsterDead())
		{
			// ������һ�½�
			m_tData.wCurPassChapter++; 

			// ��¼��ʷ��¼
			m_tData.wHistoryChapterRecord = max(m_tData.wHistoryChapterRecord, m_tData.wCurPassChapter);

			CreateMonster(false);
		}
	}

	// �˺�����
	DWORD dwTotalDamage = 0;
	for (BYTE i = 0; i < byMonsterNum; ++i)
	{
		dwTotalDamage += pMonsterList[i].dwDamage;
	}
	AddFBDamageRecord(pPlayer->GetRoleId(),dwTotalDamage);

	SaveDataToDB();

	// ���֮ǰ�ѹҵ�,����Ҫ�㲥
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

// ���͹��ḱ������
void CGuild::SendCopyData(Player *pPlayer)
{
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵĹ�������!", __FUNCTION__);
		return;
	}

	// ����ǻ�ڼ�,�һ�û��������
	if (IsFBOpenTime() && !IsTheSameDay(m_dwCreateMonterTime, time(NULL)))
	{
		CreateMonster(true);
	}

	// ������ս����
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


// ��ȡ�����½ڽ���
void CGuild::OnNetCopyChapterReward(Player *pPlayer, WORD wChapter)
{
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵĹ�������!", __FUNCTION__);
		return;
	}

	// ����Ƿ����ȡ
	if (/*wChapter < m_tData.wTodayStartChapter ||*/ wChapter > m_tData.wCurPassChapter)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���½ڲ�����ȡ����[chapter:%d]!", __FUNCTION__, wChapter);
		return;
	}

	PlayerGuildData &tPlayerGuildData = pPlayer->GetRoleFullInfo().tSecondInfo.tGuildData;

	// ����Ƿ�����ȡ
	for (WORD i = 0; i < tPlayerGuildData.wTakeChapterRewardRecordNum; ++i)
	{
		if (tPlayerGuildData.arTakeChapterRewardRecord[i] == wChapter)
		{
			MAPLOG(ERROR_LEVEL, "[%s]���Ÿ������½ڽ�������ȡ��!", __FUNCTION__);
			return;
		}
	}

	// ����б��Ƿ�����
	if (tPlayerGuildData.wTakeChapterRewardRecordNum >= MAX_GUILD_FB_CHAPTER_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���Ÿ����½ڽ�����¼�б�����!", __FUNCTION__);
		return;
	}

	const GuildFBChapter_Config *pFBCfg = g_pCfgMgr->GetGuildFB(wChapter);
	if (!pFBCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Ÿ�������[chaper:%d]!", __FUNCTION__, wChapter);
		return;
	}

	// ������
	pPlayer->AddObjectList(pFBCfg->vChapterRewardList, TRUE, FROM_GUILD_FB_CHAPTER_REWARD);

	// ��¼����ȡ
	tPlayerGuildData.arTakeChapterRewardRecord[tPlayerGuildData.wTakeChapterRewardRecordNum++] = wChapter;

	// ����ͬ������
	SendCopyData(pPlayer);

	// ��Ӧ
	MSG_S2C_GUILD_FB_CHAPTER_REWARD_RESP msg;
	msg.m_wChapter = wChapter;
	pPlayer->SendPacket(&msg);

	SaveDataToDB();
}

// ��ȡ�����ؿ�����
void CGuild::OnNetCopyMapReward(Player *pPlayer, WORD wChapter, BYTE byCamp, BYTE byBoxIdx)
{
	WORD wFromMsgId = C2S_GUILD_FB_MAP_REWARD_REQ;

	// �����Ӫ
	if (byCamp >= CAMP_NUM)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ȡ���Źؿ�������ӪԽ��[Camp:%d]!", __FUNCTION__, byCamp);
		return;
	}

	// ֻ����ȡ������ս����
	if (wChapter <= m_tData.wTodayStartChapter)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���½ڲ��ǽ�����ս�Ĳ�����ȡ[chapter:%d]!", __FUNCTION__, wChapter);
		return;
	}

	// ��ûͨ�ص��½ڲ�����ȡ
	if (wChapter > m_tData.wCurPassChapter + 1)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ȡ���½ڻ�ľ��ͨ��[chapter:%d]!", __FUNCTION__, wChapter);
		return;
	}

	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (!pMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵĹ�������!", __FUNCTION__);
		return;
	}

	// �����Ӫ�Ĺ����Ƿ�������(��������������ȡ)
	if (m_tData.wCurPassChapter + 1 == wChapter && !IsMonsterDead(byCamp))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���Ÿ�����Ӫ�Ĺ��ﻹľ�йҵ�,��������ȡ[Camp:%d]!", __FUNCTION__, byCamp);
		return;
	}

	const GuildFBChapter_Config *pFBCfg = g_pCfgMgr->GetGuildFB(wChapter);
	if (!pFBCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Ÿ�������[chaper:%d]!", __FUNCTION__, wChapter);
		return;
	}

	// ȡ��������б�(todo::����߼����е�����,�պ����Ż�)
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

			// ֻȡ��ľ����ȡ����
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

	// ��⽱���б��Ƿ�����ȡ��
	if (!vRandBoxIdxList.size())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���Źؿ������б�����ȡ��[chaper:%d,camp:%d]!", __FUNCTION__, wChapter, byCamp);
		return;
	}
	
	if (iter != m_MapRewardRecordList.end())
	{
		// �������Ƿ�����ȡ
		set<DWORD> &sTakePlayerList = iter->second.arAlreadyGetPlayerList[byCamp];
		if (sTakePlayerList.find(pPlayer->GetRoleId()) != sTakePlayerList.end())
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�Ѿ���ȡ�����Ÿ����ؿ�����[chapter:%d,camp:%d]!", __FUNCTION__, wChapter, byCamp);
			return;
		}
	}

	// ��ⱦ������
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
		MAPLOG(ERROR_LEVEL, "[%s]��ȡ���Źؿ�������������������[BoxIdx:%d]!", __FUNCTION__, byBoxIdx);
		return;
	}

	// �������
	WORD wRewardIdx = vRandBoxIdxList[g_Rand(0, vRandBoxIdxList.size()-1)];

	// ������
	pPlayer->AddObjectData(&pFBCfg->vMapRewardList[wRewardIdx], TRUE, FROM_GUILD_FB_LEVEL_REWARD);

	// ��ӽ�����¼
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

	// ��Ӧ
	MSG_S2C_GUILD_FB_MAP_REWARD_RESP msg;
	msg.m_wChapter = wChapter;
	msg.m_byCamp = byCamp;
	msg.m_byBoxIdx = byBoxIdx;
	msg.m_byRewardIdx = wRewardIdx;
	pPlayer->SendPacket(&msg);

	SaveDataToDB();
}

// ����ؿ�������¼
void CGuild::OnNetCopyMapRewardRecord(Player *pPlayer, WORD wChapter, BYTE byCamp)
{
	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ӫ����Խ��[Camp:%d]!", __FUNCTION__, byCamp);
		return;
	}

	MSG_S2C_GUILD_FB_MAP_REWARD_RECORD_RESP msg;
	msg.m_wChapter = wChapter;
	msg.m_byCamp = byCamp;
	msg.m_wRecordNum = 0;

	// ����ؿ�����״̬
// 	for (int i = 0; i < CAMP_NUM; ++i)
// 	{
// 		BYTE byStatus = 0; // Ĭ��δ��ȡ
// 		if (wChapter < m_tData.wCurPassChapter || IsMonsterDead(i))
// 		{
// 			byStatus = 1; // ͨ�غ�Ϊ����ȡ����
// 			map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(wChapter);
// 			if (iter != m_MapRewardRecordList.end())
// 			{
// 				set<DWORD> &tTakePlayerList = iter->second.arAlreadyGetPlayerList[i];
// 				if (tTakePlayerList.find(pPlayer->GetRoleId()) != tTakePlayerList.end())
// 				{
// 					byStatus = 2; // �Ѿ���ȡ����
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

// �����Ա����ս��
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

// ���򸱱���ս����
void CGuild::OnNetBuyCopyBattleTimes(Player *pPlayer, BYTE byBuyTimes)
{
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵĹ�������!", __FUNCTION__);
		return;
	}

	// ��⹺���������
	if (pMember->byBuyBattleTimes + byBuyTimes > pPlayer->GetVipValue(VIP_GUILD_FB_BUY_BATTLE_TIMES))
	{
		MAPLOG(ERROR_LEVEL, "[%s]������Ÿ�����ս�����Ѵ�����[BuyTimes:%d]!",
			__FUNCTION__, pMember->byBuyBattleTimes);
		return;
	}

	// ��⹺������
	const Server_Config *pSvrCfg = g_pCfgMgr->GetCommonCfg(69);
	if (!pSvrCfg && !pSvrCfg->vValueList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]������Ÿ�����ս�����������ô���!",__FUNCTION__);
		return;
	}
	int nCostDiamond = 0;
	BYTE byBegin = pMember->byBuyBattleTimes;
	BYTE byEnd = byBegin + byBuyTimes;
	for (BYTE i = byBegin; i < byEnd; ++i)
	{
		// ��������������õĴ���,��ȡ���һ��
		int nIndex = min(i, pSvrCfg->vValueList.size()-1);
		nCostDiamond += pSvrCfg->vValueList[nIndex];
	}
	if (nCostDiamond > pPlayer->GetProperty(ROLE_PRO_DIAMOND))
	{
		MAPLOG(ERROR_LEVEL, "[%s]������Ÿ�����ս��������Ԫ������!", __FUNCTION__);
		return;
	}

	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_BUY_GUILD_COPY_BATTLE_TIMES);

	pMember->byBuyBattleTimes += byBuyTimes;

	SendCopyData(pPlayer);

	// ��Ӧ
	MSG_S2C_GUILD_FB_BATTLE_TIMES_BUY_RESP msg;
	msg.m_byBuyTimes = byBuyTimes;
	pPlayer->SendPacket(&msg);

	SaveDataToDB();
}

// �����½�����
void CGuild::OnNetFBChapterReset(Player *pPlayer, BYTE byIsHistoryRecord)
{
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ���ҵ���Ա����[role_id:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ��ͨ��Աľ��Ȩ��
	if (GUILD_POS_MEMBER == pMember->byPosition)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Աľ��Ȩ�����ø����½�[role_id:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	m_tData.wIsFromHistoryChapter = byIsHistoryRecord;

	MSG_S2C_GUILD_FB_CHAPTER_RESET_RESP msg;
	msg.m_byIsHistoryRecord = byIsHistoryRecord;
	pPlayer->SendPacket(&msg);
}

// �����½ڹؿ�����״̬
void CGuild::OnNetMyMapRewardRecord(Player *pPlayer)
{
	MSG_S2C_GUILD_MY_MAP_REWARD_RECORD_RESP msg;
	msg.m_wRecordNum = 0;

	// �Ѿ�ͨ�صļ�¼
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

	// ������ս�ļ�¼
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

// ������������
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
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ḱ������![%d]", __FUNCTION__, m_tData.wCurPassChapter + 1);
		return;
	}

	if (pChapterCfg->vMapList.size() != CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���Ÿ����ؿ��б����ô���![map_size:%d]", __FUNCTION__, pChapterCfg->vMapList.size());
		return;
	}

	for (size_t i = 0; i < pChapterCfg->vMapList.size(); ++i)
	{
		DWORD dwMapId = pChapterCfg->vMapList[i];
		g_pCfgMgr->GetMapMonsterList(dwMapId, &m_arMonsterList[i]);
	}

	// ��ʼ����Ա��ս�����������
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

	MAPLOG(GUI_LEVEL, "[%s]�������Ÿ�������ɹ�![chapter:%d]", __FUNCTION__, m_tData.wCurPassChapter + 1);
}

// �Ƿ����������
bool CGuild::IsMonsterDead(BYTE byCamp)
{
	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ӪԽ��![Camp:%d]", __FUNCTION__, byCamp);
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

// �Ƿ�������Ӫ���ﶼ�ҵ�
bool CGuild::IsAllCampMonsterDead()
{
	for (BYTE i = 0; i < CAMP_NUM; ++i)
	{
		if (!IsMonsterDead(i))
			return false;
	}
	return true;
}

// ���¹����˺�
void CGuild::UpdateMonsterDamage(BYTE byCamp, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ӪԽ��![Camp:%d]", __FUNCTION__, byCamp);
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

// ��ǰ����HP
DWORD CGuild::GetMonsterCurHP(BYTE byCamp)
{
	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ӪԽ��![Camp:%d]", __FUNCTION__, byCamp);
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

// �Ƿ񸱱�����ʱ��
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

// ��Ӹ����˺���¼
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

// ȡ�᳤����
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

// ȡ�᳤ID
DWORD CGuild::GetCaptainId()
{
	const GuildMember *pMember =  GetCaptainData();
	return pMember ? pMember->dwRoleId : 0;
}

// ��������
void CGuild::SetDeclaration(const char *pszDeclaration)
{ 
	strcpy_s(m_tData.szDeclaration, sizeof(m_tData.szDeclaration), pszDeclaration); 
};


// ���ù���
void CGuild::SetNotice(const char *pszNotice)
{ 
	strcpy_s(m_tData.szNotice, sizeof(m_tData.szNotice), pszNotice); 
};

// ��Ӿ���
void CGuild::AddExp(DWORD dwExp)
{
	const Guild_Config *pGuildCfg = CConfigManager::Instance()->GetGuild(m_tData.wLevel);
	if (!pGuildCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������![level:%d]", __FUNCTION__, m_tData.wLevel);
		return;
	}

	m_tData.dwExp += dwExp;

	if (m_tData.dwExp >= pGuildCfg->dwNeedExp)
	{
		m_tData.dwExp -= pGuildCfg->dwNeedExp;
		m_tData.wLevel++;

		// �������а�
		CRankManager::Instance()->AddGuildRankData(m_dwSeverId, GetGuildId(), GUILD_RANK_LEVEL, m_tData.wLevel);
	}
}

// ��ӹ���ֵ
void CGuild::AddContribution(DWORD dwRoleId, int nContribution)
{
	GuildMember *pMember = FindMemberData(dwRoleId);
	if (pMember)
	{
		pMember->dwTotalContribution += nContribution;
		pMember->dwTodayContribution += nContribution;

		// 1/100 ���Ծ
		AddActive(nContribution * 0.01);
	}
}


// ��������
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

// ��ӳ�Ա
bool CGuild::AddMember(const GuildMember *pMember)
{
	if (m_tData.byMemberNum >= MAX_GUILD_MEMBER_NUM)
		return false;

	m_tData.arMemberList[m_tData.byMemberNum++] = *pMember;
	return true;
}

// ɾ����Ա
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

// ȡ��Ա����
const GuildMember* CGuild::GetMemberData(DWORD dwRoleId)
{
	return FindMemberData(dwRoleId);
}

// ȡ�᳤��Ϣ
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

// ��乤������
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

// ���ó�Աְλ
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

// ȡ��Աְλ
BYTE CGuild::GetMemberPos(DWORD dwRoleId)
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		if (m_tData.arMemberList[i].dwRoleId == dwRoleId)
			return m_tData.arMemberList[i].byPosition;
	}
	return GUILD_POS_INVALID;
}

// ȡĳ��ְλ�ϵ�������
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

// ȡ��ԱȪӿ����
BYTE CGuild::GetMemberQYType(DWORD dwRoleId)
{
	GuildMember *pMember = FindMemberData(dwRoleId);
	return pMember ? pMember->byQYType : 0;
}

// ��ӳ�ԱȪӿ����
void CGuild::SetMemberQYType(DWORD dwRoleId, BYTE byType)
{
	GuildMember *pMember = FindMemberData(dwRoleId);
	if (pMember)
	{
		pMember->byQYType = byType;
	}
}

// �Ƿ�����ȡ�˱�Ȫ����
bool CGuild::IsGetBQReward(DWORD dwRoleId, WORD wQuanShui)
{
	GuildMember *pMember = FindMemberData(dwRoleId);
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ա����![RoleId:%d]", __FUNCTION__, dwRoleId);
		return false;
	}

	for (BYTE i = 0; i < pMember->byQSRewardRecordNum; ++i)
	{
		if (pMember->arQSRewardRecord[i] == wQuanShui)
			return true;
	}
	return false;
}

// ���Ȫˮ����
void CGuild::AddQSRewardRecord(DWORD dwRoleId, WORD wQuanShui)
{
	GuildMember *pMember = FindMemberData(dwRoleId);
	if (!pMember)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ա����![RoleId:%d]", __FUNCTION__, dwRoleId);
		return;
	}

	if (pMember->byQSRewardRecordNum >= MAX_GUILD_QS_REWARD_RECORD)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����Ȫˮ������¼����![RoleId:%d]", __FUNCTION__, dwRoleId);
		return;
	}

	pMember->arQSRewardRecord[pMember->byQSRewardRecordNum++] = wQuanShui;
}

// ȡ����
const SkillData* CGuild::GetSkillData(WORD wSkillId)
{
	for (size_t i = 0; i < m_SkillList.size(); ++i)
	{
		if (m_SkillList[i].wSkillId == wSkillId)
			return &m_SkillList[i];
	}
	return NULL;
}

// ��Ӽ���
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

// ��������¼
void CGuild::AddChatRecord(const ChatRecord &tRecord)
{
	m_ChatRecordList.push_back(tRecord);

	if (m_ChatRecordList.size() > MAX_CHAT_RECORD_COUNT)
		m_ChatRecordList.pop_front();
}

// ������ս���� todo:: for test
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

// �����г�Ա������
void CGuild::SendRewardToAllMember(const RewardData &tRewardData)
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		const GuildMember &tMember = m_tData.arMemberList[i];
		DWORD dwServerId = PlayerBaseDataManager::Instance()->GetServerIdByRoleId(tMember.dwRoleId);
		CRewardCenterManager::AddReward(dwServerId, tMember.dwRoleId, tRewardData);
	}
}

// ���͹�������
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
	
	// ����Ƿ�������Ȫˮ
	if (IsTheSameDay(m_tLastQYInfo.dwTime, time(NULL)))
	{
		msg.m_dwLastQYValue = m_tLastQYInfo.dwValue;
		msg.m_byLastQYType = m_tLastQYInfo.byQSType;
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_dwSeverId, m_tLastQYInfo.dwRoleId);
		if (pBaseData)
			strcpy_s(msg.m_szLastQYTargetName, sizeof(msg.m_szLastQYTargetName) - 1, pBaseData->szRoleName);
	}

	// ���
	msg.m_wRedPointNum = 0;
	for (BYTE i = 0; i < GUILD_RED_POINT_MAX; ++i)
	{
		if (IsHaveRedPoint(pPlayer, pMember, i))
			msg.m_arRedPointList[msg.m_wRedPointNum++] = i;
	}

	pPlayer->SendPacket(&msg);
}

// ���͹�������
void CGuild::SendGuildData(DWORD dwRoleId)
{
	Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId);
	if (pPlayer)
		SendGuildData(pPlayer);
}

// ���ͳ�Ա�б�
void CGuild::SendMemberList(Player *pPlayer)
{
	// todo::�˴��߼�,�պ����Ҫ����
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

// ����������Ϣ�б� 
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

// ���������б�
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

// �����¼��б� 
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

// ��ɢ
void CGuild::Dismiss()
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		DWORD dwRoleId = m_tData.arMemberList[i].dwRoleId;
		m_pGuildMgr->RemoveMember(dwRoleId);
		Player * pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId);
		if (pPlayer)
		{
			// todo::֪ͨ���,�����ɢ
		}
	}
}

// �����Ա�Ƿ�����
bool CGuild::IsMemberFull()
{
	const Guild_Config *pGuildCfg = CConfigManager::Instance()->GetGuild(m_tData.wLevel);
	if (!pGuildCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������![level:%d]", __FUNCTION__, m_tData.wLevel);
		return true;
	}
	return m_tData.byMemberNum >= pGuildCfg->wMaxMember ? true : false;
}

// ��������������
void CGuild::AddApplyData(Player *pPlayer)
{
	GuildApplyData applydata;
	applydata.dwRoleId = pPlayer->GetCharID();
	m_ApplyList.push_back(applydata);
}

// ɾ�������������
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

// ȡ�����������
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

// ����Ƿ��������б�
bool CGuild::IsInApplyList(DWORD dwRoleId)
{
	return GetApplyData(dwRoleId) ? true : false;
}

// ���������Ϣ
void CGuild::AddLeaveMessage(const GuildLeaveMessage &sLeaveMsg)
{
	m_MessageList.push_front(sLeaveMsg);

	if (m_MessageList.size() > MAX_GUILD_LEAVEMESSAGE_NUM)
	{
		m_MessageList.pop_back();
	}

	// ����������״̬
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		m_tData.arMemberList[i].bHaveNewLeaveMessage = true;
	}
}

// ȡ��Ʒ�����¼
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

// �����Ʒ�����¼
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

// ˢ����ʱ��Ʒ 
void CGuild::UpdateLimitGoods(vector<Goods> &vGoodsList)
{
	m_vLimitGoodsList = vGoodsList;

	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		m_tData.arMemberList[i].bIsLimitShopRefresh = true;
	}
}

// ����¼�
void CGuild::AddEvent(const GuildEvent &tEvent)
{
	m_EventList.push_back(tEvent);

	if (m_EventList.size() > MAX_GM_EVENT_NUM)
		m_EventList.pop_front();
}

// ����¼�
void CGuild::AddEvent(WORD wEventId, const char *pszTargetName, DWORD dwValue)
{
	GuildEvent tEvent;
	tEvent.wEventId = wEventId;
	tEvent.dwTime = time(NULL);
	tEvent.dwValue = dwValue;
	strcpy_s(tEvent.szTargetName, sizeof(tEvent.szTargetName) - 1, pszTargetName);
	AddEvent(tEvent);
}

// �������Ȫˮ��Ϣ
void CGuild::SetLastQSInfo(DWORD dwRoleId, DWORD dwValue, BYTE byType)
{
	m_tLastQYInfo.byQSType = byType;
	m_tLastQYInfo.dwValue = dwValue;
	m_tLastQYInfo.dwRoleId = dwRoleId;
	m_tLastQYInfo.dwTime = time(NULL);
}

// ȡ��ʱ��Ʒ
const Goods* CGuild::GetLimitGoods(WORD wGoodsId)
{
	for (size_t i = 0; i < m_vLimitGoodsList.size(); ++i)
	{
		if (m_vLimitGoodsList[i].wId == wGoodsId)
			return &m_vLimitGoodsList[i];
	}
	return NULL;
}

// �����ʱ��Ʒ��������
void CGuild::AddLimitGoodsBuyNum(WORD wGoodsId, WORD wBuyNum)
{
	// ֻ�й���ˢ������Ʒ���ܹ���,����,���û������Ҫ����
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

// ȡ��Աս�������б�
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
			//MAPLOG(SPECAIL_LEVEL, "�����Ա[GuildId:%d,RoleId:%d]!", GetGuildId(), tMember.dwRoleId);
		}
	}

	std::sort(vPlayerList.begin(), vPlayerList.end(), RolePowerPred);
}


// ������ʱ��Ʒ�����¼
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

// ������ʱ��Ʒ�б�
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
	//MAPLOG(ERROR_LEVEL, "[%s]������ʱ��Ʒ�б�����[num:%d]!", __FUNCTION__, msg.m_byGoodsNum);

	// todo::��ʱ��������
	GuildMember *pMember = FindMemberData(pPlayer->GetRoleId());
	if (pMember)
		pMember->bIsLimitShopRefresh = false;
}

// ���ͼ����б�
void CGuild::SendSkillList(Player *pPlayer)
{
	MSG_S2C_GUILD_SKILL_LIST_RESP msg;
	msg.m_bySelfSkillNum = 0;
	msg.m_byGuildSkillNum = 0;

	// ���Ἴ��
	for (size_t i = 0; i < m_SkillList.size(); ++i)
	{
		if (msg.m_byGuildSkillNum >= MAX_GUILD_SKILL_NUM)
			break;
		msg.m_GuildSkillList[msg.m_byGuildSkillNum++] = m_SkillList[i];
	}

	// ���˼���
	const PlayerGuildSkillData &tPlayerGuildSkill = pPlayer->GetRoleFullInfo().tGuildSkillData;
	for (WORD i = 0; i < tPlayerGuildSkill.wSkillNum; ++i)
	{
		if (msg.m_bySelfSkillNum >= MAX_GUILD_SKILL_NUM)
			break;
		msg.m_SelfSkillList[msg.m_bySelfSkillNum++] = tPlayerGuildSkill.arSkillList[i];
	}

	pPlayer->SendPacket(&msg);
}

// ���������¼
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

// ���͹�������
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

		// ����ؿ�����״̬
// 		BYTE byStatus = 0; // Ĭ��δ��ȡ
// 		if (IsMonsterDead(i))
// 		{
// 			byStatus = 1; // ͨ�غ�Ϊ����ȡ����
// 			map<WORD, GuildFBMapReward>::iterator iter = m_MapRewardRecordList.find(m_tData.wCurPassChapter+1);
// 			if (iter != m_MapRewardRecordList.end())
// 			{
// 				set<DWORD> &tTakePlayerList = iter->second.arAlreadyGetPlayerList[i];
// 				if (tTakePlayerList.find(pPlayer->GetRoleId()) != tTakePlayerList.end())
// 				{
// 					byStatus = 2; // �Ѿ���ȡ����
// 				}
// 			}
// 		}
// 		msg.m_RewardStatusList[i] = byStatus;
	}
	pPlayer->SendPacket(&msg);
}

// ������Ϣ�����г�Ա
void CGuild::SendMsgToAllMember(NetMsgBody * pMsg)
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(m_tData.arMemberList[i].dwRoleId);
		if (pPlayer)
			pPlayer->SendPacket(pMsg);
	}
}

// �㲥���Ἴ������
void CGuild::SendSkillDataToAll()
{
	for (BYTE i = 0; i < m_tData.byMemberNum; ++i)
	{
		Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(m_tData.arMemberList[i].dwRoleId);
		if (pPlayer)
			SendSkillList(pPlayer);
	}
}

// ���ҳ�Ա����
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

// ������ս����
void CGuild::UpdateBattleTimes(GuildMember *pMember)
{
	// �ǻʱ�䲻��Ҫ�ָ�
	if (!IsFBOpenTime())
		return;

	// ��ս����������
	if (!pMember->byBattleTimes)
		return;

	int nRecoveryTimes = DoRecoveryTimes(pMember->dwLastBattleTimesRecoverTime, time(NULL), g_GetConfigValue(134) * 60);
	if (nRecoveryTimes)
	{
		int nDailyBattleTimes = g_GetConfigValue(133);
		// ����ָ�����>=Ĭ�ϴ��� �� >= ����ս�Ĵ���
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