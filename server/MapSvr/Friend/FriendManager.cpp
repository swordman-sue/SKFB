#include "stdafx.h"
#include "FriendManager.h"
#include "..\Player.h"
#include "..\PlayerManager.h"
#include "..\PlayerBaseDataManager.h"
#include "ToolKit\Rand.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "..\Guild\GuildManager.h"
#include "..\Email\EmailManager.h"
#include "..\ConfigManager.h"


CFriendManager::CFriendManager()
{

}

CFriendManager::~CFriendManager()
{

}

void CFriendManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pFriendInfo = &pPlayer->GetRoleFullInfo().tFriendInfo;
}

void CFriendManager::OnNewDay()
{
	m_pFriendInfo->wGetEnergyTimes = 0;
	for (WORD i = 0; i < m_pFriendInfo->wFriendNum; ++i)
	{
		m_pFriendInfo->arFriendList[i].wIsGiveEnergy = false;
	}
}

// ��������б�
void CFriendManager::OnFriendList()
{
	SendBlackList();

	MSG_S2C_FRIEND_LIST_RESP msg;
	msg.m_byGetEnergyTimes = m_pFriendInfo->wGetEnergyTimes;
 	msg.m_byFriendNum = 0;
	for (WORD i = 0; i < m_pFriendInfo->wFriendNum; ++i)
	{
		FriendData &sSrc = m_pFriendInfo->arFriendList[i];
		const PlayerBaseData *pFriendBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), sSrc.dwFriendId);
		if (!pFriendBaseData)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_LIST_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵĻ�������[RoleId:%d]", __FUNCTION__, sSrc.dwFriendId);
			return;
		}
		ClientFriendData &sDest = msg.m_FriendList[msg.m_byFriendNum++];
		memcpy_s(&sDest, sizeof(PlayerCommonData), pFriendBaseData, sizeof(PlayerCommonData));
		sDest.byIsGiveEnergy = sSrc.wIsGiveEnergy;
		sDest.byTakeEnergyState = sSrc.wTakeEnergyState;
		//strcpy_s(sDest.szAccount, sizeof(sDest.szAccount), pFriendBaseData->szAccount);
	}
	m_pPlayer->SendPacket(&msg);
}

// ����������б�
void CFriendManager::OnBlackList()
{
	SendBlackList();
}

// ���������б�
void CFriendManager::OnApplyList()
{
	MSG_S2C_FRIEND_APPLY_LIST_RESP msg;
	msg.m_byApplyNum = 0;
	int nCommonDataSize = sizeof(PlayerCommonData);
	for (BYTE i = 0; i < m_pFriendInfo->wApplyNum; ++i)
	{
		DWORD dwFriendId = m_pFriendInfo->arApplyList[i];
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwFriendId);
		if (pBaseData)
		{
			memcpy_s(&msg.m_ApplyList[msg.m_byApplyNum++], nCommonDataSize, pBaseData, nCommonDataSize);
		}
	}
	m_pPlayer->SendPacket(&msg);
}

// �����Ƽ��б�
void CFriendManager::OnRecommendList()
{
	MSG_S2C_FRIEND_RECOMMEND_LIST_RESP msg;
	memset(msg.m_RecommendList, 0, sizeof(msg.m_RecommendList));
	msg.m_byRecommendNum = 0;

	CFriendManager *pFriendMgr = m_pPlayer->GetFriendManager();

	// todo::��ʱ��������,�����պ�Ӧ�û����
	const PlayerBaseDataMap *pPlayerList = PlayerBaseDataManager::Instance()->GetPlayerList(m_pPlayer->GetServerId());
	if (!pPlayerList)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������б�[ServerId:%d]", __FUNCTION__, m_pPlayer->GetServerId());
		return;
	}

	for (PlayerBaseDataMap::const_iterator iter = pPlayerList->begin(); iter != pPlayerList->end(); ++iter)
	{
		if (msg.m_byRecommendNum >= MAX_FRIEND_RECOMMEND_NUM)
			break;

		const PlayerBaseData *pTarget = iter->second;

		if (m_pPlayer->GetCharID() != pTarget->dwRoleId &&			// �����Լ�
			!pFriendMgr->IsFriend(pTarget->dwRoleId) &&				// ���Ǻ���
			!pFriendMgr->IsBlack(pTarget->dwRoleId))				// ���ǵ�����
		{
			ClientFriendData &sDest = msg.m_RecommendList[msg.m_byRecommendNum++];
			memcpy_s(&sDest, sizeof(PlayerCommonData), pTarget, sizeof(PlayerCommonData));
		}
	}

	m_pPlayer->SendPacket(&msg);
}

// ��Ӻ���
void CFriendManager::OnFriendAdd(DWORD dwTargetId)
{
	MSG_S2C_FRIEND_ADD_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	msg.m_wResult = 0;

	// �Ƿ��Լ�
	if (m_pPlayer->GetCharID() == dwTargetId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ADD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���ܼ��Լ�Ϊ����[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// ����Ƿ����ں����б�
	if (GetFriendData(dwTargetId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ADD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]Ŀ�����Ǻ���[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// ����Լ���������
	if (IsFriendListFull())
	{
		msg.m_wResult = 1;
		m_pPlayer->SendPacket(&msg);
		MAPLOG(ERROR_LEVEL, "[%s]�Լ��ĺ��ѵ������Ѵ�����[Num:%d]", __FUNCTION__, GetFriendNum());
		return;
	}

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);

	// Ŀ������
	if (pTarget)
	{
		CFriendManager *pFriendMgr = pTarget->GetFriendManager();

		if (pFriendMgr->IsFriendListFull())
		{
			msg.m_wResult = 2;
			m_pPlayer->SendPacket(&msg);
			MAPLOG(GUI_LEVEL, "[%s]Ŀ��ĺ��ѵ������Ѵ�����[Num:%d]", __FUNCTION__, GetFriendNum());
			return;
		}

		// ���Ƿ���Ŀ��ĺ�������
		if (!pFriendMgr->IsBlack(m_pPlayer->GetCharID()))
		{
			// ������ӵ�Ŀ��������б���
			pFriendMgr->AddApplyData(m_pPlayer->GetCharID());
		}

		// ��ɫ��Ϣ����
		pTarget->RoleInfoUpdate();
	}
	// Ŀ������
	else
	{
		// ������ӵ�Ŀ��������б���
		PlayerBaseDataManager::Instance()->AddFriendApplyData(m_pPlayer->GetServerId(), dwTargetId, m_pPlayer->GetCharID());
	}

	m_pPlayer->SendPacket(&msg);
}

// ɾ������ 
void CFriendManager::OnFriendDel(DWORD dwTargetId)
{
	// ���Ŀ���Ƿ����
	if (!GetFriendData(dwTargetId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_DEL_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]Ŀ�겻�Ǻ���,����ɾ��[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	if (pTarget)
	{
		// Ŀ������б�ɾ����
		pTarget->GetFriendManager()->RemoveFriend(m_pPlayer->GetCharID());

		// ��ɫ��Ϣ����
		pTarget->RoleInfoUpdate();
	}
	else
	{
		// ��������ȥ����ɾ��
		FriendData sDelTargetData;
		sDelTargetData.dwFriendId = m_pPlayer->GetCharID();
		UpateFriendDataInDB(dwTargetId, sDelTargetData, FRIEND_UPDATE_DEL_FRIEND);
	}

	// �ҵĺ����б��Ŀ��ɾ��
	RemoveFriend(dwTargetId);

	MSG_S2C_FRIEND_DEL_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	m_pPlayer->SendPacket(&msg);
}

// ������ѻظ�
void CFriendManager::OnFriendApplyReply(DWORD dwTargetId, BYTE byIsAccept)
{
	// ���Ŀ���Ƿ����Լ��������б���
	if (!IsInApplyList(dwTargetId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_APPLY_REPLY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]Ŀǰû�������б���[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// �������б����õ�
	RemoveApplyData(dwTargetId);

	// ���Ŀ���Ƿ������ҵĺ���
	if (GetFriendData(dwTargetId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_APPLY_REPLY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]Ŀ�������ҵĺ���[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// ����
	if (byIsAccept)
	{
		// ����Լ����б��Ƿ�����
		if (IsFriendListFull())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_APPLY_REPLY_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]��ǰ���������Ѵ����ޣ������ټӺ���[targetid:%d]", __FUNCTION__, dwTargetId);
			return;
		}

		Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
		if (pTarget)
		{ // Ŀ������
			CFriendManager *pTargetFriendMgr = pTarget->GetFriendManager();
			// Ŀ��ĺ����б�����
			if (pTargetFriendMgr->IsFriendListFull())
			{
				m_pPlayer->SendErrorMsg(ERROR_FRIEND_LIST_SIZE_LIMIT);
				return;
			}

			// Ŀ������б������
			pTargetFriendMgr->AddFriendData(m_pPlayer->GetRoleId());
			pTargetFriendMgr->RemoveBlackData(m_pPlayer->GetRoleId());
		
			// �ҵĺ����б����Ŀ��
			AddFriendData(dwTargetId);
			RemoveBlackData(dwTargetId);

			// ��ɫ��Ϣ����
			pTarget->RoleInfoUpdate();

			// �����Ҳ��Ŀ��������б���
			if (pTargetFriendMgr->IsInApplyList(m_pPlayer->GetRoleId()))
				pTargetFriendMgr->RemoveApplyData(m_pPlayer->GetRoleId());
		}
		else
		{ // Ŀ������

			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwTargetId);
			if (!pBaseData)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_APPLY_REPLY_REQ);
				MAPLOG(GUI_LEVEL, "[%s]�Ҳ��ý�ɫ��������[RoleId:%d]", __FUNCTION__, dwTargetId);
				return;
			}

			// ���Ŀ���б��Ƿ�����
			if (pBaseData->byFriendNum >= GetFriendNumMax(pBaseData->wLevel))
			{
				m_pPlayer->SendErrorMsg(ERROR_FRIEND_LIST_SIZE_LIMIT);
				return;
			}

			// �ҵĺ����б����Ŀ��
			AddFriendData(dwTargetId);
			RemoveBlackData(dwTargetId);

			// Ŀ������б������
			FriendData sMyFriendData;
			memset(&sMyFriendData, 0, sizeof(sMyFriendData));
			sMyFriendData.dwFriendId = m_pPlayer->GetRoleId();
			UpateFriendDataInDB(dwTargetId, sMyFriendData, FRIEND_UPDATE_ADD_FRIEND);

			// Ŀ����ѵ���������
			PlayerBaseDataManager::Instance()->AddFriendNum(m_pPlayer->GetServerId(), dwTargetId);
		}

		FriendDataUpdateNotify(dwTargetId);

		// ���ʼ�֪ͨĿ��(����)
		EmailInfo tEailInfo(3);
		tEailInfo.AddContext(EMAIL_CONTEXT_STRING, m_pPlayer->GetRoleName());
		EmailManager::AddEmail(dwTargetId, tEailInfo, true);
	}

	MSG_S2C_FRIEND_APPLY_REPLY_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	msg.m_byAccept = byIsAccept;
	m_pPlayer->SendPacket(&msg);
}

// ��Ӻ�����
void CFriendManager::OnBlackAdd(DWORD dwTargetId)
{
	// ���Ŀ���Ƿ����ں�����
	if (IsBlack(dwTargetId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BLACK_ADD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]Ŀ�����ں������б�[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwTargetId);
	if (!pBaseData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BLACK_ADD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]Ŀ�겻����[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// Ŀ�����Լ��ĺ���
	if (GetFriendData(dwTargetId))
	{
		Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);

		// ����
		if (pTarget)
		{
			// Ŀ������б���Լ�ȥ��
			pTarget->GetFriendManager()->RemoveFriend(m_pPlayer->GetCharID());

			// ��ɫ��Ϣ����
			pTarget->RoleInfoUpdate();
		}
		// ����
		else
		{
			// �����������ɾ��
			FriendData sFriendData;
			sFriendData.dwFriendId = m_pPlayer->GetCharID();
			UpateFriendDataInDB(dwTargetId, sFriendData, FRIEND_UPDATE_ADD_BLACK);
		}

		// ��Ŀ��Ӻ����б�ɾ��
		m_pPlayer->GetFriendManager()->RemoveFriend(dwTargetId);
	}

	// ��Ŀ����Ӻ�����
	AddBlackData(dwTargetId);

	MSG_S2C_BLACK_ADD_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	m_pPlayer->SendPacket(&msg);
}

// ɾ��������
void CFriendManager::OnBlackDel(DWORD dwTargetId)
{
	if (!IsBlack(dwTargetId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BLACK_DEL_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]Ŀ��ľ�к�������[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	RemoveBlackData(dwTargetId);

	MSG_S2C_BLACK_DEL_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	m_pPlayer->SendPacket(&msg);
}

// ���;���
void CFriendManager::OnEnergyGive(DWORD dwTargetId)
{
	// ����Ƿ����
	FriendData *pFriendData = GetFriendData(dwTargetId);
	if (!pFriendData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ENERGY_GIVE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]Ŀ�겻�Ǻ��Ѳ������;���[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// ����Ƿ��Ѹ���
	if (pFriendData->wIsGiveEnergy)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ENERGY_GIVE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ѿ���Ŀ�����͹�����[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	if (pTarget)
	{
		pTarget->GetFriendManager()->SetGetEnergyState(m_pPlayer->GetCharID(), FIEND_ENERGY_STATE_READY);
		pTarget->GetFriendManager()->FriendDataUpdateNotify(m_pPlayer->GetCharID());

		// ��ɫ��Ϣ����
		pTarget->RoleInfoUpdate();
	}
	else
	{
		// ���;���
		PlayerBaseDataManager::Instance()->AddGiveEnergyData(m_pPlayer->GetServerId(), dwTargetId, m_pPlayer->GetRoleId());
	}

	pFriendData->wIsGiveEnergy = true;

	MSG_S2C_FRIEND_ENERGY_GIVE_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	m_pPlayer->SendPacket(&msg);

	// �ճ�����
	m_pPlayer->OnUpdateDailyMission(4);

	// ����ȼ�����
	//m_pPlayer->OnUpdateWorldTaskEvent(4);
}

// ��ȡ���� 
void CFriendManager::OnTakeEnergy(DWORD *pTargetList, BYTE byTargetNum)
{
	// �����տ���ȡ�Ĵ���
	if (m_pFriendInfo->wGetEnergyTimes + byTargetNum > GetFriendNumMax())
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ENERGY_GET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�������;�����ȡ�����Ѵ�����[times:%d]", __FUNCTION__, m_pFriendInfo->wGetEnergyTimes);
		return;
	}

	for (BYTE i = 0; i < byTargetNum; ++i)
	{
		DWORD dwTargetId = pTargetList[i];
		// ����Ƿ����
		FriendData *pFriendData = GetFriendData(dwTargetId);
		if (!pFriendData)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ENERGY_GET_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]Ŀ�겻�Ǻ���,������ȡ����[targetid:%d]", __FUNCTION__, dwTargetId);
			return;
		}

		// ����Ƿ������ȡ
		if (FIEND_ENERGY_STATE_READY != pFriendData->wTakeEnergyState)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ENERGY_GET_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]������ȡĿ�����͵ľ���[targetid:%d,state:%d]", __FUNCTION__,
				dwTargetId, pFriendData->wTakeEnergyState);
			return;
		}

		pFriendData->wTakeEnergyState = FIEND_ENERGY_STATE_TAKE;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_ENERGY, byTargetNum);

	m_pFriendInfo->wGetEnergyTimes += byTargetNum;

	MSG_S2C_FRIEND_ENERGY_GET_RESP msg;
	msg.m_byTargetNum = byTargetNum;
	memcpy_s(msg.m_TargetList, byTargetNum*sizeof(DWORD), pTargetList, byTargetNum*sizeof(DWORD));
	m_pPlayer->SendPacket(&msg);
}

// ����Ŀ��
void CFriendManager::OnNetTargetFind(const char *pszTargetName)
{
	MSG_S2C_FRIEND_TARGET_FIND_RESP msg;
	memset(&msg.m_TargetData, 0, sizeof(PlayerCommonData));
	const PlayerBaseData *pBaseData = PlayerBaseDataManager::Instance()->GetPlayerBaseDataByName(m_pPlayer->GetServerId(), pszTargetName);
	if (pBaseData)
	{
		memcpy_s(&msg.m_TargetData, sizeof(PlayerCommonData), pBaseData, sizeof(PlayerCommonData));
	}
	m_pPlayer->SendPacket(&msg);
}

// ��Ӻ�����������
bool CFriendManager::AddApplyData(DWORD dwTargetId)
{
	if (IsInApplyList(dwTargetId))
		return true;

	if (m_pFriendInfo->wApplyNum >= MAX_FRIEND_APPLY_NUM)
		return false;

	m_pFriendInfo->arApplyList[m_pFriendInfo->wApplyNum++] = dwTargetId;

	return true;
}

// ��ѯ��������
bool CFriendManager::IsInApplyList(DWORD dwRoleId)
{
	for (WORD i = 0; i < m_pFriendInfo->wApplyNum; ++i)
	{
		if (m_pFriendInfo->arApplyList[i] == dwRoleId)
		{
			return true;
		}
	}
	return false;
}

// ������;�������
void CFriendManager::AddGiveEnergyData(DWORD dwRoleId)
{
	FriendData *pFriendData = GetFriendData(dwRoleId);
	if (pFriendData)
	{
		pFriendData->wIsGiveEnergy = true;
	}
}

// ���þ�����ȡ״̬
void CFriendManager::SetGetEnergyState(DWORD dwRoleId, BYTE byState)
{
	FriendData *pFriendData = GetFriendData(dwRoleId);
	if (pFriendData)
	{
		pFriendData->wTakeEnergyState = byState;
	}
}

// ɾ������
void CFriendManager::RemoveFriend(DWORD dwRoleId)
{
	for (WORD i = 0; i < m_pFriendInfo->wFriendNum; ++i)
	{
		if (m_pFriendInfo->arFriendList[i].dwFriendId == dwRoleId)
		{
			//FriendDataUpdateNotify(m_pFriendInfo->arFriendList[i], 2);
			m_pFriendInfo->arFriendList[i] = m_pFriendInfo->arFriendList[--m_pFriendInfo->wFriendNum];

			MSG_S2C_FRIEND_DATA_UPDATE_NOTIFY msg;
			msg.m_byFlag = false;
			msg.m_byType = 1;
			msg.m_FriendData.dwRoleId = dwRoleId;
			m_pPlayer->SendPacket(&msg);
			break;
		}
	}
}

// ɾ�������������
void CFriendManager::RemoveApplyData(DWORD dwRoleId)
{
	for (WORD i = 0; i < m_pFriendInfo->wApplyNum; ++i)
	{
		if (m_pFriendInfo->arApplyList[i] == dwRoleId)
		{
			m_pFriendInfo->arApplyList[i] = m_pFriendInfo->arApplyList[--m_pFriendInfo->wApplyNum];
			break;
		}
	}
}

// ��Ӻ�������
void CFriendManager::AddFriendData(DWORD dwFriendId)
{
	if (m_pFriendInfo->wFriendNum >= MAX_FRIEND_NUM)
		return;

	if (GetFriendData(dwFriendId))
		return;

	FriendData &sDest = m_pFriendInfo->arFriendList[m_pFriendInfo->wFriendNum++];
	memset(&sDest, 0, sizeof(sDest));
	sDest.dwFriendId = dwFriendId;
}

// �Ƿ�����б�����
bool CFriendManager::IsFriendListFull()
{
	return GetFriendNum() >= GetFriendNumMax() ? true : false;
}

// ���º������ݻص�
void CFriendManager::UpdateFriendDataCallback(BYTE byType, const FriendData &tFriendData)
{
	// ɾ������
	if (FRIEND_UPDATE_DEL_FRIEND == byType)
	{

	}
	// ��Ӻ�����
	else if (FRIEND_UPDATE_ADD_BLACK == byType)
	{

	}
	// ��Ӻ���
	else
	{
		
	}
}

// �������ݸ���֪ͨ
void CFriendManager::FriendDataUpdateNotify(DWORD dwFriendRoleId, BYTE byType)
{
	FriendData *pFrindData = GetFriendData(dwFriendRoleId);
	if (pFrindData)
	{
		FriendDataUpdateNotify(*pFrindData);
	}
}

// �������ݸ���֪ͨ
void CFriendManager::FriendDataUpdateNotify(const FriendData &tFriendData, BYTE byType)
{
	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), tFriendData.dwFriendId);
	if (!pBaseData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵĻ�������[RoleId:%d]", __FUNCTION__, tFriendData.dwFriendId);
		return;
	}

	MSG_S2C_FRIEND_DATA_UPDATE_NOTIFY msg;
	msg.m_byFlag = true;
	msg.m_byType = byType;
	memcpy_s(&msg.m_FriendData, sizeof(PlayerCommonData), pBaseData, sizeof(PlayerCommonData));
	msg.m_FriendData.byIsGiveEnergy = tFriendData.wIsGiveEnergy;
	msg.m_FriendData.byTakeEnergyState = tFriendData.wTakeEnergyState;
	m_pPlayer->SendPacket(&msg);
}

// �Ƿ����
bool CFriendManager::IsFriend(DWORD dwTargetId)
{
	return GetFriendData(dwTargetId) ? true : false;
}

// �Ƿ������
bool CFriendManager::IsBlack(DWORD dwTargetId)
{
	for (WORD i = 0; i < m_pFriendInfo->wBlackNum; ++i)
	{
		if (m_pFriendInfo->arBlackList[i] == dwTargetId)
		{
			return true;
		}
	}
	return false;
}

// ȡ������������
BYTE CFriendManager::GetFriendNumMax()
{
	return GetFriendNumMax(m_pPlayer->GetLevel());
}

// ȡ������������
BYTE CFriendManager::GetFriendNumMax(WORD wRoleLevel)
{
	const RoleLevel_Config *pLevelCfg = g_pCfgMgr->GetRoleLevel(wRoleLevel);
	if (!pLevelCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ������ɫ�ȼ�����[Level:%d]", __FUNCTION__, wRoleLevel);
		return 0;
	}

	return BYTE(pLevelCfg->dwFriendNumMax);
}

// ��������
bool CFriendManager::UpdateClientFriendData(ClientFriendData &tFriendData, DWORD dwFriendId)
{
	const PlayerBaseData *pPlayerBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwFriendId);
	if (pPlayerBaseData)
	{
		memcpy_s(&tFriendData, sizeof(PlayerCommonData), pPlayerBaseData, sizeof(PlayerCommonData));
	}
	return true;
}

// ����������º�������
void CFriendManager::UpateFriendDataInDB(DWORD dwUpateRoleId, const FriendData &tFriendData, BYTE byOperateType)
{
	// �����������ɾ��
	FriendDataUpdate_Load* pQuery = FriendDataUpdate_Load::ALLOC();
	pQuery->SetIndex(DB_FRIEND_DATA_UPDATE);
	pQuery->SetThreadIndex(DB_THREAD_FRIEND);
	pQuery->SetOperateType(byOperateType);
	pQuery->SetKey(m_pPlayer->GetCharID());
	pQuery->SetUpdateRoleId(dwUpateRoleId);
	pQuery->SetFriendData(tFriendData);
	g_MapServer.MapDBQuery(pQuery);
}

// ���ͺ������б�
void CFriendManager::SendBlackList()
{
	MSG_S2C_BLACK_LIST_RESP msg;
	msg.m_byBlackNum = 0;
	int nCommonDataSize = sizeof(PlayerCommonData);
	for (WORD i = 0; i < m_pFriendInfo->wBlackNum; ++i)
	{
		DWORD dwBlackId = m_pFriendInfo->arBlackList[i];
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwBlackId);
		if (pBaseData)
		{
			memcpy_s(&msg.m_BlackList[msg.m_byBlackNum++], nCommonDataSize, pBaseData, nCommonDataSize);
		}
	}
	m_pPlayer->SendPacket(&msg);
}

// ȡ��������
const FriendData* CFriendManager::GetFriendDataEx(DWORD dwRoleId)
{
	return GetFriendData(dwRoleId);
}

// �Ƿ��к��
bool CFriendManager::IsHaveRedPoint()
{
	// �����ѽ����������б�������������ʱ
	if (m_pFriendInfo->wApplyNum)
		return true;

	// �����ѽ������п���ȡ�ľ���û����ȡʱ
	for (WORD i = 0; i < m_pFriendInfo->wFriendNum; ++i)
	{
		FriendData *pFriend = &m_pFriendInfo->arFriendList[i];
		if (FIEND_ENERGY_STATE_READY == pFriend->wTakeEnergyState)
			return true;
	}

	return false;
}

// ȡ��������
FriendData* CFriendManager::GetFriendData(DWORD dwRoleId)
{
	for (WORD i = 0; i < m_pFriendInfo->wFriendNum; ++i)
	{
		if (m_pFriendInfo->arFriendList[i].dwFriendId == dwRoleId)
		{
			return &m_pFriendInfo->arFriendList[i];
		}
	}
	return NULL;
}

// ɾ������������
void CFriendManager::RemoveBlackData(DWORD dwRoleId)
{
	for (WORD i = 0; i < m_pFriendInfo->wBlackNum; ++i)
	{
		if (m_pFriendInfo->arBlackList[i] == dwRoleId)
		{
			m_pFriendInfo->arBlackList[i] = m_pFriendInfo->arBlackList[--m_pFriendInfo->wBlackNum];
			break;
		}
	}
}

// ��Ӻ�����
void CFriendManager::AddBlackData(DWORD dwTargetId)
{
	if (m_pFriendInfo->wBlackNum >= MAX_BLACK_NUM)
		return;

	if (IsBlack(dwTargetId))
		return;

	m_pFriendInfo->arBlackList[m_pFriendInfo->wBlackNum++] = dwTargetId;

	// ֪ͨ�ͻ���
	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwTargetId);
	if (pBaseData)
	{
		FriendData tFriendData;
		memset(&tFriendData, 0, sizeof(tFriendData));
		tFriendData.dwFriendId = pBaseData->dwRoleId;
		FriendDataUpdateNotify(tFriendData, 2);
	}
}