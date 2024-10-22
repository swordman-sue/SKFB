--[[
%% @module: 晋级奖励
%% @author: yjg
%% @created: 2017年10月30日22:44:57
--]]


LogicGUI = LogicGUI or {}

LogicGUI.TempLadderPromotion = LogicGUI.TempLadderPromotion or BaseClass(GUI.Template)

function LogicGUI.TempLadderPromotion:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function ()
	    self:__Getwidget()
	    self:__ShowUI()
	end	
	self:InitTemplate(GUI.TemplateRegistry.TEMP_LADDER_PROMOTION) 
end

function LogicGUI.TempLadderPromotion:__delete()
end

function LogicGUI.TempLadderPromotion:__Getwidget()
	--背景
	self.img_selectBG = self:GetWidget("img_selectBG") 
	--内容背景
	self.img_dataBG = self:GetWidget("img_dataBG") 
	--光
	self.img_headline = self:GetWidget("img_headline") 
	--关闭按钮
	self.btn_close = Util:WidgetButton( self:GetWidget("btn_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) ) 
end

function LogicGUI.TempLadderPromotion:__ShowUI()
	--标题
	self.light = Util:Label( CPPGameLib.GetString("ladder_promotionAward") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite  )
	self.light:SetPosition(self.img_headline:GetContentWidth()/2 , self.img_headline:GetContentHeight()/2)
	self.img_headline:AddChild(self.light)

    local params =  
    {
        item_class = Modules.TempLadderPromotionItem,
        item_width = 645,
        item_height = 134 ,
        row = 4,
        col = 1,
        item_space_r = 5 ,
        -- view_width = self.img_dataBG:GetContentWidth() - 5 ,
        view_height = self.img_dataBG:GetContentHeight() - 5 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.img_dataBG:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode() )

    local lst = CallModuleFunc(ModuleType.LADDER, "GetPromotionCnf")
    self.lst_list:SetDataList( lst )

    local name = Util:Label( CPPGameLib.GetString("ladder_nowDan") , Macros.TypefaceSize.normal , Macros.Color.keypoint  )
    name:SetAnchorPoint( 0 , 0.5 )
    name:SetPosition( 10 , 23 )
    self.img_selectBG:AddChild(name)

    local myRank = CallModuleFunc(ModuleType.LADDER, "GetMyRank")

    local text = ""
    if myRank <= 0 then
        text = CPPGameLib.GetString("rebel_meimingci")
    else
        local cnf = CPPGameLib.GetConfig("LadderDan", myRank )
        text = cnf.name
    end

    local value = Util:Label( text , Macros.TypefaceSize.normal , Macros.Color.content  )
    self.img_selectBG:AddChild(value)
    PosTool.RightTo( name , value )

end


function LogicGUI.TempLadderPromotion:OnButtonClose()
	self:DeleteMe()
end









--道具列表
Modules = Modules or {}

Modules.TempLadderPromotionItem = Modules.TempLadderPromotionItem or BaseClass(GUI.ScrollItem)

function Modules.TempLadderPromotionItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
	self:InitTemplate(nil)  
end

function Modules.TempLadderPromotionItem:__CreateWidgetFromLuaCode()
	self.layout = Util:Layout(645,134)
	self.layout:SetClippingEnable(false)
	self.layout:SetTouchEnabled(false)
	return self.layout
end

function Modules.TempLadderPromotionItem:SetData(data)
  	GUI.ScrollItem.SetData(self, data)

    self:__ShowUI( data )
end

function Modules.TempLadderPromotionItem:__ShowUI(data)
    --背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_big_bg")
    local bg = Util:Sprite9(sp)
    bg:SetContentSize( self.layout:GetContentWidth() , self.layout:GetContentHeight() )
    bg:SetAnchorPoint(0.5,0.5)
    self.layout:AddChild(bg) 
    PosTool.Center( bg )  

    --奖励内容
    local params =  
    {
        item_class = Modules.IconItem,
        item_width = 80,
        item_height = 80,
        row = 1,
        col = 4,
        horizon = true ,
        item_space_c = 5 ,
        view_width = 340 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    bg:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode() , -40 )

    self.lst_list:SetDataList( data.award )

    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
    self.button = Util:Button( sp  , nil , nil , Macros.TypefaceSize.popup )
    self.button:SetAnchorPoint( 0.5, 0.5 )
    bg:AddChild(self.button)
    PosTool.Center( self.button , 230 , 0 )

    local effect
    self.effect_id , effect = self:__CreateEffect({res_name = "ui_anniuzoubian2"}, self.button )
    effect:SetPosition( self.button:GetContentWidth()/2 , self.button:GetContentHeight()/2 )

    --图标
    local receive_icon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "receive_icon" , true)
    self.receive_icon = Util:Sprite(receive_icon)  
    self.receive_icon:SetAnchorPoint(0.5,0.5)
    self.receive_icon:SetPosition( self.button:GetPositionX() , self.button:GetPositionY() )
    bg:AddChild( self.receive_icon )


    local imageData = Resource.PathTool.GetLadderIconPath(data.icon)
    local icon = Util:Sprite(imageData)
    icon:SetAnchorPoint(0.5,0.5)
    icon:SetPosition( self.layout:GetContentWidth() / 2 - 260 , self.layout:GetContentHeight() / 2 + 20 )
    icon:SetScaleFactor( 0.5 )
    self.layout:AddChild(icon)

    local dan = Util:Dan( data.id , data.star )
    dan:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild( dan )
    dan:SetPosition( icon:GetPositionX() , 25 )

    self:__SetShowType(data)
end

--设置状态
function Modules.TempLadderPromotionItem:__SetShowType(data)

    self.button:SetVisible(false)
    self.receive_icon:SetVisible(false)
    
    local level = CallModuleFunc( ModuleType.LADDER , "GetMyRank" )
    if level < data.id then 
        --置灰
        self.button:SetText( CPPGameLib.GetString("ladder_underage") )
        self.button:SetVisible(true)
        GlobalEffectMgr:SetEffectVisible(self.effect_id , false)
        Util:GreyButton( self.button )
    else
        --已领取
        self.receive_icon:SetVisible(true)
    end

end





