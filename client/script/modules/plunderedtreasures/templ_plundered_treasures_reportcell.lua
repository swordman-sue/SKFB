Modules = Modules or {}

Modules.TemplPlunderedTreasuresReportCell = Modules.TemplPlunderedTreasuresReportCell or BaseClass(GUI.ScrollItem)

function Modules.TemplPlunderedTreasuresReportCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_REPORT)
end

function Modules.TemplPlunderedTreasuresReportCell:SetData(data)
	local role_Level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local role_info = CPPGameLib.GetConfig("DuobaoMap",role_Level) 
	--第几次
	Util:WidgetLabel(self:GetWidget("Text_Times"),CPPGameLib.GetString("plunderedtreasures_SaoDangTimes",data.times),Macros.TypefaceSize.largish,Macros.Color.headline )
	--获得金币
	Util:WidgetLabel(self:GetWidget("Text_getSomething"),CPPGameLib.GetString("plunderedtreasures_getSomething"),Macros.TypefaceSize.normal,Macros.Color.keypoint)
	--获得金币数量
	Util:WidgetLabel(self:GetWidget("Text_getGoldDes"),role_info.win_gold,Macros.TypefaceSize.normal,Macros.Color.content)
	--获得经验数量
	Util:WidgetLabel(self:GetWidget("Text_getExpDes"),role_info.exp,Macros.TypefaceSize.normal,Macros.Color.content)
	--翻牌奖励
	Util:WidgetLabel(self:GetWidget("Text_cardEncourage"),CPPGameLib.GetString("plunderedtreasures_congratulateText"),Macros.TypefaceSize.normal,Macros.Color.keypoint)
	--翻牌奖励获得物品
	local itemInfo = CPPGameLib.GetConfig("Item",data.item_id)
	Util:WidgetLabel(self:GetWidget("Text_cardEncourageDes"),itemInfo.name.." x"..data.item_num,Macros.TypefaceSize.normal,Macros.Quality[itemInfo.quality],0,0,Macros.Quality_Stroke[itemInfo.quality])
	
	local Panel_head = self:GetWidget("Panel_head")
	if data.flag == 1 then
		self:GetWidget("Text_noGet"):SetVisible(false)
		local icon = LogicGUI.IconLayer.New(false,false)
		Panel_head:AddChild(icon:GetNode())			
		icon:SetData({id = data.treasure_item_id})	
		icon:adaptParentContentSize()
		PosTool.Center(icon)
		icon:ShowItemName(true)
	else
		--未抢到
		Util:WidgetLabel(self:GetWidget("Text_noGet"),CPPGameLib.GetString("plunderedtreasures_haveNoGet"),Macros.TypefaceSize.normal,Macros.Color.red)
		Panel_head:SetVisible(false)
	end
end