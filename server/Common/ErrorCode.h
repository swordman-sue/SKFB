#pragma once

// 错误码的定义
enum ERROR_CODE
{
	ERROR_CODE_OK									= 1,	//正常
	ERROR_UNKNOW									= 2,	//未知错误

	//通用错误码定义
	ERROR_CODE_DB_EXCUTE_FAIL						= 0,		// 数据库执行失败

	//登录系统错误码定义
	ERROR_LOGIN_PASSWORD_INVALID					= 1000,		// 密码无效
	ERROR_LOGIN_REPEAT_LOGIN,									// 帐号重复登录,请稍后再登录
	ERROR_LOGIN_KICK_OUT,										// 您的帐号已在别处登录
	ERROR_LOGIN_ENAME_EXIST,									// 该角色名字已被使用
	ERROR_LOGIN_ROLENAME_INVALID,								// 角色名不合法
	ERROR_LOGIN_PLAYER_FULL,									// 服务器人满为患,请过段时间再登录
	ERROR_LOGIN_SERVER_INVALID,									// 服务器木有开放
	ERROR_AUTH_CODE_INVALID,									// 手机注册认证码已过期或者无效
	ERROR_ACCOUNT_NOT_REGISTER,									// 帐号还没注册不能登录
	ERROR_BIND_AUTH_CODE_INVALID,								// 手机绑定认证码已过期或者无效
	ERROR_LOGIN_ACCOUNT_EXIST,									// 该帐号已被使用
	ERROR_ACCOUNT_LOCK,											// 帐号被锁
	ERROR_LOGIN_KICK_OUT_BY_GM,									// 您的帐号已在别处登录
	ERROR_VERSION_INVALID,										// 版本不一致
	ERROR_ROLE_ALREADY_EXIST,									// 角色已存在
	

	// 聊天
	ERROR_CHAT_SENSITIVE_WORD						= 10000,	// 聊天内容有敏感字
	ERROR_CHAT_TARGET_INVALID,									// 目标不存在
	ERROR_CHAT_VOICE_INVALID,									// 语音不存在或者已过期
	ERROR_CHAT_GUILD_INVALID,									// 玩家木有在工会
	ERROR_CHAT_CAN_NOT_SAY_TO_SELF,								// 不能对自己说
	ERROR_CHAT_IN_TARGET_BLACK_LIST,							// 目标已把你拉黑
	ERROR_CHAT_FORBID_TO_TALK,									// 禁言中

	// 邮件
	ERROR_MAIL_UNKNOW								= 10100,	// 邮件未知错误
	ERROR_MAIL_INVALID_ID,										// 邮件不存在或已删除
	ERROR_MAIL_NOT_HAVE_ATTACHMENT,								// 没有附件
	ERROR_MAIL_ATTACHMENT_EMPTY,								// 附件已被领取

	ERROR_EQUIPMENT_UNKNOW							= 10200,	// 装备未知错误
	ERROR_EQUIPMENT_STRENGTH_FAIL					= 10201,	// 装备强化失败

	// 好友
	ERROR_FRIEND_UNKNOW								= 10300,	// 好友未知错误
	ERROR_FRIEND_LIST_SIZE_LIMIT,								// 目标列表已满
	//ERROR_FRIEND_TARGET_INVALID,								// 目标不存在

	// 综合
	ERROR_LEAVE_GUILD_CD							= 20000,	// 离开公会CD中
	ERROR_ELITEFB_ENEMY_INVALID,								// 精灵副本外敌不存在
	ERROR_ACTIVATION_CODE_INVALID,								// 激活码无效
	ERROR_ACTIVATION_CODE_ALREADY_TAKE,							// 激活码已被领取
	ERROR_GUILD_NAME_EXIST,										// 工会名称已被使用
	ERROR_PVP_ACTIVITY_CLOSE,									// PVP活动还没开放
	ERROR_ATTCK_CITY_TARGET_INVALID,							// 目标无效(目标正在战斗中...)
	ERROR_DOUBAO_COMPOSE_ITEM_NOT_ENOUGH,						// 宝物合成物品不足
	ERROR_DECOMPOSE_HERO_NOT_EXIST,								// 分解英雄不存在
	ERROR_IN_BATTLE,											// 已经在战斗中(todo::客户端不需要做提示,有可能连续点击造成的)
	ERROR_ATTCK_CITY_TARGET_DIE,								// 目标已死亡
	ERROR_GUILD_APPLY_NUM_LIMIT,								// 工会申请数量限制
	ERROR_GUILD_TARGET_HAVE_GUILD,								// 目标已加入其他工会
	ERROR_GUILD_ALREADY_IN_GUILD,								// 你已经在工会
	ERROR_ACCOUNT_INVALID = 20014,								// 账号异常

	ERROR_TEAM_INVALID = 20015,									// 准备队伍无效(邀请的队伍已经不存在)
	ERROR_TEAM_MEMBER_IS_FULL = 20016,							// 目标队伍成员已满
	ERROR_CHRISTMAS_BOSS_DEAD = 20017,							// 圣诞节boss已死亡
	ERROR_ACTIVATION_CODE_TIME_LIMIT = 20018,					// 激活码已过期

	//ERROR_RESOURCE_WAR_BE_OCCUPY = 20019,						// 资源已被占领
	ERROR_RESOURCE_WAR_BE_BATTLE = 20019,						// 资源被挑战中
};

