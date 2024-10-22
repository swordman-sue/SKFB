#pragma once
#include "../ToolKit/Singleton.h"
#include "../ToolKit/MemoryPool.h"
#include "../Common/DBCDefine.h"
#include "../Common/Const.h"

class Player;
//�ʼ�ϵͳ��
class EmailSystem: public Singleton<EmailSystem>
{
public:
	EmailSystem();
	~EmailSystem();
	//�����ʼ�����
	SendEmailEntry* GetSendEmailEntry(DWORD dwID);
	EmailEntry*     GetEmailEntry(DWORD dwID);
	void            GetEmailAttachmentInfo(EmailEntry* pInfo,EmailInfo& Info);
	
	// ϵͳ�����Ե��ã��������ʼ���������ɾ��
	void            DeleteEmailByConditionSyn(DWORD dwCurrTime);
	
	// ���·����ѷ���
	// GM SendEmail2Player GM or SystemUpdate SendEmilByCondition ���߾���ע����
	void            InsertEmailSyn(MAP_QUERY_INDEX nIdx,EmailInfo& Info,char* pInfo);
	//����������� ���䷢��Email������GM�ڲ���
	void            SendEmail2Player(DWORD dwID,char* pPlayerName);
private:
};