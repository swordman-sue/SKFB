#pragma once

#include <map>
#include <hash_map>
#include "../ToolKit/Singleton.h"
#include "../Common/PacketStruct_Code.h"

class Player;

//typedef std::hash_map<DWORD, Player*>	PlayerMap;
typedef std::map<DWORD, Player*>		PlayerMap;
typedef PlayerMap::const_iterator		PlayerMapIter;

typedef std::string			 			String;
//typedef std::hash_map<String, Player*>	PlayerNameMap;
typedef std::map<String, Player*>		PlayerNameMap;
typedef PlayerNameMap::const_iterator	PlayerNameMapIter;

//��ҹ�����
class PlayerManager : public Singleton<PlayerManager>
{
public:
	PlayerManager();
	~PlayerManager();

public:
	BOOL				Init();
	VOID				Release();
	VOID				Update(DWORD dwTick);
	void                UpdateSys(SystemIDs id,ActivityOpe Ope);
	void				OnNewDay();
	void				OnNewWeek();
	void				OnNewMonth();

	VOID				AddPlayer(Player* pPlayer);
	VOID				AddPlayerName(Player* pPlayer);
	VOID				RemovePlayer(Player* pPlayer);

	// �޸Ľ�ɫ����
	VOID				RoleNameModify(Player* pPlayer, const char* pszOldName, const char* pszNewName);

	Player*				FindPlayerByRoleId(DWORD dwRoleId) const;
	Player*				FindPlayerByCharID(DWORD dwCharID) const;
	Player*				FindPlayerByGuid(DWORD dwGuid) const;
	Player*				FindPlayerByCharName(const char* pCharName) const; 
	Player*				GetFirstPlayer() const;
	void				GetAllPlayer(Player** pPlayer,int& count,int nMaxcount, DWORD dwSeverId = 0);
	void				GetAllPlayer(vector<Player*> &vPlayerList, DWORD dwSeverId = 0);
	

	void                SendEmilByCondition(DWORD dwCondition,DWORD dwID); // ������

	DWORD				GetPlayerNum() const { return m_PlayerCharIDMap.size(); }
	DWORD				GetPlayerNum(DWORD dwServerId);
	void				SendPacketAround(NetMsgBody * pMsg, DWORD dwServerId = 0, DWORD dwRoleId = 0);
	void				SendWorldChat(NetMsgBody * pMsg, DWORD dwServerId = 0, DWORD dwRoleId = 0);
	VOID				ProcessEventBroadcast(EventBroadcastType enBroadcastType, BroadcastInfo& tBroadcastInfo); 	// ������
	void                CheckTimeOutRestrictions();	// ������
	void                SendAllRestrictions(); // ������
	VOID				ProcessReset();
	VOID                UpdateSign(); // ������

public:
	VOID				ShowPlayerList();

	void				SaveAllDataToDB();

private:
	PlayerMap			m_PlayerCharIDMap;			//��CharIDΪKey��Map
	PlayerMap			m_PlayerGuidMap;			//��GUIDΪKey��Map
	PlayerNameMap		m_PlayerNameMap;			//��NameΪKey��Map

	map<DWORD, DWORD>	m_ServerRoleNumList;		//��������ɫ�����б�[server_id, player_num]
};