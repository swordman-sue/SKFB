
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Common = ConfigAdapter.Common or {}

--每天可进行普通免费召唤的次数
function ConfigAdapter.Common.GetLowRecruitFreeTimes()
	local config_common = CPPGameLib.GetConfig("Common", 1)	
	return config_common.int_value
end

--普通召唤CD(单位:分)
function ConfigAdapter.Common.GetLowRecruitFreeTimeCD()
	local config_common = CPPGameLib.GetConfig("Common", 2)	
	return config_common.int_value * 60
end

--高级召唤CD(单位:分)
function ConfigAdapter.Common.GetHighRecruitFreeTimeCD()
	local config_common = CPPGameLib.GetConfig("Common", 3)	
	return config_common.int_value * 60
end

--普通召唤消耗的物品ID
function ConfigAdapter.Common.GetLowRecruitConsumeGoodID()
	local config_common = CPPGameLib.GetConfig("Common", 4)	
	return config_common.int_value
end

--高级召唤消耗的物品ID
function ConfigAdapter.Common.GetHighRecruitConsumeGoodID()
	local config_common = CPPGameLib.GetConfig("Common", 5)	
	return config_common.int_value
end

--高级召唤消耗的元宝
function ConfigAdapter.Common.GetHighRecruitConsumeGoods()
	local config_common = CPPGameLib.GetConfig("Common", 6)	
	return config_common.int_value
end

--高级批量召唤消耗的元宝
function ConfigAdapter.Common.GetHighMoreRecruitConsumeGoods()
	local config_common = CPPGameLib.GetConfig("Common", 7)	
	return config_common.int_value
end

--高级批量召唤的次数
function ConfigAdapter.Common.GetHighRecruitMoreTimes()
	local config_common = CPPGameLib.GetConfig("Common", 8)	
	return config_common.int_value
end

--普通批量召唤的次数
function ConfigAdapter.Common.GetLowRecruitMoreTimes()
	local config_common = CPPGameLib.GetConfig("Common", 9)	
	return config_common.int_value
end

--高级召唤必出指定英雄需要的次数
function ConfigAdapter.Common.GetHighRecruitMustShowHero()
	local config_common = CPPGameLib.GetConfig("Common", 10)	
	return config_common.int_value
end

--公共怒气上限值
function ConfigAdapter.Common.GetMaxCommonAnger()
	local config_common = CPPGameLib.GetConfig("Common", 11)	
	return config_common.value_list[2]
end

--公共怒气爆点值
function ConfigAdapter.Common.GetCommonAngerStepVar()
	local config_common = CPPGameLib.GetConfig("Common", 11)	
	return config_common.value_list[1]
end

--获取技能公共CD(毫秒)
function ConfigAdapter.Common.GetSkillCommonCD()
	local config_common = CPPGameLib.GetConfig("Common", 13)
	return config_common.int_value
end

--获取近战技能起手范围(像素)
function ConfigAdapter.Common.GetTriggerRangeOfNearSkill()
	local config_common = CPPGameLib.GetConfig("Common", 12)
	return config_common.int_value	
end

--获取近战技能碰撞范围(像素)
function ConfigAdapter.Common.GetCollisionRangeOfNearSkill()
	local config_common = CPPGameLib.GetConfig("Common", 14)
	return config_common.int_value	
end

--获取碰撞后的击退距离(像素)
function ConfigAdapter.Common.GetBeatBackDistanceOfCollision()
	local config_common = CPPGameLib.GetConfig("Common", 15)
	return config_common.int_value	
end

--获取近战技能攻击前移速度系数
function ConfigAdapter.Common.GetAttackMoveSpeedFactorOfNearSkill()
	local config_common = CPPGameLib.GetConfig("Common", 28)
	return config_common.int_value	
end

--获取碰撞特效ID
function ConfigAdapter.Common.GetCollisionEffectID()
	local config_common = CPPGameLib.GetConfig("Common", 16)
	return config_common.int_value	
end

--获取每次可获得的精力点
function ConfigAdapter.Common.GetEnergyNum()
	local config_common = CPPGameLib.GetConfig("Common", 18)
	return config_common.int_value
end

--获取每次可赠送的精力点数
function ConfigAdapter.Common.GiveEnergyNum()
	local config_common = CPPGameLib.GetConfig("Common", 19)
	return config_common.int_value
end

--获取战斗中出生点与边界的最小间隔
function ConfigAdapter.Common.GetBattleBirthPosBoundaryMargin()
	local config_common = CPPGameLib.GetConfig("Common", 24)
	return config_common.int_value
end

--获取战斗中同职业队友出生点间隔
function ConfigAdapter.Common.GetBattleSameProfessionTeammateSpace()
	local config_common = CPPGameLib.GetConfig("Common", 25)
	return config_common.int_value
end

--获取战斗中不同职业队友出生点间隔加成
function ConfigAdapter.Common.GetBattleDiffProfessionTeammateSpaceAdd()
	local config_common = CPPGameLib.GetConfig("Common", 26)	
	return config_common.int_value
end

--重置爬塔消耗钻石
function ConfigAdapter.Common.GetTowerConsume()
	local config_common = CPPGameLib.GetConfig("Common", 30)	
	return config_common.value_list
end

--购买爬塔挑战次数消耗
function ConfigAdapter.Common.GetTowerEliteConsume()
	local config_common = CPPGameLib.GetConfig("Common", 31)	
	return config_common.value_list
end

--爬塔精英关卡每日免费挑战次数
function ConfigAdapter.Common.GetTowerEliteItem()
	local config_common = CPPGameLib.GetConfig("Common", 32)	
	return config_common.int_value
end

--普通批量召唤的次数
function ConfigAdapter.Common.GetRiotSuppressTimes()
	local config_common = CPPGameLib.GetConfig("Common", 33)	
	return config_common.int_value
end

--免战牌(小)
function ConfigAdapter.Common.GetAvoidWarCardSmallId()
	local config_common = CPPGameLib.GetConfig("Common", 34)	
	return config_common.int_value
end

--免战牌(大)
function ConfigAdapter.Common.GetAvoidWarCardBigId()
	local config_common = CPPGameLib.GetConfig("Common", 35)	
	return config_common.int_value
end

--全力一击消耗减半时间[开始时间|结束时间]
function ConfigAdapter.Common.GetRebelConsumeHalve()
	local config_common = CPPGameLib.GetConfig("Common", 36)	
	return config_common.value_list
end

--攻打叛军功勋翻倍时间[开始时间|结束时间]
function ConfigAdapter.Common.GetRebelExploitDouble()
	local config_common = CPPGameLib.GetConfig("Common", 37)	
	return config_common.value_list
end

--叛军普通攻击消耗(通缉令)
function ConfigAdapter.Common.GetRebelConsume1()
	local config_common = CPPGameLib.GetConfig("Common", 38)	
	return config_common.int_value
end

--叛军全力一击攻击消耗(通缉令)
function ConfigAdapter.Common.GetRebelConsume2()
	local config_common = CPPGameLib.GetConfig("Common", 39)	
	return config_common.int_value
end

--夺宝攻打玩家的关卡ID
function ConfigAdapter.Common.GetPlunderedTreasuresGameLevelID()
	local config_common = CPPGameLib.GetConfig("Common", 41)	
	return config_common.int_value
end

--获取战斗加速表
function ConfigAdapter.Common.GetBattleACCList()
	local config_common = CPPGameLib.GetConfig("Common", 43)	
	return config_common.value_list
end

--竞技场关卡ID
function ConfigAdapter.Common.GetArenaGameLevelID()
	local config_common = CPPGameLib.GetConfig("Common", 44)	
	return config_common.int_value
end

--公会限时商城刷新时间
function ConfigAdapter.Common.GetUnionLimitTimeUp()
	local config_common = CPPGameLib.GetConfig("Common", 45)	
	return config_common.value_list
end

--英雄商店免费刷新恢复间隔时间
function ConfigAdapter.Common.GetHeroShopFreeFlush()
	local config_common = CPPGameLib.GetConfig("Common", 46)	
	return config_common.int_value
end

--英雄商店消耗刷新的物品ID
function ConfigAdapter.Common.GetHeroShopConsumeGoods()
	local config_common = CPPGameLib.GetConfig("Common", 47)	
	return config_common.int_value
end

--英雄商店刷新消耗的雄魂值
function ConfigAdapter.Common.GetHeroShopConsumeNum()
	local config_common = CPPGameLib.GetConfig("Common", 48)	
	return config_common.int_value
end

--英雄商店每日免费刷新次数
function ConfigAdapter.Common.GetHeroShopFreeNum()
	local config_common = CPPGameLib.GetConfig("Common", 49)	
	return config_common.int_value
end

--主线副本扫荡开启的星数条件
function ConfigAdapter.Common.GetMainDungeonCleanOpenCondition_Star()
	local config_common = CPPGameLib.GetConfig("Common", 50)	
	return config_common.int_value	
end

--主线副本扫荡开启的角色等级条件
function ConfigAdapter.Common.GetMainDungeonCleanOpenCondition_RoleLevel()
	local config_common = CPPGameLib.GetConfig("Common", 51)	
	return config_common.int_value
end

--主线副本扫荡的默认次数
function ConfigAdapter.Common.GetMainDungeonCleanDefaultTimes()
	local config_common = CPPGameLib.GetConfig("Common", 52)	
	return config_common.int_value	
end

--世界BOSS复活时间（秒）
function ConfigAdapter.Common.GetWorldBossResurgence()
	local config_common = CPPGameLib.GetConfig("Common", 54)	
	return config_common.int_value	
end

--世界BOSS回复次数CD（秒）
function ConfigAdapter.Common.GetWorldBossChallengeCD()
	local config_common = CPPGameLib.GetConfig("Common", 151)	
	return config_common.int_value	
end

--精炼所需道具
function ConfigAdapter.Common.GetRefineItemLst()
	local config_common = CPPGameLib.GetConfig("Common", 17)	
	return config_common.value_list	
end

--主线副本重置的消耗金钱
function ConfigAdapter.Common.GetMainDungeonCleanReset()
	local config_common = CPPGameLib.GetConfig("Common", 61)	
	return config_common.value_list	
end

--摇钱树累积奖励次数
function ConfigAdapter.Common.GetAggregateMoneyNum()
	local config_common = CPPGameLib.GetConfig("Common", 62)	
	return config_common.int_value	
end

--摇钱树领取奖励的间隔时间(分)
function ConfigAdapter.Common.GetAggregateMoneyTime()
	local config_common = CPPGameLib.GetConfig("Common", 63)	
	return config_common.int_value * 60 
end

--摇钱树每日领取奖励的次数上限
function ConfigAdapter.Common.GetAggregateMoneyMax()
	local config_common = CPPGameLib.GetConfig("Common", 64)	
	return config_common.int_value	
end

--觉醒商店刷新消耗的将魂值
function ConfigAdapter.Common.GetAwakenShopCusumeNum()
	local config_common = CPPGameLib.GetConfig("Common", 67)	
	return config_common.int_value
end

--铜雀台领取的体力值
function ConfigAdapter.Common.GetManual()
	local config_common = CPPGameLib.GetConfig("Common", 65)	
	return config_common.int_value
end

--觉醒商店每日免费刷新次数
function ConfigAdapter.Common.GetAwakenShopFreeNum()
	local config_common = CPPGameLib.GetConfig("Common", 66)	
	return config_common.int_value
end

--觉醒商店刷新CD
function ConfigAdapter.Common.GetAwakenShopRefrashCD()
	local config_common = CPPGameLib.GetConfig("Common", 68)	
	return config_common.int_value
end

--公会副本次数购买价钱
function ConfigAdapter.Common.GetUnionEctypeTime()
	local config_common = CPPGameLib.GetConfig("Common", 69)	
	return config_common.value_list
end

--世界boss体力购买价钱
function ConfigAdapter.Common.GetWorldBossBuyPrice()
	local config_common = CPPGameLib.GetConfig("Common", 70)	
	return config_common.value_list
end

--挑战竞技场消耗的精力
function ConfigAdapter.Common.GetArenaCostEnery()
	local config_common = CPPGameLib.GetConfig("Common", 71)	
	return config_common.int_value
end

--世界boss默认次数
function ConfigAdapter.Common.GetWorldBossTime()
	local config_common = CPPGameLib.GetConfig("Common", 53)	
	return config_common.int_value
end

--击退初速度
function ConfigAdapter.Common.GetBeatbackInitSpeed()
	local config_common = CPPGameLib.GetConfig("Common", 74)	
	return config_common.int_value
end

--合击镜头放大倍数
function ConfigAdapter.Common.GetJointSkillCameraZoomFactor()
	local config_common = CPPGameLib.GetConfig("Common", 76)	
	return config_common.int_value
end

--合击镜头拉近时间
function ConfigAdapter.Common.GetJointSkillCameraZoomTime()
	local config_common = CPPGameLib.GetConfig("Common", 77)	
	return config_common.int_value
end

--合击镜头恢复时间
function ConfigAdapter.Common.GetJointSkillCameraRecoverTime()
	local config_common = CPPGameLib.GetConfig("Common", 78)	
	return config_common.int_value
end

--战斗胜利后的速度系数
function ConfigAdapter.Common.GetBattleSuccessSpeedFactor()
	local config_common = CPPGameLib.GetConfig("Common", 79)	
	return config_common.int_value
end

--过场的速度系数
function ConfigAdapter.Common.GetBattleChangeSpeedFactor()
	local config_common = CPPGameLib.GetConfig("Common", 80)	
	return config_common.int_value
end

--战斗结束的慢镜时长
function ConfigAdapter.Common.GetSlowMirrorTime()
	local config_common = CPPGameLib.GetConfig("Common", 84)	
	return config_common.int_value
end

--战斗结束的慢镜初始速度系数
function ConfigAdapter.Common.GetSlowMirrorInitFactor()
	local config_common = CPPGameLib.GetConfig("Common", 85)	
	return config_common.int_value
end

--跑步烟尘特效
function ConfigAdapter.Common.GetBattleRunEffect()
	local config_common = CPPGameLib.GetConfig("Common", 87)	
	return config_common.int_value
end

--击退烟尘特效
function ConfigAdapter.Common.GetBattleRepelEffect()
	local config_common = CPPGameLib.GetConfig("Common", 88)	
	return config_common.int_value
end


--爬塔扫荡需要角色等级
function ConfigAdapter.Common.GetSweepLv()
	local config_common = CPPGameLib.GetConfig("Common", 90)	
	return config_common.int_value
end

--夺宝消耗精力
function ConfigAdapter.Common.GetPlunderedTreasureCostEnery()
	local config_common = CPPGameLib.GetConfig("Common", 91)	
	return config_common.int_value
end

--夺宝5次消耗精力
function ConfigAdapter.Common.GetPlunderedTreasureFiveTimesCostEnery()
	local config_common = CPPGameLib.GetConfig("Common", 92)	
	return config_common.int_value
end

--积分赛每日免费挑战次数
function ConfigAdapter.Common.GetIntegralRaceFreeChallengeTimes()
	local config_common = CPPGameLib.GetConfig("Common", 95)	
	return config_common.int_value
end

--培养5次需要的vip等级或者战队等级
function ConfigAdapter.Common.GetHeroTrainFiveTimesNeedVipLevelAndLevel()
	local config_common = CPPGameLib.GetConfig("Common", 98)	
	return config_common.value_list
end

--培养10次需要的vip等级或者战队等级
function ConfigAdapter.Common.GetHeroTrainTenTimesNeedVipLevelAndLevel()
	local config_common = CPPGameLib.GetConfig("Common", 99)	
	return config_common.value_list
end

--培养所需最低英雄等级
function ConfigAdapter.Common.GetHeroTrainmMinimumNeedLevel()
	local config_common = CPPGameLib.GetConfig("Common", 102)	
	return config_common.int_value
end

--进阶石
function ConfigAdapter.Common.GetHeroBreakPropId()
	local config_common = CPPGameLib.GetConfig("Common", 104)	
	return config_common.int_value
end

--培养石
function ConfigAdapter.Common.GetHeroTrainPropId()
	local config_common = CPPGameLib.GetConfig("Common", 89)	
	return config_common.int_value
end

--主城没有指引的情况下添加前往主线副本指引的限制等级
function ConfigAdapter.Common.GetMainCityViewAddLeadLimitLevel()
	local config_common = CPPGameLib.GetConfig("Common", 105)	
	return config_common.int_value
end

--体力药剂ID
function ConfigAdapter.Common.GetStaminaMedicineID()
	local config_common = CPPGameLib.GetConfig("Common", 82)	
	return config_common.int_value
end

--夺宝抢夺5次功能预告等级,开放Vip等级,角色等级
function ConfigAdapter.Common.GetPlunderedTreasureFiveTimesMessageList()
	local config_common = CPPGameLib.GetConfig("Common", 107)	
	return config_common.value_list
end

--获取3倍战斗加速开启的角色等级
function ConfigAdapter.Common.GetBattleAccMaxOpenLev()
	local config_common = CPPGameLib.GetConfig("Common", 108)	
	return config_common.int_value	
end

--获取月卡持续时间
function ConfigAdapter.Common.GetMonthCardContinue()
	local config_common = CPPGameLib.GetConfig("Common", 110)	
	return config_common.int_value	
end

--获取副本BGM音量
function ConfigAdapter.Common.GetDungeonBGMVolume()
	local config_common = CPPGameLib.GetConfig("Common", 112)	
	return config_common.int_value	
end

--获取战斗跳过的加速
function ConfigAdapter.Common.GetBattleSkipACC()
	local config_common = CPPGameLib.GetConfig("Common", 113)	
	return config_common.int_value	
end

--公会创建消耗钻石
function ConfigAdapter.Common.GetUnionEstablishConsume()
	local config_common = CPPGameLib.GetConfig("Common", 114)	
	return config_common.int_value	
end

--公会创建所需VIP
function ConfigAdapter.Common.GetUnionEstablishDesiredVip()
	local config_common = CPPGameLib.GetConfig("Common", 115)	
	return config_common.int_value	
end

--装备一键强化需要的VIP等级
function ConfigAdapter.Common.GetOneKeyIntensifyVipLevel()
	local config_common = CPPGameLib.GetConfig("Common", 116)	
	return config_common.int_value	
end

--装备一键强化需要的角色等级
function ConfigAdapter.Common.GetOneKeyIntensifyRoleLevel()
	local config_common = CPPGameLib.GetConfig("Common", 117)	
	return config_common.int_value	
end

--公会可接受同时多少人申请
function ConfigAdapter.Common.GetUnionEntryNumMax()
	local config_common = CPPGameLib.GetConfig("Common", 118)	
	return config_common.int_value	
end

--弹劾会长需要的离线时间(小时)
function ConfigAdapter.Common.GetUnionTreasonTime()
	local config_common = CPPGameLib.GetConfig("Common", 120)	
	return config_common.int_value	
end

--好友每日可助战次数
function ConfigAdapter.Common.GetDaliyCouldAssistTimes()
	local config_common = CPPGameLib.GetConfig("Common", 122)	
	return config_common.int_value	
end

--每日取获得的友情点上限
function ConfigAdapter.Common.GetDaliyFriendPointLimit()
	local config_common = CPPGameLib.GetConfig("Common", 123)	
	return config_common.int_value	
end

--好友助战获得的友情点
function ConfigAdapter.Common.GetAssistFriendPoint()
	local config_common = CPPGameLib.GetConfig("Common", 125)	
	return config_common.int_value	
end

--获得七天活动进度上限
function ConfigAdapter.Common.GetSevenActivityMAX()
	local config_common = CPPGameLib.GetConfig("Common", 127)	
	return config_common.int_value	
end

--公告开启等级
function ConfigAdapter.Common.GetNoticeDisparkLevel()
	local config_common = CPPGameLib.GetConfig("Common", 128)	
	return config_common.int_value	
end

--友情点招募消耗
function ConfigAdapter.Common.GetFriendshipPointConsume()
	local config_common = CPPGameLib.GetConfig("Common", 129)	
	return config_common.value_list	
end

--工会副本活动时间[开始时间|结束时间]
function ConfigAdapter.Common.GetUnionFbStartAndEndTime()
	local config_common = CPPGameLib.GetConfig("Common", 132)	
	return config_common.value_list	
end

--工会副本每日默认挑战次数
function ConfigAdapter.Common.GetUnionDefaultChallengeTime()
	local config_common = CPPGameLib.GetConfig("Common", 133)	
	return config_common.int_value	
end

--挑战次数回复时间(分钟)
function ConfigAdapter.Common.GetUnionDefaultChallengeCD()
	local config_common = CPPGameLib.GetConfig("Common", 134)	
	return config_common.int_value	
end

--游戏帮助公告(秒)
function ConfigAdapter.Common.GetGameHelpNoticeCD()
	local config_common = CPPGameLib.GetConfig("Common", 136)	
	return config_common.int_value * 60
end

--可以使用精灵的副本类型
function ConfigAdapter.Common.GetCouldUseSpiritDungeonTypeList()
	local config_common = CPPGameLib.GetConfig("Common", 141)	
	return config_common.value_list	
end

--可以使用助战的副本类型
function ConfigAdapter.Common.GetCouldUseAssistFriendDungeonTypeList()
	local config_common = CPPGameLib.GetConfig("Common", 142)	
	return config_common.value_list	
end

--留言间隔时间
function ConfigAdapter.Common.GetGameLevelCommentCD()
	local config_common = CPPGameLib.GetConfig("Common", 138)	
	return config_common.int_value	
end

--精英副本重置的消耗金钱
function ConfigAdapter.Common.GetEliteDungeonCleanReset()
	local config_common = CPPGameLib.GetConfig("Common", 97)	
	return config_common.value_list	
end

--关卡留言长度
function ConfigAdapter.Common.GetGameLevelCommentLength()
	local config_common = CPPGameLib.GetConfig("Common", 149)	
	return config_common.int_value	
end

--获取PVP副本的关卡ID
function ConfigAdapter.Common.GetPVPGameLevelID()
	local config_common = CPPGameLib.GetConfig("Common", 150)	
	return config_common.int_value
end

--获取PVP副本加速系数
function ConfigAdapter.Common.GetPVPACCFactor()
	local config_common = CPPGameLib.GetConfig("Common", 152)	
	return config_common.int_value
end

--获取PVP可获得的奖励次数
function ConfigAdapter.Common.GetPVPAwardTime()
	local config_common = CPPGameLib.GetConfig("Common", 153)	
	return config_common.int_value
end

--获取基金的购买等级
function ConfigAdapter.Common.GetFundBuyVipLevel()
	local config_common = CPPGameLib.GetConfig("Common", 156)	
	return config_common.int_value
end

--获取基金的购买消耗
function ConfigAdapter.Common.GetFundBuyConsume()
	local config_common = CPPGameLib.GetConfig("Common", 157)	
	return config_common.int_value
end

--怪物攻城复活区域
function ConfigAdapter.Common.GetMonsterStormCastleArea()
	local config_common = CPPGameLib.GetConfig("Common", 158)	
	return config_common.value_list
end

--怪物攻城英雄缩放比例
function ConfigAdapter.Common.GetMonsterStormCastleHeroZoom()
	local config_common = CPPGameLib.GetConfig("Common", 159)	
	return config_common.int_value
end

--怪物攻城场景移动速度
function ConfigAdapter.Common.GetMonsterStormCastleSceneSpeed()
	local config_common = CPPGameLib.GetConfig("Common", 160)	
	return config_common.value_list
end

--怪物攻城可移动区域
function ConfigAdapter.Common.GetMonsterStormCastleCouldMoveArea()
	local config_common = CPPGameLib.GetConfig("Common", 161)	
	return config_common.value_list
end

--怪物攻城人物移动速度
function ConfigAdapter.Common.GetMonsterStormCastlePlayerSpeed()
	local config_common = CPPGameLib.GetConfig("Common", 162)	
	return config_common.int_value
end

--怪物攻城宝箱开启时间
function ConfigAdapter.Common.GetMonsterStormCastleOpenBoxTime()
	local config_common = CPPGameLib.GetConfig("Common", 166)	
	return config_common.int_value
end

--怪物攻城复活花费
function ConfigAdapter.Common.GetMonsterStormCastleResurgenceCost()
	local config_common = CPPGameLib.GetConfig("Common", 168)	
	return config_common.int_value
end

--刷完Boss刷下一波小怪CD
function ConfigAdapter.Common.GetMonsterStormCastleKillBossRefrashCD()
	local config_common = CPPGameLib.GetConfig("Common", 171)	
	return config_common.int_value
end

--点击的时候字幕滚动加速的倍率
function ConfigAdapter.Common.GetTouchScreenAddSpeed()
	local config_common = CPPGameLib.GetConfig("Common", 173)	
	return config_common.int_value
end


--无尽之地激战消耗无尽石
function ConfigAdapter.Common.GetCollisionConsume()
	local config_common = CPPGameLib.GetConfig("Common", 174)	
	return config_common.int_value
end

--无尽之地每天碰撞奖励次数
function ConfigAdapter.Common.GetCollisionRewardTimes()
	local config_common = CPPGameLib.GetConfig("Common", 176)	
	return config_common.int_value
end

--无尽之地免费次数
function ConfigAdapter.Common.GetCollisionFreeTimes()
	local config_common = CPPGameLib.GetConfig("Common", 177)	
	return config_common.int_value
end

--无尽之地结束恢复
function ConfigAdapter.Common.GetCollisionEndRecover()
	local config_common = CPPGameLib.GetConfig("Common", 178)	
	return config_common.int_value * 0.0001
end

--获取战斗镜头相关
function ConfigAdapter.Common.GetBattleCameraInfo()
	if ConfigAdapter.Common.battle_camera_info then
		return ConfigAdapter.Common.battle_camera_info
	end

	local tbl = 
	{
		["boundary"] = 179, 
		["init_scale"] = 180, 
		["max_scale"] = 181, 
		["zoom_out_speed"] = 182, ["zoom_in_speed"] = 183, ["move_speed"] = 184,
		["terrain_obj_factor"] = 187,
	}

	local info = {}
	local config_common
	for k, v in pairs(tbl) do
		config_common = CPPGameLib.GetConfig("Common", v)
		if config_common then
			info[k] = config_common.int_value
		end
	end
	ConfigAdapter.Common.battle_camera_info = info

	return info
end

--获取战斗镜头缩放相关
function ConfigAdapter.Common.GetBattleCameraScaleInfo()
	if ConfigAdapter.Common.battle_camera_scale_info then
		return ConfigAdapter.Common.battle_camera_scale_info
	end

	local info = {}
	local config_common = CPPGameLib.GetConfig("Common", 185)
	local i = 1
	while config_common.value_list[i] do
		local render_queue = config_common.value_list[i]
		local factor = config_common.value_list[i + 1]
		info[render_queue] = factor
		i = i + 2
	end
	ConfigAdapter.Common.battle_camera_scale_info = info

	return info
end

--获取无尽之地的关卡ID
function ConfigAdapter.Common.GetChasmGameLevelID()
	local config_common = CPPGameLib.GetConfig("Common", 186)	
	return config_common.int_value
end

--获取次日登陆奖励
function ConfigAdapter.Common.GetMorrowAward()
	local config_common = CPPGameLib.GetConfig("Common", 188)	
	return config_common.int_value
end

--公会副会长上限
function ConfigAdapter.Common.GetUnionOfficeMax()
	local config_common = CPPGameLib.GetConfig("Common", 195)	
	return config_common.int_value
end

--英雄一键升级需要等级
function ConfigAdapter.Common.GetHeroCouldOneKeyLevelUpRoleLevel()
	local config_common = CPPGameLib.GetConfig("Common", 196)	
	return config_common.int_value
end

--我要变强软指引
function ConfigAdapter.Common.GetStrongerWeakLeadRoleLevel()
	local config_common = CPPGameLib.GetConfig("Common", 197)	
	return config_common.int_value
end

--英雄等级上限
function ConfigAdapter.Common.GetHeroLevelLimit()
	local config_common = CPPGameLib.GetConfig("Common", 200)	
	return config_common.int_value
end

--外域入侵刷新时间
function ConfigAdapter.Common.GetDungeonExternalAreaRefrashTimeList()
	local config_common = CPPGameLib.GetConfig("Common", 124)	
	return config_common.value_list	
end

--直购优惠刷新
function ConfigAdapter.Common.GetDirectBuyUpTime()
	local config_common = CPPGameLib.GetConfig("Common", 201)	
	return config_common.value_list	
end

--获得特惠季时间
function ConfigAdapter.Common.GetDirectBuyShop1Time()
	local config_common = CPPGameLib.GetConfig("Common", 205)	

	return config_common.string_value , config_common.value_list
end

--获得特惠季时间
function ConfigAdapter.Common.GetDirectBuyShop2Time()
	local config_common = CPPGameLib.GetConfig("Common", 206)	

	return config_common.string_value	, config_common.value_list
end

--获得特惠季时间
function ConfigAdapter.Common.GetChatSysOpenLevel()
	local config_common = CPPGameLib.GetConfig("Common", 208)	
	return config_common.int_value
end

--获得怪物攻城战斗CD
function ConfigAdapter.Common.GetMonsterStormCastleBattleCD()
	local config_common = CPPGameLib.GetConfig("Common", 209)	
	return config_common.int_value
end

--弹劾所需消耗
function ConfigAdapter.Common.GetUnionAccuseConsume()
	local config_common = CPPGameLib.GetConfig("Common", 212)	
	return config_common.int_value
end

--副本弱指引所需等级
function ConfigAdapter.Common.GetDungeonWeakLeadNeedLevel()
	local config_common = CPPGameLib.GetConfig("Common", 213)	
	return config_common.int_value
end

--聊天CD
function ConfigAdapter.Common.GetChatCD()
	local config_common = CPPGameLib.GetConfig("Common", 218)	
	return config_common.int_value
end

--聊天CD
function ConfigAdapter.Common.GetPushShopShowDistrict()
	local config_common = CPPGameLib.GetConfig("Common", 219)	
	return config_common.value_list
end

--组队答题时长
function ConfigAdapter.Common.GetTeamQuestionTime()
	local config_common = CPPGameLib.GetConfig("Common", 214)	
	return config_common.int_value
end

--组队最大答题数
function ConfigAdapter.Common.GetTeamMaxQuestionNum()
	local config_common = CPPGameLib.GetConfig("Common", 215)	
	return config_common.int_value
end

--组队扭蛋币
function ConfigAdapter.Common.GetTeamGashaponMachineCoin()
	local config_common = CPPGameLib.GetConfig("Common", 211)	
	return config_common.int_value
end

--怪物攻城活动结束后延迟多久发奖励(秒)
function ConfigAdapter.Common.GetMonsterStormCastleDelayTimeToReward()
	local config_common = CPPGameLib.GetConfig("Common", 192)	
	return config_common.int_value
end

--天梯赛王者段位胜利后的积分增加值
function ConfigAdapter.Common.GetLadderSeniorBattleResultAddScore()
	local config_common = CPPGameLib.GetConfig("Common", 221)	
	return config_common.int_value	
end

--天梯赛关卡ID
function ConfigAdapter.Common.GetLadderGameLevelID()
	local config_common = CPPGameLib.GetConfig("Common", 222)	
	return config_common.int_value
end

--天梯赛开始时间
function ConfigAdapter.Common.GetLadderOpenTime()
	local config_common = CPPGameLib.GetConfig("Common", 223)	
	return config_common.int_value
end

--天梯赛关闭时间
function ConfigAdapter.Common.GetLadderCloseTime()
	local config_common = CPPGameLib.GetConfig("Common", 224)	
	return config_common.int_value
end

--开服后几天进天梯
function ConfigAdapter.Common.GetLadderEnterTime()
	local config_common = CPPGameLib.GetConfig("Common", 225)	
	return config_common.int_value
end

--装备一键精炼
function ConfigAdapter.Common.GetEquipOneKeyRefine()
	local config_common = CPPGameLib.GetConfig("Common", 226)	
	return config_common.int_value
end

--圣器一键强化
function ConfigAdapter.Common.GetTreasureOneKeyIntensify()
	local config_common = CPPGameLib.GetConfig("Common", 227)	
	return config_common.int_value
end

--宝石缩放系数
function ConfigAdapter.Common.GetGemScaleFactor()
	local config_common = CPPGameLib.GetConfig("Common", 229)	
	return config_common.int_value
end

--BOSS战通用时间
function ConfigAdapter.Common.GetTeamDungeonBattleTime()
	local config_common = CPPGameLib.GetConfig("Common", 230)	
	return config_common.int_value
end

--王者赛出战对象血量加成
function ConfigAdapter.Common.GetLadderBattleObjHpFactor()
	local config_common = CPPGameLib.GetConfig("Common", 231)	
	return config_common.int_value
end

--组队世界地图英雄缩放比例
function ConfigAdapter.Common.GetTeamWorldMapHeroZoom()
	local config_common = CPPGameLib.GetConfig("Common", 232)	
	return config_common.int_value
end

--世界地图移动速度
function ConfigAdapter.Common.GetTeamWorldMapPlayerSpeed()
	local config_common = CPPGameLib.GetConfig("Common", 233)	
	return config_common.int_value
end

--一键抢夺的开放条件
function ConfigAdapter.Common.GetOneKeyPlunderedtreasuresOpenCondition()
	local config_common = CPPGameLib.GetConfig("Common", 235)	
	return config_common.value_list
end

--精灵一键培养的开放条件
function ConfigAdapter.Common.GetOneKeySpiritTrainOpenCondition()
	local config_common = CPPGameLib.GetConfig("Common", 236)	
	return config_common.int_value
end

--潜能自动升级的开放条件
function ConfigAdapter.Common.GetHeroPotentialAutoLevelUpOpenCondition()
	local config_common = CPPGameLib.GetConfig("Common", 237)	
	return config_common.int_value
end

--占卜CD（单位:分）
function ConfigAdapter.Common.GetLowBDivinationCD()
	local config_common = CPPGameLib.GetConfig("Common", 239)	
	return config_common.int_value
end

--占卜免费次数上限
function ConfigAdapter.Common.GetLowBDivinationFreeMax()
	local config_common = CPPGameLib.GetConfig("Common", 240)	
	return config_common.int_value
end

--占卜消耗钻石量
function ConfigAdapter.Common.GetLowBDivinationExpend()
	local config_common = CPPGameLib.GetConfig("Common", 241)	
	return config_common.value_list
end

--占星CD（单位:分）
function ConfigAdapter.Common.GetTyrantDivinationCD()
	local config_common = CPPGameLib.GetConfig("Common", 242)	
	return config_common.int_value
end

--占星消耗钻石量
function ConfigAdapter.Common.GetTyrantDivinationExpend()
	local config_common = CPPGameLib.GetConfig("Common", 243)	
	return config_common.value_list
end

--占星消耗钻石量
function ConfigAdapter.Common.GetStarShopRefreshTime()
	local config_common = CPPGameLib.GetConfig("Common", 244)	
	return config_common.int_value
end

-- 星魂商店每次刷新上限
function ConfigAdapter.Common.GetStarShopRefreshMax()
	local config_common = CPPGameLib.GetConfig("Common", 245)	
	return config_common.int_value
end

--星魂商店刷新消耗的星魂
function ConfigAdapter.Common.GetStarShopRefreshConsume()
	local config_common = CPPGameLib.GetConfig("Common", 246)	
	return config_common.int_value
end

--低级卡组每日免费翻卡次数
function ConfigAdapter.Common.GetLowBDivinationFree()
	local config_common = CPPGameLib.GetConfig("Common", 247)	
	return config_common.int_value
end

--低级卡组翻卡消耗的金币
function ConfigAdapter.Common.GetLowBDivinationDraw()
	local config_common = CPPGameLib.GetConfig("Common", 248)	
	return config_common.int_value
end

--高级卡组翻卡消耗的物品ID
function ConfigAdapter.Common.GetTyrantDivinationDraw()
	local config_common = CPPGameLib.GetConfig("Common", 249)	
	return config_common.int_value
end

--卡牌进阶石id
function ConfigAdapter.Common.GetDivinationBreakPropId()
	local config_common = CPPGameLib.GetConfig("Common", 250)	
	return config_common.int_value
end

--精灵入场技能CD
function ConfigAdapter.Common.GetSpiritSkillCDForBirth()
	local config_common = CPPGameLib.GetConfig("Common", 253)	
	return config_common.int_value
end

--卡牌通用获取途径
function ConfigAdapter.Common.GetCommonDivinationAchievingId()
	local config_common = CPPGameLib.GetConfig("Common", 254)	
	return config_common.int_value
end

--圣诞帽id
function ConfigAdapter.Common.GetChristmasHatId()
	local config_common = CPPGameLib.GetConfig("Common", 256)	
	return config_common.int_value
end

--紫色传承核心
function ConfigAdapter.Common.GetPurpleExpHero()
	local config_common = CPPGameLib.GetConfig("Common", 257)	
	return config_common.int_value
end

--回收装备红点
function ConfigAdapter.Common.GetRecycleEquipRed()
	local config_common = CPPGameLib.GetConfig("Common", 258)	
	return config_common.int_value
end

--日常任务显示等级差值
function ConfigAdapter.Common.GetDaliyTaskShowLevelDifferenceValue()
	local config_common = CPPGameLib.GetConfig("Common", 259)	
	return config_common.int_value
end

--回收英雄红点
function ConfigAdapter.Common.GetRecycleHeroRed()
	local config_common = CPPGameLib.GetConfig("Common", 260)	
	return config_common.int_value
end

--PVP开服天数
function ConfigAdapter.Common.GetPVPOpenFate()
	local config_common = CPPGameLib.GetConfig("Common", 261)	
	return config_common.int_value
end

--英雄培养页签红点数量
function ConfigAdapter.Common.GetHeroTrainRedDotNum()
	local config_common = CPPGameLib.GetConfig("Common", 262)	
	return config_common.int_value
end

--改名消耗的道具数量
function ConfigAdapter.Common.GetRenameExpendNum()
	local config_common = CPPGameLib.GetConfig("Common", 263)	
	return config_common.int_value
end

--改名消耗的道具
function ConfigAdapter.Common.GetRenameExpendItem()
	local config_common = CPPGameLib.GetConfig("Common", 264)	
	return config_common.int_value
end

--公告弹屏显不显示
function ConfigAdapter.Common.GetTemplNoticeShowType()
	if CPPSdkTools.IsInExamine() then
		--审核中，屏蔽
		return Macros.Global.FALSE
	end

	local config_common = CPPGameLib.GetConfig("Common", 265)	
	return config_common.int_value
end

--至尊卡cd
function ConfigAdapter.Common.GetExtremeBuyCD()
	local config_common = CPPGameLib.GetConfig("Common", 266)	
	return config_common.int_value
end

--公告弹屏显不显示
function ConfigAdapter.Common.GetExtremeBuyID()
	local config_common = CPPGameLib.GetConfig("Common", 267)	
	return config_common.int_value
end

--在选章界面弹英雄降临界面的等级
function ConfigAdapter.Common.GetFirstOpenDungeonPopHeroAdventLevel()
	local config_common = CPPGameLib.GetConfig("Common", 268)	
	return config_common.int_value
end

--在推荐阵容界面未操作几秒后加退出手指
function ConfigAdapter.Common.GetRecommendLineupViewShowEffectTime()
	local config_common = CPPGameLib.GetConfig("Common", 270)	
	return config_common.int_value
end

--助战次数
function ConfigAdapter.Common.GetToalAssistTimes()
	local config_common = CPPGameLib.GetConfig("Common", 272)	
	return config_common.int_value
end

--留言弹幕开启等级
function ConfigAdapter.Common.GetMessageBoardOpenLevel()
	local config_common = CPPGameLib.GetConfig("Common", 273)	
	return config_common.int_value
end

--BT服聊天开启
function ConfigAdapter.Common.GetChatOpen()
	local config_common = CPPGameLib.GetConfig("Common", 274)	
	return config_common.int_value
end

--自动释放需要判断条件：剩余总血量/存活英雄的总血量>配置值 ，则这时候自动战斗绝对不会使用治疗类怒气技能
function ConfigAdapter.Common.GetReleaseHealingSkillHPRadioValue()
	local config_common = CPPGameLib.GetConfig("Common", 275)	
	return config_common.int_value
end

--unlock评分界面出现时间
function ConfigAdapter.Common.GetUnlockFBOpenTime()
	local config_common = CPPGameLib.GetConfig("Common", 276)	
	return config_common.int_value
end

--unlock评分界面出现等级
function ConfigAdapter.Common.GetUnlockFBOpenLv()
	local config_common = CPPGameLib.GetConfig("Common", 277)	
	return config_common.int_value
end

--水晶商店刷新次数
function ConfigAdapter.Common.GetCrystalShopUpItem()
	local config_common = CPPGameLib.GetConfig("Common", 278)	
	return config_common.int_value
end

--世界等级任务系数
function ConfigAdapter.Common.GetWorldLevelFactor()
	local config_common = CPPGameLib.GetConfig("Common", 279)	
	return config_common.int_value
end

--精灵技能拖动时场景减速系数
function ConfigAdapter.Common.GetSpiritSkillMoveDeltaSpeedTimeFactor()
	local config_common = CPPGameLib.GetConfig("Common", 280)	
	return config_common.int_value
end

--获取竞技场血量修正系数
function ConfigAdapter.Common.GetArenaHPFactor()
	local config_common = CPPGameLib.GetConfig("Common", 281)	
	return config_common.int_value or 1
end

--获取扭蛋一次消耗钻石数量
function ConfigAdapter.Common.GetGashaponCostDiamondOneTime()
	local config_common = CPPGameLib.GetConfig("Common", 284)	
	return config_common.int_value
end

--获取扭蛋十次消耗钻石数量
function ConfigAdapter.Common.GetGashaponCostDiamondTenTime()
	local config_common = CPPGameLib.GetConfig("Common", 285)	
	return config_common.int_value
end

--礼包单次可开上限
function ConfigAdapter.Common.GetOnceUserMax()
	local config_common = CPPGameLib.GetConfig("Common", 286)	
	if config_common then 
		return config_common.int_value 
	else
		return 100
	end
end

--获取积分赛关卡ID
function ConfigAdapter.Common.GetIntegralRaceGameLevelID()
	local config_common = CPPGameLib.GetConfig("Common", 289)	
	return config_common.int_value 
end

--获取算取可获得荣耀系数      
function ConfigAdapter.Common.GetIntegralRaceHonorNum()
	local config_common = CPPGameLib.GetConfig("Common", 290)	
	return config_common.int_value 
end

--获取资源蓝钻石跳转
function ConfigAdapter.Common.GetDiamondSkip()
	local config_common = CPPGameLib.GetConfig("Common", 291)	
	return config_common.int_value 
end

--gm
function ConfigAdapter.Common.GetVisibleGM()
	local config_common = CPPGameLib.GetConfig("Common", 292)	
	return config_common.int_value 
end  

--资源战军团占领人数
function ConfigAdapter.Common.GetResourceWarUnionNum()
	local config_common = CPPGameLib.GetConfig("Common", 294)	
	return config_common.int_value 
end  

--战斗中的英雄间出生点Y坐标偏移
function ConfigAdapter.Common.GetHeroYOffsetForBattle()
	local config_common = CPPGameLib.GetConfig("Common", 295)	
	return config_common and config_common.int_value or 0
end

--累充倍数
function ConfigAdapter.Common.GetAddUpMultiple()
	local config_common = CPPGameLib.GetConfig("Common", 297)	
	return config_common and config_common.int_value or 10
end

--获取不侵权的英雄ID列表
function ConfigAdapter.Common.GetSafeHeroListForExamine()
	local config_common = CPPGameLib.GetConfig("Common", 298)	
	return config_common and config_common.value_list
end

--获取不侵权的怪物ID列表
function ConfigAdapter.Common.GetSafeMonsterListForExamine()
	local config_common = CPPGameLib.GetConfig("Common", 299)	
	return config_common and config_common.value_list
end


--获取钻石与越南币奖励
function ConfigAdapter.Common.GetVNBturnDiamond()
	local config_common = CPPGameLib.GetConfig("Common", 300)	
	return config_common and config_common.int_value
end

--获得越南充值方式 0是网页 1是协议
function ConfigAdapter.Common.GetVNRechargeType()
	local config_common = CPPGameLib.GetConfig("Common", 301)	
	return config_common and config_common.int_value
end

--获取VIP招募需要的VIP等级
function ConfigAdapter.Common.GetVipRecruitNeedVipLevel()
	local config_common = CPPGameLib.GetConfig("Common", 302)	
	return config_common and config_common.int_value
end

--获取VIP招募消耗的钻石
function ConfigAdapter.Common.GetVipRecruitCostDiamonds()
	local config_common = CPPGameLib.GetConfig("Common", 303)	
	return config_common and config_common.int_value
end

--资源战血量修正
function ConfigAdapter.Common.GetResourceWarHPFactor()
	local config_common = CPPGameLib.GetConfig("Common", 307)	
	return config_common and config_common.int_value
end

--积分赛血量修正
function ConfigAdapter.Common.GetIntegralRaceHPFactor()
	local config_common = CPPGameLib.GetConfig("Common", 308)	
	return config_common and config_common.int_value
end

--获取触发QTE时的镜头拉近偏移量
function ConfigAdapter.Common.GetQTEZoomInCameraOffsetList()
	local config_common = CPPGameLib.GetConfig("Common", 310)	
	return config_common and config_common.value_list
end

--获取触发QTE时的镜头拉近时长
function ConfigAdapter.Common.GetQTEZoomInCameraTime()
	local config_common = CPPGameLib.GetConfig("Common", 311)	
	return config_common and config_common.int_value
end

--获取触发QTE后的暂停时长
function ConfigAdapter.Common.GetQTEPauseTime()
	local config_common = CPPGameLib.GetConfig("Common", 312)	
	return config_common and config_common.int_value
end

--获取触发QTE后暂停后镜头恢复时长
function ConfigAdapter.Common.GetQTEZoomOutCameraTime()
	local config_common = CPPGameLib.GetConfig("Common", 313)	
	return config_common and config_common.int_value
end
--[[
初始大小|缩放时间|完美范围百分比|一般范围百分比
--]]
--获取QTE评分参数相关
function ConfigAdapter.Common.GetQTEGradeParameter()
	local config_common = CPPGameLib.GetConfig("Common", 314)	
	return config_common and config_common.value_list or {1.5, 2, 5, 15}
end

--获取游戏版号信息
function ConfigAdapter.Common.GetGameVersionInfo()
	local config_common = CPPGameLib.GetConfig("Common", 512, false)
	return config_common and config_common.string_value
end

--审核状态下的启动剧情
function ConfigAdapter.Common.GetStoryIDForLaunchExamine()
	local config_common = CPPGameLib.GetConfig("Common", 999, false)
	return config_common and config_common.int_value
end
