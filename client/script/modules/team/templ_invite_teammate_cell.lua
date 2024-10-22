--[[
	邀请队友信息条目
]]
Modules = Modules or {}

Modules.TemplInviteTeamMateCell = Modules.TemplInviteTeamMateCell or BaseClass(GUI.ScrollItem)

function Modules.TemplInviteTeamMateCell:__init(data)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_INVITE_TEAM_TEAMMATE_CELL)
end

function Modules.TemplInviteTeamMateCell:SetData(data)   
    GUI.ScrollItem.SetData(self, data)
    --背景
    self:GetWidget("Image_bg"):SetVisible(self.__data.index%2 == 1)
    --角色名字
    local text_playername = self:GetWidget("Text_PlayerName")
    --好友不用显示区号
    if self.__data.is_friend then
       Util:WidgetLabel(text_playername,self.__data.role_name or "", Macros.TypefaceSize.largish, Macros.Color.keypoint)
    else
       Util:WidgetLabel(text_playername,CPPGameLib.GetString("team_player_name",self.__data.server_id or 0,self.__data.role_name or ""), Macros.TypefaceSize.largish, Macros.Color.keypoint)
    end  
    --战力
    local panel_battlevalue = self:GetWidget("Panel_BattleValue")
    self.__power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "maincity_battle_num", true))
    self.__power_font:SetAnchorPoint(0, 0.5)
    panel_battlevalue:AddChild(self.__power_font:GetNode())
    PosTool.LeftCenter(self.__power_font, 0, 0)

    local battle_value = self.__data.is_friend and self.__data.battle_value or self.__data.power
    if battle_value >= 1000000 then
    	battle_value = math.floor( battle_value/10000 )
    	--万
    	self.__myriad_font = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "myriad", true) )
    	panel_battlevalue:AddChild(self.__myriad_font)
    end
    self.__power_font:SetText(tostring(battle_value))
   	PosTool.RightTo( self.__power_font , self.__myriad_font )

    --副本配置信息
    local cur_team_id,cur_team_dungeon_id = CallModuleFunc(ModuleType.TEAM,"GetCurTeamIdAndTeamDungeonId")
    if not cur_team_id or not cur_team_dungeon_id then
      return
    end
    local team_dungeon_config = CPPGameLib.GetConfig("TeamFBChapter",cur_team_dungeon_id)

   	--邀请按钮
   	self.__button_invite = self:GetWidget("Button_Invite")
    --按钮
    if not self.__data.invite_status then
      local yellow_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
      self.__button_invite:SetNormalImage(yellow_sp)
    end
   	Util:WidgetButton(self.__button_invite, self.__data.invite_status and CPPGameLib.GetString("team_invite_btn_on_invite") or CPPGameLib.GetString("team_invite_btn_invite"),function ()
     		--请求邀请队友
        CallModuleFunc(ModuleType.TEAM,"TeamDungeonInviteReq",{target_id = self.__data.role_id})
        --重新设置按钮状态
        self.__button_invite:SetTouchEnabled(false)
        self.__data.invite_status = true
        local blue_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_blue")
        self.__button_invite:SetNormalImage(blue_sp)
        self.__button_invite:SetText(CPPGameLib.GetString("team_invite_btn_on_invite"))
        local r, g, b, a = unpack(Macros.Color.button_red)
        self.__button_invite:SetOutline(r, g, b, a, 2)
   	end, Macros.TypefaceSize.tab, Macros.Color.btn, self.__data.invite_status and Macros.Color.button_red or Macros.Color.button_yellow)
    --重新设置按钮状态
    self.__button_invite:SetTouchEnabled(not self.__data.invite_status and true or false)

   	--邀请队友状态
   	local text_rolestatus = self:GetWidget("Text_RoleStatus")
    Util:WidgetLabel(text_rolestatus,"", Macros.TypefaceSize.largish, Macros.Color.red)

    --是否好友
    if self.__data.is_friend then
        local role_status_txt = ""
        local is_could_invite = false
        if self.__data.level >= team_dungeon_config.recommend_level[1] then
          if self.__data.offline_time ~= 0 then
            role_status_txt = CPPGameLib.GetString("team_player_offline")
          else
            is_could_invite = true
          end
        else
          role_status_txt = CPPGameLib.GetString("hero_lineup_invalid_level")
        end
        --是否符合等级要求
        self.__button_invite:SetVisible(is_could_invite)
        --条件
        text_rolestatus:SetVisible(not is_could_invite)
        text_rolestatus:SetText(role_status_txt)
    else
      --是否符合等级要求
      self.__button_invite:SetVisible(self.__data.level >= team_dungeon_config.recommend_level[1])
      --条件
      text_rolestatus:SetVisible(self.__data.level < team_dungeon_config.recommend_level[1])
      text_rolestatus:SetText(self.__data.level < team_dungeon_config.recommend_level[1] and CPPGameLib.GetString("hero_lineup_invalid_level") or "")
    end  	
end

