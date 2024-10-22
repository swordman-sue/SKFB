Modules = Modules or {}

Modules.TemplArenaRankCell = Modules.TemplArenaRankCell or BaseClass(GUI.ScrollItem)

function Modules.TemplArenaRankCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.ARENA_RANK_CELL)	
end

function Modules.TemplArenaRankCell:SetData(data)
	-- CPPGameLib.PrintTable(data,"shhshsjksksk")
	--排名
	local num_Image = data.rank ~= 0 and data.rank <= 3 and Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_rank_num_"..data.rank) or nil
	--排名
	Util:WidgetLabel(self:GetWidget("Text_rank"),"",Macros.TypefaceSize.popup,Macros.Color.headline)
	if num_Image then
		self:GetWidget("Image_rank"):SetImage(num_Image)
		self:GetWidget("Text_rank"):SetVisible(false)
	else
		self:GetWidget("Image_rank"):SetVisible(false)
		self:GetWidget("Text_rank"):SetText(data.rank)
	end		
	--头像框
	local Panel_roleIcon = self:GetWidget("Panel_roleIcon")
	local roleIcon = LogicGUI.RoleIcon.New(false)
	Panel_roleIcon:AddChild(roleIcon:GetNode())
	PosTool.Center(roleIcon)
	roleIcon:SetData(data.head_icon)
	--名字	
	Util:WidgetLabel(self:GetWidget("Text_playerName"),data.role_name,Macros.TypefaceSize.largish,Macros.Color.headline)
	--等级文字
	Util:WidgetLabel(self:GetWidget("Text_leveltext"),CPPGameLib.GetString("arena_leveltitle"),Macros.TypefaceSize.largish,Macros.Color.keypoint)	
	--等级	
	Util:WidgetLabel(self:GetWidget("Text_level"),data.level,Macros.TypefaceSize.largish,Macros.Color.content)
	--奖励
	Util:WidgetLabel(self:GetWidget("Text_rankEncourage"),CPPGameLib.GetString("arena_encourageText"),Macros.TypefaceSize.largish,Macros.Color.keypoint)
	--具体奖励
	local info = nil
	for i,config in pairs(CPPGameLib.GetConfig("Arena", nil, nil , true )) do
		if data.rank >= config.rank_min and data.rank <= config.rank_max then
			info = config
			break
		end
	end
	--声望
	local txt_reputationnum = self:GetWidget("Text_reputationNum")
	Util:WidgetLabel(txt_reputationnum,info.award_prestige,Macros.TypefaceSize.largish,Macros.Color.content)
	--声望图标
	local image_3_0 = self:GetWidget("Image_3_0")
	--金币
	Util:WidgetLabel(self:GetWidget("Text_goldNum"),info.award_gold,Macros.TypefaceSize.largish,Macros.Color.content)
	--战力
	Util:WidgetLabel(self:GetWidget("Text_powertext"),CPPGameLib.GetString("arena_powertext"),Macros.TypefaceSize.largish,Macros.Color.keypoint)			
	--金币
	Util:WidgetLabel(self:GetWidget("Text_power"),data.battle_value,Macros.TypefaceSize.largish,Macros.Color.content)
end