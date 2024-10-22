--[[
%% @module: 扩展背包入口
%% @author: yjg
%% @created: 2019年1月4日20:08:37
--]]

Modules = Modules or {}

Modules.PackExtendEntry = Modules.PackExtendEntry or BaseClass(Modules.BaseMainCityEntry)

function Modules.PackExtendEntry:__init()
	self:__UpdateNewTag()
end

function Modules.PackExtendEntry:RegistAllEvents()
	Modules.BaseMainCityEntry.RegistAllEvents(self)

	self:BindGlobalEvent(Macros.Event.NewModule.PACK_EXTEND, function(_, system_id)
		if self.__system_id == system_id or self.__system_id == Macros.Game.SystemID.PACK_EXTEND then			
			self:__UpdateNewTag()
		end
	end)
end

--是否显示新增标记图标
function Modules.PackExtendEntry:__IsNewTagVisible()
	local new_record_map

	--背包总入口
	if self.__system_id == Macros.Game.SystemID.PACK_EXTEND then
		local sys_id_list = {
			Macros.Game.SystemID.Pack, Macros.Game.SystemID.HERO_BAG, Macros.Game.SystemID.EQUIP, 
			Macros.Game.SystemID.TREASURE, Macros.Game.SystemID.DIVINATION_PACK}
		
		for _, sys_id in ipairs(sys_id_list) do
			new_record_map = CallSystemFunc(sys_id, "GetNewRecordMap")
			if new_record_map and next(new_record_map) then
				break
			end
		end

	--各类背包入口
	else
		new_record_map = CallSystemFunc(self.__system_id, "GetNewRecordMap")
	end

	return new_record_map and next(new_record_map) ~= nil
end

--更新新增标记图标的显隐
function Modules.PackExtendEntry:__UpdateNewTag( )
	local is_new_tag_visible = self:__IsNewTagVisible()
	if is_new_tag_visible and not self.__sprite_new_tag then
		self.__sprite_new_tag = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY , "new_tips" ) )
		self.__sprite_new_tag:SetAnchorPoint( 0 , 0.5 )
		self.__entry_btn:AddChild( self.__sprite_new_tag )
		PosTool.Center( self.__sprite_new_tag , 0 , 30 )
	end
	if self.__sprite_new_tag then
		self.__sprite_new_tag:SetVisible(is_new_tag_visible)
	end
end