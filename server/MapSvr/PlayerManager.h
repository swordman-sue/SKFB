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

//玩家管理器
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

	// 修改角色名称
	VOID				RoleNameModify(Player* pPlayer, const char* pszOldName, const char* pszNewName);

	Player*				FindPlayerByRoleId(DWORD dwRoleId) const;
	Player*				FindPlayerByCharID(DWORD dwCharID) const;
	Player*				FindPlayerByGuid(DWORD dwGuid) const;
	Player*				FindPlayerByCharName(const char* pCharName) const; 
	Player*				GetFirstPlayer() const;
	void				GetAllPlayer(Player** pPlayer,int& count,int nMaxcount, DWORD dwSeverId = 0);
	void				GetAllPlayer(vector<Player*> &vPlayerList, DWORD dwSeverId = 0);
	

	void                SendEmilByCondition(DWORD dwCondition,DWORD dwID); // 已作废

	DWORD				GetPlayerNum() const { return m_PlayerCharIDMap.size(); }
	DWORD				GetPlayerNum(DWORD dwServerId);
	void				SendPacketAround(NetMsgBody * pMsg, DWORD dwServerId = 0, DWORD dwRoleId = 0);
	void				SendWorldChat(NetMsgBody * pMsg, DWORD dwServerId = 0, DWORD dwRoleId = 0);
	VOID				ProcessEventBroadcast(EventBroadcastType enBroadcastType, BroadcastInfo& tBroadcastInfo); 	// 已作废
	void                CheckTimeOutRestrictions();	// 已作废
	void                SendAllRestrictions(); // 已作废
	VOID				ProcessReset();
	VOID                UpdateSign(); // 已作废

public:
	VOID				ShowPlayerList();

	void				SaveAllDataToDB();

private:
	PlayerMap			m_PlayerCharIDMap;			//以CharID为Key的Map
	PlayerMap			m_PlayerGuidMap;			//以GUID为Key的Map
	PlayerNameMap		m_PlayerNameMap;			//以Name为Key的Map

	map<DWORD, DWORD>	m_ServerRoleNumList;		//服务器角色数量列表[server_id, player_num]
};