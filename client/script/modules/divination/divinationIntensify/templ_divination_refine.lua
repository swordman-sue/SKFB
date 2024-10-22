
-- --[[
-- %% @module: 卡牌进阶界面
-- %% @author: yjg
-- %% @created:2017年11月14日16:37:48
-- --]]

Modules = Modules or {}

Modules.TemplDivinationRefine = Modules.TemplDivinationRefine or BaseClass(BaseTabView)

function Modules.TemplDivinationRefine:__init()
end

--选项卡界面创建完成回调
function Modules.TemplDivinationRefine:__LoadedCallback()
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplDivinationRefine:__OpenCallback()
	self:__Server()
	self:__SetShowUI( nil , true )
end

--选项卡界面关闭回调
function Modules.TemplDivinationRefine:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplDivinationRefine:__Dispose()
    self:DelRollEffects()
end

function Modules.TemplDivinationRefine:__Server()
	self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_BREAK , function ( ... )
		self:UpEffects()
		self:__SetShowUI()
		self:__SetShowUI()
	end)
end

function Modules.TemplDivinationRefine:__Getwidget()
	--背景
	self.Image_packBg_0 = self:GetWidget("Image_packBg_0")
    self.Image_packBg_0:SetImage(Resource.PathTool.GetUIResPath("citycommonslices/bg_sp_equip_strengthen"), TextureResType.LOCAL)
	--名字背景
	self.Image_nameBg_0 = self:GetWidget("Image_nameBg_0")
	self.Image_nameBg_0:SetVisible(false)
	--名字
	self.Text_name = self:GetWidget("Text_name")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")   
	--信息背景
	self.Image_infoBg_0 = self:GetWidget("Image_infoBg_0")
	--箭头
	self.Image_arrows = self:GetWidget("Image_arrows")
	--分割线1
	self.Image_division_1 = self:GetWidget("Image_division_1")
	--分割线1
	self.Image_division_2 = self:GetWidget("Image_division_2")
	--分割线
	self.Image_division = self:GetWidget("Image_division")
	--提示
	self.Panel_tips = self:GetWidget("Panel_tips")
	--图标列表
	self.Image_iconLst = self:GetWidget("Image_iconLst")
	--进阶
    self.Button_advanced = Util:WidgetButton( 
                                            self:GetWidget("Button_advanced") , 
                                            CPPGameLib.GetString("divination_advanced") , 
                                            CPPGameLib.Handler(self,self.OnButtonAdvanced) ,
                                            nil,nil, Macros.Color.button_yellow
                                            )
    --价钱
	self.Image_price = self:GetWidget("Image_price")
end
	
function Modules.TemplDivinationRefine:__ShowUI()
	local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")

    -- 图标
    local path = Resource.PathTool.GetBigIconPath(20101)
    self.icon = Util:Sprite(path)
    self.icon:SetPosition(self.Panel_icon:GetContentWidth()/2 ,self.Panel_icon:GetContentHeight()/2 - 60  )
    self.icon:SetAnchorPoint(0.5, 0.5 )
    self.Panel_icon:AddChild(self.icon)
    --self.icon:SetScaleFactor( 0.8 )
    -- ActionManager.GetInstance():UpDownMoveAnimation(self.icon , 1 , 15)

    -- 图标框
    self.icon_frame = Util:Sprite(path)
    self.icon_frame:SetPosition(self.icon:GetContentWidth()/2 ,self.icon:GetContentHeight()/2 )
    self.icon_frame:SetAnchorPoint(0.5, 0.5 )
    self.icon:AddChild(self.icon_frame)
    -- ActionManager.GetInstance():UpDownMoveAnimation(self.icon , 1 , 15)

    --名字
    self.name = Util:Name( " " , Macros.TypefaceSize.tab )
    self.name:SetAnchorPoint( 0.5 , 0.5 )
    self.name:SetPosition( 60 + 70 + 13 , 35 )
    self.icon_frame:AddChild( self.name )

    --现在的名字
    self.nowName = Util:Label( " " , nil , Macros.Color.headline )
    self.nowName:SetAnchorPoint(0.5 , 0 )
    self.nowName:SetPosition( self.Image_division_1:GetContentWidth()/2 , 7 )
    self.Image_division_1:AddChild(self.nowName)

    --下级的名字
    self.nexName = Util:Label( " " , nil , Macros.Color.headline )
    self.nexName:SetAnchorPoint(0.5 , 0 )
    self.nexName:SetPosition( self.Image_division_2:GetContentWidth()/2 , 7 )
    self.Image_division_2:AddChild(self.nexName)

    self.nowPro = {}
    self.nowPro.type = {}
    self.nowPro.value = {}

    self.nexPro = {}
    self.nexPro.type = {}
    self.nexPro.value = {}

    for i = 1 , 4 do
	    --当前属性类型
	    self.nowPro.type[i] = Util:Label( " " , nil , Macros.Color.keypoint )
	    self.nowPro.type[i]:SetAnchorPoint( 1 , 0.5)
	    self.nowPro.type[i]:SetPosition( self.Image_division_1:GetContentWidth()/2 - 5 , 0 - ( (i) * 30 ) )
	    self.Image_division_1:AddChild(self.nowPro.type[i])

	    --当前属性值
	    self.nowPro.value[i] = Util:Label( " " , nil , Macros.Color.content )
	    self.nowPro.value[i]:SetAnchorPoint( 0 , 0.5)
	    self.nowPro.value[i]:SetPosition( self.Image_division_1:GetContentWidth()/2 + 5 , 0 - ( (i) * 30 ) )
	    self.Image_division_1:AddChild(self.nowPro.value[i])

	    --下级属性类型
	    self.nexPro.type[i] = Util:Label( " " , nil , Macros.Color.keypoint )
	    self.nexPro.type[i]:SetAnchorPoint( 1 , 0.5)
	    self.nexPro.type[i]:SetPosition( self.Image_division_2:GetContentWidth()/2 - 5 , 0 - ( (i) * 30 ) )
	    self.Image_division_2:AddChild(self.nexPro.type[i])

	    --下级属性值
	    self.nexPro.value[i] = Util:Label( " " , nil , Macros.Color.content )
	    self.nexPro.value[i]:SetAnchorPoint( 0 , 0.5)
	    self.nexPro.value[i]:SetPosition( self.Image_division_2:GetContentWidth()/2 + 5 , 0 - ( (i) * 30 ) )
	    self.Image_division_2:AddChild(self.nexPro.value[i])
    end

	local tab = {
					{ zi = CPPGameLib.GetString("divination_advancedTips") , ys = Macros.Color.keypoint },
					{ zi = "00/00" , ys = Macros.Color.red },
				}
	local tips = nil
	tips , self.tips = Util:LabCenter(tab)
	tips:SetAnchorPoint( 0.5 , 0.5 )
	tips:SetPosition( self.Panel_tips:GetContentWidth()/2 , self.Panel_tips:GetContentHeight()/2 )
	self.Panel_tips:AddChild( tips )
	self:__SetShowUI( nil , true )
end


function Modules.TemplDivinationRefine:__SetShowUI( _ , type_ )

	local uid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid" )
	
	local data = CallModuleFunc(ModuleType.DIVINATION, "GetDivination" , uid )

    self.icon:SetImage(Resource.PathTool.GetBigIconPath( data.bigicon ))
    --卡框
    local sp = Resource.PathTool.GetUIResPath("divinationbig/icon_sp_card_".. data.quality )
    -- Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_card_" .. data.quality )
    self.icon_frame:SetImage( sp )
    self.icon_frame:SetPosition(self.icon:GetContentWidth()/2 ,self.icon:GetContentHeight()/2 )

    self.name:SetString( data.name )

	local t1 = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo" , data.info_id )

	local nex = CallModuleFunc(ModuleType.DIVINATION, "GetDivinationBreakMAX" , uid)

    --是否保存当前强化等级
    if type_ then
        self.nowLevel = data.break_level
		local text = "+" .. data.break_level
		if data.break_level == 0 then
			text = ""
		end
		--名字设置
		self.nowName:SetString( t1.name .. text )
		--属性设置
		--设置当前
		local t1 = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo" , data.info_id , data.level , data.break_level )
		
		for i = 1 , #self.nowPro.type do
			self.nowPro.type[i]:SetString("  ")
			self.nowPro.value[i]:SetString(" ")
		end

		for i , v in ipairs( t1.break_proEx ) do
			local type_ , value_ = Util:Nature( v.type , v.value , true )
			self.nowPro.type[i]:SetString( type_ )
			self.nowPro.value[i]:SetString( value_ )
		end
    end
    
	if nex.break_level then
		local t1 = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo" , data.info_id , data.level , nex.break_level )
		self.nexName:SetString( t1.name .. " + " .. nex.break_level )
		--设置下次
		for i , v in ipairs( t1.break_proEx ) do
			local type_ , value_ = Util:Nature( v.type , v.value , true )
			self.nexPro.type[i]:SetString( type_ )
			self.nexPro.value[i]:SetString( value_ )
		end
	else
		self.nexName:SetString( "" )
		for i = 1 , #self.nexPro.type do
			self.nexPro.type[i]:SetString( " " )
			self.nexPro.value[i]:SetString( " " )
		end
	end
    
	--提示显示
	local cnf = CPPGameLib.GetConfig("CardBreak", data.quality )
	local new = cnf.break_level_list[ data.break_level ]

	print("xy" , "=========data.level " , data.level .. "/" .. new.need_card_level )
	self.tips[2]:SetString( data.level .. "/" .. new.need_card_level )
	if data.level >= new.need_card_level then
		self.tips[2]:SetColor(unpack( Macros.Color.front_color ))
	else
		self.tips[2]:SetColor(unpack( Macros.Color.red ))
	end

	--突破所需列表
	self.consumeLst = {}

	--突破所需道具
	if self.upicon then
		for i = 1 , #self.upicon do
			self.upicon[i]:DeleteMe()
			self.upicon[i] = nil
		end
	end
	self.upicon = {}
	for i,v in ipairs( new.cost_item_id_list ) do
		if new.cost_item_num_list[i] ~= 0 then
			--图标
		    self.upicon[i] = LogicGUI.IconLayer.New(false,true)		    
		    self.upicon[i]:SetPosition( 5 + ( (i-1) * (self.Image_iconLst:GetContentWidth()/2) )  , self.Image_iconLst:GetContentHeight() / 2 )
		    self.upicon[i]:SetAnchorPoint( 0 , 0.5 )
		    self.Image_iconLst:AddChild(self.upicon[i]:GetNode())
		    self.upicon[i]:SetData({id = v })
		    self.upicon[i]:SetTouchCallback(function ()
		    	--获取途径
		    	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, v)
		    end)

		    local _ , info = ObjIDTool.GetInfo( v )
		    --名字
		    local name = Util:Name( info.name , Macros.TypefaceSize.slightly , info.quality )
		    name:SetAnchorPoint( 0 , 1 )
		    name:SetPosition( self.upicon[i]:GetNode():GetContentWidth() + 5 , self.upicon[i]:GetNode():GetContentHeight() - 5 )
		    self.upicon[i]:AddChild( name )

		    --数量
		    local num = ObjIDTool.GetOwnNum( v )
			local tab = {
							{ zi = num , ys = num >= new.cost_item_num_list[i] and Macros.Color.front_color or Macros.Color.red},
							{ zi = "/" .. new.cost_item_num_list[i] , ys = Macros.Color.content },
						}
			local sum = Util:LabCenter(tab)
		    sum:SetAnchorPoint( 0 , 0 )
		    sum:SetPosition( self.upicon[i]:GetNode():GetContentWidth() + 5 , 5 )
		    self.upicon[i]:AddChild( sum )  

		    self.consumeLst[i] = num >= new.cost_item_num_list[i]
		end
	end

	if new.cost_same_card_num and new.cost_same_card_num ~= 0 then
		--图标
	    self.upicon[ #self.upicon + 1 ] = LogicGUI.IconLayer.New()
	    self.upicon[ #self.upicon ]:SetPosition( 5 + ( (#self.upicon-1) * (self.Image_iconLst:GetContentWidth()/2) )  , self.Image_iconLst:GetContentHeight() / 2 )
	    self.upicon[#self.upicon]:SetAnchorPoint( 0 , 0.5 )
	    self.Image_iconLst:AddChild(self.upicon[#self.upicon]:GetNode())
	    self.upicon[#self.upicon]:SetData({ id = data.info_id })

	    local _ , info = ObjIDTool.GetInfo( data.info_id )
	    --名字
	    local name = Util:Name( info.name , Macros.TypefaceSize.slightly , info.quality )
	    name:SetAnchorPoint( 0 , 1 )
	    name:SetPosition( self.upicon[#self.upicon]:GetNode():GetContentWidth() + 5 , self.upicon[#self.upicon]:GetNode():GetContentHeight() - 5 )
	    self.upicon[#self.upicon]:AddChild( name )

	    --数量
	    local index = CallModuleFunc(ModuleType.DIVINATION,"ToInfoIdGetDivinationNum" , data.info_id , nil , true )
		local tab = {
						{ zi = index <= 0 and 0 or index , ys = index >= new.cost_same_card_num and Macros.Color.front_color or Macros.Color.red},
						{ zi = "/" .. new.cost_same_card_num , ys = Macros.Color.content },
					}
		local sum = Util:LabCenter(tab)
	    sum:SetAnchorPoint( 0 , 0 )
	    sum:SetPosition( self.upicon[#self.upicon]:GetNode():GetContentWidth() + 5 , 5 )
	    self.upicon[#self.upicon]:AddChild( sum )  
	end

	--突破所需货币
	if self.price then
		self.price:RemoveSelf()
		self.price = nil
	end
	local tab = {
					{ tp = Util:GetMoneyIconImgData( new.cost_type_list[1] ) , dx = 0.8 },
					{ zi = new.cost_value_list[1] , ys = Macros.Color.white , jg = 10 },
				}
	self.price = Util:LabCenter(tab)
    self.price:SetAnchorPoint( 0.5 , 0.5 )
    self.price:SetPosition( self.Image_price:GetContentWidth()/2 , self.Image_price:GetContentHeight()/2 )
    self.Image_price:AddChild( self.price ) 

    local num = CallModuleFunc(ModuleType.ROLE,"GetPropValue", new.cost_type_list[1] )
    self.consumeLst[ #self.consumeLst + 1 ] =  num >= new.cost_value_list[1]

    --满级提示
    if self.divination_maxAdvance then
    	self.divination_maxAdvance:RemoveSelf(true)
    	self.divination_maxAdvance = nil
    end

	local nex = CallModuleFunc(ModuleType.DIVINATION, "GetDivinationBreakMAX" , uid)
	if not nex.break_level then
		local tab = {
						{ zi = CPPGameLib.GetString("divination_maxAdvance") , ys = Macros.Color.content },
					}
		self.divination_maxAdvance = Util:LabCenter(tab)
	    self.divination_maxAdvance:SetAnchorPoint( 0.5 , 0.5 )
	    self.divination_maxAdvance:SetPosition( self.Image_iconLst:GetContentWidth()/2 , self.Image_iconLst:GetContentHeight()/2 )
	    self.Image_iconLst:AddChild( self.divination_maxAdvance ) 
	end

end

--进阶
function Modules.TemplDivinationRefine:OnButtonAdvanced()
	CPPGameLib.PrintTable("xy" , self.consumeLst , "self.consumeLst") 

	for i = 1 , #self.consumeLst - 1 do
		if self.consumeLst[i] == false then
			--获取途径
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, ConfigAdapter.Common.GetDivinationBreakPropId())
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_materialsInsufficient"))
			return
		end
	end

	local uid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid" )
	local data = CallModuleFunc(ModuleType.DIVINATION, "GetDivination" , uid )

	local cnf = CPPGameLib.GetConfig("CardBreak", data.quality )
	local new = cnf.break_level_list[ data.break_level ]
	if self.consumeLst[ #self.consumeLst ] == false then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("currency_insufficient_"..new.cost_type_list[1]) )
		return
	end

	local nex = CallModuleFunc(ModuleType.DIVINATION, "GetDivinationBreakMAX" , uid)
	if not nex.break_level then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("divination_maxAdvance") )
		return
	end

	if data.level < new.need_card_level then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("divination_notIntensifyLevel") )
		return 
	end

	if new.cost_same_card_num and new.cost_same_card_num ~= 0 then
		--数量
		local index = CallModuleFunc(ModuleType.DIVINATION,"ToInfoIdGetDivinationNum" , data.info_id , nil , true )
		if index <= 0 then
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("direct_buy_insufficient" , data.name ) )
			return 
		end
	end

	CallModuleFunc(ModuleType.DIVINATION, "CardBreakReq" , uid)
end

--升级特效
function Modules.TemplDivinationRefine:UpEffects()
    self:DelRollEffects()
    self.faceEffects = {}

    local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")
    local info = CallModuleFunc(ModuleType.DIVINATION, "GetDivination" , divinationUid )
    
    print("xy" , "------------------" , self.nowLevel , info.break_level )
    --升级了
    if self.nowLevel ~= info.break_level then

        local layout = Util:Layout( 100 , 100 )
        layout:SetAnchorPoint( 0.5 , 0.5 )

        local tab = {
                        { zi = CPPGameLib.GetString("divination_AdvancedSucceed" , info.break_level ) , ys = 0 , mb = true, dx = Macros.TypefaceSize.popup },
                    }
        local item_des = Util:LabCenter(tab)
        item_des:SetAnchorPoint(0.5 , 0.5)
        layout:AddChild(item_des)
        PosTool.Center( item_des )

        local pro_info = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo" , info.info_id , info.level , info.break_level )
        
        local pro_info_ = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo" , info.info_id , info.level , info.break_level - 1 )
        --属性
        for i , v in ipairs( pro_info.break_proEx ) do
        	local value = v.value
        	if pro_info and pro_info.break_proEx then
		        value = v.value - pro_info_.break_proEx[i].value
	        end
            local type_ , value_ = Util:Nature( v.type , value , true )
	        local tab = {
	                        { zi = type_ .. " " .. value_ , ys = 1 , mb = true, dx = Macros.TypefaceSize.largish },
	                    }
	        local item_des = Util:LabCenter(tab)
	        item_des:SetAnchorPoint(0.5 , 0.5)
	        layout:AddChild(item_des)
	        PosTool.Center( item_des , 0 , -30 + ( ( i-1 ) * -30 ) )
        end
        GlobalTipMsg.GetInstance():ShowLayer( layout , false , true , true  )

        print("xy" , "=============pro_info.break_proEx" , pro_info.break_proEx )
		--属性
        for i , v in ipairs( pro_info.break_proEx ) do
            local type_ , value_ = Util:Nature( v.type , v.value , true )
            if self.nowPro.type[i] then
                self.faceEffects[i] = TypefaceEffects.New()
                self.faceEffects[i]:Roll( self.nowPro.value[i] , value_ , "" , function ( ... )
                	self:__SetShowUI( nil , true )
                end)
            end
        end
        -- self.nowLevel = info.break_level
    end
end

--销毁滚字特效
function Modules.TemplDivinationRefine:DelRollEffects()
    if self.faceEffects then
        for _, effect in pairs(self.faceEffects) do
            effect:DeleteMe()
        end
        self.faceEffects = nil
    end
end
