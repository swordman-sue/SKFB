Modules = Modules or {}

Modules.TemplMonsterStormCastleRankCell = Modules.TemplMonsterStormCastleRankCell or BaseClass(GUI.ScrollItem)

function Modules.TemplMonsterStormCastleRankCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.ARENA_RANK_ENCOURAGE_CELL)	
end

function Modules.TemplMonsterStormCastleRankCell:SetData(data)
	GUI.ScrollItem.SetData(self,data)
	--获取配置
	local rank_config = CPPGameLib.GetConfig("AttackCityRankReward",self.__data.rank)
	--排名
	local num_Image = self.__data.rank ~= 0 and self.__data.rank <= 3 and Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_rank_num_"..self.__data.rank) or nil
	--排名
	Util:WidgetLabel(self:GetWidget("Text_rank"),"", Macros.TypefaceSize.largish,Macros.Color.headline)
	if num_Image then
		self:GetWidget("Image_rank"):SetImage(num_Image)
		self:GetWidget("Text_rank"):SetVisible(false)
	else
		self:GetWidget("Image_rank"):SetVisible(false)
		self:GetWidget("Text_rank"):SetText(rank_config.rank_min.."-"..rank_config.rank_max)
	end	
	--声望
	local str = ""
	if rank_config.item_id_list[1] then
		local iten_config = CPPGameLib.GetConfig("Item",rank_config.item_id_list[1])
		str = iten_config.param2 * rank_config.item_num_list[1]
	end
	Util:WidgetLabel(self:GetWidget("Text_reputationNum"),str,Macros.TypefaceSize.largish,Macros.Color.content)
	str = ""
	if rank_config.item_id_list[2] then
		local iten_config = CPPGameLib.GetConfig("Item",rank_config.item_id_list[2])
		str = iten_config.param2 * rank_config.item_num_list[2]
	end
	--金币
	Util:WidgetLabel(self:GetWidget("Text_goldNum"),str,Macros.TypefaceSize.largish,Macros.Color.content)
	--背景图片
	self:GetWidget("Image_bg"):SetVisible(self.__data.rank%2 == 0)
	--货币
	for i=1,2 do
		local image_type = self:GetWidget("Image_type"..i)
		if rank_config.item_id_list[i] then
			local sp = Resource.PathTool.GetItemPath(rank_config.item_id_list[i]) 
			image_type:IgnoreContentAdaptWithSize(true)
			image_type:SetImage(sp,TextureResType.LOCAL)
			image_type:SetScaleFactor(i == 1 and 0.5 or 0.7)
		else
			image_type:SetVisible(false)
		end
	end	
end