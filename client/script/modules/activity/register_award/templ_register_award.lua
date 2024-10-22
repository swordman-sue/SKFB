
--[[
%% @module: 登录奖励
%% @author: yjg
%% @created: 2017年10月20日20:00:29
--]]

Modules = Modules or {}

Modules.TemplLoginReward = Modules.TemplLoginReward or BaseClass(BaseTabView)

function Modules.TemplLoginReward:__init()
end

--选项卡界面创建完成回调
function Modules.TemplLoginReward:__LoadedCallback()
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplLoginReward:__OpenCallback()
	if not CallModuleFunc(ModuleType.ACTIVITY, "GetLoginRewardLst") then
		CallModuleFunc(ModuleType.ACTIVITY, "LoginRewardDataReq")
	else
		self:__SetLst()
	end

    self:BindGlobalEvent(Macros.Event.ActivityModule.REGISTER_AWARD,function (_, data)

		self.vipnum:SetText( tostring( CallModuleFunc(ModuleType.VIP , "GetVipLV") ) )

		local nowExp , upExp = CallModuleFunc(ModuleType.VIP , "GetVipExp")
		self.LoadingBar_loadingBar:SetProgress( nowExp/upExp * 100 )

		self.vipExp:SetString(nowExp.."/"..upExp)

    	self:__SetLst()
    end)

end

--选项卡界面关闭回调
function Modules.TemplLoginReward:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplLoginReward:__Dispose()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

--获得所有组件
function Modules.TemplLoginReward:__Getwidget()
	--内容层
	self.Image_contentLst = self:GetWidget("Image_contentLst")
	--滑动层
	self.Panel_2 = self:GetWidget("Panel_2")
end

function Modules.TemplLoginReward:__ShowUI()
	local params =  
	{
	    item_class = Modules.TemplLoginRewardItem,
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

	local sp = Resource.PathTool.GetUIResPath("activitybig/sp_activity_register_award" , true )
	local level_gift = Util:Sprite( sp )
	level_gift:SetAnchorPoint(0.5,0.5)
	self.Panel_2:AddChild( level_gift )
	PosTool.Center( level_gift )

    --vip图样
    local vip_title_icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "vip_title_icon" , true )
    self.headHint = Util:Sprite(vip_title_icon_sp)
    self.headHint:SetAnchorPoint( 0 , 0.5 )
    self.headHint:SetPosition( 5 , self.Panel_2:GetContentHeight()/2 - 40 )
    self.Panel_2:AddChild(self.headHint) 

    --VIP等级
    local vipnum = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true)
    self.vipnum = GUI.ImgFont.New(vipnum)
    self.vipnum:SetText( tostring( CallModuleFunc(ModuleType.VIP , "GetVipLV") ) )
    PosTool.RightTo( self.headHint , self.vipnum , 5 )
    self.Panel_2:AddChild(self.vipnum:GetNode()) 

    local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_register_award_bag")
    local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_register_award_bagBg")
    --进度条
    self.LoadingBar_loadingBar = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg, 360 , 18, 368 , 25, 100, 100)
    self.LoadingBar_loadingBar:SetAnchorPoint(0.5, 0.5)
    self.Panel_2:AddChild(self.LoadingBar_loadingBar:GetNode())
    PosTool.Center(self.LoadingBar_loadingBar , -20 , -45 )

    local nowExp , upExp = CallModuleFunc(ModuleType.VIP , "GetVipExp")
    self.LoadingBar_loadingBar:SetProgress( nowExp/upExp * 100 )

    --VIP经验
    self.vipExp = Util:Label("00/00",Macros.TypefaceSize.normal)
    self.vipExp:SetPosition(self.LoadingBar_loadingBar:GetNode():GetPositionX() , self.LoadingBar_loadingBar:GetNode():GetPositionY() )
    self.vipExp:SetAnchorPoint(0.5,0.5)
    self.Panel_2:AddChild(self.vipExp)
    --经验
    self.vipExp:SetString(nowExp.."/"..upExp)

end

--设置条目
function Modules.TemplLoginReward:__SetLst()

	local t1 = {}	
	local t2 = {}
	local x = 1
	for k,v in pairs( CPPGameLib.GetConfig("LoginReward", nil , nil , true ) ) do
		local type_ = CallModuleFunc(ModuleType.ACTIVITY, "ToDaysGetLoginReward", k )
		x = x + 1
		if not type_ then
			table.insert( t1 , { days = k , item_num = v.reward_item_num , item_id = v.reward_item_id , extra = v.extra } )
		else
			table.insert( t2 , { days = k , item_num = v.reward_item_num , item_id = v.reward_item_id , extra = v.extra } )
		end
	end

	table.sort( t1, function ( a , b )
		return a.days < b.days
	end )

	table.sort( t2, function ( a , b )
		return a.days < b.days
	end )
	
	for i,v in ipairs(t2) do
		table.insert( t1 , v )
	end

	 
	self.lst_list:SetDataList( t1 )

  --   self.lst_list:ItemDataIter(function(i, item_data)
  --   	local item = self.lst_list:GetItem(i)
		-- if item then
		-- 	item:__SetShowType(item_data)
		-- end 	     
  --   end)
end

function Modules.TemplLoginReward:__GetShowType()
	return Macros.Game.SystemID.REGISTER_AWARD
end




--等级礼包商品
Modules = Modules or {}
Modules.TemplLoginRewardItem = Modules.TemplLoginRewardItem or BaseClass(GUI.ScrollItem)

function Modules.TemplLoginRewardItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACTIVITY_LEVEL_GIFTBAG_ITEM)   
	self:__Getwidget()
end

function Modules.TemplLoginRewardItem:__delete()
end

function Modules.TemplLoginRewardItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self:__SetShowUI( data )
end

function Modules.TemplLoginRewardItem:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--内容框
	self.Image_content = self:GetWidget("Image_content")
	--名字
	self.Text_name = self:GetWidget("Text_name")
	--占位容器
	self.Panel_button = self:GetWidget("Panel_button")	
end

function Modules.TemplLoginRewardItem:__SetShowUI(data)
	--名字
	local activity_ToLevel = Util:Label( CPPGameLib.GetString("activity_register") , Macros.TypefaceSize.largish , Macros.Color.keypoint )
	activity_ToLevel:SetAnchorPoint(0,0.5)
	activity_ToLevel:SetPosition( 0 , self.Text_name:GetContentHeight()/2 )
	self.Text_name:AddChild( activity_ToLevel )

    -- 天数
    local vipnum = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true)
    local viplv = GUI.ImgFont.New(vipnum)
    viplv:SetText(tostring(data.days))
    PosTool.RightTo( activity_ToLevel , viplv , 2 )
    self.Text_name:AddChild(viplv:GetNode()) 
    --天数
    local common_level_suffix = Util:Label( CPPGameLib.GetString("activity_days") , Macros.TypefaceSize.largish , Macros.Color.keypoint )
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

    -- 额外配置
    if data.extra then
	    local extra = Util:Label( " (" .. data.extra .. ") " , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
	    PosTool.RightTo( common_level_suffix  , extra , 10 )

	    self.Text_name:AddChild(extra) 
	end

    local btn_btn_green = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
    self.button = Util:Button( btn_btn_green , function ( )
    	CallModuleFunc(ModuleType.ACTIVITY, "TakeLoginRewardReq" , data)
    end, CPPGameLib.GetString("dungeon_receive") , nil , nil , Macros.Color.button_yellow  )
    self.button:SetAnchorPoint(0.5,0.5)
    self.Panel_button:AddChild( self.button )
    PosTool.Center( self.button )

    local effect
    self.effect_id , effect = self:__CreateEffect({res_name = "ui_anniuzoubian"}, self.button )
	effect:SetPosition( self.button:GetContentWidth()/2 , self.button:GetContentHeight()/2 )

    local receive_icon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "receive_icon" , true)
  	self.receive_icon = Util:Sprite(receive_icon)  
  	self.receive_icon:SetAnchorPoint(0.5,0.5)
    self.Panel_button:AddChild( self.receive_icon )
    PosTool.Center( self.receive_icon )

    self.pvp_schedule =  Util:Label( CPPGameLib.GetString("pvp_schedule") .. "00/00" , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    self.pvp_schedule :SetAnchorPoint( 0.5 , 0.5 )
    self.pvp_schedule :SetPosition( self.Panel_button:GetPositionX() , self.Panel_button:GetPositionY() + 40 )
	self.Image_bg:AddChild( self.pvp_schedule )
   
    self:__SetShowType(data)
end

--设置状态
function Modules.TemplLoginRewardItem:__SetShowType(data)
	self.button:SetVisible(false)
	self.receive_icon:SetVisible(false)

	local day = CallModuleFunc(ModuleType.ACTIVITY,"GetLoginRewardRegisterDay")
	local type_ = CallModuleFunc(ModuleType.ACTIVITY, "ToDaysGetLoginReward",data.days)

	if day < data.days then

		self.pvp_schedule:SetString( CPPGameLib.GetString("pvp_schedule") .. CPPGameLib.GetString("monster_stormcastle_total_hp" , day , data.days )  )

		self.button:SetVisible(true)
		GlobalEffectMgr:SetEffectVisible(self.effect_id , false)
		Util:GreyButton(self.button)

	elseif type_ then

		self.receive_icon:SetVisible(true)
		self.pvp_schedule:SetString( " " )

	elseif day >= data.days then

		self.pvp_schedule:SetString( CPPGameLib.GetString("pvp_schedule") .. CPPGameLib.GetString("monster_stormcastle_total_hp" , data.days , data.days )  )
		
		self.button:SetVisible(true)
		GlobalEffectMgr:SetEffectVisible(self.effect_id , true)

		local r, g, b, a = unpack( Macros.Color.button_yellow )
		self.button:SetOutline(r, g, b, a, 2)
	end
end
