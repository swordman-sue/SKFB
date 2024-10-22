
--[[
%% @module: 成员列表
%% @author: yjg
%% @created: 2016年11月5日16:58:56
--]]

Modules = Modules or {}

Modules.TemplUnionHallList = Modules.TemplUnionHallList or BaseClass(BaseTabView)

function Modules.TemplUnionHallList:__init()
	self:__AddPreloadTemplate(GUI.TemplateRegistry.UNION_HALL_LIST_ITEM)
end

--选项卡界面创建完成回调
function Modules.TemplUnionHallList:__LoadedCallback()
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplUnionHallList:__OpenCallback()
	CallModuleFunc(ModuleType.UNION , "GuildMemberListReq")
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_MEMBER,function (_, data)
        self:__SetShowUI()
    end)


end

--选项卡界面关闭回调
function Modules.TemplUnionHallList:__CloseCallback()

end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplUnionHallList:__Dispose()

end

--读取组件
function Modules.TemplUnionHallList:__Getwidget()
	--居中层
	self.Image_content = self:GetWidget("Image_content")
end

--创建组件
function Modules.TemplUnionHallList:__ShowUI()
	local params =  
	{
	    item_class = Modules.TemplUnionHallListItem,
	    item_width = self.Image_content:GetContentWidth() ,
	    item_height = 144.00,
	    row = 4,
	    col = 1,
	    view_height = self.Image_content:GetContentHeight() - 10 ,
		touchable = true,
		touch_callback = function(item_index, item_data)
			self:AppointResult(item_index, item_data)
		end,
	}
	-- 创建参数
	self.lst_list = GUI.ScrollView.New(params)
	self.Image_content:AddChild(self.lst_list:GetNode())
	PosTool.Center(self.lst_list:GetNode())
end

--设置组件
function Modules.TemplUnionHallList:__SetShowUI()
	if self.layer then
		self.layer:RemoveSelf(true)
		self.layer = nil
	end

	local lst = CallModuleFunc(ModuleType.UNION , "GetUnionMemberLst")
    self.lst_list:SetDataList( lst )
end

--任命的特效
function Modules.TemplUnionHallList:AppointResult(item_index, item_data)
    local item = self.lst_list:GetItem(item_index)

	if self.layer then
		self.layer:RemoveSelf(true)
		self.layer = nil
	end

    self.layer = Util:Layout( self.Image_content:GetContentWidth() , self.Image_content:GetContentHeight() )
    self.layer:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_content:AddChild( self.layer )
    PosTool.Center( self.layer )

	local popup = Util:Sprite9(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_tips"))

	local x = item.Button_appoint:GetPositionX() - (item.Button_appoint:GetContentWidth()/2) - (popup:GetContentWidth()/2)
	local y = item.Button_appoint:GetPositionY() + (popup:GetContentHeight()/3)
	local pos = NodeUnity.ConvertPosBetweenTwoContainer( item:GetNode() , self.layer , COCOPoint( x , y ) )

    --图片
	popup:SetPosition( pos.x , pos.y )
	popup:SetAnchorPoint( 0.5 , 0.5 )
	self.layer:AddChild(popup)

	self.layer:SetTouchEnabled(true)
	WidgetUnity.SetWidgetListener( self.layer , function( ... )
		self.layer:RemoveSelf(true)
		self.layer = nil
	end)

	--判断
	local appointType = CallModuleFunc(ModuleType.UNION , "GetMyAppointType",item_data)

	local text = {}
	--弹劾
	if appointType.accuse == true then
		text[ #text + 1 ] = "union_accuse" 
	end
	--转移会长
	if appointType.transfer == true then
		text[ #text + 1 ] = "union_appointChairman"
	end
	--任命
	if appointType.appoint_assist == true then
		text[ #text + 1 ] = "union_appointViceChairman"
	end
	--罢免
	if appointType.recall == true then
		text[ #text + 1 ] = "union_recall"
	end

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN,"btn_btn_green")

	local posY = {
			[1] = popup:GetContentHeight()/2 + 30 ,
			[2] = popup:GetContentHeight()/2 - 30 ,
	}

	if #text == 1 then
		popup:SetContentSize( popup:GetContentWidth() , popup:GetContentHeight()/2 )
		posY[1] = popup:GetContentHeight()/2
	end

	local btn = {}
	for i,v in ipairs(text) do
		btn[i] = Util:Button(sp, function()
			self:OnBtnPpoint(item_data,v)
		end , CPPGameLib.GetString(v) , nil , nil , Macros.Color.button_yellow )
		btn[i]:SetPosition(popup:GetContentWidth()/2 - 10 , posY[i] )
		popup:AddChild(btn[i])
	end

end

--任命
function Modules.TemplUnionHallList:OnBtnPpoint(item_data,text)
	local info = {}
	info.target_id = item_data.role_id

	if text == "union_appointChairman" then
		info.pos = Modules.UnionModule.CHAIRMAN
	elseif text == "union_appointViceChairman" then
		info.pos = Modules.UnionModule.VICE_CHAIRMAN

		local lst = CallModuleFunc(ModuleType.UNION , "GetUnionMemberLst")
		local index = 0
		for i,v in ipairs( lst ) do
			if v.position == Modules.UnionModule.VICE_CHAIRMAN then
				index = index + 1
			end
		end
		if index >= ConfigAdapter.Common.GetUnionOfficeMax() then
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_appointMax") )
			return 
		end
	else
		info.pos = Modules.UnionModule.MEMBER
	end
	CallModuleFunc(ModuleType.UNION , "GuildAppointReq" , info)
end


--等级礼包商品
Modules = Modules or {}
Modules.TemplUnionHallListItem = Modules.TemplUnionHallListItem or BaseClass(GUI.ScrollItem)

function Modules.TemplUnionHallListItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.UNION_HALL_LIST_ITEM)   
	self:__Getwidget()
end

function Modules.TemplUnionHallListItem:__delete()
end

function Modules.TemplUnionHallListItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self:__SetShowUI( data )
end

function Modules.TemplUnionHallListItem:__Getwidget()
	--条目
	self.Image_item = self:GetWidget("Image_item")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")	
	--名字
	self.Text_name = self:GetWidget("Text_name")	
	--等级
	self.Text_level = self:GetWidget("Text_level")	
	--战斗力
	self.Text_battleValue = self:GetWidget("Text_battleValue")	
	--功勋
	self.Text_merit = self:GetWidget("Text_merit")
	--今日贡献
	self.Text_state = self:GetWidget("Text_state")
	--离线时间
	self.Text_leave = self:GetWidget("Text_leave")
	--军衔
	self.Image_rank = self:GetWidget("Image_rank")
	--入团时间
	self.Text_addTime = self:GetWidget("Text_addTime")


	--任命
	self.Button_appoint = Util:WidgetButton( 
                                            self:GetWidget("Button_appoint") , 
                                            CPPGameLib.GetString("union_appoint") , 
                                            CPPGameLib.Handler(self,self.OnButtonAppoint) , 
                                            nil,nil, Macros.Color.button_red
                                        )
	--拒绝
	self.Button_discard = Util:WidgetButton( 
                                            self:GetWidget("Button_discard") , 
                                            CPPGameLib.GetString("union_elimination") , 
                                            CPPGameLib.Handler(self,self.OnButtonDiscard),
                                            nil,nil, Macros.Color.button_yellow
                                        )
	--拒绝
	self.Button_quit = Util:WidgetButton( 
                                            self:GetWidget("Button_quit") , 
                                            CPPGameLib.GetString("union_quit") , 
                                            CPPGameLib.Handler(self,self.OnButtonQuit),
                                            nil,nil, Macros.Color.button_yellow
                                        )

	--弹劾
	self.Button_accuse = Util:WidgetButton( 
                                            self:GetWidget("Button_accuse") , 
                                            CPPGameLib.GetString("union_accuse") , 
                                            CPPGameLib.Handler(self,self.OnButtonAccuse),
                                            nil,nil, Macros.Color.button_red
                                        )
	self.Button_accuse:SetVisible(false)

end

function Modules.TemplUnionHallListItem:__SetShowUI( data )
	--头像
	local img_head = LogicGUI.RoleIcon.New(true)
	img_head:SetTouchCallback(function()
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , data )
		-- CallModuleFunc(ModuleType.CHECKROLEINFO, "OpenView", BaseViewType.CHECKROLEINFO,data)
    end)
 	img_head:SetData(data.head_icon)
	self.Panel_icon:AddChild(img_head:GetNode())
	PosTool.Center(img_head)

	--名字
	Util:WidgetLabel( self.Text_name , data.name , Macros.TypefaceSize.normal , Macros.Color.Special )

	--等级
	Util:WidgetLabel( self.Text_level , CPPGameLib.GetString("common_lev9" , data.level ) , Macros.TypefaceSize.normal , Macros.Color.content )

	--入团时间
	Util:WidgetLabel( self.Text_addTime , CPPGameLib.GetString("common_lev9" , data.level ) , Macros.TypefaceSize.normal , Macros.Color.content )

	--战斗力
	Util:WidgetLabel( self.Text_battleValue , CPPGameLib.GetString("common_power_prefix1") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	local text_battleValue = Util:Label( data.battle_value , Macros.TypefaceSize.normal , Macros.Color.content )
	self.Image_item:AddChild( text_battleValue )
	PosTool.RightTo( self.Text_battleValue , text_battleValue )

	--功勋
	Util:WidgetLabel( self.Text_merit , CPPGameLib.GetString("union_exploit") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	local Text_merit = Util:Label( data.total_contribution , Macros.TypefaceSize.normal , Macros.Color.content )
	self.Image_item:AddChild( Text_merit )
	PosTool.RightTo( self.Text_merit , Text_merit )
	--vip
	if data.position ~= 0 then
		local vip = Util:Label( CPPGameLib.GetString( "union_position".. data.position ) , Macros.TypefaceSize.slightly , Macros.Color.golden )
		vip:SetAnchorPoint(0.5, 0 )
		self.Image_rank:AddChild( vip )
		PosTool.Center(vip , -5 , 3 )
		CPPActionManager.RotateTo(vip, 0 , -45)
	end

	--在线时间
	local color = Macros.Color.content
	if data.offline_time ~= 0 then
		color = Macros.Color.red
	end

	local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp(  data.offline_time )

	local time = Util:Label( TimeTool.OfflineTime(data.offline_time) , Macros.TypefaceSize.normal , color )
	time:SetAnchorPoint( 1 , 0.5 )
	time:SetPosition( 0 , self.Text_leave:GetContentHeight()/2 )
	self.Text_leave:AddChild( time )

	local tab = 
				{
					{ zi = CPPGameLib.GetString("union_todayFound") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
					{ zi = data.today_contribution , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },
				}
	local found = Util:LabCenter(tab)
	found:SetAnchorPoint( 0 , 0.5 )
	found:SetPosition( 0 , self.Text_state:GetContentHeight()/2 )
	self.Text_state:AddChild( found )

	self:SetAuthority(  )
end

function Modules.TemplUnionHallListItem:SetAuthority(  )
	local authority =  CallModuleFunc(ModuleType.UNION , "GetMyJurisdiction", self.__data )
	-- 任命
	self.Button_appoint:SetVisible( authority.appoint_assist == 1 )
	-- 逐出	
	self.Button_discard:SetVisible( authority.kick_out == 1 )
	-- 退出
	self.Button_quit:SetVisible( authority.leave == 1)

	self.Button_accuse:SetVisible( authority.accuse == 1 )
end

--逐出
function Modules.TemplUnionHallListItem:OnButtonDiscard()
    LogicGUI.TemplAffirmTips.New( { 
                                    type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                    content = CPPGameLib.GetString("union_discardTips") ,
                                    cancel_text = CPPGameLib.GetString("common_cancel") ,
                                    confirm_func = function ( ... )

                                    	CPPGameLib.PrintTable("xy" , self.__data , "info")
                                    	local info = {}
                                    	info.guild_id = self.__data.role_id
                                        CallModuleFunc(ModuleType.UNION , "GuildKickoutReq" , info )
                                    end,
                                    cancel_func = function ( ... )
                                    end,
                                } )
end

--退出
function Modules.TemplUnionHallListItem:OnButtonQuit()
    LogicGUI.TemplAffirmTips.New( { 
                                    type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                    content = CPPGameLib.GetString("union_quitTips") ,
                                    cancel_text = CPPGameLib.GetString("common_cancel") ,
                                    confirm_func = function ( ... )
                                        CallModuleFunc(ModuleType.UNION , "GuildLeaveReq" )
                                        --指引中，关闭主界面外的所有界面
                                        GlobalViewMgr:CloseViewsExcludeCity()
                                    end,
                                    cancel_func = function ( ... )
                                    end,
                                } )
end

--弹劾
function Modules.TemplUnionHallListItem:OnButtonAccuse()
-- 2649 军团弹劾加一个确认消耗钻的确认框
    LogicGUI.TemplAffirmTips.New( { 
                                    type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                    content = CPPGameLib.GetString("union_accuse_1" , ConfigAdapter.Common.GetUnionAccuseConsume() ) ,
                                    cancel_text = CPPGameLib.GetString("common_cancel") ,
                                    confirm_func = function ( ... )
                                    	local diamond_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
                                    	if diamond_num < ConfigAdapter.Common.GetUnionAccuseConsume() then
											LogicGUI.TemplAffirmTips.New( { 
															                type = LogicGUI.TemplAffirmTipsType.YES_NO,
															                content = CPPGameLib.GetString("fund_noDiamond") ,
															                cancel_text = CPPGameLib.GetString("common_cancel") ,
															                confirm_func = function ( ... )
															                    CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE )
															                end
															            } ) 
	                                    else
											CallModuleFunc(ModuleType.UNION , "GuildImpeachReq" )
	                                    end
                                    	-- print("xy" , "-----------1assdada")
                                        -- CallModuleFunc(ModuleType.UNION , "GuildLeaveReq" )
                                        -- --指引中，关闭主界面外的所有界面
                                        -- GlobalViewMgr:CloseViewsExcludeCity()
                                    end,
                                    cancel_func = function ( ... )
                                    end,
                                } )
end


function Modules.TemplUnionHallListItem:GetTouchNode()
	return self.Button_appoint
end
