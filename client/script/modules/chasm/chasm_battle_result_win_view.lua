
--[[
%% @module: 深渊战斗
%% @author: yjg
%% @created: 2017年7月18日10:32:37
--]]

Modules = Modules or {}

Modules.ChasmWinView = Modules.ChasmWinView or BaseClass(BaseView)

function Modules.ChasmWinView:__init()
	self.__layout_name = "chasm_battle_result_win"	
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_loading = false

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_COMMON)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_WIN)    
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, true)   
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE, true)    

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHASM)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHASM,true) 
end

function Modules.ChasmWinView:__OpenCallback()
	--血量回复
	self:__Getwidget()
	self:__ShowUI()

    self.__my_close_callback = unpack(self.__open_data)
end

function Modules.ChasmWinView:__Dispose(  )
end

function Modules.ChasmWinView:__Getwidget(  )

	--恢复底框
	self.Image_recover = self:GetWidget( "Image_recover" )
	--恢复字样
	self.Image_recover_1 = self:GetWidget( "Image_recover_1" )
	self.Image_recover_1:IgnoreContentAdaptWithSize(true)
	--奖励底框
	self.Image_award = self:GetWidget( "Image_award" )
	--奖励字样
	self.Image_award_1 = self:GetWidget( "Image_award_1" )
	self.Image_award_1:IgnoreContentAdaptWithSize( true )
	--击杀容器
	self.Panel_kill = self:GetWidget( "Panel_kill" )
	--提示
	self.Panel_tips = self:GetWidget( "Panel_tips" )
	self.Panel_tips:SetVisible( false )
	--终结
	self.Image_finality = self:GetWidget( "Image_finality" )
	self.Image_finality:SetVisible( false )
	--标题
	self.Image_conclude = self:GetWidget( "Image_conclude" )

	--下一波
	self.Button_next = Util:WidgetButton( 
											self:GetWidget("Button_next") , 
											CPPGameLib.GetString("chasm_next") , 
											CPPGameLib.Handler(self,self.OnButtonNext) , 
											Macros.TypefaceSize.largish , nil , 
											Macros.Color.button_red
										)
end

function Modules.ChasmWinView:__ShowUI(  )
	local todayWave = CallModuleFunc(ModuleType.CHASM, "GetCurWave" )

    --通关波束
    local tab = {
                    { tp = Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.CHASM , "sp_chasm_pass1" , true ) },
                    { num = "chasm_pass" , value = todayWave },
                    { tp = Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.CHASM , "sp_chasm_pass2" , true ) },
                }
    local free = Util:LabCenter( tab )
    free:SetAnchorPoint( 0.5 , 0 )
    free:SetPosition( self.Image_conclude:GetContentWidth()/2 , self.Image_conclude:GetContentHeight()/2 - 55 )
    self.Image_conclude:AddChild( free )

	local btnEffect_id , btnEffect_con = self:__CreateEffect({ res_name = "UI_wujinjiesuan2" } , self.Image_conclude )
	btnEffect_con:SetAnchorPoint( 0.5 , 1 )
	btnEffect_con:SetPosition(self.Image_conclude:GetContentWidth()/2 , -170 )

	--当前击杀
	local layou = Util:Layout()
	layou:SetAnchorPoint(0.5,0.5)
	layou:SetPosition(self.Panel_kill:GetContentWidth()/2 ,self.Panel_kill:GetContentHeight()/2 )
	self.Panel_kill:AddChild(layou)

	local tab_1 = {
					{ zi = CPPGameLib.GetString( "chasm_nowKill" ) , ys = Macros.Color.golden , dx = Macros.TypefaceSize.normal },
					{ zi =  CallModuleFunc(ModuleType.CHASM, "GetCurKeepKillNum" ) , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal },
				}
	local now = Util:LabCenter(tab_1)

	local tab_2 = {
					{ zi = CPPGameLib.GetString( "chasm_maxKill" ) , ys = Macros.Color.golden , dx = Macros.TypefaceSize.normal },
					{ zi = CallModuleFunc(ModuleType.CHASM, "GetKeepKillNum" ) , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal },
				}
	local max = Util:LabCenter(tab_2)

	layou:SetContentSize( now:GetContentWidth() + 100 + max:GetContentWidth() , self.Panel_kill:GetContentHeight() )
	local interval = layou:GetContentWidth() / 2

	now:SetPosition( interval/2 , layou:GetContentHeight()/2 )
	layou:AddChild( now )

	max:SetPosition( (layou:GetContentWidth()/2) + interval/2 , layou:GetContentHeight()/2 )
	layou:AddChild( max )

	--恢复
	--英雄头像
	local layou = Util:Layout()
	layou:SetAnchorPoint(0.5,0.5)
	layou:SetPosition(self.Image_recover:GetContentWidth()/2 ,self.Image_recover:GetContentHeight()/2 )
	self.Image_recover:AddChild(layou)

	local now_hero_num = CPPGameLib.GetTableLength(GlobalVOMgr:GetHeroMap())
	layou:SetContentSize(now_hero_num * 130 , 90 )
	local interval = layou:GetContentWidth() / now_hero_num

	--血条
    local imgdata_bar = Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.COMMON_PROGRESS , "prog_bar_bar_green2" )
    local imgdata_bg = Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_bar_bg" )

	local index = 0
	for i , v in pairs( GlobalVOMgr:GetHeroMap() ) do
		index = index + 1
		--头像
		local icon = LogicGUI.IconLayer.New(false)
		icon:SetPosition( 55 + interval * (index - 1)  , layou:GetContentHeight()/2 + 10 )
		-- 2017年9月19日10:03:18
		icon:SetData({ id = v.info_id} )
		icon:SetAnchorPoint(0.5,0.5)
		layou:AddChild(icon:GetNode())

		--血条
        local hp_bar = GUI.ProgressBarLayer.New(imgdata_bar, imgdata_bg , v.hp, v.max_hp)
        hp_bar:SetPosition(icon:GetPositionX() , 5 )
        hp_bar:SetAnchorPoint( 0.5 , 0.5 )
        layou:AddChild( hp_bar:GetNode() )

        local init_wave = CPPGameLib.GetConfig("EndlessLandWave", todayWave )
  --       --字样
		local recover = MathTool.TransProbCoeff2Decimal( init_wave.reward_hp_factor )
  --       local recover_text = Util:Label("+".. (init_wave.reward_hp_factor/100) .. "%"  , Macros.TypefaceSize.normal , Macros.Color.green )
  --       recover_text:SetAnchorPoint( 0 , 0.5 )
  --       recover_text:SetPosition( hp_bar:GetNode():GetPositionX() + 45  , hp_bar:GetNode():GetPositionY() )
  --       layou:AddChild( recover_text )
  		-- 2252 无尽 结算界面这个一直挂着的10%去掉 只留下+10%的特效
		--飘增加的经验
		local lab = TypefaceEffects:LittleFloat( "+".. (init_wave.reward_hp_factor/100) .. "%" , nil , 1.5 , 0 , 20 , nil , 1 , nil , Macros.TypefaceSize.normal )
		lab:SetAnchorPoint( 0 , 0.5 )
		lab:SetPosition( hp_bar:GetNode():GetPositionX() + 45 , hp_bar:GetNode():GetPositionY() )
		layou:AddChild(lab)

        --假装是个定时器
        CPPActionManager.DelayTo( layou , 0.7 ,GlobalCallbackMgr:AddCallBackFunc( function ( ... )
        	local recoverHp = recover * v.max_hp
    		hp_bar:SetToProgress( recoverHp + v.hp )
        end ) )
	end


	local first = CallModuleFunc(ModuleType.CHASM, "GetTodayFirstBattle" )

	--奖励
	local layou = Util:Layout()
	layou:SetAnchorPoint(0.5,0.5)
	layou:SetPosition(self.Image_award:GetContentWidth()/2 ,self.Image_award:GetContentHeight()/2 )
	self.Image_award:AddChild(layou)

	local historyWaveRecord = CallModuleFunc(ModuleType.CHASM, "GetRewardWave" )

	historyWaveRecord = 1
	if historyWaveRecord ~= 0 then
		self.Image_award:SetVisible(true)
		self.Image_recover:SetPosition( self.Image_recover:GetPositionX() , 318.98 )
	else
		self.Image_award:SetVisible(false)
		self.Image_recover:SetPosition( self.Image_recover:GetPositionX() , 239.26 )
	end

	local award = CallModuleFunc(ModuleType.CHASM, "GetCurWaveAward" )

	layou:SetContentSize(#award * 110 , 90 )
	local interval = layou:GetContentWidth() / #award
	
    local func = nil
    self.index = 0
    local function whispering ( ... )
    	self.index = self.index + 1
    	if award[ self.index ] then
			local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.CHASM )
			--头像
			local icon = LogicGUI.IconLayer.New(false)
			icon:SetPosition( 55 + interval * (self.index - 1)  , layou:GetContentHeight()/2 )
			icon:SetData({ id = award[ self.index ].id , num = award[ self.index ].num * num } )
			icon:SetAnchorPoint(0.5,0.5)
			icon:PlayAppearEffect()
			layou:AddChild(icon:GetNode())

			if CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.CHASM ) ~= 1 then
				local double_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "double" , true )
				local double = Util:Sprite( double_sp )
				double:SetAnchorPoint( 0 , 1 )
				double:SetPosition( 0 , icon:GetNode():GetContentHeight() )
				icon:AddChild( double , 999)
		        CPPActionManager.DelayTo( layou , 0.3 , func )
		    end
		end
    end 

    func = GlobalCallbackMgr:AddCallBackFunc( function ( ... )
        whispering()
    end )
    whispering()


    --打通
    --最高
    local maxWave = CallModuleFunc(ModuleType.CHASM, "GetMaxWave" )
    --当前
    local curWave = CallModuleFunc(ModuleType.CHASM, "GetCurWave" )

    local chasm_endTips = Util:Label( CPPGameLib.GetString("chasm_endTips") , Macros.TypefaceSize.normal , Macros.Color.white )
    self.Panel_tips:AddChild( chasm_endTips )
    PosTool.Center( chasm_endTips )

    if curWave >= maxWave then
    	-- self.Image_finality:SetVisible( true )
    	self.Panel_tips:SetVisible( true )

    	local btnEffect_id , btnEffect_con = self:__CreateEffect({ res_name = "UI_wujinjiesuan3_language_chs" } , self.__root_widget )
		btnEffect_con:SetPosition(self.Image_finality:GetPositionX() , self.Image_finality:GetPositionY() )

    	self.Panel_tips:SetPosition( self.Panel_tips:GetPositionX() , self.Panel_tips:GetPositionY() - 20 )
		self.Panel_kill:SetPosition( self.Panel_kill:GetPositionX() , self.Panel_kill:GetPositionY() - 15 )
    	self.Button_next:SetText( CPPGameLib.GetString("chasm_end") )
    else
		CPPActionManager.DelayTo( self.Panel_tips , 15 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			self:OnButtonNext()
		end) )
    end
end

--下一波
function Modules.ChasmWinView:OnButtonNext( ... )
    self:CloseManual()
    if self.__my_close_callback then
        self.__my_close_callback()
    end
end
