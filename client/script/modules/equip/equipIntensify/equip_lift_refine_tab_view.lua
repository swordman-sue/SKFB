
-- --[[
-- %% @module: 装备精炼界面
-- %% @author: yjg
-- %% @created: 2016年11月5日16:58:56
-- --]]

Modules = Modules or {}

Modules.EquipLiftRefineTabView = Modules.EquipLiftRefineTabView or BaseClass(BaseTabView)

Modules.EquipLiftRefineTabView.SINGLE = 0.1

Modules.EquipLiftRefineTabView.MORE = 0.5

function Modules.EquipLiftRefineTabView:__init()
end

--选项卡界面创建完成回调
function Modules.EquipLiftRefineTabView:__LoadedCallback()
	self.key = true

	self:__Getwidget()
	self:__ShowUI()

	self:ResetAll()
end

--选项卡界面打开回调
function Modules.EquipLiftRefineTabView:__OpenCallback()
	--不用动的UI
	self:Immobility()
	--要动的UI
	self:Change()

	--监听指引下一个步骤事件
	self:BindGlobalEvent(Macros.Event.LeadModule.CHECK_NEXT_SECTION, function(_, pre_section_info)
		self:__CheckSectionOperateSP()
	end)  	

    self:BindGlobalEvent(Macros.Event.EquipModule.EQUIP_REFINE,function (_,data)
    	if data.refine_level ~= data.old_refine_level then
			--要动的UI
			self:Change(data)
		else
			--要动的UI
			self:Change()
		end
    end)

	self:BindGlobalEvent(Macros.Event.PackModule.UPDATE_GOODS,function (_,data)
		self:ResetAll()
	end)
end

--选项卡界面关闭回调
function Modules.EquipLiftRefineTabView:__CloseCallback()
    if self.windwords_effect then
        self.windwords_effect:DeleteMe()
        self.windwords_effect = nil 
    end
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.EquipLiftRefineTabView:__Dispose()
	for i = 1 , #self.faceEffects_1 do
	    if self.faceEffects_1[i] then
			self.faceEffects_1[i]:DeleteMe()
			self.faceEffects_1[i] = nil
		end
	end
end

function Modules.EquipLiftRefineTabView:__Getwidget()
	--图标背景
	self.Image_packBg_0 = self:GetWidget("Image_packBg_0") 
	--名字
	self.Image_nameBg_0 = self:GetWidget("Image_nameBg_0") 
	self.Text_name = self:GetWidget("Text_name") 
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon") 
	--信息框
	self.Image_infoBg_0 = self:GetWidget("Image_infoBg_0") 
	--当前属性
	self.Text_current = self:GetWidget("Text_current") 
	--下级属性
	self.Text_junior = self:GetWidget("Text_junior") 
	--当前属性1
	self.Text_currentValue1 = self:GetWidget("Text_currentValue1") 
	--当前属性2
	self.Text_currentValue2 = self:GetWidget("Text_currentValue2") 
	--下级属性
	self.Text_juniorValue1 = self:GetWidget("Text_juniorValue1") 
	--下级属性
	self.Text_juniorValue2 = self:GetWidget("Text_juniorValue2") 
	--箭头
	self.Image_arrows = self:GetWidget("Image_arrows") 
	--线
	self.Image_1_0_0 = self:GetWidget("Image_1_0_0") 
	--精练等级
	self.Text_refiningLv = self:GetWidget("Text_refiningLv") 
	--进度条
	self.Panel_barBg = self:GetWidget("Panel_barBg") 
	--进度条文字
	self.Text_barValue = self:GetWidget("Text_barValue") 
	--提示
	self.Text_tips = self:GetWidget("Text_tips") 
	--图标框
	self.Panel_iconlst = self:GetWidget("Panel_iconlst") 
    --飘字容器
    self.Panel_upgradeEffect = self:GetWidget("Panel_upgradeEffect")
end
	
function Modules.EquipLiftRefineTabView:__ShowUI()
	-- 武器图标
  	local path = Resource.PathTool.GetBigIconPath(20101)
	self.packIcon = Util:Sprite(path)
    self.packIcon:SetPosition(self.Panel_icon:GetContentWidth()/2 ,self.Panel_icon:GetContentHeight()/2 - 60 )
    self.packIcon:SetAnchorPoint(0.5, 0.5 )
    self.Panel_icon:AddChild(self.packIcon)
    self.packIcon:SetScaleFactor( 0.8 )
    -- 图标沉浮
    ActionManager.GetInstance():UpDownMoveAnimation(self.packIcon , 1 , 15)
    
    -- 名字
	self.name = Util:Name( "  " , Macros.TypefaceSize.largish )
	self.name:SetPosition( self.Text_name:GetContentWidth()/2 , self.Text_name:GetContentHeight()/2 )
	self.name:SetAnchorPoint(0.5,0.5)
	self.Text_name:AddChild(self.name)

	-- 品级
	self.taste = Util:RichText( CPPGameLib.GetString("equip_grade_1" , 0) , Macros.TypefaceSize.largish )
	self.taste:SetAnchorPoint( 0.5 , 1 )
	self.taste:SetPosition( self.Text_name:GetContentWidth()/2 , -20 )
	self.Text_name:AddChild(self.taste)

	--当前属性
	local current = Util:Label( CPPGameLib.GetString("equip_nowProperty") , nil , Macros.Color.headline )
	current:SetAnchorPoint(0.5,0.5)
	current:SetPosition(self.Text_current:GetContentWidth() / 2 , self.Text_current:GetContentHeight() / 2)
	self.Text_current:AddChild(current)

	--下级属性
	local equip_junior2 = Util:Label( CPPGameLib.GetString("equip_nexProperty") , nil , Macros.Color.headline )
	equip_junior2:SetAnchorPoint(0.5,0.5)
	equip_junior2:SetPosition(self.Text_junior:GetContentWidth() / 2 , self.Text_junior:GetContentHeight() / 2)
	self.Text_junior:AddChild(equip_junior2)

	self.property = {}
	self.property.nowType = {}
	self.property.nowValue = {}
	self.property.nexType = {}
	self.property.nexValue = {}

	for i=1,2 do
		--当前类型
	    self.property.nowType[i] = Util:Label("===1" , nil , Macros.Color.keypoint)
	    self.property.nowType[i]:SetAnchorPoint(1,0.5)
	    self.property.nowType[i]:SetPosition(current:GetPositionX() - 2 , self["Text_currentValue"..i]:GetContentHeight()/2 )
	    self["Text_currentValue"..i]:AddChild(self.property.nowType[i])

	    --当前属性值
	    self.property.nowValue[i] = Util:Label("2===" , nil , Macros.Color.content)
	    self.property.nowValue[i]:SetAnchorPoint(0,0.5)
		self.property.nowValue[i]:SetPosition(current:GetPositionX() + 2 , self["Text_currentValue"..i]:GetContentHeight()/2 )
	    self["Text_currentValue"..i]:AddChild(self.property.nowValue[i])

	    --下级类型
	    self.property.nexType[i] = Util:Label("===1" , nil , Macros.Color.keypoint)
	    self.property.nexType[i]:SetAnchorPoint(1,0.5)
	    self.property.nexType[i]:SetPosition(current:GetPositionX() - 2, self["Text_juniorValue"..i]:GetContentHeight()/2 )
	    self["Text_juniorValue"..i]:AddChild(self.property.nexType[i])

	    --下级属性值
	    self.property.nexValue[i] = Util:Label("2===" , nil , Macros.Color.content)
	    self.property.nexValue[i]:SetAnchorPoint(0,0.5)
		self.property.nexValue[i]:SetPosition(current:GetPositionX() + 2, self["Text_currentValue"..i]:GetContentHeight()/2 )
	    self["Text_juniorValue"..i]:AddChild(self.property.nexValue[i])

	end

	local refiningLv = Util:Label( CPPGameLib.GetString("equip_refineLv") , nil , Macros.Color.keypoint )
	refiningLv:SetAnchorPoint(0.5 , 0.5 )
	refiningLv:SetPosition(self.Text_refiningLv:GetContentWidth()/2 ,self.Text_refiningLv:GetContentHeight()/2)
	self.Text_refiningLv :AddChild(refiningLv)

	--当前精练等级
	self.refiningLv = Util:Label( "000" , nil , Macros.Color.content )
	self.refiningLv:SetAnchorPoint(0,0.5)
	self.refiningLv:SetPosition( refiningLv:GetPositionX() + (refiningLv:GetContentWidth()/2) + 5 , refiningLv:GetPositionY()  )
	self.Text_refiningLv :AddChild(self.refiningLv)

	local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_preview")
	local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_interior_bg1")
    --进度条
    self.__bar_exp = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg, self.Panel_iconlst:GetContentWidth() - 5,18, self.Panel_iconlst:GetContentWidth(),23 , 100, 100)
    self.__bar_exp:SetAnchorPoint(0.5, 0.5)
    self.Panel_barBg:AddChild(self.__bar_exp:GetNode())
    PosTool.LeftCenter(self.__bar_exp)

    --进度条值
    self.barValue = Util:Label("00/00",nil,Macros.Color.lilyWhite)
    self.barValue:SetAnchorPoint(0.5,0.5)
    self.barValue:SetPosition(self.__bar_exp:GetNode():GetPositionX() , self.__bar_exp:GetNode():GetPositionY() )
    self.Panel_barBg:AddChild(self.barValue)

    local itemLst = ConfigAdapter.Common.GetRefineItemLst()

	--居中用
    self.iconLayer = Util:Layout((88 + 10) * #itemLst , 76 )
    self.iconLayer:SetAnchorPoint(0.5,0.5)
    self.iconLayer:SetPosition(self.Panel_iconlst:GetContentWidth()/2 , self.Panel_iconlst:GetContentHeight()/2 + 15)
    self.Panel_iconlst:AddChild(self.iconLayer)

    local interval = self.iconLayer:GetContentWidth() / #itemLst
	--图标
	self.icon = {}
    for i,v in ipairs(itemLst) do
    	self:PackIcon( v , 49 + interval * (i-1) , self.iconLayer:GetContentHeight()/2 ,i)
    end

    --提示
    self.equip_aSoftR = Util:Label( "" , Macros.TypefaceSize.slightly , Macros.Color.red_color )
    self.equip_aSoftR:SetAnchorPoint(0.5,0.5)
    self.equip_aSoftR:SetPosition(self.Text_tips:GetContentWidth()/2 , self.Text_tips:GetContentHeight()/2)
    self.Text_tips:AddChild(self.equip_aSoftR)

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
    --一键精炼
    self.btn = Util:Button( sp , 
                                 CPPGameLib.Handler(self, self.OneKeyRefine) , 
                                 CPPGameLib.GetString("equip_oneKeyRefine") , 
                                 Macros.TypefaceSize.button , 
                                 nil , Macros.Color.button_yellow
                                 )
    self.btn:SetPosition( self.Text_tips:GetContentWidth()/2 , self.Text_tips:GetContentHeight()/2 + 5 )
    self.Text_tips:AddChild(self.btn) 
    self.btn:SetVisible( false )    
    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    if role_level >= ConfigAdapter.Common.GetEquipOneKeyRefine() then
    	self.equip_aSoftR:SetVisible( false )
    	self.btn:SetVisible( true )
    end

    --升星
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_up_star",true)
    self.btn_up_star = Util:Button( sp , function( ... )
    	CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.EQUIP_UP_STAR ) 
    end )
    self.btn_up_star:SetPosition( self.Image_nameBg_0:GetPositionX() + self.Image_nameBg_0:GetContentWidth()/2 , self.Image_nameBg_0:GetPositionY() )
    self.btn_up_star:SetAnchorPoint( 0 , 0.5 )
    self.btn_up_star:SetVisible( false )
    self.Image_packBg_0:AddChild(self.btn_up_star) 
    self.btn_up_star:SetScaleFactor( 0.8 )
end

--重置所有
function Modules.EquipLiftRefineTabView:ResetAll()
	--图标计数
	self.packItem = 0
	--选中
	self.onId = 0

	local itemLst = ConfigAdapter.Common.GetRefineItemLst()
	--图标
	for i,v in ipairs(itemLst) do
		local itemNum = CallModuleFunc(ModuleType.Pack, "GetNumByItemID",v)
		--重置数量
		self.icon[v].icon:SetData( {id = v , num = itemNum} )

		self.icon[v].itemNum = itemNum
		
		--触摸层重置
		if itemNum ~= 0 then
		   WidgetUnity.SetWidgetListener(self.icon[v].icon:GetNode(),
			function ( ... )
				if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
					--指引过程中，松手时才触发精炼
					return
				end				
		    	self:OnTouch(v)
		    end,
			function ( pox , poy )
		    	self:OnMoved( v )
		    end,
			function ( ... )
				if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
					--指引过程中，松手时才触发精炼
					self:OnTouch(v)
				end				
				self:OnEnded( "no" )
		    end)
		else
		   WidgetUnity.SetWidgetListener(self.icon[v].icon:GetNode(),nil,nil,
			function ( ... )
				GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, v)
		    end)	
		end
	end
end

--道具图标
function Modules.EquipLiftRefineTabView:PackIcon( itemID , x , y ,i)
	--道具数量
	local itemNum = CallModuleFunc(ModuleType.Pack, "GetNumByItemID",itemID)

	if not self.icon[itemID] then
		self.icon[itemID] = {}
	end

	if not self.icon[itemID].icon then

		--图标
        self.icon[itemID].icon = LogicGUI.IconLayer.New(false)
        self.icon[itemID].icon:SetPosition( x , y) 
        self.icon[itemID].icon:SetAnchorPoint(0.5,0.5)
        self.icon[itemID].icon:SetData( {id = itemID , num = itemNum} )
        self.icon[itemID].icon:SetScaleFactor(0.90)
        self.iconLayer:AddChild(self.icon[itemID].icon:GetNode())

        --id
        self.icon[itemID].itemNum = itemNum

        --增加的经验
		local itemInfo = CPPGameLib.GetConfig("Item",itemID)


		local tab = {
			{zi = CPPGameLib.GetString("equip_exp") , ys = Macros.Color.keypoint , dx = nil },
			{zi = "+" .. itemInfo.param1 , mb = true , ys = itemInfo.quality ,dx = nil ,jg = 5 },
		}
		local lab = Util:LabCenter( tab )

		lab:SetAnchorPoint(0.5,1)
		lab:SetPosition(self.icon[itemID].icon:GetNode():GetContentWidth() / 2 , -15  )
		self.icon[itemID].icon:GetNode():AddChild(lab)

		--装备精炼，精炼石品质（索引，从1开始）
		CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.EQUIP_REFINE, self.icon[itemID].icon:GetNode(), i)                	        	
	end
end

--触摸
function Modules.EquipLiftRefineTabView:OnTouch( id )
	--获得选中的道具的信息
	local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")

	local cnf = CPPGameLib.GetConfig( "EquipmentRefine" , equipInfo.quality )
	local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local lst = CallModuleFunc( ModuleType.EQUIP , "GetMaxRefineInfo" , equipInfo.quality )

	if not cnf.level_list[equipInfo.refine_level] then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("equip_refineMax") )
		return

	elseif equipInfo.refine_level + 1 > lst.refine_level then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_train_level_no") )
		return
	end
	
	self.onId = id
	self.key = true
	self:IconEffects( id )

	if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
		--指引过程中，过滤长按逻辑
		return
	end				

	CPPActionManager.DelayTo( self.iconLayer , Modules.EquipLiftRefineTabView.MORE , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		local func = nil
		local function moveAnimate( ... )
			if self.packItem  < self.icon[id].itemNum then
				if self:JudgeExperience( id ) then
					--点击的特效
					self:IconEffects( id )
					CPPActionManager.DelayTo( self.iconLayer , Modules.EquipLiftRefineTabView.SINGLE , func)
				end
			else
				self:OnEnded()
			end
		end
		func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
		moveAnimate()
	end))
end

--移动
function Modules.EquipLiftRefineTabView:OnMoved( id )
	if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
		--指引过程中，过滤长按逻辑
		return
	end				
	
	local touch = self.icon[id].icon:GetNode():IsHighlighted()
	if not touch then
		self:OnEnded()
	end
end

--松开
function Modules.EquipLiftRefineTabView:OnEnded( key )
	if self.key == true then
		self.key = false
		self:SendDeal()
	end
	CPPActionManager.StopAllActions(self.iconLayer)
end

--不用动的ui
function Modules.EquipLiftRefineTabView:Immobility( ... )
    --获得选中的道具的信息
    local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")

    --设置图标
    local equipinfo_ = CPPGameLib.GetConfig("Equipment", equipInfo.info_id )
    self.packIcon:SetImage(Resource.PathTool.GetBigIconPath(equipinfo_.res_id))

    --背光
    if self.iconId then
        self:__DelEffect(self.iconId )
    end
    local iconLed = nil
    self.iconId , iconLed = TypefaceEffects.New():Spotlight( self , self.Panel_icon , equipInfo.quality )
    iconLed:SetPosition( self.Panel_icon:GetContentWidth()/2 , -215 )
    iconLed:SetScaleFactor(0.8)

end

--需要更改的UI
function Modules.EquipLiftRefineTabView:Change( data )

	local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")

	local info = CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipUpStarInfo", equipInfo.uid )

	local starUpIsOpen = CallModuleFunc(ModuleType.EQUIP, "StarUpIsOpen")
	if starUpIsOpen then
		if info.nexInfo and info.nexInfo.name then
			self.btn_up_star:SetVisible( true )
		else
			self.btn_up_star:SetVisible( false )
		end
	end
	
	local cnf = CPPGameLib.GetConfig( "EquipmentRefine" , equipInfo.quality )
	local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	if cnf.level_list[equipInfo.refine_level] and cnf.level_list[equipInfo.refine_level].need_level > level then
		self.equip_aSoftR:SetString( CPPGameLib.GetString("hero_train_level_no_" , cnf.level_list[equipInfo.refine_level].need_level ) )
	elseif not cnf.level_list[equipInfo.refine_level] then
		self.equip_aSoftR:SetString( CPPGameLib.GetString("equip_refineMax") )
	else
		self.equip_aSoftR:SetString( CPPGameLib.GetString("equip_aSoftR") )
	end

    --获得选中的道具的信息
    local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")	
    --选中的装备的两种上限
    local liftMaxLst = CallModuleFunc(ModuleType.EQUIP, "GetLiftMaxInfo" , equipInfo.quality )
    --将两种上限加进列表
    equipInfo.strengthenMax = liftMaxLst.strengthenMax
    equipInfo.refineMax = liftMaxLst.refineMax

	--精炼属性
    local getRefine , onceRefine , nextRefine = CallModuleFunc(ModuleType.EQUIP, "GetRefinePropPlusMap" , equipInfo.uid )

    if not self.faceEffects_1 then
    	self.faceEffects_1 = {}
    end

    local function Effects( ... )

	    --名字
	    local refine_level = ""
	    if equipInfo.refine_level ~= 0 then
	        refine_level = " +" .. equipInfo.refine_level
	    end
	    self.name:SetString( equipInfo.name .. refine_level )
	    self.name:SetColor(unpack(Macros.Quality[equipInfo.quality]))
	    self.name:SetStrokeColor(unpack(Macros.Quality_Stroke[equipInfo.quality]))
	 
	    self.taste:SetText( CPPGameLib.GetString("equip_grade_1" , equipInfo.taste) )

	    --当前属性
		for i,v in ipairs(getRefine.lst) do
			local nowType , nowValue = Util:Nature(v.type , v.value , true )
			self.property.nowType[i]:SetString( nowType .. " ")
			--属性1
			self.faceEffects_1[i] = TypefaceEffects.New()
	        self.faceEffects_1[i]:Roll( self.property.nowValue[i], nowValue , nil , function ( ... )
	        	if self.faceEffects_1[i] then
	        		self.faceEffects_1[i]:DeleteMe()
	        		self.faceEffects_1[i] = nil
	        	end
	        end)
		end

		--下级属性
		for i,v in ipairs(nextRefine.lst) do
			local nextType , nextValue = Util:Nature(v.type , v.value , true )
			if getRefine.level >= equipInfo.refineMax then
				self.property.nexType[i]:SetString(nextType.." ")
				self.property.nexValue[i]:SetString( CPPGameLib.GetString("equip_toMax") )
			else
				self.property.nexType[i]:SetString(nextType.." ")
				self.property.nexValue[i]:SetString(nextValue)
			end
		end

		local exp = CallModuleFunc(ModuleType.EQUIP, "GetUpgrade" , equipInfo.uid , equipInfo.refine_level )

		--进度条
		self.barValue:SetString( equipInfo.refine_exp .. "/" .. exp )

		if type then
			self.__bar_exp:SetProgress( equipInfo.refine_exp / exp * 100 )
		else
			self.__bar_exp:SetToProgress( equipInfo.refine_exp / exp * 100 )
		end

		local lst = CallModuleFunc( ModuleType.EQUIP , "GetMaxRefineInfo" , equipInfo.quality )
		--等级
		self.refiningLv:SetString( equipInfo.refine_level .." / " .. lst.refine_level )

    end

    if data then
		LogicGUI.TempMonster.DisposeData(data,"equipRefine")

	    --特效
	    local pos = {
	        [1] = { x = self.Text_refiningLv:GetPositionX() , y = self.Text_refiningLv:GetPositionY() },
	        [2] = { x = self.Text_currentValue1:GetPositionX() , y = self.Text_currentValue1:GetPositionY() },
	        [3] = { x = self.Text_currentValue2:GetPositionX() , y = self.Text_currentValue2:GetPositionY() }
	    }
	    local t1 = LogicGUI.EquipmentRefineFontEffects:GetEquipmentRefineTab( data , pos )
		self.windwords_effect = WindWordsEffects.New()
		self.windwords_effect:Subsection( self.__root_widget , self.Image_infoBg_0 , t1 , 10 ,function ( ... )
			Effects()
		end)
	else
	    --当前属性
		for i,v in ipairs(getRefine.lst) do
			local nowType , nowValue = Util:Nature(v.type , v.value , true )
			self.property.nowType[i]:SetString( nowType .. " ")
			self.property.nowValue[i]:SetString( nowValue )
		end

		Effects()
	end
end

--更新界面
function Modules.EquipLiftRefineTabView:__SetShowUI()
    if self.windwords_effect then
        self.windwords_effect:DeleteMe()
        self.windwords_effect = nil 
    end

	self:Immobility()
	self:ResetAll()

	--要动的UI
	self:Change()
end

--判断经验发送
function Modules.EquipLiftRefineTabView:JudgeExperience( id )
	local itemType_ , itemExp = ObjIDTool.GetInfo( id )
	itemExp = itemExp.param1

    --获得选中的道具的信息
    local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")

	local refine_exp = equipInfo.refine_exp + itemExp

	local exp = CallModuleFunc(ModuleType.EQUIP, "GetUpgrade" , equipInfo.uid , equipInfo.refine_level )
	if refine_exp < exp then
		return true
	elseif equipInfo.refine_exp > exp then
		self:OnEnded()
	else
		self:IconEffects(id)
		self:OnEnded()
	end

end

--图标特效
function Modules.EquipLiftRefineTabView:IconEffects( id )
	--计数+1
	self.packItem = self.packItem + 1

	--准备移动的图标
	local icon = LogicGUI.IconLayer.New(false)
	icon:SetPosition( 
						self.icon[id].icon:GetNode():GetPositionX() , 
						self.icon[id].icon:GetNode():GetPositionY() + 20 
					) 
	icon:SetAnchorPoint(0.5,0.5)
	icon:SetData( {id = id } )
	icon:SetScaleFactor(0.90)
	icon:ShowIconBG(false)
	icon:ShowIconQuality(false)
	self.Panel_iconlst:AddChild(icon:GetNode())
	
	--特效
	CPPActionManager.MoveTo( icon:GetNode(), 0.2 , self.Panel_barBg:GetPositionX() , self.Panel_barBg:GetPositionY() - 75)
	CPPActionManager.ScaleTo( icon:GetNode(), 0.2  , 0.1 )
	CPPActionManager.RotateBy(icon:GetNode() , 0.2  , 360 * 10  )
	CPPActionManager.FadeOut(icon:GetNode(), 0.2 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		--删除
    	icon:RemoveSelf(true)
    	icon = nil

		local itemType_ , itemExp = ObjIDTool.GetInfo( id )
		itemExp = itemExp.param1

		--飘增加的经验
		local lab = TypefaceEffects:LittleFloat(  string.format( Macros.Color.green_stroke_rt , "+" .. itemExp )  , nil , 1 , 0 , 40 , nil , nil , nil , Macros.TypefaceSize.largish )
		lab:SetPosition(self.Panel_barBg:GetContentWidth()/2 , self.Panel_barBg:GetContentHeight()/2 )
		self.Panel_barBg:AddChild(lab)

		--飘成功
		local effect_id, effect_con = self:__CreateEffect({id = 100027 , loop = false }, self.Panel_barBg )
		PosTool.Center(effect_con)	
		effect_con:SetZOrder(999)

	end) )

	--点击特效
	local effect_id, effect_con = self:__CreateEffect({res_name = "UI_zhuangbeijingliandianji" , loop = false }, self.icon[id].icon:GetNode() )
	PosTool.Center(effect_con , - 5)	
	effect_con:SetZOrder(999)

	--设置道具数量
	self:SetItemNum( id )
end

--设置道具数量
function Modules.EquipLiftRefineTabView:SetItemNum( id )
	self.icon[id].icon:SetNum( self.icon[id].itemNum - self.packItem ) 

    local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")

	local itemType_ , itemExp = ObjIDTool.GetInfo( id )
	itemExp = itemExp.param1
	equipInfo.refine_exp = equipInfo.refine_exp + itemExp

	--进度条
	local exp = CallModuleFunc(ModuleType.EQUIP, "GetUpgrade" , equipInfo.uid , equipInfo.refine_level )
	self.barValue:SetString( equipInfo.refine_exp .. "/" .. exp )
	self.__bar_exp:SetToProgress( equipInfo.refine_exp / exp * 100 )
end

--发送协议
function Modules.EquipLiftRefineTabView:SendDeal()
	-- 2495 炒作异常，一下一下点精炼
	if not self.onId or not self.packItem or self.onId == 0 or self.packItem == 0 then
		return 
	end

	local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
	--发送协议
	local info = {}
	info.uid = equipInfo.uid
	info.cost_item_list = {}
	info.cost_item_list[1] = {}
	info.cost_item_list[1].item_id = self.onId
	info.cost_item_list[1].item_num = self.packItem

	CallModuleFunc(ModuleType.EQUIP, "EquipMentRefineReq",info)
end

function Modules.EquipLiftRefineTabView:__CheckSectionOperateSP()
    local itemLst = ConfigAdapter.Common.GetRefineItemLst()
	for i, v in ipairs(itemLst) do
		if self.icon[v] then
			--装备精炼，精炼石品质（索引，从1开始）
			CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.EQUIP_REFINE, self.icon[v].icon:GetNode(), i)                	        	
		end
	end
end

function Modules.EquipLiftRefineTabView:OneKeyRefine()
	local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
	LogicGUI.TemplEquipOnekeyRefine.New( equipInfo.uid ,function ( level , lst )

		local t1 = {}
		for i , v in pairs( lst ) do
			if v.num ~= 0 then
				table.insert( t1 , { item_Id = v.id , item_Num = v.num } )
			end
		end

		if #t1 <= 0 then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_noSelectLevl"))
			return
		end

		local level_max = CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipRefineMax" , equipInfo.uid)

		if level_max == 0 then
		    LogicGUI.TemplAffirmTips.New({
		    	type = LogicGUI.TemplAffirmTipsType.YES_NO,
		        content = CPPGameLib.GetString("equip_insufficientUP") ,
		        confirm_text = CPPGameLib.GetString("common_confirm") , 
				confirm_func = function ( ... )
					--发送协议
					local info = {}
					info.uid = equipInfo.uid
					info.cost_item_list = {}
					for i , v in ipairs( t1 ) do
						info.cost_item_list[i] = {}
						info.cost_item_list[i].item_id = v.item_Id
						info.cost_item_list[i].item_num = v.item_Num
					end
					CallModuleFunc(ModuleType.EQUIP, "EquipMentRefineReq",info)
				end,
		    })
		else
			if level <= 0 then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_noSelectLevl"))
				return 
			end
			--发送协议
			local info = {}
			info.uid = equipInfo.uid
			info.cost_item_list = {}
			for i , v in ipairs( t1 ) do
				info.cost_item_list[i] = {}
				info.cost_item_list[i].item_id = v.item_Id
				info.cost_item_list[i].item_num = v.item_Num
			end
			CallModuleFunc(ModuleType.EQUIP, "EquipMentRefineReq",info)
		end
	end)
	
end
