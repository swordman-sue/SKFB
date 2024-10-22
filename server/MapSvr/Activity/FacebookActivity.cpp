#include "stdafx.h"
#include "FacebookActivity.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\MapServer.h"
#include "..\Player.h"
#include "ActivityManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\Friend\FriendManager.h"
#include "..\PlayerBaseDataManager.h"
#include "..\AccountInfo.h"


CFacebookActivity::CFacebookActivity()
{
	
}

CFacebookActivity::~CFacebookActivity()
{
	
}

void CFacebookActivity::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pPlayer = pPlayer;
	m_pActivityData = &pPlayer->GetRoleFullInfo().tActivityInfo.tFacebookActivityData;
}

// ��¼
void CFacebookActivity::OnLogin()
{
	CheckInviteFriendIsLogin();

	CheckRecallFriendIsLogin();
}

// �ǳ�
void CFacebookActivity::OnLogout()
{
	
}

// ��������
void CFacebookActivity::OnNetActivityData()
{
	SendActivityData();
}

// ��ȡ�ʺŰ󶨽���
void CFacebookActivity::OnNetTakeAccountBindingReward()
{
	// ����Ƿ�����ȡ����
	if (m_pActivityData->wIsAlreadyTakeAccountBindingReward)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��������ȡ��", __FUNCTION__);
		return;
	}

	// ��ȡ�ʺŰ�����
	const FacebookAccountBinding_Config *pAccountBindingCfg = g_pCfgMgr->GetFacebookAccountBinding(1);
	if (!pAccountBindingCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ȡ�����ʺŰ�����", __FUNCTION__);
		return;
	}

	// ��ӽ���
	m_pPlayer->AddObjectList(pAccountBindingCfg->vRewardList, TRUE, FROM_GET_BIND_FB_ACCOUNT_REWARD);

	// ��¼����ȡ
	m_pActivityData->wIsAlreadyTakeAccountBindingReward = true;

	// ��Ӧ
	MSG_S2C_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ������
void CFacebookActivity::OnNetTakeShareReward()
{
	// ����Ƿ���ͨ��ָ�����½�
	if (!m_pActivityData->dwReadyToShareTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ûͨ�����߸���ָ���½ڲ�����ȡ", __FUNCTION__);
		return;
	}

	// ����Ƿ�����ȡ��
	if (1 == m_pActivityData->dwReadyToShareTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����������ȡ��", __FUNCTION__);
		return;
	}

	// todo::����Ƿ���˿���ȡ��ʱ��

	// ȡ��������
	const FacebookShare_Config *pShareCfg = g_pCfgMgr->GetFacebookShare(2);
	if (!pShareCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ȡ�����ʺŷ�������", __FUNCTION__);
		return;
	}

	// ��ӽ���
	m_pPlayer->AddObjectList(pShareCfg->vRewardList, TRUE, FROM_GET_SHARE_REWARD);

	// ��¼����ȡ
	m_pActivityData->dwReadyToShareTime = 1;

	// ��Ӧ
	MSG_S2C_TAKE_FACEBOOK_SHARE_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ���۽���
void CFacebookActivity::OnNetTakeCommendReward()
{
	// ����Ƿ���ͨ��ָ�����½�
	if (!m_pActivityData->dwReadyToCommendTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ûͨ�����߸���ָ���½ڲ�����ȡ", __FUNCTION__);
		return;
	}

	// ����Ƿ�����ȡ��
	if (1 == m_pActivityData->dwReadyToCommendTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���۽�������ȡ��", __FUNCTION__);
		return;
	}

	// todo::����Ƿ���˿���ȡ��ʱ��

	// ȡ��������
	const FacebookCommend_Config *pShareCfg = g_pCfgMgr->GetFacebookCommend(3);
	if (!pShareCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ȡ�����ʺ���������", __FUNCTION__);
		return;
	}

	// ��ӽ���
	m_pPlayer->AddObjectList(pShareCfg->vRewardList, TRUE, FROM_GET_COMMAND_REWARD);

	// ��¼����ȡ
	m_pActivityData->dwReadyToCommendTime = 1;

	// ��Ӧ
	MSG_S2C_TAKE_FACEBOOK_COMMEND_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ��������
void CFacebookActivity::OnNetFriendInvite(const FacebookFriendInfo *pFriendInfo)
{
	// ����ѷ�������ĺ����б�
	for (WORD i = 0; i < pFriendInfo->wFriendNum; ++i)
	{
		const char *pszFriendAccount = pFriendInfo->arFriendList[i];

		AddInviteRecord(pszFriendAccount);

		MAPLOG(ERROR_LEVEL, "[%s]�������[%s]", m_pPlayer->GetUTF8RoleName(), pszFriendAccount);
	}

	// ÿ����������Ӻ�����������
	m_pPlayer->OnUpdateDailyMission(27, pFriendInfo->wFriendNum);

	// �ɾ�����Ӻ��������ۼƴ����ɾ�
	m_pPlayer->OnUpdateAchievement(29, pFriendInfo->wFriendNum);

	// ��Ӧ
	MSG_S2C_FACEBOOK_FRIEND_INVITE_RESP msg;
	memcpy_s(&msg.m_InviteFriendInfo, sizeof(FacebookFriendInfo), pFriendInfo, sizeof(FacebookFriendInfo));
	m_pPlayer->SendPacket(&msg);
}

// �����ٻ�
void CFacebookActivity::OnNetFriendSummon(DWORD *pFriendList, WORD wFriendNum)
{
	CFriendManager *pFriendMgr = m_pPlayer->GetFriendManager();

	DWORD dwCurTime = time(NULL);
	DWORD dwSummonNeedOfflineTime = g_Day2Second(5); // �ٻص���Ҫ������ʱ��

	for (WORD i = 0; i < wFriendNum; ++i)
	{
		DWORD dwFriendId = pFriendList[i];

		// ����Ƿ����
		const FriendData *pFriendData = pFriendMgr->GetFriendDataEx(dwFriendId);
		if (!pFriendData)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]Ŀ�겻�Ǻ���[role_id:%d]", __FUNCTION__, dwFriendId);
			return;
		}

		// �Ƿ����ߵ�ָ����ʱ��
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), pFriendData->dwFriendId, false);
		if (!pBaseData)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ѵĻ�������[role_id:%d]", __FUNCTION__, dwFriendId);
			continue;
		}
		if (dwCurTime < pBaseData->dwOfflineTime + dwSummonNeedOfflineTime)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��������ʱ�䲻��,�����ٻ�[role_id:%d]", __FUNCTION__, dwFriendId);
			continue;
		}

		// ����ٻغ��Ѽ�¼
		AddSummonRecord(dwFriendId);

		MAPLOG(ERROR_LEVEL, "[%s]�����ٻغ���[%d]", m_pPlayer->GetUTF8RoleName(), dwFriendId);
	}

	// ÿ����������Ӻ����ٻ�����Ŀ��
	m_pPlayer->OnUpdateDailyMission(26, wFriendNum);

	// �ɾ�����Ӻ����ٻ��ۼƳɾ�Ŀ��
	m_pPlayer->OnUpdateAchievement(28, wFriendNum);

	// ��Ӧ
	MSG_S2C_FACEBOOK_FRIEND_SUMMON_RESP msg;
	msg.m_wFriendNum = wFriendNum;
	memcpy_s(msg.m_arFiendList, sizeof(msg.m_arFiendList), pFriendList, wFriendNum*sizeof(DWORD));
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ�������뽱��
void CFacebookActivity::OnNetTakeFriendInviteReward(WORD wInviteNum)
{
	// ����Ƿ��������������
	if (m_pActivityData->wInviteSuccessFriendNum < wInviteNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ɹ�������ѵ���������[num:%d]", __FUNCTION__, wInviteNum);
		return;
	}

	// ȡ������������
	const FacebookFriendInvite_Config *pInviteCfg = g_pCfgMgr->GetFacebookFriendInvite(wInviteNum);
	if (!pInviteCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ɹ�������ѵ���������[num:%d]", __FUNCTION__, wInviteNum);
		return;
	}

	// ����Ƿ�����ȡ��
	if (IsTakeInviteReward(wInviteNum))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�������뽱������ȡ[num:%d]", __FUNCTION__, wInviteNum);
		return;
	}

	// ���Ž���
	m_pPlayer->AddObjectList(pInviteCfg->vRewardList, TRUE, FROM_GET_FRIEND_INVITE_REWARD);

	// ��¼����ȡ
	AddInviteRewardRecord(wInviteNum);

	// ��Ӧ
	MSG_S2C_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_RESP msg;
	msg.m_wInviteNum = wInviteNum;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ�����ٻ�����
void CFacebookActivity::OnNetTakeFriendSummonReward(WORD wSummonNum)
{
	// ����Ƿ�������ٻ�����
	if (m_pActivityData->wSummonSuccessFriendNum < wSummonNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ɹ��ٻغ��ѵ���������[num:%d]", __FUNCTION__, wSummonNum);
		return;
	}

	// ȡ������������
	const FacebookFriendSummon_Config *pSummonCfg = g_pCfgMgr->GetFacebookFriendSummon(wSummonNum);
	if (!pSummonCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ɹ��ٻغ��ѵ���������[num:%d]", __FUNCTION__, wSummonNum);
		return;
	}

	// ����Ƿ�����ȡ��
	if (IsTakeSummonReward(wSummonNum))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�����ٻؽ�������ȡ[num:%d]", __FUNCTION__, wSummonNum);
		return;
	}

	// ���Ž���
	m_pPlayer->AddObjectList(pSummonCfg->vRewardList, TRUE, FROM_GET_FRIEND_SUMMON_REWARD);

	// ��¼����ȡ
	AddInviteRewardRecord(wSummonNum);

	// ��Ӧ
	MSG_S2C_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_RESP msg;
	msg.m_wSummonNum = wSummonNum;
	m_pPlayer->SendPacket(&msg);
}

// �����Ѿ�����ĺ��Ѽ�¼
void CFacebookActivity::OnNetFriendInviteRecord()
{
	// ��Ӧ
	MSG_S2C_FACEBOOK_FRIEND_INVITE_RECORD_RESP msg;
	msg.m_InviteFriendRecord.wFriendNum = 0;
	m_pPlayer->SendPacket(&msg);
}


// ���ͻ����
void CFacebookActivity::SendActivityData()
{
	MSG_S2C_FACEBOOK_ACTIVITY_DATA_RESP msg;
	msg.m_tActivityData = *m_pActivityData;
	m_pPlayer->SendPacket(&msg);
}

// �Ƿ�����ȡ���뽱��
bool CFacebookActivity::IsTakeInviteReward(WORD wInviteNum)
{
	for (WORD i = 0; i < m_pActivityData->wInviteRewardRecordNum; ++i)
	{
		if (m_pActivityData->arInviteRewardRecord[i] == wInviteNum)
			return true;
	}

	return false;
}

// �Ƿ�����ȡ�ٻؽ���
bool CFacebookActivity::IsTakeSummonReward(WORD wSummonNum)
{
	for (WORD i = 0; i < m_pActivityData->wSummonRewardRecordNum; ++i)
	{
		if (m_pActivityData->arSummonRewardRecord[i] == wSummonNum)
			return true;
	}

	return false;
}

// ������뽱����¼
void CFacebookActivity::AddInviteRewardRecord(WORD wInviteNum)
{
	if (m_pActivityData->wInviteRewardRecordNum >= MAX_FACEBOOK_INVITE_REWARD_RECORD)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���뽱����¼�б�����[num:%d,role_id:%d]", 
			__FUNCTION__, m_pActivityData->wInviteRewardRecordNum, m_pPlayer->GetRoleId());
		return;
	}

	m_pActivityData->arInviteRewardRecord[m_pActivityData->wInviteRewardRecordNum++] = wInviteNum;
}

// ����ٻؽ�����¼
void CFacebookActivity::AddSummonRewardRecord(WORD wSummonNum)
{
	if (m_pActivityData->wSummonRewardRecordNum >= MAX_FACEBOOK_SUMMON_REWARD_RECORD)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ٻؽ�����¼�б�����[num:%d,role_id:%d]",
			__FUNCTION__, m_pActivityData->wSummonRewardRecordNum, m_pPlayer->GetRoleId());
		return;
	}

	m_pActivityData->arSummonRewardRecord[m_pActivityData->wSummonRewardRecordNum++] = wSummonNum;
}

// ����ٻغ��Ѽ�¼
void CFacebookActivity::AddSummonRecord(DWORD dwFriendId)
{
	if (IsSummonRecord(dwFriendId))
		return;

	if (m_pActivityData->wSummonFriendRecordNum >= MAX_FRIEND_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ٻؼ�¼�б�����",__FUNCTION__);
		return;
	}

	RecallFriendData &tRecallData = m_pActivityData->arSummonFriendRecord[m_pActivityData->wSummonFriendRecordNum++];
	tRecallData.dwFriendId = dwFriendId;
	tRecallData.dwTime = time(NULL);
}

// ɾ���ٻؼ�¼
void CFacebookActivity::DelSummonRecord(DWORD dwFriendId)
{
	for (WORD i = 0; i < m_pActivityData->wSummonFriendRecordNum; i++)
	{
		if (m_pActivityData->arSummonFriendRecord[i].dwFriendId == dwFriendId)
		{
			m_pActivityData->arSummonFriendRecord[i] = m_pActivityData->arSummonFriendRecord[--m_pActivityData->wSummonFriendRecordNum];
			break;
		}
	}
}

// ���������Ѽ�¼
void CFacebookActivity::AddInviteRecord(const char *pszFacebookAccount)
{
	if (IsInviteRecord(pszFacebookAccount))
		return;

	FacebookFriendInfo &tFriendInfo = m_pActivityData->tSendInviteFriendInfo;

	if (tFriendInfo.wFriendNum >= MAX_FACEBOOK_FRIEND_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����¼�б�����", __FUNCTION__);
		return;
	}

	strcpy_s(tFriendInfo.arFriendList[tFriendInfo.wFriendNum], 
		sizeof(tFriendInfo.arFriendList[tFriendInfo.wFriendNum]), pszFacebookAccount);

	tFriendInfo.wFriendNum++;
}

// ɾ�������¼
void CFacebookActivity::DelInviteRecord(const char *pszFacebookAccount)
{
	DWORD dwPlatformType = m_pPlayer->GetAccountInfo()->GetChannelUID();
	FacebookFriendInfo &tInviteFriendInfo = m_pActivityData->tSendInviteFriendInfo;
	for (WORD i = 0; i < tInviteFriendInfo.wFriendNum; ++i)
	{
		// ����Ƿ����
		if (strcmp(pszFacebookAccount, tInviteFriendInfo.arFriendList[i]) == 0)
		{
			strcpy_s(tInviteFriendInfo.arFriendList[i], sizeof(tInviteFriendInfo.arFriendList[i]),
				tInviteFriendInfo.arFriendList[tInviteFriendInfo.wFriendNum - 1]);
			tInviteFriendInfo.wFriendNum--;
			break;
		}
	}
}

// �Ƿ��ٻؼ�¼
bool CFacebookActivity::IsSummonRecord(DWORD dwFriendId)
{
	for (WORD i = 0; i < m_pActivityData->wSummonFriendRecordNum; ++i)
	{
		if (m_pActivityData->arSummonFriendRecord[i].dwFriendId == dwFriendId)
			return true;
	}
	return false;
}

// �Ƿ������¼
bool CFacebookActivity::IsInviteRecord(const char *pszFacebookAccount)
{
	for (WORD i = 0; i < m_pActivityData->tSendInviteFriendInfo.wFriendNum; ++i)
	{
		if (strcmp(m_pActivityData->tSendInviteFriendInfo.arFriendList[i], pszFacebookAccount) == 0)
			return true;
	}
	return false;
}

// ��ⱻ��������Ƿ��¼
void CFacebookActivity::CheckInviteFriendIsLogin()
{
	vector<string> vDeleteList;
	DWORD dwPlatformType = m_pPlayer->GetAccountInfo()->GetChannelUID();
	FacebookFriendInfo &tInviteFriendInfo = m_pActivityData->tSendInviteFriendInfo;
	for (WORD i = 0; i < tInviteFriendInfo.wFriendNum; ++i)
	{
		char *pAccount = tInviteFriendInfo.arFriendList[i];
		if (PlayerBaseDataManager::Instance()->IsInvalidAccount(dwPlatformType, pAccount))
		{
			vDeleteList.push_back(pAccount);
		}
	}

	// ɾ���Ѿ����������
	for (vector<string>::iterator iter = vDeleteList.begin(); iter != vDeleteList.end(); ++iter)
	{
		DelInviteRecord(iter->c_str());
	}

	m_pActivityData->wInviteSuccessFriendNum += vDeleteList.size();
}

// ��ⱻ�ٻغ����Ƿ��¼
void CFacebookActivity::CheckRecallFriendIsLogin()
{
	DWORD dwCurTime = time(NULL);
	vector<DWORD> vDeleteList;
	for (WORD i = 0; i < m_pActivityData->wSummonFriendRecordNum; ++i)
	{
		DWORD dwFriendId = m_pActivityData->arSummonFriendRecord[i].dwFriendId;
		const PlayerBaseData *pBaseData = PlayerBaseDataManager::Instance()->GetPlayerBaseDataById(dwFriendId);
		if (!pBaseData)
			continue;

		// ���� ���� ����ʱ��С��5��
		if (!pBaseData->dwOfflineTime || g_GetPassTime(pBaseData->dwOfflineTime, dwCurTime) < g_Day2Second(5))
		{
			vDeleteList.push_back(dwFriendId);
		}
	}

	// ɾ���Ѿ����������
	for (vector<DWORD>::iterator iter = vDeleteList.begin(); iter != vDeleteList.end(); ++iter)
	{
		DelSummonRecord(*iter);
	}

	m_pActivityData->wSummonSuccessFriendNum += vDeleteList.size();
}

// ����׼������ʱ��
void CFacebookActivity::SetReadyToCommendTime(DWORD dwTime)
{
	m_pActivityData->dwReadyToCommendTime = dwTime;
}

// ����׼������ʱ��
void CFacebookActivity::SetReadyToShareTime(DWORD dwTime)
{
	m_pActivityData->dwReadyToShareTime = dwTime;
}