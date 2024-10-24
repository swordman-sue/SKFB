#include "StdAfx.h"
#include "PacketHandler.h"
#include "HandlerFromGate.h"
#include "HandlerFromOpen.h"
#include "GateServerSession.h"
#include "AccountInfoManager.h"
#include "AccountInfo.h"
#include "PlayerManager.h"
#include "Player.h"
#include "../Common/MessageIdDefine.h"
#include "../Common/PacketStruct_Code.h"
#include "../Common/MsgDefine.h"
#include "NetBase.h"
#include "header.pb.h"
#include "login.pb.h"
#include "server.pb.h"
#include "common_msg.pb.h"
#include "MsgRegister.h"
#include "MapServer.h"
//#include "../user_info.pb.h"

PacketHandler::PacketHandler(void)
{
}

PacketHandler::~PacketHandler(void)
{
}

BOOL PacketHandler::Resister()
{
	//注册消息处理函数
	ResisterGate();
	ResisterOpen();

	return TRUE;
}

BOOL PacketHandler::AddHandlerGate(DWORD dwMsgId, fnHandler fnHandler)
{
	m_FunctionMapGate.Add(dwMsgId, fnHandler);
	return TRUE;
}

//解析网关服务器的消息包
void PacketHandler::ParsePacketGate( GateServerSession * pSession, unsigned char *pMsg, WORD wSize )
{
	//对包进行解析
	unsigned char* pInBuff = pMsg;
	DWORD dwGuid;
	WORD wMsgID;
	DWORD dwSeriesId; // 系列ID

	//解析包头
	if (NetMsgHeadHandler::DecodeServerHead(pInBuff, wSize, dwGuid, dwSeriesId, wMsgID))
	{
		MAPLOG(ERROR_LEVEL, "[%s]包头解析错误[size:%d]!", __FUNCTION__, wSize);
		return;
	}

	NetMsgBody* pMsgBody = MsgRegister::Instance().GetMsgBody(wMsgID);
	if (!pMsgBody)
	{
		MAPLOG(ERROR_LEVEL, "[%s]未定义的消息结构[MsgID=%d]!", __FUNCTION__, wMsgID);
		return;
	}

	WORD wHeadLength = sizeof(dwGuid)+sizeof(dwSeriesId)+sizeof(wMsgID);

	//解析包体
	if (pMsgBody->DecodeMsg(pInBuff + wHeadLength, wSize - wHeadLength) == -1)
	{
		MAPLOG(ERROR_LEVEL, "[%s]消息解析出错[MsgID=%d]!", __FUNCTION__, wMsgID);
		return;
	}

	//根据消息ID查找处理回调函数
	fnHandler pfnHandler = m_FunctionMapGate.Find(wMsgID);
	//回调函数存在时
	if (pfnHandler)
	{
//#ifdef _DEBUG
		MAPLOG(SPECAIL_LEVEL, "Recv GateServer Msg 收到网关消息(guid:%d,id:%d)", dwGuid, wMsgID);
//#endif
		UINT64 lOldTime = GetTickCount64();

		// 来自客户端的消息
		if (wMsgID >= C2M_MIN && wMsgID <= C2M_MAX)
		{
			Player *pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
			if (pPlayer)
			{
				if (g_MapServer.IsCheckMsgSeriesId() && dwSeriesId <= pPlayer->GetNetworkSeriesId())
				{
					MSG_M2G_CLIENT_KICKOUT_CMD msg;
					msg.m_dwErrorCode = 0;
					pPlayer->SendPacket(&msg);
					AccountInfoManager::Instance()->KickoutAccount(pPlayer->GetAccountInfo());
					MAPLOG(ERROR_LEVEL, "[%s]玩家消息系列ID出错(AccID=%s,msg_id:%d,ClientSeriesId:%d,ServerSerirsId:%d)",
						__FUNCTION__, pPlayer->GetAccID(), wMsgID, dwSeriesId, pPlayer->GetNetworkSeriesId());
					return;
				}
				pPlayer->SetNetworkSeriesId(dwSeriesId);
			}
		}

		//回调函数处理消息
		pfnHandler(pSession, pMsgBody, dwGuid);
		
		g_MapServer.RecvClientMsgCallback(dwGuid, wMsgID);

		DWORD dwCostTime = GetTickCount64() - lOldTime;
		if (dwCostTime)
			MAPLOG(ERROR_LEVEL, "Do msg cost(cost:%d,id:%d,name:%s)", dwCostTime, wMsgID, pMsgBody->GetClassName());

//#ifdef _DEBUG
		MAPLOG(SPECAIL_LEVEL, "Done gatesvr msg(guid:%d,id:%d,size:%d,name:%s)", dwGuid, wMsgID, wSize, pMsgBody->GetClassName());
//#endif
	}
	//回调函数不存在时
	else
	{
		//无效的消息ID，可能是玩家使用外挂试探消息结构，踢出玩家
		Player* pPlayer = PlayerManager::Instance()->FindPlayerByGuid(dwGuid);
		if (!pPlayer)
			return;

		AccountInfo* pAccountInfo = pPlayer->GetAccountInfo();
		if (!pAccountInfo)
		{
			MAPLOG(GUI_LEVEL, "收到消息时,找不到角色帐号信息(guid:%d,id:%d,role_id:%d", 
				dwGuid, wMsgID, pPlayer->GetRoleId());
			return;
		}

		MSG_M2G_CLIENT_KICKOUT_CMD msg;
		msg.m_dwErrorCode = 0;
		pPlayer->SendPacket(&msg);
		AccountInfoManager::Instance()->KickoutAccount(pAccountInfo);
		MAPLOG(ERROR_LEVEL, "玩家消息ID解析失败(AccID=%s)", pAccountInfo->GetAccID());
	}
}

BOOL PacketHandler::AddHandlerOpen(DWORD dwMsgId, fnHandler_O fnHandler)
{
	m_FunctionMapOpen.Add(dwMsgId, fnHandler);
	return TRUE;
}

//解析开放服务器的消息包
void PacketHandler::ParsePacketOpen( OpenServerSession * pSession, unsigned char *pMsg, WORD wSize )
{
	//对包进行解析
	unsigned char* pInBuff = pMsg;
	DWORD dwGuid;
	WORD wMsgID;
	DWORD dwSeriesId; // 系列ID

	//解析包头
	if (NetMsgHeadHandler::DecodeServerHead(pInBuff, wSize, dwGuid, dwSeriesId, wMsgID))
	{
		MAPLOG(ERROR_LEVEL, "[%s]包头解析错误[size:%d]!", __FUNCTION__, wSize);
		return;
	}

	NetMsgBody* pMsgBody = MsgRegister::Instance().GetMsgBody(wMsgID);
	if (!pMsgBody)
	{
		MAPLOG(ERROR_LEVEL, "[%s]未定义的消息结构[MsgID=%d]!", __FUNCTION__, wMsgID);
		return;
	}

	WORD wHeadLength = sizeof(dwGuid)+sizeof(dwSeriesId)+sizeof(wMsgID);

	//解析包体
	if (pMsgBody->DecodeMsg(pInBuff + wHeadLength, wSize - wHeadLength) == -1)
	{
		MAPLOG(ERROR_LEVEL, "[%s]消息解析出错[MsgID=%d]!", __FUNCTION__, wMsgID);
		return;
	}

	fnHandler_O pfnHandler = m_FunctionMapOpen.Find(wMsgID);

	if (pfnHandler)
		pfnHandler(pSession, pMsgBody, dwGuid);

#ifdef _DEBUG
	MAPLOG(GUI_LEVEL, "open_msg(guid:%d,id:%d,size:%d,name:%s)", dwGuid, wMsgID, wSize, pMsgBody->GetClassName());
#endif
}


BOOL PacketHandler::ResisterGate()
{
	//#define RESISTER_MSG(X,Y) AddHandlerGate(X,		HandlerFromGate::On##Y);
	#define RESISTER_MSG(X)		AddHandlerGate(X,		HandlerFromGate::OnMSG_##X);

	//其他
	RESISTER_MSG(C2S_HEART_BEAT_REQ)
	RESISTER_MSG(C2S_SERVER_TIME_REQ);
	RESISTER_MSG(C2S_GUIDE_UPDATE_REQ);

	//
	RESISTER_MSG(G2M_CLIENT_DISCONNECT_CMD)
	RESISTER_MSG(G2M_HEART_BEAT)

	/////////////////////注册处理回调函数////////////////////////////

	// 登录,角色
	RESISTER_MSG(C2S_LOGIN_ACCOUNT_REQ);
	RESISTER_MSG(C2S_LOGIN_SELECT_SERVER_REQ);
	RESISTER_MSG(C2S_LOGIN_CREATE_ROLE_REQ);
	RESISTER_MSG(C2S_ROLE_DETAILS_REQ);
	RESISTER_MSG(C2S_ROLE_PRO_RECOVER_REQ);
	RESISTER_MSG(C2S_HEAD_IMAGE_REPLACE_REQ);
	RESISTER_MSG(C2S_ROLE_NAME_MODIFY_REQ);
	RESISTER_MSG(C2S_START_GAME_REQ);
	RESISTER_MSG(C2S_ACCOUNT_REGISTER_REQ);
	RESISTER_MSG(C2S_ACCOUNT_PASSWORD_CHANGE_REQ);
	RESISTER_MSG(C2S_TAKE_ACTIVITY_GIFT_REQ);
	RESISTER_MSG(C2S_ACTIVITY_NOTICE_REQ);
	RESISTER_MSG(C2S_AUTHENTICATION_CODE_REQ);
	RESISTER_MSG(C2S_MOBILE_BIND_REQ);
	RESISTER_MSG(C2S_SDK_LOGIN_REQ);
	RESISTER_MSG(C2S_SDK_ORDER_ID_REQ);
	RESISTER_MSG(C2S_RECHARGE_SUCCESS_REPORT_REQ);
	RESISTER_MSG(C2S_HERO_GOTO_REQ);
	RESISTER_MSG(C2S_ENTER_SCENE_REQ);
	RESISTER_MSG(C2S_LEAVE_SCENE_REQ);
	RESISTER_MSG(C2S_SCENE_ITEM_LOCK_REQ);
	RESISTER_MSG(C2S_SCENE_ITEM_PICK_UP_REQ);
	RESISTER_MSG(C2S_LOGIN_REQ);
	RESISTER_MSG(C2S_NEW_ROLE_GUIDE_UPDATE_REQ);


	RESISTER_MSG(C2S_BATTLE_START_REQ);
	RESISTER_MSG(C2S_BATTLE_END_REQ);
	RESISTER_MSG(C2S_HELP_BATTLE_TARGET_REQ);

	// 排行榜
	RESISTER_MSG(C2S_RANK_LIST_REQ);
	RESISTER_MSG(C2S_RANK_LIKE_REQ);
	RESISTER_MSG(C2S_RANK_LEAVE_MESSAGE_REQ);
	RESISTER_MSG(C2S_RANK_LEAVE_MESSAGE_LIST_REQ);
	RESISTER_MSG(C2S_GUILD_RANK_LIST_REQ);
	RESISTER_MSG(C2S_MY_RANK_REQ);

	RESISTER_MSG(C2S_CHAT_REQ);
	RESISTER_MSG(C2S_CHAT_VOICE_REQ);
	RESISTER_MSG(C2S_CHAT_RECORD_LIST_REQ);

	RESISTER_MSG(C2M_GM_REQ)

	// 商店
	RESISTER_MSG(C2S_SHOP_GOODS_BUY_REQ);
	RESISTER_MSG(C2S_SHOP_GOODS_BUY_RECORD_REQ);
	RESISTER_MSG(C2S_SHOP_GOODS_SELL_REQ);
	RESISTER_MSG(C2S_SHOP_LIMIT_GOODS_LIST_REQ);
	RESISTER_MSG(C2S_SHOP_REFRESH_REQ);
	RESISTER_MSG(C2S_HERO_SHOP_DATA_REQ);
	RESISTER_MSG(C2S_AWAKEN_SHOP_DATA_REQ);
	RESISTER_MSG(C2S_STAR_SOUL_SHOP_DATA_REQ);
	RESISTER_MSG(C2S_DIRECT_SHOP_DATA_REQ);
	RESISTER_MSG(C2S_TAKE_DIRECT_BUY_GOODS_REQ);
	RESISTER_MSG(C2S_CRYSTAL_SHOP_DATA_REQ);

	// 副本
	RESISTER_MSG(C2S_FB_MAIN_DATA_REQ);
	RESISTER_MSG(C2S_FB_AWARD_GET_REQ);
	RESISTER_MSG(C2S_FB_DAILY_DATA_REQ);
	RESISTER_MSG(C2S_FB_HERO_DATA_REQ);
	RESISTER_MSG(C2S_FB_CLEAN_REQ);
	RESISTER_MSG(C2S_FB_ELITE_DATA_REQ);
	RESISTER_MSG(C2S_FB_RESET_REQ);
	RESISTER_MSG(C2S_FB_ONE_KEY_TAKE_REWARD_REQ);
	RESISTER_MSG(C2S_TAKE_MAIN_FB_CHAPTER_REWARD_REQ);
	
	RESISTER_MSG(C2S_TOWER_DATA_REQ);
	RESISTER_MSG(C2S_TOWER_BUY_TIMES_REQ);
	RESISTER_MSG(C2S_TOWER_RESET_REQ);
	RESISTER_MSG(C2S_TOWER_SELECT_PRO_REQ);
	RESISTER_MSG(C2S_TOWER_BUY_STAR_GOODS_REQ);
	RESISTER_MSG(C2S_TOWER_ONE_KEY_CLEAN_REQ);

	// 领地
	RESISTER_MSG(C2S_PLAYER_LAND_DATA_REQ);
	RESISTER_MSG(C2S_LAND_PATROL_REQ);
	RESISTER_MSG(C2S_LAND_VIEW_PATROL_AWARD_REQ);
	RESISTER_MSG(C2S_LAND_GET_PATROL_AWARD_REQ);
	RESISTER_MSG(C2S_LAND_SKILL_LEVELUP_REQ);
	RESISTER_MSG(C2S_LAND_SUPPRESS_RIOT_REQ);
	RESISTER_MSG(C2S_FRIEND_LAND_LIST_REQ);
	RESISTER_MSG(C2S_LAND_CITY_DETAILS_REQ);

	// 精灵
	RESISTER_MSG(C2S_SPIRIT_DATA_REQ);
	RESISTER_MSG(C2S_HEART_OF_WORLD_LEVEL_UP_REQ);
	RESISTER_MSG(C2S_SPIRIT_ENHANCE_REQ);
	RESISTER_MSG(C2S_SPIRIT_USE_REQ);
	RESISTER_MSG(C2S_HEART_OF_WORLD_DATA_REQ);
	RESISTER_MSG(C2S_SPIRIT_TRAIN_REQ);
	RESISTER_MSG(C2S_SPRITE_STAR_UP_REQ);
	RESISTER_MSG(C2S_SPRITE_BLESS_REQ);
	RESISTER_MSG(C2S_SPRITE_COMPOSE_REQ);

	// 工会
	RESISTER_MSG(C2S_GUILD_DATE_REQ);
	RESISTER_MSG(C2S_GUILD_CREATE_REQ);
	RESISTER_MSG(C2S_GUILD_DISMISS_REQ);
	RESISTER_MSG(C2S_GUILD_APPLY_REQ);
	RESISTER_MSG(C2S_GUILD_APPLY_CHECK_REQ);
	RESISTER_MSG(C2S_GUILD_APPLY_CANCEL_REQ);
	RESISTER_MSG(C2S_GUILD_KICKOUT_REQ);
	RESISTER_MSG(C2S_GUILD_LEAVE_REQ);
	RESISTER_MSG(C2S_GUILD_APPOINT_REQ);
	RESISTER_MSG(C2S_GUILD_LEAVE_MESSAGE_REQ);
	RESISTER_MSG(C2S_GUILD_SEARCH_REQ);
	RESISTER_MSG(C2S_GUILD_QY_REQ);
	RESISTER_MSG(C2S_GUILD_QS_AWARD_REQ);
	RESISTER_MSG(C2S_GUILD_APPLY_LIST_REQ);
	RESISTER_MSG(C2S_GUILD_LEAVEMSG_LIST_REQ);
	RESISTER_MSG(C2S_GUILD_MEMBER_LIST_REQ);
	RESISTER_MSG(C2S_GUILD_LIST_REQ);
	RESISTER_MSG(C2S_GUILD_INFO_MODIFY_REQ);
	RESISTER_MSG(C2S_GUILD_EVENT_LIST_REQ);
	RESISTER_MSG(C2S_GUILD_SKILL_LIST_REQ);
	RESISTER_MSG(C2S_GUILD_SKILL_LEVEL_UP_REQ);
	RESISTER_MSG(C2S_GUILD_FB_DATA_REQ);
	RESISTER_MSG(C2S_GUILD_FB_CHAPTER_REWARD_REQ);
	RESISTER_MSG(C2S_GUILD_FB_MAP_REWARD_REQ);
	RESISTER_MSG(C2S_GUILD_FB_MAP_REWARD_RECORD_REQ);
	RESISTER_MSG(C2S_GUILD_FB_DAMAGE_RECORD_REQ);
	RESISTER_MSG(C2S_GUILD_FB_BATTLE_TIMES_BUY_REQ);
	RESISTER_MSG(C2S_GUILD_IMPEACH_REQ);
	RESISTER_MSG(C2S_GUILD_MONSTER_DATA_REQ);
	RESISTER_MSG(C2S_GUILD_FB_CHAPTER_RESET_REQ);
	RESISTER_MSG(C2S_GUILD_MY_MAP_REWARD_RECORD_REQ);

	// 成就
	RESISTER_MSG(C2S_ACHIEVEMENT_LIST_REQ);
	RESISTER_MSG(C2S_ACHIEVEMENT_AWARD_REQ);

	// 日常任务
	RESISTER_MSG(C2S_DAILY_MISSION_LIST_REQ);
	RESISTER_MSG(C2S_DAILY_MISSION_AWARD_REQ);
	RESISTER_MSG(C2S_DAILY_MISSION_POINT_AWARD_REQ);

	// 世界BOSS
	RESISTER_MSG(C2S_WORLD_BOSS_DATA_REQ);
	RESISTER_MSG(C2S_WORLD_BOSS_SELECT_CAMP_REQ);
	RESISTER_MSG(C2S_WORLD_BOSS_REWARD_REQ);
	RESISTER_MSG(C2S_WORLD_BOSS_REWARD_RECORD_REQ);
	RESISTER_MSG(C2S_WORLD_BOSS_BATTLE_RECORD_REQ);
	RESISTER_MSG(C2S_WORLD_BOSS_BATTLE_TIMES_BUY_REQ);

	// 竞技场
	RESISTER_MSG(C2S_ARENA_TARGET_LIST_REQ);
	RESISTER_MSG(C2S_ARENA_BATTLE_RECORD_REQ);

	// 物品
	RESISTER_MSG(C2S_ITEM_LIST_REQ);
	RESISTER_MSG(C2S_ITEM_USE_REQ);
	RESISTER_MSG(C2S_ITEM_FP_REQ);
	RESISTER_MSG(C2S_ITEM_COMPOSE_REQ);
	RESISTER_MSG(C2S_ITEM_DECOMPOSE_REQ);

	// 邮件
	RESISTER_MSG(C2S_EMAIL_LIST_REQ);

	// 装备
	RESISTER_MSG(C2S_EQUIPMENT_LIST_REQ);
	RESISTER_MSG(C2S_EQUIPMENT_COMPOSE_REQ);
	//RESISTER_MSG(C2S_EQUIPMENT_FRAGMENT_LIST_REQ);
	RESISTER_MSG(C2S_EQUIPMENT_STRENG_REQ);
	RESISTER_MSG(C2S_EQUIPMENT_REFINE_REQ);
	RESISTER_MSG(C2S_EQUIPMENT_PUT_ON_REQ);
	RESISTER_MSG(C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ);
	RESISTER_MSG(C2S_EQUIPMENT_STAR_UP_REQ);
	RESISTER_MSG(C2S_EQUIPMENT_QUALITY_UP_REQ);


	// 宝物
	RESISTER_MSG(C2S_TREASURE_LIST_REQ);
	RESISTER_MSG(C2S_TREASURE_STRENGTHEN_REQ);
	RESISTER_MSG(C2S_TREASURE_REFINE_REQ);
	RESISTER_MSG(C2S_TREASURE_COMPOSE_REQ);
	RESISTER_MSG(C2S_TREASURE_PUT_ON_REQ);
	RESISTER_MSG(C2S_TREASURE_SMELT_REQ);
	RESISTER_MSG(C2S_TREASURE_QUALITY_UP_REQ);

	// 叛军
	RESISTER_MSG(C2S_REBEL_DATA_REQ);
	RESISTER_MSG(C2S_REBEL_EXPLOIT_AWARD_REQ);
	RESISTER_MSG(C2S_REBEL_EXPLOIT_AWARD_RECORD_REQ);
	RESISTER_MSG(C2S_REBEL_SHARE_REQ);

	// 回收
	RESISTER_MSG(C2S_RECYCLE_DECOMPOSE_REQ);
	RESISTER_MSG(C2S_RECYCLE_RESET_REQ);
	RESISTER_MSG(C2S_RECYCLE_DECOMPOSE_RETURN_REQ);
	RESISTER_MSG(C2S_RECYCLE_RESET_RETURN_REQ);

	// 夺宝
	RESISTER_MSG(C2S_DUOBAO_DATA_REQ);
	RESISTER_MSG(C2S_DUOBAO_TARGET_LIST_REQ);
	RESISTER_MSG(C2S_DUOBAO_FIVE_TIMES_REQ);
	RESISTER_MSG(C2S_TAKE_DUOBAO_PROGRESS_REWARD_REQ);

	// 英雄
	RESISTER_MSG(C2S_HERO_BASE_LIST_REQ);
	RESISTER_MSG(C2S_HERO_INFO_REQ);
	//RESISTER_MSG(C2S_FRAGMENT_LIST_REQ);
	RESISTER_MSG(C2S_HERO_TO_BATTLE_REQ);
	RESISTER_MSG(C2S_HERO_COLLECTION_LIST_REQ);
	RESISTER_MSG(C2S_HERO_COMPOSE_REQ);
	RESISTER_MSG(C2S_HERO_LEVEL_UP_REQ);
	RESISTER_MSG(C2S_HERO_BREAK_REQ);
	RESISTER_MSG(C2S_HERO_TRAIN_REQ);
	RESISTER_MSG(C2S_HERO_SUMMON_REQ);
	RESISTER_MSG(C2S_HERO_DESTINY_REQ);
	RESISTER_MSG(C2S_HERO_SELL_REQ);
	RESISTER_MSG(C2S_REINFORCEMENT_TO_BATTLE_REQ);
	RESISTER_MSG(C2S_HERO_SUMMON_DATA_REQ);
	RESISTER_MSG(C2S_HERO_AWAKEN_REQ);
	RESISTER_MSG(C2S_HERO_EQUIP_AWAKEN_ITEM_REQ);
	RESISTER_MSG(C2S_HERO_TRAIN_REPLACE_REQ);
	RESISTER_MSG(C2S_HERO_TRAIN_REWARD_GET_REQ);
	RESISTER_MSG(C2S_HERO_EVOLUTION_REQ);
	RESISTER_MSG(C2S_HERO_VIP_SUMMON_REQ);

	// 好友
	RESISTER_MSG(C2S_FRIEND_LIST_REQ);
	RESISTER_MSG(C2S_BLACK_LIST_REQ);
	RESISTER_MSG(C2S_FRIEND_APPLY_LIST_REQ);
	RESISTER_MSG(C2S_FRIEND_RECOMMEND_LIST_REQ);
	RESISTER_MSG(C2S_FRIEND_ADD_REQ);
	RESISTER_MSG(C2S_FRIEND_DEL_REQ);
	RESISTER_MSG(C2S_FRIEND_APPLY_REPLY_REQ);
	RESISTER_MSG(C2S_BLACK_ADD_REQ);
	RESISTER_MSG(C2S_BLACK_DEL_REQ);
	RESISTER_MSG(C2S_FRIEND_ENERGY_GIVE_REQ);
	RESISTER_MSG(C2S_FRIEND_ENERGY_GET_REQ);
	RESISTER_MSG(C2S_FRIEND_TARGET_FIND_REQ);

	// 奖励中心
	RESISTER_MSG(C2S_REWARD_CENTER_LIST_REQ);
	RESISTER_MSG(C2S_REWARD_CENTER_GET_REQ);

	// 签到
	RESISTER_MSG(C2S_SIGN_IN_DATA_REQ);
	RESISTER_MSG(C2S_SIGN_IN_REQ);
	RESISTER_MSG(C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ);
	RESISTER_MSG(C2S_LUXURY_SIGN_IN_DATA_REQ);

	// 迎财神
	RESISTER_MSG(C2S_YCS_DATA_REQ);
	RESISTER_MSG(C2S_YCS_REWARD_REQ);

	// 铜雀台
	RESISTER_MSG(C2S_TQT_DATA_REQ);
	RESISTER_MSG(C2S_TQT_REWARD_REQ);

	// 7天
	RESISTER_MSG(C2S_SEVEN_DAY_TARGET_LIST_REQ);
	RESISTER_MSG(C2S_SEVEN_DAY_GET_REWARD_REQ);
	RESISTER_MSG(C2S_SEVEN_DAY_COMPLETION_REWARD_REQ);

	// 占卜
	RESISTER_MSG(C2S_PLAYER_ZHANBU_DATA_REQ);
	RESISTER_MSG(C2S_CARD_LIST_REQ);
	RESISTER_MSG(C2S_FAIRY_POKEDEX_LIST_REQ);
	RESISTER_MSG(C2S_CARD_LEVEL_UP_REQ);
	RESISTER_MSG(C2S_CARD_BREAK_REQ);
	RESISTER_MSG(C2S_FAIRY_RECRUIT_REQ);
	RESISTER_MSG(C2S_FAIRY_POKEDEX_STAR_UP_REQ);
	RESISTER_MSG(C2S_FAIRY_POKEDEX_REVIVE_REQ);
	RESISTER_MSG(C2S_FAIRY_MAP_REFRESH_REQ);
	RESISTER_MSG(C2S_FAIRY_ACHIEVEMENT_ACTIVATION_REQ);
	RESISTER_MSG(C2S_CARD_REFRESH_REQ);
	RESISTER_MSG(C2S_OPEN_CARD_REQ);
	RESISTER_MSG(C2S_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_REQ);
	RESISTER_MSG(C2S_CELLECTION_GROUP_ACTIVATE_REQ);
	RESISTER_MSG(C2S_CELLECTION_GROUP_STAR_UP_REQ);
	RESISTER_MSG(C2S_COLLECTION_BOOK_LIST_REQ);
	RESISTER_MSG(C2S_CELLECTION_GROUP_RESET_REQ);
	RESISTER_MSG(C2S_COLLECTION_EQUIP_CARD_REQ);
	RESISTER_MSG(C2S_COLLECTION_UNLOAD_CARD_REQ);
	RESISTER_MSG(C2S_ZHAN_BU_ACHIEVEMENT_ACIVATE_REQ);

	// 积分赛
	RESISTER_MSG(C2S_SCORE_MATCH_DATA_REQ);
	RESISTER_MSG(C2S_SCORE_MATCH_SELECT_CAMP_REQ);
	RESISTER_MSG(C2S_SCORE_MATCH_REFRESH_TARGET_REQ);
	RESISTER_MSG(C2S_SCORE_MATCH_BUY_BATTLE_TIMES_REQ);
	RESISTER_MSG(C2S_SCORE_MATCH_TAKE_TASK_REWARD_REQ);

	// 争霸赛
	RESISTER_MSG(C2S_WARCRAFT_DATA_REQ);
	RESISTER_MSG(C2S_WARCRAFT_BUY_BATTLE_TIMES_REQ);
	RESISTER_MSG(C2S_WARCRAFT_TARGET_LIST_REQ);
	RESISTER_MSG(C2S_WARCRAFT_RANK_LIST_REQ);
	RESISTER_MSG(C2S_WARCRAFT_CHEER_REQ);

	// 称号
	RESISTER_MSG(C2S_USE_TITLE_REQ);
	RESISTER_MSG(C2S_ROLE_TITLE_INFO_REQ);

	// 红点
	RESISTER_MSG(C2S_RED_POINT_LIST_REQ);

	// VIP礼包
	RESISTER_MSG(C2S_VIP_GIFT_DATA_REQ);
	RESISTER_MSG(C2S_VIP_DAILY_GIFT_GET_REQ);
	RESISTER_MSG(C2S_VIP_WEEK_GIFT_BUY_REQ);
	RESISTER_MSG(C2S_TAKE_TODAY_RECOMMEND_REWARD_REQ);

	// 等级
	RESISTER_MSG(C2S_LEVEL_SHOP_ACTIVITY_DATA_REQ);

	// 开服基金
	RESISTER_MSG(C2S_OPEN_SERVER_FUND_DATA_REQ);
	RESISTER_MSG(C2S_OPEN_SERVER_FUND_BUY_REQ);
	RESISTER_MSG(C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ);
	RESISTER_MSG(C2S_ALL_PEOPLE_WELFARE_TAKE_REQ);

	// 限时优惠
	RESISTER_MSG(C2S_LIMIT_PREFERENTIAL_DATA_REQ);
	RESISTER_MSG(C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ);

	// 等级礼包
	RESISTER_MSG(C2S_LEVEL_GIFT_DATA_REQ);
	RESISTER_MSG(C2S_TAKE_LEVEL_GIFT_REWARD_REQ);

	// 次日奖励
	RESISTER_MSG(C2S_NEXT_DAY_REWARD_DATA_REQ);
	RESISTER_MSG(C2S_NEXT_DAY_REWARD_TAKE_REQ);

	// 玩家留言
	RESISTER_MSG(C2S_PLAYER_LEVEL_MESSAGE_REQ);

	// 关卡评论
	RESISTER_MSG(C2S_MAP_COMMENT_LIST_REQ);
	RESISTER_MSG(C2S_MAP_COMMENT_REQ);
	RESISTER_MSG(C2S_MAP_COMMENT_LIKE_REQ);

	// 充值/月卡
	RESISTER_MSG(C2S_RECHARGE_DATA_REQ);
	RESISTER_MSG(C2S_RECHARGE_REQ);
	RESISTER_MSG(C2S_MONTH_CARD_DATA_REQ);
	RESISTER_MSG(C2S_MONTH_CARD_BUY_REQ);
	RESISTER_MSG(C2S_TAKE_MONTH_CARD_REWARD_REQ);

	// PVP
	RESISTER_MSG(C2S_PVP_SEARCH_TARGET_REQ);
	//RESISTER_MSG(C2S_PVP_STOP_SEARCH_TARGET_REQ);
	RESISTER_MSG(C2S_PVP_READY_FINISH_REQ);
	RESISTER_MSG(C2S_PVP_USE_SKILL_REQ);
	RESISTER_MSG(C2S_PVP_HP_CHANGE_REQ);
	RESISTER_MSG(C2S_PVP_CREATE_BUFF_REQ);
	RESISTER_MSG(C2S_PVP_BATTLE_RESULT_REQ);
	RESISTER_MSG(C2S_PVP_TAKE_BATTLE_REWARD_REQ);
	RESISTER_MSG(C2S_PLAYER_PVP_DATA_REQ);
	RESISTER_MSG(C2S_PVP_SELECT_SPRIT_SKILL_REQ);
	RESISTER_MSG(C2S_PVP_CANCEL_SEARCH_TARGET_REQ);

	// 限时活动
	RESISTER_MSG(C2S_LIMIT_FIRST_RECHARGE_DATA_REQ);
	RESISTER_MSG(C2S_LIMIT_SINGLE_RECHARGE_DATA_REQ);
	RESISTER_MSG(C2S_LIMIT_ACCUMULATE_RECHARGE_DATA_REQ);
	RESISTER_MSG(C2S_LIMIT_RECRUIT_DATA_REQ);
	RESISTER_MSG(C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ);

	// 攻城
	RESISTER_MSG(C2S_ATTACK_CITY_DATA_REQ);
	RESISTER_MSG(C2S_ATTACK_CITY_ENTER_REQ);
	RESISTER_MSG(C2S_ATTACK_CITY_LEAVE_REQ);
	RESISTER_MSG(C2S_ATTACK_CITY_GOTO_REQ);
	RESISTER_MSG(C2S_TREASURE_BOX_LOCK_REQ);
	RESISTER_MSG(C2S_TREASURE_BOX_PICK_UP_REQ);
	RESISTER_MSG(C2S_ATTACK_CITY_RELIVE_REQ);
	RESISTER_MSG(C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ);
	RESISTER_MSG(C2S_ATTACK_CITY_ENCOURAGE_REQ);

	// 无尽之地
	RESISTER_MSG(C2S_ENDLESS_LAND_DATA_REQ);
	RESISTER_MSG(C2S_ENDLESS_LAND_ENTER_REQ);
	RESISTER_MSG(C2S_ENDLESS_LAND_BATTLE_START_REQ);
	RESISTER_MSG(C2S_ENDLESS_LAND_BATTLE_OVER_REQ);
	RESISTER_MSG(C2S_ENDLESS_LAND_KILL_MONSTER_REQ);
	RESISTER_MSG(C2S_ENDLESS_LAND_COLLISION_REQ);
	RESISTER_MSG(C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ);

	// 爵位
	RESISTER_MSG(C2S_NOBILITY_ACTIVATE_REQ);

	// 超值折扣
	RESISTER_MSG(C2S_SUPER_DISCOUNT_DATA_REQ);
	RESISTER_MSG(C2S_SUPER_DISCOUNT_BUY_REWARD_REQ);
	RESISTER_MSG(C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ);

	// 超值折扣(新)
	RESISTER_MSG(C2S_NEW_SUPER_DISCOUNT_DATA_REQ);
	RESISTER_MSG(C2S_NEW_SUPER_DISCOUNT_BUY_REWARD_REQ);
	RESISTER_MSG(C2S_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ);

	// 节日活动
	RESISTER_MSG(C2S_FESTIVAL_ACTIVITY_DATA_REQ);
	RESISTER_MSG(C2S_TAKE_FESTIVAL_ACTIVITY_REWARD_REQ);
	RESISTER_MSG(C2S_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_REQ);
	RESISTER_MSG(C2S_FESTIVAL_ACTIVITY_RED_POINT_REQ);

	// 精准推荐
	RESISTER_MSG(C2S_ACCURATE_RECOMMEND_DATA_REQ);
	RESISTER_MSG(C2S_TAKE_ACCURATE_RECOMMEND_GIFT_REQ);

	// 宝石
	RESISTER_MSG(C2S_GEM_LIST_REQ);
	RESISTER_MSG(C2S_GEM_COMPOSE_REQ);
	RESISTER_MSG(C2S_GEM_INLAY_REQ);
	RESISTER_MSG(C2S_GEM_UNLOAD_REQ);
	RESISTER_MSG(C2S_GEM_HOLE_OPEN_REQ);
	RESISTER_MSG(C2S_GEM_DECOMPOSE_REQ);

	// 组队
	RESISTER_MSG(C2S_TEAM_DUNGEON_ANSWER_QUESTION_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_DIALOG_OVER_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_NEXT_NPC_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_BOSS_TASK_START_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_LOTTERY_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_LEAVE_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_CREATE_TEAM_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_INVITE_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_ACCEPT_INVITE_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_RECOMMEND_LIST_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_MATCH_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_CANCEL_MATCH_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_ACCEPT_TASK_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_TASK_START_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_RANK_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_DATA_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_LOTTERY_RECORD_REQ);
	RESISTER_MSG(C2S_TEAM_DUNGEON_JOIN_IN_REQ);


	// 登录奖励
	RESISTER_MSG(C2S_LOGIN_REWARD_DATA_REQ);
	RESISTER_MSG(C2S_TAKE_LOGIN_REWARD_REQ);

	// 王者赛
	RESISTER_MSG(C2S_KING_MATCH_DATA_REQ);
	RESISTER_MSG(C2S_KING_MATCH_SEARCH_TARGET_REQ);
	RESISTER_MSG(C2S_TAKE_KING_MATCH_TASK_REWARD_REQ);
	RESISTER_MSG(C2S_KING_MATCH_SEASON_RANK_REQ);

	// 圣诞节
	RESISTER_MSG(C2S_CHRISTMAS_DATA_REQ);
	RESISTER_MSG(C2S_CHRISTMAS_BATTLE_ENTER_REQ);
	RESISTER_MSG(C2S_CHRISTMAS_BATTLE_LEAVE_REQ);
	RESISTER_MSG(C2S_CHRISTMAS_ATTACK_BOSS_REQ);
	RESISTER_MSG(C2S_CHRISTMAS_TAKE_BOSS_REWARD_REQ);

	// facebook活动
	RESISTER_MSG(C2S_FACEBOOK_ACTIVITY_DATA_REQ);
	RESISTER_MSG(C2S_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_REQ);
	RESISTER_MSG(C2S_TAKE_FACEBOOK_SHARE_REWARD_REQ);
	RESISTER_MSG(C2S_FACEBOOK_FRIEND_INVITE_REQ);
	RESISTER_MSG(C2S_FACEBOOK_FRIEND_SUMMON_REQ);
	RESISTER_MSG(C2S_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_REQ);
	RESISTER_MSG(C2S_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_REQ);
	RESISTER_MSG(C2S_FACEBOOK_FRIEND_INVITE_RECORD_REQ);
	RESISTER_MSG(C2S_TAKE_FACEBOOK_COMMEND_REWARD_REQ);

	// 在线奖励
	RESISTER_MSG(C2S_ONLINE_REWARD_DATA_REQ);
	RESISTER_MSG(C2S_TAKE_ONLINE_REWARD_REQ);

	// 7天登录
	RESISTER_MSG(C2S_SEVEN_DAY_LOGIN_DATA_REQ);
	RESISTER_MSG(C2S_TAKE_SEVEN_DAY_LOGIN_REWARD_REQ);

	// 充值回馈
	RESISTER_MSG(C2S_RECHARGE_REWARD_DATA_REQ);
	RESISTER_MSG(C2S_TAKE_RECHARGE_REWARD_REQ);
	RESISTER_MSG(C2S_TAKE_RECHARGE_EXTRA_REWARD_REQ);

	// 累充奖励
	RESISTER_MSG(C2S_TOTAL_RECHARGE_REWARD_DATA_REQ);
	RESISTER_MSG(C2S_TAKE_TOTAL_RECHARGE_REWARD_REQ);

	// 扭蛋抽奖
	RESISTER_MSG(C2S_ND_LOTTERY_REQ);
	RESISTER_MSG(C2S_ND_LOTTERY_RECORD_REQ);

	// 热卖商品
	RESISTER_MSG(C2S_HOT_SALE_GOODS_DATA_REQ);
	RESISTER_MSG(C2S_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_REQ);

	// 阵营招募
	RESISTER_MSG(C2S_CAMP_RECRUIT_DATA_REQ);
	RESISTER_MSG(C2S_CAMP_RECRUIT_REQ);
	RESISTER_MSG(C2S_CAMP_RECRUIT_SCORE_TASK_REWARD_REQ);
	RESISTER_MSG(C2S_CAMP_RECRUIT_RECORD_REQ);

	// unlock活动
	RESISTER_MSG(C2S_UNLOCK_ACTIVITY_DATA_REQ);
	RESISTER_MSG(C2S_SHOP_SCORE_FINISH_REQ);
	RESISTER_MSG(C2S_WATCHING_VIDEO_NOTIFY_REQ);

	// 世界等级任务
	RESISTER_MSG(C2S_WORLD_LEVEL_TASK_DATA_REQ);
	RESISTER_MSG(C2S_TAKE_WORLD_LEVEL_TASK_REWARD_REQ);
	RESISTER_MSG(C2S_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_REQ);

	// 资源战
	RESISTER_MSG(C2S_PLAYER_RESOURCE_WAR_DATA_REQ);
	RESISTER_MSG(C2S_RESOURCE_WAR_ENEMY_LIST_REQ);
	RESISTER_MSG(C2S_RESOURCE_WAR_BATTLE_RECORD_REQ);
	RESISTER_MSG(C2S_RESOURCE_WAR_GIVE_UP_OCCUPY_REQ);
	RESISTER_MSG(C2S_RESOURCE_WAR_KEEP_OCCUPY_REQ);
	RESISTER_MSG(C2S_RESOURCE_WAR_CHAPTER_DATA_REQ);
	RESISTER_MSG(C2S_RESOURCE_WAR_SAME_RESOURCE_REQ);
	RESISTER_MSG(C2S_RESOURCE_WAR_GUILD_RESOURCE_REQ);
	RESISTER_MSG(C2S_RESOURCE_WAR_TECHNOLOGY_UPGRADE_REQ);

	// 军团战
	RESISTER_MSG(C2S_GUILD_WAR_ROLE_DATA_REQ);
	RESISTER_MSG(C2S_GUILD_WAR_OCCUPY_STAR_LIST_REQ);
	RESISTER_MSG(C2S_GUILD_WAR_DECLARE_WAR_STAR_LIST_REQ);
	RESISTER_MSG(C2S_GUILD_WAR_STAR_LIST_REQ);
	RESISTER_MSG(C2S_GUILD_WAR_DECLARE_WAR_REQ);
	RESISTER_MSG(C2S_GUILD_WAR_ORGANIZE_DEFENSE_REQ);
	RESISTER_MSG(C2S_GUILD_WAR_EVACUATE_REQ);
	RESISTER_MSG(C2S_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_REQ);
	RESISTER_MSG(C2S_GUILD_WAR_TAKE_STAR_REWARD_REQ);
	RESISTER_MSG(C2S_GUILD_WAR_STAR_DETAIL_REQ);
	RESISTER_MSG(C2S_GUILD_WAR_TOWER_LIST_REQ);

	// 转盘
	RESISTER_MSG(C2S_TURNTABLE_DATA_REQ);
	RESISTER_MSG(C2S_TURNTABLE_LOTTERY_REQ);

	// 挂机
	RESISTER_MSG(C2S_HANG_UP_DROP_DATA_REQ);
	RESISTER_MSG(C2S_HANG_UP_DROP_REWARD_OBTAIN_REQ);

	return TRUE;
}

BOOL PacketHandler::ResisterOpen()
{
	//#define RESISTER_OPEN_MSG(MSG_NSP, MSG_NAME) AddHandlerOpen(MSG_NSP::MSG_NAME::descriptor()->full_name(), HandlerFromOpen::On##MSG_NAME);

	AddHandlerOpen(O2M_HEART_BEAT, HandlerFromOpen::OnMSG_O2M_HEART_BEAT);

	AddHandlerOpen(O2M_AUTHENTICATION_CODE_RESP, HandlerFromOpen::OnMSG_O2M_AUTHENTICATION_CODE);

	AddHandlerOpen(O2M_CHECK_USER_INFO_RESP, HandlerFromOpen::OnMsgCheckUserInfoResp);

	AddHandlerOpen(O2M_RECHARGE_SEND_GOODS_REQ, HandlerFromOpen::OnMsgRechargeSendGoodsReq);

	AddHandlerOpen(O2M_CP_ORDER_ID_REQ, HandlerFromOpen::OnNetCPOrderId);

	AddHandlerOpen(O2M_ONLINE_PLAYER_NUM_REQ, HandlerFromOpen::OnNetOnLinePlayerNum);

	AddHandlerOpen(O2M_YGAME_RECHARGE_SEND_GOODS_REQ, HandlerFromOpen::OnMsgYameRechargeSendGoodsReq);
	
	AddHandlerOpen(O2M_YGAME_CASHBACK_NOTIFY_REQ, HandlerFromOpen::OnMsgYameCashbackNotifyReq);

	AddHandlerOpen(O2M_YGAME_BONUS_NOTIFY_REQ, HandlerFromOpen::OnMsgYameBonusNotifyReq);
	
	return TRUE;
}
