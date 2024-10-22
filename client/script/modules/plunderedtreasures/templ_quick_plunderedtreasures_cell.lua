Modules = Modules or {}

Modules.TemplQuickPlunderedTreasuresCell = Modules.TemplQuickPlunderedTreasuresCell or BaseClass(GUI.ScrollItem)

function Modules.TemplQuickPlunderedTreasuresCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.QUICK_PLUNDERED_TREASURES_CELL)
end

function Modules.TemplQuickPlunderedTreasuresCell:SetData(data)
	GUI.ScrollItem.SetData(self,data)
	local role_Level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local role_info = CPPGameLib.GetConfig("DuobaoMap",role_Level) 
	--第几次
	Util:WidgetLabel(self:GetWidget("Text_Times"),CPPGameLib.GetString("plunderedtreasures_SaoDangTimes",self.__data.times),Macros.TypefaceSize.largish,Macros.Color.headline )
	--获得金币
	Util:WidgetLabel(self:GetWidget("Text_getSomething"),CPPGameLib.GetString("plunderedtreasures_getSomething"),Macros.TypefaceSize.normal,Macros.Color.keypoint)
	--获得金币数量
	Util:WidgetLabel(self:GetWidget("Text_getGoldDes"),role_info.win_gold,Macros.TypefaceSize.normal,Macros.Color.content)
	--获得经验数量
	Util:WidgetLabel(self:GetWidget("Text_getExpDes"),role_info.exp,Macros.TypefaceSize.normal,Macros.Color.content)
	--翻牌奖励
	Util:WidgetLabel(self:GetWidget("Text_cardEncourage"),CPPGameLib.GetString("plunderedtreasures_congratulateText"),Macros.TypefaceSize.normal,Macros.Color.keypoint)
	--翻牌奖励获得物品
	local itemInfo = CPPGameLib.GetConfig("Item",self.__data.item_id)
	Util:WidgetLabel(self:GetWidget("Text_cardEncourageDes"),itemInfo.name.." x"..self.__data.item_num,Macros.TypefaceSize.normal,Macros.Quality[itemInfo.quality],0,0,Macros.Quality_Stroke[itemInfo.quality])
	--头像
	local Panel_head = self:GetWidget("Panel_head")
	local txt_piecename = self:GetWidget("Text_PieceName")
	itemInfo = CPPGameLib.GetConfig("Item",self.__data.treasure_item_id)
	Util:WidgetLabel(txt_piecename,itemInfo.name,Macros.TypefaceSize.normal,Macros.Quality[itemInfo.quality],0,0,Macros.Quality_Stroke[itemInfo.quality])
	--是否获取碎片
	if self.__data.flag == 1 then
		self:GetWidget("Text_noGet"):SetVisible(false)
		local icon = LogicGUI.IconLayer.New(false,false)
		Panel_head:AddChild(icon:GetNode())			
		icon:SetData({id = self.__data.treasure_item_id})	
		icon:adaptParentContentSize()
		PosTool.Center(icon)
	else
		--未抢到
		Util:WidgetLabel(self:GetWidget("Text_noGet"),CPPGameLib.GetString("plunderedtreasures_haveNoGet"),Macros.TypefaceSize.popup,Macros.Color.red)
		Panel_head:SetVisible(false)
	end
end