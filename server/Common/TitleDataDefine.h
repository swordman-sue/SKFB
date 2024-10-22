#ifndef _TITLE_DATA_DEFINE_H_
#define _TITLE_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// �ƺ�����
 struct TitleData
 {
	 int nTitleId;		// �ƺ�ID
	 int nOverTime;		// ����ʱ��(0=������Ч)
 };
 
 // ��ҳƺ���Ϣ
 struct PlayerTitleInfo
 {
	 int nCurUseTitleId;	// ��ǰʹ�õĳƺ�
	 int nCurUseNicknameId;	// ��ǰʹ�õ��ǳ�

	 // �ƺ��б�
	 TitleData arTitleList[MAX_TITAL_NUM];
	 WORD wTitleNum;	
 };

#pragma pack(pop)
#endif
