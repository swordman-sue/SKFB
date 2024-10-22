Modules = Modules or {}

--怪物攻城BOSS奖励
Modules.TemplMonsterStormCastleRewardView = Modules.TemplMonsterStormCastleRewardView or BaseClass(GUI.Template)

function Modules.TemplMonsterStormCastleRewardView:__init(start_chapter,cur_chapter,reward_list,is_boss_alive)
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__is_auto_load = true
	self.__open_callback = function()
		--关闭按钮
		Util:WidgetButton(self:GetWidget("Button_close"),"", function ()
			self:DeleteMe()
		end)
		--标题
		Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("monster_stormcastle_boss_reward_title"), Macros.TypefaceSize.tab, Macros.Color.btn)
		--滚动容器
		local panel_bossreward = self:GetWidget("Panel_bossReward")
		local params = {
			item_class = Modules.TemplMonsterStormCastleRewardCell,
		    item_width = 470,
		    item_height = 134,
		    row	= 3,
		    col	= 1,
		    item_space_r = 5,
		    view_width = 470,
		    view_height = 412, 
		}
		self.__boss_reward_srollview = GUI.ScrollView.New(params)
		panel_bossreward:AddChild(self.__boss_reward_srollview:GetNode())
		PosTool.Center(self.__boss_reward_srollview)
		--构造数据
		local structure_data = {}
		for i,reward_info in ipairs(CPPGameLib.GetConfig("AttackCityBossReward", nil, nil , true )) do
			if reward_info.chapter >= start_chapter then
				local data = CPPGameLib.CopyTbl(reward_info)
				data.__have_reward = false
				data.__could_reward = false
				if cur_chapter > reward_info.chapter and reward_list[reward_info.chapter] then
					data.__have_reward = true
				elseif cur_chapter > reward_info.chapter then
					data.__could_reward = true
				elseif cur_chapter == reward_info.chapter and not is_boss_alive then
					data.__could_reward = true
				end
				table.insert(structure_data,data)
			end
		end
		--排序
		table.sort(structure_data,function (data_1,data_2)
			if data_1.__have_reward and data_2.__have_reward then
				return data_1.chapter < data_2.chapter
			elseif data_1.__have_reward and not data_2.__have_reward then
				return false
			elseif not data_1.__have_reward and data_2.__have_reward then
				return true
			else
				if data_1.__could_reward == data_2.__could_reward then	
					return data_1.chapter < data_2.chapter
				else
					return data_1.__could_reward
				end
			end	
		end)
		self.__boss_reward_srollview:SetDataList(structure_data)
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_MONSTER_STROMCASTLE_REWARD_CELL)	
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_MONSTER_STROMCASTLE_REWARD_VIEW)	
end

function Modules.TemplMonsterStormCastleRewardView:__delete()
	if self.__boss_reward_srollview then
		self.__boss_reward_srollview:DeleteMe()
		self.__boss_reward_srollview = nil
	end
end

function Modules.TemplMonsterStormCastleRewardView:__RegistAllEvents()
	--领取怪物攻城Boss奖励响应
	self:BindGlobalEvent(Macros.Event.MonsterStormCastleModule.ATTACKCITY_TAKE_BOSSREWARD_RESP, function(_,chapter)
		self.__boss_reward_srollview:ItemDataIter(function (index, data,item)
			if data.chapter == chapter then
				item:ResetData()
				return true
			end
		end)
	end)
end