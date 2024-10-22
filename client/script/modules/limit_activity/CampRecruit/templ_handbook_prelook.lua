
Modules = Modules or {}

--阵营招募预览模板
Modules.TemplCampRecruitPreLook = Modules.TemplCampRecruitPreLook or BaseClass(GUI.Template)

function Modules.TemplCampRecruitPreLook:__init(cur_data_id,camp)	
	self.__bg_params = {type = BaseView.BGType.SOLID}	

	self.__is_auto_load = true
	self.__open_callback = function()
							
		--关卡留言标题
		Util:WidgetLabel(self:GetWidget("Text_Title"), CPPGameLib.GetString("camp_recruit_prelook_title"), Macros.TypefaceSize.popup, Macros.Color.btn)		
		
	    --关闭按钮
		Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
			self:DeleteMe()
		end)

		self.__cur_data_id = cur_data_id

	    self.__panel_tab = self:GetWidget("Panel_Tab")
	    self.__panel_recordscrollview = self:GetWidget("Panel_ScrollView")
	    --左边列表
	    local btn_res = {"btn_btn_biggreen","btn_btn_bigblue"}
	    self.__btn_res = {}
	    for _,btn_res_name in ipairs(btn_res) do
	        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW,btn_res_name)
	        table.insert(self.__btn_res,sp)
	    end 
	    --初始化
	    local left_namelist = {}
	    local left_iconlist = {}
	    for i=1,3 do
	        local camp_name = CPPGameLib.GetString("hero_handbook_buttonType_"..(i-1))
	        table.insert(left_namelist,camp_name)
	        local camp_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"camp_icon_"..(i-1),true)
	        table.insert(left_iconlist,camp_sp)
	    end 
	    self.__btn_list = {}
	    local offset_y = 0
	    local interval = 20
	    for i,name_str in ipairs(left_namelist) do
	        local sp_data = {}
	        local btn = Util:Button(self.__btn_res[2],function ()
	            self:SetSelectedCallBack(i)
	        end)
	        btn:SetAnchorPoint(0,1)
	        self.__panel_tab:AddChild(btn)
	        PosTool.LeftTop(btn,0,offset_y)
	        sp_data.btn = btn
	        table.insert(self.__btn_list,sp_data)
	        offset_y = offset_y - btn:GetContentHeight() - interval
	        --图标
	        local icon = Util:Sprite9(left_iconlist[i])
	        icon:SetAnchorPoint(0,0.5)
	        btn:AddChild(icon)
	        PosTool.LeftCenter(icon,5,3)
	        icon:SetScaleFactor(0.9)
	        --文字
	        sp_data.label = Util:Label(name_str, Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.button_yellow)
	        sp_data.label:SetAnchorPoint(0,0.5)
	        btn:AddChild(sp_data.label)
	        PosTool.LeftCenter(sp_data.label,77,0)
	    end
	    self:SetSelectedCallBack(camp)
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEADICON)	
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_HANDBOOK)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CHALLENGE,true)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_CAMPRECRUIT_PRELOOK, parent_node)	
end

function Modules.TemplCampRecruitPreLook:__delete()
	if self.__record_scrollview then
		self.__record_scrollview:DeleteMe()
		self.__record_scrollview = nil
	end
end

function Modules.TemplCampRecruitPreLook:SetSelectedCallBack(index)
    if self.__selected_index and self.__selected_index == index then
        return
    end
    local config_camp_recruit = CPPGameLib.GetConfig("CampRecruit",self.__cur_data_id)
    local config_camp_recruit_herolist = CPPGameLib.GetConfig("CampRecruitHeroList",self.__cur_data_id)
    if not config_camp_recruit_herolist then
        return
    end
    self.__selected_index = index
    for i,sp_data in ipairs(self.__btn_list) do
        sp_data.btn:SetNormalImage(index == i and self.__btn_res[1] or self.__btn_res[2])
    end
    local camp_data_list = {}
    for _,camp_info in pairs(config_camp_recruit_herolist[index - 1]) do
    	if camp_info and type(camp_info) ~= 'number' and camp_info.reward_id then
    		local cur_camp_data = CPPGameLib.CopyTblShallow(camp_info)
    		cur_camp_data.is_hot_hero = camp_info.reward_id == config_camp_recruit.recruit_data_list[index - 1].hot_hero
    		table.insert(camp_data_list,cur_camp_data)
    	end
    end
    table.sort(camp_data_list,function (data1,data2)
    	return data1.sort_id < data2.sort_id
    end)
    --滚动容器容器
    local panel_scrollview = self:GetWidget("Panel_ScrollView") 
	if not self.__record_scrollview then
        local params = {
            item_class = Modules.TemplCampRecruitHandBookHeroIcon,
            item_width = 120,
            item_height = 150,
            row = 3,
            col = 4,
            item_space_r = 15,
            item_space_c = 15,
            view_height = 433,
        }
        self.__record_scrollview = GUI.ScrollView.New(params)
        self.__panel_recordscrollview:AddChild(self.__record_scrollview:GetNode())
        PosTool.Center(self.__record_scrollview)
	end
	self.__record_scrollview:SetDataList(camp_data_list)
end