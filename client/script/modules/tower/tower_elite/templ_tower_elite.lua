
--
-- @brief 爬塔精英挑战
-- @author: yjg
-- @date: 2016年9月26日17:01:28
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplTowerElite = LogicGUI.TemplTowerElite or BaseClass(GUI.Template)


function LogicGUI.TemplTowerElite:__init( fun )
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self.fun = fun
		self:__Getwidget()
		self:__ShowUI()
	end

	self.tower_elite_time = self:BindGlobalEvent(Macros.Event.TowerModule.TOWER_ELITE_TIME,function()
		self:__UpShowUI()
	end)

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TOWER)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CHALLENGE,true)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TOWER_EILTE_ITEM)

	self:InitTemplate(GUI.TemplateRegistry.TOWER_EILTE)	
end


function LogicGUI.TemplTowerElite:__delete()

	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end

	-- if self.modes then
	-- 	self.modes:DeleteMe()
	-- 	self.modes = nil
	-- end

	if self.__zones_scrollview then
		self.__zones_scrollview:DeleteMe()
		self.__zones_scrollview = nil
	end

end

function LogicGUI.TemplTowerElite:SetData(data)
end

function LogicGUI.TemplTowerElite:__Getwidget( ... )
	--背景
    self.Image_centre = self:GetWidget("Image_centre") 
    -- 光
    self.Image_light = self:GetWidget("Image_light") 
    --关闭按钮
    self.Button_close = Util:WidgetButton(self:GetWidget("Button_close"), " " , CPPGameLib.Handler( self,self.OnBtnExit ) )  
	--列表容器
    self.Image_content = self:GetWidget("Image_contentBg") 

    --关卡背景
    self.Panel_mapBg = self:GetWidget("Panel_mapBg") 
   
    --购买挑战次数
    self.Button_add = Util:WidgetButton(self:GetWidget("Button_add"), " " , CPPGameLib.Handler( self,self.OnButtonAdd ) )  

     --挑战
    self.Button_challenge = Util:WidgetButton(	
    											self:GetWidget("Button_challenge"), 
    											CPPGameLib.GetString("tower_challenge") , 
    											CPPGameLib.Handler( self,self.OnBtnChallenge ) ,
    											nil,nil,Macros.Color.button_red
    											)  

end

function LogicGUI.TemplTowerElite:__ShowUI( ... )	
	--标题
	self.light = Util:Label( CPPGameLib.GetString("tower_elite") , Macros.TypefaceSize.popup , Macros.Color.white )
	self.light:SetAnchorPoint(0.5 , 0.5)
	self.light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(self.light)

	self.modes = Util:Sprite( Resource.PathTool.GetHalfHeadPath(31341) )
	self.modes:SetAnchorPoint(0.5,0)
	self.modes:SetFlipX(true)
	self.modes:SetPosition( self.Panel_mapBg:GetContentWidth() - (self.modes:GetContentWidth()/4) , 70 )
	self.modes:SetScaleFactor(0.85)
	self.Panel_mapBg:AddChild( self.modes )

	local talkSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_talk")
	self.talkSp = Util:Sprite9( talkSp )
	self.talkSp:SetAnchorPoint(0.5,0.5)
	self.talkSp:SetScaleFactor( 0.8 )
	self.talkSp:SetFlipX(true)
	self.talkSp:SetPosition( self.Panel_mapBg:GetContentWidth()/2 - 90 , self.Panel_mapBg:GetContentHeight()/2 + 100 )
	self.Panel_mapBg:AddChild( self.talkSp )

	self.kail = Util:Label(" " , Macros.TypefaceSize.normal , Macros.Color.common_stroke , self.talkSp:GetContentWidth() - 120 )
	self.kail:SetAnchorPoint(0.5 , 0.5)
	self.kail:SetPosition( self.talkSp:GetPositionX() + 8 , self.talkSp:GetPositionY() )
	self.Panel_mapBg:AddChild( self.kail , 6)

	--道具
	self.item_icon = {}
	for i = 1 , 2 do
		self.item_icon[i] = LogicGUI.IconLayer.New(true)
		self.item_icon[i]:SetPosition( 20 + (90* (i - 1) ) , 80 )
		self.item_icon[i]:SetAnchorPoint( 0 , 0 )
		self.item_icon[i]:SetData({id = 13999})
		self.Panel_mapBg:AddChild( self.item_icon[i]:GetNode() )
	end

	--通关奖励
	local tower_congratsGetAward = Util:Label(CPPGameLib.GetString("battle_result_common_reward_title") , Macros.TypefaceSize.largish , Macros.Color.white )
	tower_congratsGetAward:SetAnchorPoint( 0 , 0 )
	tower_congratsGetAward:SetPosition( 5 , self.item_icon[1]:GetPositionY() + self.item_icon[1]:GetNode():GetContentHeight() + 5 )
	self.Panel_mapBg:AddChild( tower_congratsGetAward )

	--手刹奖励
	local bg_first_win = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_CHALLENGE, "bg_first_win" , true ) )
	bg_first_win:SetAnchorPoint( 0 , 1 )
	bg_first_win:SetPosition( self.item_icon[1]:GetNode():GetPositionX() + 1 , 
							  self.item_icon[1]:GetNode():GetPositionY() + self.item_icon[1]:GetNode():GetContentHeight() - 2
							 )
	self.Panel_mapBg:AddChild( bg_first_win )	

	--已经获取
	self.tower_get = Util:Label( 
									CPPGameLib.GetString("tower_get") ,  
									Macros.TypefaceSize.minimum , 
									Macros.Color.btn ,nil,nil,
									Macros.Color.red_stroke_1
								)
	self.tower_get:SetPosition( 
								self.item_icon[1]:GetNode():GetPositionX() + (self.item_icon[1]:GetNode():GetContentWidth()/2) - (self.tower_get:GetContentWidth()/2) , 
								self.item_icon[1]:GetNode():GetPositionY() + (self.item_icon[1]:GetNode():GetContentHeight()/2) - (self.tower_get:GetContentHeight()/2) )
	self.tower_get:SetAnchorPoint( 0 , 0 )
	self.Panel_mapBg:AddChild(self.tower_get)
	self.tower_get:SetVisible(false)

	--道具
	local params =  
	{
	  	item_class = Modules.TemplTowerEliteItem,
	  	item_width = self.Image_content:GetContentWidth() ,
	  	item_height = self.Image_content:GetContentHeight()/4 ,
	  	row = 4,
	  	col = 1,
	  	-- item_space_r = 5,
	  	-- item_space_c = 5,
	  	view_width = self.Image_content:GetContentWidth() ,
	  	-- horizon = true,
        selectable = true,
        select_callback = function ( index , item )
	        self:Setting( item )
			self.data = item
        end
	}
	-- 创建参数
	self.__zones_scrollview = GUI.ScrollView.New(params)
	self.__zones_scrollview:SetDataList( CallModuleFunc(ModuleType.TOWER , "GetEliteLst") )
	self.__zones_scrollview:SetAnchorPoint(0.5,0.5)
	self.__zones_scrollview:SetPosition(self.Image_content:GetContentWidth()/2,self.Image_content:GetContentHeight()/2)
	self.Image_content:AddChild(self.__zones_scrollview:GetNode())

	--今日可挑战次数
	self.challengeTime = Util:Label("000" ,Macros.TypefaceSize.normal , Macros.Color.green )
	self.challengeTime:SetAnchorPoint( 1 , 0.5 )
	self.challengeTime:SetPosition( self.Button_add:GetPositionX() - (self.Button_add:GetContentWidth()/2) - 10 ,
									self.Button_add:GetPositionY() )
	self.Image_centre:AddChild(self.challengeTime)

	self.dungeon_challenge_times_title = Util:Label( CPPGameLib.GetString("dungeon_challenge_times_title"),Macros.TypefaceSize.normal ,Macros.Color.white )
	self.dungeon_challenge_times_title:SetAnchorPoint(1,0.5)
	self.dungeon_challenge_times_title:SetPosition( self.challengeTime:GetPositionX() - (self.challengeTime:GetContentWidth()/2) , 
													self.challengeTime:GetPositionY() )
	self.Image_centre:AddChild(self.dungeon_challenge_times_title)

	local lst = CallModuleFunc(ModuleType.TOWER , "GetEliteLst")

	local data = nil
	local index = 1
	local level = CallModuleFunc(ModuleType.TOWER , "GetEliteLevel")

	for i,v in ipairs(lst) do
		if v.level <= (level+1) then
			data = v
			index = i
		else
			break
		end
	end

	self:Setting(data or lst[1])
	self.data = data or lst[1]
    
	self.__zones_scrollview:SelectItem(index)
	self.__zones_scrollview:LocateItem(index)

	self:__UpShowUI()
end

function LogicGUI.TemplTowerElite:__UpShowUI( ... )	
	self.challengeTime:SetString( CallModuleFunc(ModuleType.TOWER , "GetEliteTime") )
	self.dungeon_challenge_times_title:SetPosition( self.challengeTime:GetPositionX() - (self.challengeTime:GetContentWidth()/2) , 
													self.challengeTime:GetPositionY() )
	CallModuleFunc(ModuleType.TOWER , "GetBuyConsume") 
end

--关闭按钮
function LogicGUI.TemplTowerElite:OnBtnExit( ... )
	self:DeleteMe()
	if self.fun then
		self.fun()
	end
end

--布阵按钮
function LogicGUI.TemplTowerElite:OnButton_2( ... )
	print("buzheng")
end

--挑战按钮
function LogicGUI.TemplTowerElite:OnBtnChallenge( ... )
	
	local checkBattle = CallModuleFunc(ModuleType.TOWER , "CheckBattleCondition" , Macros.Game.FBType.TOWER_ELITE )
	if checkBattle then
        CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.TOWER_ELITE, self.data.level)
	else
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("dungeon_challenge_tips3") )
	end
end

--购买挑战次数
function LogicGUI.TemplTowerElite:OnButtonAdd( ... )
	local function BuyJudge( ... )
		local item = CallModuleFunc(ModuleType.VIP , "GetPrivilege" , Macros.Game.VIPType.BUY_KRUUNU_ELITE_MAX )

		local num = CallModuleFunc(ModuleType.TOWER , "GetAlreadyBuyTime" )
		if num < item then
			CallModuleFunc(ModuleType.TOWER , "TowerBuyTimesReq")
		else
	        local type_ , value_ = CallModuleFunc(ModuleType.VIP , "ToTypeGetResetItem" , Macros.Game.VIPType.BUY_KRUUNU_ELITE_MAX )
	        if type_ == true then
	            local tips = LogicGUI.TemplAffirmTips.New( { 
	                                                            type = LogicGUI.TemplAffirmTipsType.YES_NO,
	                                                            content = CPPGameLib.GetString("union_surplus") ,
	                                                            cancel_text = CPPGameLib.GetString("vip_Isee") ,
	                                                            confirm_text = CPPGameLib.GetString("vip_Toimprove") ,
	                                                            confirm_func = function ( ... )
	                                                                CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE )
	                                                                self:OnBtnExit()
	                                                            end,
	                                                            cancel_func = function ( ... )
	                                                            end
	                                                        } )
	        else
				GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("shop_today_can_notbuy") )
	        end
		end
	end

	local tips = LogicGUI.TemplAffirmTips.New( { 
													type = LogicGUI.TemplAffirmTipsType.YES_NO,
													content	= " ",
													confirm_text = CPPGameLib.GetString("common_confirm") ,
													confirm_func = function ( ... )
														BuyJudge()
													end,
													close_func = function ( ... )
														-- body
													end,
												} )
	
	tips:SetLoadedCallback(function()
		local Affirm = Util:LabCenter( { 
										{ zi = CPPGameLib.GetString("tower_expenditureAffirm") , ys = Macros.Color.content },
										{ tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , jg = 5 ,dx = 0.8},
										{ zi = CallModuleFunc(ModuleType.TOWER , "GetBuyConsume") , ys = Macros.Color.content },
										{ zi = CPPGameLib.GetString("tower_buyChallengeTime") , ys = Macros.Color.content },
									} )
		Affirm:SetPosition(tips.txt_content:GetContentWidth()/2 , tips.txt_content:GetContentHeight()/2 )
		tips.txt_content:AddChild(Affirm)
	end)
end

--设置背景
function LogicGUI.TemplTowerElite:Setting( data )
	self.kail:SetString( data.boss_chat )

	--设置怪物模型
	self.modes:SetImage( Resource.PathTool.GetHalfHeadPath(data.half_head) )

	--拿到关卡表的奖励
	local award = CPPGameLib.GetConfig("GameLevel", data.map_id)

	--首杀图标
	self.item_icon[1]:SetData( { id = award.first_win_item_id_list[1] , value = award.first_win_item_num_list[1] })

	--奖励图标
	self.item_icon[2]:SetData( { id = award.reward_item_id_list[1] , value = award.reward_item_num_min_list[1] })

	--首杀奖励
	local once = CallModuleFunc(ModuleType.TOWER , "GetEliteLevel")
	--如果这一关小于打过的
	if data.level <= once then
		self.tower_get:SetVisible(true)
	else
		self.tower_get:SetVisible(false)
	end

    --如果这个关卡的等级小于我打过的
    if data.level <= once then
    	self.Button_challenge:SetEnabled(true)
	--如果这个关卡的等级大于我打过的
    elseif data.level > once then
		local level = CallModuleFunc(ModuleType.TOWER , "GetLevelRecord")
		--如果需求的等级 小于等于 我已经打到的	
		if data.level == 1 then
			if level >= data.normal_map_level then
				self.Button_challenge:SetEnabled(true)
			else
				self.Button_challenge:SetEnabled(false)
			end
		else
			if data.normal_map_level <= level and once+1 >= data.level then
				self.Button_challenge:SetEnabled(true)
			else
				self.Button_challenge:SetEnabled(false)
			end
		end
    end
end



Modules = Modules or {}

Modules.TemplTowerEliteItem = Modules.TemplTowerEliteItem or BaseClass(GUI.ScrollItem)

function Modules.TemplTowerEliteItem:__init(data,direct)
    -- self:InitTemplate(GUI.TemplateRegistry.TOWER_EILTE_ITEM)
    -- self:__Getwidget()

	self.__layout_mode = GUI.Template.LayoutMode.CODE
	self:InitTemplate(nil) 

end

function Modules.TemplTowerEliteItem:__delete()
end

function Modules.TemplTowerEliteItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( 250 , 107.25)
    self.layout:SetClippingEnable(false)
    self.layout:SetAnchorPoint(0.5,0.5)

    return self.layout
end

function Modules.TemplTowerEliteItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	local btn_rlite_no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TOWER, "btn_rlite_no")
	self.btn_elite = Util:Sprite( btn_rlite_no )
	self.layout:AddChild( self.btn_elite )
	PosTool.Center( self.btn_elite )

	local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_bg_0")
	local icon_sp_item_bg = Util:Sprite( imgdata )
	icon_sp_item_bg:SetAnchorPoint( 0 , 0.5 )
	icon_sp_item_bg:SetPosition( 10 , self.btn_elite:GetContentHeight()/2 )
	self.btn_elite:AddChild( icon_sp_item_bg )

    --头像
    self.icon = LogicGUI.IconLayer.New(false)
    self.icon:SetAnchorPoint( 0.5 , 0.5 )
    self.icon:SetPosition( icon_sp_item_bg:GetContentWidth()/2 , icon_sp_item_bg:GetContentHeight()/2 )
    self.icon:SetTouchSwallow(false)
    icon_sp_item_bg:AddChild(self.icon:GetNode())
    local iconData = {id = data.half_head}
    self.icon:SetData(iconData)
    self.icon:ShowIconBG(false)
	self.icon:ShowIconQuality(false)

	local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_0")
	local icon_sp_item = Util:Sprite( imgdata )
	icon_sp_item:SetAnchorPoint( 0.5 , 0.5 )
	icon_sp_item:SetPosition( icon_sp_item_bg:GetContentWidth()/2 , icon_sp_item_bg:GetContentHeight()/2 )
	icon_sp_item_bg:AddChild( icon_sp_item )


    --名字
    local name = Util:Label( data.name , Macros.TypefaceSize.normal , Macros.Color.content )
    name:SetAnchorPoint( 0.5 , 0.5 )
    name:SetPosition(self.btn_elite:GetContentWidth()/2 + 44  , self.btn_elite:GetContentHeight()/2 )
    self.btn_elite:AddChild(name)

-- 
    --打过的
    local once = CallModuleFunc(ModuleType.TOWER , "GetEliteLevel")
	local level = CallModuleFunc(ModuleType.TOWER , "GetLevelRecord")
    local text = " "
    name:SetPosition(self.btn_elite:GetContentWidth()/2 + 44  , self.btn_elite:GetContentHeight()/2 )

	--如果要求的等级大于现在的等级
	if data.normal_map_level > level then
		text = CPPGameLib.GetString("tower_openTips2" , data.normal_map_level)

	    name:SetPosition(self.btn_elite:GetContentWidth()/2 + 44  , self.btn_elite:GetContentHeight()/2 + 15 )
	elseif data.level > (once+1) then
		local cnf = CPPGameLib.GetConfig("TowerEliteMap", data.level - 1)
		text = CPPGameLib.GetString("tower_openTips1" , cnf.name)

	    name:SetPosition(self.btn_elite:GetContentWidth()/2 + 44  , self.btn_elite:GetContentHeight()/2 + 15 )
	end

  	--提示
    local tips = Util:Label( text ,Macros.TypefaceSize.normal , Macros.Color.red_color , 140 )
    tips:SetPosition(name:GetPositionX() , name:GetPositionY() - (name:GetContentHeight()/2) - 5 )
    tips:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
    tips:SetAnchorPoint(0.5 , 1)
    self.btn_elite:AddChild(tips)

end

function Modules.TemplTowerEliteItem:__AddToParent()
end

function Modules.TemplTowerEliteItem:GetTouchNode()
    return self.layout
end
--选中特效
function Modules.TemplTowerEliteItem:SetSelected( var )
	if var then
		self.btn_elite:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TOWER, "btn_rlite_on") )
	else
		self.btn_elite:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TOWER, "btn_rlite_no") )
	end
end

