--[[
%% @module: 英雄降临界面
%% @author: ldx
%% @created: 2017-06-20
--]]

Modules = Modules or {} 

Modules.HeroAdventView = Modules.HeroAdventView or BaseClass(BaseView)

local progress_to_time = 5

function Modules.HeroAdventView:__init()
	self.__layout_name = "dungeon_passed_encourage"	
    self.__bg_params = {type = BaseView.BGType.SOLID} 
	self.__open_callback = function()
  	 	self:__OpenCallback()
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_ADVENT)    
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_ADVENT,true)  
end

function Modules.HeroAdventView:__OpenCallback()
	self.__is_special_open = unpack(self.__open_data)
	--设置定时器
    self:SetUpdateFunc(function (elapse_time)
        --进度条
        if self.__smooth_end_time and self.__smooth_end_time > 0 then
        	local is_end
            self.__smooth_elapse_time = self.__smooth_elapse_time + elapse_time
            local progress = self.__cur_passed_chapter_num + self.__smooth_cur_speed * self.__smooth_elapse_time
            if CPPGameEngine:GetNowTime() - self.__smooth_end_time >= 0 then
                self.__smooth_end_time = 0
                progress = self.__cur_passed_chapter_num + 1   
                is_end = true                                                               
            end
            --设置进度
            self.__loading_bar:SetPercent(math.min(1,(progress/self.__target_passed_chapter_num))*100)
            if is_end then
				--当前通关章节数
				self.__text_progressnum:SetText((self.__cur_passed_chapter_num + 1).."/"..self.__target_passed_chapter_num)
			end
        end
    end)
    --关闭按钮  
    Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
        --请求红点
        self:__UpdateRedDotNum()
    end)
    --闪光特效
	local effect_id, effect = self:__CreateEffect({id = 100017}, self:GetWidget("Panel_effect"),2)
	effect:SetAnchorPoint(0.5,0)
	PosTool.CenterBottom(effect)		
    --图标
    self.__image_adventicon = self:GetWidget("Image_adventIcon")
    --降临文字
    self.__image_txt = self:GetWidget("Image_txt")
    --领取提示
	self.__text_sectitle = self:GetWidget("Text_secTitle")
	Util:WidgetLabel(self.__text_sectitle,"",Macros.TypefaceSize.largish, Macros.Color.btn,0,0,Macros.Color.common_stroke)	

	--宝箱
	self.__image_dungeonbox = self:GetWidget("Image_dungeonBox")
	--宝箱特效容器
	self.__panel_boxeffect = self:GetWidget("Panel_boxEffect")
	--领取按钮
	self.__button_get = self:GetWidget("Button_moreType")
	--进度条
	self.__loading_bar = self:GetWidget("LoadingBar")		
	--进度条进度文字
	self.__text_progressnum = self:GetWidget("Text_progressNum")
	self.__text_progressnum:SetPosition(self.__text_progressnum:GetPositionX(),self.__text_progressnum:GetPositionY()+3)
	Util:WidgetLabel(self.__text_progressnum,"", Macros.TypefaceSize.largish,Macros.Color.btn,0,0,Macros.Color.common_stroke)
	--初始化
	self:__InitAllWidget()
end	

function Modules.HeroAdventView:__RegistAllEvents()    
	--领取完奖励
    self:BindGlobalEvent(Macros.Event.DungeonModule.TAKE_MAIN_FB_CHAPTER_REWARD,function (_,data)
  		local dungeonbox_reward_level = CallModuleFunc(ModuleType.DUNGEON,"GetFBPassedBoxRewardLevel")
  		if dungeonbox_reward_level < #CPPGameLib.GetConfig("MainFBChapterReward",nil,nil,true) then
  			local reward_info = CPPGameLib.GetConfig("MainFBChapterReward",dungeonbox_reward_level + 1)
  			--当前通关章节数
			local passed_chapter_num = CallModuleFunc(ModuleType.DUNGEON,"GetPassedAllChapterNum")	
			if passed_chapter_num >= reward_info.need_chapter then
				self.__button_get:SetEnabled(true)
				self:__InitAllWidget()				
			else
				self:__UpdateRedDotNum()			
			end
  		else
  			self:__UpdateRedDotNum()
  		end	
  		--更新进度
  		self:FireNextFrame(Macros.Event.MaincityModule.REFRASH_HERO_ADVENT_RESP)
  		--奖励
  		local reward_list = {}
        for i,info in ipairs(data.item_list) do
            table.insert(reward_list,{id = info.item_id , num = info.item_num})
        end
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , reward_list )	    	
    end)
end

function Modules.HeroAdventView:__InitAllWidget()
	self.__dungeonbox_reward_level = CallModuleFunc(ModuleType.DUNGEON,"GetFBPassedBoxRewardLevel")
	local reward_info = CPPGameLib.GetConfig("MainFBChapterReward",self.__dungeonbox_reward_level + 1,false)
	if reward_info then
		local item_info = CPPGameLib.GetConfig("Item",reward_info.item_id)
		--当前通关章节数
		local passed_chapter_num = CallModuleFunc(ModuleType.DUNGEON,"GetPassedAllChapterNum")
		--控件
		self.__image_dungeonbox:SetVisible(not (passed_chapter_num >= reward_info.need_chapter))
		self.__panel_boxeffect:SetVisible(passed_chapter_num >= reward_info.need_chapter)	
		local tips_str,bt_str
		if passed_chapter_num >= reward_info.need_chapter then
			tips_str = CPPGameLib.GetString("dungeon_chapter_reward_could_take")
			bt_str = CPPGameLib.GetString("dungeon_receive")		
			if not self.__box_effectid then
				local effect 
				self.__box_effectid,effect = self:__CreateEffect({res_name = "UI_yaoqianshuguang2"},self:GetWidget("Panel_boxEffect"))
				PosTool.Center(effect)
			end
		else
			tips_str = CPPGameLib.GetString("dungeon_chapter_reward_condition",reward_info.need_chapter - passed_chapter_num , item_info.name)
			bt_str = CPPGameLib.GetString("common_confirm")	
		end	
		--次标题
		self.__text_sectitle:SetText(tips_str)
		--是否特殊处理
		if self.__is_special_open then			
			local effectid,effect = self:__CreateEffect({id = 100102,loop = false},self.__loading_bar,2)
			PosTool.Center(effect)
			if not (passed_chapter_num >= reward_info.need_chapter) then
				--未满进度
				self.__smooth_cur_speed = 1
	            self.__smooth_elapse_time = 0
	            self.__smooth_end_time = CPPGameEngine:GetNowTime() + 1/self.__smooth_cur_speed
	            self.__cur_passed_chapter_num = passed_chapter_num - 1
	            self.__target_passed_chapter_num = reward_info.need_chapter
	        else
				--满进度
				--当前通关章节数
				self.__text_progressnum:SetText(passed_chapter_num.."/"..reward_info.need_chapter)
				--进度条
				self.__loading_bar:SetPercent(math.min(1,(passed_chapter_num/reward_info.need_chapter))*100)
			end
		else
			--当前通关章节数
			self.__text_progressnum:SetText(passed_chapter_num.."/"..reward_info.need_chapter)
			--进度条
			self.__loading_bar:SetPercent(math.min(1,(passed_chapter_num/reward_info.need_chapter))*100)
		end
		--图标
		local icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_ADVENT, reward_info.is_equip == Macros.Global.TRUE and "equip_icon" or "hero_icon")
		self.__image_adventicon:SetImage(icon_sp)
		--降临文字
		local txt_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_ADVENT, reward_info.is_equip == Macros.Global.TRUE and "equip_advent_txt" or "hero_advent_txt",true)
		self.__image_txt:SetImage(txt_sp)
		--按钮
		Util:WidgetButton(self.__button_get,bt_str, function ()
			if passed_chapter_num >= reward_info.need_chapter then
				--判断英雄或者装备数量是否已满			 
				CallModuleFunc(ModuleType.SUNDRY, "PackVacancy",reward_info.is_equip == Macros.Global.TRUE and Macros.Game.ObjType.EQUIP or Macros.Game.ObjType.HERO , nil , true , function (item_num)
				    if item_num >= 1 then
				    	self.__button_get:SetEnabled(false)
						CallModuleFunc(ModuleType.DUNGEON,"FBPassedBoxRewardReq",self.__dungeonbox_reward_level+1)
					else
						local tips_string = reward_info.is_equip == Macros.Global.TRUE and CPPGameLib.GetString("pack_equipMax") or CPPGameLib.GetString("pack_heroMax")
						GlobalTipMsg.GetInstance():Show(tips_string)
						return
				    end
				end)
			else
				self:__UpdateRedDotNum()
				if self.__is_special_open then
					return
				end				
			end
		end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_yellow )	
		--可领取
		if passed_chapter_num >= reward_info.need_chapter then
			if not self.__button_zoubian_effect_id then
				local effect
				self.__button_zoubian_effect_id,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.__button_get)
				effect:SetScaleXFactor(1.25)
				effect:SetScaleYFactor(1.15)
				PosTool.Center(effect)
			end
		else
			if self.__button_zoubian_effect_id then
				self:__DelEffect(self.__button_zoubian_effect_id)
				self.__button_zoubian_effect_id = nil
			end
		end
	end
end

function Modules.HeroAdventView:__UpdateRedDotNum()
	self:CloseManual()
end

function Modules.HeroAdventView:__Dispose()
	if self.__button_zoubian_effect_id then
		self:__DelEffect(self.__button_zoubian_effect_id)
		self.__button_zoubian_effect_id = nil
	end
end