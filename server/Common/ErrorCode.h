#pragma once

// ������Ķ���
enum ERROR_CODE
{
	ERROR_CODE_OK									= 1,	//����
	ERROR_UNKNOW									= 2,	//δ֪����

	//ͨ�ô����붨��
	ERROR_CODE_DB_EXCUTE_FAIL						= 0,		// ���ݿ�ִ��ʧ��

	//��¼ϵͳ�����붨��
	ERROR_LOGIN_PASSWORD_INVALID					= 1000,		// ������Ч
	ERROR_LOGIN_REPEAT_LOGIN,									// �ʺ��ظ���¼,���Ժ��ٵ�¼
	ERROR_LOGIN_KICK_OUT,										// �����ʺ����ڱ𴦵�¼
	ERROR_LOGIN_ENAME_EXIST,									// �ý�ɫ�����ѱ�ʹ��
	ERROR_LOGIN_ROLENAME_INVALID,								// ��ɫ�����Ϸ�
	ERROR_LOGIN_PLAYER_FULL,									// ����������Ϊ��,�����ʱ���ٵ�¼
	ERROR_LOGIN_SERVER_INVALID,									// ������ľ�п���
	ERROR_AUTH_CODE_INVALID,									// �ֻ�ע����֤���ѹ��ڻ�����Ч
	ERROR_ACCOUNT_NOT_REGISTER,									// �ʺŻ�ûע�᲻�ܵ�¼
	ERROR_BIND_AUTH_CODE_INVALID,								// �ֻ�����֤���ѹ��ڻ�����Ч
	ERROR_LOGIN_ACCOUNT_EXIST,									// ���ʺ��ѱ�ʹ��
	ERROR_ACCOUNT_LOCK,											// �ʺű���
	ERROR_LOGIN_KICK_OUT_BY_GM,									// �����ʺ����ڱ𴦵�¼
	ERROR_VERSION_INVALID,										// �汾��һ��
	ERROR_ROLE_ALREADY_EXIST,									// ��ɫ�Ѵ���
	

	// ����
	ERROR_CHAT_SENSITIVE_WORD						= 10000,	// ����������������
	ERROR_CHAT_TARGET_INVALID,									// Ŀ�겻����
	ERROR_CHAT_VOICE_INVALID,									// ���������ڻ����ѹ���
	ERROR_CHAT_GUILD_INVALID,									// ���ľ���ڹ���
	ERROR_CHAT_CAN_NOT_SAY_TO_SELF,								// ���ܶ��Լ�˵
	ERROR_CHAT_IN_TARGET_BLACK_LIST,							// Ŀ���Ѱ�������
	ERROR_CHAT_FORBID_TO_TALK,									// ������

	// �ʼ�
	ERROR_MAIL_UNKNOW								= 10100,	// �ʼ�δ֪����
	ERROR_MAIL_INVALID_ID,										// �ʼ������ڻ���ɾ��
	ERROR_MAIL_NOT_HAVE_ATTACHMENT,								// û�и���
	ERROR_MAIL_ATTACHMENT_EMPTY,								// �����ѱ���ȡ

	ERROR_EQUIPMENT_UNKNOW							= 10200,	// װ��δ֪����
	ERROR_EQUIPMENT_STRENGTH_FAIL					= 10201,	// װ��ǿ��ʧ��

	// ����
	ERROR_FRIEND_UNKNOW								= 10300,	// ����δ֪����
	ERROR_FRIEND_LIST_SIZE_LIMIT,								// Ŀ���б�����
	//ERROR_FRIEND_TARGET_INVALID,								// Ŀ�겻����

	// �ۺ�
	ERROR_LEAVE_GUILD_CD							= 20000,	// �뿪����CD��
	ERROR_ELITEFB_ENEMY_INVALID,								// ���鸱����в�����
	ERROR_ACTIVATION_CODE_INVALID,								// ��������Ч
	ERROR_ACTIVATION_CODE_ALREADY_TAKE,							// �������ѱ���ȡ
	ERROR_GUILD_NAME_EXIST,										// ���������ѱ�ʹ��
	ERROR_PVP_ACTIVITY_CLOSE,									// PVP���û����
	ERROR_ATTCK_CITY_TARGET_INVALID,							// Ŀ����Ч(Ŀ������ս����...)
	ERROR_DOUBAO_COMPOSE_ITEM_NOT_ENOUGH,						// ����ϳ���Ʒ����
	ERROR_DECOMPOSE_HERO_NOT_EXIST,								// �ֽ�Ӣ�۲�����
	ERROR_IN_BATTLE,											// �Ѿ���ս����(todo::�ͻ��˲���Ҫ����ʾ,�п������������ɵ�)
	ERROR_ATTCK_CITY_TARGET_DIE,								// Ŀ��������
	ERROR_GUILD_APPLY_NUM_LIMIT,								// ����������������
	ERROR_GUILD_TARGET_HAVE_GUILD,								// Ŀ���Ѽ�����������
	ERROR_GUILD_ALREADY_IN_GUILD,								// ���Ѿ��ڹ���
	ERROR_ACCOUNT_INVALID = 20014,								// �˺��쳣

	ERROR_TEAM_INVALID = 20015,									// ׼��������Ч(����Ķ����Ѿ�������)
	ERROR_TEAM_MEMBER_IS_FULL = 20016,							// Ŀ������Ա����
	ERROR_CHRISTMAS_BOSS_DEAD = 20017,							// ʥ����boss������
	ERROR_ACTIVATION_CODE_TIME_LIMIT = 20018,					// �������ѹ���

	//ERROR_RESOURCE_WAR_BE_OCCUPY = 20019,						// ��Դ�ѱ�ռ��
	ERROR_RESOURCE_WAR_BE_BATTLE = 20019,						// ��Դ����ս��
};

