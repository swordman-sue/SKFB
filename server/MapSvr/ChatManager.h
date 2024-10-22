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

	// ������Ϣ
	VOID				OnNetChat(Player* pPlayer, const ChatReqData *pChatInfo);

	// ��������
	VOID				OnNetChatVoice(Player* pPlayer, DWORD dwVoiceId);

	// ���������¼
	void				OnNetChatRecordList(Player* pPlayer, BYTE byChatType);

public:

	VOID				ChangeLocalChar(char* pszSrc, char* pszDst, size_t nDstSize);

	// ���͹㲥��Ϣ
	VOID				SendBroadcastMsg(DWORD dwBroadCastId, DWORD dwValue1, const char *pszText1, 
		DWORD dwValue2 = 0, const char *pszText2 = NULL, DWORD dwValue3 = 0, const char *pszText3 = NULL);

	// ���͹㲥��Ϣ
	VOID				SendBroadcastMsg(DWORD dwBroadCastId, const char *pszText = NULL);

protected:

	// ����Ƶ��
	int					OnWorldChat(Player* pPlayer, NetMsgBody *pMsg);

	// ˽��
	int					OnP2PChat(Player* pPlayer, NetMsgBody *pMsg, const char *pszTargetName);

	// ����
	int					OnGuildChat(Player* pPlayer, NetMsgBody *pMsg);

	// ����
	int					OnTeamChat(Player* pPlayer, NetMsgBody *pMsg);

	// ��Ӵ���
	int					OnTeamHallChat(Player* pPlayer, NetMsgBody *pMsg);

protected:
	// �������
	void				AddVoice(DWORD dwVoiceId, const char *pszVoice);

	// ȡ��������
	ChatVoice*		    GetVoice(DWORD dwVoiceId);

	// �������Ƶ�������¼
	void				AddWorldChatRecord(DWORD dwServerId, const ChatRecord &tChatRecord);

	// ��Ӵ��������¼
	void				AddTeamHallChatRecord(const ChatRecord &tChatRecord);

	// ȡһ���µ�ϵ��ID
	DWORD				GetNewSeriesId();

	// ���������������
	bool				CheckWorldChat(Player* pPlayer);

private:
	//ChatEntry*			m_pChatEntry;
	//DWORD				m_dwProcessBroadcastTimer;

	// ��ʱ����ʹ��
	ChatRecord			m_tChatRecord;

	// �����б�
	map<DWORD, ChatVoice> m_VoiceList;

	// ����Ƶ�������¼�б�
	//vector<ChatRecord> ;
	map<DWORD, list<ChatRecord>> m_ChatRecordList;

	// ��Ӵ��������¼�б� todo::��ʱ������,�ʺ�˵���ô���... 2017-10-16-23:59:58
	list<ChatRecord> m_TeamHallChatRecordList;

	// ϵ��ID
	DWORD m_dwSeriesId;		
};