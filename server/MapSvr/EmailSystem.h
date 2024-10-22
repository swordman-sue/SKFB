#pragma once
#include "../ToolKit/Singleton.h"
#include "../ToolKit/MemoryPool.h"
#include "../Common/DBCDefine.h"
#include "../Common/Const.h"

class Player;
//邮件系统类
class EmailSystem: public Singleton<EmailSystem>
{
public:
	EmailSystem();
	~EmailSystem();
	//发送邮件条件
	SendEmailEntry* GetSendEmailEntry(DWORD dwID);
	EmailEntry*     GetEmailEntry(DWORD dwID);
	void            GetEmailAttachmentInfo(EmailEntry* pInfo,EmailInfo& Info);
	
	// 系统周期性调用，将过期邮件从数据中删除
	void            DeleteEmailByConditionSyn(DWORD dwCurrTime);
	
	// 以下方法已废弃
	// GM SendEmail2Player GM or SystemUpdate SendEmilByCondition 两者均被注释了
	void            InsertEmailSyn(MAP_QUERY_INDEX nIdx,EmailInfo& Info,char* pInfo);
	//根据玩家名字 给其发送Email，都是GM在操作
	void            SendEmail2Player(DWORD dwID,char* pPlayerName);
private:
};