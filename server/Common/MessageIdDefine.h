#pragma once

/*
C = Client;
G = GateServer;
M = MapServer;
O = OpenServer;
*/

/*--------------------------------------------------------------------------------------------------------------------------*/
// 1)�ͻ��˺�GateSvr֮��ͨ��ID����(��δʹ��)
/*--------------------------------------------------------------------------------------------------------------------------*/
const unsigned short C2G_MIN = 10000;
const unsigned short C2G_MAX = 19999;

/*--------------------------------------------------------------------------------------------------------------------------*/
// 2)�ͻ��˺�MapSvr֮��ͨ��ID����
/*--------------------------------------------------------------------------------------------------------------------------*/
const unsigned short C2M_MIN = 20000;
const unsigned short C2M_MAX = 29999;

/*--------------------------------------------------------------------------------------------------------------------------*/
// 3) GateSvr��MapSvr֮��ͨ��ID����
/*--------------------------------------------------------------------------------------------------------------------------*/
const unsigned short M2G_MIN = 30000;
const unsigned short M2G_MAX = 39999;

/*--------------------------------------------------------------------------------------------------------------------------*/
// 4) MapSvr��OpenSvr֮��ͨ��ID����
/*--------------------------------------------------------------------------------------------------------------------------*/
const unsigned short M2O_MIN = 40000;
const unsigned short M2O_MAX = 49999;
/*------------------------------------------------------------------------------------------------*/


enum EMsgId
{
	// ��¼
	LOGIN_START = 20000,
	C2S_LOGIN_ACCOUNT_REQ,				// �ʺŵ�¼����		x
	S2C_LOGIN_ACCOUNT_RESP,				// �ʺŵ�¼��Ӧ     
	C2S_LOGIN_SELECT_SERVER_REQ,		// ѡ�����������	x
	S2C_LOGIN_SELECT_SERVER_RESP,		// ѡ���������Ӧ	
	C2S_LOGIN_CREATE_ROLE_REQ,			// ������ɫ����		x
	S2C_LOGIN_CREATE_ROLE_RESP,			// ������ɫ��Ӧ		
	S2C_LOGIN_ENTER_GAME_NOTIFY,		// ������Ϸ	
	C2S_START_GAME_REQ,					// ��ʼ��Ϸ����		x
	S2C_START_GAME_RESP,				// ��ʼ��Ϸ��Ӧ
	C2S_ACCOUNT_REGISTER_REQ,			// �ʺ�ע������		x
	S2C_ACCOUNT_REGISTER_RESP,			// �ʺ�ע����Ӧ
	C2S_ACCOUNT_PASSWORD_CHANGE_REQ,	// �ʺ������޸�����	x
	S2C_ACCOUNT_PASSWORD_CHANGE_RESP,	// �ʺ������޸���Ӧ
	C2S_TAKE_ACTIVITY_GIFT_REQ,			// ��ȡ����	
	S2C_TAKE_ACTIVITY_GIFT_RESP,		// ��ȡ������Ӧ
	C2S_ACTIVITY_NOTICE_REQ,			// ��������		x
	S2C_ACTIVITY_NOTICE_RESP,			// ����������Ӧ
	C2S_AUTHENTICATION_CODE_REQ,		// ��֤������		x
	S2C_AUTHENTICATION_CODE_RESP,		// ��֤����Ӧ
	C2S_MOBILE_BIND_REQ,				// �ֻ��Ű�����	x
	S2C_MOBILE_BIND_RESP,				// �ֻ��Ű���Ӧ
	S2C_NEW_DAY_NOTIFY,					// �µ�һ��֪ͨ
	C2S_SDK_LOGIN_REQ,					// SDK��¼����		x
	S2C_SDK_LOGIN_RESP,					// SDK��¼��Ӧ
	C2S_SDK_ORDER_ID_REQ,				// SDK����������	x
	S2C_SDK_ORDER_ID_RESP,				// SDK��������Ӧ
	C2S_RECHARGE_SUCCESS_REPORT_REQ,	// ��ֵ�ɹ��ϱ�����	x
	S2C_RECHARGE_SUCCESS_REPORT_RESP,	// ��ֵ�ɹ��ϱ���Ӧ
	S2C_CLIENT_UPDATE_NOTIFY,			// �ͻ��˸���֪ͨ

	C2S_LOGIN_REQ,						// ��¼����
	S2C_LOGIN_RESP,						// ��¼��Ӧ

	// ��ɫϵͳ
	ROLE_START = 20100,
	S2C_GAME_ERROR_NOTIFY,				// ��Ϸ������ʾ     
	S2C_ROLE_INFO_NOTIFY,				// ��ɫ������Ϣ		
 	S2C_ROLE_PROPERTY_NOTIFY,			// ��ɫ������Ϣͬ�� 
	S2C_ROLE_LEVEL_UP_NOTIFY,			// ��ɫ����			
	C2S_GUIDE_UPDATE_REQ,				// ������������	
	S2C_GUIDE_UPDATE_RESP,				// ����������Ӧ	
	C2S_HEART_BEAT_REQ,					// �ͻ���������	x
	S2C_HEART_BEAT_RESP,				// �����������
	C2S_SERVER_TIME_REQ,				// ��ȡ������ʱ�� x
	S2C_SERVER_TIME_RESP,				// ���ط�����ʱ��
	C2S_ROLE_DETAILS_REQ,				// ��ɫ��ϸ���� x
	S2C_ROLE_DETAILS_RESP,				// ��ɫ��ϸ��Ӧ
	C2S_ROLE_PRO_RECOVER_REQ,			// ��ɫ���Իָ�����
	S2C_ROLE_PRO_RECOVER_RESP,			// ��ɫ���Իָ���Ӧ
	C2S_HEAD_IMAGE_REPLACE_REQ,			// ͷ���滻����
	S2C_HEAD_IMAGE_REPLACE_RESP,		// ͷ���滻��Ӧ
	C2S_ROLE_NAME_MODIFY_REQ,			// ��ɫ�����޸�����
	S2C_ROLE_NAME_MODIFY_RESP,			// ��ɫ�����޸���Ӧ
	S2C_SYSTEM_NOTICE,					// ϵͳ����
	C2S_HERO_GOTO_REQ,					// Ӣ���������� x
	S2C_HERO_GOTO_RESP,					// Ӣ��������Ӧ
	S2C_HERO_GOTO_NOTIFY,				// Ӣ������֪ͨ
	C2S_ENTER_SCENE_REQ,				// ���볡������ x
	S2C_ENTER_SCENE_RESP,				// ���볡����Ӧ
	S2C_ENTER_SCENE_NOTICE,				// ���볡��֪ͨ
	C2S_LEAVE_SCENE_REQ,				// �뿪�������� x
	S2C_LEAVE_SCENE_RESP,				// �뿪������Ӧ
	S2C_LEAVE_SCENE_NOTICE,				// �뿪����֪ͨ
	S2C_SCENE_MONSTER_LIST_NOTICE,		// �����б�
	S2C_SCENE_MONSTER_DATA_NOTICE,		// ��������֪ͨ
	S2C_SCENE_ITEM_LIST_NOTICE,			// ������Ʒ�б�(1.���� 2.�ɼ���)
	S2C_SCENE_ITEM_DISAPPEAR_NOTICE,	// ������Ʒ��ʧ֪ͨ
	C2S_SCENE_ITEM_LOCK_REQ,			// ������Ʒ��ס���� x
	S2C_SCENE_ITEM_LOCK_RESP,			// ������Ʒ��ס��Ӧ
	C2S_SCENE_ITEM_PICK_UP_REQ,			// ������Ʒʰȡ���� x
	S2C_SCENE_ITEM_PICK_UP_RESP,		// ������Ʒʰȡ��Ӧ
	S2C_SCENE_ITEM_LOCK_NOTIFY,			// ������Ʒ��ס֪ͨ
	S2C_SCENE_ITEM_PICK_UP_NOTIFY,		// ������Ʒʰȡ֪ͨ
	C2S_NEW_ROLE_GUIDE_UPDATE_REQ,		// ������������

	// GM
	GM_START = 20200,
	C2M_GM_REQ	= GM_START,		// ����GM����

	//�ʼ�
	EMAIL_START = 20300,
	C2S_EMAIL_LIST_REQ,				//�ʼ��б����� x
	S2C_EMAIL_LIST_RESP,			//�ʼ��б���Ӧ
	S2C_EMAIL_NEW_NOTIFY,			//���ʼ�֪ͨ

	//����ϵͳ
	CHAT_START = 20400,
	C2S_CHAT_REQ,					//�������� x
	S2C_CHAT_RESP,					//����Ӧ��
	S2C_CHAT_NOTIFY,				//����֪ͨ
	C2S_CHAT_VOICE_REQ,				//������������
	S2C_CHAT_VOICE_RESP,			//����������Ӧ
	S2C_SYS_BROADCAST_NOTIFY,		//ϵͳ����㲥
	C2S_CHAT_RECORD_LIST_REQ,		//�����¼�б����� x
	S2C_CHAT_RECORD_LIST_RESP,		//�����¼�б���Ӧ

	//������ϵͳ
	ROBOT_START = 20500,
	C2M_ROBOT_LOGIN_SYN,			// �����˵�¼����
	M2C_ROBOT_LOGIN_ACK,			// �����˵�¼Ӧ��

	// ս��ϵͳ
	BATTLE_START = 20600,
	C2S_BATTLE_START_REQ,			// ����ս��
	S2C_BATTLE_START_RESP,			// ����ս����Ӧ
	C2S_BATTLE_END_REQ,				// ս������
	S2C_BATTLE_END_RESP,			// ս��������Ӧ
	C2S_BATTLE_DAMAGE_REQ,			// �˺�ͬ��
//	S2C_BATTLE_DAMAGE_RESP,			// �˺�ͬ����Ӧ
	S2C_BATTLE_DAMAGE_NOTIFY,		// �˺�֪ͨ
	C2S_HELP_BATTLE_TARGET_REQ,		// ������սĿ�� x
	S2C_HELP_BATTLE_TARGET_RESP,	// ������սĿ����Ӧ

	// װ��
	EQUIPMENT_START = 20700,
	C2S_EQUIPMENT_LIST_REQ,				// װ���б����� x
	S2C_EQUIPMENT_LIST_RESP,			// װ���б���Ӧ
	C2S_EQUIPMENT_INFO_REQ,				// װ���������� x
	S2C_EQUIPMENT_INFO_RESP,			// װ��������Ӧ
	C2S_EQUIPMENT_STRENG_REQ,			// ǿ��
	S2C_EQUIPMENT_STRENG_RESP,			// ǿ����Ӧ
	C2S_EQUIPMENT_REFINE_REQ,			// ����
	S2C_EQUIPMENT_REFINE_RESP,			// ������Ӧ
	C2S_EQUIPMENT_COMPOSE_REQ,			// �ϳ�
	S2C_EQUIPMENT_COMPOSE_RESP,			// �ϳ���Ӧ
	C2S_EQUIPMENT_FRAGMENT_LIST_REQ,	// ����װ����Ƭ�б� x
	S2C_EQUIPMENT_FRAGMENT_LIST_RESP,	// ����װ����Ƭ�б���Ӧ
	S2C_EQUIPMENT_INFO_NOTIFY,			// װ����Ϣ֪ͨ 
	C2S_EQUIPMENT_PUT_ON_REQ,			// ����װ������ 
	S2C_EQUIPMENT_PUT_ON_RESP,			// ����װ����Ӧ
	C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ,	// һ��ǿ������
	S2C_EQUIPMENT_ONE_KEY_ENHANCE_RESP,	// һ��ǿ����Ӧ
	C2S_EQUIPMENT_STAR_UP_REQ,			// װ���������� 
	S2C_EQUIPMENT_STAR_UP_RESP,			// ����������Ӧ
	C2S_EQUIPMENT_QUALITY_UP_REQ,		// װ����Ʒ���� 
	S2C_EQUIPMENT_QUALITY_UP_RESP,		// ������Ʒ��Ӧ


	// ���а�
	RANK_START = 20800,
	C2S_RANK_LIST_REQ,					// ���а����� x
	S2C_RANK_LIST_RESP,					// ���а�������Ӧ
	C2S_RANK_LIKE_REQ,					// ����
	S2C_RANK_LIKE_RESP,					// ������Ӧ
	C2S_RANK_LEAVE_MESSAGE_REQ,			// ���� 
	S2C_RANK_LEAVE_MESSAGE_RESP,		// ������Ӧ
	C2S_RANK_LEAVE_MESSAGE_LIST_REQ,	// ���������б� x
	S2C_RANK_LEAVE_MESSAGE_LIST_RESP,	// ���������б���Ӧ
	C2S_GUILD_RANK_LIST_REQ,			// �������а����� x
	S2C_GUILD_RANK_LIST_RESP,			// �������а�������Ӧ
	C2S_MY_RANK_REQ,					// �����ҵ����� x
	S2C_MY_RANK_RESP,					// �����ҵ�������Ӧ

	// �̵�
	SHOP_START = 20900,
	C2S_SHOP_GOODS_BUY_REQ,			// ��Ʒ��������
	S2C_SHOP_GOODS_BUY_RESP,		// ��Ʒ������Ӧ
	C2S_SHOP_GOODS_BUY_RECORD_REQ,	// ��Ʒ�����¼���� x
	S2C_SHOP_GOODS_BUY_RECORD_RESP,	// ��Ʒ�����¼��Ӧ
	C2S_SHOP_GOODS_SELL_REQ,		// ��Ʒ��������
	S2C_SHOP_GOODS_SELL_RESP,		// ��Ʒ������Ӧ
	C2S_SHOP_LIMIT_GOODS_LIST_REQ,	// ��ʱ��Ʒ�б����� x
	S2C_SHOP_LIMIT_GOODS_LIST_RESP,	// ��ʱ��Ʒ�б���Ӧ
	C2S_SHOP_REFRESH_REQ,			// ˢ���̵�
	S2C_SHOP_REFRESH_RESP,			// ˢ���̵���Ӧ
	C2S_HERO_SHOP_DATA_REQ,			// Ӣ���̵���������
	S2C_HERO_SHOP_DATA_RESP,		// Ӣ���̵�������Ӧ
	C2S_AWAKEN_SHOP_DATA_REQ,		// �����̵���������
	S2C_AWAKEN_SHOP_DATA_RESP,		// �����̵�������Ӧ
	C2S_STAR_SOUL_SHOP_DATA_REQ,	// �ǻ��̵���������
	S2C_STAR_SOUL_SHOP_DATA_RESP,	// �ǻ��̵�������Ӧ
	C2S_DIRECT_SHOP_DATA_REQ,		// ֱ���̵���������
	S2C_DIRECT_SHOP_DATA_RESP,		// ֱ���̵�������Ӧ
	C2S_TAKE_DIRECT_BUY_GOODS_REQ,	// ��ȡֱ����Ʒ��������
	S2C_TAKE_DIRECT_BUY_GOODS_RESP,	// ��ȡֱ����Ʒ������Ӧ
	C2S_CRYSTAL_SHOP_DATA_REQ,		// ˮ���̵���������
	S2C_CRYSTAL_SHOP_DATA_RESP,		// ˮ���̵�������Ӧ

	// ����
	FUBEN_START = 21000,
	C2S_FB_MAIN_DATA_REQ,			// ���߸����������� x
	S2C_FB_MAIN_DATA_RESP,			// ���߸���������Ӧ
	C2S_FB_AWARD_GET_REQ,			// ����������ȡ���� x
	S2C_FB_AWARD_GET_RESP,			// ����������ȡ��Ӧ
	C2S_FB_DAILY_DATA_REQ,			// �ճ������������� x
	S2C_FB_DAILY_DATA_RESP,			// �ճ�����������Ӧ
	C2S_FB_HERO_DATA_REQ,			// Ӣ�۸����������� x
	S2C_FB_HERO_DATA_RESP,			// Ӣ�۸���������Ӧ
	C2S_FB_CLEAN_REQ,				// ����ɨ������
	S2C_FB_CLEAN_RESP,				// ����ɨ����Ӧ
	C2S_FB_ELITE_DATA_REQ,			// ��Ӣ������������ x
	S2C_FB_ELITE_DATA_RESP,			// ��Ӣ����������Ӧ
	C2S_FB_RESET_REQ,				// ��������
	S2C_FB_RESET_RESP,				// ��������
	C2S_FB_ONE_KEY_TAKE_REWARD_REQ,	// һ����ȡ��������(�ؿ�+����)
	S2C_FB_ONE_KEY_TAKE_REWARD_RESP,// һ����ȡ������Ӧ(�ؿ�+����)
	C2S_TAKE_MAIN_FB_CHAPTER_REWARD_REQ, // ��ȡ���߸����½ڽ������� x
	S2S_TAKE_MAIN_FB_CHAPTER_REWARD_RESP, // ��ȡ���߸����½ڽ�����Ӧ

	// ��Ʒ
	ITEM_START = 21100,
	C2S_ITEM_LIST_REQ,				// ��Ʒ�б����� x
	S2C_ITEM_LIST_RESP,				// ��Ʒ�б���Ӧ
	C2S_ITEM_USE_REQ,				// ʹ����Ʒ����
	S2C_ITEM_USE_RESP,				// ʹ����Ʒ��Ӧ
	S2C_ITEM_DATA_NOTIFY,			// ��Ʒ����ͬ��
	C2S_ITEM_FP_REQ,				// ��Ʒ��������
	S2C_ITEM_FP_RESP,				// ��Ʒ������Ӧ
	C2S_ITEM_COMPOSE_REQ,			// ��Ʒ�ϳ�����
	S2C_ITEM_COMPOSE_RESP,			// ��Ʒ�ϳ���Ӧ
	C2S_ITEM_DECOMPOSE_REQ,			// ��Ʒ�ֽ����� 
	S2C_ITEM_DECOMPOSE_RESP,		// ��Ʒ�ֽ���Ӧ

	// Ӣ��
	HERO_START = 21200,
	C2S_HERO_BASE_LIST_REQ,			// ����Ӣ�ۻ�����Ϣ�б� x
	S2C_HERO_BASE_LIST_RESP,		// ����Ӣ�ۻ�����Ϣ�б���Ӧ	
	C2S_HERO_INFO_REQ,				// ����Ӣ����ϸ��Ϣ x
	S2C_HERO_INFO_RESP,				// ����Ӣ����ϸ��Ϣ��Ӧ
	C2S_FRAGMENT_LIST_REQ,			// ������Ƭ�б� x
	S2C_FRAGMENT_LIST_RESP,			// ������Ƭ�б���Ӧ
	C2S_HERO_TO_BATTLE_REQ,			// ����Ӣ������ 
	S2C_HERO_TO_BATTLE_RESP,		// Ӣ��������Ӧ
	C2S_HERO_COLLECTION_LIST_REQ,	// ����Ӣ���ռ��б� x
	S2C_HERO_COLLECTION_LIST_RESP,	// ����Ӣ���ռ��б���Ӧ
	C2S_HERO_COMPOSE_REQ,			// �ϳ�����
	S2C_HERO_COMPOSE_RESP,			// �ϳ���Ӧ
	C2S_HERO_LEVEL_UP_REQ,			// ��������
	S2C_HERO_LEVEL_UP_RESP,			// ������Ӧ
	C2S_HERO_BREAK_REQ,				// ͻ������
	S2C_HERO_BREAK_RESP,			// ͻ����Ӧ
	C2S_HERO_TRAIN_REQ,				// ��������
	S2C_HERO_TRAIN_RESP,			// ������Ӧ
	C2S_HERO_SUMMON_REQ,			// �ٻ�����
	S2C_HERO_SUMMON_RESP,			// �ٻ���Ӧ
	S2C_HERO_BASE_INFO_NOTIFY,		// Ӣ�ۻ�����Ϣ֪ͨ
	S2C_FORMATION_LIST_NOTIFY,		// �����б�֪ͨ x
	C2S_HERO_DESTINY_REQ,			// ����(Ǳ��)����
	S2C_HERO_DESTINY_RESP,			// ����(Ǳ��)��Ӧ
	C2S_HERO_SELL_REQ,				// ��������
	S2C_HERO_SELL_RESP,				// ������Ӧ
	C2S_REINFORCEMENT_TO_BATTLE_REQ,// Ԯ������
	S2C_REINFORCEMENT_TO_BATTLE_RESP,// Ԯ��������Ӧ
	C2S_HERO_SUMMON_DATA_REQ,		// �ٻ��������� x
	S2C_HERO_SUMMON_DATA_RESP,		// �ٻ�������Ӧ
	C2S_HERO_AWAKEN_REQ,			// ��������
	S2C_HERO_AWAKEN_RESP,			// ������Ӧ
	C2S_HERO_EQUIP_AWAKEN_ITEM_REQ,	// װ�����ѵ�������
	S2C_HERO_EQUIP_AWAKEN_ITEM_RESP,// װ�����ѵ�������
	C2S_HERO_TRAIN_REPLACE_REQ,		// Ӣ�������滻����
	S2C_HERO_TRAIN_REPLACE_RESP,	// Ӣ�������滻��Ӧ
	C2S_HERO_TRAIN_REWARD_GET_REQ,	// Ӣ������������ȡ����
	S2C_HERO_TRAIN_REWARD_GET_RESP,	// Ӣ������������ȡ��Ӧ
	C2S_HERO_EVOLUTION_REQ,			// ��������
	S2C_HERO_EVOLUTION_RESP,		// ������Ӧ
	C2S_HERO_VIP_SUMMON_REQ,		// VIP�ٻ�����
	S2C_HERO_VIP_SUMMON_RESP,		// VIP�ٻ���Ӧ

	// ����
	FRIEND_START = 21300,
	C2S_FRIEND_LIST_REQ,			// ��������б� x
	S2C_FRIEND_LIST_RESP,			// ��������б���Ӧ						
	C2S_BLACK_LIST_REQ,				// ����������б� x
	S2C_BLACK_LIST_RESP,			// ����������б���Ӧ							
	C2S_FRIEND_APPLY_LIST_REQ,		// ���������б� x
	S2C_FRIEND_APPLY_LIST_RESP,		// ���������б���Ӧ							
	C2S_FRIEND_RECOMMEND_LIST_REQ,	// �����Ƽ��б� x
	S2C_FRIEND_RECOMMEND_LIST_RESP,	// �Ƽ��б���Ӧ
	C2S_FRIEND_ADD_REQ,				// ��Ӻ���
	S2C_FRIEND_ADD_RESP,			// ��Ӻ�����Ӧ						
	C2S_FRIEND_DEL_REQ,				// ɾ������
	S2C_FRIEND_DEL_RESP,			// ɾ��������Ӧ							
	C2S_FRIEND_APPLY_REPLY_REQ,		// ������ѻظ�
	S2C_FRIEND_APPLY_REPLY_RESP,	// ������ѻظ���Ӧ
	C2S_BLACK_ADD_REQ,				// ��Ӻ�����
	S2C_BLACK_ADD_RESP,				// ��Ӻ�������Ӧ
	C2S_BLACK_DEL_REQ,				// ɾ��������
	S2C_BLACK_DEL_RESP,				// ɾ����������Ӧ
	C2S_FRIEND_ENERGY_GIVE_REQ,		// ���;���
	S2C_FRIEND_ENERGY_GIVE_RESP,	// ���;�����Ӧ
	C2S_FRIEND_ENERGY_GET_REQ,		// ��ȡ����
	S2C_FRIEND_ENERGY_GET_RESP,		// ��ȡ������Ӧ
	S2C_FRIEND_DATA_UPDATE_NOTIFY,	// �������ݸ���֪ͨ
	C2S_FRIEND_TARGET_FIND_REQ,		// Ŀ����� x
	S2C_FRIEND_TARGET_FIND_RESP,	// Ŀ�������Ӧ

	// ����
	TREASURE_START = 21400,
	C2S_TREASURE_LIST_REQ,				// �����б����� x
	S2C_TREASURE_LIST_RESP,				// �����б���Ӧ
	C2S_TREASURE_STRENGTHEN_REQ,		// ����ǿ�� 
	S2C_TREASURE_STRENGTHEN_RESP,		// ����ǿ����Ӧ
	C2S_TREASURE_REFINE_REQ,			// ���ﾫ��
	S2C_TREASURE_REFINE_RESP,			// ���ﾫ����Ӧ
	C2S_TREASURE_COMPOSE_REQ,			// ����ϳ�
	S2C_TREASURE_COMPOSE_RESP,			// ����ϳ���Ӧ
	S2C_TREASURE_INFO_NOTIFY,			// ������Ϣ֪ͨ
	C2S_TREASURE_PUT_ON_REQ,			// ������������ 
	S2C_TREASURE_PUT_ON_RESP,			// ����������Ӧ
	C2S_TREASURE_SMELT_REQ,				// ������������
	S2C_TREASURE_SMELT_RESP,			// ����������Ӧ
	C2S_TREASURE_QUALITY_UP_REQ,		// ������Ʒ����
	S2C_TREASURE_QUALITY_UP_RESP,		// ������Ʒ��Ӧ

	// �Ѿ�
	REBEL_START = 21500,
	C2S_REBEL_DATA_REQ,					// �Ѿ��������� x
	S2C_REBEL_DATA_RESP,				// �Ѿ�����������Ӧ
	C2S_REBEL_EXPLOIT_AWARD_REQ,		// ��ȡ�Ѿ���ѫ��������
	S2C_REBEL_EXPLOIT_AWARD_RESP,		// ��ȡ�Ѿ���ѫ������Ӧ
	C2S_REBEL_EXPLOIT_AWARD_RECORD_REQ,	// �Ѿ���ѫ������¼���� x
	S2C_REBEL_EXPLOIT_AWARD_RECORD_RESP,// �Ѿ���ѫ������¼��Ӧ
	C2S_REBEL_SHARE_REQ,				// �Ѿ�����
	S2C_REBEL_SHARE_RESP,				// �Ѿ�������Ӧ
	S2C_REBEL_DISCOVER_NOTIFY,			// �����Ѿ�֪ͨ

	// ����
	RECYCLING_START = 21600,
	C2S_RECYCLE_DECOMPOSE_REQ,			// �ֽ��������
	S2C_RECYCLE_DECOMPOSE_RESP,			// �ֽ������Ӧ
	C2S_RECYCLE_RESET_REQ,				// ������������
	S2C_RECYCLE_RESET_RESP,				// ����������Ӧ
	C2S_RECYCLE_DECOMPOSE_RETURN_REQ,	// �ֽⷵ������ x
	S2C_RECYCLE_DECOMPOSE_RETURN_RESP,	// �ֽⷵ����Ӧ
	C2S_RECYCLE_RESET_RETURN_REQ,		// ������������ x
	S2C_RECYCLE_RESET_RETURN_RESP,		// ����������Ӧ

	// �ᱦ
	DUOBAO_START = 21700, 
	C2S_DUOBAO_DATA_REQ,				// �ᱦ�������� x
	S2C_DUOBAO_DATA_RESP,				// �ᱦ������Ӧ
	C2S_DUOBAO_TARGET_LIST_REQ,			// ����ᱦĿ���б� x
	S2C_DUOBAO_TARGET_LIST_RESP,		// ����ᱦĿ���б���Ӧ
	C2S_DUOBAO_FIVE_TIMES_REQ,			// �ᱦ5������
	S2C_DUOBAO_FIVE_TIMES_RESP,			// �ᱦ5����Ӧ
	C2S_TAKE_DUOBAO_PROGRESS_REWARD_REQ,// ��ȡ�ᱦ���Ƚ�������
	S2C_TAKE_DUOBAO_PROGRESS_REWARD_RESP,// ��ȡ�ᱦ���Ƚ�����Ӧ

	// ����
	TOWER_START = 21800,			
	C2S_TOWER_DATA_REQ,				// ������������ x
	S2C_TOWER_DATA_RESP,			// ����������Ӧ
	C2S_TOWER_BUY_TIMES_REQ,		// ������ս��������
	S2C_TOWER_BUY_TIMES_RESP,		// ������ս������Ӧ
	C2S_TOWER_RESET_REQ,			// ���ùؿ�
	S2C_TOWER_RESET_RESP,			// ���ùؿ���Ӧ
	C2S_TOWER_SELECT_PRO_REQ,		// ѡ������
	S2C_TOWER_SELECT_PRO_RESP,		// ѡ��������Ӧ
	C2S_TOWER_BUY_STAR_GOODS_REQ,	// ����������Ʒ
	S2C_TOWER_BUY_STAR_GOODS_RESP,	// ����������Ʒ��Ӧ
	C2S_TOWER_ONE_KEY_CLEAN_REQ,	// ����һ��ɨ������
	S2C_TOWER_ONE_KEY_CLEAN_RESP,	// ����һ��ɨ����Ӧ

	// ���
	LAND_START = 21900,
	C2S_PLAYER_LAND_DATA_REQ,			// �������������� x
	S2C_PLAYER_LAND_DATA_RESP,			// ����������������Ӧ
	C2S_LAND_PATROL_REQ,				// ��ʼѲ��
	S2C_LAND_PATROL_RESP,				// Ѳ����Ӧ
	C2S_LAND_VIEW_PATROL_AWARD_REQ,		// �鿴Ѳ�߽���(����Ч)
	S2C_LAND_VIEW_PATROL_AWARD_RESP,	// �鿴Ѳ�߽�����Ӧ(����Ч)
	C2S_LAND_GET_PATROL_AWARD_REQ,		// ��ȡѲ�߽���
	S2C_LAND_GET_PATROL_AWARD_RESP,		// ��ȡѲ�߽�����Ӧ
	C2S_LAND_SKILL_LEVELUP_REQ,			// ��ؼ�������(����,����)
	S2C_LAND_SKILL_LEVELUP_RESP,		// ��ؼ�������(����,����)��Ӧ
	C2S_LAND_SUPPRESS_RIOT_REQ,			// �����ѹ����
	S2C_LAND_SUPPRESS_RIOT_RESP,		// �����ѹ������Ӧ
	C2S_FRIEND_LAND_LIST_REQ,			// �����������б� x
	S2C_FRIEND_LAND_LIST_RESP,			// �����������б���Ӧ
	C2S_LAND_CITY_DETAILS_REQ,			// ��سǳ��������� x
	S2C_LAND_CITY_DETAILS_RESP,			// ��سǳ�����������Ӧ

	// ����
	SPIRIT_START = 22000,
	C2S_HEART_OF_WORLD_DATA_REQ,		// ����֮���������� x
	S2C_HEART_OF_WORLD_DATA_RESP,		// ����֮��������Ӧ
	C2S_HEART_OF_WORLD_LEVEL_UP_REQ,	// ����֮����������
	S2C_HEART_OF_WORLD_LEVEL_UP_RESP,	// ����֮��������Ӧ
	C2S_SPIRIT_ENHANCE_REQ,				// ����ǿ������
	S2C_SPIRIT_ENHANCE_RESP,			// ����ǿ����Ӧ
	C2S_SPIRIT_USE_REQ,					// ����ʹ������
	S2C_SPIRIT_USE_RESP,				// ����ʹ����Ӧ
	C2S_SPIRIT_DATA_REQ,				// ������������ x
	S2C_SPIRIT_DATA_RESP,				// ����������Ӧ
	C2S_SPIRIT_TRAIN_REQ,				// ������������
	S2C_SPIRIT_TRAIN_RESP,				// ����������Ӧ
	C2S_SPRITE_STAR_UP_REQ,				// ������������
	S2C_SPRITE_STAR_UP_RESP,			// ����������Ӧ
	C2S_SPRITE_BLESS_REQ,				// ����ף������
	S2C_SPRITE_BLESS_RESP,				// ����ף����Ӧ
	C2S_SPRITE_COMPOSE_REQ,				// ����ϳ�����
	S2C_SPRITE_COMPOSE_RESP,			// ����ϳ���Ӧ


	// ����
	GUILD_START = 22100,
	C2S_GUILD_DATE_REQ,					// ������������ x
	S2C_GUILD_DATE_RESP,				// ����������Ӧ
	C2S_GUILD_CREATE_REQ,				// ��������
	S2C_GUILD_CREATE_RESP,				// ����������Ӧ
	C2S_GUILD_DISMISS_REQ,				// ��ɢ����
	S2C_GUILD_DISMISS_RESP,				// ��ɢ������Ӧ
	C2S_GUILD_APPLY_REQ,				// ������빤�� x
	S2C_GUILD_APPLY_RESP,				// ������빤����Ӧ
	C2S_GUILD_APPLY_CHECK_REQ,			// �������
	S2C_GUILD_APPLY_CHECK_RESP,			// ���������Ӧ
	C2S_GUILD_APPLY_CANCEL_REQ,			// ȡ������		
	S2C_GUILD_APPLY_CANCEL_RESP,		// ȡ��������Ӧ
	C2S_GUILD_KICKOUT_REQ,				// �߳����� x
	S2C_GUILD_KICKOUT_RESP,				// �߳�������Ӧ
	C2S_GUILD_LEAVE_REQ,				// �˳����� x
	S2C_GUILD_LEAVE_RESP,				// �˳�������Ӧ
	C2S_GUILD_APPOINT_REQ,				// ����
	S2C_GUILD_APPOINT_RESP,				// ������Ӧ
	C2S_GUILD_LEAVE_MESSAGE_REQ,		// ����	x
	S2C_GUILD_LEAVE_MESSAGE_RESP,		// ������Ӧ
	C2S_GUILD_SEARCH_REQ,				// �������� x
	S2C_GUILD_SEARCH_RESP,				// ����������Ӧ
	C2S_GUILD_QY_REQ,					// Ȫӿ
	S2C_GUILD_QY_RESP,					// Ȫӿ��Ӧ
	C2S_GUILD_QS_AWARD_REQ,				// Ȫˮ������ȡ	
	S2C_GUILD_QS_AWARD_RESP,			// Ȫˮ������ȡ	
	C2S_GUILD_APPLY_LIST_REQ,			// �����б�����	x			
	S2C_GUILD_APPLY_LIST_RESP,			// �����б���Ӧ
	C2S_GUILD_LEAVEMSG_LIST_REQ,		// �����б����� x
	S2C_GUILD_LEAVEMSG_LIST_RESP,		// �����б���Ӧ
	C2S_GUILD_MEMBER_LIST_REQ,			// ��Ա�б����� x
	S2C_GUILD_MEMBER_LIST_RESP,			// ��Ա�б���Ӧ 
	C2S_GUILD_LIST_REQ,					// �����б����� x
	S2C_GUILD_LIST_RESP,				// �����б���Ӧ
	C2S_GUILD_INFO_MODIFY_REQ,			// �޸Ĺ�����Ϣ����
	S2C_GUILD_INFO_MODIFY_RESP,			// �޸Ĺ�����Ϣ��Ӧ
	C2S_GUILD_EVENT_LIST_REQ,			// �����¼��б����� x
	S2C_GUILD_EVENT_LIST_RESP,			// �����¼��б���Ӧ
	C2S_GUILD_SKILL_LIST_REQ,			// ���Ἴ���б����� x
	S2C_GUILD_SKILL_LIST_RESP,			// ���Ἴ���б���Ӧ
	C2S_GUILD_SKILL_LEVEL_UP_REQ,		// ������������
	S2C_GUILD_SKILL_LEVEL_UP_RESP,		// ����������Ӧ

	C2S_GUILD_FB_DATA_REQ,				// ���Ÿ����������� x
	S2C_GUILD_FB_DATA_RESP,				// ���Ÿ���������Ӧ
	C2S_GUILD_FB_CHAPTER_REWARD_REQ,	// ���Ÿ����½ڽ�������
	S2C_GUILD_FB_CHAPTER_REWARD_RESP,	// ���Ÿ����½ڽ�����Ӧ
	C2S_GUILD_FB_MAP_REWARD_REQ,		// ���Ÿ����ؿ���������
	S2C_GUILD_FB_MAP_REWARD_RESP,		// ���Ÿ����ؿ�������Ӧ
	C2S_GUILD_FB_MAP_REWARD_RECORD_REQ,	// ���Ÿ����ؿ�������¼���� x
	S2C_GUILD_FB_MAP_REWARD_RECORD_RESP,// ���Ÿ����ؿ�������¼��Ӧ
	C2S_GUILD_FB_DAMAGE_RECORD_REQ,		// ���Ÿ����˺���¼���� x
	S2C_GUILD_FB_DAMAGE_RECORD_RESP,	// ���Ÿ����˺���¼��Ӧ
	C2S_GUILD_FB_BATTLE_TIMES_BUY_REQ,	// ���Ÿ�����ս��������
	S2C_GUILD_FB_BATTLE_TIMES_BUY_RESP,	// ���Ÿ�����ս����������Ӧ
	C2S_GUILD_IMPEACH_REQ,				// ���ᵯ������
	S2C_GUILD_IMPEACH_RESP,				// ���ᵯ����Ӧ
	C2S_GUILD_MONSTER_DATA_REQ,			// ����ؿ������������� x
	S2C_GUILD_MONSTER_DATA_RESP,		// ����ؿ�����������Ӧ
	C2S_GUILD_FB_CHAPTER_RESET_REQ,		// ���ḱ���½�����
	S2C_GUILD_FB_CHAPTER_RESET_RESP,	// ���ḱ���½�������Ӧ
	S2C_GUILD_MONSNTER_DAMAGE_NOTIFY,	// ��������˺�֪ͨ

	C2S_GUILD_MY_MAP_REWARD_RECORD_REQ,	// �ҵĹؿ�������¼���� x
	S2C_GUILD_MY_MAP_REWARD_RECORD_RESP,// �ҵĹؿ�������¼��Ӧ	

	S2C_GUILD_IMPEACH_NOTICE,			// ���ᵯ��֪ͨ

	// ������
	ARENA_START = 22200,
	C2S_ARENA_TARGET_LIST_REQ,			// ��������սĿ���б����� x
	S2C_ARENA_TARGET_LIST_RESP,			// ��������սĿ���б���Ӧ
	C2S_ARENA_BATTLE_RECORD_REQ,		// ������ս������	 x
	S2C_ARENA_BATTLE_RECORD_RESP,		// ������ս������	

	// �ɾ�
	ACHIEVEMENT_START = 22300,
	C2S_ACHIEVEMENT_LIST_REQ,			// �ɾ��б����� x
	S2C_ACHIEVEMENT_LIST_RESP,			// �ɾ��б���Ӧ	
	S2C_ACHIEVEMENT_DATA_NOTIFY,		// �ɾ�����֪ͨ
	C2S_ACHIEVEMENT_AWARD_REQ,			// �ɾͽ�����ȡ���� 
	S2C_ACHIEVEMENT_AWARD_RESP,			// �ɾͽ�����ȡ��Ӧ

	// �ճ�����
	DAILY_MISSION_START = 22400,
	C2S_DAILY_MISSION_LIST_REQ,			// �ճ������б����� x
	S2C_DAILY_MISSION_LIST_RESP,		// �ճ������б�������Ӧ
	S2C_DAILY_MISSION_DATA_NOTIFY,		// �ճ���������֪ͨ
	C2S_DAILY_MISSION_AWARD_REQ,		// �ճ���������ȡ����
	S2C_DAILY_MISSION_AWARD_RESP,		// �ճ���������ȡ��Ӧ
	C2S_DAILY_MISSION_POINT_AWARD_REQ,	// �ճ�������ֽ�����ȡ����
	S2C_DAILY_MISSION_POINT_AWARD_RESP,	// �ճ�������ֽ�����ȡ��Ӧ

	// ����BOSS
	WORLD_BOSS_START = 22500,
	C2S_WORLD_BOSS_DATA_REQ,				// ����BOSS�������� x
	S2C_WORLD_BOSS_DATA_RESP,				// ����BOSS������Ӧ
	C2S_WORLD_BOSS_SELECT_CAMP_REQ,			// ѡ����Ӫ����
	S2C_WORLD_BOSS_SELECT_CAMP_RESP,		// ѡ����Ӫ��Ӧ
	C2S_WORLD_BOSS_REWARD_REQ,				// ��ȡ��������
	S2C_WORLD_BOSS_REWARD_RESP,				// ��ȡ������Ӧ
	C2S_WORLD_BOSS_REWARD_RECORD_REQ,		// ��ȡ������¼���� x
	S2C_WORLD_BOSS_REWARD_RECORD_RESP,		// ��ȡ������¼��Ӧ
	C2S_WORLD_BOSS_BATTLE_RECORD_REQ,		// ����BOSSս������ x
	S2C_WORLD_BOSS_BATTLE_RECORD_RESP,		// ����BOSSս����Ӧ
	C2S_WORLD_BOSS_BATTLE_TIMES_BUY_REQ,	// ����BOSS��ս��������
	S2C_WORLD_BOSS_BATTLE_TIMES_BUY_RESP,	// ����BOSS��ս����������Ӧ
	S2C_WORLD_BOSS_DAMAGE_NOTIFY,			// ����BOSS�˺�����
	S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY,	// ����BOSS���ʼ
	S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY,	// ����BOSS�����

	// ��������
	REWARD_CENTER_START = 22600,
	C2S_REWARD_CENTER_LIST_REQ,			// �����б����� x
	S2C_REWARD_CENTER_LIST_RESP,		// �����б���Ӧ
	C2S_REWARD_CENTER_GET_REQ,			// ������ȡ����
	S2C_REWARD_CENTER_GET_RESP,			// ������ȡ��Ӧ 
	S2C_REWARD_NOTICE,					// �콱���Ľ���֪ͨ			

	// ǩ��
	SIGN_IN_START = 22700,
	C2S_SIGN_IN_DATA_REQ,				// ǩ���������� x
	S2C_SIGN_IN_DATA_RESP,				// ǩ��������Ӧ
	C2S_SIGN_IN_REQ,					// ǩ������
	S2C_SIGN_IN_RESP,					// ǩ����Ӧ
	C2S_LUXURY_SIGN_IN_DATA_REQ,		// ����ǩ���������� x
	S2C_LUXURY_SIGN_IN_DATA_RESP,		// ����ǩ����������
	C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ,	// ��ȡ����ǩ������ 
	S2C_TAKE_LUXURY_SIGN_IN_REWARD_RESP,// ��ȡ����ǩ��������Ӧ

	// ӭ����
	YCS_START = 22800,
	C2S_YCS_DATA_REQ,					// ӭ������������ x
	S2C_YCS_DATA_RESP,					// ӭ����������Ӧ
	C2S_YCS_REWARD_REQ,					// ӭ����������
	S2C_YCS_REWARD_RESP,				// ӭ��������Ӧ

	// ͭȸ̨
	TQT_START = 22900,
	C2S_TQT_DATA_REQ,					// ͭȸ̨�������� x
	S2C_TQT_DATA_RESP,					// ͭȸ̨������Ӧ
	C2S_TQT_REWARD_REQ,					// ͭȸ̨��������
	S2C_TQT_REWARD_RESP,				// ͭȸ̨������Ӧ

	// 7��
	SEVEN_DAY_START = 23000,
	C2S_SEVEN_DAY_TARGET_LIST_REQ,		// ����Ŀ���б� x
	S2C_SEVEN_DAY_TARGET_LIST_RESP,		// ����Ŀ���б���Ӧ
	C2S_SEVEN_DAY_GET_REWARD_REQ,		// ��ȡ��������
	S2C_SEVEN_DAY_GET_REWARD_RESP,		// ��ȡ������Ӧ
	S2C_SEVEN_DAY_TARGET_DATA_NOTICE,	// 7��Ŀ������֪ͨ
	C2S_SEVEN_DAY_COMPLETION_REWARD_REQ,// ��ȡ��ɶȽ�������
	S2C_SEVEN_DAY_COMPLETION_REWARD_RESP,// ��ȡ��ɶȽ�����Ӧ

	// ռ��
	FAIRY_START = 23100,	
	C2S_PLAYER_ZHANBU_DATA_REQ,				// ���ռ����������
	S2C_PLAYER_ZHANBU_DATA_RESP,			// ���ռ��������Ӧ
	C2S_CARD_LIST_REQ,						// �����б�����
	S2C_CARD_LIST_RESP,						// �����б���Ӧ
	C2S_FAIRY_POKEDEX_LIST_REQ,				// ����ͼ���б�����(todo::Ŀǰ��ʹ��)
	S2C_FAIRY_POKEDEX_LIST_RESP,			// ����ͼ���б���Ӧ(todo::Ŀǰ��ʹ��)
	C2S_CARD_LEVEL_UP_REQ,					// ������������
	S2C_CARD_LEVEL_UP_RESP,					// ����������Ӧ
	C2S_CARD_BREAK_REQ,						// ����ͻ������
	S2C_CARD_BREAK_RESP,					// ����ͻ����Ӧ
	C2S_CARD_REFRESH_REQ,							// ����ˢ������
	S2C_CARD_REFRESH_RESP,							// ����ˢ����Ӧ
	C2S_OPEN_CARD_REQ,								// ��������
	S2C_OPEN_CARD_RESP,								// ������Ӧ
	C2S_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_REQ,	// �ͼ�����ˢ�´�����������
	S2C_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_RESP,	// �ͼ�����ˢ�´���������Ӧ
	C2S_CELLECTION_GROUP_ACTIVATE_REQ,				// �ղ��鼤������
	S2C_CELLECTION_GROUP_ACTIVATE_RESP,				// �ղ��鼤����Ӧ
	C2S_CELLECTION_GROUP_STAR_UP_REQ,				// �ղ�����������
	S2C_CELLECTION_GROUP_STAR_UP_RESP,				// �ղ���������Ӧ
	S2C_FAIRY_DATA_NOTICE,							// ��������֪ͨ
	C2S_CELLECTION_GROUP_RESET_REQ,					// �ղ�������(ȡ��)����
	S2C_CELLECTION_GROUP_RESET_RESP,				// �ղ�������(ȡ��)��Ӧ
	C2S_COLLECTION_BOOK_LIST_REQ,					// �ղ����б�����
	S2C_COLLECTION_BOOK_LIST_RESP,					// �ղ����б���Ӧ
	C2S_COLLECTION_EQUIP_CARD_REQ,					// װ����������
	S2C_COLLECTION_EQUIP_CARD_RESP,					// װ��������Ӧ
	C2S_COLLECTION_UNLOAD_CARD_REQ,					// ж�¿�������
	S2C_COLLECTION_UNLOAD_CARD_RESP,				// ж�¿�����Ӧ
	C2S_ZHAN_BU_ACHIEVEMENT_ACIVATE_REQ,			// ռ���ɾͼ�������
	S2C_ZHAN_BU_ACHIEVEMENT_ACIVATE_RESP,			// ռ���ɾͼ�����Ӧ

	C2S_FAIRY_RECRUIT_REQ,					// ������ļ����
	S2C_FAIRY_RECRUIT_RESP,					// ������ļ��Ӧ
	C2S_FAIRY_POKEDEX_STAR_UP_REQ,			// ����ͼ����������
	S2C_FAIRY_POKEDEX_STAR_UP_RESP,			// ����ͼ��������Ӧ
	C2S_FAIRY_POKEDEX_REVIVE_REQ,			// ����ͼ����������
	S2C_FAIRY_POKEDEX_REVIVE_RESP,			// ����ͼ��������Ӧ
	C2S_FAIRY_MAP_REFRESH_REQ,				// ����ؿ�ˢ������
	S2C_FAIRY_MAP_REFRESH_RESP,				// ����ؿ�ˢ����Ӧ
	C2S_FAIRY_ACHIEVEMENT_ACTIVATION_REQ,	// ����ͼ����������
	S2C_FAIRY_ACHIEVEMENT_ACTIVATION_RESP,	// ����ͼ��������Ӧ
	S2C_FAIRY_POKEDEX_DATA_NOTICE,			// ����ͼ������֪ͨ


	// ������
	SCORE_MATCH_START = 23200,
	C2S_SCORE_MATCH_DATA_REQ,				// �������������� x
	S2C_SCORE_MATCH_DATA_RESP,				// ������������Ӧ
	C2S_SCORE_MATCH_SELECT_CAMP_REQ,		// ѡ����Ӫ
	S2C_SCORE_MATCH_SELECT_CAMP_RESP,		// ѡ����Ӫ��Ӧ
	C2S_SCORE_MATCH_REFRESH_TARGET_REQ,		// ˢ��Ŀ�� x
	S2C_SCORE_MATCH_REFRESH_TARGET_RESP,	// ˢ��Ŀ����Ӧ
	C2S_SCORE_MATCH_BUY_BATTLE_TIMES_REQ,	// ������ս����
	S2C_SCORE_MATCH_BUY_BATTLE_TIMES_RESP,	// ������ս������Ӧ
	C2S_SCORE_MATCH_TAKE_TASK_REWARD_REQ,	// ��ȡʤ����������	
	S2C_SCORE_MATCH_TAKE_TASK_REWARD_RESP,	// ��ȡʤ������������Ӧ

	// ������
	WARCRAFT_START = 23300,
	C2S_WARCRAFT_DATA_REQ,					// ��������������
	S2C_WARCRAFT_DATA_RESP,					// ������������Ӧ
	C2S_WARCRAFT_BUY_BATTLE_TIMES_REQ,		// ������ս��������
	S2C_WARCRAFT_BUY_BATTLE_TIMES_RESP,		// ������ս������Ӧ
	C2S_WARCRAFT_TARGET_LIST_REQ,			// ��սĿ������
	S2C_WARCRAFT_TARGET_LIST_RESP,			// ��սĿ����Ӧ
	C2S_WARCRAFT_RANK_LIST_REQ,				// ���������а�����
	S2C_WARCRAFT_RANK_LIST_RESP,			// ���������а���Ӧ
	C2S_WARCRAFT_CHEER_REQ,					// ��������������
	S2C_WARCRAFT_CHEER_RESP,				// ������������Ӧ

	// �ƺ�
	TITLE_START = 23400,
	C2S_ROLE_TITLE_INFO_REQ,				// �����ɫ�ƺ���Ϣ x
	S2C_ROLE_TITLE_INFO_RESP,				// �����ɫ�ƺ���Ϣ��Ӧ
	C2S_USE_TITLE_REQ,						// ʹ�óƺ�����
	S2C_USE_TITLE_RESP,						// ʹ�óƺ���Ӧ

	// ���
	RED_POINT_START = 23500,
	C2S_RED_POINT_LIST_REQ,					// �������б� x
	S2C_RED_POINT_LIST_RESP,				// �������б���Ӧ
	S2C_RED_POINT_NOTIFY,					// ���֪ͨ

	// VIP���
	VIP_GIFT_START = 23600,
	C2S_VIP_GIFT_DATA_REQ,				// VIP����������� x
	S2C_VIP_GIFT_DATA_RESP,				// VIP���������Ӧ
	C2S_VIP_DAILY_GIFT_GET_REQ,			// VIPÿ�������ȡ
	S2C_VIP_DAILY_GIFT_GET_RESP,		// VIPÿ�������ȡ��Ӧ
	C2S_VIP_WEEK_GIFT_BUY_REQ,			// VIP���������
	S2C_VIP_WEEK_GIFT_BUY_RESP,			// VIP�����������Ӧ
	C2S_TAKE_TODAY_RECOMMEND_REWARD_REQ,// ��ȡ�����Ƽ�����
	S2C_TAKE_TODAY_RECOMMEND_REWARD_RESP,// ��ȡ�����Ƽ�������Ӧ

	// �ȼ��̵�����
	LEVEL_SHOP_ACTIVITY_START = 23700,
	C2S_LEVEL_SHOP_ACTIVITY_DATA_REQ,	// �ȼ��̵��������� x
	S2C_LEVEL_SHOP_ACTIVITY_DATA_RESP,	// �ȼ��̵�������Ӧ

	// ��������
	OPEN_SERVER_FUND_START = 23800,
	C2S_OPEN_SERVER_FUND_DATA_REQ,		// ���������������� x
	S2C_OPEN_SERVER_FUND_DATA_RESP,		// ��������������Ӧ
	C2S_OPEN_SERVER_FUND_BUY_REQ,		// ��������������
	S2C_OPEN_SERVER_FUND_BUY_RESP,		// ������������Ӧ
	C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ,	// ������������ȡ����
	S2C_OPEN_SERVER_FUND_REWARD_TAKE_RESP,	// ������������ȡ��Ӧ
	C2S_ALL_PEOPLE_WELFARE_TAKE_REQ,		// ȫ������ȡ����
	S2C_ALL_PEOPLE_WELFARE_TAKE_RESP,		// ȫ������ȡ��Ӧ

	// ��ʱ�Ż�
	LIMIT_PREFERENTIAL_START = 23900,
	C2S_LIMIT_PREFERENTIAL_DATA_REQ,			// ��ʱ�Ż���������
	S2C_LIMIT_PREFERENTIAL_DATA_RESP,			// ��ʱ�Ż�������Ӧ
	C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ,	// ��ʱ�Żݸ�����ȡ����
	S2C_LIMIT_PREFERENTIAL_WELFARE_TAKE_RESP,	// ��ʱ�Żݸ�����ȡ��Ӧ

	// �ȼ����
	LEVEL_GIFT_START = 24000,
	C2S_LEVEL_GIFT_DATA_REQ,					// �ȼ������������ x
	S2C_LEVEL_GIFT_DATA_RESP,					// �ȼ����������Ӧ
	C2S_TAKE_LEVEL_GIFT_REWARD_REQ,				// ��ȡ�ȼ��������
	S2C_TAKE_LEVEL_GIFT_REWARD_RESP,			// ��ȡ�ȼ������Ӧ

	// �������
	PLAYER_LEAVE_MESSAGE = 24100,
	C2S_PLAYER_LEVEL_MESSAGE_REQ,			// ����������� x
	S2C_PLAYER_LEVEL_MESSAGE_RESP,			// ���������Ӧ

	// �ؿ�����
	MAP_COMMENT_START = 24200,
	C2S_MAP_COMMENT_LIST_REQ,				// �ؿ������б����� x
	S2C_MAP_COMMENT_LIST_RESP,				// �ؿ������б���Ӧ
	C2S_MAP_COMMENT_REQ,					// �ؿ���������		 
	S2C_MAP_COMMENT_RESP,					// �ؿ�������Ӧ
	C2S_MAP_COMMENT_LIKE_REQ,				// �ؿ����۵���
	S2C_MAP_COMMENT_LIKE_RESP,				// �ؿ����۵�����Ӧ

	// ��ֵ/�¿�
	MAP_RECHARGE_START = 24300,
	C2S_RECHARGE_DATA_REQ,					// ��ֵ�������� x
	S2C_RECHARGE_DATA_RESP,					// ��ֵ������Ӧ
	C2S_RECHARGE_REQ,						// ��ֵ����
	S2C_RECHARGE_RESP,						// ��ֵ��Ӧ
	C2S_MONTH_CARD_DATA_REQ,				// �¿��������� x
	S2C_MONTH_CARD_DATA_RESP,				// �¿�������Ӧ
	C2S_MONTH_CARD_BUY_REQ,					// �¿�����
	S2C_MONTH_CARD_BUY_RESP,				// �¿�������Ӧ
	C2S_TAKE_MONTH_CARD_REWARD_REQ,			// ��ȡ�¿�����
	S2C_TAKE_MONTH_CARD_REWARD_RESP,		// ��ȡ�¿�������Ӧ
	S2C_RECHARGE_NOTIFY_RESP,				// ��ֵ֪ͨ

	// PVP
	PVP_START = 24400,
	C2S_PVP_SEARCH_TARGET_REQ,				// ����Ŀ�� x
	S2C_PVP_SEARCH_TARGET_RESP,				// ����Ŀ����Ӧ
	S2C_PVP_READY_NOTIFY,					// ս��׼��֪ͨ 
	C2S_PVP_READY_FINISH_REQ,				// ս��׼��������� x
	S2C_PVP_READY_FINISH_RESP,				// ս��׼�������Ӧ
	S2C_PVP_BATTLE_START_NOTIFY,			// ս����ʼ֪ͨ
	S2C_PVP_BATTLE_OVER_NOTIFY,				// ս������֪ͨ
	C2S_PVP_USE_SKILL_REQ,					// ʹ�ü��� x
	S2C_PVP_USE_SKILL_RESP,					// ʹ�ü�����Ӧ
	S2C_PVP_USE_SKILL_NOTIFY,				// ʹ�ü���֪ͨ
	C2S_PVP_HP_CHANGE_REQ,					// HP�ı����� x
	S2C_PVP_HP_CHANGE_RESP,					// HP�ı���Ӧ
	S2C_PVP_HP_CHANGE_NOTIFY,				// HP�ı�֪ͨ
	C2S_PVP_CREATE_BUFF_REQ,				// ����buff���� x
	S2C_PVP_CREATE_BUFF_RESP,				// ����buff��Ӧ
	S2C_PVP_CREATE_BUFF_NOTIFY,				// ����buff֪ͨ
	C2S_PVP_BATTLE_RESULT_REQ,				// ս���������
	S2C_PVP_BATTLE_RESULT_RESP,				// ս�������Ӧ
	C2S_PVP_TAKE_BATTLE_REWARD_REQ,			// ��ȡս������	
	S2C_PVP_TAKE_BATTLE_REWARD_RESP,		// ��ȡս��������Ӧ
	C2S_PLAYER_PVP_DATA_REQ,				// ���PVP�������� x
	S2C_PLAYER_PVP_DATA_RESP,				// ���PVP������Ӧ
	C2S_PVP_SELECT_SPRIT_SKILL_REQ,			// ѡ���鼼������
	S2C_PVP_SELECT_SPRIT_SKILL_RESP,		// ѡ���鼼����Ӧ
	//C2S_PVP_POSITION_SYN_REQ,				// λ��ͬ��
	C2S_PVP_CANCEL_SEARCH_TARGET_REQ,		// ȡ������Ŀ������ x
	S2C_PVP_CANCEL_SEARCH_TARGET_RESP,		// ȡ������Ŀ����Ӧ


	// ��ʱ�
	LIMIT_ACTIVITY_START = 24500,
	C2S_LIMIT_FIRST_RECHARGE_DATA_REQ,			// �׳��������� x
	S2C_LIMIT_FIRST_RECHARGE_DATA_RESP,			// �׳�������Ӧ
	C2S_LIMIT_SINGLE_RECHARGE_DATA_REQ,			// ������������ x
	S2C_LIMIT_SINGLE_RECHARGE_DATA_RESP,		// ����������Ӧ
	C2S_LIMIT_ACCUMULATE_RECHARGE_DATA_REQ,		// �۳��������� x
	S2C_LIMIT_ACCUMULATE_RECHARGE_DATA_RESP,	// �۳�������Ӧ
	C2S_LIMIT_RECRUIT_DATA_REQ,					// ��ʱ��ļ�������� x
	S2C_LIMIT_RECRUIT_DATA_RESP,				// ��ʱ��ļ������Ӧ
	C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ,			// ��ȡ��ʱ���������
	S2C_TAKE_LIMIT_ACTIVITY_REWARD_RESP,		// ��ȡ��ʱ�������Ӧ

	// ���﹥��
	ATTACK_CITY_START = 24600,
	C2S_ATTACK_CITY_DATA_REQ,					// ������������ x
	S2C_ATTACK_CITY_DATA_RESP,					// ����������Ӧ
	C2S_ATTACK_CITY_ENTER_REQ,					// ���ǽ������� x
	S2C_ATTACK_CITY_ENTER_RESP,					// ���ǽ�����Ӧ
	S2C_ATTACK_CITY_ENTER_NOTICE,				// ���ǽ���֪ͨ	
	C2S_ATTACK_CITY_LEAVE_REQ,					// �����뿪���� x
	S2C_ATTACK_CITY_LEAVE_RESP,					// �����뿪��Ӧ
	S2C_ATTACK_CITY_LEAVE_NOTICE,				// �����뿪֪ͨ
	C2S_ATTACK_CITY_GOTO_REQ,					// ������������ x
	S2C_ATTACK_CITY_GOTO_RESP,					// ����������Ӧ
	S2C_ATTACK_CITY_GOTO_NOTICE,				// ��������֪ͨ
	S2C_ATTACK_CITY_MONSTER_LIST_NOTICE,		// �����б�֪ͨ
	S2C_ATTACK_CITY_MONSTER_DATA_NOTICE,		// ��������ͬ��֪ͨ
	S2C_TREASURE_BOX_LIST_NOTICE,				// �����б�֪ͨ
	S2C_TREASURE_BOX_DISAPPEAR_NOTICE,			// ������ʧ֪ͨ
	C2S_TREASURE_BOX_LOCK_REQ,					// ������ס���� x
	S2C_TREASURE_BOX_LOCK_RESP,					// ������ס��Ӧ
	C2S_TREASURE_BOX_PICK_UP_REQ,				// ����ʰȡ���� x
	S2C_TREASURE_BOX_PICK_UP_RESP,				// ����ʰȡ��Ӧ
	C2S_ATTACK_CITY_RELIVE_REQ,					// �������� x
	S2C_ATTACK_CITY_RELIVE_RESP,				// ������Ӧ
	C2S_ATTACK_CITY_ENCOURAGE_REQ,				// ��������
	S2C_ATTACK_CITY_ENCOURAGE_RESP,				// ������Ӧ
	C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ, 		// ������ȡ����	
	S2C_ATTACK_CITY_TAKE_BOSS_REWARD_RESP, 		// ������ȡ��Ӧ
	S2C_ATTACK_CITY_TOP_RANK_LIST_NOTICE,		// �������б�֪ͨ
	S2C_ATTACK_CITY_RANK_UPDATE_NOTICE,			// ������������֪ͨ
	S2C_ATTACK_CITY_ACTIVITY_START_NOTICE,		// ���ǻ��ʼ֪ͨ
	S2C_ATTACK_CITY_ACTIVITY_OVER_NOTICE,		// ���ǻ����֪ͨ

	// �޾�֮��
	ENDLESS_LAND_START = 24700,
	C2S_ENDLESS_LAND_DATA_REQ,					// �޾�֮���������� x
	S2C_ENDLESS_LAND_DATA_RESP,					// �޾�֮��������Ӧ
	C2S_ENDLESS_LAND_ENTER_REQ,					// �޾�֮�ؽ�������
	S2C_ENDLESS_LAND_ENTER_RESP,				// �޾�֮�ؽ�����Ӧ
	C2S_ENDLESS_LAND_KILL_MONSTER_REQ,			// ɱ������ x
	S2C_ENDLESS_LAND_KILL_MONSTER_RESP,			// ɱ����Ӧ
	C2S_ENDLESS_LAND_COLLISION_REQ,				// ��ײ���� x
	S2C_ENDLESS_LAND_COLLISION_RESP,			// ��ײ��Ӧ
	C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ,		// ��ȡ��ɱ��������
	S2C_ENDLESS_LAND_KEEP_KILL_REWARD_RESP,		// ��ȡ��ɱ������Ӧ

	C2S_ENDLESS_LAND_BATTLE_START_REQ,			// ��ʼս������
	S2C_ENDLESS_LAND_BATTLE_START_RESP,			// ��ʼս����Ӧ	
	C2S_ENDLESS_LAND_BATTLE_OVER_REQ,			// ����ս������
	S2C_ENDLESS_LAND_BATTLE_OVER_RESP,			// ����ս����Ӧ	

	// ���ս���
	NEXT_DAY_REWARD_START = 24800,
	C2S_NEXT_DAY_REWARD_DATA_REQ,				// ���ս����������� x
	S2C_NEXT_DAY_REWARD_DATA_RESP,				// ���ս���������Ӧ
	C2S_NEXT_DAY_REWARD_TAKE_REQ,				// ���ս�����ȡ����
	S2C_NEXT_DAY_REWARD_TAKE_RESP,				// ���ս�����ȡ��Ӧ

	// ��λ
	NOBILITY_START = 24900,
	C2S_NOBILITY_ACTIVATE_REQ,					// ��λ��������
	S2C_NOBILITY_ACTIVATE_RESP,					// ��λ������Ӧ

	// ��ֵ�ۿ�
	SUPER_DISCOUNT = 25000,
	C2S_SUPER_DISCOUNT_DATA_REQ,				// ��ֵ�ۿ��������� x
	S2C_SUPER_DISCOUNT_DATA_RESP,				// ��ֵ�ۿ�������Ӧ
	C2S_SUPER_DISCOUNT_BUY_REWARD_REQ,			// ��ȡ��ֵ�ۿ۹���������
	S2C_SUPER_DISCOUNT_BUY_REWARD_RESP,			// ��ȡ��ֵ�ۿ۹�������Ӧ
	C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ,	// ��ȡ��ֵ�ۿ��ۻ���������
	S2C_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP,	// ��ȡ��ֵ�ۿ��ۻ�������Ӧ

	// ���ջ
	FESTIVAL_ACTIVITY_START = 25100,
	C2S_FESTIVAL_ACTIVITY_DATA_REQ,				// ���ջ�������� x
	S2C_FESTIVAL_ACTIVITY_DATA_RESP,			// ���ջ������Ӧ
	C2S_TAKE_FESTIVAL_ACTIVITY_REWARD_REQ,		// ���ջ������ȡ����
	S2C_TAKE_FESTIVAL_ACTIVITY_REWARD_RESP,		// ���ջ������ȡ��Ӧ
	C2S_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_REQ,	// ������Ʒ�һ�����
	S2C_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_RESP,	// ������Ʒ�һ���Ӧ
	C2S_FESTIVAL_ACTIVITY_RED_POINT_REQ,		// ���ջ�������
	S2C_FESTIVAL_ACTIVITY_RED_POINT_RESP,		// ���ջ�����Ӧ

	// ��׼�Ƽ�
	ACCURATE_RECOMMEND_START = 25200,
	C2S_ACCURATE_RECOMMEND_DATA_REQ,			// ��׼�Ƽ��������� x
	S2C_ACCURATE_RECOMMEND_DATA_RESP,			// ��׼�Ƽ�������Ӧ
	C2S_TAKE_ACCURATE_RECOMMEND_GIFT_REQ,		// ��ȡ��׼�Ƽ��������
	S2C_TAKE_ACCURATE_RECOMMEND_GIFT_RESP,		// ��ȡ��׼�Ƽ������Ӧ

	// ��ʯ
	GEM_START = 25300,
	C2S_GEM_LIST_REQ,		// ��ʯ�б����� x
	S2C_GEM_LIST_RESP,		// ��ʯ�б���Ӧ
	C2S_GEM_COMPOSE_REQ,	// ��ʯ�ϳ�����
	S2C_GEM_COMPOSE_RESP,	// ��ʯ�ϳ���Ӧ
	C2S_GEM_INLAY_REQ,		// ��ʯ��Ƕ����
	S2C_GEM_INLAY_RESP,		// ��ʯ��Ƕ��Ӧ
	C2S_GEM_UNLOAD_REQ,		// ��ʯж������
	S2C_GEM_UNLOAD_RESP,	// ��ʯж����Ӧ
	C2S_GEM_HOLE_OPEN_REQ,	// ��ʯ��λ��������
	S2C_GEM_HOLE_OPEN_RESP,	// ��ʯ��λ������Ӧ
	S2C_GEM_DATA_NOTIFY,	// ��ʯ����֪ͨ
	C2S_GEM_DECOMPOSE_REQ,	// ��ʯ�ֽ�����
	S2C_GEM_DECOMPOSE_RESP,	// ��ʯ�ֽ���Ӧ

	// ��Ӹ���
	TEAM_DUNGEON_START = 25400,
	C2S_TEAM_DUNGEON_ANSWER_QUESTION_REQ,	// �������� x
	S2C_TEAM_DUNGEON_ANSWER_QUESTION_RESP,	// ������Ӧ
	S2C_TEAM_DUNGEON_ANSWER_QUESTION_NOTIFY,// ����֪ͨ
	C2S_TEAM_DUNGEON_DIALOG_OVER_REQ,		// �Ի��������� x
	S2C_TEAM_DUNGEON_DIALOG_OVER_RESP,		// �Ի�������Ӧ
	C2S_TEAM_DUNGEON_NEXT_NPC_REQ,			// ��һ��NPC���� x
	S2C_TEAM_DUNGEON_NEXT_NPC_RESP,			// ��һ��NPC��Ӧ
	S2C_TEAM_DUNGEON_NEXT_NPC_NOTIFY,		// ��һ��NPC֪ͨ
	C2S_TEAM_DUNGEON_BOSS_TASK_START_REQ,	// Boss����ʼ���� x
	S2C_TEAM_DUNGEON_BOSS_TASK_START_RESP,	// Boss��ս��Ӧ
	C2S_TEAM_DUNGEON_LOTTERY_REQ,			// �齱����
	S2C_TEAM_DUNGEON_LOTTERY_RESP,			// �齱��Ӧ
	S2C_TEAM_DUNGEON_NEW_TASK_NOTIFY,		// ������֪ͨ
	S2C_TEAM_DUNGEON_TASK_DATA_NOTIFY,		// ������������
	C2S_TEAM_DUNGEON_LEAVE_REQ,				// �뿪�������� x
	S2C_TEAM_DUNGEON_LEAVE_RESP,			// �뿪������Ӧ
	S2C_TEAM_DUNGEON_LEAVE_NOTIFY,			// �뿪����֪ͨ
	S2C_TEAM_DUNGEON_DISMISS_NOTIFY,		// ��ɢ����֪ͨ	
	C2S_TEAM_DUNGEON_CREATE_TEAM_REQ,		// ������������ x
	S2C_TEAM_DUNGEON_CREATE_TEAM_RESP,		// ����������Ӧ
	C2S_TEAM_DUNGEON_INVITE_REQ,			// ����������� x
	S2C_TEAM_DUNGEON_INVITE_RESP,			// ���������Ӧ
	S2C_TEAM_DUNGEON_INVITE_NOTIFY,			// �������֪ͨ
	C2S_TEAM_DUNGEON_ACCEPT_INVITE_REQ,		// ��������������� x
	S2C_TEAM_DUNGEON_ACCEPT_INVITE_RESP,	// �������������Ӧ
	S2C_TEAM_DUNGEON_ACCEPT_INVITE_NOTIFY,	// �����������֪ͨ
	C2S_TEAM_DUNGEON_RECOMMEND_LIST_REQ,	// ��������Ƽ��б����� x
	S2C_TEAM_DUNGEON_RECOMMEND_LIST_RESP,	// ��������Ƽ��б���Ӧ
	C2S_TEAM_DUNGEON_MATCH_REQ,				// ���ƥ������ x
	S2C_TEAM_DUNGEON_MATCH_RESP,			// ���ƥ����Ӧ
	C2S_TEAM_DUNGEON_CANCEL_MATCH_REQ,		// ���ȡ��ƥ������ x
	S2C_TEAM_DUNGEON_CANCEL_MATCH_RESP,		// ���ȡ��ƥ����Ӧ
	S2C_TEAM_DUNGEON_READY_TEAM_DATA_NOTIFY,// ׼����������֪ͨ
	S2C_TEAM_DUNGEON_DIALOG_OVER_NOITY,		// �Ի�����֪ͨ
	C2S_TEAM_DUNGEON_ACCEPT_TASK_REQ,		// ����������
	S2C_TEAM_DUNGEON_ACCEPT_TASK_RESP,		// ��������Ӧ
	S2C_TEAM_DUNGEON_ACCEPT_TASK_NOTIFY,	// ������֪ͨ
	S2C_TEAM_DUNDEON_TASK_LINK_FINISH_NOTIFY,// ���������֪ͨ
	S2S_TEAM_DUNDEON_TASK_REWARD_NOTIFY,	// ������֪ͨ
	S2C_TEAM_DUNGEON_MATCH_NOTIFY,			// ���ƥ��֪ͨ
	S2C_TEAM_DUNGEON_CANNEL_MATCH_NOTIFY,	// ȡ�����ƥ��֪ͨ
	C2S_TEAM_DUNGEON_TASK_START_REQ,		// ����ʼ����
	S2C_TEAM_DUNGEON_TASK_START_RESP,		// ����ʼ��Ӧ
	S2C_TEAM_DUNGEON_TASK_START_NOTIFY,		// ����ʼ֪ͨ(��ʱ��ʹ��)
	C2S_TEAM_DUNGEON_RANK_REQ,				// �������а�����
	S2C_TEAM_DUNGEON_RANK_RESP,				// �������а���Ӧ
	S2C_TEAM_DUNGEON_BOSS_REWARD_NOTIFY,	// ��Ӹ���BOSS����֪ͨ
	C2S_TEAM_DUNGEON_DATA_REQ,				// ��Ӹ�����������
	S2C_TEAM_DUNGEON_DATA_RESP,				// ��Ӹ���������Ӧ
	S2C_TEAM_DUNGEON_BOSS_TASK_START_NOTIFY,// boss����ʼ֪ͨ
	C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ,		// ��ӿ����������
	S2C_TEAM_DUNGEON_SHORTCUT_CHAT_RESP,	// ��ӿ��������Ӧ
	C2S_TEAM_DUNGEON_LOTTERY_RECORD_REQ,	// ��Ӹ����齱��¼����
	S2C_TEAM_DUNGEON_LOTTERY_RECORD_RESP,	// ��Ӹ����齱��¼��Ӧ
	S2C_TEAM_DUNGEON_MATCH_TARGET_NUM_NOTIFY,// ��Ӹ���ƥ��Ŀ������֪ͨ	
	C2S_TEAM_DUNGEON_JOIN_IN_REQ,			// ����������� x
	S2C_TEAM_DUNGEON_JOIN_IN_RESP,			// ���������Ӧ
	S2C_TEAM_DUNGEON_JOIN_IN_NOTIFY,		// �������֪ͨ

	// ��¼����
	LOGIN_REWARD_START = 25500,
	C2S_LOGIN_REWARD_DATA_REQ,					// ��¼������������ x
	S2C_LOGIN_REWARD_DATA_RESP,					// ��¼����������Ӧ
	C2S_TAKE_LOGIN_REWARD_REQ,					// ��ȡ��¼��������
	S2C_TAKE_LOGIN_REWARD_RESP,					// ��ȡ��¼������Ӧ

	// ������
	KING_MATCH_START = 25600,
	C2S_KING_MATCH_DATA_REQ,					// �����������������
	S2C_KING_MATCH_DATA_RESP,					// ���������������Ӧ
	C2S_KING_MATCH_SEARCH_TARGET_REQ,			// ����Ŀ������
	S2C_KING_MATCH_SEARCH_TARGET_RESP,			// ����Ŀ����Ӧ
	C2S_TAKE_KING_MATCH_TASK_REWARD_REQ,		// ��ȡ��������������
	S2C_TAKE_KING_MATCH_TASK_REWARD_RESP,		// ��ȡ������������Ӧ
	C2S_KING_MATCH_SEASON_RANK_REQ,				// �������а����� 
	S2C_KING_MATCH_SEASON_RANK_RESP,			// �������а���Ӧ

	// ʥ����
	CHRISTMAS_START = 25700,
	C2S_CHRISTMAS_DATA_REQ,						// ����ʥ��������
	S2C_CHRISTMAS_DATA_RESP,					// ����ʥ����������Ӧ
	C2S_CHRISTMAS_BATTLE_ENTER_REQ,				// ����ս������
	S2C_CHRISTMAS_BATTLE_ENTER_RESP,			// ����ս����Ӧ
	C2S_CHRISTMAS_BATTLE_LEAVE_REQ,				// �뿪ս������
	S2C_CHRISTMAS_BATTLE_LEAVE_RESP,			// �뿪ս����Ӧ
	C2S_CHRISTMAS_ATTACK_BOSS_REQ,				// ����Boss����
	S2C_CHRISTMAS_ATTACK_BOSS_RESP,				// ����Boss��Ӧ
	S2C_CHRISTMAS_ATTACK_BOSS_NOTIFY,			// ����Boss֪ͨ
	C2S_CHRISTMAS_TAKE_BOSS_REWARD_REQ,			// ��ȡ��������
	S2C_CHRISTMAS_TAKE_BOSS_REWARD_RESP,		// ��ȡ������Ӧ
	S2C_CHRISTMAS_BOSS_DEAD_NOTIFY,				// Boss����֪ͨ

	// facebook�
	FACEBOOK_ACTIVITY_START = 25800,
	C2S_FACEBOOK_ACTIVITY_DATA_REQ,					// ���������
	S2C_FACEBOOK_ACTIVITY_DATA_RESP,				// �������Ӧ
	C2S_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_REQ,	// ��ȡ�ʺŰ󶨽�������
	S2C_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_RESP,	// ��ȡ�ʺŰ󶨽�����Ӧ
	C2S_TAKE_FACEBOOK_SHARE_REWARD_REQ,				// ��ȡ����������
	S2C_TAKE_FACEBOOK_SHARE_REWARD_RESP,			// ��ȡ��������Ӧ
	C2S_FACEBOOK_FRIEND_INVITE_REQ,					// ������������
	S2C_FACEBOOK_FRIEND_INVITE_RESP,				// ����������Ӧ
	C2S_FACEBOOK_FRIEND_SUMMON_REQ,					// �����ٻ�����
	S2C_FACEBOOK_FRIEND_SUMMON_RESP,				// �����ٻ���Ӧ
	C2S_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_REQ,		// ��ȡ�������뽱������
	S2C_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_RESP,	// ��ȡ�������뽱����Ӧ
	C2S_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_REQ,		// ��ȡ�����ٻ���������
	S2C_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_RESP,	// ��ȡ�����ٻ�������Ӧ
	C2S_FACEBOOK_FRIEND_INVITE_RECORD_REQ,			// �Ѿ�����ĺ��Ѽ�¼����
	S2C_FACEBOOK_FRIEND_INVITE_RECORD_RESP,			// �Ѿ�����ĺ��Ѽ�¼��Ӧ
	C2S_TAKE_FACEBOOK_COMMEND_REWARD_REQ,			// ��ȡ���۽�������
	S2C_TAKE_FACEBOOK_COMMEND_REWARD_RESP,			// ��ȡ���۽�����Ӧ

	// ���߽���
	ONLINE_REWARD_START = 25900,
	C2S_ONLINE_REWARD_DATA_REQ,						// ���߽�����������
	S2C_ONLINE_REWARD_DATA_RESP,					// ���߽���������Ӧ
	C2S_TAKE_ONLINE_REWARD_REQ,						// ��ȡ���߽�������
	S2C_TAKE_ONLINE_REWARD_RESP,					// ��ȡ���߽�����Ӧ

	// 7���¼
	SEVEN_DAY_LOGIN_START = 26000,
	C2S_SEVEN_DAY_LOGIN_DATA_REQ,					// 7���¼��������
	S2C_SEVEN_DAY_LOGIN_DATA_RESP,					// 7���¼������Ӧ
	C2S_TAKE_SEVEN_DAY_LOGIN_REWARD_REQ,			// 7���¼������ȡ����
	S2C_TAKE_SEVEN_DAY_LOGIN_REWARD_RESP,			// 7���¼������ȡ��Ӧ

	// ��ֵ����
	RECHARGE_REWARD_START = 26100,
	C2S_RECHARGE_REWARD_DATA_REQ,			// �����ֵ��������
	S2C_RECHARGE_REWARD_DATA_RESP,			// �����ֵ����������Ӧ
	C2S_TAKE_RECHARGE_REWARD_REQ, 			// ��ȡ��ֵ��������
	S2C_TAKE_RECHARGE_REWARD_RESP,			// ��ȡ��ֵ������Ӧ
	C2S_TAKE_RECHARGE_EXTRA_REWARD_REQ,		// ��ȡ���⽱��
	S2C_TAKE_RECHARGE_EXTRA_REWARD_RESP,	// ��ȡ���⽱����Ӧ

	// �۳佱��
	TOTAL_RECHARGE_REWARD_START = 26200,
	C2S_TOTAL_RECHARGE_REWARD_DATA_REQ,			// �����۳佱������
	S2C_TOTAL_RECHARGE_REWARD_DATA_RESP,		// �����۳佱��������Ӧ
	C2S_TAKE_TOTAL_RECHARGE_REWARD_REQ, 		// ��ȡ�۳佱������
	S2C_TAKE_TOTAL_RECHARGE_REWARD_RESP,		// ��ȡ�۳佱����Ӧ

	// �³�ֵ�ۿ�
	NEW_SUPER_DISCOUNT_START = 26300,
	C2S_NEW_SUPER_DISCOUNT_DATA_REQ,				// ��ֵ�ۿ��������� x
	S2C_NEW_SUPER_DISCOUNT_DATA_RESP,				// ��ֵ�ۿ�������Ӧ
	C2S_NEW_SUPER_DISCOUNT_BUY_REWARD_REQ,			// ��ȡ��ֵ�ۿ۹���������
	S2C_NEW_SUPER_DISCOUNT_BUY_REWARD_RESP,			// ��ȡ��ֵ�ۿ۹�������Ӧ
	C2S_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ,	// ��ȡ��ֵ�ۿ��ۻ���������
	S2C_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP,	// ��ȡ��ֵ�ۿ��ۻ�������Ӧ

	// Ť���齱
	ND_LOTTERY_START = 26400,
	C2S_ND_LOTTERY_REQ,							// Ť���齱����
	S2C_ND_LOTTERY_RESP,						// Ť���齱��Ӧ
	C2S_ND_LOTTERY_RECORD_REQ,					// Ť���齱��¼����
	S2C_ND_LOTTERY_RECORD_RESP,					// Ť���齱��¼��Ӧ

	// ������Ʒ
	HOT_SALE_GOODS_START = 26500,
	C2S_HOT_SALE_GOODS_DATA_REQ,					// ������Ʒ��������
	S2C_HOT_SALE_GOODS_DATA_RESP,					// ������Ʒ������Ӧ
	C2S_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_REQ,	// ��ȡ������Ʒ��ֵ��������
	S2C_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_RESP,	// ��ȡ������Ʒ��ֵ������Ӧ

	// ת�̳齱
	TURNTABLE_LOTTERY_START = 26600,
	C2S_TURNTABLE_DATA_REQ,					// ת����������
	S2C_TURNTABLE_DATA_RESP,				// ת��������Ӧ
	C2S_TURNTABLE_LOTTERY_REQ,				// ת�̳齱����
	S2C_TURNTABLE_LOTTERY_RESP,				// ת�̳齱��Ӧ

	// ��Ӫ��ļ
	CAMP_RECRUIT_START = 26700,
	C2S_CAMP_RECRUIT_DATA_REQ,					// ��Ӫ��ļ��������
	S2C_CAMP_RECRUIT_DATA_RESP,					// ��Ӫ��ļ������Ӧ
	C2S_CAMP_RECRUIT_REQ,						// ��Ӫ��ļ����
	S2C_CAMP_RECRUIT_RESP,						// ��Ӫ��ļ��Ӧ
	C2S_CAMP_RECRUIT_SCORE_TASK_REWARD_REQ,		// ��Ӫ��ļ��������������
	S2C_CAMP_RECRUIT_SCORE_TASK_REWARD_RESP,	// ��Ӫ��ļ������������Ӧ
	C2S_CAMP_RECRUIT_RECORD_REQ,				// ��Ӫ��ļ��¼����
	S2C_CAMP_RECRUIT_RECORD_RESP,				// ��Ӫ��ļ��¼��Ӧ

	// unlock
	UNLOCK_ACTIVITY_START = 26800,
	C2S_UNLOCK_ACTIVITY_DATA_REQ,				// unlock���������
	S2C_UNLOCK_ACTIVITY_DATA_RESP,				// unlock�������Ӧ
	C2S_SHOP_SCORE_FINISH_REQ,					// �̳������������
	S2C_SHOP_SCORE_FINISH_RESP,					// �̳����������Ӧ
	C2S_WATCHING_VIDEO_NOTIFY_REQ,				// �ۿ���Ƶ֪ͨ����
	S2C_WATCHING_VIDEO_NOTIFY_RESP,				// �ۿ���Ƶ֪ͨ��Ӧ

	// ����ȼ�����
	WORLD_LEVEL_TASK_START = 26900,
	C2S_WORLD_LEVEL_TASK_DATA_REQ,				// ����ȼ�������������
	S2C_WORLD_LEVEL_TASK_DATA_RESP,				// ����ȼ�����������Ӧ
	C2S_TAKE_WORLD_LEVEL_TASK_REWARD_REQ,		// ��ȡ����ȼ�����������
	S2C_TAKE_WORLD_LEVEL_TASK_REWARD_RESP,		// ��ȡ����ȼ���������Ӧ
	C2S_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_REQ,	// ��ȡ����ȼ��������ս�������
	S2C_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_RESP,// ��ȡ����ȼ��������ս�����Ӧ

	// ��Դս
	RESOURCE_WAR_START = 27000,
	C2S_PLAYER_RESOURCE_WAR_DATA_REQ,			// ���������Դս����
	S2C_PLAYER_RESOURCE_WAR_DATA_RESP,			// �����Դս������Ӧ
	C2S_RESOURCE_WAR_ENEMY_LIST_REQ,			// ��������б�
	S2C_RESOURCE_WAR_ENEMY_LIST_RESP,			// ��������б���Ӧ
	C2S_RESOURCE_WAR_BATTLE_RECORD_REQ,			// ����ս��
	S2C_RESOURCE_WAR_BATTLE_RECORD_RESP,		// ����ս����Ӧ
	C2S_RESOURCE_WAR_GIVE_UP_OCCUPY_REQ,		// ����ռ�����Դ
	S2C_RESOURCE_WAR_GIVE_UP_OCCUPY_RESP,		// ����ռ�����Դ��Ӧ
	C2S_RESOURCE_WAR_KEEP_OCCUPY_REQ,			// �ӳ�ռ�����Դ
	S2C_RESOURCE_WAR_KEEP_OCCUPY_RESP,			// �ӳ�ռ�����Դ��Ӧ
	C2S_RESOURCE_WAR_CHAPTER_DATA_REQ,			// ������Դս�½�����
	S2C_RESOURCE_WAR_CHAPTER_DATA_RESP,			// ������Դս�½�������Ӧ
	C2S_RESOURCE_WAR_SAME_RESOURCE_REQ,			// ������Դսͬ����Դ similar
	S2C_RESOURCE_WAR_SAME_RESOURCE_RESP,		// ������Դսͬ����Դ��Ӧ
	C2S_RESOURCE_WAR_GUILD_RESOURCE_REQ,		// ������Դս������Դ
	S2C_RESOURCE_WAR_GUILD_RESOURCE_RESP,		// ������Դս������Դ��Ӧ
	C2S_RESOURCE_WAR_TECHNOLOGY_UPGRADE_REQ,	// ��Դս�Ƽ���������
	S2C_RESOURCE_WAR_TECHNOLOGY_UPGRADE_RESP,	// ��Դս�Ƽ�������Ӧ

	// ����ս
	GUILD_WAR_START = 27100,
	C2S_GUILD_WAR_ROLE_DATA_REQ,					// ���󹤻�ս��ɫ����
	S2C_GUILD_WAR_ROLE_DATA_RESP,					// ���󹤻�ս��ɫ������Ӧ
	C2S_GUILD_WAR_OCCUPY_STAR_LIST_REQ,				// ���󱾾�����ռ�������
	S2C_GUILD_WAR_OCCUPY_STAR_LIST_RESP,			// ���󱾾�����ռ���������Ӧ
	C2S_GUILD_WAR_DECLARE_WAR_STAR_LIST_REQ,		// ���󱾾�������ս�������б�
	S2C_GUILD_WAR_DECLARE_WAR_STAR_LIST_RESP,		// ���󱾾�������ս�������б���Ӧ
	C2S_GUILD_WAR_STAR_LIST_REQ,					// ���������б�
	S2C_GUILD_WAR_STAR_LIST_RESP,					// ���������б���Ӧ
	C2S_GUILD_WAR_DECLARE_WAR_REQ,					// ������ս
	S2C_GUILD_WAR_DECLARE_WAR_RESP,					// ��ս��Ӧ
	C2S_GUILD_WAR_ORGANIZE_DEFENSE_REQ,				// ���󲼷�
	S2C_GUILD_WAR_ORGANIZE_DEFENSE_RESP,			// ������Ӧ
	C2S_GUILD_WAR_EVACUATE_REQ,						// ������
	S2C_GUILD_WAR_EVACUATE_RESP,					// ������Ӧ
	C2S_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_REQ,	// ������Ը߼�������ս����
	S2C_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_RESP,	// ����Ը߼�������ս������Ӧ
	C2S_GUILD_WAR_TAKE_STAR_REWARD_REQ,				// ������ȡ������
	S2C_GUILD_WAR_TAKE_STAR_REWARD_RESP,			// ��ȡ��������Ӧ
	C2S_GUILD_WAR_STAR_DETAIL_REQ,					// ����������ϸ
	S2C_GUILD_WAR_STAR_DETAIL_RESP,					// ������ϸ��Ӧ
	C2S_GUILD_WAR_TOWER_LIST_REQ,					// ���󹤻�ս���б�
	S2C_GUILD_WAR_TOWER_LIST_RESP,					// ����ս���б���Ӧ
	S2C_GUILD_WAR_STAR_UPDATE_NOTIFY,				// �������ݸ���֪ͨ

	// �һ�
	C2S_HANG_UP_DROP_DATA_REQ,							// �һ�������������
	S2C_HANG_UP_DROP_DATA_RESP,							// �һ�����������Ӧ
	C2S_HANG_UP_DROP_REWARD_OBTAIN_REQ,					// ��ȡ�һ����佱������
	S2C_HANG_UP_DROP_REWARD_OBTAIN_RESP,				// ��ȡ�һ����佱����Ӧ




	/*--------------------------------------------------------------------------------------------------------------------------*/
	// 3) GateSvr��MapSvr֮��ͨ��ID����
	/*--------------------------------------------------------------------------------------------------------------------------*/
	//��ɫϵͳ
	G2M_CLIENT_DISCONNECT_CMD = M2G_MIN,	// ��ɫ�Ͽ�����֪ͨ
	M2G_CLIENT_KICKOUT_CMD,					// �߳���ɫ֪ͨ
	M2G_HEART_BEAT,							// ����
	G2M_HEART_BEAT,							// ����
	M2G_CLIENT_INFO,						// �ͻ�����Ϣ
	

	/*--------------------------------------------------------------------------------------------------------------------------*/
	// 4) MapSvr��OpenSvr֮��ͨ��ID����
	/*--------------------------------------------------------------------------------------------------------------------------*/
	M2O_AUTH_SYN = M2O_MIN,
	O2M_AUTH_ACK,
	M2O_SERVERID_CMD,
	M2O_HEART_BEAT,			//����
	O2M_HEART_BEAT,			//����

	M2O_LOGIC_COMMON_REQ,		// �߼�ͨ����Ϣ����
	O2M_LOGIC_COMMON_RESP,		// �߼�ͨ����Ϣ��Ӧ

	M2O_AUTHENTICATION_CODE_REQ,	// ��֤������
	O2M_AUTHENTICATION_CODE_RESP,	// ��֤����Ӧ

	M2O_REYUN_STATISTIC_NOTIFY,		// ��Ϊͳ��֪ͨ

	M2O_CHECK_USER_INFO_REQ,		// ����û���Ϣ����
	O2M_CHECK_USER_INFO_RESP,		// ����û���Ϣ��Ӧ

	O2M_RECHARGE_SEND_GOODS_REQ,	// ��ֵ��������
	M2O_RECHARGE_SEND_GOODS_RESP,	// ��ֵ������Ӧ

	O2M_RECHARGE_CALLBACK_NOTICE_REQ,	// ��ֵ�ص�֪ͨ����	
	M2O_RECHARGE_CALLBACK_NOTICE_RESP,	// ��ֵ�ص�֪ͨ��Ӧ

	M2O_SDK_ORDER_ID_REQ,				// SDK����������
	O2M_SDK_ORDER_ID_RESP,				// SDK��������Ӧ

	O2M_CP_ORDER_ID_REQ,				// CP����������
	M2O_CP_ORDER_ID_RESP,				// CP��������Ӧ

	O2M_ONLINE_PLAYER_NUM_REQ,			// ���������������
	M2O_ONLINE_PLAYER_NUM_RESP,			// �������������Ӧ

	M2O_RECHARGE_SUCCESS_REPORT_NOTIFY,	// ��ֵ�ɹ��ϱ�(����)

	M2O_MYCARD_RECHARGE_SUCCESS_REPORT_NOTIFY,	// MyCard��ֵ�ɹ��ϱ�(����)

	M2O_STATISTICS_NOTIFY,				// ����ͳ��֪ͨ

	O2M_YGAME_RECHARGE_SEND_GOODS_REQ,	// YGame��ֵ��������
	M2O_YGAME_RECHARGE_SEND_GOODS_RESP,	// YGame��ֵ������Ӧ

	O2M_YGAME_CASHBACK_NOTIFY_REQ,		// YGame����֪ͨ����
	M2O_YGAME_CASHBACK_NOTIFY_RESP,		// YGame����֪ͨ��Ӧ

	O2M_YGAME_BONUS_NOTIFY_REQ,			// YGame����֪ͨ����
	M2O_YGAME_BONUS_NOTIFY_RESP,		// YGame����֪ͨ��Ӧ
};																								
