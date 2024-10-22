--[[
%% @module: 外域入侵界面
%% @author: ldx
%% @created: 2017-05-08
	
--]]

Modules = Modules or {}

Modules.TemplDungeonExternalAreaView = Modules.TemplDungeonExternalAreaView or BaseClass(GUI.Template)

function Modules.TemplDungeonExternalAreaView:__init(data)	
	self.__bg_params = {type = BaseView.BGType.SOLID}	

	self:SetData(data)

	self.__is_auto_load = true
	self.__open_callback = function()

		--关闭按钮
		Util:WidgetButton(self:GetWidget("Button_close"), "",function ()
			self:DeleteMe()
		end)

		--提示
		local time_list = {}
		for i,time in ipairs(ConfigAdapter.Common.GetDungeonExternalAreaRefrashTimeList()) do
			local hour = tonumber(string.sub(time,1,string.len(time) == 4 and 2 or 1))	
			local min = tonumber(string.sub(time,string.len(time) == 4 and 3 or 2,string.len(time)))
			if min <= 9 then
				time_list[i] = string.format("%d:0%d",hour,min)
			else
				time_list[i] = string.format("%d:%d",hour,min)
			end			
		end
		Util:WidgetLabel(self:GetWidget("Text_tips"),CPPGameLib.GetString("dungeon_external_area_tips",time_list[1],time_list[2],time_list[3]), Macros.TypefaceSize.largish, Macros.Color.keypoint , 580, 50)
		
		--条目容器
		local panel_scrollview = self:GetWidget("Panel_scrollview")
		local params = {
			item_class = Modules.TemplDungeonExternalAreaCell,
		    item_width = 210,
		    item_height	= 369,
		    row	 = 1,
		    col	 = 3,
		    horizon = true,
		    view_width = 630,
		    view_height = 369,
		}
		self.__external_scrollview = GUI.ScrollView.New(params)
		panel_scrollview:AddChild(self.__external_scrollview:GetNode())
		PosTool.Center(self.__external_scrollview)
		self.__external_scrollview:SetDataList(self.__data)

		--错误码回调
		self:BindGlobalEvent(Macros.Event.ErrorNotifyModule.ERROR_NOTIFY, function(_,error_code)									
			if error_code == 20001 then
				self:DeleteMe()
			end
		end)
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CHALLENGE)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_ELITE)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_ELITE,true)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_EXTERNAL_AREA_CELL)
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_EXTERNAL_AREA_VIEW)	
end

function Modules.TemplDungeonExternalAreaView:__delete()
	if self.__external_scrollview then
		self.__external_scrollview:DeleteMe()
		self.__external_scrollview = nil
	end
end