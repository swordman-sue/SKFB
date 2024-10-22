--[[
%% @module: 占卜抽卡界面预览
%% @author: yjg
%% @created: 2017年12月4日11:15:02
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplDivinationRewardPrelook = LogicGUI.TemplDivinationRewardPrelook or BaseClass(GUI.Template)

function LogicGUI.TemplDivinationRewardPrelook:__init( type )
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self.type = type
		self:InitBaseUI()
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_TEAM_LOTTERY_REWARD_CELL)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_TEAM_LOTTERY_REWARD_PRELOOK)
end

function LogicGUI.TemplDivinationRewardPrelook:__delete()
	for _,panel in ipairs(self.__panel_list) do
		panel:DeleteMe()	
		panel = nil		
	end
end

function LogicGUI.TemplDivinationRewardPrelook:__RegistAllEvents()
   
end

function LogicGUI.TemplDivinationRewardPrelook:InitBaseUI()
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"", function ()
		self:DeleteMe()		
	end)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("divination_preview"),Macros.TypefaceSize.tab, Macros.Color.btn)
	--奖励预览
	self.__listview_prelook = self:GetWidget("ListView_preLook")
	--容器队列
	self.__panel_list = {}

	local lst = {}
	local lst_len = 0
	if self.type == Macros.Game.CARD_TYPE.LOW then
		local mapIdx = CallModuleFunc(ModuleType.DIVINATION, "GetLowCardGroupMapIdx" )	
		local info = CPPGameLib.GetConfig("LowCardGroup", mapIdx )

		local t1 = {}
		for i , v in pairs( info.card_group_list ) do
			if not t1[v.quality] then
				t1[v.quality] = {}
				lst_len = lst_len + 1
			end
			table.insert( t1[v.quality] , v )
		end

		local t2 = {}
		for i = 0 , #t1 do
			local t3 = {}
			if t1[i] then
				for i , v in ipairs(t1[i]) do
					table.insert( t3 , v.card_id )
				end
				table.sort( t3 , function ( a , b )
					return a < b
				end )
				table.insert( t2 , { card_list = t3 , sort = t1[i][1].quality + 1 , title = CPPGameLib.GetString("divination_quality"..( t1[i][1].quality + 1 )  ) } )
			end
		end

		table.sort( t2 , function ( a , b )
			return a.sort > b.sort 
		end )	

		lst = t2

	elseif self.type == Macros.Game.CARD_TYPE.TALL then
		local card_group_list = CPPGameLib.GetConfig("HighCardGroup", nil, nil, true )
		local t1 = {}
		for i , v in pairs( card_group_list ) do
			if not t1[v.quality] then
				t1[v.quality] = {}
				lst_len = lst_len + 1				
			end
			table.insert( t1[v.quality] , v )
		end

		local t2 = {}
		for i = 0 , #t1 do
			local t3 = {}
			if t1[i] then
				for i , v in ipairs(t1[i]) do
					table.insert( t3 , v.card_id )
				end
				table.sort( t3 , function ( a , b )
					return a < b
				end )
				table.insert( t2 , { card_list = t3 , sort = t1[i][1].quality + 1 , title = CPPGameLib.GetString("divination_quality"..( t1[i][1].quality + 1 )  ) } )
			end
		end

		table.sort( t2 , function ( a , b )
			return a.sort > b.sort 
		end )	

		lst = t2
	else

		local info = CPPGameLib.GetConfig("CardHandBook", nil , nil , true )
		local t1 = {}
		for i , v in ipairs( info ) do
			table.insert( t1 , { card_list = v.card_list , title = v.title } )
			lst_len = lst_len + 1				
		end
		lst = t1
	end

	for i = 1, lst_len do
		if lst[i] then
			local panel = GUI.TemplDivinationRewardCell.New( lst[i] )
			self.__listview_prelook:PushBackCustomItem(panel:GetNode())
		end
	end
end

