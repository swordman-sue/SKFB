Modules = Modules or {}

Modules.TemplArenaRankEncourageCell = Modules.TemplArenaRankEncourageCell or BaseClass(GUI.ScrollItem)

function Modules.TemplArenaRankEncourageCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.ARENA_RANK_ENCOURAGE_CELL)	
end

function Modules.TemplArenaRankEncourageCell:SetData(data)
	--排名
	local num_Image = data.listNum ~= 0 and data.listNum <= 3 and Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_rank_num_"..data.listNum) or nil
	--排名
	Util:WidgetLabel(self:GetWidget("Text_rank"),"", Macros.TypefaceSize.largish,Macros.Color.headline)
	if num_Image then
		self:GetWidget("Image_rank"):SetImage(num_Image)
		self:GetWidget("Text_rank"):SetVisible(false)
	else
		self:GetWidget("Image_rank"):SetVisible(false)
		self:GetWidget("Text_rank"):SetText(data.rank_min.."-"..data.rank_max)
	end	
	--声望
	Util:WidgetLabel(self:GetWidget("Text_reputationNum"),data.award_prestige,Macros.TypefaceSize.largish,Macros.Color.content)
	--金币
	Util:WidgetLabel(self:GetWidget("Text_goldNum"),data.award_gold,Macros.TypefaceSize.largish,Macros.Color.content)
	--背景图片
	self:GetWidget("Image_bg"):SetVisible(data.listNum%2 == 0)
		
end