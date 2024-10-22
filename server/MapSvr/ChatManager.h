#pragma once
#include "../ToolKit/Singleton.h"

class NetMsgBody;
class  Player;
struct ChatEntry;

class ChatManager : public Singleton<ChatManager>
{
public:
	ChatManager();
	~ChatManager();

public:
	BOOL				Init();
	VOID				Release();
	VOID				Update();

public:

	// 聊天消息
	VOID				OnNetChat(Player* pPlayer, const ChatReqData *pChatInfo);

	// 请求语音
	VOID				OnNetChatVoice(Player* pPlayer, DWORD dwVoiceId);

	// 请求聊天记录
	void				OnNetChatRecordList(Player* pPlayer, BYTE byChatType);

public:

	VOID				ChangeLocalChar(char* pszSrc, char* pszDst, size_t nDstSize);

	// 发送广播消息
	VOID				SendBroadcastMsg(DWORD dwBroadCastId, DWORD dwValue1, const char *pszText1, 
		DWORD dwValue2 = 0, const char *pszText2 = NULL, DWORD dwValue3 = 0, const char *pszText3 = NULL);

	// 发送广播消息
	VOID				SendBroadcastMsg(DWORD dwBroadCastId, const char *pszText = NULL);

protected:

	// 世界频道
	int					OnWorldChat(Player* pPlayer, NetMsgBody *pMsg);

	// 私聊
	int					OnP2PChat(Player* pPlayer, NetMsgBody *pMsg, const char *pszTargetName);

	// 公会
	int					OnGuildChat(Player* pPlayer, NetMsgBody *pMsg);

	// 队伍
	int					OnTeamChat(Player* pPlayer, NetMsgBody *pMsg);

	// 组队大厅
	int					OnTeamHallChat(Player* pPlayer, NetMsgBody *pMsg);

protected:
	// 添加语音
	void				AddVoice(DWORD dwVoiceId, const char *pszVoice);

	// 取语音数据
	ChatVoice*		    GetVoice(DWORD dwVoiceId);

	// 添加世界频道聊天记录
	void				AddWorldChatRecord(DWORD dwServerId, const ChatRecord &tChatRecord);

	// 添加大厅聊天记录
	void				AddTeamHallChatRecord(const ChatRecord &tChatRecord);

	// 取一个新的系列ID
	DWORD				GetNewSeriesId();

	// 检测世界聊天条件
	bool				CheckWorldChat(Player* pPlayer);

private:
	//ChatEntry*			m_pChatEntry;
	//DWORD				m_dwProcessBroadcastTimer;

	// 临时数据使用
	ChatRecord			m_tChatRecord;

	// 语音列表
	map<DWORD, ChatVoice> m_VoiceList;

	// 世界频道聊天记录列表
	//vector<ChatRecord> ;
	map<DWORD, list<ChatRecord>> m_ChatRecordList;

	// 组队大厅聊天记录列表 todo::暂时不处理,肥涵说不用处理... 2017-10-16-23:59:58
	list<ChatRecord> m_TeamHallChatRecordList;

	// 系列ID
	DWORD m_dwSeriesId;		
};