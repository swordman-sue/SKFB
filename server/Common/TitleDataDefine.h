#ifndef _TITLE_DATA_DEFINE_H_
#define _TITLE_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// 称号数据
 struct TitleData
 {
	 int nTitleId;		// 称号ID
	 int nOverTime;		// 结束时间(0=永久有效)
 };
 
 // 玩家称号信息
 struct PlayerTitleInfo
 {
	 int nCurUseTitleId;	// 当前使用的称号
	 int nCurUseNicknameId;	// 当前使用的昵称

	 // 称号列表
	 TitleData arTitleList[MAX_TITAL_NUM];
	 WORD wTitleNum;	
 };

#pragma pack(pop)
#endif
