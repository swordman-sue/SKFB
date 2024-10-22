LogicGUI = LogicGUI or {}

LogicGUI.TemplTeamLotteryRewardPreLook = LogicGUI.TemplTeamLotteryRewardPreLook or BaseClass(GUI.Template)

function LogicGUI.TemplTeamLotteryRewardPreLook:__init(coin_type)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitBaseUI(coin_type)
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_TEAM_LOTTERY_REWARD_CELL)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_TEAM_LOTTERY_REWARD_PRELOOK)
end

function LogicGUI.TemplTeamLotteryRewardPreLook:__delete()
	for _,panel in ipairs(self.__panel_list) do
		panel:DeleteMe()	
		panel = nil		
	end
end

function LogicGUI.TemplTeamLotteryRewardPreLook:__RegistAllEvents()
   
end

function LogicGUI.TemplTeamLotteryRewardPreLook:InitBaseUI(coin_type)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"", function ()
		self:DeleteMe()		
	end)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("team_btn_rewardpreLook"),Macros.TypefaceSize.tab, Macros.Color.btn)
	--奖励预览
	self.__listview_prelook = self:GetWidget("ListView_preLook")
	--容器队列
	self.__panel_list = {}
	--奖励类型
	for reward_type=1,2 do
		local panel = GUI.TemplTeamLotteryRewardCell.New(reward_type,coin_type)
		table.insert(self.__panel_list,panel)
		self.__listview_prelook:PushBackCustomItem(panel:GetNode())
	end
end

