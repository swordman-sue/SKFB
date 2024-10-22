Modules = Modules or {}

--副本关卡首胜模板
Modules.TemplDungeonFirstWinView = Modules.TemplDungeonFirstWinView or BaseClass(GUI.Template)

function Modules.TemplDungeonFirstWinView:__init(gamelevel_id,fun)
	self.__bg_params = {type = BaseView.BGType.SOLID}	

	self:SetData(data)

	self.__is_auto_load = true
	self.__open_callback = function()
		local panel_center = self:GetWidget("Panel_center")
		panel_center:SetImage(Resource.PathTool.GetUIResPath("citycommonslices/bg_first_win"), TextureResType.LOCAL)

		--关闭
		Util:WidgetButton(self:GetWidget("Button_sure"),CPPGameLib.GetString("common_confirm"),function ()
			self:DeleteMe()
			if fun then
				fun()
			end		
		end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)
		--关卡信息
		local offset_x = 0
		local config_gamelevel = CPPGameLib.GetConfig("GameLevel",gamelevel_id)

		local lst = config_gamelevel.first_reward_type_list or config_gamelevel.first_win_item_id_list
		local num = config_gamelevel.first_reward_value_list or config_gamelevel.first_win_item_num_list

		if config_gamelevel and lst then
			for i,item_type in ipairs( lst ) do
				local icon = LogicGUI.IconLayer.New()
				-- local icon_info = CPPGameLib.GetConfig("HuoBi",item_type)
				local type_ , icon_info = ObjIDTool.GetInfo( item_type )
				self:GetNode():AddChild(icon:GetNode())
				PosTool.Center(icon,offset_x,0)				
				icon:SetData({id = icon_info.big_icon or item_type ,num = num[i] })
				icon:ShowItemName(true)
				offset_x = offset_x + 90
			end		
		end
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CHALLENGE)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CHALLENGE,true)
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_FIRSTWIN, parent_node)
end
