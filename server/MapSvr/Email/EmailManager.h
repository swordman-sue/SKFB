/**\file
    ���������
   \author Kevin
   \date 2015/8/4
   \note �ʼ�����: ��¼(�����ݿ�����ʼ�����)--->��Ϸ��(������������)--->����(�洢���ݵ����ݿ�)  
*/

#pragma once
#include <map>
#include "../Common/DataDefine.h"

class Player;
typedef std::map<DWORD,EmailInfo> EMAIL_MAP;
typedef std::map<DWORD,EmailInfo>::iterator EMAIL_MAP_ITR;

//�ʼ���
class EmailManager
{
public:
	EmailManager();
	~EmailManager();

	// ��ʼ��
	bool Init(Player* pPlayer);
		
public:

	// �����ʼ��б�
	VOID OnNetEmailList();

public:
	// �����ʼ��б�
	void SendEmailList();

	// ����ʼ�
	bool AddEmail(EmailInfo& tInfo, bool bIsSend = false);

	// ����ʼ�
	static bool AddEmail(DWORD dwRoleId, DWORD dwInfoId, EmailContext *pContextList, WORD wContextNum, bool bIsSend = false);

	// ����ʼ�
	static bool AddEmail(DWORD dwRoleId, EmailInfo &tEmailInfo, bool bIsSend);

	// ����ʼ�
	static bool AddEmailByGM(DWORD dwRoleId, EmailInfo &tEmailInfo);

	// ȡһ����Ч���ʼ�ID
	static DWORD GetEmailId();

	// ɾ�������ʼ�
	void DeleteTimeOutEmail();

	//�������ʼ����浽����
	void  SaveAllEmailIntoDB();

	// �����ʼ������ӿ�(��δʹ��)
	//void  UpdateEmailSyn(EmailInfo& Info);

	// �Ƿ��к��
	bool IsHaveRedPoint();

protected:
	EmailInfo* GetEmailInfo(DWORD dwEmailId);

private:
	static DWORD m_dwSerialId;	 // ����ID
	Player* m_pPlayer;

	// �ʼ�����
	PlayerEmailInfo *m_pEmailInfo;
};
