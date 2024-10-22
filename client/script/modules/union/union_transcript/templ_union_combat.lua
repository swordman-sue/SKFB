
--
-- @brief 进入战斗
-- @author: yjg
-- @date: 2017年5月25日17:21:50
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionCombat = LogicGUI.TemplUnionCombat or BaseClass(GUI.Template)

function LogicGUI.TemplUnionCombat:__init( date )
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__render_queue = Scene.RenderQueue.GRQ_UI	
	self.__is_auto_load = true
	self.__open_callback = function()
		self.date = date
		self:__Getwidget()
		self:__ShowUI( date )
	end

    --数据刷新
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_FB , function (_, data)
        self.union_challengeTime[2]:SetString( CallModuleFunc(ModuleType.UNION , "GetNowChallengeTime" ) )
    end)
    
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_UNION_COMPILE)	
end

function LogicGUI.TemplUnionCombat:__Getwidget()
	--居中层
	self.Image_1 = self:GetWidget("Image_1")
	--进度条
	self.Panel_bar = self:GetWidget("Panel_bar")
	--挑战按钮
	self.Button_challenge =  Util:WidgetButton( 
													self:GetWidget("Button_challenge") , 
													CPPGameLib.GetString("challenge") , 
													CPPGameLib.Handler(self,self.OnButtonChallenge) ,
													Macros.TypefaceSize.largish,nil,Macros.Color.button_red 
												)
	--挑战次数
	self.Text_challengeTime = self:GetWidget("Text_challengeTime")
	--内容框
	self.Image_content = self:GetWidget("Image_content")
	--击杀奖励
	local Text_kill = Util:WidgetLabel(
										self:GetWidget("Text_kill") , 
										CPPGameLib.GetString("union_killAward"),
										Macros.TypefaceSize.normal,
										Macros.Color.keypoint
										)
	--击杀奖励
	self.Text_kill_award = self:GetWidget("Text_kill_award")
	--挑战奖励
	local Text_challenge = Util:WidgetLabel(
										self:GetWidget("Text_challenge") , 
										CPPGameLib.GetString("union_challengeAward"),
										Macros.TypefaceSize.normal,
										Macros.Color.keypoint
										)
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")
	--名字（贡献
	self.Text_contribute = self:GetWidget("Text_contribute")
	--数量
	self.Text_contribute_num = self:GetWidget("Text_contribute_num")
	--提示
	self.Text_contribute_tips = self:GetWidget("Text_contribute_tips")
	--模型
	self.Panel_mode = self:GetWidget("Panel_mode")
	--名字
	self.Panel_name = self:GetWidget("Panel_name")
	--提示
	self.Panel_tips = self:GetWidget("Panel_tips")
	--关闭
	self.Button_close = Util:WidgetButton( 
											self:GetWidget("Button_close") ,
											"" , 
											CPPGameLib.Handler(self,self.OnBtnClose) 
										)
end

function LogicGUI.TemplUnionCombat:__ShowUI( date )

    local levelCnf = CPPGameLib.GetConfig("GameLevel" , date.level )
    local monsterCnf = CPPGameLib.GetConfig("Monster" , levelCnf.icon_id )

    --怪物地光
   	local bg_shadow = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_shadow"))
	bg_shadow:SetAnchorPoint( 0.5 , 0 )
	bg_shadow:SetPosition( self.Panel_mode:GetContentWidth()/2 , 0 )
	self.Panel_mode:AddChild(bg_shadow)

	--模型
	local monster = LogicGUI.Model.New({info_id = levelCnf.icon_id , model_type = LogicGUI.ModelType.MONSTER , parent = bg_shadow })
	monster:SetPosition( bg_shadow:GetContentWidth()/2  , bg_shadow:GetContentHeight()/2 )

	--名字
    local tab = {
        { tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "camp_icon_"..date.type , true ) , dx = 0.6 },
        { zi = monsterCnf.name , dx = Macros.TypefaceSize.normal , jg = 10 , mb = true , ys = Macros.Game.QualityType.PURPLE },
    }
    local name , nameLst = Util:LabCenter( tab )
    name:SetAnchorPoint( 0 , 0.5 )
    name:SetPosition( 0 , self.Panel_name:GetContentHeight()/2 )
    self.Panel_name:AddChild( name )

    nameLst[1]:SetPosition( nameLst[1]:GetPositionX() , nameLst[1]:GetPositionY() + 5 )

    local icon_upgrade_tips = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_upgrade_tips" )
    local icon_upgrade_tips_ = Util:Sprite( icon_upgrade_tips )
    icon_upgrade_tips_:SetAnchorPoint( 0.5 , 0 )
    icon_upgrade_tips_:SetPosition( nameLst[1]:GetContentWidth() - 7 , 1 )
    nameLst[1]:AddChild( icon_upgrade_tips_ )
    icon_upgrade_tips_:SetScaleFactor( 1 )

	local campCnf = CPPGameLib.GetConfig("Camp", date.type )

	local text = CPPGameLib.GetString("union_transcriptTips" , CPPGameLib.GetString("hero_camp_"..date.contrapose) , CPPGameLib.GetString("nature"..campCnf.guild_fb_damage_add[1][1]) , ( campCnf.guild_fb_damage_add[1][2] / 10000 * 100 ) .. "%" )

    --描述
 	local describe = Util:Label( text , Macros.TypefaceSize.normal , Macros.Color.keypoint , self.Panel_bar:GetContentWidth() , 0 )
 	describe:SetAnchorPoint( 0 , 1 )
 	describe:SetPosition( 0 , self.Panel_tips:GetContentHeight() )
 	self.Panel_tips:AddChild( describe )

 	--进度条
    local progressBar_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_little_preview_bg")
    local progressBar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_preview")
    local __progressbar = GUI.ProgressBar9Layer.New( progressBar , progressBar_bg , self.Panel_bar:GetContentWidth() - 3 , self.Panel_bar:GetContentHeight() - 3 , self.Panel_bar:GetContentWidth() , self.Panel_bar:GetContentHeight() ) 
    __progressbar:SetAnchorPoint(0.5,0.5)
    self.Panel_bar:AddChild( __progressbar:GetNode())
    PosTool.Center( __progressbar )
    __progressbar:SetProgressMax( date.max )
    __progressbar:SetProgress( date.hp )

    --血量
    local hp = Util:Name( date.hp.."/"..date.max )
    hp:SetAnchorPoint(0.5,0.5)
    self.Panel_bar:AddChild( hp )
    PosTool.Center( hp , 0 , -2 )

    --挑战次数
    local tab = {
        { zi = CPPGameLib.GetString("union_challengeTime") , dx = Macros.TypefaceSize.normal , jg = 5 , ys = Macros.Color.keypoint },
        { zi = CallModuleFunc(ModuleType.UNION , "GetNowChallengeTime" ) , dx = Macros.TypefaceSize.normal , jg = 5 , ys = Macros.Color.content },
    }
    local union_challengeTime  = nil
    union_challengeTime , self.union_challengeTime = Util:LabCenter( tab )
    union_challengeTime:SetAnchorPoint( 0 , 0.5 )
    union_challengeTime:SetPosition( 0 , self.Text_challengeTime:GetContentHeight()/2 )
    self.Text_challengeTime:AddChild( union_challengeTime )

    -- 击杀奖励
	local section = CallModuleFunc(ModuleType.UNION, "GetNowAttackSection")	
	local fbcnf = CPPGameLib.GetConfig("GuildFB", section )

	local union_contribute = Util:Label( CPPGameLib.GetString("union_contribute" , fbcnf.kill_exp ) , Macros.TypefaceSize.normal , Macros.Color.content )
	union_contribute:SetAnchorPoint( 0 , 0.5 )
	union_contribute:SetPosition( 0 , self.Text_kill_award:GetContentHeight()/2 )
	self.Text_kill_award:AddChild( union_contribute )

	
	local huobiCnf = CPPGameLib.GetConfig("HuoBi", Macros.Game.RolePropType.GUILD_CONTRIBUTION )
	--图标
    local icon = LogicGUI.IconLayer.New(false)
    local iconData = { id = huobiCnf.big_icon }
    icon:SetData(iconData)
    self.Panel_icon:AddChild(icon:GetNode())
    PosTool.Center( icon:GetNode() )

    local type_ , info_ = ObjIDTool.GetInfo(huobiCnf.big_icon)
    --军团贡献
	local battle_contribution = Util:Label( info_.name , Macros.TypefaceSize.normal , Macros.Color.content  )
	battle_contribution:SetAnchorPoint( 0 , 0.5 )
	battle_contribution:SetPosition( 0 , self.Text_contribute:GetContentHeight()/2 )
	self.Text_contribute:AddChild( battle_contribution )

	--数量
	local tab = {
					{ zi = CPPGameLib.GetString("union_num") , ys = Macros.Color.keypoint, dx = Macros.TypefaceSize.normal },
					{ zi = fbcnf.battle_contribution , ys = Macros.Color.content, dx = Macros.TypefaceSize.normal },
				}
	local union_num = Util:LabCenter(tab)
	union_num:SetAnchorPoint( 0 , 0.5 )
	union_num:SetPosition( 0 , self.Text_contribute_num:GetContentHeight()/2 )
	self.Text_contribute_num:AddChild( union_num )

	--提示
	local union_whoSmoteTips = Util:Label( CPPGameLib.GetString("union_whoSmoteTips" , fbcnf.kill_contribution ) , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	union_whoSmoteTips:SetAnchorPoint( 0 , 0.5 )
	union_whoSmoteTips:SetPosition( 0 , self.Text_contribute_tips:GetContentHeight()/2 )
	self.Text_contribute_tips:AddChild( union_whoSmoteTips )
end

function LogicGUI.TemplUnionCombat:OnButtonChallenge()
	local checkBattle = CallModuleFunc(ModuleType.UNION , "CheckBattleCondition" , Macros.Game.FBType.UNION )
	if checkBattle then
		CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.UNION, self.date.type )
	else
		--剩余次数
		local time_ = CallModuleFunc(ModuleType.UNION, "GetUnionTranscriptBuyTime")
		if time_ ~= 0 then
			LogicGUI.TemplUnionBuytime.New()
		else
	        local tips = LogicGUI.TemplAffirmTips.New( { 
	                                                        type = LogicGUI.TemplAffirmTipsType.YES_NO,
	                                                        content = CPPGameLib.GetString("union_surplus") ,
	                                                        cancel_text = CPPGameLib.GetString("vip_Isee") ,
	                                                        confirm_text = CPPGameLib.GetString("vip_Toimprove") ,
	                                                        confirm_func = function ( ... )
	                                                            CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE )
	                                                            self:OnBtnClose()
	                                                        end,
	                                                        cancel_func = function ( ... )
	                                                        end
	                                                    } )
		end
	end
end

function LogicGUI.TemplUnionCombat:OnBtnClose()
	self:DeleteMe()
end