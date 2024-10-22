--
-- @brief 组队副本结算 
-- @author: yjg
-- @date: 2017年11月10日11:30:46
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplTeamTranscriptCount = LogicGUI.TemplTeamTranscriptCount or BaseClass(GUI.Template)

function LogicGUI.TemplTeamTranscriptCount:__init( func )
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}	
    self.__open_callback = function()
    	self.func = func
        self:__Getwidget()
        self:__ShowUI(data)
    end
	-- self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TEAM)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_TEAM_TRANSCRIPT_COUNT)
end

function LogicGUI.TemplTeamTranscriptCount:__delete()
	if self.func then
		self.func()
	end
end

function LogicGUI.TemplTeamTranscriptCount:__Getwidget( ... )
    -- 触摸层
    self.__root_widget:SetTouchEnabled(true)
    --假装是个定时器
    CPPActionManager.DelayTo( self.__root_widget , 3 ,GlobalCallbackMgr:AddCallBackFunc( function ( ... )
		WidgetUnity.SetWidgetListener(self.__root_widget , nil , nil, function ( ... )
			self:DeleteMe()		
			CallModuleFunc(ModuleType.SCENE, "EnterSceneReq", Macros.Game.SceneType.TEAM_HALL, 1)
		end)
    end ) )

	--队员容器
	self.Panel_troops = self:GetWidget("Panel_troops") 
	--队员条目
	self.Image_player = {}
	--队员头像
	self.Panel_icon = {}
	--队员奖励
	self.Panel_pack = {}
	--队员额外
	self.Panel_extra = {}	
	for i = 1 , 4 do
		self.Image_player[i] = self:GetWidget("Image_player_"..i) 
		self.Panel_icon[i] = self:GetWidget("Panel_icon_"..i) 
		self.Panel_pack[i] = self:GetWidget("Panel_pack_"..i) 
		self.Panel_extra[i] = self:GetWidget("Panel_extra_"..i) 
		self.Image_player[i]:SetVisible(false)
	end

	--模型
	self.Panel_monster = self:GetWidget("Panel_monster") 
	--标题
	self.Panel_headline = self:GetWidget("Panel_headline")
end

function LogicGUI.TemplTeamTranscriptCount:__ShowUI()
	local info = CallModuleFunc(ModuleType.TEAM, "GetBossRewardData")
	if info then
		self.UI_zudui_bossjiesuan , effect_root = self:__CreateEffect({res_name = "UI_zudui_bossjiesuan" } , self.Panel_monster )
		PosTool.Center(effect_root , 0 , 50 )

		--怪物模型
		local dungeonTaskData = CallModuleFunc(ModuleType.TEAM, "GetTeamDungeonTaskData")
		--任务信息
		local taskLink = CPPGameLib.GetConfig("TeamDungeonTaskLink", dungeonTaskData.task_link_id )
		--关卡信息
		local levelCnf = CPPGameLib.GetConfig("GameLevel",taskLink.boss_map_id)
		--关卡信息
		local monsterCnf = CPPGameLib.GetConfig("Monster", levelCnf.icon_id )
		--关卡模型
		local __hero_module = LogicGUI.Model.New({	
													info_id = levelCnf.icon_id, 
													parent = self.Panel_monster , 
													model_type = LogicGUI.ModelType.MONSTER ,
													dir = MathTool.HORZ_RIGHT
												})
		if not taskLink.die_position then
			PosTool.Center( __hero_module , 0 , -140 )
		else
			PosTool.Center( __hero_module , 0 + taskLink.die_position[1] , -140 + taskLink.die_position[2] )
		end
		__hero_module:SetScaleFactor( monsterCnf.show_scale )

		--成员信息
		local memberList = CallModuleFunc(ModuleType.TEAM,"GetTeamDungeonTeamMemberList")
		local t1 = {}
		for i , v in ipairs( memberList ) do
			t1[v.role_id] = v
		end

		for i , v in ipairs( info.lst ) do
			self.Image_player[i]:SetVisible(true)

			--角色头像
			local ownIcon = LogicGUI.RoleIcon.New(false)
			self.Panel_icon[i]:AddChild(ownIcon:GetNode())
			PosTool.Center(ownIcon)
			ownIcon:SetData( t1[v.role_id].head_icon )

			local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID")
			if role_id == v.role_id then
				self.Image_player[i]:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_clearing_my" ) )
			end

			--角色名
			local name = Util:Label( "s" .. t1[v.role_id].server_id .."." .. t1[v.role_id].role_name , Macros.TypefaceSize.slightly , Macros.Color.white )
			name:SetAnchorPoint( 0 , 0 )
			name:SetPosition( 5 , self.Panel_pack[i]:GetContentHeight() + 10 )
			self.Panel_pack[i]:AddChild( name )

			--伤害
			local team_harm = Util:Label( CPPGameLib.GetString("team_harm") .. v.attack_damage , Macros.TypefaceSize.slightly , Macros.Color.white )
			team_harm:SetAnchorPoint( 0 , 0 )
			team_harm:SetPosition( self.Panel_pack[i]:GetContentWidth()/2  , self.Panel_pack[i]:GetContentHeight() + 10 )
			self.Panel_pack[i]:AddChild( team_harm )

			--通常奖励
			for ii , vv in ipairs( v.reward_list ) do
				local __icon = LogicGUI.IconLayer.New()
				self.Panel_pack[i]:AddChild(__icon:GetNode())
				__icon:SetData( {id = vv.Item_Id , num = vv.Item_Num } )
				__icon:SetAnchorPoint( 0 , 0.5 )
				__icon:SetPosition( 5 + ( (ii-1) * 65 )  , self.Panel_pack[i]:GetContentHeight()/2 )
				__icon:SetScaleFactor(0.7)
			end
			if #v.reward_list <= 0 then
				--伤害
				local team_transcript_notAward = Util:Label( CPPGameLib.GetString("team_transcript_notAward") , Macros.TypefaceSize.slightly , Macros.Color.white )
				team_transcript_notAward:SetAnchorPoint( 0.5 , 0.5 )
				team_transcript_notAward:SetPosition( self.Panel_pack[i]:GetContentWidth()/2  , self.Panel_pack[i]:GetContentHeight()/2 )
				self.Panel_pack[i]:AddChild( team_transcript_notAward )
			end

			--额外奖励——mvp
			if v.extra.mvp and v.extra.mvp[1] then
				local data = v.extra.mvp[1]

				local __icon = LogicGUI.IconLayer.New()
				self.Panel_extra[i]:AddChild(__icon:GetNode())
				__icon:SetData( {id = data.item_id , num = data.item_num } )
				__icon:SetAnchorPoint( 0 , 0.5 )
				__icon:SetPosition( 5  , self.Panel_extra[i]:GetContentHeight()/2 )
				__icon:SetScaleFactor(0.7)

				local mvp_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "mvp" , true )
				local mvp = Util:Sprite( mvp_sp )
				mvp:SetAnchorPoint( 0.5 , 0 )
				mvp:SetPosition( __icon:GetNode():GetPositionX() + ( (__icon:GetNode():GetContentWidth() * 0.7) /2) , self.Panel_extra[i]:GetContentHeight() + 10 )
				self.Panel_extra[i]:AddChild(mvp)

		        local effect_id , effect = self:__CreateEffect({res_name = "UI_zudui_mvp"}, __icon )
		        PosTool.Center( effect )
			end

			--额外奖励——击杀
			if v.extra.kill and v.extra.kill[1] then
				local data = v.extra.kill[1]

				local __icon = LogicGUI.IconLayer.New()
				self.Panel_extra[i]:AddChild(__icon:GetNode())
				__icon:SetData( {id = data.item_id , num = data.item_num } )
				__icon:SetAnchorPoint( 0 , 0.5 )
				__icon:SetPosition( 5 + 65 , self.Panel_extra[i]:GetContentHeight()/2 )
				__icon:SetScaleFactor(0.7)

				local kill_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "kill" , true )
				local kill = Util:Sprite( kill_sp )
				kill:SetAnchorPoint( 0.5 , 0 )
				kill:SetPosition( __icon:GetNode():GetPositionX() + ( (__icon:GetNode():GetContentWidth() * 0.7) /2) , self.Panel_extra[i]:GetContentHeight() + 10 )
				self.Panel_extra[i]:AddChild(kill)

		        local effect_id , effect = self:__CreateEffect({id = 100085}, __icon )
		        PosTool.Center( effect )


				__hero_module:SetGray(true)
				__hero_module:SetToPlayTime(0)
				local kill_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "killed_icon" , true )
				local kill = Util:Sprite( kill_sp )
				kill:SetAnchorPoint( 0.5 , 0.5 )
				self.Panel_monster:AddChild( kill )
				PosTool.Center(kill)
			
			end
		end

		self:PlayActionTimeline("animation0" , false , nil)	
	end

end
