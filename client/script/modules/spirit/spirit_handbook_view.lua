--[[
--
-- @brief 精灵图鉴
-- @auther: ldx
-- @date 2017年5月17日 11:58:28
--
]]

Modules = Modules or {}
Modules.SpiritHandBookView = Modules.SpiritHandBookView or BaseClass(BaseView)

function Modules.SpiritHandBookView:__init()
	self.__layout_name = "spirit_handbook_view"
	self.__bg_params = {type = BaseView.BGType.IMAGE,image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_spirit")}
	self.__title_bar_params = 
	{
		close_func = function ()
		   self:CloseManual()
		end ,
		title = CPPGameLib.GetString("spirit_handbook_system"),
	}
	self.__open_callback = function ()
		self.__spirit_data = self:CallModuleFunc("GetSpiritData")
		self:__OpenCallback()
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SPIRIT)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SPIRIT,true)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_SPIRIT_HANDBOOK_CELL)
end

function Modules.SpiritHandBookView:__OpenCallback()
	--精灵图鉴
	local panel_handbookscrollview = self:GetWidget("Panel_handBookScrollView")
	local params = {
		item_class = Modules.TemplSpiritHandBookCell,
	    item_width = 555,
	    item_height	= 440,
	    row = 1,
	    col	= 2,
	    horizon = true,
	    view_width = Macros.Global.DesignSize.width,
	    view_height = 440,
	}
	self.__handbook_scrollview = GUI.ScrollView.New(params)
	panel_handbookscrollview:AddChild(self.__handbook_scrollview:GetNode())
	PosTool.Center(self.__handbook_scrollview)
	self:__CreateHandBookData()
	self.__handbook_scrollview:SetDataList(self.__handbook_data)
	--属性背景
	self.__image_totalhandbookproperty = self:GetWidget("Image_totalHandBookProperty")
	--初始化总属性
	self:__InitTotalProperty()
	--默认没打开
	self.__is_open = false
	--弹出总属性框
	WidgetUnity.SetWidgetListener(self.__image_totalhandbookproperty, nil, nil,function ()
		self.__image_totalhandbookproperty:SetTouchEnabled(false)
		local str = self.__is_open and "animation_close" or "animation_open"
		self:PlayActionTimeline(str,false,function ()
			self.__is_open = not self.__is_open
			self.__image_totalhandbookproperty:SetTouchEnabled(true)
		end)
	end)
end

--构造图鉴数据
function Modules.SpiritHandBookView:__CreateHandBookData()
	self.__handbook_data = {}
	for handbook_index,handbook_info in ipairs(CPPGameLib.GetConfig("SpiritPokedex", nil, nil , true )) do
		local data = {}
		data.handbook_index = handbook_index
		data.pokedex_list = handbook_info.pokedex_list[1].spirit_list
		table.insert(self.__handbook_data,data)
	end
end

--初始化总属性
function Modules.SpiritHandBookView:__InitTotalProperty()	
	if #self.__spirit_data.pokedex_list > 0 then
		local total_property = {}
		for _,pokedex_info in ipairs(self.__spirit_data.pokedex_list) do
			local pokedex_data = CPPGameLib.GetConfig("SpiritPokedex",pokedex_info.id)
			local pro_type = pokedex_data.pokedex_list[pokedex_info.level].pro_type
			total_property[pro_type] = total_property[pro_type] or 0
			total_property[pro_type] = total_property[pro_type] +  pokedex_data.pokedex_list[pokedex_info.level].pro_value
		end
		--属性
		local height = 280
		local num_count = 0
		for pro_type,pro_value in pairs(total_property) do
			num_count = num_count + 1
			local pre_pos = num_count%2 == 1 and 46 or 266
			local back_pos = num_count%2 == 1 and 156 or 376
			local type_str,value_str = Util:Nature(pro_type,pro_value, true, true)
			type_str = type_str..CPPGameLib.GetString("common_colon")
			--属性
			local type_txt = Util:Label(type_str, Macros.TypefaceSize.largish, Macros.Color.Special)
			type_txt:SetAnchorPoint(0,0.5)
			self.__image_totalhandbookproperty:AddChild(type_txt)
			type_txt:SetPosition(pre_pos,height)
			--数值
			local value_txt = Util:Label(value_str, Macros.TypefaceSize.largish, Macros.Color.btn)
			value_txt:SetAnchorPoint(0,0.5)
			self.__image_totalhandbookproperty:AddChild(value_txt)
			value_txt:SetPosition(back_pos,height)
			height = height - 36
		end
	else
		local tips = Util:Label(CPPGameLib.GetString("spirit_lightup_addproperty_tips"), Macros.TypefaceSize.largish, Macros.Color.Special)
		self.__image_totalhandbookproperty:AddChild(tips)
		tips:SetPosition(251,280)
	end	
end

function Modules.SpiritHandBookView:__Dispose()
	if self.__handbook_scrollview then
		self.__handbook_scrollview:DeleteMe()
		self.__handbook_scrollview = nil
	end
end