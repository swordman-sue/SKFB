#include "StdAfx.h"
#include "ConfigManager.h"
#include "..\ToolKit\DBCFileManager.h"
#include "..\ToolKit\DBCFile.h"
#include "FileEx\TsFile.h"
#include "CommonLogic.h"

// 读取Item
bool ReadItem(const char *pszSrc, const char *pszKey, Item &tItem)
{
	vector<DWORD> vList;
	if (!ReadArry1(pszSrc, pszKey, vList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]读取物品数据失败[Key:%s]!",__FUNCTION__, pszKey);
		return false;
	}

	if (vList.size() == 2)
	{
		tItem.dwId = vList[0];
		tItem.dwNum = vList[1];
	}
	else
	{
		return false;
	}

	return true;
}

// 读取uint
DWORD ReadUInt(const char *pszSrc, const char *pszKey, int nDefault)
{
	if (!pszKey || !pszSrc) return 0;
	string strRealKey = pszKey;
	strRealKey += " =";
	const char *pszResult = strstr(pszSrc, pszKey);
	return pszResult ? atol(pszResult + strlen(pszKey)) : nDefault;
}

// 读取int
int ReadInt(const char *pszSrc, const char *pszKey, int nDefault)
{
	if (!pszKey || !pszSrc) return 0;
	const char *pszResult = strstr(pszSrc, pszKey);
	return pszResult ? atoi(pszResult + strlen(pszKey)) : nDefault;
}

// 读取double
double ReadDouble(const char *pszSrc, const char *pszKey, double fDefault)
{
	if (!pszKey || !pszSrc) return 0;
	const char *pszResult = strstr(pszSrc, pszKey);
	return pszResult ? atof(pszResult + strlen(pszKey)) : fDefault;
}

// 读取配对数据
bool ReadPair(const char *pszSrc, const char *pszKey, DWORD &dwValue1, DWORD &dwValue2)
{
	vector<DWORD> vValueList;
	ReadArry1(pszSrc, pszKey, vValueList);
	if (vValueList.size() != 2)
	{
		MAPLOG(ERROR_LEVEL, "匹配对数据配置出错[%s]!", pszKey);
		return false;
	}

	dwValue1 = vValueList[0];
	dwValue2 = vValueList[1];
	
	return true;
}

// 读取字符串
string ReadString(const char *pszSrc, const char *pszKey, const char *pszDefault)
{
	if (!pszKey || !pszSrc) return NULL;
	const char *pszBegin = strstr(pszSrc, pszKey);
	const char *pszEnd = pszBegin ? strstr(pszBegin, "\r\n") : NULL;
	if (pszBegin && pszEnd)
	{
		const char *pszValue = pszBegin + strlen(pszKey);
		return string(pszValue, pszEnd - pszValue);
	}
	return pszDefault;
}

// 读取一维数组
bool ReadArry1(const char *pszSrc, const char *pszKey, vector<DWORD> &list)
{
	list.clear();
	if (!pszKey || !pszSrc)
		return false;

	char szKey[128] = { 0 };
	if (strstr(pszKey, "="))
		strcpy_s(szKey, sizeof(szKey)-1, pszKey);
	else
		sprintf_s(szKey, sizeof(szKey)-1, "%s =", pszKey);

	const char *pszResult = strstr(pszSrc, szKey);
	if (!pszResult)
		return false;

	const char *pszBegin = strstr(pszResult, "[");
	const char *pszEnd = strstr(pszResult, "]");
	if (pszBegin && pszEnd)
	{
		string strValue(pszBegin + 1, pszEnd - pszBegin);
		int nNum = GetStringSpacingNum(strValue.c_str(), ',');
		if (nNum > 0)
		{
			for (int i = 0; i <= nNum; ++i)
			{
				int nValue = CPXValue(strValue.c_str(), i, ',');
				list.push_back(nValue);
			}
		}
		else if (strValue.size())
		{
			list.push_back(atoi(strValue.c_str()));
		}
	}
	return true;
}

// 读取二维数组
bool ReadArry2(const char *pszSrc, const char *pszKey, vector<vector<DWORD>> &list)
{
	list.clear();
	if (!pszKey || !pszSrc)
		return false;

	const char *pszResult = strstr(pszSrc, pszKey);
	if (!pszResult)
		return false;

	const char *pszBegin = strstr(pszResult, "[");
	const char *pszEnd = strstr(pszResult, "]");
	if (pszBegin && pszEnd)
	{
		// 解析一维数据(todo::要记录做调整)
		vector<string> list1;
		string strValue(pszBegin + 1, pszEnd - pszBegin - 1);
		int nNum = GetStringSpacingNum(strValue.c_str(), '{');
		const char *pTemp = strValue.c_str();
		if (nNum > 0)
		{
			for (int i = 0; i < nNum; ++i)
			{
				pszBegin = strstr(pTemp, "{");
				pszEnd = strstr(pTemp, "}");
				string strTmp(pszBegin + 1, pszEnd - pszBegin - 1);
				//string strTmp = CPXValue(strValue.c_str(), 2*i+1, ',');
				//strTmp = strTmp.substr(1, strTmp.size() - 2);
				list1.push_back(strTmp);
				pTemp = pszEnd + 1;
			}
		}
// 		else if (strValue.size())
// 		{
// 			list1.push_back(strValue.c_str());
// 		}

		// 解析二维数据
		vector<string>::iterator iter = list1.begin();
		for (; iter != list1.end(); ++iter)
		{
			vector<DWORD> list2;
			int nNum = GetStringSpacingNum(iter->c_str(), ',');
			if (nNum > 0)
			{
				for (int i = 0; i <= nNum; ++i)
				{
					int nValue = CPXValue(iter->c_str(), i, ',');
					list2.push_back(nValue);
				}
			}
			else if (iter->size())
			{
				list2.push_back(atoi(iter->c_str()));
			}

			list.push_back(list2);
		}
	}
	return true;
}

// 加载通用消耗配置逻辑
bool LoadCommonCost(const char *pszSrc, vector<CostData> &cost_list, const char *pszTypeKey, const char *pszValueKey)
{
	cost_list.clear();
	vector<DWORD> cost_type_list;
	vector<DWORD> cost_value_list;
	ReadArry1(pszSrc, pszTypeKey, cost_type_list);
	ReadArry1(pszSrc, pszValueKey, cost_value_list);

	if (cost_type_list.size() != cost_value_list.size())
		return false;

	CostData tData;
	for (size_t i = 0; i < cost_type_list.size(); ++i)
	{
		tData.wType = cost_type_list[i];
		tData.nValue = cost_value_list[i];
		cost_list.push_back(tData);
	}

	return true;
}

// 读取通用物品列表
bool ReadCommonItemList(const char *pszSrc, const char *pszIdKey, const char *pszNumKey,  vector<Item> &item_list)
{
	item_list.clear();
	vector<DWORD> item_id_list;
	vector<DWORD> item_num_list;
	ReadArry1(pszSrc, pszIdKey, item_id_list);
	ReadArry1(pszSrc, pszNumKey, item_num_list);

	if (item_id_list.size() != item_num_list.size())
		return false;

	for (size_t i = 0; i < item_id_list.size(); ++i)
	{
		item_list.push_back(Item(item_id_list[i], item_num_list[i]));
	}

	return true;
}


// 读取通用属性列表
bool ReadCommonPropertyList(const char *pszSrc, const char *pszTypeKey, const char *pszValueKey, vector<Property> &sOutProList)
{
	sOutProList.clear();
	vector<DWORD> pro_type_list;
	vector<DWORD> pro_value_list;
	ReadArry1(pszSrc, pszTypeKey, pro_type_list);
	ReadArry1(pszSrc, pszValueKey, pro_value_list);

	if (pro_type_list.size() != pro_value_list.size())
	{
		MAPLOG(ERROR_LEVEL, "配置的属性不匹配!");
		return false;
	}

	Property pro;
	for (int k = 0; k < pro_type_list.size(); ++k)
	{
		pro.wType = pro_type_list[k];
		pro.nValue = pro_value_list[k];
		sOutProList.push_back(pro);
	}

	return true;
}


DWORD g_GetConfigValue(DWORD dwKey, DWORD dwDefault)
{
	return CConfigManager::Instance()->GetServerCfg(dwKey, dwDefault);
}


bool TestCallback(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &record = data_list[i];
	}

	return true;
}

// 加载通用配置
bool LoadCommon(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	Server_Config sConfig;
	vector<DWORD> value_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.dwKey = ReadUInt(tmp.data(), "id = ");
		sConfig.dwValue = ReadUInt(tmp.data(), "int_value = ");
		sConfig.strValue = ReadString(tmp.data(), "string_value = ");
		ReadArry1(tmp.c_str(), "value_list =", sConfig.vValueList);
		CConfigManager::Instance()->m_CommonList.insert(make_pair(sConfig.dwKey, sConfig));
	}

	return true;
}

// 加载服务器信息配置
bool LoadServerInfo(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ServerInfo_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwInitServerId = ReadUInt(tmp.data(), "init_server_id = ");
		tConfig.dwCurServerId = ReadUInt(tmp.data(), "cur_server_id = ");
		ReadArry1(tmp.c_str(), "server_list =", tConfig.vServerRecordList);
		CConfigManager::Instance()->m_ServerInfoList.insert(make_pair(tConfig.dwInitServerId, tConfig));
	}

	return true;
}

// 添加商店商品
bool AddShopGoods(BYTE byShopType, const ShopGoodsData_Config &tGoods)
{
	DWORD dwFindId = 10000 * byShopType + tGoods.wGoodsId;
	map<DWORD, ShopGoodsData_Config> *pShopGoodsList = &CConfigManager::Instance()->m_ShopGoodshList;
	map<DWORD, ShopGoodsData_Config>::iterator iter = pShopGoodsList->find(dwFindId);
	if (iter != pShopGoodsList->end())
	{
		MAPLOG(ERROR_LEVEL, "商品ID出现重复[ShopType:%d, GoodsFindId:%d]!", byShopType, dwFindId);
		return false;
	}
	pShopGoodsList->insert(make_pair(dwFindId, tGoods));
	return true;
}


// 加载怪物配置
bool LoadMonster(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	Monster_Config monster;
	vector<DWORD> drop_id_list;
	vector<DWORD> drop_type_list;
	vector<DWORD> drop_num_list;
	vector<DWORD> drop_rate_list;
	DropData sDropData;
	for (int i = 0; i < data_list.size(); ++i)
	{
		monster.Init();
		string &tmp = data_list[i];
		monster.dwMonsterId = ReadUInt(tmp.data(), "id = ");
		monster.dwBlood = ReadUInt(tmp.data(), "blood_max = ");
		
		ReadArry1(tmp.c_str(), "drop_id_list =", drop_id_list);
		ReadArry1(tmp.c_str(), "drop_type_list =", drop_type_list);
		ReadArry1(tmp.c_str(), "drop_num_list =", drop_num_list);
		ReadArry1(tmp.c_str(), "drop_rate_list =", drop_rate_list);

		if (drop_id_list.size() != drop_type_list.size() ||
			drop_id_list.size() != drop_num_list.size() ||
			drop_id_list.size() != drop_rate_list.size())
		{
			MAPLOG(ERROR_LEVEL, "怪物掉落数据配置不匹配[monster:%d]!", monster.dwMonsterId);
			return false;
		}

		for (int k = 0; k < drop_type_list.size(); ++k)
		{
			sDropData.dwDropObjectId = drop_id_list[k];
			//sDropData.byType = drop_type_list[k];
			sDropData.wDropObjectNum = drop_num_list[k];
			sDropData.wRate = drop_rate_list[k];
			monster.vDropList.push_back(sDropData);
		}

		CConfigManager::Instance()->m_MonsterList.insert(make_pair(monster.dwMonsterId, monster));
	}

	return true;
}

// 加载战役配置
bool LoadBattle(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	Battle_Config sBattle;
	DWORD dwMonsterId;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sBattle.Inti();
		string &tmp = data_list[i];
		sBattle.dwBattleId = ReadUInt(tmp.data(), "id =");
		dwMonsterId = ReadUInt(tmp.data(), "moster_1 =");
		if (dwMonsterId)
			sBattle.arMonsterList[0] = dwMonsterId;
			//sBattle.vMonsterList.push_back(dwMonsterId);
		dwMonsterId = ReadUInt(tmp.data(), "moster_2 =");
		if (dwMonsterId)
			sBattle.arMonsterList[1] = dwMonsterId;
			//sBattle.vMonsterList.push_back(dwMonsterId);
		dwMonsterId = ReadUInt(tmp.data(), "moster_3 =");
		if (dwMonsterId)
			sBattle.arMonsterList[2] = dwMonsterId;
			//sBattle.vMonsterList.push_back(dwMonsterId);
		dwMonsterId = ReadUInt(tmp.data(), "moster_4 =");
		if (dwMonsterId)
			sBattle.arMonsterList[3] = dwMonsterId;
			//sBattle.vMonsterList.push_back(dwMonsterId);
		dwMonsterId = ReadUInt(tmp.data(), "moster_5 =");
		if (dwMonsterId)
			sBattle.arMonsterList[4] = dwMonsterId;
			//sBattle.vMonsterList.push_back(dwMonsterId);

		CConfigManager::Instance()->m_BattleList.insert(make_pair(sBattle.dwBattleId, sBattle));
	}

	return true;
}

// 加载关卡配置 
bool LoadMap(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vDropIdList;
	vector<DWORD> vDropNumList;
	vector<DWORD> vDropRateList;

	vector<DWORD> obj_id_list;
	vector<DWORD> obj_num_min_list;
	vector<DWORD> obj_num_max_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		Map_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwMapId					= ReadUInt(tmp.data(), "id =");
		sConfig.byDailyBattleTimesLimit = ReadUInt(tmp.data(), "battle_times_max =");
		sConfig.byIsCanComment = ReadUInt(tmp.data(), "is_can_comment =");
		sConfig.dwNeedMinPower = ReadUInt(tmp.data(), "need_min_power =");
		ReadArry1(tmp.c_str(), "battle_list =", sConfig.vBattleList);

		// 宝箱奖励列表
		if (!ReadCommonItemList(tmp.data(), "box_id_list =", "box_num_list =", sConfig.vBoxList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]关卡宝箱奖励ID与数量不匹配[MapId:%d]!", __FUNCTION__, sConfig.dwMapId);
			return false;
		}
		// 物品奖励
// 		if (!ReadCommonItemList(tmp.data(), "obj_id_list =", "obj_num_list =", sConfig.vRewardItemList))
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]关卡奖励物品的ID与数量不匹配[MapId:%d]!", __FUNCTION__, sConfig.dwMapId);
// 			return false;
// 		}
		// 物品奖励
		ReadArry1(tmp.c_str(), "reward_item_id_list =", obj_id_list);
		ReadArry1(tmp.c_str(), "reward_item_num_min_list =", obj_num_min_list);
		ReadArry1(tmp.c_str(), "reward_item_num_max_list =", obj_num_max_list);
		if ((obj_id_list.size() != obj_num_min_list.size()) || (obj_id_list.size() != obj_num_max_list.size()))
		{
			MAPLOG(ERROR_LEVEL, "[%s]关卡奖励物品配置不匹配[MapId:%d]!", __FUNCTION__, sConfig.dwMapId);
			return false;
		}
		RewardItem tRewardData;
		for (size_t k = 0; k < obj_id_list.size(); ++k)
		{
			tRewardData.dwItemId = obj_id_list[k];
			tRewardData.wNumMin = obj_num_min_list[k];
			tRewardData.wNumMax = obj_num_max_list[k];
			sConfig.vRandRewardList.push_back(tRewardData);
		}

		ReadArry1(tmp.c_str(), "drop_obj_id_list =", vDropIdList);
		ReadArry1(tmp.c_str(), "drop_obj_num_list =", vDropNumList);
		ReadArry1(tmp.c_str(), "drop_obj_rate_list =", vDropRateList);
		if ((vDropIdList.size() != vDropNumList.size()) || (vDropIdList.size() != vDropRateList.size()))
		{
			MAPLOG(ERROR_LEVEL, "[%s]关卡掉落配置不匹配[MapId:%d]!", __FUNCTION__, sConfig.dwMapId);
			return false;
		}
		DropData tDropData;
		for (size_t k = 0; k < vDropIdList.size(); ++k)
		{
			tDropData.dwDropObjectId = vDropIdList[k];
			tDropData.wDropObjectNum = vDropNumList[k];
			tDropData.wRate = vDropRateList[k];
			sConfig.vDropList.push_back(tDropData);
		}

		// 挑战消耗
  		if (!LoadCommonCost(tmp.data(), sConfig.vCostList))
  		{
  			MAPLOG(ERROR_LEVEL, "[%s]关卡消耗配置不匹配[MapId:%d]!", __FUNCTION__, sConfig.dwMapId);
  			return false;
  		}
		// 首胜物品奖励
		if (!ReadCommonItemList(tmp.data(), "first_win_item_id_list =", "first_win_item_num_list =", sConfig.vFirstWinItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]关卡首胜奖励物品配置不匹配[MapId:%d]!", __FUNCTION__, sConfig.dwMapId);
			return false;
		}
  		// 首胜货币奖励
		if (!ReadCommonPropertyList(tmp.data(), "first_reward_type_list =", "first_reward_value_list =", sConfig.vFirstWinCurrencyList))
  		{
  			MAPLOG(ERROR_LEVEL, "[%s]关卡首胜奖励配置不匹配[MapId:%d]!", __FUNCTION__, sConfig.dwMapId);
  			return false;
  		}
  		// 普通奖励
  		if (!ReadCommonPropertyList(tmp.data(), "reward_currency_type_list =", "reward_currency_value_list =", sConfig.vNormalRewardList))
  		{
  			MAPLOG(ERROR_LEVEL, "[%s]关卡普通奖励配置不匹配[MapId:%d]!", __FUNCTION__, sConfig.dwMapId);
  			return false;
  		}
		
		CConfigManager::Instance()->m_MapList.insert(make_pair(sConfig.dwMapId, sConfig));
	}

	return true;
}

// 加载关卡配置
bool LoadMainFuben(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{	
	vector<DWORD> map_list;
	vector<DWORD> star_list;
	vector<DWORD> box_list;
	StarAwardData sStarAward;
	for (size_t i = 0; i < data_list.size(); ++i)
	{
		FubenChapter_Config sConfig;
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.dwChapter = ReadUInt(tmp.data(), "id =");
		sConfig.dwNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");

		ReadArry1(tmp.c_str(), "map_list =", map_list);
		ReadArry1(tmp.c_str(), "star_list =", star_list);
		//ReadArry1(tmp.c_str(), "box_list =", box_list);

		if (star_list.size() != 3)
		{
			MAPLOG(ERROR_LEVEL, "主线副本星数奖励配置不为3个[chapter:%d]!", sConfig.dwChapter);
			return false;
		}

		sConfig.vMapList = map_list;
		char szBoxIdKey[64];
		char szBoxNumKey[64];
		for (size_t k = 0; k < star_list.size(); ++k)
		{
			sStarAward.byStar = star_list[k];
			memset(szBoxIdKey, 0, sizeof(szBoxIdKey));
			memset(szBoxNumKey, 0, sizeof(szBoxNumKey));
			sprintf_s(szBoxIdKey, sizeof(szBoxNumKey)-1, "box_%d_id_list", k+1);
			sprintf_s(szBoxNumKey, sizeof(szBoxNumKey)-1, "box_%d_num_list", k + 1);
			if (!ReadCommonItemList(tmp.data(), szBoxIdKey, szBoxNumKey, sStarAward.vBoxList))
			{
				MAPLOG(ERROR_LEVEL, "[%s]主线副本章节星数奖励不匹配[Chapter:%d]!", __FUNCTION__, sConfig.dwChapter);
				return false;
			}
			sConfig.vStarAwardList.push_back(sStarAward);
		}

		CConfigManager::Instance()->m_MainFBChapterList.insert(make_pair(sConfig.dwChapter, sConfig));
	}
	return true;
}

// 加载精英副本配置
bool LoadEliteFB(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> map_list;
	vector<DWORD> star_list;
	//vector<DWORD> box_list;
	StarAwardData sStarAward;
	for (int i = 0; i < data_list.size(); ++i)
	{
		FubenChapter_Config tConfig;
		tConfig.Init();
		string &tmp = data_list[i];
		tConfig.dwChapter = ReadUInt(tmp.data(), "id =");

		ReadArry1(tmp.c_str(), "map_list =", map_list);
		ReadArry1(tmp.c_str(), "star_list =", star_list);
		if (star_list.size() != 3)
		{
			MAPLOG(ERROR_LEVEL, "精英副本星数奖励个数据不为3[chapter:%d]!", tConfig.dwChapter);
			return false;
		}

		tConfig.vMapList = map_list;
		char szBoxIdKey[BUFF_LEN_64];
		char szBoxNumKey[BUFF_LEN_64];
		for (int k = 0; k < star_list.size(); ++k)
		{
			sStarAward.byStar = star_list[k];
			memset(szBoxIdKey, 0, sizeof(szBoxIdKey));
			memset(szBoxNumKey, 0, sizeof(szBoxNumKey));
			sprintf_s(szBoxIdKey, sizeof(szBoxNumKey)-1, "box_%d_id_list", k + 1);
			sprintf_s(szBoxNumKey, sizeof(szBoxNumKey)-1, "box_%d_num_list", k + 1);
			if (!ReadCommonItemList(tmp.data(), szBoxIdKey, szBoxNumKey, sStarAward.vBoxList))
			{
				MAPLOG(ERROR_LEVEL, "[%s]精英副本章节星数奖励不匹配[Chapter:%d]!", __FUNCTION__, tConfig.dwChapter);
				return false;
			}
			tConfig.vStarAwardList.push_back(sStarAward);
		}

		ReadArry1(tmp.c_str(), "enemy_map_list =", tConfig.vEnemyMapList);
		ReadArry1(tmp.c_str(), "enemy_map_rate_list =", tConfig.vEnemyMapRateList);
		if (tConfig.vEnemyMapList.size() != tConfig.vEnemyMapRateList.size())
		{
			MAPLOG(ERROR_LEVEL, "精英副本外敌关卡列表配置不匹配[chapter:%d]!", tConfig.dwChapter);
			return false;
		}

		tConfig.wEnemyRandNumMin = ReadUInt(tmp.data(), "enemy_rand_num_min =");
		tConfig.wEnemyRandNumMax = ReadUInt(tmp.data(), "enemy_rand_num_max =");
		vector<DWORD> vEnemyRankChapterList;
		ReadArry1(tmp.c_str(), "enemy_rand_chapter_list =", vEnemyRankChapterList);
		if (vEnemyRankChapterList.size() != 2)
		{
			MAPLOG(ERROR_LEVEL, "精英副本外敌随机章节列表配置不匹配[chapter:%d]!", tConfig.dwChapter);
			return false;
		}
		tConfig.wEnemyRandChapterMin = vEnemyRankChapterList[0];
		tConfig.wEnemyRandChapterMax = vEnemyRankChapterList[1];

		CConfigManager::Instance()->m_EliteFBChapterList.insert(make_pair(tConfig.dwChapter, tConfig));
	}
	return true;
}


// 加载日常副本伤害奖励配置
bool LoadDailyFBDamageReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		DailyFBDamageReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wChapter = ReadUInt(tmp.data(), "chapter =");
		tConfig.wMapIdx = ReadUInt(tmp.data(), "map_idx =");
		tConfig.wContext = ReadUInt(tmp.data(), "context =");
		if (ReadUInt(tmp.data(), "is_sweep_reward ="))
		{
			map<DWORD, FubenChapter_Config>::iterator iter = CConfigManager::Instance()->m_DailyChapterList.find(tConfig.wChapter);
			if (iter != CConfigManager::Instance()->m_DailyChapterList.end())
			{
				iter->second.dwSweepRewardContext = tConfig.wContext;
			}
		}
		if (!ReadCommonItemList(tmp.data(), "item_id_list", "item_num_list", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]日常副本奖励配置出错[chapter:%d,max_idx,context:%d]!", __FUNCTION__,
				tConfig.wChapter, tConfig.wMapIdx, tConfig.wContext);
			return false;
		}
		DWORD dwFindId = 1000000 * tConfig.wChapter + 10000 * tConfig.wMapIdx + tConfig.wContext;
		CConfigManager::Instance()->m_DailyFBDamageReward.insert(make_pair(dwFindId, tConfig));
	}
	return true;
}

// 加载主线副本章节奖励
bool LoadMainFBChapterReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		MainFBChapterReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wLevel = ReadUInt(tmp.data(), "level =");
		tConfig.wNeedChapter = ReadUInt(tmp.data(), "need_chapter =");
		tConfig.dwItemId = ReadUInt(tmp.data(), "item_id =");
		CConfigManager::Instance()->m_MainFBChapterRewardList.insert(make_pair(tConfig.wLevel, tConfig));
	}
	return true;
}

// 加载限时掉落物品
bool LoadLimitDrop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, vector<LimitDrop_Config>> &vLimitDropList = CConfigManager::Instance()->m_vLimitDropList;

	for (int i = 0; i < data_list.size(); ++i)
	{
		LimitDrop_Config tConfig;
		string &tmp = data_list[i];
		DWORD dwDropType = ReadUInt(tmp.data(), "type =");
		tConfig.dwDropItemId = ReadUInt(tmp.data(), "drop_item_id =");
		tConfig.dwStartDate = ReadUInt(tmp.data(), "star_date =");
		tConfig.dwOverDate = ReadUInt(tmp.data(), "over_date =");
		tConfig.dwDropRate = ReadUInt(tmp.data(), "drop_rate =");
		tConfig.dwDropNumMin = ReadUInt(tmp.data(), "drop_num_min =");
		tConfig.dwDropNumMax = ReadUInt(tmp.data(), "drop_num_max =");

		map<DWORD, vector<LimitDrop_Config>>::iterator iter = vLimitDropList.find(dwDropType);
		if (iter != vLimitDropList.end())
		{
			iter->second.push_back(tConfig);
		}
		else
		{
			vector<LimitDrop_Config> vList;
			vList.push_back(tConfig);
			vLimitDropList.insert(make_pair(dwDropType, vList));
		}
	}
	return true;
}




// 加载物品配置
bool LoadItem(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> item_id_list;
	vector<DWORD> item_min_num_list;
	vector<DWORD> item_max_num_list;
	vector<DWORD> item_rate_list;
	Item_Config item;
	GiftRandItem sGiftItem;
	for (int i = 0; i < data_list.size(); ++i)
	{
		item.Init();
		string &tmp = data_list[i];
		item.dwId				= ReadUInt(tmp.data(), "id =");
		item.byType				= ReadUInt(tmp.data(), "type =");
		item.dwPrice			= ReadUInt(tmp.data(), "price =");
		item.dwSellPrice		= ReadUInt(tmp.data(), "sell_price =");
		item.byIsBroadcasting	= ReadUInt(tmp.data(), "is_broadcasting =");
		item.byIsUseInGet		= ReadUInt(tmp.data(), "is_use_in_get =");
		item.wMaxNum			= ReadUInt(tmp.data(), "max_num =");
		item.wUseLevel			= ReadUInt(tmp.data(), "use_level =");
		item.nParam1			= ReadUInt(tmp.data(), "param1 =");
		item.nParam2			= ReadUInt(tmp.data(), "param2 =");
		item.byIsCanUse			= ReadUInt(tmp.data(), "is_can_use =");
		item.bySubType			= ReadUInt(tmp.data(), "sub_type =");
		item.dwExpiryDate		= ReadUInt(tmp.data(), "expiry_date =");
		

		memset(&sGiftItem, 0, sizeof(sGiftItem));
		ReadArry1(tmp.c_str(), "item_id_list =", item_id_list);
		ReadArry1(tmp.c_str(), "item_min_num_list =", item_min_num_list);
		ReadArry1(tmp.c_str(), "item_max_num_list =", item_max_num_list);
		ReadArry1(tmp.c_str(), "item_rate_list =", item_rate_list);
		for (int k = 0; k < item_id_list.size(); ++k)
		{
			sGiftItem.dwItemId = item_id_list[k];

			if (item_min_num_list.size() > k)
				sGiftItem.wNumMin = item_min_num_list[k];

			if (item_max_num_list.size() > k)
				sGiftItem.wNumMax = item_max_num_list[k];

			if (item_rate_list.size() > k)
				sGiftItem.wRate = item_rate_list[k];

			item.vGiftItemList.push_back(sGiftItem);
		}
		

		CConfigManager::Instance()->m_ItemList.insert(make_pair(item.dwId, item));
	}

	return true;
}

// 加载角色等级配置
bool LoadRoleLevel(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	RoleLevel_Config sRoleLevel;
	for (int i = 0; i < data_list.size(); ++i)
	{
		memset(&sRoleLevel, 0, sizeof(sRoleLevel));
		string &tmp = data_list[i];
		sRoleLevel.wLevel = ReadUInt(tmp.data(), "level =");
		sRoleLevel.dwExp = ReadUInt(tmp.data(), "exp =");
		sRoleLevel.dwStamina = ReadUInt(tmp.data(), "stamina =");
		sRoleLevel.dwEnergy = ReadUInt(tmp.data(), "energy =");
		sRoleLevel.dwStaminaMax = ReadUInt(tmp.data(), "stamina_max =");
		sRoleLevel.dwEnergyMax = ReadUInt(tmp.data(), "energy_max =");
		sRoleLevel.dwWantedMax = ReadUInt(tmp.data(), "wanted_max =");
		sRoleLevel.dwFriendNumMax = ReadUInt(tmp.data(), "friend_num_max =");
		sRoleLevel.dwBlessSpriteNumMax = ReadUInt(tmp.data(), "bless_sprite_num_max =");
		sRoleLevel.dwDailyChatTimesMax = ReadUInt(tmp.data(), "daily_chat_times_max =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list", "item_num_list", sRoleLevel.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]角色升级奖励物品配置不匹配[level:%d]!", __FUNCTION__, sRoleLevel.wLevel);
			return false;
		}

		CConfigManager::Instance()->m_RoleLevelList.insert(make_pair(sRoleLevel.wLevel, sRoleLevel));
	}

	return true;
}


// 加载英雄配置
bool LoadHero(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	Hero_Config sHero;
	DWORD dwHeroInfoId;
	vector<vector<DWORD>> object_list;
	vector<vector<DWORD>> pro_type_list;
	vector<vector<DWORD>> pro_value_list;
	Property tProperty;
	for (int i = 0; i < data_list.size(); ++i)
	{
		memset(&sHero, 0, sizeof(sHero));
		string &tmp = data_list[i];
		dwHeroInfoId = ReadUInt(tmp.data(), "id =");
		sHero.bySex = ReadUInt(tmp.data(), "sex ="); 
		sHero.byHeroType = ReadUInt(tmp.data(), "hero_type =");	
		sHero.byProfession = ReadUInt(tmp.data(), "profession =");
		sHero.wInitLevel = ReadUInt(tmp.data(), "init_level =");
		sHero.wLimitLevel = ReadUInt(tmp.data(), "limit_level =");
		sHero.dwToExpAsCostItem = ReadUInt(tmp.data(), "to_exp =");
		sHero.dwNeedGoldAsCostItem = 100; // todo::暂时还没读取
		sHero.wInitQuality = ReadUInt(tmp.data(), "init_quality =");
		sHero.wQualityMax = ReadUInt(tmp.data(), "quality_limit =");
		sHero.wQualityStageMax = ReadUInt(tmp.data(), "quality_stage_limit =");	
		sHero.nHeroSoul = ReadUInt(tmp.data(), "hero_soul =");
		sHero.nResetCostDiamond = ReadUInt(tmp.data(), "reset_cost_diamond =");
		sHero.nPrice = ReadUInt(tmp.data(), "price =");
		sHero.nQualifications = ReadUInt(tmp.data(), "qualifications =");
		

		// 关联英雄
		{
			ReadArry2(tmp.c_str(), "relation_hero_list =", object_list);
			ReadArry2(tmp.c_str(), "relation_hero_pro_type_list =", pro_type_list);
			ReadArry2(tmp.c_str(), "relation_hero_pro_value_list =", pro_value_list);
			if ((object_list.size() != pro_type_list.size()) || (object_list.size() != pro_value_list.size()))
			{
				MAPLOG(ERROR_LEVEL, "[%s]关联英雄配置不匹配[InfoId:%d]", __FUNCTION__, dwHeroInfoId);
				continue;
			}
			RelationHeroProperty tRelationHero;
			for (size_t k = 0; k < object_list.size(); ++k)
			{
				// 初始数据
				tRelationHero.vHeroList.clear();
				tRelationHero.vProList.clear();

				tRelationHero.vHeroList = object_list[k];
				vector<DWORD> &vProTypeList = pro_type_list[k];
				vector<DWORD> &vProValueList = pro_value_list[k];
				if (vProTypeList.size() != vProValueList.size())
				{
					MAPLOG(ERROR_LEVEL, "[%s]关联英雄配置不匹配[InfoId:%d]", __FUNCTION__, dwHeroInfoId);
					continue;
				}
				for (size_t x = 0; x < vProTypeList.size(); ++x)
				{
					tProperty.wType = vProTypeList[x];
					tProperty.nValue = vProValueList[x];
					tRelationHero.vProList.push_back(tProperty);
				}
				sHero.vRelationHeroList.push_back(tRelationHero);
			}
		}

		// 关联卡牌
		{
			ReadArry2(tmp.c_str(), "relation_card_list =", object_list);
			ReadArry2(tmp.c_str(), "relation_card_pro_type_list =", pro_type_list);
			ReadArry2(tmp.c_str(), "relation_card_pro_value_list =", pro_value_list);
			if ((object_list.size() != pro_type_list.size()) || (object_list.size() != pro_value_list.size()))
			{
				MAPLOG(ERROR_LEVEL, "[%s]关联卡牌配置不匹配[InfoId:%d]", __FUNCTION__, dwHeroInfoId);
				continue;
			}
			RelationCardProperty tRelationCard;
			for (size_t k = 0; k < object_list.size(); ++k)
			{
				// 初始数据
				tRelationCard.vCardTypeList.clear();
				tRelationCard.vProList.clear();

				tRelationCard.vCardTypeList = object_list[k];
				vector<DWORD> &vProTypeList = pro_type_list[k];
				vector<DWORD> &vProValueList = pro_value_list[k];
				if (vProTypeList.size() != vProValueList.size())
				{
					MAPLOG(ERROR_LEVEL, "[%s]关联卡牌配置不匹配[InfoId:%d]", __FUNCTION__, dwHeroInfoId);
					continue;
				}
				for (size_t x = 0; x < vProTypeList.size(); ++x)
				{
					tProperty.wType = vProTypeList[x];
					tProperty.nValue = vProValueList[x];
					tRelationCard.vProList.push_back(tProperty);
				}
				sHero.vRelationCardList.push_back(tRelationCard);
			}
		}

		// 关联装备
		{
			ReadArry2(tmp.c_str(), "relation_equip_list =", object_list);
			ReadArry2(tmp.c_str(), "relation_equip_pro_type_list =", pro_type_list);
			ReadArry2(tmp.c_str(), "relation_equip_pro_value_list =", pro_value_list);
			if ((object_list.size() != pro_type_list.size()) || (object_list.size() != pro_value_list.size()))
			{
				MAPLOG(ERROR_LEVEL, "[%s]关联装备配置不匹配[InfoId:%d]", __FUNCTION__, dwHeroInfoId);
				continue;
			}
			RelationEquipmentProperty tRelationEquip;
			for (size_t k = 0; k < object_list.size(); ++k)
			{
				// 初始数据
				tRelationEquip.vEquipmentList.clear();
				tRelationEquip.vProList.clear();

				tRelationEquip.vEquipmentList = object_list[k];
				vector<DWORD> &vProTypeList = pro_type_list[k];
				vector<DWORD> &vProValueList = pro_value_list[k];
				if (vProTypeList.size() != vProValueList.size())
				{
					MAPLOG(ERROR_LEVEL, "[%s]关联装备配置不匹配[InfoId:%d]", __FUNCTION__, dwHeroInfoId);
					continue;
				}
				for (size_t x = 0; x < vProTypeList.size(); ++x)
				{
					tProperty.wType = vProTypeList[x];
					tProperty.nValue = vProValueList[x];
					tRelationEquip.vProList.push_back(tProperty);
				}
				sHero.vRelationEquipmentList.push_back(tRelationEquip);
			}
		}

		// 关联宝物
		{
			ReadArry2(tmp.c_str(), "relation_treasure_list =", object_list);
			ReadArry2(tmp.c_str(), "relation_treasure_pro_type_list =", pro_type_list);
			ReadArry2(tmp.c_str(), "relation_treasure_pro_value_list =", pro_value_list);
			if ((object_list.size() != pro_type_list.size()) || (object_list.size() != pro_value_list.size()))
			{
				MAPLOG(ERROR_LEVEL, "[%s]关联宝物配置不匹配[InfoId:%d]", __FUNCTION__, dwHeroInfoId);
				continue;
			}
			RelationTreasureProperty tRelationTreasure;
			for (size_t k = 0; k < object_list.size(); ++k)
			{
				// 初始数据
				tRelationTreasure.vTreasureList.clear();
				tRelationTreasure.vProList.clear();

				tRelationTreasure.vTreasureList = object_list[k];
				vector<DWORD> &vProTypeList = pro_type_list[k];
				vector<DWORD> &vProValueList = pro_value_list[k];
				if (vProTypeList.size() != vProValueList.size())
				{
					MAPLOG(ERROR_LEVEL, "[%s]关联宝物配置不匹配[InfoId:%d]", __FUNCTION__, dwHeroInfoId);
					continue;
				}
				for (size_t x = 0; x < vProTypeList.size(); ++x)
				{
					tProperty.wType = vProTypeList[x];
					tProperty.nValue = vProValueList[x];
					tRelationTreasure.vProList.push_back(tProperty);
				}
				sHero.vRelationTreasureList.push_back(tRelationTreasure);
			}
		}

		// 关联装备(额外)
		{
			ReadArry2(tmp.c_str(), "relation_equip_ex_list =", object_list);
			ReadArry2(tmp.c_str(), "relation_equip_pro_type_ex_list =", pro_type_list);
			ReadArry2(tmp.c_str(), "relation_equip_pro_value_ex_list =", pro_value_list);
			if ((object_list.size() != pro_type_list.size()) || (object_list.size() != pro_value_list.size()))
			{
				MAPLOG(ERROR_LEVEL, "[%s]关联装备配置不匹配(额外)[InfoId:%d]", __FUNCTION__, dwHeroInfoId);
				continue;
			}
			RelationEquipmentProperty tRelationEquip;
			for (size_t k = 0; k < object_list.size(); ++k)
			{
				// 初始数据
				tRelationEquip.vEquipmentList.clear();
				tRelationEquip.vProList.clear();

				tRelationEquip.vEquipmentList = object_list[k];
				vector<DWORD> &vProTypeList = pro_type_list[k];
				vector<DWORD> &vProValueList = pro_value_list[k];
				if (vProTypeList.size() != vProValueList.size())
				{
					MAPLOG(ERROR_LEVEL, "[%s]关联装备配置不匹配(额外)[InfoId:%d]", __FUNCTION__, dwHeroInfoId);
					continue;
				}
				for (size_t x = 0; x < vProTypeList.size(); ++x)
				{
					tProperty.wType = vProTypeList[x];
					tProperty.nValue = vProValueList[x];
					tRelationEquip.vProList.push_back(tProperty);
				}
				sHero.vRelationEquipmentListEx.push_back(tRelationEquip);
			}
		}

		// 关联宝物(额外)
		{
			ReadArry2(tmp.c_str(), "relation_treasure_ex_list =", object_list);
			ReadArry2(tmp.c_str(), "relation_treasure_pro_type_ex_list =", pro_type_list);
			ReadArry2(tmp.c_str(), "relation_treasure_pro_value_ex_list =", pro_value_list);
			if ((object_list.size() != pro_type_list.size()) || (object_list.size() != pro_value_list.size()))
			{
				MAPLOG(ERROR_LEVEL, "[%s]关联宝物配置不匹配(额外)[InfoId:%d]", __FUNCTION__, dwHeroInfoId);
				continue;
			}
			RelationTreasureProperty tRelationTreasure;
			for (size_t k = 0; k < object_list.size(); ++k)
			{
				// 初始数据
				tRelationTreasure.vTreasureList.clear();
				tRelationTreasure.vProList.clear();

				tRelationTreasure.vTreasureList = object_list[k];
				vector<DWORD> &vProTypeList = pro_type_list[k];
				vector<DWORD> &vProValueList = pro_value_list[k];
				if (vProTypeList.size() != vProValueList.size())
				{
					MAPLOG(ERROR_LEVEL, "[%s]关联宝物配置不匹配(额外)[InfoId:%d]", __FUNCTION__, dwHeroInfoId);
					continue;
				}
				for (size_t x = 0; x < vProTypeList.size(); ++x)
				{
					tProperty.wType = vProTypeList[x];
					tProperty.nValue = vProValueList[x];
					tRelationTreasure.vProList.push_back(tProperty);
				}
				sHero.vRelationTreasureListEx.push_back(tRelationTreasure);
			}
		}

		CConfigManager::Instance()->m_HeroList.insert(make_pair(dwHeroInfoId, sHero));
	}

	return true;
}

// 加载英雄属性配置
bool LoadHeroPro(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	Property  tPro;
	for (int i = 0; i < data_list.size(); ++i)
	{
		HeroPro_Config sHeroPro;
		//memset(&sHeroPro, 0, sizeof(sHeroPro));
		string &tmp = data_list[i];
		sHeroPro.byProfession		= ReadUInt(tmp.data(), "id =");
		sHeroPro.byQuality			= ReadUInt(tmp.data(), "quality =");
		sHeroPro.wLevel				= ReadUInt(tmp.data(), "level =");
		tPro.nValue = ReadInt(tmp.data(), "blood_max =");
		tPro.wType = HERO_PRO_BLOOD;
		sHeroPro.vProList.push_back(tPro);

		tPro.nValue = ReadInt(tmp.data(), "attack =");
		tPro.wType = HERO_PRO_ATTACK;
		sHeroPro.vProList.push_back(tPro);

		tPro.nValue = ReadInt(tmp.data(), "physical_defence =");
		tPro.wType = HERO_PRO_PHY_DEF;
		sHeroPro.vProList.push_back(tPro);

		tPro.nValue = ReadInt(tmp.data(), "magic_defence =");
		tPro.wType = HERO_PRO_MAG_DEF;
		sHeroPro.vProList.push_back(tPro);

		DWORD dwFindId = 100000 * sHeroPro.byProfession + 1000 * sHeroPro.byQuality + sHeroPro.wLevel;
		CConfigManager::Instance()->m_HeroProList.insert(make_pair(dwFindId, sHeroPro));
	}

	return true;
}

// 加载装备配置
bool LoadEquipment(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> strengthen_init_pro;
	for (int i = 0; i < data_list.size(); ++i)
	{
		Equipment_Config sEquipment;
		//memset(&sEquipment, 0, sizeof(sEquipment));
		string &tmp = data_list[i];
		sEquipment.dwId = ReadUInt(tmp.data(), "id =");
		sEquipment.byType = ReadUInt(tmp.data(), "item_type =");
		sEquipment.wQuality = ReadUInt(tmp.data(), "quality =");
		sEquipment.dwPrice = ReadUInt(tmp.data(), "price =");
		sEquipment.nTowerPrestige = ReadUInt(tmp.data(), "tower_prestige ="); 
		sEquipment.wStrengthLevelMax = ReadUInt(tmp.data(), "streng_level_max =");
		sEquipment.wRefineLevelMax = ReadUInt(tmp.data(), "refine_level_max =");
		sEquipment.nResetCostDiamond = ReadUInt(tmp.data(), "reset_cost_diamond =");
		sEquipment.wTalentId = ReadUInt(tmp.data(), "talent_id =");
		sEquipment.wStarMax = ReadUInt(tmp.data(), "star_max =");
		sEquipment.dwStarUpCostItemId = ReadUInt(tmp.data(), "star_up_cost_item_id =");
		sEquipment.wNextQualityEquipmentId = ReadUInt(tmp.data(), "next_quality_equipment_id =");
		ReadArry1(tmp.data(), "strengthen_init_pro =", strengthen_init_pro);
		if (strengthen_init_pro.size() != 2)
		{
			MAPLOG(ERROR_LEVEL, "装备强化初始属性配置不匹配[InfoId:%d]!", sEquipment.dwId);
			continue;
		}
		sEquipment.tStrengthenInitPro.wType = strengthen_init_pro[0];
		sEquipment.tStrengthenInitPro.nValue = strengthen_init_pro[1];
		sEquipment.tStrengthenLevelPro.wType = sEquipment.tStrengthenInitPro.wType;
		sEquipment.tStrengthenLevelPro.nValue = ReadUInt(tmp.data(), "strengthen_add_pro =");
		if (!ReadCommonPropertyList(tmp.data(), "refine_pro_type_list", "refine_pro_value_list", sEquipment.vRefineLevelProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]装备精炼属性配置不匹配[EquipmentId:%d]!", __FUNCTION__, sEquipment.dwId);
			return false;
		}
		
		CConfigManager::Instance()->m_EquipmentList.insert(make_pair(sEquipment.dwId, sEquipment));
	}

	return true;
}

// 加载装备合成配置
bool LoadEquipmentCompose(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	EquipmentCompose_Config sCompose;
	for (int i = 0; i < data_list.size(); ++i)
	{
		memset(&sCompose, 0, sizeof(sCompose));
		string &tmp = data_list[i];
		DWORD dwId			= ReadUInt(tmp.data(), "id =");
		sCompose.dwFragmentId = ReadUInt(tmp.data(), "item_id =");
		sCompose.wFragmentNum = ReadUInt(tmp.data(), "item_num =");

		CConfigManager::Instance()->m_EquipmentComposeList.insert(make_pair(dwId, sCompose));
	}

	return true;
}

// 加载装备强化配置
bool LoadEquipmentStrengthen(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		EquipmentEnhanceLevel_Config sStrength;
		string &tmp = data_list[i];
		DWORD dwQuality = ReadUInt(tmp.data(), "quality =");
		int nStrengthLevel = ReadUInt(tmp.data(), "strength_level =");
		sStrength.nCostGold = ReadUInt(tmp.data(), "cost_gold =");
		sStrength.nTotalCostGold = ReadUInt(tmp.data(), "total_cost_gold =");
		DWORD dwFindId = dwQuality * 1000 + nStrengthLevel;
		CConfigManager::Instance()->m_EquipmentEnhanceList.insert(make_pair(dwFindId, sStrength));
	}

	return true;
}

// 加载装备精练配置
bool LoadEquipmentRefine(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	EquipmentRefineLevel_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		//sConfig.Init();
		string &tmp = data_list[i];
		sConfig.byQuality = ReadUInt(tmp.data(), "quality =");
		sConfig.wLevel = ReadUInt(tmp.data(), "refine_level =");
		sConfig.dwExp = ReadUInt(tmp.data(), "exp =");
		sConfig.dwTotalExp = ReadUInt(tmp.data(), "total_exp =");
		CConfigManager::Instance()->m_EquipmentRefineList.insert(make_pair(sConfig.byQuality * 1000 + sConfig.wLevel, sConfig));
	}

	return true;
}

// 加载装备天赋配置
bool LoadEquipmentTalent(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<WORD, vector<EquipmentTalent_Config>> *pTalentList = &CConfigManager::Instance()->m_EuipmentTalentList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		EquipmentTalent_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wId = ReadUInt(tmp.data(), "id =");
		sConfig.wLevel = ReadUInt(tmp.data(), "level =");
		sConfig.wNeedEnhanceLevel = ReadUInt(tmp.data(), "need_enhance_level =");
		if (!ReadCommonPropertyList(tmp.data(), "pro_type_list", "pro_value_list", sConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]装备天赋属性配置不匹配[talent_id:%d]!", __FUNCTION__, sConfig.wId);
			return false;
		}
		map<WORD, vector<EquipmentTalent_Config>>::iterator iter = pTalentList->find(sConfig.wId);
		if (iter != pTalentList->end())
		{
			iter->second.push_back(sConfig);
		}
		else
		{
			vector<EquipmentTalent_Config> vTmpList;
			vTmpList.push_back(sConfig);
			pTalentList->insert(std::make_pair(sConfig.wId, vTmpList));
		}
	}

	return true;
}

// 加载装备升星配置
bool LoadEquipmentStarUp(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		EquipmentStarUp_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wType = ReadUInt(tmp.data(), "type =");
		tConfig.wQuality = ReadUInt(tmp.data(), "quality =");
		tConfig.wStar = ReadUInt(tmp.data(), "star =");
		tConfig.wLevel = ReadUInt(tmp.data(), "level =");
		tConfig.bIsStarUp = ReadUInt(tmp.data(), "is_star_up =");
		tConfig.tCostItem.dwId = ReadUInt(tmp.data(), "cost_item_id =");
		tConfig.tCostItem.dwNum = ReadUInt(tmp.data(), "cost_item_num =");
		tConfig.dwCostSelfNum = ReadUInt(tmp.data(), "cost_self_num =");
		tConfig.nCostGold = ReadUInt(tmp.data(), "cost_gold =");
		DWORD dwKey = tConfig.wLevel + tConfig.wStar * 100 + tConfig.wQuality * 10000 + tConfig.wType * 1000000;
		if (!ReadCommonPropertyList(tmp.data(), "pro_type_list", "pro_value_list", tConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]装备升星属性配置不匹配[key:%d]!", __FUNCTION__, dwKey);
			return false;
		}
		CConfigManager::Instance()->m_EuipmentStarUpList.insert(make_pair(dwKey, tConfig));
	}

	return true;
}

// 加载装备升品配置
bool LoadEquipmentQualityUp(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		EquipmentQualityUp_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wQuality = ReadUInt(tmp.data(), "quality =");
		tConfig.tCostItem.dwId = ReadUInt(tmp.data(), "cost_item_id =");
		tConfig.tCostItem.dwNum = ReadUInt(tmp.data(), "cost_item_num =");
		tConfig.tCostCurrency.wType = ReadUInt(tmp.data(), "cost_currency_type =");
		tConfig.tCostCurrency.nValue = ReadUInt(tmp.data(), "cost_currency_value =");
		CConfigManager::Instance()->m_EuipmentQualityUpList.insert(make_pair(tConfig.wQuality, tConfig));
	}

	return true;
}

// 加载援军助威配置
bool LoadReinforcementEncourage(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	WORD wLevelMin;
	WORD wLevelMax;
	for (int i = 0; i < data_list.size(); ++i)
	{
		ReinforcementEncourage_Config tConfig;
		string &tmp = data_list[i];
		wLevelMin = ReadUInt(tmp.data(), "reinforcement_level_min =");
		wLevelMax = ReadUInt(tmp.data(), "reinforcement_level_max =");
		tConfig.wEncourageLevel = ReadUInt(tmp.data(), "encourage_level =");
		if (!ReadCommonPropertyList(tmp.data(), "pro_type_list", "pro_value_list", tConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]援军助威属性配置不匹配[reinforcement_level:%d]!", __FUNCTION__, tConfig.wReinforcementLevel);
			return false;
		}
		for (WORD k = wLevelMin; k <= wLevelMax; ++k)
		{
			tConfig.wReinforcementLevel = k;
			CConfigManager::Instance()->m_ReinforcementEncourageList.insert(make_pair(tConfig.wReinforcementLevel, tConfig));
		}
	}

	return true;
}





// 加载商城物品
bool LoadMallGoods_Item(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> price_list;
	vector<DWORD> vip_buy_num_list;
	ShopGoodsData_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "id =");
		//sConfig.byEntityType = OBJECT_ITEM;
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		//sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.nPriceListType = ReadUInt(tmp.data(), "cost_type =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		sConfig.nVipIdx = ReadUInt(tmp.data(), "vip_idx =");	
		ReadArry1(tmp.c_str(), "price_list =", price_list);
		ReadArry1(tmp.c_str(), "vip_buy_num =", vip_buy_num_list);

		for (int k = 0; k < price_list.size(); ++k)
		{
			sConfig.vPriceList.push_back(price_list[k]);
		}
		
		AddShopGoods(SHOP_MALL_ITEM, sConfig);
	}

	return true;
}

// 加载商城礼包商品
bool LoadMallGoods_Gift(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> price_list;
	vector<DWORD> vip_buy_num_list;
	ShopGoodsData_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "id =");
		//sConfig.byEntityType = OBJECT_ITEM;
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = 1;
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		//sConfig.nCostGold = ReadUInt(tmp.data(), "price =");
		sConfig.nNeedVipLevel = ReadUInt(tmp.data(), "need_vip_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]商城礼包消耗配置不匹配!", __FUNCTION__);
			return false;
		}

		AddShopGoods(SHOP_MALL_GIFT, sConfig);
	}

	return true;
}

// 加载圣诞节积分兑换商品
bool LoadChristmasScoreExchangeGoods(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> price_list;
	vector<DWORD> vip_buy_num_list;
	ShopGoodsData_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.nContext = ReadUInt(tmp.data(), "need_history_scor =");
		sConfig.nSpecialCostValue = ReadUInt(tmp.data(), "cost_score =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		sConfig.nVipIdx = ReadUInt(tmp.data(), "vip_idx =");
		AddShopGoods(SHOP_CHRISTMAS_SCORE_EXCHANGE, sConfig);
	}

	return true;
}

// 加载圣诞节物品兑换商品
bool LoadChristmasItemExchangeGoods(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> price_list;
	vector<DWORD> vip_buy_num_list;
	ShopGoodsData_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		sConfig.nVipIdx = ReadUInt(tmp.data(), "vip_idx =");
		if (!ReadCommonItemList(tmp.data(), "cost_item_id_list", "cost_item_num_list", sConfig.vCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]圣诞节物品兑换商品消耗的道具不匹配[GoodsId:%d]!", __FUNCTION__, sConfig.wGoodsId);
			return false;
		}
		AddShopGoods(SHOP_CHRISTMAS_ITEM_EXCHANGE, sConfig);
	}

	return true;
}

// 加载英雄突破等级配置
bool LoadHeroBreakLevelUp(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		HeroBreakLevelUp_Config tConfig;
		string &tmp = data_list[i];
		DWORD dwBreakLevel = ReadUInt(tmp.data(), "break_level =");
		tConfig.wNeedHeroLevel = ReadUInt(tmp.data(), "need_hero_level =");
		tConfig.nCostGold = ReadUInt(tmp.data(), "cost_gold =");
		tConfig.dwCostItemId = ReadUInt(tmp.data(), "cost_item_id =");
		tConfig.wCostItemNum = ReadUInt(tmp.data(), "cost_item_num =");
		tConfig.dwCostHeroId = ReadUInt(tmp.data(), "cost_hero_id =");
		tConfig.wNeedHeroQuality = ReadUInt(tmp.data(), "need_hero_quality =");
		tConfig.wCostSameNameHeroNum = ReadUInt(tmp.data(), "cost_same_name_hero_num =");
		tConfig.wCostSameQualityHeroNum = ReadUInt(tmp.data(), "cost_same_quality_hero_num =");
		tConfig.wCostSameProfessionHeroNum = ReadUInt(tmp.data(), "cost_same_profession_hero_num =");
		
		CConfigManager::Instance()->m_BreakLevelUpList.insert(make_pair(dwBreakLevel, tConfig));
	}

	return true;
}

// 加载英雄合成配置
bool LoadHeroCompose(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	HeroCompose_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		DWORD dwHeroInfoId = ReadUInt(tmp.data(), "id =");
		sConfig.dwItemId = ReadUInt(tmp.data(), "item_id =");
		sConfig.wItemNum = ReadUInt(tmp.data(), "item_num =");
		CConfigManager::Instance()->m_HeroComposeList.insert(make_pair(dwHeroInfoId, sConfig));
	}

	return true;
}

// 加载英雄等级配置
bool LoadHeroLevel(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	HeroLevel_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		DWORD byQuality = ReadUInt(tmp.data(), "quality =");
		sConfig.wLevel = ReadUInt(tmp.data(), "level =");
		sConfig.dwExp = ReadUInt(tmp.data(), "exp =");
		sConfig.dwTotalExp = ReadUInt(tmp.data(), "total_exp =");
		DWORD dwFindId = 1000 * byQuality + sConfig.wLevel;
		CConfigManager::Instance()->m_HeroLevelList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}

// 加载英雄上阵配置
bool LoadHeroToBattle(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	HeroToBattle_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		sConfig.byType = ReadUInt(tmp.data(), "type =");
		sConfig.byPos = ReadUInt(tmp.data(), "pos =");
		sConfig.wNeedLevel = ReadUInt(tmp.data(), "need_level =");
		CConfigManager::Instance()->m_HeroToBattleList.insert(make_pair(sConfig.byType * 100 + sConfig.byPos, sConfig));
	}

	return true;
}

// 加载英雄培养配置
bool LoadHeroTrain(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, vector<HeroTrain_Config>> *pTrainList = &CConfigManager::Instance()->m_HeroTrainList;
	map<DWORD, vector<HeroTrain_Config>>::iterator iter;
	for (int i = 0; i < data_list.size(); ++i)
	{
		HeroTrain_Config sConfig;
		string &tmp = data_list[i];
		sConfig.byProType = ReadUInt(tmp.data(), "pro_type =");
		sConfig.byProLevel = ReadUInt(tmp.data(), "pro_level =");
		sConfig.wNeedHeroLevel = ReadUInt(tmp.data(), "need_hero_level =");
		sConfig.nProMax = ReadUInt(tmp.data(), "pro_value_max =");
		if (!ReadCommonPropertyList(tmp.data(), "level_up_pro_type_list =", "level_up_pro_value_list =", sConfig.vLevelUpProList))
		{
			MAPLOG(ERROR_LEVEL, "英雄培养配置的属性不匹配[pro_type:%d,pro_level:%d]!", sConfig.byProType, sConfig.byProLevel);
			return false;
		}

		iter = pTrainList->find(sConfig.byProType);
		if (iter != pTrainList->end())
		{
			iter->second.push_back(sConfig);
		}
		else
		{
			vector<HeroTrain_Config> vList;
			vList.push_back(sConfig);
			pTrainList->insert(make_pair(sConfig.byProType, vList));
		}
		//DWORD dwFindId = 10000 * sConfig.byProType + sConfig.byProLevel;
		//CConfigManager::Instance()->m_HeroTrainList.insert(make_pair(dwFindId, sConfig));
	}
	return true;
}

// 加载英雄培养消耗配置
bool LoadHeroTrainCost(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		HeroTrainCost_Config sConfig;
		string &tmp = data_list[i];
		sConfig.byCostType = ReadUInt(tmp.data(), "cost_type =");
		sConfig.tCostItem.dwId = ReadUInt(tmp.data(), "cost_item_id =");
		sConfig.tCostItem.dwNum = ReadUInt(tmp.data(), "cost_item_num =");
		sConfig.tCostMoney.byType = ReadUInt(tmp.data(), "cost_currency_type =");
		sConfig.tCostMoney.nValue = ReadUInt(tmp.data(), "cost_currency_value =");
		CConfigManager::Instance()->m_HeroTrainCostList.insert(make_pair(sConfig.byCostType, sConfig));
	}

	return true;
}


// 加载英雄培养属性配置
bool LoadHeroTrainPro(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		HeroTrainRandPro_Config sConfig;
		string &tmp = data_list[i];
		sConfig.byProType = ReadUInt(tmp.data(), "pro_type =");
		sConfig.nMin = ReadInt(tmp.data(), "pro_min =");
		sConfig.nMax = ReadInt(tmp.data(), "pro_max =");
		CConfigManager::Instance()->m_HeroTrainRandProList.push_back(sConfig);
	}

	return true;
}


// 加载装备强化锻造配置 
bool LoadEquipmentStrengthenForge(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> pro_type_list;
	vector<DWORD> pro_value_list;
	int nNum = 0;
	for (int i = 0; i < data_list.size(); ++i)
	{
		EquipmentForge_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wLevel = ReadUInt(tmp.data(), "strengthen_level =");
		ReadArry1(tmp.c_str(), "pro_type_list =", pro_type_list);
		ReadArry1(tmp.c_str(), "pro_value_list =", pro_value_list);
		if (pro_type_list.size() != pro_value_list.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]装备强化锻造属性配置不匹配[StrengthenLevel:%d]!",
				__FUNCTION__, sConfig.wLevel);
			return false;
		}

		Property sProperty;
		for (int k = 0; k < pro_type_list.size(); k++)
		{
			sProperty.wType = pro_type_list[k];
			sProperty.nValue = pro_value_list[k];
			sConfig.vProList.push_back(sProperty);
		}

		// 不需要累积
// 		if (nNum)
// 		{
// 			EquipmentForge_Config &sPreviousData = CConfigManager::Instance()->m_EquipmentEnhanceForgeList[nNum - 1];
// 			vector<Property>::iterator iter = sPreviousData.vProList.begin();
// 			for (iter; iter != sPreviousData.vProList.end(); ++iter)
// 			{
// 				sConfig.AddProperty(*iter);
// 			}
// 		}

		CConfigManager::Instance()->m_EquipmentEnhanceForgeList.push_back(sConfig);
		++nNum;
	}

	return true;
}

// 加载装备精练锻造配置
bool LoadEquipmentRefineForge(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> pro_type_list;
	vector<DWORD> pro_value_list;
	//int nNum = 0;
	for (int i = 0; i < data_list.size(); ++i)
	{
		EquipmentForge_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wLevel = ReadUInt(tmp.data(), "refine_level =");
		ReadArry1(tmp.c_str(), "pro_type_list =", pro_type_list);
		ReadArry1(tmp.c_str(), "pro_value_list =", pro_value_list);
		if (pro_type_list.size() != pro_value_list.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]装备强化锻造属性配置不匹配[refine_level:%d]!",
				__FUNCTION__, tConfig.wLevel);
			return false;
		}

		Property sProperty;
		for (int k = 0; k < pro_type_list.size(); k++)
		{
			sProperty.wType = pro_type_list[k];
			sProperty.nValue = pro_value_list[k];
			tConfig.vProList.push_back(sProperty);
		}

// 		if (nNum)
// 		{
// 			EquipmentForge_Config &sPreviousData = CConfigManager::Instance()->m_EquipmentRefineForgeList[nNum - 1];
// 			vector<Property>::iterator iter = sPreviousData.vProList.begin();
// 			for (iter; iter != sPreviousData.vProList.end(); ++iter)
// 			{
// 				tConfig.AddProperty(*iter);
// 			}
// 		}

		CConfigManager::Instance()->m_EquipmentRefineForgeList.push_back(tConfig);
//		++nNum;
	}

	return true;
}

// 加载宝物配置
bool LoadTreasure(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	//int nNum = 0;
	vector<vector<DWORD>> vEnhanceInitProList;
	vector<vector<DWORD>> vEnhanceLevelProList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		Treasure_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wInfoId = ReadUInt(tmp.data(), "id =");
		sConfig.wQuality = ReadUInt(tmp.data(), "quality =");
		sConfig.wStrengthenLevelMax = ReadUInt(tmp.data(), "strengthen_level_max =");
		sConfig.wRefineLevelMax = ReadUInt(tmp.data(), "refine_level_max =");
		sConfig.dwAsExp = ReadUInt(tmp.data(), "as_exp =");
		sConfig.byType = ReadUInt(tmp.data(), "treasure_type =");
		sConfig.nResetCostDiamond = ReadUInt(tmp.data(), "reset_cost_diamond =");
		sConfig.dwNextQualityTreasureId = ReadUInt(tmp.data(), "next_quality_treasure_id =");
		sConfig.dwQualityUPCostItemId = ReadUInt(tmp.data(), "quality_up_cost_item_id =");
		sConfig.nPrice = ReadUInt(tmp.data(), "price =");
		// 精练属性
		if (!ReadCommonPropertyList(tmp.data(), "refine_pro_type_list =", "refine_pro_value_list =", sConfig.vRefineLevelProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]宝物精练属性配置不匹配[info_id:%d]!", __FUNCTION__, sConfig.wInfoId);
			return false;
		}
		// 强化初始属性
		ReadArry2(tmp.c_str(), "strengthen_init_pro_list =", vEnhanceInitProList);
		for (size_t k = 0; k < vEnhanceInitProList.size(); ++k)
		{
			vector<DWORD> vProList = vEnhanceInitProList[k];
			if (2 != vProList.size())
			{
				MAPLOG(ERROR_LEVEL, "[%s]宝物强化属性配置不匹配[info_id:%d]!", __FUNCTION__, sConfig.wInfoId);
				continue;
			}
			Property tPro;
			tPro.wType = vProList[0];
			tPro.nValue = vProList[1];
			sConfig.vStrengthenInitProList.push_back(tPro);
		}
		// 强化等级属性
		ReadArry2(tmp.c_str(), "strengthen_add_pro_list =", vEnhanceLevelProList);
		for (size_t k = 0; k < vEnhanceLevelProList.size(); ++k)
		{
			vector<DWORD> vProList = vEnhanceLevelProList[k];
			if (2 != vProList.size())
			{
				MAPLOG(ERROR_LEVEL, "[%s]宝物强化属性配置不匹配[info_id:%d]!", __FUNCTION__, sConfig.wInfoId);
				continue;
			}
			Property tPro;
			tPro.wType = vProList[0];
			tPro.nValue = vProList[1];
			sConfig.vStrengthenLevelProList.push_back(tPro);
		}

		CConfigManager::Instance()->m_TreasureList.insert(make_pair(sConfig.wInfoId, sConfig));
	}

	return true;
}

// 加载宝物强化配置
bool LoadTreasureStrengthen(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	TreasureEnhance_Config sConfig;
	int nNum = 0;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		DWORD dwQuality = ReadUInt(tmp.data(), "quality =");
		DWORD dwStrengthenLevel = ReadUInt(tmp.data(), "strengthen_level =");
		sConfig.dwNeedExp = ReadUInt(tmp.data(), "exp =");
		sConfig.dwTotalExp = ReadUInt(tmp.data(), "total_exp =");
		DWORD dwFindId = 1000 * dwQuality + dwStrengthenLevel;
		CConfigManager::Instance()->m_TreasureEnhanceList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}

// 加载宝物精练配置
bool LoadTreasureRefine(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	TreasureRefine_Config sConfig;
	int nNum = 0;
	vector<CostData> vCostList;
	for (int i = 0; i < data_list.size(); ++i)
	{
 		string &tmp = data_list[i];
 		DWORD dwQuality = ReadUInt(tmp.data(), "quality =");
 		DWORD dwRefineLevel = ReadUInt(tmp.data(), "refine_level =");
		sConfig.byCostTreasureNum = ReadUInt(tmp.data(), "cost_treasure_num =");
		//sConfig.nCostGold = ReadUInt(tmp.data(), "cost_gold =");
		sConfig.dwCostItemId = ReadUInt(tmp.data(), "cost_item_id =");
		sConfig.wCostItemNum = ReadUInt(tmp.data(), "cost_item_num =");

		// 精练属性
		if (!LoadCommonCost(tmp.data(), vCostList, "cost_type_list =", "cost_value_list ="))
		{
			MAPLOG(ERROR_LEVEL, "[%s]宝物精练消耗配置不匹配[quality:%d,refine_level:%d]!", 
				__FUNCTION__, dwQuality, dwRefineLevel);
			return false;
		}

		sConfig.nCostGold = vCostList.size() ? vCostList[0].nValue : 0;

		DWORD dwFindId = 1000 * dwQuality + dwRefineLevel;
 		CConfigManager::Instance()->m_TreasureRefineList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}

// 加载宝物合成配置
bool LoadTreasureCompose(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	TreasureCompose_Config sConfig;
	DWORD dwTreasureInfoId;
	vector<DWORD> cost_item_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		dwTreasureInfoId = ReadUInt(tmp.data(), "id =");
		ReadArry1(tmp.c_str(), "cost_item_list =", cost_item_list);
		sConfig.cost_item_list = cost_item_list;
		CConfigManager::Instance()->m_TreasureComposeList.insert(make_pair(dwTreasureInfoId, sConfig));
	}

	return true;
}

// 加载宝物熔炼配置
bool LoadTreasureSmelt(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	TreasureSmelt_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		sConfig.dwTreasureInfoId = ReadUInt(tmp.data(), "id ="); 
		sConfig.dwCostTreasureInfoId = ReadUInt(tmp.data(), "cost_treasure_id =");
		sConfig.dwItemId = ReadUInt(tmp.data(), "item_id =");
		CConfigManager::Instance()->m_TreasureSmeltList.insert(make_pair(sConfig.dwTreasureInfoId, sConfig));
	}

	return true;
}

// 加载宝物升品消耗配置
bool LoadTreasureQualityUPCost(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		TreasureQualityUPCost_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wRefineLevel = ReadUInt(tmp.data(), "refine_level =");
		tConfig.dwCostItemNum = ReadUInt(tmp.data(), "cost_item_num =");
		if (!LoadCommonCost(tmp.data(), tConfig.vCostCurrencyList, "cost_currency_type_list =", "cost_currency_value_list ="))
		{
			MAPLOG(ERROR_LEVEL, "[%s]宝物升品消耗配置消耗配置不匹配[refine_level:%d]!",__FUNCTION__, tConfig.wRefineLevel);
			return false;
		}
		CConfigManager::Instance()->m_TreasureQualityUPCostList.insert(make_pair(tConfig.wRefineLevel, tConfig));
	}

	return true;
}

// 加载宝物强化锻造配置 
bool LoadTreasureEnhanceForge(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		TreasureForge_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wLevel = ReadUInt(tmp.data(), "strengthen_level =");
		if (!ReadCommonPropertyList(tmp.data(), "pro_type_list =", "pro_value_list =", sConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]宝物强化锻造属性配置出错[MapId:%d]!", __FUNCTION__, sConfig.wLevel);
			return false;
		}
		CConfigManager::Instance()->m_TreasureEnhanceForgeList.push_back(sConfig);
	}
	return true;
}


// 加载宝物精炼锻造配置 
bool LoadTreasureRefineForge(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		TreasureForge_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wLevel = ReadUInt(tmp.data(), "refine_level =");
		if (!ReadCommonPropertyList(tmp.data(), "pro_type_list =", "pro_value_list =", sConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]宝物精炼锻造属性配置出错[MapId:%d]!", __FUNCTION__, sConfig.wLevel);
			return false;
		}
		CConfigManager::Instance()->m_TreasureRefineForgeList.push_back(sConfig);
	}
	return true;
}


// 加载叛军功勋配置
bool LoadRebelExploit(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	RebelExploit_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		sConfig.dwListId = ReadUInt(tmp.data(), "list_id =");
		sConfig.dwAwardId = ReadUInt(tmp.data(), "award_id =");
		sConfig.nNeedExploit = ReadUInt(tmp.data(), "need_exploit =");
		sConfig.wAwardItemId = ReadUInt(tmp.data(), "award_item_id =");
		sConfig.wAwardItemNum = ReadUInt(tmp.data(), "award_item_num =");
		sConfig.nAwardGold = ReadUInt(tmp.data(), "award_gold =");

		DWORD dwFindId = sConfig.dwListId * 10000 + sConfig.dwAwardId;
		CConfigManager::Instance()->m_RebelExpoitList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}


// 加载叛军战役配置
bool LoadRebelBattle(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		RebelBattle_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wLevel = ReadUInt(tmp.data(), "level =");
		//sConfig.wMapId = ReadUInt(tmp.data(), "map_id =");
		ReadArry1(tmp.c_str(), "rand_map_list =", sConfig.vRandMapList);
		if (!ReadCommonPropertyList(tmp.c_str(), "add_pro_type_list", "add_pro_value_list", sConfig.vAddProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]叛军战役怪物影响属性配置不匹配[Level:%d]!", __FUNCTION__, sConfig.wLevel);
			return false;
		}

		CConfigManager::Instance()->m_RebelBattleList.insert(make_pair(sConfig.wLevel, sConfig));
	}

	return true;
}

// 加载叛军品质配置
bool LoadRebelQuality(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	RebelQuality_Config sConfig;
	vector<DWORD> pro_type_list;
	vector<DWORD> pro_ratio_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.byQuality = ReadUInt(tmp.data(), "quality =");
		sConfig.nDiscoverAwardDiamond = ReadUInt(tmp.data(), "discoverer_award =");
		sConfig.nKillerAwardDiamond = ReadUInt(tmp.data(), "killer_award =");
		sConfig.dwLiveTime = ReadUInt(tmp.data(), "live_time =");
		ReadArry1(tmp.c_str(), "pro_type_list =", pro_type_list);
		ReadArry1(tmp.c_str(), "pro_ratio_list =", pro_ratio_list);
		if (pro_type_list.size() != pro_ratio_list.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]叛军品质属性配置不匹配[quality:%d]", __FUNCTION__,sConfig.byQuality);
			continue;
		}

		Property tProperty;
		for (int i = 0; i < pro_type_list.size(); ++i)
		{
			tProperty.wType = pro_type_list[i];
			tProperty.nValue = pro_ratio_list[i];
			sConfig.vProList.push_back(tProperty);
		}

		CConfigManager::Instance()->m_RebelQualityList.insert(make_pair(sConfig.byQuality, sConfig));
	}

	return true;
}

// 加载角色叛军配置
bool LoadRoleRebel(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	RoleRebel_Config sConfig;
	vector<DWORD> quality_list;
	vector<DWORD> quality_rate_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wRoleLevel = ReadUInt(tmp.data(), "role_level =");
		sConfig.wAwardListId = ReadUInt(tmp.data(), "award_list_id =");

		ReadArry1(tmp.c_str(), "quality_list =", quality_list);
		ReadArry1(tmp.c_str(), "quality_rate_list =", quality_rate_list);
		if (quality_list.size() != quality_rate_list.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]角色叛军品质配置不匹配[RoleLevel:%d]", __FUNCTION__, sConfig.wRoleLevel);
			continue;
		}

		RateDataEx tRateData;
		for (int i = 0; i < quality_list.size(); ++i)
		{
			tRateData.nRate = quality_rate_list[i];
			tRateData.nValue = quality_list[i];
			sConfig.vRandList.push_back(tRateData);
		}

		CConfigManager::Instance()->m_RoleRebelList.insert(make_pair(sConfig.wRoleLevel, sConfig));
	}

	return true;
}


// 加载回收返还配置
bool LoadRecycleReturn(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	RecycleReturn_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		sConfig.byObjectType = ReadUInt(tmp.data(), "object_type =");
		sConfig.byObjectLevel = ReadUInt(tmp.data(), "object_level =");
		sConfig.dwObjectId = ReadUInt(tmp.data(), "object_id =");
		sConfig.dwNeedExp = ReadUInt(tmp.data(), "need_exp =");
		DWORD dwFindId = 10000 * sConfig.byObjectType + sConfig.byObjectLevel;
		CConfigManager::Instance()->m_RecycleReturnList.insert(make_pair(dwFindId, sConfig));
	}

	return true;
}

// 加载夺宝关卡配置
bool LoadDuobaoMap(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	DuobaoMap_Config sConfig;
	vector<DWORD> map_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wRoleLevel = ReadUInt(tmp.data(), "role_level =");
		sConfig.nWinGold = ReadUInt(tmp.data(), "win_gold =");
		sConfig.nFailGold = ReadUInt(tmp.data(), "fail_gold =");
		sConfig.dwExp = ReadUInt(tmp.data(), "exp =");
		sConfig.wPFGitfId = ReadUInt(tmp.data(), "fp_gift_id =");
		ReadArry1(tmp.c_str(), "map_list =", map_list);
		sConfig.vMapList = map_list;
		CConfigManager::Instance()->m_DuobaoMapList.insert(make_pair(sConfig.wRoleLevel, sConfig));
	}

	return true;
}

// 加载夺宝掉落配置
bool LoadDuobaoDrop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	DuobaoDrop_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		sConfig.byQuality = ReadUInt(tmp.data(), "quality =");
		sConfig.wUserRate = ReadUInt(tmp.data(), "user_rate =");
		sConfig.wRobotRate = ReadUInt(tmp.data(), "robot_rate =");
		sConfig.wTimesOfSureDrop = ReadUInt(tmp.data(), "times_of_sure_drop =");
		CConfigManager::Instance()->m_DuobaoDropList.insert(make_pair(sConfig.byQuality, sConfig));
	}

	return true;
}

// 加载夺宝进度奖励配置
bool LoadDuobaoPregrossReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	DuobaoPregrossReward_Config tConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		tConfig.wId = ReadUInt(tmp.data(), "id =");
		tConfig.wTimes = ReadUInt(tmp.data(), "times =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list", "reward_item_num_list", tConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]夺宝进度奖励配置不匹配[id:%d]!", __FUNCTION__, tConfig.wId);
			return false;
		}
		CConfigManager::Instance()->m_DuobaoPregrossRewardList.insert(make_pair(tConfig.wId, tConfig));
	}

	return true;
}

// 加载叛军商店商品
bool LoadRebelShopGoods(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	ShopGoodsData_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "id =");
		//sConfig.byEntityType = OBJECT_ITEM;
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		//sConfig.nCostGold = ReadUInt(tmp.data(), "cost_rebel_value =");
		sConfig.nNeedVipLevel = ReadUInt(tmp.data(), "need_vip_level =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]叛军商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonItemList(tmp.data(), "cost_item_id_list =", "cost_item_num_list =", sConfig.vCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]叛军商店道具消耗配置不匹配[GoodsId:%d]!", __FUNCTION__, sConfig.wGoodsId);
			return false;
		}

		AddShopGoods(SHOP_REBEL, sConfig);
	}

	return true;
}

// 加载叛军排行奖励配置
bool LoadRebelRankAward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	RebelRankAward_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		int nRankMin = ReadUInt(tmp.data(), "rank_min =");
		int nRankMax = ReadUInt(tmp.data(), "rank_max =");
		sConfig.nExploitAward = ReadUInt(tmp.data(), "exploit_award =");
		sConfig.nDamageAward = ReadUInt(tmp.data(), "damage_award =");
		for (int nRank = nRankMin; nRank <= nRankMax; ++nRank)
		{
			sConfig.wRank = nRank;
			CConfigManager::Instance()->m_RebelRankAwardList.insert(make_pair(sConfig.wRank, sConfig));
		}
	}

	return true;
}

// 加载英雄普通召唤配置
bool LoadHeroNormalRecruit(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	HeroNormalSummon_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		sConfig.dwHeroId = ReadUInt(tmp.data(), "id =");
		sConfig.dwRate = ReadUInt(tmp.data(), "rate =");
		CConfigManager::Instance()->m_NormalSummonList.push_back(sConfig);
	}

	return true;
}

// 加载英雄高级召唤配置
bool LoadHeroHighRecruit(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	HeroAdvancedSummon_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		sConfig.dwHeroId = ReadUInt(tmp.data(), "id =");
		sConfig.dwRate = ReadUInt(tmp.data(), "rate =");
		sConfig.dwRate1 = ReadUInt(tmp.data(), "rate_1 =");
		CConfigManager::Instance()->m_AdvancedSummonList.push_back(sConfig);
	}

	return true;
}

// 加载英雄高级召唤配置
bool LoadHeroVIPRecruit(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, vector<HeroVipSummon_Config>> *pSummonList = &CConfigManager::Instance()->m_HeroVipSummonList;
	HeroVipSummon_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		DWORD dwCamp = ReadUInt(tmp.data(), "camp =");
		sConfig.dwHeroId = ReadUInt(tmp.data(), "hero_id =");
		sConfig.dwHeroNum = ReadUInt(tmp.data(), "hero_num =");
		sConfig.dwNormalRate = ReadUInt(tmp.data(), "normal_rate =");
		sConfig.dwSpecialRate = ReadUInt(tmp.data(), "special_rate =");
		
		map<DWORD, vector<HeroVipSummon_Config>>::iterator iter = pSummonList->find(dwCamp);
		if (iter == pSummonList->end())
		{
			pSummonList->insert(make_pair(dwCamp, vector<HeroVipSummon_Config>()));
			iter = pSummonList->find(dwCamp);
		}

		iter->second.push_back(sConfig);
	}
	return true;
}

// 取爬塔关卡配置
bool LoadTowerMap(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	TowerMap_Config sConfig;
	vector<DWORD> map_list;
	vector<DWORD> award_star_list;
 	for (int i = 0; i < data_list.size(); ++i)
 	{
		sConfig.MapDataList.clear();
 		string &tmp = data_list[i];
 		sConfig.wMapLevel = ReadUInt(tmp.data(), "map_idx =");
		sConfig.wChapter = ReadUInt(tmp.data(), "chapter =");
		sConfig.bIsLast = ReadUInt(tmp.data(), "is_last_map =");
		ReadArry1(tmp.c_str(), "map_list =", map_list);
		ReadArry1(tmp.c_str(), "award_star_list =", award_star_list);
		if (map_list.size() != award_star_list.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]爬塔普通关卡与奖励配置不匹配", __FUNCTION__);
			continue;
		}
		for (int k = 0; k < map_list.size(); ++k)
		{
			TowerMap_Config::MapData sData;
			sData.wMapId = map_list[k];
			sData.byStar = award_star_list[k];
			sConfig.MapDataList.push_back(sData);
		}
		CConfigManager::Instance()->m_TowerMapList.insert(make_pair(sConfig.wMapLevel, sConfig));
 	}

	return true;
}

// 取爬塔章节配置
bool LoadTowerChapter(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> map_list;
	vector<DWORD> star_list;
	vector<vector<DWORD>> star_award_id_list;
	vector<vector<DWORD>> star_award_num_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		TowerChapter_Config sConfig;
		sConfig.vAwardList.clear();
		sConfig.vMapList.clear();
		string &tmp = data_list[i];
		sConfig.byChapter = ReadUInt(tmp.data(), "chapter =");
		ReadArry1(tmp.c_str(), "map_list =", map_list);
		for (int k = 0; k < map_list.size(); ++k)
		{
			sConfig.vMapList.push_back(map_list[k]);
		}
		ReadArry1(tmp.c_str(), "star_list =", star_list);
		ReadArry2(tmp.c_str(), "star_award_id_list =", star_award_id_list);
		ReadArry2(tmp.c_str(), "star_award_num_list =", star_award_num_list);
		if ((star_list.size() != map_list.size()) || 
			(star_list.size() != star_award_id_list.size()) || 
			(star_list.size() != star_award_num_list.size()))
		{
			MAPLOG(ERROR_LEVEL, "[%s]爬塔普通章节星数与奖励配置不匹配", __FUNCTION__);
			continue;
		}
		//Item tItem;
		for (int m = 0; m < star_list.size(); ++m)
		{
			TowerStarAward tStarAward;
			tStarAward.byStar = star_list[m];
			vector<DWORD> &id_list = star_award_id_list[m];
			vector<DWORD> &num_list = star_award_num_list[m];
			if (id_list.size() != num_list.size())
			{
				MAPLOG(ERROR_LEVEL, "[%s]爬塔普通章节星数奖励的对象ID与数量不匹配!",__FUNCTION__);
				continue;
			}
			for (int k = 0; k < id_list.size(); ++k)
			{
				tStarAward.vAwardList.push_back(Item(id_list[k], num_list[k]));
			}
			sConfig.vAwardList.push_back(tStarAward);
		}

		CConfigManager::Instance()->m_TowerChapterList.insert(make_pair(sConfig.byChapter, sConfig));
	}

	return true;
}

// 取爬塔精英关卡
bool LoadTowerEliteMap(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	TowerEliteMap_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		sConfig.wMapLevel = ReadUInt(tmp.data(), "level =");
		sConfig.wMapId = ReadUInt(tmp.data(), "map_id =");
		sConfig.wNeedNormalMapLevel = ReadUInt(tmp.data(), "need_nomal_map_level =");
		CConfigManager::Instance()->m_TowerEliteMapList.insert(make_pair(sConfig.wMapLevel, sConfig));
	}

	return true;
}

// 取随机属性
bool LoadTowerRandPro(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> pro_type_list;
	vector<DWORD> pro_value_list;
	Property tPro;
	for (int i = 0; i < data_list.size(); ++i)
	{
		TowerRandPro_Config sConfig;
		string &tmp = data_list[i];
		sConfig.byStar = ReadUInt(tmp.data(), "star =");
		ReadArry1(tmp.c_str(), "pro_type_list =", pro_type_list);
		ReadArry1(tmp.c_str(), "pro_value_list =", pro_value_list);
		if (pro_type_list.size() != pro_value_list.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]爬塔随机属性类型与值不匹配!",__FUNCTION__);
			continue;
		}
		for (int k = 0; k < pro_type_list.size(); ++k)
		{
			tPro.wType = pro_type_list[k];
			tPro.nValue = pro_value_list[k];
			sConfig.vProList.push_back(tPro);
		}
		CConfigManager::Instance()->m_TowerRandProList.insert(make_pair(sConfig.byStar, sConfig));
	}

	return true;
}

// 取星数商品
bool LoadTowerStarGoods(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> award_id_list;
	vector<DWORD> award_num_list;
	vector<DWORD> award_price_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		TowerStarGoods_Config sConfig;
		string &tmp = data_list[i]; 
		sConfig.wStarMin = ReadUInt(tmp.data(), "star_min =");
		sConfig.wStarMax = ReadUInt(tmp.data(), "star_max =");
		ReadArry1(tmp.c_str(), "award_id_list =", award_id_list);
		ReadArry1(tmp.c_str(), "award_num_list =", award_num_list);
		ReadArry1(tmp.c_str(), "award_price_list =", award_price_list);
		if ((award_id_list.size() != award_num_list.size()) || (award_id_list.size() != award_price_list.size()))
		{
			MAPLOG(ERROR_LEVEL, "[%s]爬塔星数商品(id,num,price)配置不匹配!", __FUNCTION__);
			continue;
		}
		TowerStarGoods_Config::StarGoods tGoods;
		for (int k = 0; k < award_id_list.size(); ++k)
		{
			tGoods.nId = award_id_list[k];
			tGoods.wNum = award_num_list[k];
			tGoods.nPrice = award_price_list[k];
			sConfig.vGoodsList.push_back(tGoods);
		}

		for (WORD k = sConfig.wStarMin; k <= sConfig.wStarMax; ++k)
		{
			CConfigManager::Instance()->m_TowerStarGoodsList.insert(make_pair(k, sConfig));
		}
	}

	return true;
}

// 加载爬塔商店商品
bool LoadTowerShopGoods(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	ShopGoodsData_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "id =");
		sConfig.byType = ReadUInt(tmp.data(), "type =");
		//sConfig.byEntityType = OBJECT_ITEM;
		sConfig.dwEntityId = ReadUInt(tmp.data(), "object_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "object_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		sConfig.nContext = ReadUInt(tmp.data(), "need_star =");
		sConfig.nNeedVipLevel = ReadUInt(tmp.data(), "need_vip_level =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]爬塔商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonItemList(tmp.data(), "cost_item_id_list =", "cost_item_num_list =", sConfig.vCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]爬塔商店消耗的物品配置不匹配!", __FUNCTION__);
			continue;
		}
		AddShopGoods(SHOP_TOWER, sConfig);
	}

	return true;
}

// 加载精灵配置
bool LoadHeartOfWorld(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> add_pro;
	vector<DWORD> award_item;
	vector<DWORD> cost_item;
	for (int i = 0; i < data_list.size(); ++i)
	{
		HeartOfWorld_Config sConfig;
		HeartOfWorld_Config::LevelData tLevelData;
		memset(&tLevelData, 0, sizeof(tLevelData)); 
		string &tmp = data_list[i];
		BYTE byStage = ReadUInt(tmp.data(), "stage =");
		tLevelData.byLevel = ReadUInt(tmp.data(), "level =");
		tLevelData.wRoleQuality = ReadUInt(tmp.data(), "role_quality =");
		ReadArry1(tmp.c_str(), "award_item =", award_item);
		ReadArry1(tmp.c_str(), "cost_item =", cost_item);
		if (!ReadCommonPropertyList(tmp.c_str(), "add_pro_type", "add_pro_value", tLevelData.vAddProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]精灵属性配置不匹配[stage:%d,level:%d]!", __FUNCTION__, byStage, tLevelData.byLevel);
			return false;
		}

		if (award_item.size() == 2)
		{
			tLevelData.sAwardItem.dwId = award_item[0];
			tLevelData.sAwardItem.dwNum = award_item[1];
		}
		if (cost_item.size() == 2)
		{
			tLevelData.sCostItem.dwId = cost_item[0];
			tLevelData.sCostItem.dwNum = cost_item[1];
		}

		map<DWORD, HeartOfWorld_Config>::iterator iter = CConfigManager::Instance()->m_HeartOfWorldList.find(byStage);
		if (iter != CConfigManager::Instance()->m_HeartOfWorldList.end())
		{
			iter->second.vLeveDatalist.push_back(tLevelData);
		}
		else
		{
			sConfig.byStage = byStage;
			sConfig.vLeveDatalist.push_back(tLevelData);
			CConfigManager::Instance()->m_HeartOfWorldList.insert(make_pair(sConfig.byStage, sConfig));
		}
	}

	return true;
}

// 加载精灵配置
bool LoadSpirit(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Spirit_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwId = ReadUInt(tmp.data(), "id =");
		sConfig.wPokedexId = ReadUInt(tmp.data(), "pokedex_id =");
		sConfig.wInitStar = ReadUInt(tmp.data(), "init_star =");
		sConfig.wStarMax = ReadUInt(tmp.data(), "star_max =");
		sConfig.dwTtarUpCostItemId = ReadUInt(tmp.data(), "star_up_cost_item_id =");
		sConfig.dwFragmentNum = ReadUInt(tmp.data(), "fragment_num =");
		ReadArry1(tmp.c_str(), "qualifications =", sConfig.vQualificationsList);
		CConfigManager::Instance()->m_SpiritList.insert(make_pair(sConfig.dwId, sConfig));
	}
	return true;
}

// 加载精灵强化配置
bool LoadSpiritEnhance(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		SpiritEnhance_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwId = ReadUInt(tmp.data(), "id =");
		sConfig.wEnhanceLevel = ReadUInt(tmp.data(), "enhance_level =");
		if (!ReadCommonPropertyList(tmp.c_str(), "pro_type_list", "pro_value_list", sConfig.vAddProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]精灵强化属性配置不匹配[id:%d,enhance_level:%d]!", 
				__FUNCTION__, sConfig.dwId, sConfig.wEnhanceLevel);
			return false;
		}
		if (!ReadCommonItemList(tmp.c_str(), "cost_item_id_list", "cost_item_num_list", sConfig.vCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]精灵强化消耗配置不匹配[id:%d,enhance_level:%d]!",
				__FUNCTION__, sConfig.dwId, sConfig.wEnhanceLevel);
			return false;
		}
		DWORD dwFindId = sConfig.dwId * 1000 + sConfig.wEnhanceLevel;
		CConfigManager::Instance()->m_SpiritEnhanceList.insert(make_pair(dwFindId, sConfig));
	}
	return true;
}

// 加载精灵图鉴配置
bool LoadSpirtPokedex(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		SpiritPokedex_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwPokedexId = ReadUInt(tmp.data(), "id =");
		sConfig.wPokedexLevel = ReadUInt(tmp.data(), "pokedex_level =");
		sConfig.wNeedEnhanceLevel = ReadUInt(tmp.data(), "need_enhance_level =");
		sConfig.vAddPro.wType = ReadUInt(tmp.data(), "pro_type =");
		sConfig.vAddPro.nValue = ReadUInt(tmp.data(), "pro_value =");
		ReadArry1(tmp.c_str(), "spirit_list =", sConfig.vNeedSpiritList);
		DWORD dwFindId = sConfig.dwPokedexId * 100 + sConfig.wPokedexLevel;
		CConfigManager::Instance()->m_SpiritPokedexList.insert(make_pair(dwFindId, sConfig));
	}
	return true;
}

bool LoadSpirtTrain(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vStageUpCostItem;
	for (int i = 0; i < data_list.size(); ++i)
	{
		SpriteTrain_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwId = ReadUInt(tmp.data(), "id =");
		sConfig.wStage = ReadUInt(tmp.data(), "stage =");
		sConfig.wNeedEnhanceLevel = ReadUInt(tmp.data(), "need_enhance_level =");
		sConfig.wNeedTrainTimes = ReadUInt(tmp.data(), "need_train_times =");
		sConfig.wSkillLevel = ReadUInt(tmp.data(), "skill_leve =");
		sConfig.dwModulId = ReadUInt(tmp.data(), "model_id =");
		sConfig.dwProConversionRate = ReadUInt(tmp.data(), "pro_conversion_rate =");
		sConfig.wTalentLevel = ReadUInt(tmp.data(), "talent_level =");
		ReadArry1(tmp.c_str(), "stage_up_cost_item =", vStageUpCostItem);
		if (2 == vStageUpCostItem.size())
		{
			sConfig.tStageUpCostItem.dwId = vStageUpCostItem[0];
			sConfig.tStageUpCostItem.dwNum = vStageUpCostItem[1];
		}
		else
		{
			memset(&sConfig.tStageUpCostItem, 0, sizeof(sConfig.tStageUpCostItem));
		}
		if (!ReadCommonItemList(tmp.c_str(), "cost_item_id_list", "cost_item_num_list", sConfig.vTrainCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]精灵培养消耗配置不匹配[id:%d]!",__FUNCTION__, sConfig.dwId);
			return false;
		}
		if (!ReadCommonPropertyList(tmp.c_str(), "pro_type_list", "pro_value_list", sConfig.vTrainProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]精灵培养属性配置不匹配[id:%d]!", __FUNCTION__, sConfig.dwId);
			return false;
		}
		if (!ReadCommonPropertyList(tmp.c_str(), "total_pro_type_list", "total_pro_value_list", sConfig.vTrainTotalProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]精灵培养总属性配置不匹配[id:%d]!", __FUNCTION__, sConfig.dwId);
			return false;
		}
		DWORD dwFindId = sConfig.dwId * 100 + sConfig.wStage;
		CConfigManager::Instance()->m_SpiritTrainList.insert(make_pair(dwFindId, sConfig));
	}
	return true;
}

// 加载精灵天赋技能
bool LoadSpriteTalentSkill(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		SpriteTalentSkill_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwSpriteId = ReadUInt(tmp.data(), "id =");
		sConfig.wTalentSkillId = ReadUInt(tmp.data(), "level =");
		DWORD dwKey = sConfig.dwSpriteId * 100 + sConfig.wTalentSkillId;

		ReadArry1(tmp.c_str(), "fb_type =", sConfig.m_vFBTypeList);
		ReadArry1(tmp.c_str(), "extra_reward_drop_item_id =", sConfig.vExtraRewardDropItemId);
		ReadArry1(tmp.c_str(), "extra_reward_drop_item_rate =", sConfig.vExtraRewardDropItemRate);
		ReadArry1(tmp.c_str(), "extra_reward_currency_type =", sConfig.vExtraRewardCurrencyType);
		ReadArry1(tmp.c_str(), "extra_reward_currency_rate =", sConfig.vExtraRewardCurrencyRate);
		if (!ReadCommonPropertyList(tmp.c_str(), "hero_pro_type_list", "hero_pro_value_list", sConfig.vHeroProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]精灵天赋影响的英雄属性配置不匹配[id:%d]!", __FUNCTION__, dwKey);
			return false;
		}
		if (!ReadCommonPropertyList(tmp.c_str(), "spirit_pro_type_list", "spirit_pro_value_list", sConfig.vSpriteProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]精灵天赋影响的精灵属性配置不匹配[id:%d]!", __FUNCTION__, dwKey);
			return false;
		}

		// 配置数据检测
		if (sConfig.vExtraRewardDropItemId.size() != sConfig.vExtraRewardDropItemRate.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]精灵天赋影响的精灵额外掉落物品配置不匹配[key:%d]!", __FUNCTION__, dwKey);
			return false;
		}
		if (sConfig.vExtraRewardCurrencyType.size() != sConfig.vExtraRewardCurrencyRate.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]精灵天赋影响的精灵额外掉落货币配置不匹配[key:%d]!", __FUNCTION__, dwKey);
			return false;
		}

		CConfigManager::Instance()->m_SpriteTalentSkillList.insert(make_pair(dwKey, sConfig));
	}
	return true;
}


// 加载精灵升星配置
bool LoadSpriteStarUp(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		SpriteStarUp_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wStar = ReadUInt(tmp.data(), "star =");
		sConfig.dwStarUpCostItemNum = ReadUInt(tmp.data(), "star_up_cost_item_num =");
		CConfigManager::Instance()->m_SpriteStarUpList.insert(make_pair(sConfig.wStar, sConfig));
	}
	return true;
}

// 加载精灵特性配置
bool LoadSpriteSpecialProperty(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		SpriteSpecialProperty_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwSpriteId = ReadUInt(tmp.data(), "id =");
		sConfig.wStar = ReadUInt(tmp.data(), "star =");
		DWORD dwKey = sConfig.dwSpriteId * 100 + sConfig.wStar;
		if (!ReadCommonPropertyList(tmp.c_str(), "bless_pro_type_list", "bless_pro_value_list", sConfig.vBlessProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]精灵特性属性配置不匹配[id:%d]!", __FUNCTION__, dwKey);
			return false;
		}

		CConfigManager::Instance()->m_SpriteSpecialPropertyList.insert(make_pair(dwKey, sConfig));
	}
	return true;
}

// 加载精灵资质配置
bool LoadSpriteQualifications(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		SpriteQualifications_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wQualifications = ReadUInt(tmp.data(), "qualifications =");
		sConfig.nAddRate = ReadUInt(tmp.data(), "add_rate =");
		CConfigManager::Instance()->m_SpriteQualificationsList.insert(make_pair(sConfig.wQualifications, sConfig));
	}
	return true;
}






// 加载领地城池
bool LoadLandCity(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> skill_level_list;
	vector<DWORD> double_award_rate_list;
	vector<DWORD> need_time_list;
	vector<DWORD> cost_diamond_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		LandCity_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wCityId = ReadUInt(tmp.data(), "id =");
		sConfig.wPreCityId = ReadUInt(tmp.data(), "pre_city_id =");
		sConfig.wMapId = ReadUInt(tmp.data(), "map_id =");
		sConfig.nSuppressEnergy = ReadUInt(tmp.data(), "suppress_energy =");
		sConfig.nHelpSuppressDiamond = ReadUInt(tmp.data(), "help_suppress_diamond =");
		ReadArry1(tmp.c_str(), "skill_level_list =", skill_level_list);
		ReadArry1(tmp.c_str(), "double_award_rate_list =", double_award_rate_list);
		ReadArry1(tmp.c_str(), "need_time_list =", need_time_list);
		ReadArry1(tmp.c_str(), "cost_diamond_list =", cost_diamond_list);
		if (skill_level_list.size() != double_award_rate_list.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]领地技能等级与双倍奖励几率配置不匹配!", __FUNCTION__);
			return false;
		}
		if (skill_level_list.size() != need_time_list.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]领地技能等级与需要的巡逻时间配置不匹配!", __FUNCTION__);
			return false;
		}
		if (skill_level_list.size() != cost_diamond_list.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]领地技能等级与消耗的钻石配置不匹配!", __FUNCTION__);
			return false;
		}
		for (int k = 0; k < skill_level_list.size(); ++k)
		{
			LandSkill tLandSkill;
			tLandSkill.wLevel = skill_level_list[k];
			tLandSkill.wDoubleAwardRate = double_award_rate_list[k];
			tLandSkill.wNeedPatrolTime = need_time_list[k];
			tLandSkill.nCostDiamond = cost_diamond_list[k];
			sConfig.vSkillList.push_back(tLandSkill);
		}
		CConfigManager::Instance()->m_LandCityList.insert(make_pair(sConfig.wCityId, sConfig));
	}

	return true;
}

// 加载领地巡逻事件
bool LoadLandPatrolEvent(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, LandPatrolEvent_Config> *pPatrolEventList = &CConfigManager::Instance()->m_LandPatrolEventList;

	for (int i = 0; i < data_list.size(); ++i)
	{
		LandPatrolEvent_Config::EventData tEventData;
		string &tmp = data_list[i];
		WORD wCityId = ReadUInt(tmp.data(), "city_id =");
		tEventData.wEventId = ReadUInt(tmp.data(), "event_id =");
		tEventData.wEventRate = ReadUInt(tmp.data(), "event_rate =");
		tEventData.dwObjId = ReadUInt(tmp.data(), "object_id =");
		tEventData.dwOjbNum = ReadUInt(tmp.data(), "object_num =");
		map<DWORD, LandPatrolEvent_Config>::iterator iter = pPatrolEventList->find(wCityId);
		if (iter != pPatrolEventList->end())
		{
			iter->second.vEventList.push_back(tEventData);
		}
		else
		{
			LandPatrolEvent_Config sConfig;
			sConfig.wCityId = wCityId;
			sConfig.vEventList.push_back(tEventData);
			pPatrolEventList->insert(make_pair(sConfig.wCityId, sConfig));
		}
	}

	return true;
}

// 加载领地巡逻配置
bool LoadLandPatrol(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> time_type_data;
	for (int i = 0; i < data_list.size(); ++i)
	{
		LandPatrol_Config sConfig;
		LandPatrol_Config::TimeData tTimeData;
		string &tmp = data_list[i];
		sConfig.byType = ReadUInt(tmp.data(), "patrol_type =");
		sConfig.wIntervalTime = ReadUInt(tmp.data(), "interval_time =") * 60;
		sConfig.byVipIdx = ReadUInt(tmp.data(), "vip_idx =");
		for (int k = 1; k <= 3; ++k)
		{
			char szKey[64] = { 0 };
			sprintf_s(szKey, sizeof(szKey)-1, "time_type_data_%d", k);
			ReadArry1(tmp.c_str(), szKey, time_type_data);

			if (time_type_data.size() != 5)
			{
				MAPLOG(ERROR_LEVEL, "[%s]领地巡逻时间数据块配置不匹配!!", __FUNCTION__);
				return false;
			}
	
			// [总时间|消耗精力|消耗钻石|碎片数量下限|碎片数量上限]
			tTimeData.byTimeType = k;
			tTimeData.dwTotalTime = time_type_data[0] * 60;
			tTimeData.nCostEnergy = time_type_data[1];
			tTimeData.nCostGold = time_type_data[2];
			tTimeData.wItemNumMin = time_type_data[3];
			tTimeData.wItemNumMax = time_type_data[4];
			sConfig.vTimeDataList.push_back(tTimeData);	
		}
		CConfigManager::Instance()->m_LandPatrolList.insert(make_pair(sConfig.byType, sConfig));
	}

	return true;
}

// 加载日常副本章节配置
bool LoadDailyChapter(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		FubenChapter_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwChapter = ReadUInt(tmp.data(), "id ="); 
		sConfig.wOpenLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wDailyBattleTimes = ReadUInt(tmp.data(), "daily_battle_times =");
		sConfig.byIsDamageReward = ReadUInt(tmp.data(), "is_damage_reward =");
		sConfig.byIsLoseAlsoHaveReward = ReadUInt(tmp.data(), "is_lose_also_have_reward =");
		ReadArry1(tmp.c_str(), "week_day_list", sConfig.vOpenWeekdayList);
		ReadArry1(tmp.c_str(), "map_list", sConfig.vMapList);
		ReadArry1(tmp.c_str(), "map_level_limit_list", sConfig.vLevelLimitList);
		if (sConfig.vMapList.size() != sConfig.vLevelLimitList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]日常副本关卡与开放等级配置不匹配[chapter:%d]!!", __FUNCTION__, sConfig.dwChapter);
			return false;
		}
		CConfigManager::Instance()->m_DailyChapterList.insert(make_pair(sConfig.dwChapter, sConfig));
	}
	return true;
}

// 加载竞技场机器人配置
bool LoadArenaRobot(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ArenaRobot_Config sConfig;
		string &tmp = data_list[i];
		sConfig.dwRobotId = ReadUInt(tmp.data(), "id =");
		sConfig.wRank = ReadUInt(tmp.data(), "rank =");
		sConfig.strRobotName = ReadString(tmp.data(), "name = ");
		// 检测名字的长度(有可能部分语言的名字比较长,比如:维语 所以放大两倍处理)
		if (strlen(sConfig.strRobotName.c_str()) >= MAX_ROLE_LEN * 2)
		{
			MAPLOG(ERROR_LEVEL, "[%s]竞技场机器人名称过长[RobotId:%d,NameLen:%d]!!", 
				__FUNCTION__, sConfig.dwRobotId, strlen(sConfig.strRobotName.c_str()));

			sConfig.strRobotName = "HelloWorld";
			//continue;
		}
		CConfigManager::Instance()->m_ArenaRobotList.push_back(sConfig);
	}
	return true;
}

// 加载竞技场配置
bool LoadArenaRank(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ArenaRank_Config sConfig;
		string &tmp = data_list[i];
		WORD wRankMin = ReadUInt(tmp.data(), "rank_min =");
		WORD wRankMax = ReadUInt(tmp.data(), "rank_max =");
		sConfig.wTargetRankInterval = ReadUInt(tmp.data(), "target_rank_interval =");
		sConfig.nAwardPrestige = ReadUInt(tmp.data(), "award_prestige =");
		sConfig.nAwardGold = ReadUInt(tmp.data(), "award_gold =");
		sConfig.nFirstRankDiamond = 0;
		for (WORD i = wRankMin; i <= wRankMax; ++i)
		{
			sConfig.wRank = i;
			CConfigManager::Instance()->m_ArenaRankList.insert(make_pair(sConfig.wRank, sConfig));
		}
	}
	return true;
}

// 加载竞技场奖励配置
bool LoadArenaAward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ArenaAward_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wRoleLevel = ReadUInt(tmp.data(), "level =");
		sConfig.dwExp = ReadUInt(tmp.data(), "exp =");
		sConfig.nWinGold = ReadUInt(tmp.data(), "win_gold =");
		sConfig.nFailGold = ReadUInt(tmp.data(), "fail_gold =");
		sConfig.nWinArenaPrestige = ReadUInt(tmp.data(), "win_arena_prestige =");
		sConfig.nFailArenaPrestige = ReadUInt(tmp.data(), "fail_arena_prestige =");
		sConfig.wFPItemId = ReadUInt(tmp.data(), "fp_item_id =");
		CConfigManager::Instance()->m_ArenaAwardList.insert(make_pair(sConfig.wRoleLevel, sConfig));
	}
	return true;
}




// 加载竞技场首次排名奖励
bool LoadArenaFirstRankAward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		WORD wRank = ReadUInt(tmp.data(), "rank =");
		int nDiamond = ReadUInt(tmp.data(), "diamond =");
// 		map<DWORD, ArenaRank_Config>::iterator iter = CConfigManager::Instance()->m_ArenaRankList.find(wRank);
// 		if (iter == CConfigManager::Instance()->m_ArenaRankList.end())
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]找不到竞技场排名奖励数据[rank:%d]!!", __FUNCTION__, wRank);
// 			return false;
// 		}
//		iter->second.nFirstRankDiamond = ReadUInt(tmp.data(), "diamond =");
		CConfigManager::Instance()->m_ArenaFirstRankRewardList.insert(make_pair(wRank, nDiamond));	
	}
	return true;
}

// 加载竞技场商店商品
bool LoadArenaShopGoods(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "id =");
		sConfig.byType = ReadUInt(tmp.data(), "type =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		sConfig.nContext = ReadUInt(tmp.data(), "need_rank =");
		sConfig.nNeedVipLevel = 0;
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]竞技场消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonItemList(tmp.data(), "cost_item_id_list =", "cost_item_num_list =", sConfig.vCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]竞技场道具消耗配置不匹配[GoodsId:%d]!", __FUNCTION__, sConfig.wGoodsId);
			return false;
		}

		AddShopGoods(SHOP_ARENA, sConfig);

		// 奖励商品
		if (2 == sConfig.byType)
			CConfigManager::Instance()->m_ArenaShopGoodsList.push_back(sConfig.wGoodsId);
	}

	return true;
}

// 加载成就配置
bool LoadAchievement(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, Achievement_Config> *pAchievementList = &CConfigManager::Instance()->m_AchievementList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		Achievement_Config::LvData tLvData;
		string &tmp = data_list[i];
		WORD wAchievementId = ReadUInt(tmp.data(), "id =");
		tLvData.wNeedRoleLv = ReadUInt(tmp.data(), "need_role_level =");
		tLvData.wLv = ReadUInt(tmp.data(), "level =");
		tLvData.wValue = ReadUInt(tmp.data(), "value =");
		tLvData.tRewardItem.dwId = ReadUInt(tmp.data(), "award_item_id =");
		tLvData.tRewardItem.dwNum = ReadUInt(tmp.data(), "award_item_num =");
		//tLvData.vAwardList.push_back(tRewardItem);
		map<DWORD, Achievement_Config>::iterator iter = pAchievementList->find(wAchievementId);
		if (iter != pAchievementList->end())
		{
			iter->second.vLvDataList.push_back(tLvData);
		}
		else
		{
			Achievement_Config sConfig;
			sConfig.wAchievementId = wAchievementId;
			sConfig.vLvDataList.push_back(tLvData);
			pAchievementList->insert(make_pair(sConfig.wAchievementId, sConfig));
		}
	}

	return true;
}


// 加载工会配置
bool LoadGuild(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Guild_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wLevel = ReadUInt(tmp.data(), "level =");
		sConfig.dwNeedExp = ReadUInt(tmp.data(), "need_exp =");
		sConfig.wMaxMember = ReadUInt(tmp.data(), "member_max =");
		CConfigManager::Instance()->m_GuildList.insert(make_pair(sConfig.wLevel, sConfig));
	}

	return true;
}

// 加载工会图标配置
bool LoadGuildIcon(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		WORD wIconId = ReadUInt(tmp.data(), "id =");
		WORD wNeedRank = ReadUInt(tmp.data(), "need_rank =");
		CConfigManager::Instance()->m_GuildIconList.insert(make_pair(wIconId, wNeedRank));
	}

	return true;
}

// 加载工会权限配置
bool LoadGuildPrivilege(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		WORD wIconId = ReadUInt(tmp.data(), "id =");
		WORD wNeedRank = ReadUInt(tmp.data(), "need_rank =");
		//CConfigManager::Instance()->m_GuildIconList.insert(make_pair(wIconId, wNeedRank));
	}

	return true;
}

// 加载工会泉涌配置
bool LoadGuildQY(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		GuildQY_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wType = ReadUInt(tmp.data(), "type =");
		sConfig.wCritRate = ReadUInt(tmp.data(), "crit_rate =");
		sConfig.dwExp = ReadUInt(tmp.data(), "exp =");
		sConfig.dwCritExp = ReadUInt(tmp.data(), "crit_exp =");
		sConfig.dwContribution = ReadUInt(tmp.data(), "contribution =");
		sConfig.dwCritContribution = ReadUInt(tmp.data(), "crit_contribution =");
		sConfig.wNeedVipLevel = ReadUInt(tmp.data(), "need_vip_level =");
		sConfig.dwQuanShui = ReadUInt(tmp.data(), "qs =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]工会泉涌消耗配置不匹配!", __FUNCTION__);
			return false;
		}

		CConfigManager::Instance()->m_GuildQYList.insert(make_pair(sConfig.wType, sConfig));
	}

	return true;
}

// 加载工会泉水奖励配置
bool LoadGuildQSReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		GuildQSAward_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wQS = ReadUInt(tmp.data(), "qs =");
		sConfig.wGuildLevel = ReadUInt(tmp.data(), "guild_level =");
		if (!ReadCommonItemList(tmp.c_str(), "item_id_list =", "item_num_list =", sConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]工会泉涌消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		DWORD wFindId = sConfig.wQS * 100 + sConfig.wGuildLevel;
		CConfigManager::Instance()->m_GuildQSRewardList.insert(make_pair(wFindId, sConfig));
	}

	return true;
}

// 加载Vip配置
bool LoadVip(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Vip_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wLevel = ReadUInt(tmp.data(), "level =");
		sConfig.dwNeedExp = ReadUInt(tmp.data(), "need_vip_exp =");
		int nVipValue = 0;
		char szKey[32];
		for (BYTE i = 0; i < VIP_TYPE_MAX; ++i)
		{
			sprintf_s(szKey, "value%d =", i+1);
			nVipValue = ReadUInt(tmp.data(), szKey);
			sConfig.vVipList.push_back(nVipValue);
		}
		CConfigManager::Instance()->m_VipList.insert(make_pair(sConfig.wLevel, sConfig));
	}

	return true;
}

bool LoadVipGM(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		VipGM_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wGMLevel = ReadUInt(tmp.data(), "gm_level =");
		tConfig.dwNeedRechargeAmount = ReadUInt(tmp.data(), "need_recharge_amount =");
		CConfigManager::Instance()->m_VipGMList.insert(make_pair(tConfig.wGMLevel, tConfig));
	}

	return true;
}

// 加载Vip配置
bool LoadVipNotice(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		WORD wLevel = ReadUInt(tmp.data(), "level =");
		DWORD dwNoticeId = ReadUInt(tmp.data(), "notice_id =");
		CConfigManager::Instance()->m_VipNoticeList.insert(make_pair(wLevel, dwNoticeId));
	}

	return true;
}



// 加载英雄商店
bool LoadHeroShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wRefreshRate = ReadUInt(tmp.data(), "rate =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_HERO, sConfig);
	}

	return true;
}



// 加载英雄商店槽位数据
bool LoadHeroShopSlot(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, ShopGoodsData_Config> *pShopGoodsList = &CConfigManager::Instance()->m_ShopGoodshList;
	vector<DWORD> goods_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		RefreshShopSlot_Config sConfig;
		string &tmp = data_list[i];
		sConfig.bySlot = ReadUInt(tmp.data(), "slot =");
		//sConfig.byIsCanRepeat = ReadUInt(tmp.data(), "is_can_repeat =");
		ReadArry1(tmp.c_str(), "goods_list", goods_list);
		for (size_t k = 0; k < goods_list.size(); ++k)
		{
			DWORD dwFindId = 10000 * SHOP_HERO + goods_list[k];
			map<DWORD, ShopGoodsData_Config>::iterator iter = CConfigManager::Instance()->m_ShopGoodshList.find(dwFindId);
			if (iter == pShopGoodsList->end())
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到英雄商店商品[GoodsId:%d]!", __FUNCTION__, dwFindId);
				continue;
			}
			sConfig.vGoodsList.push_back(&iter->second);
		}
		CConfigManager::Instance()->m_HeroShopSlotList.push_back(sConfig);
	}

	return true;
}

// 加载水晶商店
bool LoadCrystalShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wRefreshRate = ReadUInt(tmp.data(), "rate =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]水晶商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_CRYSTAL, sConfig);
	}

	return true;
}

// 加载水晶商店槽位数据
bool LoadCrystalShopSlot(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, ShopGoodsData_Config> *pShopGoodsList = &CConfigManager::Instance()->m_ShopGoodshList;
	vector<DWORD> goods_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		RefreshShopSlot_Config sConfig;
		string &tmp = data_list[i];
		sConfig.bySlot = ReadUInt(tmp.data(), "slot =");
		ReadArry1(tmp.c_str(), "goods_list", goods_list);
		for (size_t k = 0; k < goods_list.size(); ++k)
		{
			DWORD dwFindId = 10000 * SHOP_CRYSTAL + goods_list[k];
			map<DWORD, ShopGoodsData_Config>::iterator iter = pShopGoodsList->find(dwFindId);
			if (iter == pShopGoodsList->end())
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到水晶商店商品[GoodsId:%d]!", __FUNCTION__, dwFindId);
				continue;
			}
			sConfig.vGoodsList.push_back(&iter->second);
		}
		CConfigManager::Instance()->m_CrystalShopSlotList.push_back(sConfig);
	}

	return true;
}

// 加载日常任务
bool LoadDailyMission(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, vector<WORD>> *pTypeMissionList = &CConfigManager::Instance()->m_DailyMissionTypeList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		DailyMission_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wMissionId = ReadUInt(tmp.data(), "id =");
		WORD wMissionType = ReadUInt(tmp.data(), "type =");
		sConfig.dwValue = ReadUInt(tmp.data(), "value =");
		sConfig.wPoint = ReadUInt(tmp.data(), "point =");
		sConfig.wNeedRoleLv = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.dwBaseExp = ReadUInt(tmp.data(), "base_exp =");
		if (!ReadCommonItemList(tmp.data(), "award_item_id =", "award_item_num =", sConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]日常任务奖励不匹配[Chapter:%d]!", __FUNCTION__, sConfig.wMissionId);
			return false;
		}
		CConfigManager::Instance()->m_DailyMissionList.insert(make_pair(sConfig.wMissionId, sConfig));

		map<DWORD, vector<WORD>>::iterator iter =pTypeMissionList->find(wMissionType);
		if (iter != pTypeMissionList->end())
		{
			iter->second.push_back(sConfig.wMissionId);
		}
		else
		{
			vector<WORD> vMissionIdList;
			vMissionIdList.push_back(sConfig.wMissionId);
			pTypeMissionList->insert(make_pair(wMissionType, vMissionIdList));
		}
	}

	return true;
}

// 加载角色等级对应的日常任务
bool LoadRoleLvDailyMission(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> point_list;
	vector<DWORD> point_award_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		RoleLvDailyMission_Config sConfig;
		string &tmp = data_list[i];
		WORD wLvMin = ReadUInt(tmp.data(), "level_min =");
		WORD wLvMax = ReadUInt(tmp.data(), "level_max =");
		//sConfig.wLevel = ReadUInt(tmp.data(), "need_role_level =");
		ReadArry1(tmp.c_str(), "mission_list", sConfig.vMissionList);
		ReadArry1(tmp.c_str(), "point_list", point_list);
		ReadArry1(tmp.c_str(), "point_award_list", point_award_list);	
		if (point_list.size() != point_award_list.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]日常任务积分与积分奖励配置不匹配!", __FUNCTION__);
			continue;
		}
		for (size_t k = 0; k < point_list.size(); ++k)
		{
			RoleLvDailyMission_Config::PointData tPoint;
			tPoint.wPoint = point_list[k];
			tPoint.wRewardItemId = point_award_list[k];
			sConfig.vPointList.push_back(tPoint);
		}

		for (WORD k = wLvMin; k <= wLvMax; ++k)
		{
			CConfigManager::Instance()->m_RoleLvDailyMissionList.insert(make_pair(k, sConfig));
		}
	}

	return true;
}


// 加载英雄天命配置
bool LoadHeroDestiny(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vCostItem;
	vector<DWORD> pro_type_list;
	vector<DWORD> pro_value_list;
	vector<vector<DWORD>> level_up_rate;
	for (int i = 0; i < data_list.size(); ++i)
	{
		HeroDestiny_Config sConfig;
		string &tmp = data_list[i];
		DWORD dwDestinyLevel = ReadUInt(tmp.data(), "level =");
		sConfig.dwNeedExp = ReadUInt(tmp.data(), "need_exp =");
		sConfig.dwAddExp = ReadUInt(tmp.data(), "add_exp =");
		ReadArry1(tmp.c_str(), "cost_item", vCostItem);
		if (2 != vCostItem.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]天命消息物品配置出错[Lv:%d]!", __FUNCTION__, dwDestinyLevel);
			continue;
		}
		sConfig.dwCostItemId = vCostItem[0];
		sConfig.wCostItemNum = vCostItem[1];
		sConfig.dwSkillId = ReadUInt(tmp.data(), "skill_level =");
		ReadArry1(tmp.c_str(), "pro_type_list", pro_type_list);
		ReadArry1(tmp.c_str(), "pro_value_list", pro_value_list);
		if (pro_type_list.size() != pro_value_list.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]天命属性配置不匹配[Lv:%d]!", __FUNCTION__, dwDestinyLevel);
			continue;
		}
		for (size_t k = 0; k < pro_type_list.size(); ++k)
		{
			Property tPro;
			tPro.wType = pro_type_list[k];
			tPro.nValue = pro_value_list[k];
			sConfig.vProList.push_back(tPro);
		}
		ReadArry2(tmp.c_str(), "level_up_rate =", level_up_rate);
		for (size_t k = 0; k < level_up_rate.size(); ++k)
		{
			if (level_up_rate[k].size() != 3)
			{
				MAPLOG(ERROR_LEVEL, "[%s]天命升级概率数据匹配出错[Lv:%d]!", __FUNCTION__, dwDestinyLevel);
				continue;
			}
			RateData tRateData;
			tRateData.dwMin = level_up_rate[k][0];
			tRateData.dwMax = level_up_rate[k][1];
			tRateData.nRate = level_up_rate[k][2];
			sConfig.vLevelUpRateList.push_back(tRateData);
		}

		CConfigManager::Instance()->m_HeroDestinyList.insert(make_pair(dwDestinyLevel, sConfig));
	}

	return true;
}

// 加载英雄副本章节配置
bool LoadHeroFBChapter(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		FubenChapter_Config sChapter;
		sChapter.Init();
		string &tmp = data_list[i];
		sChapter.dwChapter = ReadUInt(tmp.data(), "id =");
		sChapter.wExtraMapId = ReadUInt(tmp.data(), "extra_map_id =");
		ReadArry1(tmp.c_str(), "map_list =", sChapter.vMapList);
		CConfigManager::Instance()->m_HeroFBChapterList.insert(make_pair(sChapter.dwChapter, sChapter));
	}
	return true;
}

// 加载激活码配置
bool LoadActivationCode(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	char szKey[64];
	for (int i = 0; i < data_list.size(); ++i)
	{
		ActivationCode_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwId = ReadUInt(tmp.data(), "id =");
		tConfig.dwRewardId = ReadUInt(tmp.data(), "reward_id =");
		tConfig.strCode = ReadString(tmp.data(), "code = ");
		memset(szKey, 0, sizeof(szKey));
		ChangeLocalChar(tConfig.strCode.c_str(), szKey, sizeof(szKey)); // 防止出现中文,做转换
		CConfigManager::Instance()->m_ActivationCodeList.insert(make_pair(szKey, tConfig));
	}
	return true;
}

// 加载激活码奖励配置
bool LoadActivationRewardCode(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ActivationCodeReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wRewardId = ReadUInt(tmp.data(), "id =");
		tConfig.dwItemId = ReadUInt(tmp.data(), "item_id =");
		tConfig.nValidDate = ReadUInt(tmp.data(), "valid_date = ");
		tConfig.nValidTime = ReadUInt(tmp.data(), "valid_time = ");
		CConfigManager::Instance()->m_ActivationCodeRewardList.insert(make_pair(tConfig.wRewardId, tConfig));
	}
	return true;
}



// 加载热云统计配置
bool LoadStatisticCount(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ReyunStatistic_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wType = ReadUInt(tmp.data(), "type =");
		tConfig.strPath = ReadString(tmp.data(), "path = ");
		tConfig.strContext = ReadString(tmp.data(), "context = ");
		CConfigManager::Instance()->m_ReyunStatisticList.insert(make_pair(tConfig.wType, tConfig));
	}
	return true;
}

// 加载PVP战斗奖励配置
bool LoadPVPBattleReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		PVPBattleReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wBattleTimes = ReadUInt(tmp.data(), "times =");
		tConfig.wType = ReadUInt(tmp.data(), "type =");
		DWORD dwFindId = tConfig.wBattleTimes * 10 + tConfig.wType;
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]PVP战斗奖励不匹配[find_id:%d]!", __FUNCTION__, dwFindId);
			return false;
		}
		CConfigManager::Instance()->m_PVPBattleRewardList.insert(make_pair(dwFindId, tConfig));
	}
	return true;
}

// 加载PVP战斗奖励配置
bool LoadPVPRankReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		PVPRankReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wMinRank = ReadUInt(tmp.data(), "rank_min =");
		tConfig.wMaxRank = ReadUInt(tmp.data(), "rank_max =");
		tConfig.dwTitleId = ReadUInt(tmp.data(), "title_id =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]PVP排名奖励不匹配[min_rank:%d]!", __FUNCTION__, tConfig.wMinRank);
			return false;
		}
		for (WORD k = tConfig.wMinRank; k <= tConfig.wMaxRank; ++k)
		{
			CConfigManager::Instance()->m_PVPRankRewardList.insert(make_pair(k, tConfig));
		}
	}
	return true;
}

// 加载PVP活动时间
bool LoadPVPActivityTime(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vStartTimeList;
	vector<DWORD> vOverTimeList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		PVPActivityTime_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwWeekDay = ReadUInt(tmp.data(), "week_day =");
		ReadArry1(tmp.data(), "start_time_list =", vStartTimeList);
		ReadArry1(tmp.data(), "over_time_list =", vOverTimeList);
		if (vStartTimeList.size() != vOverTimeList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]PVP活动时间配置出错[week_day:%d]!", __FUNCTION__, tConfig.dwWeekDay);
			return false;
		}
		TimeData tTimeData;
		for (size_t k = 0; k < vStartTimeList.size(); k++)
		{
			tTimeData.nStart = vStartTimeList[k];
			tTimeData.nOver = vOverTimeList[k];
			tConfig.vTimeList.push_back(tTimeData);
		}
// 		tConfig.nStartTime = ReadUInt(tmp.data(), "start_time =");
// 		tConfig.nOverTime = ReadUInt(tmp.data(), "over_time =");
		CConfigManager::Instance()->m_PVPActivityTimeList.insert(make_pair(tConfig.dwWeekDay, tConfig));
	}
	return true;
}

// 加载攻城波数据配置
bool LoadAttackCityChapter(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vRefreshItemNumRange;
	vector<DWORD> vBossItemNumRange;
	for (int i = 0; i < data_list.size(); ++i)
	{
		AttackCityChapter_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wChapter = ReadUInt(tmp.data(), "chapter =");
		tConfig.byIsBoss = ReadUInt(tmp.data(), "is_boss =");
		tConfig.wResetInitChapter = ReadUInt(tmp.data(), "reset_init_chapter =");
		tConfig.wRefreshMonsterNum = ReadUInt(tmp.data(), "refresh_monster_num =");
		ReadArry1(tmp.c_str(), "map_id_list", tConfig.vMapList);
		ReadArry1(tmp.c_str(), "map_rate_list", tConfig.vMapRateList);
		if (tConfig.vMapList.size() != tConfig.vMapRateList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]怪物攻城关卡列表与概率配置不匹配[chatpter:%d,id_size:%d,rate_size:%d]!", 
				__FUNCTION__, tConfig.wChapter, tConfig.vMapList.size(), tConfig.vMapRateList.size());
			return false;
		}
		if (!ReadCommonPropertyList(tmp.data(), "add_pro_type_list =", "add_pro_value_list =", tConfig.vMonsterProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]攻城怪物属性配置不匹配[chapter:%d]!", __FUNCTION__, tConfig.wChapter);
			return false;
		}
		ReadArry1(tmp.c_str(), "refresh_item_num_range", vRefreshItemNumRange);
		if (2 != vRefreshItemNumRange.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]怪物攻城刷宝箱数量范围配置不匹配[wave_num:%d]!", __FUNCTION__, tConfig.wChapter);
			return false;
		}


		tConfig.wRefreshItemNumMin = vRefreshItemNumRange[0];
		tConfig.wRefreshItemNumMax = vRefreshItemNumRange[1];
		tConfig.dwRefreshItemId = ReadUInt(tmp.data(), "refresh_item_id =");

		tConfig.dwBossItemId = ReadUInt(tmp.data(), "boss_item_id =");
		ReadArry1(tmp.c_str(), "boss_item_num_range", vBossItemNumRange);
		if (2 == vBossItemNumRange.size())
		{
			tConfig.wBossItemNumMin = vBossItemNumRange[0];
			tConfig.wBossItemNumMax = vBossItemNumRange[1];
		}
		else
		{
			tConfig.wBossItemNumMin = 0;
			tConfig.wBossItemNumMax = 0;
// 			MAPLOG(ERROR_LEVEL, "[%s]boss宝箱数量范围配置不匹配[chatpter:%d]!", __FUNCTION__, tConfig.wChapter);
// 			return false;
		}
		
		tConfig.dwBattleScore = ReadUInt(tmp.data(), "battle_score =");
		tConfig.dwKillScore = ReadUInt(tmp.data(), "kill_score =");
		if (!ReadCommonPropertyList(tmp.data(), "battle_reward_currency_type_list =", "battle_reward_currency_value_list =", tConfig.vBattleMoneyList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]攻城挑战奖励货币配置不匹配[chapter:%d]!", __FUNCTION__, tConfig.wChapter);
			return false;
		}
		if (!ReadCommonItemList(tmp.data(), "kill_reward_item_id_list =", "kill_reward_item_num_list =", tConfig.vKillRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]攻城怪击杀奖励物品配置不匹配[chapter:%d]!", __FUNCTION__, tConfig.wChapter);
			return false;
		}
		CConfigManager::Instance()->m_AttackCityChapterList.insert(make_pair(tConfig.wChapter, tConfig));
	}
	return true;
}


// 加载攻城BOSS奖励
bool LoadAttackCityBossReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		AttackCityBossReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wChapter = ReadUInt(tmp.data(), "chapter =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]攻城BOSS奖励物品配置不匹配[chapter:%d]!", __FUNCTION__, tConfig.wChapter);
			return false;
		}
		CConfigManager::Instance()->m_AttackCityBossRewardList.insert(make_pair(tConfig.wChapter, tConfig));
	}
	return true;
}

bool LoadAttackCityWeak(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		AttackCityWeak_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwHPRemainRateMin = ReadUInt(tmp.data(), "hp_remain_rate_min =");
		tConfig.dwHPRemainRateMax = ReadUInt(tmp.data(), "hp_remain_rate_max =");
		tConfig.wWeakCD = ReadUInt(tmp.data(), "weak_cd =");
		CConfigManager::Instance()->m_AttackCityWeakList.push_back(tConfig);
	}
	return true;
}



// 加载攻城BOSS奖励
bool LoadAttackCityEncourage(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		AttackCityEncourage_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wTimes = ReadUInt(tmp.data(), "times =");
		tConfig.tCostData.wType = ReadUInt(tmp.data(), "cost_type =");
		tConfig.tCostData.nValue = ReadUInt(tmp.data(), "cost_value =");
		tConfig.tAddPro.wType = ReadUInt(tmp.data(), "pro_type =");
		tConfig.tAddPro.nValue = ReadUInt(tmp.data(), "pro_value =");
		tConfig.nScoreAddRate = ReadUInt(tmp.data(), "score_add_rate =");
		CConfigManager::Instance()->m_AttackCityEncourageList.insert(make_pair(tConfig.wTimes, tConfig));
	}
	return true;
}

// 加载攻城活动时间配置
bool LoadAttackCityActivityTime(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		AttackCityActivityTime_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wWeekDay = ReadUInt(tmp.data(), "week_day =");
		tConfig.nStartTime = ReadUInt(tmp.data(), "start_time =");
		tConfig.nOverTime = ReadUInt(tmp.data(), "over_time =");
		CConfigManager::Instance()->m_AttackCityActivityTimeList.insert(make_pair(tConfig.wWeekDay, tConfig));
	}
	return true;
}

// 加载攻城排名奖励配置
bool LoadAttackCityRankReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		AttackCityRankReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wRankMin = ReadUInt(tmp.data(), "rank_min =");
		tConfig.wRankMax = ReadUInt(tmp.data(), "rank_max =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]攻城排名奖励物品配置不匹配!", __FUNCTION__);
			return false;
		}
		for (WORD i = tConfig.wRankMin; i <= tConfig.wRankMax; ++i)
		{
			CConfigManager::Instance()->m_AttackCityRankRewardList.insert(make_pair(i, tConfig));
		}
	}
	return true;
}

// 加载无尺之地碰撞配置
bool LoadEndlessLandCollision(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		EndlessLandCollision_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wRewardId = ReadUInt(tmp.data(), "id =");
		tConfig.dwRate = ReadUInt(tmp.data(), "rate =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]无尺之地碰撞奖励物品配置不匹配!", __FUNCTION__);
			return false;
		}
		CConfigManager::Instance()->m_EndlessLandCollisionList.push_back(tConfig);
	}
	return true;
}


// 加载无尺之地连杀奖励配置
bool LoadEndlessLandContinueKillReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		EndlessLandContinueKillReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwContinueKillNum = ReadUInt(tmp.data(), "keep_kill_num =");
		tConfig.nDiamond = ReadUInt(tmp.data(), "diamond =");
		CConfigManager::Instance()->m_EndlessLandContinueKillRewardList.insert(make_pair(tConfig.dwContinueKillNum, tConfig));
	}
	return true;
}

// 加载无尺之地波配置
bool LoadEndlessLandWave(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vRateList;
	vector<DWORD> vDropOjbIdList;
	vector<DWORD> vDropOjbNumList;
	DropData tDropData;
	for (int i = 0; i < data_list.size(); ++i)
	{
		EndlessLandWave_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wWave = ReadUInt(tmp.data(), "wave =");
		tConfig.wInitWave = ReadUInt(tmp.data(), "init_wave =");
		tConfig.wMonsterNum = ReadUInt(tmp.data(), "monster_num =");
		tConfig.nEndlessStone = ReadUInt(tmp.data(), "endless_stone =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", tConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]无尺之地连杀奖励物品配置不匹配!", __FUNCTION__);
			return false;
		}
		ReadArry1(tmp.data(), "monster_id_list =", tConfig.vMonsterList);

		ReadArry1(tmp.data(), "drop_item_rate_list =", vRateList);
		ReadArry1(tmp.data(), "drop_item_id_list =", vDropOjbIdList);
		ReadArry1(tmp.data(), "drop_item_num_list =", vDropOjbNumList);
		if (vRateList.size() != vDropOjbIdList.size() || vRateList.size() != vDropOjbNumList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]无尺之地掉落配置不匹配!", __FUNCTION__);
			return false;
		}
		for (size_t i = 0; i < vRateList.size(); ++i)
		{
			tDropData.dwDropObjectId = vDropOjbIdList[i];
			tDropData.wDropObjectNum = vDropOjbNumList[i];
			tDropData.wRate = vRateList[i];
			tConfig.vDropDataList.push_back(tDropData);
		}

		CConfigManager::Instance()->m_EndlessLandWaveList.insert(make_pair(tConfig.wWave, tConfig));
	}
	return true;
}

// 加载无尺之地商店
bool LoadEndlessLandShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	ShopGoodsData_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.nNeedVipLevel = ReadUInt(tmp.data(), "need_vip_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		sConfig.nContext = ReadUInt(tmp.data(), "need_kill_monster_num =");
		int nGoodsType = ReadUInt(tmp.data(), "goods_type =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]无尽之地商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		AddShopGoods(nGoodsType == 1 ? SHOP_ENDLESS_LAND_ITEM : SHOP_ENDLESS_LAND_REWARD, sConfig);
	}
	return true;
}

// 加载无尺之地排行奖励
bool LoadEndlessLandRankReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		EndlessLandRankReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wRankMin = ReadUInt(tmp.data(), "rank_min =");
		tConfig.wRankMax = ReadUInt(tmp.data(), "rank_max =");
		tConfig.dwTitleId = ReadUInt(tmp.data(), "title_id =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]攻城排名奖励物品配置不匹配!", __FUNCTION__);
			return false;
		}
		for (WORD i = tConfig.wRankMin; i <= tConfig.wRankMax; ++i)
		{
			CConfigManager::Instance()->m_EndlessLandRankRewardList.insert(make_pair(i, tConfig));
		}
	}
	return true;
}

// 加载无尺之地排行奖励
bool LoadEndlessLandBattleCost(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		EndlessLandBattleCost_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wBattleTimes = ReadUInt(tmp.data(), "battle_times =");
		tConfig.tCostCurrency.wType = ReadUInt(tmp.data(), "cost_currency_type =");
		tConfig.tCostCurrency.nValue = ReadUInt(tmp.data(), "cost_currency_value =");
		tConfig.tCostItem.dwId = ReadUInt(tmp.data(), "cost_item_id =");
		tConfig.tCostItem.dwNum = ReadUInt(tmp.data(), "cost_item_num =");
		CConfigManager::Instance()->m_EndlessLandBattleCostList.insert(make_pair(tConfig.wBattleTimes, tConfig));
	}
	return true;
}



// 加载爵位配置
bool LoadNobility(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Nobility_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wLevel = ReadUInt(tmp.data(), "level =");
		tConfig.dwNeedPower = ReadUInt(tmp.data(), "need_power =");
		if (!ReadCommonPropertyList(tmp.data(), "pro_type_list =", "pro_value_list =", tConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]爵位属性配置不匹配!", __FUNCTION__);
			return false;
		}

		CConfigManager::Instance()->m_NobilityList.insert(make_pair(tConfig.wLevel, tConfig));

	}
	return true;
}

bool LoadLuxurySignIn(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<WORD, map<WORD, LuxurySign_Config>> &luxurySignInList = CConfigManager::Instance()->m_LuxurySignInList;
	vector<DWORD> reward_id_list;
	vector<DWORD> reward_rate_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		LuxurySign_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wSignInId = ReadUInt(tmp.data(), "id =");
		sConfig.wLevelMin = ReadUInt(tmp.data(), "level_min =");
		sConfig.wLevelMax = ReadUInt(tmp.data(), "level_max =");
		sConfig.wRechangeId = ReadUInt(tmp.data(), "recharge_id =");
		ReadArry1(tmp.data(), "reward_id_list =", reward_id_list);
		ReadArry1(tmp.data(), "reward_rate_list =", reward_rate_list);
		if (reward_id_list.size() != reward_rate_list.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s] 豪华签到与概率配置不匹配[sign_id:%d]!", __FUNCTION__, sConfig.wSignInId);
			return false;
		}
		for (size_t k = 0; k < reward_id_list.size(); ++k)
		{
			RateDataEx tRateData;
			tRateData.nRate = reward_rate_list[k]; 
			tRateData.nValue = reward_id_list[k];
			sConfig.vRewardRandList.push_back(tRateData);
		}

		map<WORD, map<WORD, LuxurySign_Config>>::iterator iter;
		iter = luxurySignInList.find(sConfig.wSignInId);
		if (iter == luxurySignInList.end())
		{
			luxurySignInList.insert(make_pair(sConfig.wSignInId, map<WORD, LuxurySign_Config>()));
			iter = luxurySignInList.find(sConfig.wSignInId);
		}

		for (WORD i = sConfig.wLevelMin; i <= sConfig.wLevelMax; ++i)
		{
			iter->second.insert(make_pair(i, sConfig));
		}
	}

	return true;
}

// 加载豪华签到奖励配置
bool LoadLuxurySignInReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		LuxurySignInReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwRewardId = ReadUInt(tmp.data(), "id =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]豪华签到奖励物品配置不匹配!", __FUNCTION__);
			return false;
		}
		CConfigManager::Instance()->m_LuxurySignInRewardList.insert(make_pair(tConfig.dwRewardId, tConfig));
	}
	return true;
}

// 加载超值折扣购买奖励配置
bool LoadSuperDiscountBuyReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		SuperDiscountBuyReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wDay = ReadUInt(tmp.data(), "day =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]超值折扣奖励物品配置不匹配!", __FUNCTION__);
			return false;
		}
		CConfigManager::Instance()->m_SuperDiscountBuyRewardList.insert(make_pair(tConfig.wDay, tConfig));
	}
	return true;
}

// 加载超值折扣累积奖励配置
bool LoadSuperDiscountAccumulatReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		SuperDiscountAccumulateReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wTimes = ReadUInt(tmp.data(), "times =");
		tConfig.wNeedBuyTimes = ReadUInt(tmp.data(), "need_buy_times =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]超值折扣累积奖励物品配置不匹配!", __FUNCTION__);
			return false;
		}
		CConfigManager::Instance()->m_SuperDiscountAccumulateRewardList.insert(make_pair(tConfig.wTimes, tConfig));
	}
	return true;
}

// 加载超值折扣购买奖励配置(新)
bool LoadNewSuperDiscountBuyReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		SuperDiscountBuyReward_Config tConfig;
		string &tmp = data_list[i];
		WORD wDataId = ReadUInt(tmp.data(), "data_id =");
		tConfig.wDay = ReadUInt(tmp.data(), "day =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]超值折扣奖励物品配置不匹配!", __FUNCTION__);
			return false;
		}
		DWORD dwKey = 10000 * wDataId + tConfig.wDay;
		CConfigManager::Instance()->m_NewSuperDiscountBuyRewardList.insert(make_pair(dwKey, tConfig));
	}
	return true;
}

// 加载超值折扣累积奖励配置(新)
bool LoadNewSuperDiscountAccumulatReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		SuperDiscountAccumulateReward_Config tConfig;
		string &tmp = data_list[i];
		WORD wDataId = ReadUInt(tmp.data(), "data_id =");
		tConfig.wTimes = ReadUInt(tmp.data(), "times =");
		tConfig.wNeedBuyTimes = ReadUInt(tmp.data(), "need_buy_times =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]超值折扣累积奖励物品配置不匹配!", __FUNCTION__);
			return false;
		}
		DWORD dwKey = 10000 * wDataId + tConfig.wTimes;
		CConfigManager::Instance()->m_NewSuperDiscountAccumulateRewardList.insert(make_pair(dwKey, tConfig));
	}
	return true;
}

// 加载7天排行活动奖励配置
bool LoadSevenDayRankReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		RoleRankReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wRankType = ReadUInt(tmp.data(), "rank_type =");
		tConfig.wRankMin = ReadUInt(tmp.data(), "rank_min =");
		tConfig.wRankMax = ReadUInt(tmp.data(), "rank_max =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]7天排行活动奖励配置不匹配!", __FUNCTION__);
			return false;
		}
		for (WORD i = tConfig.wRankMin; i <= tConfig.wRankMax; ++i)
		{
			DWORD dwKey = 100000 * tConfig.wRankType + i;
			CConfigManager::Instance()->m_SevenDayRankRewardList.insert(make_pair(dwKey, tConfig));
		}
	}
	return true;
}

// 加载直购商店
bool LoadDirectBuyShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wRefreshRate = ReadUInt(tmp.data(), "rate =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		sConfig.nContext = ReadUInt(tmp.data(), "recharge_id =");
		sConfig.wRefreshType = ReadUInt(tmp.data(), "refresh_type =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]直购商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_DIRECT_BUY, sConfig);
	}

	return true;
}

// 加载兑换商店
bool LoadExchangeShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	ShopGoodsData_Config sConfig;
	for (int i = 0; i < data_list.size(); ++i)
	{
		sConfig.Init();
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.nNeedVipLevel = ReadUInt(tmp.data(), "need_vip_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		sConfig.nContext = ReadUInt(tmp.data(), "need_kill_monster_num =");
		int nGoodsType = ReadUInt(tmp.data(), "goods_type =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]兑换商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		if (!ReadCommonItemList(tmp.c_str(), "cost_item_id_list =", "cost_item_num_list =", sConfig.vCostItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]兑换商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}	
		AddShopGoods(SHOP_EXCHANGE, sConfig);
	}
	return true;
}

// 加载推荐商店
bool LoadRecommendShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wRefreshRate = ReadUInt(tmp.data(), "rate =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		sConfig.nContext = ReadUInt(tmp.data(), "recharge_id =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]推荐商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_RECOMMEND, sConfig);
	}
	return true;
}

// 加载组队商店
bool LoadTeamDungeonShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.byType = ReadUInt(tmp.data(), "goods_type =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		sConfig.nContext = ReadUInt(tmp.data(), "need_pass_dungeonid =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList, "cost_type_list =", "cost_value_list ="))
		{
			MAPLOG(ERROR_LEVEL, "[%s]商店消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		BYTE byShopType = sConfig.byType == 1 ? SHOP_TEAM_DUNGEON_NORMAL : SHOP_TEAM_DUNGEON_REWARD;
		AddShopGoods(byShopType, sConfig);
	}
	return true;
}

// 加载今日推荐充值
bool LoadTodayRecommendRecharge(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<WORD, vector<RateDataEx>> *pRandList = &CConfigManager::Instance()->m_TodayRecommendRechargeRandList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		TodayRecommendRecharge_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wGiftId = ReadUInt(tmp.data(), "id =");
		tConfig.wRechargeId = ReadUInt(tmp.data(), "recharge_id =");
		tConfig.wVipLevel = ReadUInt(tmp.data(), "vip_level =");
		tConfig.nRate = ReadUInt(tmp.data(), "rate =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]今日推荐充值奖励配置不匹配!", __FUNCTION__);
			return false;
		}
		CConfigManager::Instance()->m_TodayRecommendRechargeList.insert(make_pair(tConfig.wGiftId, tConfig));

		// 随机列表
		RateDataEx rateData;
		rateData.nRate = tConfig.nRate;
		rateData.nValue = tConfig.wGiftId;
		map<WORD, vector<RateDataEx>>::iterator iter = pRandList->find(tConfig.wVipLevel);
		if (iter != pRandList->end())
		{
			iter->second.push_back(rateData);
		}
		else
		{
			vector<RateDataEx> vRateDataList;
			vRateDataList.push_back(rateData);
			pRandList->insert(make_pair(tConfig.wVipLevel, vRateDataList));
		}
	}
	return true;
}

// 加载节日活动列表
bool LoadFestivalActivity(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		FestivalActivity_Config sConfig;
		string &tmp = data_list[i];
		sConfig.nActivityType = ReadUInt(tmp.data(), "activity_type =");
		sConfig.nTimeType = ReadUInt(tmp.data(), "time_type =");
		sConfig.nDays = ReadUInt(tmp.data(), "days =");
		ReadArry1(tmp.data(), "server_list =", sConfig.vOpenServerList);
		sConfig.nStartDate = ReadUInt(tmp.data(), "star_date =");
		sConfig.nStartTime = ReadUInt(tmp.data(), "star_time =");
		sConfig.nOverDate = ReadUInt(tmp.data(), "over_date =");
		sConfig.nOverTime = ReadUInt(tmp.data(), "over_time =");
		sConfig.nRewardOverDate = ReadUInt(tmp.data(), "reward_over_date =");
		sConfig.nRewardOverTime = ReadUInt(tmp.data(), "reward_over_time =");
		ReadArry1(tmp.data(), "param_list =", sConfig.vParamList);
		map<DWORD, vector<FestivalActivity_Config>> *pActivityList = &CConfigManager::Instance()->m_FestivalActivityList;
		map<DWORD, vector<FestivalActivity_Config>>::iterator iter = pActivityList->find(sConfig.nActivityType);
		if (iter != pActivityList->end())
		{
			iter->second.push_back(sConfig);
		}
		else
		{
			vector<FestivalActivity_Config> vList;
			vList.push_back(sConfig);
			pActivityList->insert(make_pair(sConfig.nActivityType, vList));
		}
	}

	return true;
}

// 加载节日活动目标
bool LoadFestivalActivtiyTarget(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<WORD, vector<WORD>> *pSameLogicTargetList = &CConfigManager::Instance()->m_FestivalActivitySameLogicTargetList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		FestivalActivtiyTarget_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wTargetId = ReadUInt(tmp.data(), "id =");
		sConfig.wLogicType = ReadUInt(tmp.data(), "logic_type =");
		sConfig.byIsAsc = ReadUInt(tmp.data(), "is_asc =");
		sConfig.nX = ReadUInt(tmp.data(), "x =");
		sConfig.nY = ReadUInt(tmp.data(), "y =");
		sConfig.wRewardTimes = ReadUInt(tmp.data(), "reward_times =");
		sConfig.byIsDailyReset = ReadUInt(tmp.data(), "is_daily_reset =");
		sConfig.byIsStageTarget = ReadUInt(tmp.data(), "is_stage_target =");
		sConfig.byIsNeedTake = ReadUInt(tmp.data(), "is_need_take =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", sConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]节日活动目标奖励配置不匹配[TargetId:%d]!", __FUNCTION__, sConfig.wTargetId);
			continue;
		}
		CConfigManager::Instance()->m_FestivalActivtiyTargetList.insert(make_pair(sConfig.wTargetId, sConfig));

		map<WORD, vector<WORD>>::iterator iter = pSameLogicTargetList->find(sConfig.wLogicType);
		if (iter != pSameLogicTargetList->end())
		{
			iter->second.push_back(sConfig.wTargetId);
		}
		else
		{
			vector<WORD> vIdList;
			vIdList.push_back(sConfig.wTargetId);
			pSameLogicTargetList->insert(make_pair(sConfig.wLogicType, vIdList));
		}
	}

	return true;
}

// 加载精准推荐商品
bool LoadAccurateRecommendGoods(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		AccurateRecommendGoods_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wGoodsId = ReadUInt(tmp.data(), "id =");
		tConfig.dwItemId = ReadUInt(tmp.data(), "item_id =");
		tConfig.dwItemNum = ReadUInt(tmp.data(), "item_num =");
		tConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_max_num =");
		tConfig.wRechargeId = ReadUInt(tmp.data(), "recharge_id =");
		CConfigManager::Instance()->m_AccurateRecommendGoodsList.insert(make_pair(tConfig.wGoodsId, tConfig));
	}
	return true;
}

// 加载宝石配置
bool LoadGem(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Gem_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwGemId = ReadUInt(tmp.data(), "id =");
		tConfig.wType = ReadUInt(tmp.data(), "type =");
		tConfig.wComposeCostNum = ReadUInt(tmp.data(), "compose_cost_num =");
		tConfig.dwNewGemFromCompose = ReadUInt(tmp.data(), "new_gem_from_compose =");
		tConfig.nGemCoin = ReadUInt(tmp.data(), "gem_coin =");
		tConfig.wSkillId = ReadUInt(tmp.data(), "skill_id =");
		tConfig.wSkillLevel = ReadUInt(tmp.data(), "skill_level =");
		if (!ReadCommonPropertyList(tmp.data(), "init_pro_type_list =", "init_pro_value_list =", tConfig.vInitProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]宝石属性配置不匹配!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_GemList.insert(make_pair(tConfig.dwGemId, tConfig));
	}
	return true;
}

// 加载宝石技能配置
bool LoadGemSkill(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		GemSkill_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wSkillId = ReadUInt(tmp.data(), "id =");
		tConfig.wSkillLevel = ReadUInt(tmp.data(), "level =");
		if (!ReadCommonPropertyList(tmp.data(), "pro_type_list =", "pro_value_list =", tConfig.vProList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]宝石属性配置不匹配!", __FUNCTION__);
			continue;
		}
		DWORD dwKey = tConfig.wSkillId * 100 + tConfig.wSkillLevel;
		CConfigManager::Instance()->m_GemSkillList.insert(make_pair(dwKey, tConfig));
	}
	return true;
}

// 加载宝石孔位配置
bool LoadGemHole(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		GemHole_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wHole = ReadUInt(tmp.data(), "hole =");
		tConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "role_level =");
		tConfig.wIsDefaultOpen = ReadUInt(tmp.data(), "is_default_open =");
		tConfig.vCostItem.dwId = ReadUInt(tmp.data(), "cost_item_id =");
		tConfig.vCostItem.dwNum = ReadUInt(tmp.data(), "cost_item_num =");
		CConfigManager::Instance()->m_GemHoleList.insert(make_pair(tConfig.wHole, tConfig));
	}
	return true;
}

// 加载宝石镶嵌配置
bool LoadGemInlay(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		GemInlay_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wType = ReadUInt(tmp.data(), "type =");
		tConfig.wInlayNumLimit = ReadUInt(tmp.data(), "inlay_num_limit =");
		CConfigManager::Instance()->m_GemInlayList.insert(make_pair(tConfig.wType, tConfig));
	}
	return true;
}

// 加载场景配置
bool LoadScene(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Scene_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwSceneId = ReadUInt(tmp.data(), "scene_id =");
		tConfig.dwMapId = ReadUInt(tmp.data(), "map_id =");
		tConfig.byIsSyncMove = ReadUInt(tmp.data(), "is_sync_move =");
		ReadArry1(tmp.data(), "move_area =", tConfig.vMoveArea);
		ReadArry1(tmp.data(), "hero_birth_area =", tConfig.vHeroBirthArea);
		ReadArry1(tmp.data(), "hero_relive_area =", tConfig.vHeroReliveArea);
		ReadArry1(tmp.data(), "monster_refresh_area =", tConfig.vMonsterRefrshArea);
		ReadArry1(tmp.data(), "item_refresh_area =", tConfig.vItemRefrshArea);

		if (tConfig.vMoveArea.size() && tConfig.vMoveArea.size() != 4)
		{
			MAPLOG(ERROR_LEVEL, "[%s]场景可移动区域配置出错[SceneId:%d]!", __FUNCTION__, tConfig.dwSceneId);
			return false;
		}
		if (tConfig.vHeroBirthArea.size() && tConfig.vHeroBirthArea.size() != 4)
		{
			MAPLOG(ERROR_LEVEL, "[%s]场景英雄出生区域配置出错[SceneId:%d]!", __FUNCTION__, tConfig.dwSceneId);
			return false;
		}
		if (tConfig.vHeroReliveArea.size() && tConfig.vHeroReliveArea.size() != 4)
		{
			MAPLOG(ERROR_LEVEL, "[%s]场景英雄复活区域配置出错[SceneId:%d]!", __FUNCTION__, tConfig.dwSceneId);
			return false;
		}
		if (tConfig.vMonsterRefrshArea.size() && tConfig.vMonsterRefrshArea.size() != 4)
		{
			MAPLOG(ERROR_LEVEL, "[%s]场景怪物刷新区域配置出错[SceneId:%d]!", __FUNCTION__, tConfig.dwSceneId);
			return false;
		}
		if (tConfig.vItemRefrshArea.size() && tConfig.vItemRefrshArea.size() != 4)
		{
			MAPLOG(ERROR_LEVEL, "[%s]场景物品刷新区域配置出错[SceneId:%d]!", __FUNCTION__, tConfig.dwSceneId);
			return false;
		}

		CConfigManager::Instance()->m_SceneList.insert(make_pair(tConfig.dwSceneId, tConfig));
	}
	return true;
}



// 加载登录奖励配置
bool LoadLoginReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		LoginReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wDays = ReadUInt(tmp.data(), "days =");	
		if (!ReadCommonItemList(tmp.data(), "reward_item_id =", "reward_item_num =", tConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]登录奖励活动奖励配置不匹配!", __FUNCTION__);
			continue;
		}
		CConfigManager::Instance()->m_LoginRewardList.insert(make_pair(tConfig.wDays, tConfig));
	}
	return true;
}

// 加载组队副本抽奖配置
bool LoadTeamDungeonLottery(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		TeamDungeonLottery_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwItemId = ReadUInt(tmp.data(), "item_id =");
		tConfig.dwItemNum = ReadUInt(tmp.data(), "item_num =");
		tConfig.nWeight = ReadUInt(tmp.data(), "weight =");
		tConfig.byIsNeedRecord = ReadUInt(tmp.data(), "is_need_record =");
		CConfigManager::Instance()->m_TeamDungeonLotteryList.push_back(tConfig);
	}
	return true;
}

// 加载组队副本配置
bool LoadTeamDungeon(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vRecommendLevelList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		TeamDungeon_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwDungeonId = ReadUInt(tmp.data(), "dungeon_id =");
		tConfig.dwWorldMapId = ReadUInt(tmp.data(), "map_id =");
		ReadArry1(tmp.data(), "recommend_level =", vRecommendLevelList);
		if (vRecommendLevelList.size() != 2)
		{
			MAPLOG(ERROR_LEVEL, "[%s]组队副本推荐等级配置出错!", __FUNCTION__);
			return false;
		}
		tConfig.wRecommendLevelMin = vRecommendLevelList[0];
		tConfig.wRecommendLevelMax = vRecommendLevelList[1];
		ReadArry1(tmp.data(), "task_list =", tConfig.vTaskList);
		CConfigManager::Instance()->m_TeamDungeonList.insert(make_pair(tConfig.dwDungeonId, tConfig));
	}
	return true;
}

// 加载组队副本任务链配置
bool LoadTeamDungeonTaskLink(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vBossMapPos;
	for (int i = 0; i < data_list.size(); ++i)
	{
		TeamDungeonTaskLink_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwTaskListId = ReadUInt(tmp.data(), "id =");
		tConfig.dwBossMapId = ReadUInt(tmp.data(), "map_id =");
		//tConfig.dwBossKillRewardId = ReadUInt(tmp.data(), "boss_kill_reward_id =");
		tConfig.dwTaskLimitTime = ReadUInt(tmp.data(), "task_time =");		
		//tConfig.dwMVPRewardId = ReadUInt(tmp.data(), "mvp_reward_id =");
		tConfig.wTaskNum = ReadUInt(tmp.data(), "task_num =");
		ReadArry1(tmp.data(), "boss_scene_pos =", vBossMapPos);
		if (vBossMapPos.size() != 2)
		{
			MAPLOG(ERROR_LEVEL, "[%s]组队副本Boss地图坐标配置出错[TaskListId:%d]!", __FUNCTION__, tConfig.dwTaskListId);
			return false;
		}
		tConfig.tBossScenePos.wMapX = vBossMapPos[0];
		tConfig.tBossScenePos.wMapY = vBossMapPos[1];
		ReadArry2(tmp.data(), "task_list =", tConfig.vTaskList);
		if (tConfig.wTaskNum != tConfig.vTaskList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]组队副本任务链数据配置不匹配!", __FUNCTION__);
			return false;
		}
		ReadArry1(tmp.data(), "boss_kill_reward_list =", tConfig.vBossKillRewardList);
		ReadArry1(tmp.data(), "mvp_reward_list =", tConfig.vMVPRewardList);
		CConfigManager::Instance()->m_TeamDungeonTaskLinkList.insert(make_pair(tConfig.dwTaskListId, tConfig));
	}
	return true;
}

// 加载组队副本任务配置
bool LoadTeamDungeonTask(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		TeamDungeonTask_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwTaskId = ReadUInt(tmp.data(), "id =");
		tConfig.wTaskType = ReadUInt(tmp.data(), "type =");
		tConfig.dwSceneId = ReadUInt(tmp.data(), "scene_id =");
		tConfig.wTaskNpc = ReadUInt(tmp.data(), "task_npc =");
		tConfig.dwExtraRewardId = ReadUInt(tmp.data(), "extra_reward_id =");
		ReadArry1(tmp.data(), "task_param1 =", tConfig.vTaskParam);
		ReadArry1(tmp.data(), "score_list =", tConfig.vScoreList);
		ReadArry1(tmp.data(), "energy_list =", tConfig.vEnergyList);
		ReadArry1(tmp.data(), "reward_item_list =", tConfig.vRewardItemList);
		if (tConfig.vScoreList.size() != tConfig.vEnergyList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]组队副本任务评分与能量数据配置不匹配!", __FUNCTION__);
			return false;
		}
		if (tConfig.vEnergyList.size() != tConfig.vRewardItemList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]组队副本任务能量与奖励数据配置不匹配!", __FUNCTION__);
			return false;
		}
		CConfigManager::Instance()->m_TeamDungeonTaskList.insert(make_pair(tConfig.dwTaskId, tConfig));
	}
	return true;
}

// 加载组队Boss奖励配置
bool LoadTeamDungeonBossReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, vector<TeamDungeonBossReward_Config>> *pBossRewardList = &CConfigManager::Instance()->m_TeamDungeonBossRewardList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		TeamDungeonBossReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwDungeonId = ReadUInt(tmp.data(), "dungeon_id =");
		tConfig.nEnergyMin = ReadUInt(tmp.data(), "energy_min =");
		tConfig.nEnergyMax = ReadUInt(tmp.data(), "energy_max =");
		tConfig.nDamageRateMin = ReadUInt(tmp.data(), "damage_rate_min =");
		tConfig.nDamageRateMax = ReadUInt(tmp.data(), "damage_rate_max =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]组队副本Boss奖励配置不匹配[DungeonId:%d]!", __FUNCTION__, tConfig.dwDungeonId);
			continue;
		}
		map<DWORD, vector<TeamDungeonBossReward_Config>>::iterator iter = pBossRewardList->find(tConfig.dwDungeonId);
		if (iter != pBossRewardList->end())
		{
			iter->second.push_back(tConfig);
		}
		else
		{
			vector<TeamDungeonBossReward_Config> vList;
			vList.push_back(tConfig);
			pBossRewardList->insert(make_pair(tConfig.dwDungeonId, vList));
		}
	}
	return true;
}


// 加载工会建筑配置
bool LoadGuildBuilding(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		GuildBuilding_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wBuildingId = ReadUInt(tmp.data(), "id =");
		tConfig.wNeedGuildLevel = ReadUInt(tmp.data(), "need_guild_level =");
		CConfigManager::Instance()->m_GuildBuildingList.insert(make_pair(tConfig.wBuildingId, tConfig));
	}
	return true;
}

// 加载封测返利配置
bool LoadClosedTestRebate(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ClosedTestRebate_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wId = ReadUInt(tmp.data(), "id =");
		tConfig.dwChannel = ReadUInt(tmp.data(), "channel =");
		tConfig.nDiamond = ReadUInt(tmp.data(), "diamond =");
		tConfig.strAcount = ReadString(tmp.data(), "account = "); 
		CConfigManager::Instance()->m_ClosedTestRebateList.insert(make_pair(tConfig.strAcount, tConfig));
	}
	return true;
}

// 加载帐号白名单
bool LoadAccountWhite(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	string strAccount;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		strAccount = ReadString(tmp.data(), "account = ");
		CConfigManager::Instance()->m_AccountWhiteList.insert(make_pair(strAccount, strAccount));
	}
	return true;
}

bool LoadIPWhite(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	string strAccount;
	for (int i = 0; i < data_list.size(); ++i)
	{
		string &tmp = data_list[i];
		strAccount = ReadString(tmp.data(), "ip = ");
		CConfigManager::Instance()->m_IPWhiteList.insert(make_pair(strAccount, strAccount));
	}
	return true;
}

// 加载王者赛段位
bool LoadKingMatchStage(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vMatchingStageLevelList;
	vector<DWORD> vMatchingStageRateList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		KingMatchStage_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wStageLevel = ReadUInt(tmp.data(), "stage_level =");
		tConfig.wLevelUpNeedStar = ReadUInt(tmp.data(), "level_up_need_star =");
		tConfig.byIsDropStage = ReadUInt(tmp.data(), "is_drop_stage =");
		tConfig.byIsDropStar = ReadUInt(tmp.data(), "is_drop_star =");
		tConfig.byIsLastStage = ReadUInt(tmp.data(), "is_the_last_stage =");
		tConfig.byIsBroadcastLeveUp = ReadUInt(tmp.data(), "is_broadcast_leve_up =");	
		ReadArry1(tmp.data(), "matching_stage_level_list =", vMatchingStageLevelList);
		ReadArry1(tmp.data(), "matching_stage_rate_list =", vMatchingStageRateList);
		if (vMatchingStageLevelList.size() != vMatchingStageRateList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]王者赛段位等级与几率数据配置不匹配[StageLevel:%d]!", __FUNCTION__, tConfig.wStageLevel);
			return false;
		}
		for (size_t i = 0; i < vMatchingStageLevelList.size(); ++i)
		{
			RateDataEx rateData;
			rateData.nRate = vMatchingStageRateList[i];
			rateData.nValue = vMatchingStageLevelList[i];
			tConfig.vMatchingStageList.push_back(rateData);
		}
		if (!ReadCommonItemList(tmp.data(), "promotion_reward_id_list =", "promotion_reward_num_list =", tConfig.vLevelUpReward))
		{
			MAPLOG(ERROR_LEVEL, "[%s]王者赛升级奖励配置不匹配[StageLevel:%d]!", __FUNCTION__, tConfig.wStageLevel);
			return false;
		}
		if (!ReadCommonItemList(tmp.data(), "daily_first_win_reward_id_list =", "daily_first_win_reward_num_list =", tConfig.vDailyFirstWinReward))
		{
			MAPLOG(ERROR_LEVEL, "[%s]王者赛首胜奖励配置不匹配[StageLevel:%d]!", __FUNCTION__, tConfig.wStageLevel);
			return false;
		}
		if (!ReadCommonItemList(tmp.data(), "season_reward_id_list =", "season_reward_num_list =", tConfig.vSeasonReward))
		{
			MAPLOG(ERROR_LEVEL, "[%s]王者赛季奖励配置不匹配[StageLevel:%d]!", __FUNCTION__, tConfig.wStageLevel);
			return false;
		}
		
		CConfigManager::Instance()->m_KingMatchStageList.insert(make_pair(tConfig.wStageLevel, tConfig));
	}

	return true;
}

// 加载王者赛季任务配置
bool LoadKingMatchSeasonTask(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		KingMatchSeasonTask_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wWinTimes = ReadUInt(tmp.data(), "win_times =");
		if (!ReadCommonItemList(tmp.data(), "reward_id_list =", "reward_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]王者赛季任务奖励配置不匹配[WinTimes:%d]!", __FUNCTION__, tConfig.wWinTimes);
			return false;
		}
		CConfigManager::Instance()->m_KingMatchSeasonTaskList.insert(make_pair(tConfig.wWinTimes, tConfig));
	}
	return true;
}


// 加载王者赛排行奖励配置
bool LoadKingMatchRankReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		KingMatchRankReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wRankMin = ReadUInt(tmp.data(), "rank_min =");
		tConfig.wRankMax = ReadUInt(tmp.data(), "rank_max =");
		tConfig.dwTitleId = ReadUInt(tmp.data(), "title_id =");
		if (!ReadCommonItemList(tmp.data(), "reward_id_list =", "reward_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]王者赛排行奖励配置不匹配[RankMin:%d]!", __FUNCTION__, tConfig.wRankMin);
			return false;
		}
		CConfigManager::Instance()->m_KingMatchRankRewardList.push_back(tConfig);
	}
	return true;
}

// 加载王者赛时间配置
bool LoadKingMatchSeasonTime(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		KingMatchSeasonTime_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wSeason = ReadUInt(tmp.data(), "index =");
		tConfig.dwStartDate = ReadUInt(tmp.data(), "start_date =");
		tConfig.dwStartTime = ReadUInt(tmp.data(), "start_time =");
		tConfig.dwOverDate = ReadUInt(tmp.data(), "over_date =");
		tConfig.dwOverTime = ReadUInt(tmp.data(), "over_time =");
		CConfigManager::Instance()->m_KingMatchSeasonTimeList.insert(make_pair(tConfig.wSeason, tConfig));
	}
	return true;
}

// 加载圣诞节Boss等级配置
bool LoadChristmasBossLevel(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ChristmasBossLevel_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wType = ReadUInt(tmp.data(), "type =");
		tConfig.dwLevel = ReadUInt(tmp.data(), "level =");
		tConfig.dwBossHP = ReadUInt(tmp.data(), "boss_hp =");
		if (!ReadCommonItemList(tmp.data(), "kill_reward_id_list =", "kill_reward_num_list =", tConfig.vKillRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]圣诞节Boss击杀奖励配置不匹配[type:%d,level:%d]!", __FUNCTION__, tConfig.wType, tConfig.dwLevel);
			return false;
		}
		if (!ReadCommonItemList(tmp.data(), "boss_reward_id_list =", "boss_reward_num_list =", tConfig.vBossRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]圣诞节Boss奖励配置不匹配[type:%d,level:%d]!", __FUNCTION__, tConfig.wType, tConfig.dwLevel);
			return false;
		}
		DWORD dwFindId = tConfig.wType * 100000 + tConfig.dwLevel;
		CConfigManager::Instance()->m_ChristmasBossLevelList.insert(make_pair(dwFindId, tConfig));
	}
	return true;
}

// 加载圣诞节Boss配置
bool LoadChristmasBoss(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ChristmasBoss_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wType = ReadUInt(tmp.data(), "type =");
		tConfig.dwReviveTime = ReadUInt(tmp.data(), "revive_time =");
		ReadArry1(tmp.data(), "limit_item_list =", tConfig.vLimitSnowBallList);
		CConfigManager::Instance()->m_ChristmasBossList.insert(make_pair(tConfig.wType, tConfig));
	}
	return true;
}

// 加载圣诞节雪球配置
bool LoadChristmasSnowball(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ChristmasSnowball_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwSnowballId = ReadUInt(tmp.data(), "id =");
		tConfig.dwDamageMin = ReadUInt(tmp.data(), "damage_min =");
		tConfig.dwDamageMax = ReadUInt(tmp.data(), "damage_max =");
		ReadArry1(tmp.data(), "reward_id_list =", tConfig.vRewardItemList);
		ReadArry1(tmp.data(), "reward_num_list =", tConfig.vRewardNumList);
		ReadArry1(tmp.data(), "reward_rate_list =", tConfig.vRewardRateList);
		if ((tConfig.vRewardItemList.size() != tConfig.vRewardNumList.size()) || 
			(tConfig.vRewardNumList.size() != tConfig.vRewardRateList.size()))
		{
			MAPLOG(ERROR_LEVEL, "[%s]圣诞节雪球奖励配置不匹配[SnowballId:%d]!", __FUNCTION__, tConfig.dwSnowballId);
			return false;
		}
		CConfigManager::Instance()->m_ChristmasSnowballList.insert(make_pair(tConfig.dwSnowballId, tConfig));
	}
	return true;
}

// 加载facebook好友邀请配置
bool LoadFacebookFriendInvite(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		FacebookFriendInvite_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwInviteNum = ReadUInt(tmp.data(), "invite_num =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]facebook好友邀请奖励配置不匹配[InviteNum:%d]!", __FUNCTION__, tConfig.dwInviteNum);
			return false;
		}
		CConfigManager::Instance()->m_FacebookFriendInvitList.insert(make_pair(tConfig.dwInviteNum, tConfig));
	}
	return true;
}

// 加载facebook好友召唤配置
bool LoadFacebookFriendRecall(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		FacebookFriendSummon_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwSummonNum = ReadUInt(tmp.data(), "summon_num =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]facebook好友召唤奖励配置不匹配[InviteNum:%d]!", __FUNCTION__, tConfig.dwSummonNum);
			return false;
		}
		CConfigManager::Instance()->m_FacebookFriendSummonList.insert(make_pair(tConfig.dwSummonNum, tConfig));
	}
	return true;
}

// 加载facebook账号绑定配置
bool LoadFacebookAccountBinding(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		FacebookAccountBinding_Config tConfig;
		string &tmp = data_list[i];
		DWORD dwRewardId = ReadUInt(tmp.data(), "id =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]facebook账号绑定奖励配置不匹配[RewardId:%d]!", __FUNCTION__, dwRewardId);
			return false;
		}
		CConfigManager::Instance()->m_FacebookAccountBindingList.insert(make_pair(dwRewardId, tConfig));
	}
	return true;
}

// 加载facebook分享配置
bool LoadFacebookShare(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		FacebookShare_Config tConfig;
		string &tmp = data_list[i];
		DWORD dwShareChapter = ReadUInt(tmp.data(), "share_chapter =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]facebook分享奖励配置不匹配[RewardId:%d]!", __FUNCTION__, dwShareChapter);
			return false;
		}
		CConfigManager::Instance()->m_FacebookChareList.insert(make_pair(dwShareChapter, tConfig));
	}
	return true;
}

// 加载facebook评论配置
bool LoadFacebookComend(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		FacebookCommend_Config tConfig;
		string &tmp = data_list[i];
		DWORD dwChapter = ReadUInt(tmp.data(), "reward_chapter =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]facebook分享评论配置不匹配[RewardId:%d]!", __FUNCTION__, dwChapter);
			return false;
		}
		CConfigManager::Instance()->m_FacebookCommendList.insert(make_pair(dwChapter, tConfig));
	}
	return true;
}



// 加载在线奖励配置
bool LoadOnlineReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vTimeList;
	vector<vector<DWORD>> reward_id_list;
	vector<vector<DWORD>> reward_num_list;
	for (int i = 0; i < data_list.size(); ++i)
	{
		OnlineReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wDays = ReadUInt(tmp.data(), "day =");
		ReadArry1(tmp.data(), "time_list =", vTimeList);
		ReadArry2(tmp.c_str(), "reward_id_list =", reward_id_list);
		ReadArry2(tmp.c_str(), "reward_num_list =", reward_num_list);
		if ((vTimeList.size() != reward_num_list.size()) || (vTimeList.size() != reward_id_list.size()))
		{
			MAPLOG(ERROR_LEVEL, "[%s]在线奖励配置不匹配[day:%d]", __FUNCTION__, tConfig.wDays);
			continue;
		}
	
		for (size_t k = 0; k < vTimeList.size(); ++k)
		{
			OnlineRewardTime_Config time_config;
			time_config.dwTime = vTimeList[k] * 60;
			vector<DWORD> &vIdList = reward_id_list[k];
			vector<DWORD> &vNumList = reward_num_list[k];
			if (vIdList.size() != vNumList.size())
			{
				MAPLOG(ERROR_LEVEL, "[%s]在线奖励配奖励物品置不匹配[day:%d]", __FUNCTION__, tConfig.wDays);
				continue;
			}
			for (int y = 0; y < vIdList.size(); ++y)
			{
				time_config.vRewardList.push_back(Item(vIdList[y], vNumList[y]));
			}

			tConfig.vTimeDataList.push_back(time_config);
		}

		CConfigManager::Instance()->m_OnlineRewardList.insert(make_pair(tConfig.wDays, tConfig));
	}
	return true;
}

// 加载7天登录配置
bool LoadSevenDayLogin(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		SevenDayLogin_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wDays = ReadUInt(tmp.data(), "day =");
		if (!ReadCommonItemList(tmp.data(), "reward_id_list =", "reward_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]7天登录奖励配置不匹配[day:%d]!", __FUNCTION__, tConfig.wDays);
			return false;
		}
		CConfigManager::Instance()->m_SevenDayLoginList.insert(make_pair(tConfig.wDays, tConfig));
	}
	return true;
}

// 加载充值回馈配置
bool LoadRechargeReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		RechargeReward_Config tConfig;
		string &tmp = data_list[i];
		DWORD dwDay = ReadUInt(tmp.data(), "day =");
		tConfig.wDataId = ReadUInt(tmp.data(), "data_id =");
		tConfig.wRechargeId = ReadUInt(tmp.data(), "recharge_id =");
		DWORD dwKey = tConfig.wDataId * 1000 + dwDay;
		if (!ReadCommonItemList(tmp.data(), "reward_id_list =", "reward_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]充值回馈奖励配置不匹配[DataId:%d,Day:%d]!", __FUNCTION__, tConfig.wDataId, dwDay);
			return false;
		}
		CConfigManager::Instance()->m_RechargeRewardList.insert(make_pair(dwKey, tConfig));
	}
	return true;
}

// 加载充值回馈配置
bool LoadRechargeExtraReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		RechargeExtraReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wDataId = ReadUInt(tmp.data(), "data_id =");
		//tConfig.wFindId = ReadUInt(tmp.data(), "id =");
		tConfig.wNeedRechargeDay = ReadUInt(tmp.data(), "need_recharge_day =");
		if (!ReadCommonItemList(tmp.data(), "reward_id_list =", "reward_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]充值回馈额外奖励配置不匹配[DataId:%d]!", __FUNCTION__, tConfig.wDataId);
			return false;
		}
		CConfigManager::Instance()->m_RechargeExtraRewardList.insert(make_pair(tConfig.wDataId, tConfig));
	}
	return true;
}

bool LoadAppleIAP(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		AppleIap_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwRechargeId = ReadUInt(tmp.data(), "recharge_id =");
		tConfig.strIapId = ReadString(tmp.data(), "iap_recharge_id = ");
		CConfigManager::Instance()->m_AppleIAPList.insert(make_pair(tConfig.strIapId, tConfig));
	}
	return true;
}



// 加载累充奖励配置
bool LoadTotalRechargeReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		TotalRechargeReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wDataId = ReadUInt(tmp.data(), "data_id =");
		tConfig.wRewardId = ReadUInt(tmp.data(), "reward_id =");
		tConfig.dwTotalRchargetAmount = ReadUInt(tmp.data(), "need_recharge_amount =");
		if (!ReadCommonItemList(tmp.data(), "reward_id_list =", "reward_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]累充奖励配置不匹配[id:%d]!", __FUNCTION__, tConfig.wRewardId);
			return false;
		}
		DWORD dwFindId = 100000 * tConfig.wDataId + tConfig.wRewardId;
		CConfigManager::Instance()->m_TotalRechargeRewardList.insert(make_pair(dwFindId, tConfig));
	}
	return true;
}

// 加载扭蛋抽奖配置
bool LoadNDLottery(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, vector<NDLottery_Config>> *pLotteryList = &CConfigManager::Instance()->m_NDLotteryList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		NDLottery_Config tConfig;
		string &tmp = data_list[i];
		DWORD dwDataId = ReadUInt(tmp.data(), "data_id =");
		tConfig.dwItemId = ReadUInt(tmp.data(), "item_id =");
		tConfig.dwItemNum = ReadUInt(tmp.data(), "item_num =");
		tConfig.nWeight = ReadUInt(tmp.data(), "weight =");
		tConfig.byIsNeedRecord = ReadUInt(tmp.data(), "is_need_record =");
		map<DWORD, vector<NDLottery_Config>>::iterator iter = pLotteryList->find(dwDataId);
		if (iter != pLotteryList->end())
		{
			iter->second.push_back(tConfig);
		}
		else
		{
			vector<NDLottery_Config> list;
			list.push_back(tConfig);
			pLotteryList->insert(make_pair(dwDataId, list));
		}
	}
	return true;
}

// 加载扭蛋抽奖消耗配置
bool LoadNDLotteryCost(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		NDLotteryCost_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwDataId = ReadUInt(tmp.data(), "data_id =");
		tConfig.dwCostItemId = ReadUInt(tmp.data(), "cost_item_id =");
		tConfig.nSingleCostDiamond = ReadUInt(tmp.data(), "nd_onetime_cost =");
		tConfig.nOenKeyCostDiamond = ReadUInt(tmp.data(), "nd_tentime_cost =");
		CConfigManager::Instance()->m_NDLotteryCostList.insert(make_pair(tConfig.dwDataId, tConfig));
	}
	return true;
}

// 加载热卖商品配置
bool LoadHotSaleGoods(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		HotSaleGoods_Config tConfig;
		string &tmp = data_list[i];
		DWORD dwDataId = ReadUInt(tmp.data(), "data_id =");
		tConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]热卖商品配置不匹配[id:%d]!", __FUNCTION__, tConfig.wGoodsId);
			return false;
		}
		if (!LoadCommonCost(tmp.data(), tConfig.vCostList, "cost_type_list =", "cost_value_list ="))
		{
			MAPLOG(ERROR_LEVEL, "[%s]热卖商品消耗配置不匹配[id:%d]!", __FUNCTION__, tConfig.wGoodsId);
			return false;
		}
		ReadArry1(tmp.data(), "buy_num_max_list =", tConfig.vBuyNumMaxList);
		ReadArry1(tmp.data(), "need_level_list =", tConfig.vNeedLevelList);
		ReadArry1(tmp.data(), "recharge_id_list =", tConfig.vRechargeIdList);
		ReadArry1(tmp.data(), "reset_day_list =", tConfig.vResetDayList);
		DWORD dwKey = dwDataId * 100000 + tConfig.wGoodsId;
		CConfigManager::Instance()->m_HotSaleGoodsList.insert(make_pair(dwKey, tConfig));
	}
	return true;
}

// 加载转盘配置
bool LoadTurntableRank(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		TurntableRank_Config tConfig;
		string &tmp = data_list[i];
		WORD wDataId = ReadUInt(tmp.data(), "data_id =");
		WORD wRankMin = ReadUInt(tmp.data(), "rank_min =");
		WORD wRankMax = ReadUInt(tmp.data(), "rank_max =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]转盘排行榜奖励配置不匹配[DataId:%d,RankMin:%d]!", __FUNCTION__, wDataId, wRankMin);
			return false;
		}

		for (WORD k = wRankMin; k <= wRankMax; ++k)
		{
			DWORD dwKey = wDataId * 10000 + k;
			CConfigManager::Instance()->m_TurntableRankList.insert(make_pair(dwKey, tConfig));
		}
	}
	return true;
}

// 加载转盘配置
bool LoadTurntable(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Turntable_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wDataId = ReadUInt(tmp.data(), "data_id =");
		tConfig.nInitDiamond = ReadUInt(tmp.data(), "init_diamond =");
		tConfig.dwCostItemId = ReadUInt(tmp.data(), "cost_item_id =");
		tConfig.dwCostItemNum = ReadUInt(tmp.data(), "cost_item_num =");
		tConfig.nNormalLotteryScore = ReadUInt(tmp.data(), "normal_lottery_score =");
		tConfig.nOneKeyLotteryScore = ReadUInt(tmp.data(), "one_key_lottery_score =");
		tConfig.nNormalLotteryCostDiamond = ReadUInt(tmp.data(), "normal_lottery_cost_diamond =");
		tConfig.nOneKeyLotteryCostDiamond = ReadUInt(tmp.data(), "one_key_lottery_cost_diamond =");
		tConfig.nRankNeedScore = ReadUInt(tmp.data(), "rank_need_score =");
		tConfig.nNormalLotteryDiamond = ReadUInt(tmp.data(), "normal_lottery_diamond =");
		tConfig.nOneKeyLotteryDiamond = ReadUInt(tmp.data(), "one_key_lottery_diamond =");
		tConfig.nOneKeyLotteryTimes = ReadUInt(tmp.data(), "one_key_lottery_times =");
		CConfigManager::Instance()->m_TurntableList.insert(make_pair(tConfig.wDataId, tConfig));
	}
	return true;
}

// 加载转盘抽奖配置
bool LoadTurntableLottery(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, vector<TurntableLottery_Config>> *pLotteryList = &CConfigManager::Instance()->m_TurntableLotteryList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		TurntableLottery_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wDataId = ReadUInt(tmp.data(), "data_id =");
		tConfig.wIndex = ReadUInt(tmp.data(), "index =");
		tConfig.dwRewardItemId = ReadUInt(tmp.data(), "item_id =");
		tConfig.dwRewardItemNumMin = ReadUInt(tmp.data(), "item_num_min =");
		tConfig.dwRewardItemNumMax = ReadUInt(tmp.data(), "item_num_max =");
		tConfig.nRewardDiamondRate = ReadUInt(tmp.data(), "diamond_rate =");
		tConfig.nWeight = ReadUInt(tmp.data(), "weight =");
		tConfig.bIsNeedRecord = ReadUInt(tmp.data(), "is_need_record =");
		map<DWORD, vector<TurntableLottery_Config>>::iterator iter = pLotteryList->find(tConfig.wDataId);
		if (iter == pLotteryList->end())
		{
			pLotteryList->insert(make_pair(tConfig.wDataId, vector<TurntableLottery_Config>()));
			iter = pLotteryList->find(tConfig.wDataId);
		}
		iter->second.push_back(tConfig);
	}
	return true;
}

// 加载限时产出配置
bool LoadLimitProduct(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		LimitProduct_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wMapType = ReadUInt(tmp.data(), "type =");
		tConfig.nStartDate = ReadUInt(tmp.data(), "start_date =");
		tConfig.nOverDate = ReadUInt(tmp.data(), "over_date =");
		ReadArry1(tmp.data(), "start_time_list =", tConfig.vStartTimeList);
		ReadArry1(tmp.data(), "over_time_list =", tConfig.vOverTimeList);
		tConfig.nMultiple = ReadUInt(tmp.data(), "multiple =");
		CConfigManager::Instance()->m_LimitProductList.insert(make_pair(tConfig.wMapType, tConfig));
	}
	return true;
}

// 加载阵营招募配置
bool LoadCampRecruit(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		CampRecruit_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wDataId = ReadUInt(tmp.data(), "data_id =");
		tConfig.wCamp = ReadUInt(tmp.data(), "camp =");
		tConfig.dwNormalRecruitScore = ReadUInt(tmp.data(), "normal_recruit_score =");
		tConfig.dwOneKeyRecruitScore = ReadUInt(tmp.data(), "one_key_recruit_score =");
		tConfig.wDailyFreeRecruitTimes = ReadUInt(tmp.data(), "daily_free_recruit_times =");
		tConfig.wOneKeyRecruitTimes = ReadUInt(tmp.data(), "one_key_recruit_times =");
		tConfig.dwNormalRecruitCostDiamond = ReadUInt(tmp.data(), "normal_recruit_cost_diamond =");
		tConfig.dwOneKeyRecruitCostDiamond = ReadUInt(tmp.data(), "one_key_recruit_cost_diamond =");
		ReadArry1(tmp.data(), "score_task_list =", tConfig.vScoreTaskList);
		if (!ReadCommonItemList(tmp.data(), "score_task_reward_id_list =", "score_task_reward_num_list =", tConfig.vScoreTaskRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]阵营招募奖励配置不匹配[DataId:%d,Camp:%d]!", __FUNCTION__, tConfig.wDataId, tConfig.wCamp);
			return false;
		}
		if (tConfig.vScoreTaskList.size() != tConfig.vScoreTaskRewardList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]阵营招募积分任务数据与奖励数据配置不匹配[TaskList:%d,RewardList:%d]!", 
				__FUNCTION__, tConfig.vScoreTaskList.size(), tConfig.vScoreTaskRewardList.size());
			return false;
		}

		DWORD dwKey = tConfig.wDataId * 100 + tConfig.wCamp;
		CConfigManager::Instance()->m_CampRecruitList.insert(make_pair(dwKey, tConfig));
	}
	return true;
}

// 加载阵营招募英雄配置
bool LoadCampRecruitHero(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, vector<CampRecruitHero_Config>> *pRecruitHeroList = &CConfigManager::Instance()->m_CampRecruitHeroList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		CampRecruitHero_Config tConfig;
		string &tmp					= data_list[i];
		tConfig.wDataId				= ReadUInt(tmp.data(), "data_id =");
		tConfig.wCamp				= ReadUInt(tmp.data(), "camp =");
		tConfig.dwWeight			= ReadUInt(tmp.data(), "weight =");
		tConfig.tRewardItem.dwId	= ReadUInt(tmp.data(), "reward_id =");
		tConfig.tRewardItem.dwNum	= ReadUInt(tmp.data(), "reward_num =");
		tConfig.byIsNeedRecord		= ReadUInt(tmp.data(), "is_need_record =");
		DWORD dwKey = tConfig.wDataId * 100 + tConfig.wCamp;
		map<DWORD, vector<CampRecruitHero_Config>>::iterator iter = pRecruitHeroList->find(dwKey);
		if (iter == pRecruitHeroList->end())
		{
			pRecruitHeroList->insert(make_pair(dwKey, vector<CampRecruitHero_Config>()));
			iter = pRecruitHeroList->find(dwKey);
		}
		iter->second.push_back(tConfig);
	}
	return true;
}



// 加载unlock活动奖励
bool LoadUnlockActivityReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		UnlockActivityReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wRewardId = ReadUInt(tmp.data(), "reward_id =");
		if (!ReadCommonItemList(tmp.data(), "reward_id_list =", "reward_num_list =", tConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]unlock活动奖励配置不匹配[RewardId:%d!", __FUNCTION__, tConfig.wRewardId);
			return false;
		}
		CConfigManager::Instance()->m_UnlockActivityRewardList.insert(make_pair(tConfig.wRewardId, tConfig));
	}
	return true;
}

// 加载unlock观看video奖励
bool LoadUnlockWatchingVideoReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		UnlockWatchingVideoReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wWatchingTimes = ReadUInt(tmp.data(), "wathching_times =");
		if (!ReadCommonItemList(tmp.data(), "reward_id_list =", "reward_num_list =", tConfig.vRewardItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]unlock观看视频奖励配置不匹配[WatchingTimes:%d!", __FUNCTION__, tConfig.wWatchingTimes);
			return false;
		}
		CConfigManager::Instance()->m_UnlockWathingVideoRewardList.insert(make_pair(tConfig.wWatchingTimes, tConfig));
	}
	return true;
}

// 加载世界任务等级配置
bool LoadWorldTaskLevel(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		WorldTaskLevel_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wDays = ReadUInt(tmp.data(), "days =");
		tConfig.wWorldLevel = ReadUInt(tmp.data(), "world_level =");
		vector<DWORD> vRewardLevelSection;
		ReadArry1(tmp.data(), "reward_level_section =", vRewardLevelSection);
		vector<vector<DWORD>> reward_id_list;
		ReadArry2(tmp.c_str(), "reward_id_list =", reward_id_list);
		vector<vector<DWORD>> reward_num_list;
		ReadArry2(tmp.c_str(), "reward_num_list =", reward_num_list);

		// 检测配置数据是否匹配
		if (vRewardLevelSection.size() != reward_id_list.size() || vRewardLevelSection.size() != reward_num_list.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]世界任务最终奖励配置不匹配[OpenServerDays:%d!", __FUNCTION__, tConfig.wDays);
			return false;
		}

		WORD wMinLevel = 0;
		DWORD wMaxLevel = 0;
		CConfigManager::Instance()->m_WorldTaskLevelList.insert(make_pair(tConfig.wDays, tConfig));
		map<DWORD, WorldTaskLevel_Config>::iterator iter = CConfigManager::Instance()->m_WorldTaskLevelList.find(tConfig.wDays);
		for (int k = 0; k < vRewardLevelSection.size(); ++k)
		{
			wMaxLevel = vRewardLevelSection[k];
			vector<DWORD> vIdList = reward_id_list[k];
			vector<DWORD> vNumList = reward_num_list[k];

			// 检测配置是否匹配
			if (vIdList.size() != vNumList.size())
			{
				MAPLOG(ERROR_LEVEL, "[%s]世界任务最终奖励物品配置不匹配[OpenServerDays:%d,Level:%d]", 
					__FUNCTION__, tConfig.wDays, wMaxLevel);
			}

			// 把奖励的物品组成列表
			vector<Item> vRewardList;
			for (int m = 0; m < vIdList.size(); m++)
			{
				vRewardList.push_back(Item(vIdList[m], vNumList[m]));
			}

			// 每个等级都对应一条数据
			for (int m = wMinLevel + 1; m <= wMaxLevel; ++m)
			{
				iter->second.vRewardList.insert(make_pair(m, vRewardList));
			}

			// 更新始起等级
			wMinLevel = wMaxLevel;
		}
	}
	return true;
}

// 加载世界等级任务ID列表
bool LoadWorldLevelTaskIdList(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		WorldLevelTaskIdList_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wTaskLevel = ReadUInt(tmp.data(), "task_level =");
		tConfig.nExp = ReadUInt(tmp.data(), "exp =");
		ReadArry1(tmp.data(), "task_id_list =", tConfig.vTaskIdList);

		// 任务等级为作Key的列表
		CConfigManager::Instance()->m_WorldLevelTaskIdList.insert(make_pair(tConfig.wTaskLevel, tConfig));

		// 任务ID为作Key的列表
		for (int k = 0; k < tConfig.vTaskIdList.size(); ++k)
		{
			CConfigManager::Instance()->m_WorldLevelTaskIdListEx.insert(make_pair(tConfig.vTaskIdList[k], tConfig));
		}
	}
	return true;
}

// 加载世界等级任务事件列表
bool LoadWorldLevelTaskEvent(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		WorldLevelTaskEvent_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wTaskId = ReadUInt(tmp.data(), "task_id =");
		tConfig.wTaskType = ReadUInt(tmp.data(), "task_type =");
		tConfig.nX = ReadUInt(tmp.data(), "param_x =");
		tConfig.nY = ReadUInt(tmp.data(), "param_y =");
		if (!ReadCommonItemList(tmp.data(), "reward_item_id_list =", "reward_item_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]unlock观看视频奖励配置不匹配[TaskId:%d!", __FUNCTION__, tConfig.wTaskId);
			return false;
		}
		CConfigManager::Instance()->m_WorldLevelTaskEventList.insert(make_pair(tConfig.wTaskId, tConfig));
	}
	return true;
}

// 加载资源战商店配置
bool LoadResourceWarShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.wRefreshType = ReadUInt(tmp.data(), "refresh_type =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]资源战道具商品消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_RESOURCE_WAR_SHOP, sConfig);
	}

	return true;
}

// 加载资源战章节配置
bool LoadResourceWarChapter(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vResourceList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		ResourceWarChapter_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wChapter = ReadUInt(tmp.data(), "chapter =");
		ReadArry1(tmp.data(), "resource_id_list =", vResourceList);
		for (vector<DWORD>::iterator iter = vResourceList.begin(); iter != vResourceList.end(); ++iter){
			tConfig.resourceList.insert(make_pair(*iter, *iter));
		}
		CConfigManager::Instance()->m_ResourceWarChapterList.insert(make_pair(tConfig.wChapter, tConfig));
	}
	return true;
}

// 加载资源战资源配置
bool LoadResourceWarResource(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vKeepOccupyCostDiamondList;
	vector<DWORD> vExtraRewardNeedTimeList;
	vector<vector<DWORD>> vExtraRewardItemIdList;
	vector<vector<DWORD>> vExtraRewardItemNumList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		ResourceWarResource_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wResourceId = ReadUInt(tmp.data(), "resource_id =");
		tConfig.wResourceType = ReadUInt(tmp.data(), "resource_type =");
		tConfig.dwMapId = ReadUInt(tmp.data(), "map_id =");
		tConfig.wBaseProfitIntervalTime = ReadUInt(tmp.data(), "base_reward_interval_time =");
		tConfig.nCostDiamond = ReadUInt(tmp.data(), "cost_diamond =");
		tConfig.nBaseRewardResourceWarSocre = ReadUInt(tmp.data(), "base_reward_currency =");

		ReadArry1(tmp.data(), "keep_occupy_cost_diamond_list =", vKeepOccupyCostDiamondList);
		ReadArry1(tmp.data(), "extra_reward_need_time_list =", vExtraRewardNeedTimeList);
		ReadArry2(tmp.c_str(), "extra_reward_item_id_list =", vExtraRewardItemIdList);
		ReadArry2(tmp.c_str(), "extra_reward_item_num_list =", vExtraRewardItemNumList);

		// 检测配置是否匹配
		if ((vExtraRewardItemNumList.size() != vExtraRewardItemIdList.size()) || 
			(vExtraRewardItemNumList.size() != vKeepOccupyCostDiamondList.size()) || 
			(vExtraRewardItemNumList.size() != vExtraRewardNeedTimeList.size()))
		{
			MAPLOG(ERROR_LEVEL, "[%s]资源战资源配置不匹配[ResourceId:%d]!", __FUNCTION__, tConfig.wResourceId);
			continue;
		}

		ResourceWarExtraReward tExtraRewardData;
		for (int k = 0; k < vKeepOccupyCostDiamondList.size(); ++k)
		{
			tExtraRewardData.nCostDiamond = vKeepOccupyCostDiamondList[k];	// 消耗钻石
			tExtraRewardData.vRewardNeedTime = vExtraRewardNeedTimeList[k];	// 
			vector<DWORD> vItemIdList = vExtraRewardItemIdList[k];
			vector<DWORD> vItemNumList = vExtraRewardItemNumList[k];
			if (vItemIdList.size() != vItemNumList.size())
			{
				MAPLOG(ERROR_LEVEL, "[%s]资源战资源配置额外奖励物品不匹配[ResourceId:%d]!", __FUNCTION__, tConfig.wResourceId);
				continue;
			}
			tExtraRewardData.tRewardItemList.clear();
			for (int m = 0; m < vItemIdList.size(); ++m)
			{
				tExtraRewardData.tRewardItemList.push_back(Item(vItemIdList[m], vItemNumList[m]));
			}
			tConfig.vExtraRewardDataList.push_back(tExtraRewardData);
		}


		CConfigManager::Instance()->m_ResourceWarResourceList.insert(make_pair(tConfig.wResourceId, tConfig));
	}
	return true;
}

// 加载资源战科技配置
bool LoadResourceWarTechnology(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ResourceWarTechnology_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wLevel = ReadUInt(tmp.data(), "level =");
		tConfig.dwNeedAccupyTime = g_Hour2Second(ReadUInt(tmp.data(), "need_accupy_time ="));
		tConfig.nBaseRewardAddRate = ReadUInt(tmp.data(), "base_reward_add_rate =");
		CConfigManager::Instance()->m_ResourceWarTechnologyList.insert(make_pair(tConfig.wLevel, tConfig));
	}
	return true;
}

// 加载资源战工会收益配置
bool LoadResourceWarGuildProfit(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ResourceWarGuildProfit_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wMemberNum = ReadUInt(tmp.data(), "member_num =");
		tConfig.nBaseRewardAddRate = ReadUInt(tmp.data(), "base_reward_add_rate =");
		CConfigManager::Instance()->m_ResourceWarGuildProfitList.insert(make_pair(tConfig.wMemberNum, tConfig));
	}
	return true;
}

// 加载工会战星球配置
bool LoadGuildWarStar(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		GuildWarStar_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwStarId = ReadUInt(tmp.data(), "star_id =");
		tConfig.wStarType = ReadUInt(tmp.data(), "star_type =");
		tConfig.dwMapId = ReadUInt(tmp.data(), "map_id =");
		ReadArry1(tmp.data(), "tower_list =", tConfig.vTowerList);
		// 每日奖励
		if (!ReadCommonItemList(tmp.data(), "local_product_id_list =", "local_product_num_list =", tConfig.vLocalProductList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]星球特产奖励配置不匹配[StarId:%d!", __FUNCTION__, tConfig.dwStarId);
			return false;
		}
		// 每日奖励数量列表(对战模式)
		if (!ReadCommonItemList(tmp.data(), "daily_reward_id_list =", "daily_reward_num_list =", tConfig.vDailyRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]每日奖励配置不匹配[StarId:%d!", __FUNCTION__, tConfig.dwStarId);
			return false;
		}
		// 挑战失败奖励(积分模式)
		if (!ReadCommonItemList(tmp.data(), "loser_reward_id_list =", "loser_reward_num_list =", tConfig.vLoserRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]挑战失败奖励配置不匹配[StarId:%d!", __FUNCTION__, tConfig.dwStarId);
			return false;
		}
		// 挑战胜利奖励(积分模式)
		if (!ReadCommonItemList(tmp.data(), "winner_reward_id_list =", "winner_reward_num_list =", tConfig.vWinnerRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]挑战胜利奖励配置不匹配[StarId:%d!", __FUNCTION__, tConfig.dwStarId);
			return false;
		}

		CConfigManager::Instance()->m_GuildWarStarList.insert(make_pair(tConfig.dwStarId, tConfig));
	}
	return true;
}

// 加载工会战塔配置
bool LoadGuildWarTower(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		GuildWarTower_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwTowerId = ReadUInt(tmp.data(), "tower_id =");
		tConfig.wTowerType = ReadUInt(tmp.data(), "tower_type =");
		tConfig.dwMapId = ReadUInt(tmp.data(), "map_id =");
		tConfig.dwDurability = ReadUInt(tmp.data(), "durability =");
		tConfig.wMonsterDefenseTimes = ReadUInt(tmp.data(), "monster_defense_times =");
		tConfig.wDefenderNumMax = ReadUInt(tmp.data(), "defender_num_max =");
		tConfig.wRoleDefenseTimes = ReadUInt(tmp.data(), "role_defense_times =");
		// 攻破塔奖励列表
		if (!ReadCommonItemList(tmp.data(), "break_tower_reward_id_list =", "break_tower_reward_num_list =", tConfig.vBreakTowerRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]破塔奖励配置不匹配[TowerId:%d!", __FUNCTION__, tConfig.dwTowerId);
			return false;
		}
		// 挑战塔失败奖励
		if (!ReadCommonItemList(tmp.data(), "loser_reward_id_list =", "loser_reward_num_list =", tConfig.vLoserRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]挑战塔失败奖励配置不匹配[TowerId:%d!", __FUNCTION__, tConfig.dwTowerId);
			return false;
		}
		// 挑战塔成功奖励
		if (!ReadCommonItemList(tmp.data(), "winner_reward_id_list =", "winner_reward_num_list =", tConfig.vWinnerRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]挑战塔成功奖励配置不匹配[TowerId:%d!", __FUNCTION__, tConfig.dwTowerId);
			return false;
		}

		CConfigManager::Instance()->m_GuildWarTowerList.insert(make_pair(tConfig.dwTowerId, tConfig));
	}
	return true;
}

// 加载工会战排行榜奖励配置
bool LoadGuildWarRankReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		GuildWarRankReward_Config tConfig;
		string &tmp = data_list[i];
		WORD wMinRank = ReadUInt(tmp.data(), "rank_min =");
		WORD wMaxRank = ReadUInt(tmp.data(), "rank_max =");
		if (!ReadCommonItemList(tmp.data(), "reward_id_list =", "reward_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]星球排行榜奖励配置不匹配[Rank:%d!", __FUNCTION__, i);
			return false;
		}
		for (WORD i = wMinRank; i <= wMaxRank; ++i)
		{
			tConfig.wRank = i;
			CConfigManager::Instance()->m_GuildWarRankRewardList.insert(make_pair(tConfig.wRank, tConfig));
		}
	}
	return true;
}

// 加载工会战对抗战奖励配置
bool LoadGuildWarVSReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<vector<DWORD>> vRewardIdList;
	vector<vector<DWORD>> vRewardNumList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		GuildWarVSReward_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wVSType = ReadUInt(tmp.data(), "vs_type =");
		tConfig.wVSResult = ReadUInt(tmp.data(), "vs_result =");
		ReadArry1(tmp.data(), "break_tower_num_list =", tConfig.vBreakTowerNumList);

		ReadArry2(tmp.c_str(), "reward_id_list =", vRewardIdList);
		ReadArry2(tmp.c_str(), "reward_num_list =", vRewardNumList);
		if ((vRewardIdList.size() != vRewardNumList.size()) || 
			(tConfig.vBreakTowerNumList.size() && vRewardIdList.size() != tConfig.vBreakTowerNumList.size()))
		{
			MAPLOG(ERROR_LEVEL, "[%s]工会战对抗战奖励配置不匹配1[VSType:%d,VSResult:%d]",
				__FUNCTION__, tConfig.wVSType, tConfig.wVSResult);
			return false;
		}

		for (int k = 0; k < vRewardIdList.size(); ++k)
		{
			vector<DWORD> &vIdList = vRewardIdList[k];
			vector<DWORD> &vNumList = vRewardNumList[k];
			if (vIdList.size() != vNumList.size())
			{
				MAPLOG(ERROR_LEVEL, "[%s]工会战对抗战奖励配置不匹配2[VSType:%d,VSResult:%d]", 
					__FUNCTION__, tConfig.wVSType, tConfig.wVSResult);
				return false;
			}
			vector<Item> vRewardList;
			for (int m = 0; m < vIdList.size(); ++m)
			{
				vRewardList.push_back(Item(vIdList[m], vNumList[m]));
			}
			tConfig.vRewardList.push_back(vRewardList);
		}

		DWORD dwKey = tConfig.wVSType * 100 + tConfig.wVSResult;
		CConfigManager::Instance()->m_GuildWarVSRewardList.insert(make_pair(dwKey, tConfig));
	}
	return true;
}

// 加载工会战高级星球活动时间配置
bool LoadGuildWarVSActivityTime(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	map<DWORD, vector<GuildWarVSActivityTime_Config>> *pTmpList = &CConfigManager::Instance()->m_GuildWarVSActivityTimeList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		GuildWarVSActivityTime_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wTimeType = ReadUInt(tmp.data(), "time_type =");
		tConfig.dwDecideAttackerTime = ReadUInt(tmp.data(), "decide_attacker_time =");
		tConfig.dwBeginTime = ReadUInt(tmp.data(), "begin_time =");
		tConfig.dwEndTime = ReadUInt(tmp.data(), "end_time =");
		ReadArry1(tmp.data(), "take_reward_time_list =", tConfig.vTakeRewardTime);
		map<DWORD, vector<GuildWarVSActivityTime_Config>>::iterator iter = pTmpList->find(tConfig.wTimeType);
		if (iter == pTmpList->end())
		{
			pTmpList->insert(make_pair(tConfig.wTimeType, vector<GuildWarVSActivityTime_Config>()));
			iter = pTmpList->find(tConfig.wTimeType);
		}
		iter->second.push_back(tConfig);
	}
	return true;
}

// 加载工会战通用配置
bool LoadGuildWarCommonCfg(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		Server_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwKey = ReadUInt(tmp.data(), "key =");
		tConfig.dwValue = ReadUInt(tmp.data(), "int_value =");
		ReadArry1(tmp.data(), "list_value =", tConfig.vValueList);
		CConfigManager::Instance()->m_GuildWarCommonCfgList.insert(make_pair(tConfig.dwKey, tConfig));
	}
	return true;
}

// 加载工会战大区配置
bool LoadGuildWarRegion(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	vector<DWORD> vServerList;
	for (int i = 0; i < data_list.size(); ++i)
	{
		GuildWarRegion_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wRegionId = ReadUInt(tmp.data(), "region_id =");
		ReadArry1(tmp.data(), "server_id_list =", vServerList);
		for (int k = 0; k < vServerList.size(); ++k)
		{
			tConfig.mServerList.insert(make_pair(vServerList[k], vServerList[k]));
		}
		CConfigManager::Instance()->m_GuildWarRegionList.insert(make_pair(tConfig.wRegionId, tConfig));
	}
	return true;
}

// 加载工会战布防配置
bool LoadGuildWarOrganizeDefense(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		GuildWarOrganizeDefense_Config tConfig;
		string &tmp = data_list[i];
		tConfig.wTowerType = ReadUInt(tmp.data(), "tower_type =");
		ReadArry1(tmp.data(), "power_idx_list =", tConfig.vPowerIdxList); 
		CConfigManager::Instance()->m_GuildWarOrganizeDefenseList.insert(make_pair(tConfig.wTowerType, tConfig));
	}
	return true;
}

// 加载ygame支付配置
bool LoadYGamePayment(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		YGamePayment_Config tConfig;
		string &tmp = data_list[i];
		tConfig.strPaymentId = ReadString(tmp.data(), "payment_id = ");
		tConfig.dwAssistId = ReadUInt(tmp.data(), "assist_id =");
		tConfig.dwRechargeId = ReadUInt(tmp.data(), "recharge_id =");
		tConfig.nPaymentFromType = ReadUInt(tmp.data(), "from_type =");
		tConfig.nYCoin = ReadUInt(tmp.data(), "ycoin =");
		tConfig.nVipExp = ReadUInt(tmp.data(), "vip_exp =");
		tConfig.nDiamond = ReadUInt(tmp.data(), "diamond =");
		tConfig.nBonus = ReadUInt(tmp.data(), "bonus =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vItemList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]YGame充值奖励配置不匹配(payment_id:%s)", __FUNCTION__, tConfig.strPaymentId.c_str());
			return false;
		}
		CConfigManager::Instance()->m_YGamePaymentList.insert(make_pair(tConfig.strPaymentId, tConfig));
	}
	return true;
}

// 加载充值排行榜奖励配置
bool LoadRechargeRankReward(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		RechargeRankReward_Config tConfig;
		string &tmp = data_list[i];
		WORD wRankType = ReadUInt(tmp.data(), "rank_type =");
		WORD wMinRank = ReadUInt(tmp.data(), "rank_min =");
		WORD wMaxRank = ReadUInt(tmp.data(), "rank_max =");
		if (!ReadCommonItemList(tmp.data(), "item_id_list =", "item_num_list =", tConfig.vRewardList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]充值排行榜奖励配置不匹配[Rank:%d!", __FUNCTION__, i);
			return false;
		}
		for (WORD i = wMinRank; i <= wMaxRank; ++i)
		{
			tConfig.wRank = i;
			DWORD dwKey = wRankType * 10000 + tConfig.wRank;
			CConfigManager::Instance()->m_RechargeRankRewardList.insert(make_pair(dwKey, tConfig));
		}
	}
	return true;
}

// 加载挂机掉落配置
bool LoadHangUpDrop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		HangUpDrop_Config tConfig;
		string &tmp = data_list[i];
		tConfig.dwDropItemId = ReadUInt(tmp.data(), "drop_item_id =");
		tConfig.dwDropItemNum = ReadUInt(tmp.data(), "drop_item_num =");
		tConfig.nDropRate = ReadUInt(tmp.data(), "drop_rate =");
		CConfigManager::Instance()->m_vHangUpDropList.push_back(tConfig);
	}
	return true;
}



// 加载GM商店
bool LoadGMShop(CConfigManager *pConfigMgr, std::vector<string> &data_list)
{
	for (int i = 0; i < data_list.size(); ++i)
	{
		ShopGoodsData_Config sConfig;
		string &tmp = data_list[i];
		sConfig.wGoodsId = ReadUInt(tmp.data(), "goods_id =");
		sConfig.byType = ReadUInt(tmp.data(), "shop_sub_type =");
		sConfig.dwEntityId = ReadUInt(tmp.data(), "item_id =");
		sConfig.dwEntityNum = ReadUInt(tmp.data(), "item_num =");
		sConfig.wNeedRoleLevel = ReadUInt(tmp.data(), "need_role_level =");
		sConfig.nContext = ReadUInt(tmp.data(), "need_gm_level =");
		sConfig.wBuyNumMax = ReadUInt(tmp.data(), "buy_num_max =");
		if (!LoadCommonCost(tmp.c_str(), sConfig.vCostList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]资源战道具商品消耗配置不匹配!", __FUNCTION__);
			return false;
		}
		AddShopGoods(SHOP_GM, sConfig);
	}

	return true;
}


bool CConfigManager::Init()
{
	m_CallbackDataList.clear();

	// 注册回调函数
	RegisterCallbackFunc("config_common.txt",						LoadCommon);
	RegisterCallbackFunc("config_server_list.txt",					LoadServerInfo);
	RegisterCallbackFunc("config_monster.txt",						LoadMonster);
	RegisterCallbackFunc("config_battle.txt",						LoadBattle);
	RegisterCallbackFunc("config_map.txt",							LoadMap);
	RegisterCallbackFunc("config_main_fuben.txt",					LoadMainFuben);
	RegisterCallbackFunc("config_item.txt",							LoadItem);
	RegisterCallbackFunc("config_role_level.txt",					LoadRoleLevel);
	RegisterCallbackFunc("config_hero.txt",							LoadHero);
	RegisterCallbackFunc("config_hero_property.txt",				LoadHeroPro);
	RegisterCallbackFunc("config_equipment.txt",					LoadEquipment);
	RegisterCallbackFunc("config_equipment_compose.txt",			LoadEquipmentCompose);
	RegisterCallbackFunc("config_equipment_strengthen.txt",			LoadEquipmentStrengthen);
	RegisterCallbackFunc("config_equipment_refine.txt",				LoadEquipmentRefine);
	RegisterCallbackFunc("config_item_shop.txt",					LoadMallGoods_Item);
	RegisterCallbackFunc("config_gift_shop.txt",					LoadMallGoods_Gift);
	RegisterCallbackFunc("config_hero_breaklevelup.txt",			LoadHeroBreakLevelUp);
	RegisterCallbackFunc("config_hero_evolution.txt",				LoadHeroEvolution);
	RegisterCallbackFunc("config_hero_qualifications.txt",			LoadHeroQualifications); 
	RegisterCallbackFunc("config_hero_compose.txt",					LoadHeroCompose);
	RegisterCallbackFunc("config_hero_level.txt",					LoadHeroLevel);
	RegisterCallbackFunc("config_hero_to_battle.txt",				LoadHeroToBattle);
	RegisterCallbackFunc("config_equipment_strengthen_forge.txt",	LoadEquipmentStrengthenForge);
	RegisterCallbackFunc("config_equipment_refine_forge.txt",		LoadEquipmentRefineForge);
	RegisterCallbackFunc("config_hero_train.txt",					LoadHeroTrain);
	RegisterCallbackFunc("config_hero_train_cost.txt",				LoadHeroTrainCost);
	RegisterCallbackFunc("config_hero_train_pro.txt",				LoadHeroTrainPro);
	RegisterCallbackFunc("config_treasure.txt",						LoadTreasure);
	RegisterCallbackFunc("config_treasure_compose.txt",				LoadTreasureCompose);
	RegisterCallbackFunc("config_treasure_strengthen.txt",			LoadTreasureStrengthen);
	RegisterCallbackFunc("config_treasure_refine.txt",				LoadTreasureRefine);
	RegisterCallbackFunc("config_rebel_exploit.txt",				LoadRebelExploit);
	RegisterCallbackFunc("config_rebel_battle.txt",					LoadRebelBattle);
	RegisterCallbackFunc("config_role_rebel.txt",					LoadRoleRebel);
	RegisterCallbackFunc("config_rebel_quality.txt",				LoadRebelQuality);
	RegisterCallbackFunc("config_recycle_return.txt",				LoadRecycleReturn);
	RegisterCallbackFunc("config_duobao_map.txt",					LoadDuobaoMap);
	RegisterCallbackFunc("config_duobao_drop.txt",					LoadDuobaoDrop);
	RegisterCallbackFunc("doubao_pregross_reward.txt",				LoadDuobaoPregrossReward);
	RegisterCallbackFunc("config_rebel_shop.txt",					LoadRebelShopGoods);
	RegisterCallbackFunc("config_rebel_rank_award.txt",				LoadRebelRankAward);
	RegisterCallbackFunc("config_hero_normal_recruit.txt",			LoadHeroNormalRecruit);
	RegisterCallbackFunc("config_hero_high_recruit.txt",			LoadHeroHighRecruit);
	RegisterCallbackFunc("config_hero_vip_recruit.txt",				LoadHeroVIPRecruit);
	RegisterCallbackFunc("config_tower_map.txt",					LoadTowerMap);
	RegisterCallbackFunc("config_tower_chapter.txt",				LoadTowerChapter);
	RegisterCallbackFunc("config_tower_elite_map.txt",				LoadTowerEliteMap);
	RegisterCallbackFunc("config_tower_rand_pro.txt",				LoadTowerRandPro);
	RegisterCallbackFunc("config_tower_star_goods.txt",				LoadTowerStarGoods);
	RegisterCallbackFunc("config_tower_shop.txt",					LoadTowerShopGoods);
	RegisterCallbackFunc("heart_of_world.txt",						LoadHeartOfWorld);
	RegisterCallbackFunc("config_land_city.txt",					LoadLandCity);
	RegisterCallbackFunc("config_land_patrol_event.txt",			LoadLandPatrolEvent);
	RegisterCallbackFunc("config_land_patrol.txt",					LoadLandPatrol);
	RegisterCallbackFunc("config_daily_chapter.txt",				LoadDailyChapter);
	RegisterCallbackFunc("config_arena_robot.txt",					LoadArenaRobot);
	RegisterCallbackFunc("config_arena_rank.txt",					LoadArenaRank);
	RegisterCallbackFunc("config_arena_first_rank_award.txt",		LoadArenaFirstRankAward);
	RegisterCallbackFunc("config_arena_shop.txt",					LoadArenaShopGoods);
	RegisterCallbackFunc("config_achievement.txt",					LoadAchievement);
	RegisterCallbackFunc("config_arena_award.txt",					LoadArenaAward);
	RegisterCallbackFunc("config_guild.txt",						LoadGuild);
	RegisterCallbackFunc("config_guild_icon.txt",					LoadGuildIcon);
	RegisterCallbackFunc("config_guild_privilege.txt",				LoadGuildPrivilege);
	RegisterCallbackFunc("config_guild_qy.txt",						LoadGuildQY);
	RegisterCallbackFunc("config_guild_qs_reward.txt",				LoadGuildQSReward);
	RegisterCallbackFunc("config_vip.txt",							LoadVip);
	RegisterCallbackFunc("config_vip_gm.txt",						LoadVipGM);	
	RegisterCallbackFunc("config_vip_notice.txt",					LoadVipNotice);
	RegisterCallbackFunc("config_guild_item_shop.txt",				LoadGuildItemShop);
	RegisterCallbackFunc("config_guild_limit_shop.txt",				LoadGuildLimitShop);
	RegisterCallbackFunc("config_guild_award_shop.txt",				LoadGuildRewardShop);
	RegisterCallbackFunc("config_guild_limit_shop_slot.txt",		LoadGuildLimitShopSlot);
	RegisterCallbackFunc("config_hero_shop.txt",					LoadHeroShop);
	RegisterCallbackFunc("config_daily_mission.txt",				LoadDailyMission);
	RegisterCallbackFunc("config_team_level_daily_mission.txt",		LoadRoleLvDailyMission);
	RegisterCallbackFunc("config_hero_shop_slot.txt",				LoadHeroShopSlot);
	RegisterCallbackFunc("config_hero_destiny.txt",					LoadHeroDestiny);
	RegisterCallbackFunc("config_hero_fb_chapter.txt",				LoadHeroFBChapter);
	RegisterCallbackFunc("config_world_boss_open_time.txt",			LoadWorldBossOpenTime);
	RegisterCallbackFunc("config_world_boss.txt",					LoadWorldBoss);
	RegisterCallbackFunc("config_world_boss_rank_reward.txt",		LoadWorldBossRankReward);
	RegisterCallbackFunc("config_world_boss_honor_reward.txt",		LoadWorldBossHonorReward);
	RegisterCallbackFunc("config_world_boss_kill_reward.txt",		LoadWorldBossDamageReward);
	RegisterCallbackFunc("config_awakening_item.txt",				LoadAwakeningItem);
	RegisterCallbackFunc("config_awaken_shop.txt",					LoadAwakenShop);
	RegisterCallbackFunc("config_awaken_shop_slot.txt",				LoadAwakenShopSlot);
	RegisterCallbackFunc("config_hero_awakening.txt",				LoadHeroAwaken);
	RegisterCallbackFunc("config_elite_fb_chapter.txt",				LoadEliteFB);
	RegisterCallbackFunc("config_guild_skill.txt",					LoadGuildSkill);
	RegisterCallbackFunc("config_sign_in.txt",						LoadSignIn);
	RegisterCallbackFunc("config_ycs.txt",							LoadYCS);
	RegisterCallbackFunc("config_tqt.txt",							LoadTQT);
	RegisterCallbackFunc("config_guild_fb.txt",						LoadGuildFB);
	RegisterCallbackFunc("config_seven_day_target_data.txt",		LoadSevenDayTarget);
	RegisterCallbackFunc("config_seven_day_activity.txt",			LoadSevenDayActivity);
	RegisterCallbackFunc("config_card.txt",							LoadCard);

	// 占卜
	RegisterCallbackFunc("config_card_level_up.txt",				LoadCardLevelUp);
	RegisterCallbackFunc("config_card_break.txt",					LoadCardBreak);
	RegisterCallbackFunc("config_destiny_shop.txt",					LoadDestinyShop);
	RegisterCallbackFunc("config_star_soul_shop.txt",				LoadStarSoulShop);
	RegisterCallbackFunc("config_star_soul_shop_slot.txt",			LoadStarSoulShopSlot);
	RegisterCallbackFunc("config_low_card_group.txt",				LoadLowCardGroup);
	RegisterCallbackFunc("config_high_card_group.txt",				LoadHighCardGroup);
	RegisterCallbackFunc("config_card_group_map.txt",				LoadCardGroupMap);
	RegisterCallbackFunc("config_collection_book.txt",				LoadCollectionBook);
	RegisterCallbackFunc("config_collection_group.txt",				LoadCollectionGroup);
	RegisterCallbackFunc("config_zhanbu_achievement.txt",			LoadZhanBuAchievement);
	RegisterCallbackFunc("config_card_slot.txt",					LoadCardSlot);
	

	RegisterCallbackFunc("config_property_recover.txt",				LoadProRecover);
	RegisterCallbackFunc("config_role_init.txt",					LoadRoleInit);
	RegisterCallbackFunc("config_score_match_shop.txt",				LoadScoreMatchShopGoods);
	RegisterCallbackFunc("config_score_match_target_server.txt",	LoadScoreMatchTargetServer);
	RegisterCallbackFunc("config_score_match_battle_task.txt",		LoadScoreMatchBattleTask);
	RegisterCallbackFunc("config_score_match_rank_reward.txt",		LoadScoreMatchRankReward);
	RegisterCallbackFunc("config_score_match_activity_time.txt",	LoadScoreMatchActivityTime);
	RegisterCallbackFunc("config_score_match_times_cost.txt",		LoadScoreMatchTimesCost);
	RegisterCallbackFunc("config_score_match_search_target.txt",	LoadScoreMatchSearchTarget);
	

	RegisterCallbackFunc("config_equipment_suit.txt",				LoadEquipmentSuit);
	RegisterCallbackFunc("config_system_open.txt",					LoadSystemOpen);
	RegisterCallbackFunc("config_title.txt",						LoadTitle);
	RegisterCallbackFunc("config_treasure_smelt.txt",				LoadTreasureSmelt);
	RegisterCallbackFunc("config_treasure_quality_up_cost.txt",		LoadTreasureQualityUPCost);
	RegisterCallbackFunc("config_reward_center.txt",				LoadReward);
	RegisterCallbackFunc("config_activity.txt",						LoadActivity);
	RegisterCallbackFunc("config_seven_day_shop.txt",				LoadSevenDayShopGoods);
	RegisterCallbackFunc("config_hero_break.txt",					LoadHeroBreak);
	RegisterCallbackFunc("config_hero_pokedex.txt",					LoadHeroPokedex);
	RegisterCallbackFunc("seven_day_target_completion_reward.txt",	LoadSevenDayCompletionReward);
	RegisterCallbackFunc("config_vip_daily_gift.txt",				LoadVipDailyGift);
	RegisterCallbackFunc("config_vip_week_gift.txt",				LoadVipWeekGift);
	RegisterCallbackFunc("config_vip_week_gift_rand_list.txt",		LoadVipWeekGiftRandList);
	RegisterCallbackFunc("config_level_shop_goods.txt",				LoadLevelShopGoods);
	RegisterCallbackFunc("config_level_shop.txt",					LoadLevelShopData);
	RegisterCallbackFunc("config_level_gift.txt",					LoadLevelGiftData);
	RegisterCallbackFunc("config_open_server_fund.txt",				LoadOpenServerFundReward);
	RegisterCallbackFunc("config_all_the_people_welfare.txt",		LoadAllThePeopleWelfare);
	RegisterCallbackFunc("config_recharge.txt",						LoadRecharge);
	RegisterCallbackFunc("config_month_card.txt",					LoadMonthCard);
	RegisterCallbackFunc("config_recharge_mycard.txt",				LoadMycardRecharge);
	
// 	RegisterCallbackFunc("config_limit_preferential_goods.txt",		LoadLimitPreferentialGoods);
// 	RegisterCallbackFunc("config_limit_preferential_shop.txt",		LoadLimitPreferentialShop);
// 	RegisterCallbackFunc("config_limit_preferential_recharge.txt",	LoadLimitPreferentialRecharge);
// 	RegisterCallbackFunc("config_limit_preferential_welfare.txt",	LoadLimitPreferentialWelfare);
	RegisterCallbackFunc("config_daily_fb_damage_reward.txt",		LoadDailyFBDamageReward);
	RegisterCallbackFunc("config_main_fb_chapter_reward.txt",		LoadMainFBChapterReward);
	RegisterCallbackFunc("limit_drop.txt",							LoadLimitDrop);
	
	RegisterCallbackFunc("config_treasure_strengthen_forge.txt",	LoadTreasureEnhanceForge);
	RegisterCallbackFunc("config_treasure_refine_forge.txt",		LoadTreasureRefineForge);
	//RegisterCallbackFunc("personal_activation_code.txt",			LoadActivationCode);
	//RegisterCallbackFunc("public_activation_code.txt",				LoadActivationCode);
	RegisterCallbackFunc("activation_code_reward.txt",				LoadActivationRewardCode);
	RegisterCallbackFunc("spirit.txt",								LoadSpirit);
	RegisterCallbackFunc("spirit_enhance.txt",						LoadSpiritEnhance);
	RegisterCallbackFunc("spirit_pokedex.txt",						LoadSpirtPokedex);
	RegisterCallbackFunc("sprite_train.txt",						LoadSpirtTrain);
	RegisterCallbackFunc("sprite_talent_skill.txt",					LoadSpriteTalentSkill);
	RegisterCallbackFunc("sprite_star_up.txt",						LoadSpriteStarUp);
	RegisterCallbackFunc("sprite_special_property.txt",				LoadSpriteSpecialProperty);
	RegisterCallbackFunc("sprite_qualifications.txt",				LoadSpriteQualifications);
	RegisterCallbackFunc("action_count.txt",						LoadStatisticCount);
	RegisterCallbackFunc("pvp_rank_reward.txt",						LoadPVPRankReward);
	RegisterCallbackFunc("pvp_battle_times_reward.txt",				LoadPVPBattleReward);
	RegisterCallbackFunc("pvp_activity_time.txt",					LoadPVPActivityTime);
	RegisterCallbackFunc("config_limit_activity.txt",				LoadLimitActivity);
	RegisterCallbackFunc("attack_city_chapter.txt",					LoadAttackCityChapter);
	RegisterCallbackFunc("attack_city_boss_reward.txt",				LoadAttackCityBossReward);
	RegisterCallbackFunc("attack_city_weak.txt",					LoadAttackCityWeak);
	RegisterCallbackFunc("attack_city_encourage.txt",				LoadAttackCityEncourage);
	RegisterCallbackFunc("attack_city_activity_time.txt",			LoadAttackCityActivityTime);
	RegisterCallbackFunc("attack_city_rank_reward.txt",				LoadAttackCityRankReward);
	RegisterCallbackFunc("endless_land_collision.txt",				LoadEndlessLandCollision);
	RegisterCallbackFunc("endless_land_keep_kill_reward.txt",		LoadEndlessLandContinueKillReward);
	RegisterCallbackFunc("endless_land_wave.txt",					LoadEndlessLandWave);
	RegisterCallbackFunc("endless_land_shop.txt",					LoadEndlessLandShop);
	RegisterCallbackFunc("endless_land_rank_reward.txt",			LoadEndlessLandRankReward);
	RegisterCallbackFunc("endless_land_battle_cost.txt",			LoadEndlessLandBattleCost);
	RegisterCallbackFunc("config_nobility.txt",						LoadNobility);
	RegisterCallbackFunc("config_luxury_sign_in.txt",				LoadLuxurySignIn);
	RegisterCallbackFunc("config_luxury_sign_in_reward_list.txt",	LoadLuxurySignInReward);
	RegisterCallbackFunc("super_discount_buy_reward.txt",			LoadSuperDiscountBuyReward);
	RegisterCallbackFunc("super_discount_accumulate_reward.txt",	LoadSuperDiscountAccumulatReward);
	RegisterCallbackFunc("new_super_discount_buy_reward.txt",		LoadNewSuperDiscountBuyReward);
	RegisterCallbackFunc("new_super_discount_accumulate_reward.txt",LoadNewSuperDiscountAccumulatReward);
	RegisterCallbackFunc("config_direct_buy_shop.txt",				LoadDirectBuyShop);
	RegisterCallbackFunc("config_direct_buy_shop_slot.txt",			LoadDirectBuyShopSlot);
	RegisterCallbackFunc("config_exchange_shop.txt",				LoadExchangeShop);
	RegisterCallbackFunc("seven_day_rank_reward.txt",				LoadSevenDayRankReward);
	RegisterCallbackFunc("today_recommend_recharge.txt",			LoadTodayRecommendRecharge);
	RegisterCallbackFunc("recommend_shop.txt",						LoadRecommendShop);
	RegisterCallbackFunc("festival_activity.txt",					LoadFestivalActivity);
	RegisterCallbackFunc("festival_activity_param.txt",				LoadFestivalActivtiyTarget);
	RegisterCallbackFunc("accurate_recommend_goods.txt",			LoadAccurateRecommendGoods);
	RegisterCallbackFunc("gem.txt",									LoadGem);
	RegisterCallbackFunc("gem_skill.txt",							LoadGemSkill);
	RegisterCallbackFunc("gem_hole.txt",							LoadGemHole);
	RegisterCallbackFunc("gem_inlay.txt",							LoadGemInlay);
	RegisterCallbackFunc("config_equipment_talent.txt",				LoadEquipmentTalent);
	RegisterCallbackFunc("scene.txt",								LoadScene);
	RegisterCallbackFunc("config_guild_building.txt",				LoadGuildBuilding);
	RegisterCallbackFunc("config_equipment_star_up.txt",			LoadEquipmentStarUp);
	RegisterCallbackFunc("config_equipment_quality_up.txt",			LoadEquipmentQualityUp);
	RegisterCallbackFunc("reinforcement_encourage.txt",				LoadReinforcementEncourage);

	
	// 组队副本
	RegisterCallbackFunc("team_dungeon_shop.txt",					LoadTeamDungeonShop);
	RegisterCallbackFunc("team_dungeon_lottery.txt",				LoadTeamDungeonLottery);
	RegisterCallbackFunc("team_dungeon.txt",						LoadTeamDungeon);
	RegisterCallbackFunc("team_dungeon_task_link.txt",				LoadTeamDungeonTaskLink);
	RegisterCallbackFunc("team_dungeon_task.txt",					LoadTeamDungeonTask);
	RegisterCallbackFunc("team_dungeon_boss_reward.txt",			LoadTeamDungeonBossReward);

	RegisterCallbackFunc("login_reward.txt",						LoadLoginReward);
	RegisterCallbackFunc("closed_test_rebate.txt",					LoadClosedTestRebate);
	RegisterCallbackFunc("account_white_list.txt",					LoadAccountWhite);
	RegisterCallbackFunc("ip_white_list.txt",						LoadIPWhite);
	

	// 王者赛
	RegisterCallbackFunc("king_match_stage.txt",					LoadKingMatchStage);
	RegisterCallbackFunc("king_match_season_task.txt",				LoadKingMatchSeasonTask);
	RegisterCallbackFunc("king_match_rank_reward.txt",				LoadKingMatchRankReward);
	RegisterCallbackFunc("king_match_season_time.txt",				LoadKingMatchSeasonTime);

	RegisterCallbackFunc("config_hide_shop.txt",					LoadHideShop);
	
	// 圣诞节
	RegisterCallbackFunc("christmas_boss_level.txt",				LoadChristmasBossLevel);
	RegisterCallbackFunc("christmas_boss.txt",						LoadChristmasBoss);
	RegisterCallbackFunc("christmas_snowball.txt",					LoadChristmasSnowball);
	RegisterCallbackFunc("christmas_score_exchange_shop.txt",		LoadChristmasScoreExchangeGoods);
	RegisterCallbackFunc("christmas_item_exchange_shop.txt",		LoadChristmasItemExchangeGoods);
	
	// 在线奖励
	RegisterCallbackFunc("online_reward.txt",						LoadOnlineReward);

	// 7天登录
	RegisterCallbackFunc("serven_day_login.txt",					LoadSevenDayLogin);

	// facebook活动
	RegisterCallbackFunc("facebook_account_binding.txt",			LoadFacebookAccountBinding);
	RegisterCallbackFunc("facebook_friend_invite.txt",				LoadFacebookFriendInvite);
	RegisterCallbackFunc("facebook_friend_recall.txt",				LoadFacebookFriendRecall);
	RegisterCallbackFunc("facebook_share.txt",						LoadFacebookShare);
	RegisterCallbackFunc("fivestar_highpraise.txt",					LoadFacebookComend);
	
	
	// 充值回馈
	RegisterCallbackFunc("recharge_reward.txt",						LoadRechargeReward);
	RegisterCallbackFunc("recharge_extra_reward.txt",				LoadRechargeExtraReward);
	RegisterCallbackFunc("apple_iap_id.txt",						LoadAppleIAP);
	
	
	// 累充奖励
	RegisterCallbackFunc("total_recharge_reward.txt",				LoadTotalRechargeReward);

	// 扭蛋抽奖
	RegisterCallbackFunc("nd_lottery.txt",							LoadNDLottery);
	RegisterCallbackFunc("nd_lottery_cost.txt",						LoadNDLotteryCost);

	// 热卖商品
	RegisterCallbackFunc("hot_sale_goods.txt",						LoadHotSaleGoods);

	// 转盘
	RegisterCallbackFunc("turntable.txt",							LoadTurntable);
	RegisterCallbackFunc("turntable_lottery.txt",					LoadTurntableLottery);
	RegisterCallbackFunc("turntable_rank.txt",						LoadTurntableRank);
	
	// 限时产出
	RegisterCallbackFunc("limit_product.txt",						LoadLimitProduct);
	
	// 阵营招募
	RegisterCallbackFunc("camp_recruit.txt",						LoadCampRecruit);
	RegisterCallbackFunc("camp_recruit_hero_list.txt",				LoadCampRecruitHero);

	// unlock活动相关
	RegisterCallbackFunc("unlock_activity_reward.txt",				LoadUnlockActivityReward);
	RegisterCallbackFunc("unlock_watching_video_reward.txt",		LoadUnlockWatchingVideoReward);

	RegisterCallbackFunc("config_crystal_shop.txt",					LoadCrystalShop);
	RegisterCallbackFunc("config_crystal_shop_slot.txt",			LoadCrystalShopSlot);	

	// 世界等级任务
	RegisterCallbackFunc("world_task_level.txt",					LoadWorldTaskLevel);
	RegisterCallbackFunc("world_level_task_id_list.txt",			LoadWorldLevelTaskIdList);
	RegisterCallbackFunc("world_level_task_event.txt",				LoadWorldLevelTaskEvent);

	// 资源战
	RegisterCallbackFunc("resource_war_shop.txt",					LoadResourceWarShop);
	RegisterCallbackFunc("resource_war_chapter.txt",				LoadResourceWarChapter);
	RegisterCallbackFunc("resource_war_resource.txt",				LoadResourceWarResource);
	RegisterCallbackFunc("resource_war_technology.txt",				LoadResourceWarTechnology);
	RegisterCallbackFunc("resource_war_guild_profit.txt",			LoadResourceWarGuildProfit);

	// 工会战
	RegisterCallbackFunc("guild_war_star.txt",						LoadGuildWarStar);
	RegisterCallbackFunc("guild_war_tower.txt",						LoadGuildWarTower);
	RegisterCallbackFunc("guild_war_rank_reward.txt",				LoadGuildWarRankReward);
	RegisterCallbackFunc("guild_war_vs_reward.txt",					LoadGuildWarVSReward);
	RegisterCallbackFunc("guild_war_vs_activity_time.txt",			LoadGuildWarVSActivityTime);
	RegisterCallbackFunc("guild_war_common.txt",					LoadGuildWarCommonCfg);
	RegisterCallbackFunc("guild_war_region.txt",					LoadGuildWarRegion);
	RegisterCallbackFunc("guild_war_organize_defence.txt",			LoadGuildWarOrganizeDefense);

	// ygame相关
	RegisterCallbackFunc("ygame_payment.txt",						LoadYGamePayment);

	// 充值排行
	RegisterCallbackFunc("recharge_rank_reward.txt",				LoadRechargeRankReward);

	// GM商店
	RegisterCallbackFunc("config_gm_shop.txt",						LoadGMShop);
	
	// 挂机相关
	RegisterCallbackFunc("hang_up_drop.txt",						LoadHangUpDrop);
	

	// 活动码注册
	//RegisterActivationCodeFile();

	// 加载文件
	UINT64 lCurTickCount = g_GetCurrentTime();
	if (!LoadAllFile())
	{
		MAPLOG(ERROR_LEVEL, "Load config file fail!");
		return false;
	}
	MAPLOG(GUI_LEVEL, "Load all config file success[%d]", g_GetCurrentTime() - lCurTickCount);

	return true;
}