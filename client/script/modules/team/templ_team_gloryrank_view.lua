LogicGUI = LogicGUI or {}

LogicGUI.TemplTeamGloryRankView = LogicGUI.TemplTeamGloryRankView or BaseClass(GUI.Template)

function LogicGUI.TemplTeamGloryRankView:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
		
	self.__open_callback = function ()
		self:InitBaseUI()
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TEAM_GLORY_RANK)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TEAM_GLORY_RANK,true)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_TEAM_GLORY_RANK_VIEW)
end

function LogicGUI.TemplTeamGloryRankView:__delete()
	for i=1,3 do
		if self["__hero_model"..i] then
			self["__hero_model"..i]:DeleteMe()
			self["__hero_model"..i] = nil
		end
	end
end

function LogicGUI.TemplTeamGloryRankView:__RegistAllEvents()
    --阵容红点
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_PASS_RECORD_DATA, function(_,protocol_data)
        self.__dungeon_pass_record_data[self.__selected_tab_index] = protocol_data.rank_list
        self:ShowRankMessage()
    end)

    --匹配成功关闭界面
    self:BindGlobalEvent(Macros.Event.TeamModule.BREAK_THE_TEAM, function(_,is_match)
        if is_match then
        	self:DeleteMe()
        end
    end)
end

function LogicGUI.TemplTeamGloryRankView:InitBaseUI()
	--标签
	local tab_sp1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM_GLORY_RANK, "unselected_tag")
	local tab_sp2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM_GLORY_RANK, "selected_tag")

	local dungeon_name = {} 
	for i,dungeon_config in ipairs(CPPGameLib.GetConfig("TeamFBChapter",nil,nil,true)) do
		table.insert(dungeon_name,dungeon_config.dungeon_name)
	end
	self.__tab_params = {
		{tab_sp1, tab_sp2},
		dungeon_name, 
        10, 
        CPPGameLib.Handler(self, self.SelectedTab),
        false
    }
    self.__tab = GUI.FeignCheckBoxBtn.New(unpack(self.__tab_params))
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"", function ()
		self:DeleteMe()
	end)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_Title"),CPPGameLib.GetString("team_glory_rank"), Macros.TypefaceSize.tab, Macros.Color.btn)
	--初始化各个位置
	for i=1,3 do
		self["__panel_pos"..i] = self:GetWidget("Panel_pos"..i)
		--模型容器
		self["__panel_teamleadermodule"..i] = WidgetUnity.GetWidgetByName(self["__panel_pos"..i], "Panel_TeamLeaderModule")
		--通关时长
		self["__text_costtime"..i] = WidgetUnity.GetWidgetByName(self["__panel_pos"..i], "Text_CostTime")
		Util:WidgetLabel(self["__text_costtime"..i],"", Macros.TypefaceSize.normal, Macros.Color.golden)
		--队员名字
		for k=1,4 do
			--队员名字
			self["__text_teammembername"..i..k] = WidgetUnity.GetWidgetByName(self["__panel_pos"..i], "Text_TeamMemberName"..k)
			Util:WidgetLabel(self["__text_teammembername"..i..k],"", Macros.TypefaceSize.normal, Macros.Color.Special)			
		end
	end
	self.__dungeon_pass_record_data = {}
	--选项卡
	local panel_tab = self:GetWidget("Panel_tab")
	panel_tab:AddChild(self.__tab:GetNode())
	--跳转初始页面
	self.__tab:On(1)
end

function LogicGUI.TemplTeamGloryRankView:SelectedTab(tab_index)	
	self.__selected_tab_index = tab_index
	--设置字体颜色
	for i=1,#self.__tab.btn_list do
		self.__tab.btn_list[i]:SetTextColor(unpack(i == tab_index and Macros.Color.headline or Macros.Color.btn))
		self.__tab.btn_list[i]:SetOutline(0, 0, 0, 0, 2)
	end	
	if not self.__dungeon_pass_record_data[tab_index] then
		local dungeon_config = CPPGameLib.GetConfig("TeamFBChapter",tab_index)
		CallModuleFunc(ModuleType.TEAM,"TeamDungeonRankReq",dungeon_config.dungeon_id)
	else
		self:ShowRankMessage()
	end
end

--设置排行信息
function LogicGUI.TemplTeamGloryRankView:ShowRankMessage()	
	local message_data = self.__dungeon_pass_record_data[self.__selected_tab_index]
	--前3名信息
	for i=1,3 do
		self["__panel_teamleadermodule"..i]:SetVisible(message_data[i] and message_data[i].captain_head_img_id and true or false)
		if message_data[i] and message_data[i].captain_head_img_id and not self["__hero_model"..i] then
			self["__hero_model"..i] = LogicGUI.Model.New({parent = self["__panel_teamleadermodule"..i],info_id = message_data[i].captain_head_img_id == 0 and 30998 or message_data[i].captain_head_img_id})
		elseif message_data[i] and message_data[i].captain_head_img_id and self["__hero_model"..i] then
			self["__hero_model"..i]:SetInfoID(message_data[i].captain_head_img_id == 0 and 30998 or message_data[i].captain_head_img_id)	
		end
		if message_data[i] and message_data[i].captain_head_img_id then
			local hero_config = CPPGameLib.GetConfig("Hero",message_data[i].captain_head_img_id == 0 and 30998 or message_data[i].captain_head_img_id)
			self["__hero_model"..i]:SetPosition(50,0)
			self["__hero_model"..i]:SetScaleFactor(hero_config.ui_scale or 1)
			if self["__waiting_for_someone"..i] then
				self["__waiting_for_someone"..i]:SetVisible(false)
			end
		else
			if not self["__waiting_for_someone"..i] then
				local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_chasm_copy",true)
				self["__waiting_for_someone"..i] = Util:Sprite(sp)
				self["__waiting_for_someone"..i]:SetAnchorPoint(0.5,1)
				self["__panel_pos"..i]:AddChild(self["__waiting_for_someone"..i])
				PosTool.CenterTop(self["__waiting_for_someone"..i],0,30)
			end
			self["__waiting_for_someone"..i]:SetVisible(true)
		end
		--通关时长
		local time_str = message_data[i] and message_data[i].cost_time and TimeTool.TransTimeStamp("MM:SS",message_data[i].cost_time)
		self["__text_costtime"..i]:SetText(message_data[i] and message_data[i].cost_time and CPPGameLib.GetString("team_glory_cost_time", time_str) or "")
		--队员名字
		for k=1,4 do
			--队员名字
			self["__text_teammembername"..i..k]:SetText(message_data[i] and message_data[i].role_name_list[k] and message_data[i].role_name_list[k].server_id and message_data[i].role_name_list[k].role_name
			 and CPPGameLib.GetString("team_player_name",message_data[i].role_name_list[k].server_id,message_data[i].role_name_list[k].role_name) or "")		
		end
	end	
end
