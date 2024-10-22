--
-- @brief 英雄招募
-- @auther: ldx
-- @date 2016年9月20日 20:58:28
--

Modules = Modules or {}
Modules.HeroRecruitView = Modules.HeroRecruitView or BaseClass(BaseView)

--处于第几层界面
local HeroRecruitViewState =
{
	MainState = 1,
	RecruitState = 2,
}

function Modules.HeroRecruitView:__init()
 	self.__layout_name = "hero_recruit"
 	self.__bg_params = { type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_recruit")}
 	self.__state = HeroRecruitViewState.MainState
 	self.__title_bar_params = {
 		title = CPPGameLib.GetString("hero_recruitTitle"),
 		show_diamond = true,
 		show_gold = true,
 		show_stamina = true,
 		show_energy = true,
 		close_func = function ( )			
 			if self.__state == HeroRecruitViewState.MainState then
 				self:CloseManual()
 			else
 				if self.__on_recruiting or self.__is_onplaying_action then
 					return
 				end	 						
 				self.__state = HeroRecruitViewState.MainState
 				--播放回退动画	
 				self.LowPanelSpecialEffect:SetVisible(true)
				self.HighPanelSpecialEffect:SetVisible(true)
				self.__vip_panel_specialeffect:SetVisible(true)
				self.__vip_panel_specialeffect:SetVisible(true)
				self.Image_timeBg:SetVisible(false)
				if self.recruitEncourageText then
					self.recruitEncourageText:SetVisible(false)
				end
				self:PlayActionTimeline("closeRecruitView", false,nil)
 			end

 		end
 	}	 	
 	--初级召唤免费次数
 	self.lowFreeTimes = ConfigAdapter.Common.GetLowRecruitFreeTimes()
 	--初级召唤免费CD
 	self.lowfreeTimeCD = ConfigAdapter.Common.GetLowRecruitFreeTimeCD()
 	--高级召唤免费CD
 	self.highfreeTimeCD = ConfigAdapter.Common.GetHighRecruitFreeTimeCD()
 	--初级批量召唤的次数
 	self.lowCallTimes = ConfigAdapter.Common.GetLowRecruitMoreTimes()
 	--高级批量召唤的次数
 	self.highCallTimes = ConfigAdapter.Common.GetHighRecruitMoreTimes()

 	--TODO 绑定一个回调函数刷新剩余时间	
 	self.__open_callback = function ()
 		local data = CallModuleFunc(ModuleType.HERO,"GetHeroRecruitInfo")
 		self:__OpenCallback(data)
 	end
 	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_RECRUIT)
 	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_RECRUIT,true)
end 

--重写GetWidget
function Modules.HeroRecruitView:GetWidget(name, show_error)
	if name == "Image_blueCrystal" or name == "Image_purpleCrystal" then
		local root_widget = self.__is_show_vip_recruit and self.__panel_after or self.__panel_before
		local tgt_widget = WidgetUnity.GetWidgetByName(root_widget, name, show_error)
		if not tgt_widget then
			return WidgetUnity.GetWidgetByName(self.__root_widget, name, show_error)
		end
		return tgt_widget
	end
	return WidgetUnity.GetWidgetByName(self.__root_widget, name, show_error)
end

function Modules.HeroRecruitView:__OpenCallback(data)
	if not data then
		return
	end
	local is_open,is_prepare_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.VIP_RECRUIT)
	--是否显示Vip招募
	self.__is_show_vip_recruit = is_open or is_prepare_open

	--两种招募类型的容器
	self.__panel_before = self:GetWidget("Panel_Before")
	self.__panel_before:SetVisible(not self.__is_show_vip_recruit)
	--三种招募类型的容器
	self.__panel_after = self:GetWidget("Panel_After")
	self.__panel_after:SetVisible(self.__is_show_vip_recruit)
	--普通招募容器
	self.__panel_normalrecruit = self:GetWidget("Panel_normalRecruit")
	--vip招募容器
	self.__panel_viprecruit = self:GetWidget("Panel_VipRecruit")

	self.__data = data

	--图片适配
    self:GetWidget("Image_7_0"):IgnoreContentAdaptWithSize(true)
    self:GetWidget("Image_8_0"):IgnoreContentAdaptWithSize(true)
    self:GetWidget("Image_9_0"):IgnoreContentAdaptWithSize(true)
    self:GetWidget("Image_10_0"):IgnoreContentAdaptWithSize(true)   
    self:GetWidget("Image_RedTxt"):IgnoreContentAdaptWithSize(true)
    self:GetWidget("Image_RecruitTxt"):IgnoreContentAdaptWithSize(true)

    if CPPSdkTools.GetCPChannelType() == Macros.SDK.CPChannelType.NNS_BT then
    	self:GetWidget("Image_8_0"):SetVisible(false)
    end

	--召唤主界面
	--传送门特效
	self.PanelGateSpecialEffect = self:GetWidget("Panel_gateSpecialEffect")
	local effect_id, effect = self:__CreateEffect({res_name = "UI_zhaomumen",scale = 0.7}, self.PanelGateSpecialEffect,1)
	PosTool.Center(effect,0,0)
	--水波纹特效
	self.Panel_waterSpecialEffect = self:GetWidget("Panel_waterSpecialEffect")
	local effect_id, effect = self:__CreateEffect({res_name = "UI_zhaomushuichi"}, self.Panel_waterSpecialEffect,1)
	PosTool.Center(effect)
	local type
	type,self.LowRecruitItemInfo = ObjIDTool.GetInfo(ConfigAdapter.Common.GetLowRecruitConsumeGoodID())
	--初级召唤水晶
	local Image_blueCrystal = self:GetWidget("Image_blueCrystal")
	Image_blueCrystal:IgnoreContentAdaptWithSize(true)
	WidgetUnity.SetWidgetListener(Image_blueCrystal, nil, nil,function ()
		if not self.__is_onplaying_action then
			self.__is_onplaying_action = true
			self.__state = HeroRecruitViewState.RecruitState
			self.__hero_recruitstate = Macros.Game.HERO_RECRUIT.LOW_RECRUIT
			self:SetViewData()		
			-- 播放动画
			self.LowPanelSpecialEffect:SetVisible(false)
			self.HighPanelSpecialEffect:SetVisible(false)
			self.__vip_panel_specialeffect:SetVisible(false)
			self:PlayActionTimeline("openRecruitView", false,function ()
				--消耗道具
				self.__image_recruititembg1:SetVisible(self.__hero_recruitstate ~= Macros.Game.HERO_RECRUIT.VIP_RECRUIT)
				self.__image_recruititembg2:SetVisible(self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.LOW_RECRUIT)
				self.__image_recruititemcoin1:SetScaleFactor(0.5)
				self.__image_recruititemcoin2:SetScaleFactor(0.5)
				self.__is_onplaying_action = false
				if self.IsLowRecruitShowTime then
					self.Image_timeBg:SetVisible(true)
				end
				--新手指引特殊操作检测(英雄招募召唤1次)
				CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_RECRUIT_ONE_TIME, self.Button_recruitOneTime)
			end)
		end
	end)
	--初级召唤免费文字
	self.LowRecruitTextfree = WidgetUnity.GetWidgetByName(Image_blueCrystal,"Text_free")
	Util:WidgetLabel(self.LowRecruitTextfree,"", Macros.TypefaceSize.slightly, Macros.Color.btn)
	--初级召唤免费次数文字
	self.LowRecruitTextFreeTimes = WidgetUnity.GetWidgetByName(Image_blueCrystal,"Text_freeTimes")
	Util:WidgetLabel(self.LowRecruitTextFreeTimes,"", Macros.TypefaceSize.slightly, Macros.Color.btn)		
	--剩余时间
	self.LowRecruitNextFreeTime = WidgetUnity.GetWidgetByName(Image_blueCrystal,"Text_timeRetain")
	Util:WidgetLabel(self.LowRecruitNextFreeTime,"", Macros.TypefaceSize.slightly, Macros.Color.golden, 0, 0,Macros.Color.common_stroke)
	self.LowRecruitTextTimeAfter = WidgetUnity.GetWidgetByName(Image_blueCrystal,"Text_timeAfter")
	Util:WidgetLabel(self.LowRecruitTextTimeAfter,CPPGameLib.GetString("hero_recruitAfterFree"), Macros.TypefaceSize.slightly,Macros.Color.btn)	
	--初级召唤消耗容器
	self.Panel_lowCost = WidgetUnity.GetWidgetByName(Image_blueCrystal,"Panel_lowCost")
	Util:WidgetLabel(WidgetUnity.GetWidgetByName(self.Panel_lowCost,"Text_cost"),CPPGameLib.GetString("hero_recruitCostText"), Macros.TypefaceSize.slightly, Macros.Color.btn)
	self.__lowrecruittextcostnum = WidgetUnity.GetWidgetByName(self.Panel_lowCost,"Text_costNum")
	Util:WidgetLabel(self.__lowrecruittextcostnum,1, Macros.TypefaceSize.slightly, Macros.Color.btn)
	--初级召唤道具数量
	self.__text_lowhavenum = WidgetUnity.GetWidgetByName(self.Panel_lowCost,"Text_lowhaveNum")
	Util:WidgetLabel(self.__text_lowhavenum,"", Macros.TypefaceSize.slightly, Macros.Color.btn)
	--初级召唤控件Table
	self.LowRecruitTable = {[1] = self.LowRecruitTextfree,[2] = self.LowRecruitTextFreeTimes,[3] = self.LowRecruitNextFreeTime,[4] = self.LowRecruitTextTimeAfter,[5] = self.Panel_lowCost}
	--初级召唤消耗道具
	self.__low_image_coin = WidgetUnity.GetWidgetByName(self.Panel_lowCost,"Image_coin")
	self.__low_image_coin:IgnoreContentAdaptWithSize(true)
	self.__low_image_coin:SetScaleFactor(0.5)
	self.__low_image_coin:SetImage(Resource.PathTool.GetItemPath(self.LowRecruitItemInfo.res_id),TextureResType.LOCAL)
	--初级召唤特效
	self.LowPanelSpecialEffect = WidgetUnity.GetWidgetByName(Image_blueCrystal,"Panel_specialEffect")
	local effect_id, effect = self:__CreateEffect({res_name = "UI_zhaomuchuji"}, self.LowPanelSpecialEffect)
	PosTool.Center(effect)
	--高级召唤水晶
	local Image_purpleCrystal = self:GetWidget("Image_purpleCrystal")
	Image_purpleCrystal:IgnoreContentAdaptWithSize(true)
	WidgetUnity.SetWidgetListener(Image_purpleCrystal, nil, nil,function ()
		if not self.__is_onplaying_action then
			self.__is_onplaying_action = true
			self.__state = HeroRecruitViewState.RecruitState
			self.__hero_recruitstate = Macros.Game.HERO_RECRUIT.HIGH_RECRUIT
			self:SetViewData()
			-- 播放动画
			self.LowPanelSpecialEffect:SetVisible(false)
			self.HighPanelSpecialEffect:SetVisible(false)
			self.__vip_panel_specialeffect:SetVisible(false)
			self:PlayActionTimeline("openRecruitView", false, function ()
				--消耗道具
				self.__image_recruititembg1:SetVisible(self.__hero_recruitstate ~= Macros.Game.HERO_RECRUIT.VIP_RECRUIT)
				self.__image_recruititembg2:SetVisible(self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.LOW_RECRUIT)
				self.__image_recruititemcoin1:SetScaleFactor(0.5)
				self.__image_recruititemcoin2:SetScaleFactor(0.5)
				self.__is_onplaying_action = false
				if self.IsHighRecruitShowTime then
					self.Image_timeBg:SetVisible(true)
				end
			end)
		end
	end)
	--高级召唤免费文字
	self.HighRecruitTextfree = WidgetUnity.GetWidgetByName(Image_purpleCrystal,"Text_free")
	Util:WidgetLabel(self.HighRecruitTextfree,CPPGameLib.GetString("hero_recruitFreeText"), Macros.TypefaceSize.slightly, Macros.Color.btn)
	--剩余时间
	self.HighRecruitNextFreeTime = WidgetUnity.GetWidgetByName(Image_purpleCrystal,"Text_timeRetain")
	Util:WidgetLabel(self.HighRecruitNextFreeTime,"", Macros.TypefaceSize.slightly, Macros.Color.golden, 0, 0,Macros.Color.common_stroke)
	self.HighRecruitTextTimeAfter = WidgetUnity.GetWidgetByName(Image_purpleCrystal,"Text_timeAfter")
	Util:WidgetLabel(self.HighRecruitTextTimeAfter,CPPGameLib.GetString("hero_recruitAfterFree"), Macros.TypefaceSize.slightly, Macros.Color.btn)
	--高级召唤免费次数用完文字
	self.HighRecruitTextFreeTimeEnd = WidgetUnity.GetWidgetByName(Image_purpleCrystal,"Text_freeTimes")
	Util:WidgetLabel(self.HighRecruitTextFreeTimeEnd,CPPGameLib.GetString("hero_recruitForToday"), Macros.TypefaceSize.slightly, Macros.Color.btn)
	self.HighRecruitTextFreeTimeEnd:SetVisible(false)
	--高级召唤消耗容器
	self.Panel_highCost = WidgetUnity.GetWidgetByName(Image_purpleCrystal,"Panel_highCost")
	--高级召唤道具数量
	self.__text_highhavenum = WidgetUnity.GetWidgetByName(self.Panel_highCost,"Text_highhaveNum")
	Util:WidgetLabel(self.__text_highhavenum,"", Macros.TypefaceSize.slightly, Macros.Color.btn)
	--高级召唤消耗容器
	Util:WidgetLabel(WidgetUnity.GetWidgetByName(self.Panel_highCost,"Text_cost"),CPPGameLib.GetString("hero_recruitCostText"), Macros.TypefaceSize.slightly, Macros.Color.btn)
	self.HighRecruitCostCoin = WidgetUnity.GetWidgetByName(self.Panel_highCost,"Image_coin")
	self.HighRecruitCostCoin:IgnoreContentAdaptWithSize(true)	
	self.HighRecruitCostCoin:SetScaleFactor(0.5)
	self.HighRecruitTextCostNum = WidgetUnity.GetWidgetByName(self.Panel_highCost,"Text_costNum")
	Util:WidgetLabel(self.HighRecruitTextCostNum,1, Macros.TypefaceSize.slightly, Macros.Color.btn)
	--高级召唤控件Table
	self.HighRecruitTable = {[1] = self.HighRecruitTextfree,[2] = self.HighRecruitNextFreeTime,[3] = self.HighRecruitTextTimeAfter,[4] = self.Panel_highCost}
	--高级召唤特效
	self.HighPanelSpecialEffect = WidgetUnity.GetWidgetByName(Image_purpleCrystal,"Panel_specialEffect")
	local effect_id, effect = self:__CreateEffect({res_name = "UI_zhaomugaoji"}, self.HighPanelSpecialEffect)
	PosTool.Center(effect,-8,0)
	--招募预览
	self.Button_preLook = self:GetWidget("Button_preLook")
	self.Button_preLook:SetVisible(false)
	Util:WidgetButton(self:GetWidget("Button_preLook"),"",function ()
		if self.__hero_recruitstate ~= Macros.Game.HERO_RECRUIT.VIP_RECRUIT then
			LogicGUI.TemplPreLookView.New(self.__hero_recruitstate)
		else
			LogicGUI.TemplVipRecruitPrelook.New()
		end
	end)
	Util:WidgetLabel(self:GetWidget("Text_preLook"),CPPGameLib.GetString("hero_recruitPreLook"), Macros.TypefaceSize.normal, Macros.Color.btn)
	--招募卷
	--名字	
	for i=1,2 do
		self["__image_recruititembg"..i] = self:GetWidget("Image_recruitItemBg"..i)
		self["__image_recruititembg"..i]:SetVisible(false)
		self["__text_recruititemname"..i] = self:GetWidget("Text_recruitItemName"..i)
		Util:WidgetLabel(self["__text_recruititemname"..i],"", Macros.TypefaceSize.slightly, Macros.Color.btn)
		--货币
		self["__image_recruititemcoin"..i] = self:GetWidget("Image_recruitItemCoin"..i)
		self["__image_recruititemcoin"..i]:IgnoreContentAdaptWithSize(true)
		--货币数量
		self["__text_recruititemnum"..i] = self:GetWidget("Text_recruitItemNum"..i)
		Util:WidgetLabel(self["__text_recruititemnum"..i],"", Macros.TypefaceSize.slightly, Macros.Color.btn)
	end
		
	--免费倒计时
	self.Image_timeBg = self:GetWidget("Image_timeBg")
	self.Image_timeBg:SetVisible(false)
	self.NextFreeTime = WidgetUnity.GetWidgetByName(self.Image_timeBg,"Text_time")
	Util:WidgetLabel(self.NextFreeTime,"", Macros.TypefaceSize.slightly, Macros.Color.golden, 0, 0,Macros.Color.common_stroke)
	local txt_timefree = WidgetUnity.GetWidgetByName(self.Image_timeBg,"Text_timeFree")
	Util:WidgetLabel(txt_timefree,CPPGameLib.GetString("hero_recruitAfterFree"), Macros.TypefaceSize.slightly, Macros.Color.btn)

	--召唤一次水晶
	local Panel_lowRecruit = self:GetWidget("Panel_lowRecruit")
	--水晶
	self.__one_calltime_crystal = WidgetUnity.GetWidgetByName(Panel_lowRecruit,"Image_recruitCrystal")
	--货币
	self.OneCallImageRecruitCoin = WidgetUnity.GetWidgetByName(Panel_lowRecruit,"Image_recruitCoin")
	self.OneCallImageRecruitCoin:IgnoreContentAdaptWithSize(true)
	self.OneCallImageRecruitCoin:SetScaleFactor(0.4)
	--本次免费
	self.TextForFree = WidgetUnity.GetWidgetByName(Panel_lowRecruit,"Text_freeTimes")
	Util:WidgetLabel(self.TextForFree,"", Macros.TypefaceSize.minimum, Macros.Color.btn)
	--消耗货币数量
	self.OneCallTextRecruitCoinNum = WidgetUnity.GetWidgetByName(Panel_lowRecruit,"Text_recruitCoinNum")
	Util:WidgetLabel(self.OneCallTextRecruitCoinNum,"", Macros.TypefaceSize.slightly, Macros.Color.btn)
	--召唤一次按钮
	self.Button_recruitOneTime = Util:WidgetButton(WidgetUnity.GetWidgetByName(Panel_lowRecruit,"Button_recruitOneTime"),CPPGameLib.GetString("hero_recruitCallOneTime"),function ()
		local heroNum,heroMaxNum = CallModuleFunc(ModuleType.HERO,"GetHeroSumAndMax")
		if heroNum >= heroMaxNum then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_recruitFullHeroTip"))
			return
		end
		self:JudgeIsCouldRecruit()	
		--检查是否在指引
		if CallModuleFunc(ModuleType.LEAD,"IsInLead") then
			CallModuleFunc(ModuleType.LEAD,"HideArrowAndTips")
		end	
	end,
Macros.TypefaceSize.normal)

	--召唤十次水晶
	local Panel_highRecruit = self:GetWidget("Panel_highRecruit")
	--水晶
	self.__ten_calltime_crystal = WidgetUnity.GetWidgetByName(Panel_highRecruit,"Image_recruitCrystal")
	--货币
	self.TenCallImageRecruitCoin = WidgetUnity.GetWidgetByName(Panel_highRecruit,"Image_recruitCoin")	
	self.TenCallImageRecruitCoin:IgnoreContentAdaptWithSize(true)
	self.TenCallImageRecruitCoin:SetScaleFactor(0.4)
	--消耗货币数量
	self.TenCallTextRecruitCoinNum = WidgetUnity.GetWidgetByName(Panel_highRecruit,"Text_recruitCoinNum")
	Util:WidgetLabel(self.TenCallTextRecruitCoinNum,"", Macros.TypefaceSize.slightly, Macros.Color.btn)
	--召唤10次按钮
	Util:WidgetButton(WidgetUnity.GetWidgetByName(Panel_highRecruit,"Button_recruitTenTime"),CPPGameLib.GetString("hero_recruitCallTenTime"),function ()
		local heroNum,heroMaxNum = CallModuleFunc(ModuleType.HERO,"GetHeroSumAndMax")
		if heroNum > (heroMaxNum - 10) then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_morerecruitFullHeroTip"))
			return
		end
		if self.__hero_recruitstate ==  Macros.Game.HERO_RECRUIT.LOW_RECRUIT then
			local recruitGoodNums = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",ConfigAdapter.Common.GetLowRecruitConsumeGoodID())
			local friend_point = CallModuleFunc(ModuleType.ROLE,"GetTotalFriendShipPoint")
			local value_list = ConfigAdapter.Common.GetFriendshipPointConsume()
			if recruitGoodNums >= 10 or friend_point >= value_list[2] then
				local recruit_Type = recruitGoodNums >= 10 and Macros.Game.HERO_RECRUIT_DETAIL_TYPE.LOWMOREPROP_RECRUIT 
										or Macros.Game.HERO_RECRUIT_DETAIL_TYPE.FRIENDPOINT_MORERECRUIT
				self:CallModuleFunc("HeroRecruitReq",recruit_Type)
			else
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_recruitGoodNoEnough"))	
			end
		else
			local recruitGoodNums = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",ConfigAdapter.Common.GetHighRecruitConsumeGoodID())
			if recruitGoodNums >= 10 then
				--高级道具招募
				self:CallModuleFunc("HeroRecruitReq",Macros.Game.HERO_RECRUIT_DETAIL_TYPE.HIGHMOREPROP_RECRUIT)
			else
				local now_diamond = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
				if now_diamond >= ConfigAdapter.Common.GetHighMoreRecruitConsumeGoods() then
					self:CallModuleFunc("HeroRecruitReq",Macros.Game.HERO_RECRUIT_DETAIL_TYPE.HIGHMOREMONEY_RECRUIT)
				else
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_recruitDiamondNoEnough"))
				end
			end
		end
	end,
Macros.TypefaceSize.normal)
	
	--高级召唤提示
	if not GlobalCustomizedLogicAdapterMgr:Call("IsHideRecruitEncourageTips") then
		self.recruitEncourageText = Util:RichText("",Macros.TypefaceSize.normal * 1.4,0,0,nil,Macros.Color.btn_hex)
		self:GetWidget("Panel_heroRecurit"):AddChild(self.recruitEncourageText)
		self.recruitEncourageText:SetAnchorPoint(0.5,0)
		PosTool.CenterBottom(self.recruitEncourageText,0,15)
		self.recruitEncourageText:SetVisible(false)
	end

	--VIP招募
	--VIP召唤水晶
	local img_redcrystal = self:GetWidget("Image_redCrystal")
	img_redcrystal:IgnoreContentAdaptWithSize(true)
	img_redcrystal:SetGray(not is_open) 
	WidgetUnity.SetWidgetListener(img_redcrystal, nil, nil,function ()
		--判断开启
		if GlobalModulesMgr:IsOpen(Macros.Game.SystemID.VIP_RECRUIT,true) then
			if not self.__is_onplaying_action then
				self.__is_onplaying_action = true
				self.__state = HeroRecruitViewState.RecruitState
				self.__hero_recruitstate = Macros.Game.HERO_RECRUIT.VIP_RECRUIT
				self:SetViewData()
				-- 播放动画
				self.LowPanelSpecialEffect:SetVisible(false)
				self.HighPanelSpecialEffect:SetVisible(false)
				self.__vip_panel_specialeffect:SetVisible(false)
				self:PlayActionTimeline("openRecruitView", false, function ()
					--消耗道具
					self.__image_recruititembg1:SetVisible(self.__hero_recruitstate ~= Macros.Game.HERO_RECRUIT.VIP_RECRUIT)
					self.__image_recruititembg2:SetVisible(self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.LOW_RECRUIT)
					self.__is_onplaying_action = false
				end)
			end
		end
	end)
	--丝带
	self:GetWidget("Image_RecruitTxt"):SetGray(not is_open)
	--消耗容器
	local panel_vipcost = self:GetWidget("Panel_vipCost")
	panel_vipcost:SetVisible(is_open)
	--消耗
	Util:WidgetLabel(WidgetUnity.GetWidgetByName(panel_vipcost,"Text_cost"),CPPGameLib.GetString("hero_recruitCostText"), Macros.TypefaceSize.slightly, Macros.Color.btn)
	--钻石
	local vip_icon = WidgetUnity.GetWidgetByName(panel_vipcost,"Image_coin")
	vip_icon:IgnoreContentAdaptWithSize(true)	
	vip_icon:SetScaleFactor(0.7)
	--钻石数量
	local costnum = WidgetUnity.GetWidgetByName(panel_vipcost,"Text_costNum")
	Util:WidgetLabel(costnum,ConfigAdapter.Common.GetVipRecruitCostDiamonds() or 0, Macros.TypefaceSize.slightly, Macros.Color.btn)
	--VIP召唤道具数量
	self.__txt_tophavenum = WidgetUnity.GetWidgetByName(panel_vipcost,"Text_highhaveNum")
	Util:WidgetLabel(self.__txt_tophavenum,"", Macros.TypefaceSize.slightly, Macros.Color.btn)
	--开放等级
	local config_system = CPPGameLib.GetConfig("SystemEntry", Macros.Game.SystemID.VIP_RECRUIT, false)
	local txt_openlevel = self:GetWidget("Text_OpenLevel")
	txt_openlevel:SetVisible(is_prepare_open)
	Util:WidgetLabel(txt_openlevel,CPPGameLib.GetString("common_open_lev",config_system and config_system.level or ""), Macros.TypefaceSize.normal, Macros.Color.golden)
	--VIP招募特效
	self.__vip_panel_specialeffect = WidgetUnity.GetWidgetByName(img_redcrystal,"Panel_specialEffect")
	local effect_id, effect = self:__CreateEffect({res_name = "UI_zhaomuchuji"}, self.__vip_panel_specialeffect)
	PosTool.Center(effect)
	effect:SetVisible(is_open)

	self:HandlerChangeData()
end

function Modules.HeroRecruitView:__RegistAllEvents()
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_RECRUIT_UPDATE, function(_, callType, data) 
 		if callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.LOWFREE_RECRUIT or callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.LOWPROP_RECRUIT or callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.FRIENDPOINT_RECRUIT then
 			self:RecruitTips(data[1].info_id)
 			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.HERO_SINGLE_RECRUIT_VIEW, false, data[1].info_id,Macros.Game.HERO_RECRUIT.LOW_RECRUIT)
 		elseif callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.LOWMOREPROP_RECRUIT or callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.FRIENDPOINT_MORERECRUIT then	
 			LogicGUI.TemplRecruitTenCallView.New(data)			
 		elseif callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.HIGHFREE_RECRUIT or callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.HIGHPROP_RECRUIT or callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.HIGHMONEY_RECRUIT then
 			self:RecruitTips(data[1].info_id)
 			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.HERO_SINGLE_RECRUIT_VIEW, false, data[1].info_id,Macros.Game.HERO_RECRUIT.HIGH_RECRUIT,nil,self.__data.advanced_summon_times)
 		elseif callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.HIGHMOREMONEY_RECRUIT or callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.HIGHMOREPROP_RECRUIT then 			
 			LogicGUI.TemplRecruitTenCallView.New(data)
 		elseif callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.UPDATE_RECRUIT_TIME then 
 			self.__data = data
 			self:HandlerChangeData()
 			self:SetViewData()	
 		elseif callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.RECRUIT_ONE_TIME then 
 			if data then
 				local heroNum,heroMaxNum = CallModuleFunc(ModuleType.HERO,"GetHeroSumAndMax")
				if heroNum >= heroMaxNum then
					self:RunRecruitAnimation()
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_recruitFullHeroTip"))
					return
				end
				self:JudgeIsCouldRecruit(true)				
 			else
 				self:RunRecruitAnimation()
 			end 
 		elseif callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.VIP_RECRUIT then 			
 			LogicGUI.TemplRecruitTenCallView.New(data,true)			
 		else
 		end
 	end)
end

--播放显示动画
function Modules.HeroRecruitView:RunRecruitAnimation()
	self:PlayActionTimeline("showRecruitCrystal",false,function ()
		self.Button_preLook:SetVisible(true)
		self.__image_recruititembg1:SetVisible(self.__hero_recruitstate ~= Macros.Game.HERO_RECRUIT.VIP_RECRUIT)
		self.__image_recruititembg2:SetVisible(self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.LOW_RECRUIT)
		self.__on_recruiting = false
		if self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.LOW_RECRUIT then
			if self.IsLowRecruitShowTime then
				self.Image_timeBg:SetVisible(true)
			end
		else
			if self.IsHighRecruitShowTime then
				self.Image_timeBg:SetVisible(true)
			end
		end
	end)
end

function Modules.HeroRecruitView:RecruitTips(info_id)
	if not info_id then
		return
	end
	local hero_config = CPPGameLib.GetConfig("Hero",info_id)
	if hero_config.init_quality >= Macros.Game.QualityType.ORANGE then
		self:FireNextFrame(Macros.Event.HeroModule.HERO_RECRUIT_NOTICE,{hero_config.name,hero_config.init_quality})			
	end
end

--判断是否具有招募资格
function Modules.HeroRecruitView:JudgeIsCouldRecruit(IsContinute)
	local recruit_Type = 0
	if self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.LOW_RECRUIT then		
		if self.IsLowRecruitHaveFreeTime then
			self:CallModuleFunc("SetHeroLowRecruitFreeState",false)		
			recruit_Type = Macros.Game.HERO_RECRUIT_DETAIL_TYPE.LOWFREE_RECRUIT																			
		else
			local recruitGoodNums = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",ConfigAdapter.Common.GetLowRecruitConsumeGoodID())
			local friend_point = CallModuleFunc(ModuleType.ROLE,"GetTotalFriendShipPoint")
			local value_list = ConfigAdapter.Common.GetFriendshipPointConsume()
			if recruitGoodNums >= 1 or friend_point >= value_list[1] then
				recruit_Type = recruitGoodNums >= 1 and Macros.Game.HERO_RECRUIT_DETAIL_TYPE.LOWPROP_RECRUIT		
								or 	Macros.Game.HERO_RECRUIT_DETAIL_TYPE.FRIENDPOINT_RECRUIT					
			else
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_recruitGoodNoEnough"))
				if IsContinute then
					self:RunRecruitAnimation()
				end
				return
			end
		end
	else
		if self.IsHighRecruitHaveFreeTime then
			self:CallModuleFunc("SetHeroHighRecruitFreeState",false)
			recruit_Type = Macros.Game.HERO_RECRUIT_DETAIL_TYPE.HIGHFREE_RECRUIT
		else
			local recruitGoodNums = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",ConfigAdapter.Common.GetHighRecruitConsumeGoodID())
			if recruitGoodNums >= 1 then
				recruit_Type = Macros.Game.HERO_RECRUIT_DETAIL_TYPE.HIGHPROP_RECRUIT
			else
				local now_diamond = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
				if now_diamond >= ConfigAdapter.Common.GetHighRecruitConsumeGoods() then
					recruit_Type = Macros.Game.HERO_RECRUIT_DETAIL_TYPE.HIGHMONEY_RECRUIT
				else
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_recruitDiamondNoEnough"))
					if IsContinute then
						self:RunRecruitAnimation()
					end
					return
				end
			end
		end
	end
	self.Button_preLook:SetVisible(false)
	self.__image_recruititembg1:SetVisible(false)
	self.__image_recruititembg2:SetVisible(false)
	self.__on_recruiting = true
	if IsContinute then
		self:RecruitOneTime(recruit_Type)
		-- self:PlayActionTimeline("showRecruitCrystal", false, function ()
		-- 	self:PlayActionTimeline("hideRecruitCrystal", false, function ()				
				
		-- 	end)
		-- end)
	else
		self:PlayActionTimeline("hideRecruitCrystal", false, function ()
			if self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.LOW_RECRUIT then
				if self.IsLowRecruitShowTime then
					self.Image_timeBg:SetVisible(false)
				end
			else
				if self.IsHighRecruitShowTime then
					self.Image_timeBg:SetVisible(false)
				end
			end
			self:RecruitOneTime(recruit_Type)
		end)
	end
end

--招募一次操作
function Modules.HeroRecruitView:RecruitOneTime(recruit_Type)	
	if self.SpecialEffectId then
		self:__DelEffect(self.SpecialEffectId)
		self.SpecialEffectId = nil
		self:__DelEffect(self.SpecialBoomEffectId)
		self.SpecialBoomEffectId = nil
	end	
	local effect = nil
	self.SpecialEffectId, effect = self:__CreateEffect({id = 100021,loop = false,}, self.PanelGateSpecialEffect,2)
	PosTool.Center(effect,0,0)
	CPPActionManager.DelayTo(self.Button_preLook,1,GlobalCallbackMgr:AddCallBackFunc(function()
		local effect_id,effect = self:__CreateEffect({res_name = "UI_qianghuabeiguang",loop = false}, self.PanelGateSpecialEffect,2) 
		PosTool.Center(effect,0,0)			
	end))
	CPPActionManager.DelayTo(self.Button_preLook,1.5,GlobalCallbackMgr:AddCallBackFunc(function()
			self.SpecialBoomEffectId,effect = self:__CreateEffect({id = 100022,loop = false}, self.PanelGateSpecialEffect,3) 
			PosTool.Center(effect,0,0)				
			CPPActionManager.DelayTo(self.Button_preLook,0.4,GlobalCallbackMgr:AddCallBackFunc(function()
				self:CallModuleFunc("HeroRecruitReq",recruit_Type)			
			end))			
	end))
end


--处理变化的数据
function Modules.HeroRecruitView:HandlerChangeData()
	self.lowFreeRecruitTime = self.lowFreeTimes - self.__data.normal_summon_times
	local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	--初级召唤
	if self.__data.normal_summon_times >= self.lowFreeTimes then
		self:SetLayoutVisible(Macros.Game.HERO_RECRUIT.LOW_RECRUIT,{[1] = 1,[5] = 5})
		self.LowRecruitTextfree:SetText(CPPGameLib.GetString("hero_recruitForToday"))
		self.IsLowRecruitHaveFreeTime = false
		self:CallModuleFunc("SetHeroLowRecruitFreeState",self.IsLowRecruitHaveFreeTime)
		self.IsLowRecruitShowTime = false
	elseif self.__data.normal_summon_times == 0 then
		self:SetLayoutVisible(Macros.Game.HERO_RECRUIT.LOW_RECRUIT,{[1] = 1,[2] = 2})
		self.LowRecruitTextfree:SetText(CPPGameLib.GetString("hero_recruitFreeText"))
		self.LowRecruitTextFreeTimes:SetText(CPPGameLib.GetString("hero_recruitForFree",self.lowFreeTimes - self.__data.normal_summon_times,self.lowFreeTimes))
		self.IsLowRecruitHaveFreeTime = true
		self:CallModuleFunc("SetHeroLowRecruitFreeState",self.IsLowRecruitHaveFreeTime)
		self.IsLowRecruitShowTime = false
	else
		if severTime - self.__data.normal_summon_time  >= self.lowfreeTimeCD  then
			if self.__hero_lowrecruit_timer then
				GlobalTimerQuest:CancelQuest(self.__hero_lowrecruit_timer)
				self.__hero_lowrecruit_timer = nil
			end		
			self:SetLayoutVisible(Macros.Game.HERO_RECRUIT.LOW_RECRUIT,{[1] = 1,[2] = 2})
			self.LowRecruitTextfree:SetText(CPPGameLib.GetString("hero_recruitFreeText"))			
			self.LowRecruitTextFreeTimes:SetText(CPPGameLib.GetString("hero_recruitForFree",self.lowFreeTimes - self.__data.normal_summon_times,self.lowFreeTimes))
			self.IsLowRecruitHaveFreeTime = true
			self:CallModuleFunc("SetHeroLowRecruitFreeState",self.IsLowRecruitHaveFreeTime)
			self.IsLowRecruitShowTime = false
		else		
			self:SetLayoutVisible(Macros.Game.HERO_RECRUIT.LOW_RECRUIT,{[3] = 3,[4] = 4,[5] = 5})
			local retainTime = self.__data.normal_summon_time + self.lowfreeTimeCD - severTime
			self.LowRecruitNextFreeTime:SetText(TimeTool.TransTimeStamp("HH:MM:SS", retainTime))
			--倒计时
			if not self.__hero_lowrecruit_timer then
				self.__hero_lowrecruit_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
				function()
					self:HeartBeatrecruitTimer()
				end), 1, -1)
			end	
			self.IsLowRecruitHaveFreeTime = false
			self:CallModuleFunc("SetHeroLowRecruitFreeState",self.IsLowRecruitHaveFreeTime)
			self.IsLowRecruitShowTime = true
			self:HeartBeatrecruitTimer()			
		end
	end
	--召唤道具显示
	local recruitGoodNums = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",ConfigAdapter.Common.GetHighRecruitConsumeGoodID())
	local type
	type,self.HighRecruitItemInfo = ObjIDTool.GetInfo(ConfigAdapter.Common.GetHighRecruitConsumeGoodID())
	--召唤道具数量更新
	local lowrecruitGoodNums = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",ConfigAdapter.Common.GetLowRecruitConsumeGoodID())
	local friend_point = CallModuleFunc(ModuleType.ROLE,"GetTotalFriendShipPoint")	
	local value_list = ConfigAdapter.Common.GetFriendshipPointConsume()
	self.__text_lowhavenum:SetText(CPPGameLib.GetString("hero_recruit_haveItemNum",lowrecruitGoodNums > 0 and lowrecruitGoodNums or friend_point))	
	self.__lowrecruittextcostnum:SetText(lowrecruitGoodNums > 0 and 1 or value_list[1])	
	if lowrecruitGoodNums > 0 then
		self.__low_image_coin:SetImage(Resource.PathTool.GetItemPath(self.LowRecruitItemInfo.res_id),TextureResType.LOCAL)
	else
		local huobi_config = CPPGameLib.GetConfig("HuoBi",Macros.Game.RolePropType.TOTAL_FRIENDSHIP_POINT)
		self.__low_image_coin:SetImage(Resource.PathTool.GetItemPath(huobi_config.big_icon),TextureResType.LOCAL)
	end
	--高级召唤		
	if severTime - self.__data.advanced_summon_time  >= self.highfreeTimeCD  then
		if self.__hero_highrecruit_timer then
			GlobalTimerQuest:CancelQuest(self.__hero_highrecruit_timer)
			self.__hero_highrecruit_timer = nil
		end		
		self:SetLayoutVisible(Macros.Game.HERO_RECRUIT.HIGH_RECRUIT,{[1] = 1})
		self.IsHighRecruitHaveFreeTime = true
		self:CallModuleFunc("SetHeroHighRecruitFreeState",self.IsHighRecruitHaveFreeTime)
		self.IsHighRecruitShowTime = false
	else
		self.IsHighRecruitHaveFreeTime = false
		self:CallModuleFunc("SetHeroHighRecruitFreeState",self.IsHighRecruitHaveFreeTime)
		self.IsHighRecruitShowTime = true
		self:SetLayoutVisible(Macros.Game.HERO_RECRUIT.HIGH_RECRUIT,{[2] = 2,[3] = 3,[4] = 4})
		self.HighRecruitTable = {[1] = self.HighRecruitTextfree,[2] = self.HighRecruitNextFreeTime,[3] = self.HighRecruitTextTimeAfter,[4] = self.Panel_highCost}
		local retainTime = self.__data.advanced_summon_time + self.highfreeTimeCD - severTime
		self.HighRecruitNextFreeTime:SetText(TimeTool.TransTimeStamp("HH:MM:SS", retainTime))
		--判断消耗道具
		if recruitGoodNums >= 1 then
			self.HighRecruitCostCoin:SetImage(Resource.PathTool.GetItemPath(self.HighRecruitItemInfo.res_id),TextureResType.LOCAL)
			self.HighRecruitTextCostNum:SetText(1)
			self.__text_highhavenum:SetText(CPPGameLib.GetString("hero_recruit_haveItemNum",recruitGoodNums))	
		else
			local huobi_config = CPPGameLib.GetConfig("HuoBi",Macros.Game.RolePropType.DIAMOND)
			self.HighRecruitCostCoin:SetImage(Resource.PathTool.GetItemPath(huobi_config.big_icon),TextureResType.LOCAL)
			self.HighRecruitTextCostNum:SetText(ConfigAdapter.Common.GetHighRecruitConsumeGoods())
			local now_diamond = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
			self.__text_highhavenum:SetText(CPPGameLib.GetString("hero_recruit_haveItemNum",now_diamond))	
		end
		--倒计时
		if not self.__hero_highrecruit_timer then
			self.__hero_highrecruit_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
			function()
				self:HeartBeatHighrecruitTimer()
			end), 1, -1)
		end	
		self:HeartBeatHighrecruitTimer()
	end
	self.__text_lowhavenum:SetVisible(not self.IsLowRecruitHaveFreeTime)
	self.__text_highhavenum:SetVisible(not self.IsHighRecruitHaveFreeTime)
	--更新钻石数量
	self.__txt_tophavenum:SetText(CPPGameLib.GetString("hero_recruit_haveItemNum",CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)))
	self:__UpdateRedDotNum()
end

--设置控件显隐
function Modules.HeroRecruitView:SetLayoutVisible(layoutType,visibleTable)
	if layoutType == 1 then
		for key,layout in ipairs(self.LowRecruitTable) do
			if visibleTable[key] then
				layout:SetVisible(true)
			else
				layout:SetVisible(false)
			end
		end
	else
		for key,layout in ipairs(self.HighRecruitTable) do
			if visibleTable[key] then
				layout:SetVisible(true)
			else
				layout:SetVisible(false)
			end
		end
	end
end

function Modules.HeroRecruitView:__Dispose()
	if self.__hero_lowrecruit_timer then
		GlobalTimerQuest:CancelQuest(self.__hero_lowrecruit_timer)
		self.__hero_lowrecruit_timer = nil
	end
	if self.__hero_highrecruit_timer then
		GlobalTimerQuest:CancelQuest(self.__hero_highrecruit_timer)
		self.__hero_highrecruit_timer = nil
	end	
	if self.lowHeroModel then
		self.lowHeroModel:DeleteMe()
		self.lowHeroModel = nil
	end
	if self.highHeroModel then
		self.highHeroModel:DeleteMe()
		self.highHeroModel = nil
	end	
end

function Modules.HeroRecruitView:HeartBeatrecruitTimer()
	local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local retainTime = self.__data.normal_summon_time + self.lowfreeTimeCD - severTime
	if retainTime > 0 then
		self.LowRecruitNextFreeTime:SetText(TimeTool.TransTimeStamp("HH:MM:SS", retainTime))
		if self.__hero_recruitstate then
			if self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.LOW_RECRUIT then

				self.NextFreeTime:SetText(TimeTool.TransTimeStamp("HH:MM:SS", retainTime))
			end
		end
	else
		self.IsLowRecruitHaveFreeTime = true
		self:CallModuleFunc("SetHeroLowRecruitFreeState",self.IsLowRecruitHaveFreeTime)
		self.IsLowRecruitShowTime = false
		self:FireNextFrame(Macros.Event.HeroModule.HERO_RECRUIT_FREE_UPDATE)
		--更新红点
		self:__UpdateRedDotNum()
		if self.__hero_recruitstate then
			if self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.LOW_RECRUIT then
				self.Image_timeBg:SetVisible(false)
				self.TextForFree:SetVisible(true)
				self.OneCallImageRecruitCoin:SetVisible(false)
				self.OneCallTextRecruitCoinNum:SetVisible(false)
				self.TextForFree:SetText(CPPGameLib.GetString("hero_recruitForFree",self.lowFreeTimes - self.__data.normal_summon_times,self.lowFreeTimes))
			end
		end		
		self:SetLayoutVisible(Macros.Game.HERO_RECRUIT.LOW_RECRUIT,{[1] = 1,[2] = 2})
		self.LowRecruitTextfree:SetText(CPPGameLib.GetString("hero_recruitFreeText"))
		self.LowRecruitTextFreeTimes:SetText(CPPGameLib.GetString("hero_recruitForFree",self.lowFreeTimes - self.__data.normal_summon_times,self.lowFreeTimes))
		GlobalTimerQuest:CancelQuest(self.__hero_lowrecruit_timer)
		self.__hero_lowrecruit_timer = nil
	end
end 

function Modules.HeroRecruitView:HeartBeatHighrecruitTimer()
	local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local retainTime = self.__data.advanced_summon_time + self.highfreeTimeCD - severTime			
	if retainTime > 0 then
		self.HighRecruitNextFreeTime:SetText(TimeTool.TransTimeStamp("HH:MM:SS", retainTime))
		if self.__hero_recruitstate then
			if self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.HIGH_RECRUIT then
				self.NextFreeTime:SetText(TimeTool.TransTimeStamp("HH:MM:SS", retainTime))
			end
		end
	else
		if self.__hero_recruitstate then
			if self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.HIGH_RECRUIT then
				self.Image_timeBg:SetVisible(false)
				self.TextForFree:SetVisible(true)
				self.OneCallImageRecruitCoin:SetVisible(false)
				self.OneCallTextRecruitCoinNum:SetVisible(false)
				self.TextForFree:SetText(CPPGameLib.GetString("hero_recruitForFreeOnce"))
			end
		end	
		self.IsHighRecruitHaveFreeTime = true
		self:CallModuleFunc("SetHeroHighRecruitFreeState",self.IsHighRecruitHaveFreeTime)
		self:FireNextFrame(Macros.Event.HeroModule.HERO_RECRUIT_FREE_UPDATE)
		self.IsHighRecruitShowTime = false
		self:SetLayoutVisible(Macros.Game.HERO_RECRUIT.HIGH_RECRUIT,{[1] = 1})
		GlobalTimerQuest:CancelQuest(self.__hero_highrecruit_timer)
		self.__hero_highrecruit_timer = nil
		--更新红点
		self:__UpdateRedDotNum()
	end
end

--打开对应的召唤界面前先处理数据
function Modules.HeroRecruitView:SetViewData()
	--普通招募容器
	self.__panel_normalrecruit:SetVisible(self.__hero_recruitstate ~= Macros.Game.HERO_RECRUIT.VIP_RECRUIT)
	--vip招募容器
	self.__panel_viprecruit:SetVisible(self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.VIP_RECRUIT)
	--高级召唤显示
	if self.recruitEncourageText then
		self.recruitEncourageText:SetVisible(self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.HIGH_RECRUIT)
	end
	
	if self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.VIP_RECRUIT then
		if not self.__init_vip_recruit then
			self.__init_vip_recruit = true
			local huobi_config = CPPGameLib.GetConfig("HuoBi",Macros.Game.RolePropType.DIAMOND)
			for i=0,2 do
				local panel_camp = self:GetWidget("Panel_camp"..i)
				--阵营图标
				local img_camp = WidgetUnity.GetWidgetByName(panel_camp, "Image_recruitCrystal")
				local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "camp_icon_big_"..i, true)
				img_camp:SetImage(sp)
				--阵营特效
				local effect_id,effect = self:__CreateEffect({id = 100104 + i},img_camp,-1)
				PosTool.Center(effect)
				--钻石
				local img_coin = WidgetUnity.GetWidgetByName(panel_camp, "Image_recruitCoin")
				img_coin:SetImage(Resource.PathTool.GetItemPath(huobi_config.big_icon),TextureResType.LOCAL)
				--消耗钻石数量
				local txt_recruitcoinnum = self:GetWidget("Text_recruitCoinNum")	
				Util:WidgetLabel(txt_recruitcoinnum, ConfigAdapter.Common.GetVipRecruitCostDiamonds() or 0, Macros.TypefaceSize.slightly, Macros.Color.btn)
				--招募按钮
				local btn_recruitcamp = self:GetWidget("Button_recruitCamp"..i)	
				Util:WidgetButton(btn_recruitcamp, CPPGameLib.GetString("hero_vip_camp_recruit_"..i), function ()
					--检查VIP等级
					local vip_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.VIP_LEVEL)
					if ConfigAdapter.Common.GetVipRecruitNeedVipLevel() > vip_level then
						GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_vip_camp_recruit_viplevel",ConfigAdapter.Common.GetVipRecruitNeedVipLevel()))
						return
					end
					--检查背包
					local heroNum,heroMaxNum = CallModuleFunc(ModuleType.HERO,"GetHeroSumAndMax")
					if heroNum > (heroMaxNum - 10) then
						GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_morerecruitFullHeroTip"))
						return
					end
					--检查钻石
					local diamond_nums = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
					if diamond_nums < ConfigAdapter.Common.GetVipRecruitCostDiamonds() then						
						GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_recruitDiamondNoEnough"))
						return
					end 
					--VIP招募
					self:CallModuleFunc("HeroVipSummonReq",i)
				end, Macros.TypefaceSize.normal, Macros.Color.btn)			
			end
		end
	else
		local is_low = self.__hero_recruitstate == Macros.Game.HERO_RECRUIT.LOW_RECRUIT
		--水晶
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_RECRUIT, is_low and "blue_message" or "purple_message")
		self.__one_calltime_crystal:SetImage(sp)
		self.__ten_calltime_crystal:SetImage(sp)

		--计算免费倒计时
		local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		local coutdown_time_for_free
		if is_low then
			coutdown_time_for_free = self.__data.normal_summon_time + self.lowfreeTimeCD - severTime
		else
			coutdown_time_for_free = self.__data.advanced_summon_time + self.highfreeTimeCD - severTime			
		end

		--本次免费
		self.TextForFree:SetText(is_low and CPPGameLib.GetString("hero_recruitForFree",self.lowFreeTimes - self.__data.normal_summon_times,self.lowFreeTimes) or CPPGameLib.GetString("hero_recruitForFreeOnce"))				
		--是否免费
		self.TextForFree:SetVisible((is_low and self.IsLowRecruitHaveFreeTime or self.IsHighRecruitHaveFreeTime) and coutdown_time_for_free <= 0)
		--货币
		local huobi_config = CPPGameLib.GetConfig("HuoBi",is_low and Macros.Game.RolePropType.TOTAL_FRIENDSHIP_POINT or Macros.Game.RolePropType.DIAMOND)
		if is_low then			
			--货币
			local lowrecruitGoodNums = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",ConfigAdapter.Common.GetLowRecruitConsumeGoodID())
			local friend_point = CallModuleFunc(ModuleType.ROLE,"GetTotalFriendShipPoint")
			--招募预览
			self.__text_recruititemname1:SetText(self.LowRecruitItemInfo.name)		
			self.__text_recruititemnum1:SetText(lowrecruitGoodNums)
			self.__image_recruititemcoin1:SetImage(Resource.PathTool.GetItemPath(self.LowRecruitItemInfo.res_id),TextureResType.LOCAL)
			self.__text_recruititemname2:SetText(CPPGameLib.GetString("hero_recruit_friendpoint"))		
			self.__text_recruititemnum2:SetText(friend_point)			
			self.__image_recruititemcoin2:SetImage(Resource.PathTool.GetItemPath(huobi_config.big_icon),TextureResType.LOCAL)
			--消耗货币数量
			local value_list = ConfigAdapter.Common.GetFriendshipPointConsume()
			self.TenCallTextRecruitCoinNum:SetText((lowrecruitGoodNums >= 10 or friend_point < value_list[2]) and 10 or value_list[2])
			--消耗货币数量
			self.OneCallTextRecruitCoinNum:SetText((lowrecruitGoodNums > 0 or friend_point < value_list[1]) and 1 or value_list[1])		
			if lowrecruitGoodNums > 0 or friend_point < value_list[1] then				
				self.OneCallImageRecruitCoin:SetImage(Resource.PathTool.GetItemPath(self.LowRecruitItemInfo.res_id),TextureResType.LOCAL)			
			else						
				self.OneCallImageRecruitCoin:SetImage(Resource.PathTool.GetItemPath(huobi_config.big_icon),TextureResType.LOCAL)			
			end	
			if lowrecruitGoodNums >= 10 or friend_point < value_list[2] then				
				self.TenCallImageRecruitCoin:SetImage(Resource.PathTool.GetItemPath(self.LowRecruitItemInfo.res_id),TextureResType.LOCAL)			
			else						
				self.TenCallImageRecruitCoin:SetImage(Resource.PathTool.GetItemPath(huobi_config.big_icon),TextureResType.LOCAL)			
			end		
			self.OneCallImageRecruitCoin:SetVisible(not self.IsLowRecruitHaveFreeTime)
			self.OneCallTextRecruitCoinNum:SetVisible(not self.IsLowRecruitHaveFreeTime)						
		else			
			if self.recruitEncourageText then
				if ConfigAdapter.Common.GetHighRecruitMustShowHero() - self.__data.advanced_summon_times ~= 1 then
					self.recruitEncourageText:SetText(CPPGameLib.GetString("hero_recruitEncourage", ConfigAdapter.Common.GetHighRecruitMustShowHero() - self.__data.advanced_summon_times))
				else
					self.recruitEncourageText:SetText(CPPGameLib.GetString("hero_recruitEncourage_1"))
				end
			end
			--货币
			self.TenCallImageRecruitCoin:SetImage(Resource.PathTool.GetItemPath(huobi_config.big_icon),TextureResType.LOCAL)			
			
			--消耗货币数量
			self.TenCallTextRecruitCoinNum:SetText(ConfigAdapter.Common.GetHighMoreRecruitConsumeGoods())
			--消耗货币数量
			self.OneCallTextRecruitCoinNum:SetText(1)
			--招募预览
			local recruitGoodNums = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",ConfigAdapter.Common.GetHighRecruitConsumeGoodID())
			self.__text_recruititemname1:SetText(self.HighRecruitItemInfo.name)
			self.__image_recruititemcoin1:SetImage(Resource.PathTool.GetItemPath(self.HighRecruitItemInfo.res_id),TextureResType.LOCAL)
			self.__text_recruititemnum1:SetText(recruitGoodNums)
			--判断消耗道具		
			if recruitGoodNums >= 1 then
				self.OneCallImageRecruitCoin:SetImage(Resource.PathTool.GetItemPath(self.HighRecruitItemInfo.res_id),TextureResType.LOCAL)
				self.OneCallTextRecruitCoinNum:SetText(1)
				--高级招募消耗道具
				if recruitGoodNums >= 10 then
					--消耗货币数量
					self.TenCallTextRecruitCoinNum:SetText(10)
					--货币
					self.TenCallImageRecruitCoin:SetImage(Resource.PathTool.GetItemPath(self.HighRecruitItemInfo.res_id),TextureResType.LOCAL)
				end
			else			
				self.OneCallImageRecruitCoin:SetImage(Resource.PathTool.GetItemPath(huobi_config.big_icon),TextureResType.LOCAL)
				self.OneCallTextRecruitCoinNum:SetText(ConfigAdapter.Common.GetHighRecruitConsumeGoods())
			end		
			self.OneCallImageRecruitCoin:SetVisible(not self.IsHighRecruitHaveFreeTime)
			self.OneCallTextRecruitCoinNum:SetVisible(not self.IsHighRecruitHaveFreeTime)	
		end
	end
end

function Modules.HeroRecruitView:__UpdateRedDotNum()
	local red_dot_num = 0
	if self.IsLowRecruitHaveFreeTime then
		red_dot_num = red_dot_num + 1
		self:__AddRedDot(Macros.Game.HERO_RECRUIT_DETAIL_TYPE.LOWFREE_RECRUIT,self:GetWidget("Image_blueCrystal"))
	else
		self:__HideRedDot(Macros.Game.HERO_RECRUIT_DETAIL_TYPE.LOWFREE_RECRUIT)
	end
	if self.IsHighRecruitHaveFreeTime then
		red_dot_num = red_dot_num + 1
		self:__AddRedDot(Macros.Game.HERO_RECRUIT_DETAIL_TYPE.HIGHFREE_RECRUIT,self:GetWidget("Image_purpleCrystal"))
	else
		self:__HideRedDot(Macros.Game.HERO_RECRUIT_DETAIL_TYPE.HIGHFREE_RECRUIT)
	end
	local recruitGoodNums = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",ConfigAdapter.Common.GetLowRecruitConsumeGoodID())
	local friend_point = CallModuleFunc(ModuleType.ROLE,"GetTotalFriendShipPoint")
	local value_list = ConfigAdapter.Common.GetFriendshipPointConsume()
	if recruitGoodNums >= 1 or friend_point >= value_list[1] then
		red_dot_num = red_dot_num + 1
		self:__AddRedDot(Macros.Game.HERO_RECRUIT_DETAIL_TYPE.LOWFREE_RECRUIT,self:GetWidget("Image_blueCrystal"))
	end
end

--添加红点
function Modules.HeroRecruitView:__AddRedDot(type,node)
	--红点
	if not self["entry_red_dot"..type] then
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self["entry_red_dot"..type] = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		self["entry_red_dot"..type]:SetAnchorPoint(1, 1)
		node:AddChild(self["entry_red_dot"..type],2)
		if type == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.LOWFREE_RECRUIT then
			PosTool.Center(self["entry_red_dot"..type],130,-65)
		else
			PosTool.Center(self["entry_red_dot"..type],130,-62)
		end		
	end
	self["entry_red_dot"..type]:SetVisible(true)
end

--隐藏红点
function Modules.HeroRecruitView:__HideRedDot(type)
	if self["entry_red_dot"..type] then
		self["entry_red_dot"..type]:SetVisible(false)
	end	
end
