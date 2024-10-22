--[[
%% @module：一键升级1模板
%% @author: ldx
%% @created: 2017/8/4
--]]

Modules = Modules or {}

Modules.TemplHeroOnekeyLevelUpTipsOne = Modules.TemplHeroOnekeyLevelUpTipsOne or BaseClass(GUI.Template)

function Modules.TemplHeroOnekeyLevelUpTipsOne:__init(material_list,hero_info,cur_hero_uid,confirm_func)
	self.__is_auto_load = true	
	self.__open_callback = function()
		self:__OpenCallback(material_list,hero_info,cur_hero_uid,confirm_func)
	end
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self:InitTemplate(GUI.TemplateRegistry.HERO_ONEKEY_LEVELUP_TIPS_ONE)
end

function Modules.TemplHeroOnekeyLevelUpTipsOne:__OpenCallback(material_list,hero_info,cur_hero_uid,confirm_func)
	self.__material_list = material_list
	self.__hero_info = hero_info
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_upgrade_onekey_levelup_btn"), Macros.TypefaceSize.tab, Macros.Color.btn)
	--关闭
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)
	--提示1
	Util:WidgetLabel(self:GetWidget("Text_tip1"),CPPGameLib.GetString("hero_onekey_levelup_tips3"), Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--消耗
	Util:WidgetLabel(self:GetWidget("Text_cost"),CPPGameLib.GetString("hero_recruitCostText"), Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--消耗金币
	self.__text_costgold = self:GetWidget("Text_costGold")
	Util:WidgetLabel(self.__text_costgold,"", Macros.TypefaceSize.largish, Macros.Color.content)
	--消耗经验
	self.__text_costexp = self:GetWidget("Text_costExp")
	Util:WidgetLabel(self.__text_costexp,"", Macros.TypefaceSize.largish, Macros.Color.content)
	--确定
	Util:WidgetButton(self:GetWidget("Button_sure"), CPPGameLib.GetString("common_confirm"),function ()
		local gold_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.GOLD)
		if gold_num < self.__cost_exp_list[self.__cur_choose_num] then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_onekey_levelup_tips4"))		
			return
		end
		CPPGameLib.PrintTable("ldx",self.__cost_hero_list[self.__cur_choose_num],"self.__cost_hero_list[self.__cur_choose_num]self.__cost_hero_list[self.__cur_choose_num]")
		if confirm_func then
			confirm_func(self.__cur_choose_num)
		end
		CallModuleFunc(ModuleType.HERO,"HeroUpgradeReq", cur_hero_uid, self.__cost_hero_list[self.__cur_choose_num])
		self:DeleteMe()
	end, Macros.TypefaceSize.popup, Macros.Color.btn, Macros.Color.button_red)
	--取消
	Util:WidgetButton(self:GetWidget("Button_cancel"), CPPGameLib.GetString("common_cancel"),function ()
		self:DeleteMe()
	end, Macros.TypefaceSize.popup, Macros.Color.btn, Macros.Color.button_yellow)
	--选择数量
	self.__cur_choose_num = 1
	self.__text_choosenum = self:GetWidget("Text_chooseNum")
	Util:WidgetLabel(self.__text_choosenum,self.__cur_choose_num, Macros.TypefaceSize.largish, Macros.Color.content)
	--数量按钮
	--加1
	Util:WidgetButton(self:GetWidget("Button_add"),"",function ()
		self:OnBtnAdd(1)
	end,nil,nil,nil,nil,0)
	--加10
	Util:WidgetButton(self:GetWidget("Button_add10"),"",function ()
		self:OnBtnAdd(10)
	end,nil,nil,nil,nil,0)
	--减1
	Util:WidgetButton(self:GetWidget("Button_sub"),"",function ()
		self:OnBtnSub(1)
	end,nil,nil,nil,nil,0)
	--减10
	Util:WidgetButton(self:GetWidget("Button_sub10"),"",function ()
		self:OnBtnSub(10)
	end,nil,nil,nil,nil,0)
	--
	self:CountTotalExp()
end

--增加数量
function Modules.TemplHeroOnekeyLevelUpTipsOne:OnBtnAdd(num)
	if self.__cur_choose_num >= self.__cur_max_num then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_onekey_levelup_tips5"))
		return
	elseif self.__cur_choose_num + num >= self.__cur_max_num then
		self.__cur_choose_num = self.__cur_max_num			
	else
		self.__cur_choose_num = self.__cur_choose_num + num
	end
	self:CountTotalExp(self.__cur_choose_num)
end

--减少数量
function Modules.TemplHeroOnekeyLevelUpTipsOne:OnBtnSub(num)
	if self.__cur_choose_num <= 1 then
		return 
	elseif self.__cur_choose_num - num <= 1  then
		self.__cur_choose_num = 1
	else 
		self.__cur_choose_num = self.__cur_choose_num - num		
	end
	self:CountTotalExp(self.__cur_choose_num)
end

--计算总数量
function Modules.TemplHeroOnekeyLevelUpTipsOne:CountTotalExp(choose_num)
	if choose_num then
		self.__text_choosenum:SetText(choose_num)
		--消耗金币
		self.__text_costgold:SetText(self.__cost_exp_list[choose_num] or 0)
		--消耗经验
		self.__text_costexp:SetText(self.__cost_exp_list[choose_num] or 0)
	else
		self.__cost_hero_list = {}
		self.__cost_exp_list = {}
		--计算每级升级需要的经验
		local level_up_exp_list = {}
		local config = CPPGameLib.GetConfig("HeroLevel", self.__hero_info.quality_level)
		local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)	
		--英雄等级上限
		local config_info = CPPGameLib.GetConfig("Hero", self.__hero_info.info_id)
		local add_exp = 0
		local up_limit = math.min(config_info.limit_level - 1,role_level -1)
		for i = self.__hero_info.level,up_limit do
			add_exp = add_exp + config.level_list[i].exp
			level_up_exp_list[i - self.__hero_info.level + 1] = add_exp - self.__hero_info.exp
		end	
		--计算最大和最小级数
		self.__cur_max_num = 1
		--升级所需经验	
		local count_exp = 0	
		for _, cost_hero_info in ipairs(self.__material_list) do
			self.__cost_hero_list[self.__cur_max_num] = self.__cost_hero_list[self.__cur_max_num] or {}
			table.insert(self.__cost_hero_list[self.__cur_max_num],{uid = cost_hero_info.uid})
			count_exp = count_exp + Modules.HeroModule.GetHeroExp(cost_hero_info)			
			if count_exp >= level_up_exp_list[self.__cur_max_num] then
				for i=1,#level_up_exp_list do
					if count_exp >= level_up_exp_list[self.__cur_max_num] then					
						self.__cost_exp_list[self.__cur_max_num] = count_exp
						self.__cost_hero_list[self.__cur_max_num + 1] = CPPGameLib.CopyTbl(self.__cost_hero_list[self.__cur_max_num])
						self.__cur_max_num = self.__cur_max_num + 1
						if self.__cur_max_num > #level_up_exp_list then
							break
						end
					else
						break
					end
				end				
			end
			if self.__cur_max_num > #level_up_exp_list then
				break
			end
		end
		self.__cur_max_num = self.__cur_max_num - 1
		--默认1级
		self:CountTotalExp(1)
	end
end