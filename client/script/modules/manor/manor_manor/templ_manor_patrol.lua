
--[[
%% @module: 领地巡逻
%% @author: yjg
%% @created: 2016年10月11日10:59:36
--]]

Modules = Modules or {}

Modules.TemplManorPatrol = Modules.TemplManorPatrol or BaseClass(GUI.Template)

function Modules.TemplManorPatrol:__init( data )
	self.__is_auto_load = true
		
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self.data = data
		self:__Getwidget()
		self:__ShowUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.MANOR_PATROL)	
end

function Modules.TemplManorPatrol:__delete()
end

function Modules.TemplManorPatrol:__Getwidget()
	--背景
	self.Image_BG = self:GetWidget("Image_BG") 
	--光
	self.Image_light = self:GetWidget("Image_light") 
	--关闭
	self.Button_close = Util:WidgetButton(self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
	--副本图片
	self.Image_setting = self:GetWidget("Image_setting") 

	--控制面板
	self.Image_controlPanel = self:GetWidget("Image_controlPanel") 
	--条目
	self.Image_itemBg = self:GetWidget("Image_itemBg") 
	--线
	self.Image_1 = self:GetWidget("Image_1") 
	--名字
	self.Text_name = self:GetWidget("Text_name")
	--图标列表 
	self.Panel_iconBg = self:GetWidget("Panel_iconBg") 

	--左箭头 
	self.Image_left = self:GetWidget("Image_left") 
	self.Image_left:SetVisible(false)
	--右箭头 
	self.Image_right = self:GetWidget("Image_right") 
	self.Image_right:SetVisible(false)

	--图标列表 
	self.Panel_control = self:GetWidget("Panel_control") 

	--单选框 
	self.CheckBox_1 = self:GetWidget("CheckBox_1") 
	self.CheckBox_2 = self:GetWidget("CheckBox_2") 
	self.CheckBox_3 = self:GetWidget("CheckBox_3") 

	--类型背景 
	self.Image_typeBg = self:GetWidget("Image_typeBg") 

	--选择按钮 
	self.Button_select = Util:WidgetButton( 	
												self:GetWidget("Button_select") , nil , 
												CPPGameLib.Handler(self,self.OnButtonSelect) 
											)
	--巡逻 
	self.Button_begin = Util:WidgetButton( 	
												self:GetWidget("Button_begin") , 
												CPPGameLib.GetString("manor_seek") , 
												CPPGameLib.Handler(self,self.OnButtonBegin) ,
												nil,nil,Macros.Color.button_yellow
											)
end

function Modules.TemplManorPatrol:__ShowUI( )
	--领地表
	local landCityCnf = CPPGameLib.GetConfig("LandCity", self.data[1] )

	--标题
	local light = Util:Label( landCityCnf.name , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	light:SetAnchorPoint(0.5,0.5)
	light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(light)

----------
	self.Image_setting:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR_BG, landCityCnf.build_bg ) )

	local bg_shadow = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_shadow"))
	bg_shadow:SetPosition(self.Image_setting:GetContentWidth()/2 , 30 )
	self.Image_setting:AddChild(bg_shadow)

	--怪物模型
	local modes = LogicGUI.Model.New({info_id = self.data[2].info_id , parent = bg_shadow })
	modes:SetPosition(bg_shadow:GetContentWidth()/2 , bg_shadow:GetContentHeight()/2 )
	modes:SetScaleFactor(0.9)
	modes:SetTouchRect(-50, 0, 100, 150)
	modes:SetTouchFun(nil, nil, function()
		CallModuleFunc(ModuleType.MANOR, "OpenView", BaseViewType.MANORHEROLST , self.data[1] )
		self:DeleteMe()
	end)

	local type_ , info_ = ObjIDTool.GetInfo(self.data[2].info_id)
	--名字
	local name = Util:Name(info_.name , Macros.TypefaceSize.slightly , info_.init_quality)
	name:SetAnchorPoint(0.5,0.5)
	name:SetPosition(bg_shadow:GetContentWidth()/2 , bg_shadow:GetContentHeight()/2 + 160)
	bg_shadow:AddChild(name)
	
	local item_id = CPPGameLib.GetConfig("HeroCompound", self.data[2].info_id).item_id
	local packType_ , packInfo_ = ObjIDTool.GetInfo(item_id)

	--派遣提示
	local manor_dispatch = Util:LabCenter( { 
											{ zi = CPPGameLib.GetString("manor_dispatch1") , mb = true },
											{ zi = " " .. info_.name .." " , mb = true , ys = info_.init_quality},
											{ zi = ", " .. CPPGameLib.GetString("manor_dispatch2") , mb = true },
											{ zi = " " .. info_.name .. " " , mb = true , ys = info_.init_quality },
											{ zi = CPPGameLib.GetString("manor_dispatch3") , mb = true },
											{ zi = " " .. packInfo_.name .. " " , mb = true , ys = info_.init_quality },
										 } )
	manor_dispatch:SetAnchorPoint(0.5,0.5)
	manor_dispatch:SetPosition(bg_shadow:GetContentWidth()/2 , bg_shadow:GetContentHeight()/2 - 15)
	bg_shadow:AddChild(manor_dispatch)

----------
 	--巡逻奖励
	local manor_patrolEnd = Util:Label( CPPGameLib.GetString("manor_patrolEnd") , Macros.TypefaceSize.normal , Macros.Color.headline )
	manor_patrolEnd:SetAnchorPoint(0,0.5)
	manor_patrolEnd:SetPosition( 0 , self.Text_name:GetContentHeight()/2  )
	self.Text_name:AddChild(manor_patrolEnd)

 	--icon列表
	local pack = CallModuleFunc(ModuleType.MANOR, "GetCityPack",self.data[1])
	local pieceInfo = CPPGameLib.GetConfig("HeroCompound",self.data[2].info_id)
	table.insert(pack, 1 ,{id = pieceInfo.item_id  , num = 1})

    local common =  
    {
        item_class = Modules.TempManorShotsItem,
        horizon = true,
        item_width = 90,
        item_height = 90,
        row = 1,
        col = 4,
        selectable = true,
        update_move_func = function ( data )
        	if #pack > 4 then
	        	local min = self.common:GetScrollMinX()
				local max = self.common:GetScrollMaxX()
				local x = self.common:GetScrollX()
				self.Image_left:SetVisible(false)
				self.Image_right:SetVisible(false)
				if x <= -30 then
					self.Image_left:SetVisible(true)
				end 

				if math.abs(x - min) > 30 then
					self.Image_right:SetVisible(true)
				end
			end
        end,
        view_width = self.Panel_iconBg:GetContentWidth() - 10 ,
    }

    -- 普通列表
    self.common = GUI.ScrollView.New(common)
    self.common:SetDataList( pack )
    self.Panel_iconBg:AddChild(self.common:GetNode())
    PosTool.Center(self.common:GetNode() )

-- ----------
-- 	--单选框

	local landPatrolCnf = CPPGameLib.GetConfig("LandPatrol", 1)
	for i=1,3 do
	    local manor_patrolTime = Util:Label( CPPGameLib.GetString( "manor_patrolTime" , landPatrolCnf["time_type_data_"..i][1] / 60 ) , Macros.TypefaceSize.normal , Macros.Color.content )
	    manor_patrolTime:SetAnchorPoint(0,0.5)
	    manor_patrolTime:SetPosition( self["CheckBox_"..i]:GetContentWidth() + self["CheckBox_"..i]:GetPositionX() , self["CheckBox_"..i]:GetPositionY()  )
	    self.Panel_control:AddChild(manor_patrolTime)
	    self["CheckBox_"..i]:SetSelectedState(false)
	   	self["CheckBox_"..i]:SetEventListener(
	        GlobalCallbackMgr:AddCallBackFunc(function()
			    self:OnSetCbx(i)
	        end)
	    )  

	end

	self.methodLst = {
			[1] = CPPGameLib.GetString("manor_common"),
			[2] = CPPGameLib.GetString("manor_intermediate"),
			[3] = CPPGameLib.GetString("manor_advanced"),
	}
	self.index = 1

	self.method = Util:Label( self.methodLst[self.index] , Macros.TypefaceSize.normal , Macros.Color.content )
	self.method:SetAnchorPoint(0,0.5)
	self.method:SetPosition( 10 , self.Image_typeBg:GetContentHeight()/2 )
	self.Image_typeBg:AddChild( self.method )

	self.timeIndex = 1
	self:OnSetCbx(self.timeIndex)
end

--关闭按钮
function Modules.TemplManorPatrol:OnButtonClose( ... )
	self:DeleteMe()
end

--添加按钮
function Modules.TemplManorPatrol:OnButtonSelect( ... )
	Modules.TempManorDifficulty.New( self.index , function ( index )
		self.index = index
		self.method:SetString( self.methodLst[self.index] )

		self:CountSettle()
	end)

end

--时间选择
function Modules.TemplManorPatrol:OnSetCbx( index )
	for i=1,3 do
	    self["CheckBox_"..i]:SetSelectedState(false)
	end
	--选择的时间
	self.timeIndex = index
	self["CheckBox_"..index]:SetSelectedState(true)

	self:CountSettle()
end



--数量整理
function Modules.TemplManorPatrol:CountSettle(  )
	--时间
	-- self.timeIndex
	--难度
	-- self.index

	local landPatrolCnf = CPPGameLib.GetConfig("LandPatrol", self.index )
	local pack = CallModuleFunc(ModuleType.MANOR, "GetCityPack",self.data[1])

	local pieceInfo = CPPGameLib.GetConfig("HeroCompound",self.data[2].info_id)

	for i,v in ipairs(pack) do
		v.num = v.num * landPatrolCnf.multiple 
	end

	local num = 0
	local multiple = landPatrolCnf.multiple
	if multiple == 0 then
		multiple = 1
	end

	-- 2791 领地 高级探索给的碎片奖励不对
	if landPatrolCnf["time_type_data_"..self.timeIndex][4] == landPatrolCnf["time_type_data_"..self.timeIndex][5] then
		num = landPatrolCnf["time_type_data_"..self.timeIndex][4] 
	else
		num = landPatrolCnf["time_type_data_"..self.timeIndex][4] .. "~" .. landPatrolCnf["time_type_data_"..self.timeIndex][5] 
	end

	table.insert(pack, 1 ,{id = pieceInfo.item_id  , num = num})


	if self.consume then
		self.consume:RemoveSelf(true)
		self.consume = nil
	end

	if landPatrolCnf["time_type_data_"..self.timeIndex][3] ~= 0 then
		local tab = {
						{ tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.DIAMOND ) , jg = 10 , dx = 0.6 },
						{ zi = landPatrolCnf["time_type_data_"..self.timeIndex][3] , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal , jg = 5 },
					}
		self.consume =  Util:LabCenter(tab)
		self.consume:SetAnchorPoint( 0.5 , 0.5 )
		self.consume:SetPosition( self.Button_begin:GetPositionX() , self.Button_begin:GetPositionY() + 40 )
		self.Panel_control:AddChild( self.consume )

	elseif landPatrolCnf["time_type_data_"..self.timeIndex][2] ~= 0 then
		local tab = {
						{ tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.ENERGY ) , jg = 10 , dx = 0.6 },
						{ zi = landPatrolCnf["time_type_data_"..self.timeIndex][2] , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal , jg = 5 },
					}
		self.consume =  Util:LabCenter(tab)
		self.consume:SetAnchorPoint( 0.5 , 0.5 )
		self.consume:SetPosition( self.Button_begin:GetPositionX() , self.Button_begin:GetPositionY() + 40 )
		self.Panel_control:AddChild( self.consume )
	end

	self.common:SetDataList( pack )
end

--发送
function Modules.TemplManorPatrol:OnButtonBegin( index )
	local now_diamond = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
	local energy = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.ENERGY)
	local landPatrolCnf = CPPGameLib.GetConfig("LandPatrol", self.index )

	if now_diamond < landPatrolCnf["time_type_data_"..self.timeIndex][3] and landPatrolCnf["time_type_data_"..self.timeIndex][3] ~= 0 then
		
		local txt = "currency_insufficient_"..Macros.Game.RolePropType.DIAMOND
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
		return

	elseif energy < landPatrolCnf["time_type_data_"..self.timeIndex][2] and landPatrolCnf["time_type_data_"..self.timeIndex][2] ~= 0 then

		local txt = "currency_insufficient_"..Macros.Game.RolePropType.ENERGY
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
		return

	end

	local info = {}
	info.city_id = self.data[1]
	info.patrol_type = self.index
	info.time_type = self.timeIndex
	info.patrol_hero_uid = self.data[2].uid

	CallModuleFunc(ModuleType.MANOR, "LandPatrolReq",info)
	self:DeleteMe()
end



-- -----------------------------------------------------------------------------------------------------------------------------------
-- -----------------------------------------------------------------------------------------------------------------------------------
-- -----------------------------------------------------------------------------------------------------------------------------------
-- --[[
-- %% @module: 领地攻击难度选择界面
-- %% @author: yjg
-- %% @created: 2016年10月11日10:59:36
-- --]]

Modules = Modules or {}

Modules.TempManorDifficulty = Modules.TempManorDifficulty or BaseClass(GUI.Template)

function Modules.TempManorDifficulty:__init( index , fun)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self.index = index
		self.fun = fun
		self:__Getwidget()
		self:__ShowUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.MANOR_DIFFICULTY)	
end


function Modules.TempManorDifficulty:__delete()
end

function Modules.TempManorDifficulty:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg") 
	--光
	self.Image_light = self:GetWidget("Image_light")
	--关闭
	self.Button_close = Util:WidgetButton(self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
	--内容框
	self.Image_contentBg = self:GetWidget("Image_contentBg")
	--确定
	self.Button_confirm = Util:WidgetButton(
												self:GetWidget("Button_confirm") , 
												CPPGameLib.GetString("common_confirm") , 
												CPPGameLib.Handler(self,self.OnButtonConfirm) ,
												nil,nil, Macros.Color.button_yellow  )
	
	--牌子
	self.Image_type = {}
	--名字
	self.Image_name = {}
	--vip等级
	self.vipLevel = {}

	for i=1,3 do
		self.Image_name[i] = self:GetWidget("Image_name"..i) 
		self.Image_name[i]:SetTouchEnabled(true)
		WidgetUnity.SetWidgetListener(self.Image_name[i], nil, nil, function ( ... )

			local level = CallModuleFunc( ModuleType.ROLE,"GetPropValue" , Macros.Game.RolePropType.VIP_LEVEL )
			if self.vipLevel[i] and self.vipLevel[i] > level then
				GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("vip_noRights") )
			else
				self:OnShow( self.Image_name[i] )
				self.index = i
			end

		end)
		-- self.Image_name[i] = self:GetWidget("Image_name"..i) 
	end
	self:OnShow( self.Image_name[self.index] )

end

function Modules.TempManorDifficulty:__ShowUI( index )
	--标题
	local light = Util:Label( CPPGameLib.GetString("manor_patrolWay") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	light:SetAnchorPoint(0.5,0.5)
	light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(light)

	for i,v in ipairs(CPPGameLib.GetConfig("LandPatrol", nil, nil , true )) do
		local lab = Util:Label( CPPGameLib.GetString("manor_earningsTime",v.interval_time) , Macros.TypefaceSize.normal , Macros.Color.content , self.Image_name[i]:GetContentWidth() - 30 )
		lab:SetAnchorPoint(0.5, 1 )
		lab:SetPosition( self.Image_name[i]:GetContentWidth()/2 , self.Image_name[i]:GetContentHeight()/2 + 20 )
		lab:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
		self.Image_name[i]:AddChild(lab)

		local y = lab:GetPositionY() - lab:GetContentHeight() - 5

		if v.multiple ~= 1 then
		    local multiple = Util:Label( CPPGameLib.GetString("manor_multiple" , v.multiple)  , Macros.TypefaceSize.normal , Macros.Color.content )
		    multiple:SetAnchorPoint( 0.5 , 1 )
		    multiple:SetPosition( self.Image_name[i]:GetContentWidth()/2 , lab:GetPositionY()  - lab:GetContentHeight() - 5 )
		    self.Image_name[i]:AddChild(multiple)

		    y = multiple:GetPositionY() - multiple:GetContentHeight() - 5
		end

		if v.vip_idx ~= 0 then
			local level = 0
			for i,vv in ipairs(CPPGameLib.GetConfig("Vip", nil, nil , true )) do
				if vv["value"..v.vip_idx] ~= 0 then
					level = i
					break
				end
			end

			self.vipLevel[i] = level

			local prop_level = CallModuleFunc( ModuleType.ROLE,"GetPropValue" , Macros.Game.RolePropType.VIP_LEVEL )

			local color = Macros.Color.front_color
			if self.vipLevel[i] > prop_level then
				color = Macros.Color.red_color
			end
		    local manor_vipOpen = Util:Label( CPPGameLib.GetString("manor_vipOpen" , level)  , Macros.TypefaceSize.normal , color )
		    manor_vipOpen:SetAnchorPoint( 0.5 , 1 )
		    manor_vipOpen:SetPosition( self.Image_name[i]:GetContentWidth()/2 , y )
		    self.Image_name[i]:AddChild(manor_vipOpen)
		end
	end
end 

--显示选中
function Modules.TempManorDifficulty:OnShow( node )
    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes")
	if self.on then
		self.on:RemoveSelf(true)
		self.on = nil
	end
	self.on = Util:Sprite(sp_1)
	self.on:SetPosition(node:GetContentWidth() , node:GetContentHeight() )
	node:AddChild(self.on)
end


--关闭按钮
function Modules.TempManorDifficulty:OnButtonClose( ... )
	self:DeleteMe()
end

--确定按钮
function Modules.TempManorDifficulty:OnButtonConfirm( ... )
	self.fun(self.index)
	self:DeleteMe()
end

-- -----------------------------------------------------------------------------------------------------------------------------------
-- -----------------------------------------------------------------------------------------------------------------------------------
-- -----------------------------------------------------------------------------------------------------------------------------------
--[[
%% @module: 可能获得道具预览
%% @author: yjg
%% @created: 2016年10月11日10:59:36
--]]

Modules = Modules or {}

Modules.TempManorShotsItem = Modules.TempManorShotsItem or BaseClass(GUI.ScrollItem)

function Modules.TempManorShotsItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)
end

function Modules.TempManorShotsItem:__CreateWidgetFromLuaCode()
	self.layout = Util:Layout()
	self.layout:SetAnchorPoint(0.5, 0.5)
	self.layout:SetContentSize(80, 80)
	return self.layout
end

function Modules.TempManorShotsItem:__delete()
end

function Modules.TempManorShotsItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self.icon = LogicGUI.IconLayer.New(true)
	self.icon:SetPosition(self.layout:GetContentWidth()/2 ,self.layout:GetContentHeight()/2)
	self.icon:SetTouchSwallow(false)
	self.layout:AddChild(self.icon:GetNode())

	local iconData = { id = data.id ,num = data.num }
	self.icon:SetData(iconData)

	local manor_absolute = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR, "manor_absolute",true)
	self.absolute = Util:Sprite(manor_absolute)
	self.absolute:SetAnchorPoint(0.5,0.5)
	self.absolute:SetPosition(self.icon:GetNode():GetContentWidth() - 5 , self.icon:GetNode():GetContentHeight() - 5)
	self.icon:GetNode():AddChild( self.absolute , 999)
	self.absolute:SetVisible(false)

end

function Modules.TempManorShotsItem:__AddToParent()
end
