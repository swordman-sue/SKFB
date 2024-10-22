--[[
%% @module: 在线奖励入口
%% @author: ldx
%% @created: 2018-12-22
--]]

Modules = Modules or {}

Modules.OnlineAwardEntry = Modules.OnlineAwardEntry or BaseClass(Modules.BaseMainCityEntry)

function Modules.OnlineAwardEntry:__init()
	local entry_bg = self:GetChildByTag(self.__entry_info.id)

	--时间底
	self.__bg_time = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY,"bg_time"))
	self.__bg_time:SetAnchorPoint(0.5, 0.5)
	entry_bg:AddChild(self.__bg_time)
	PosTool.CenterBottom(self.__bg_time, self.__region_layout_info.entry_txt_offset_x, self.__region_layout_info.entry_txt_offset_y + 32 )

	--倒计时
   	self.onlineaward = Util:Label("",Macros.TypefaceSize.slightly)
	self.onlineaward:SetAnchorPoint(0.5, 0.5)
	self.__bg_time:AddChild(self.onlineaward)
	PosTool.Center(self.onlineaward)

	self:OnlineawardCountdown( 0 )		
end

function Modules.OnlineAwardEntry:RegistAllEvents()
	Modules.BaseMainCityEntry.RegistAllEvents(self)

	--在线奖励推送
	self:BindGlobalEvent(Macros.Event.MaincityModule.ONLINE_REWARD_PUSH, function(_,is_start,online_push_params)
		self:OnlineRewardPush(is_start, online_push_params)
	end)

	--在线倒计时
	self:BindGlobalEvent(Macros.Event.OnlineawardModule.ONLINE_COUNTDOWN, function( _ , time )
		self:OnlineawardCountdown( time )
	end)

	--在线奖励信息
	self:BindGlobalEvent(Macros.Event.OnlineawardModule.ONLINE_REWARD_DATA, function( _ , time )
		self:OnlineawardCountdown( 0 )
	end)
end

--在线奖励推送
function Modules.OnlineAwardEntry:OnlineRewardPush(is_start,online_push_params)
	--剧情开始
	if is_start and online_push_params then
		--文字
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY, "reward_joint_hero",true)
		self.__online_reward_push_txt = Util:Sprite(sp)
		GlobalScene:AddToRenderGroup(self.__online_reward_push_txt, Scene.RenderQueue.GRQ_UI_MAX_II)
		PosTool.Center(self.__online_reward_push_txt,0,200)
		--容器
	    self.__online_push_panel_center = Util:Layout(100, 100)
	    self.__online_push_panel_center:SetAnchorPoint(0.5,0.5)
	    GlobalScene:AddToRenderGroup(self.__online_push_panel_center,Scene.RenderQueue.GRQ_UI_MAX)
	    PosTool.Center(self.__online_push_panel_center)
		--英雄
		self.__online_reward_push_hero = LogicGUI.Model.New({info_id = online_push_params[1],parent = self.__online_push_panel_center,zorder = 5,act_index = RenderObj.ActionIndex.BOSS_SKILL2})
		self.__online_reward_push_hero:SetPosition(50,0)
		--英雄名字
		local config_hero = CPPGameLib.GetConfig("Hero",online_push_params[1])
		self.__online_reward_push_hero_name = Util:Label(config_hero.name, Macros.TypefaceSize.popup, Macros.Quality[config_hero.init_quality], 0, 0, Macros.Quality_Stroke[config_hero.init_quality])
		self.__online_push_panel_center:AddChild(self.__online_reward_push_hero_name)
		PosTool.Center(self.__online_reward_push_hero_name,0,-80)
	else
		if self.__online_reward_push_txt then
			self.__online_reward_push_txt:RemoveSelf(true)
			self.__online_reward_push_txt = nil
		end
		if self.__online_reward_push_hero_name then
			self.__online_reward_push_hero_name:RemoveSelf(true)
			self.__online_reward_push_hero_name = nil
		end
		local del_func = function ()
			self.__online_reward_push_hero:DeleteMe()
			self.__online_reward_push_hero = nil
			if self.__online_push_panel_center then
				self.__online_push_panel_center:RemoveSelf(true)
				self.__online_push_panel_center = nil
			end
		end
		if self.__online_reward_push_hero and self.__region_panel then
			local pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__region_panel, self.__online_push_panel_center, COCOPoint(self:GetPositionX(), self:GetPositionY()))
			ActionManager.GetInstance():RunScaleToAndMoveToEffectEx(self.__online_reward_push_hero:GetNode(), 0.5, pos.x - 60, pos.y, 0, function ()
				del_func()

				local effectid,effect = self:__CreateEffect({id = 100028,loop = false,time_scale = 1,sacle = 0.2},self:GetNode(),100)
			    PosTool.Center(effect,-40,0)

				CPPActionManager.ScaleTo(effect, 0.2, 0.4, GlobalCallbackMgr:AddCallBackFunc(function()
					CPPActionManager.ScaleTo(effect, 0.2, 0, GlobalCallbackMgr:AddCallBackFunc(function()							
					end))
				end))
			    
			end)
		elseif self.__online_reward_push_hero then
			del_func()
		end
	end
end

--在线奖励倒计时
function Modules.OnlineAwardEntry:OnlineawardCountdown( time )
	if self.onlineaward then
		self.__bg_time:SetVisible(true)

		if time > 0 then
			self.onlineaward:SetString(TimeTool.TransTimeStamp( "HH:MM:SS" , time ))
			self.onlineaward:SetColor(unpack(Macros.Color.green))
		else
			local can_received = CallModuleFunc(ModuleType.ONLINE_AWARD, "CheckOnlineReward")
	        if can_received then
				self.onlineaward:SetString( CPPGameLib.GetString("onlineawardMay") )
				self.onlineaward:SetColor(unpack(Macros.Color.green))
			else
				self.__bg_time:SetVisible( false )
				self.onlineaward:SetString("")
			end
		end
	end
end