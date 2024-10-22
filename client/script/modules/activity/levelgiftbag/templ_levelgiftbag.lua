
--[[
%% @module: 等级礼包
%% @author: yjg
%% @created: 2017年3月20日11:17:34
--]]
-- level_gift
-- Panel_2

Modules = Modules or {}

Modules.TemplLevelGiftBag = Modules.TemplLevelGiftBag or BaseClass(BaseTabView)

function Modules.TemplLevelGiftBag:__init()
end

--选项卡界面创建完成回调
function Modules.TemplLevelGiftBag:__LoadedCallback()
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplLevelGiftBag:__OpenCallback()
	if not CallModuleFunc(ModuleType.ACTIVITY, "GetLevelGiftType") then
		CallModuleFunc(ModuleType.ACTIVITY, "LevelGiftDataReq")
	else
		self:__SetLst()
	end

    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_LEVEL_GIFTBAG,function (_, data)
    	self:__SetLst()
    end)

end

--选项卡界面关闭回调
function Modules.TemplLevelGiftBag:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplLevelGiftBag:__Dispose()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

--获得所有组件
function Modules.TemplLevelGiftBag:__Getwidget()
	--内容层
	self.Image_contentLst = self:GetWidget("Image_contentLst")
	--滑动层
	self.Panel_2 = self:GetWidget("Panel_2")
end

function Modules.TemplLevelGiftBag:__ShowUI()
	local params =  
	{
	    item_class = Modules.TemplLevelGiftBagItem,
	    item_width = 660.00,
	    item_height = 143.00 ,
	    item_space_c = 0 , 
	    row = 3,
	    col = 1,
	    view_height = self.Image_contentLst:GetContentHeight() - 5 ,
	}
	-- 创建参数
	self.lst_list = GUI.ScrollView.New(params)
	self.Image_contentLst:AddChild(self.lst_list:GetNode())
	PosTool.Center(self.lst_list:GetNode())

    local sp = Resource.PathTool.GetUIResPath("activitybig/level_gift" , true )
	local level_gift = Util:Sprite( sp )
	level_gift:SetAnchorPoint(0.5,0.5)
	self.Panel_2:AddChild( level_gift )
	PosTool.Center( level_gift )
    
end

--设置条目
function Modules.TemplLevelGiftBag:__SetLst()

	local t1 = {}	
	local t2 = {}
	for k,v in pairs( CPPGameLib.GetConfig("LevelGift",nil,nil,true) ) do
		local type_ = CallModuleFunc(ModuleType.ACTIVITY, "ToLevelGetLevelGiftType",v.level)
		if not type_ then
			table.insert( t1 , {level = v.level , item_num = v.item_num_list , item_id = v.item_id_list} )
		else
			table.insert( t2 , {level = v.level , item_num = v.item_num_list , item_id = v.item_id_list} )
		end
	end

	table.sort( t1, function ( a , b )
		return a.level < b.level
	end )
	table.sort( t2, function ( a , b )
		return a.level < b.level
	end )

	for i,v in ipairs(t2) do
		table.insert( t1 , v )
	end

	self.lst_list:SetDataList( t1 )

    self.lst_list:ItemDataIter(function(i, item_data)
    	local item = self.lst_list:GetItem(i)
		if item then
			item:__SetShowType(item_data)
		end 	     
    end)

    --新手指引特殊操作检测
	local operate_id, data_id = CallModuleFunc(ModuleType.LEAD, "GetOperateInfo")
	if operate_id == Macros.Game.LeadOperateSpecialID.ACTIVITY_GET_REWARD then				
		local locate_index
		self.lst_list:ItemDataIter(function(item_index, item_data)
			if item_data.level == data_id then
				locate_index = item_index
				return true
			end
		end)				
		if locate_index then
			self.lst_list:LocateItem(locate_index, true)

			--列表相关调整
			local item = self.lst_list:GetItem(locate_index)
			if item then
				item:CheckSectionOperateSP()
			end
		end
	end	     
end



--等级礼包商品
Modules = Modules or {}
Modules.TemplLevelGiftBagItem = Modules.TemplLevelGiftBagItem or BaseClass(GUI.ScrollItem)

function Modules.TemplLevelGiftBagItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACTIVITY_LEVEL_GIFTBAG_ITEM)   
	self:__Getwidget()
end

function Modules.TemplLevelGiftBagItem:__delete()
end

function Modules.TemplLevelGiftBagItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self:__SetShowUI( data )
end

function Modules.TemplLevelGiftBagItem:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--内容框
	self.Image_content = self:GetWidget("Image_content")
	--名字
	self.Text_name = self:GetWidget("Text_name")
	--占位容器
	self.Panel_button = self:GetWidget("Panel_button")	
end

function Modules.TemplLevelGiftBagItem:__SetShowUI(data)
	--名字
	local activity_ToLevel = Util:Label( CPPGameLib.GetString("activity_ToLevel") , Macros.TypefaceSize.largish , Macros.Color.keypoint )
	activity_ToLevel:SetAnchorPoint(0,0.5)
	activity_ToLevel:SetPosition( 0 , self.Text_name:GetContentHeight()/2 )
	self.Text_name:AddChild( activity_ToLevel )

    -- VIP等级
    local vipnum = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true)
    local viplv = GUI.ImgFont.New(vipnum)
    viplv:SetText(tostring(data.level))
    PosTool.RightTo( activity_ToLevel , viplv , 2 )
    self.Text_name:AddChild(viplv:GetNode()) 

    --VIP等级
    local common_level_suffix = Util:Label( CPPGameLib.GetString("common_level_suffix") , Macros.TypefaceSize.largish , Macros.Color.keypoint )
    PosTool.RightTo( viplv , common_level_suffix , 2 )
    self.Text_name:AddChild(common_level_suffix) 

    --物品
    local x = 5
    for i,v in ipairs(data.item_id) do
		local icon = LogicGUI.IconLayer.New(true)
		icon:SetData( { id = v , num = data.item_num[i] } )
		icon:SetPosition( x , self.Image_content:GetContentHeight()/2 ) 
		icon:SetAnchorPoint( 0 , 0.5 )
		self.Image_content:AddChild( icon:GetNode() )
		icon:SetTouchSwallow( false )
		x = x + icon:GetNode():GetContentWidth() + 5
    end

    local btn_btn_green = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
    self.button = Util:Button( btn_btn_green , function ( )
    	CallModuleFunc(ModuleType.ACTIVITY, "TakeLevelGiftRewardReq",data)
    end, CPPGameLib.GetString("dungeon_receive")  , nil , nil , nil )

    self.button:SetAnchorPoint(0.5,0.5)
    self.Panel_button:AddChild( self.button )
    PosTool.Center( self.button )

    local effect
    self.effect_id , effect = self:__CreateEffect({res_name = "ui_anniuzoubian"}, self.button )
	effect:SetPosition( self.button:GetContentWidth()/2 , self.button:GetContentHeight()/2 + 2 )

    local receive_icon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "receive_icon" , true)
  	self.receive_icon = Util:Sprite(receive_icon)  
  	self.receive_icon:SetAnchorPoint(0.5,0.5)
    self.Panel_button:AddChild( self.receive_icon )
    PosTool.Center( self.receive_icon )

    self:__SetShowType(data)
end

--设置状态
function Modules.TemplLevelGiftBagItem:__SetShowType(data)
	self.button:SetVisible(false)
	self.receive_icon:SetVisible(false)

	local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local type_ = CallModuleFunc(ModuleType.ACTIVITY, "ToLevelGetLevelGiftType",data.level)

	if roleLevel < data.level then

		self.button:SetVisible(true)
		GlobalEffectMgr:SetEffectVisible(self.effect_id , false)
		Util:GreyButton(self.button)

	elseif type_ then

		self.receive_icon:SetVisible(true)

	elseif roleLevel >= data.level then

		self.button:SetVisible(true)
		GlobalEffectMgr:SetEffectVisible(self.effect_id , true)

		local r, g, b, a = unpack( Macros.Color.button_yellow )
		self.button:SetOutline(r, g, b, a, 2)

	end
end

--新手指引特殊操作检测
function Modules.TemplLevelGiftBagItem:CheckSectionOperateSP()
	--日常活动领取等级礼包（礼包ID）
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.ACTIVITY_GET_REWARD, self.button, self.__data.level)                				
end