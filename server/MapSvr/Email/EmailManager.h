/**\file
    工会管理类
   \author Kevin
   \date 2015/8/4
   \note 邮件流程: 登录(从数据库加载邮件数据)--->游戏中(操作缓存数据)--->下线(存储数据到数据库)  
*/

#pragma once
#include <map>
#include "../Common/DataDefine.h"

class Player;
typedef std::map<DWORD,EmailInfo> EMAIL_MAP;
typedef std::map<DWORD,EmailInfo>::iterator EMAIL_MAP_ITR;

//邮件类
class EmailManager
{
public:
	EmailManager();
	~EmailManager();

	// 初始化
	bool Init(Player* pPlayer);
		
public:

	// 请求邮件列表
	VOID OnNetEmailList();

public:
	// 发送邮件列表
	void SendEmailList();

	// 添加邮件
	bool AddEmail(EmailInfo& tInfo, bool bIsSend = false);

	// 添加邮件
	static bool AddEmail(DWORD dwRoleId, DWORD dwInfoId, EmailContext *pContextList, WORD wContextNum, bool bIsSend = false);

	// 添加邮件
	static bool AddEmail(DWORD dwRoleId, EmailInfo &tEmailInfo, bool bIsSend);

	// 添加邮件
	static bool AddEmailByGM(DWORD dwRoleId, EmailInfo &tEmailInfo);

	// 取一个有效的邮件ID
	static DWORD GetEmailId();

	// 删除过期邮件
	void DeleteTimeOutEmail();

	//将所有邮件保存到数据
	void  SaveAllEmailIntoDB();

	// 请求邮件操作接口(暂未使用)
	//void  UpdateEmailSyn(EmailInfo& Info);

	// 是否有红点
	bool IsHaveRedPoint();

protected:
	EmailInfo* GetEmailInfo(DWORD dwEmailId);

private:
	static DWORD m_dwSerialId;	 // 序列ID
	Player* m_pPlayer;

	// 邮件数据
	PlayerEmailInfo *m_pEmailInfo;
};
