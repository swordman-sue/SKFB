--[[
%% @module: 天梯任务
%% @author: yjg
%% @created:2017年10月30日15:09:58
--]]


LogicGUI = LogicGUI or {}

LogicGUI.TempLadderTask = LogicGUI.TempLadderTask or BaseClass(GUI.Template)

function LogicGUI.TempLadderTask:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function ()
	    self:__Getwidget()
	    self:__ShowUI()
	end	
	self:InitTemplate(GUI.TemplateRegistry.TEMP_LADDER_TASK) 

    self:BindGlobalEvent(Macros.Event.LadderModule.LADDER_TASK , function()
        local lst = CallModuleFunc(ModuleType.LADDER, "GetTaskCnf")
        self.lst_list:SetDataList( lst )
    end)
end

function LogicGUI.TempLadderTask:__delete()
end

function LogicGUI.TempLadderTask:__Getwidget()
	--背景
	self.img_selectBG = self:GetWidget("img_selectBG") 
	--内容背景
	self.img_dataBG = self:GetWidget("img_dataBG") 
	--光
	self.img_headline = self:GetWidget("img_headline") 
	--关闭按钮
	self.btn_close = Util:WidgetButton( self:GetWidget("btn_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) ) 
end

function LogicGUI.TempLadderTask:__ShowUI()
	--标题
	self.light = Util:Label( CPPGameLib.GetString("ladder_season_task") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite  )
	self.light:SetPosition(self.img_headline:GetContentWidth()/2 , self.img_headline:GetContentHeight()/2)
	self.img_headline:AddChild(self.light)

    local params =  
    {
        item_class = Modules.TempLadderTaskItem,
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
    
    local lst = CallModuleFunc(ModuleType.LADDER, "GetTaskCnf")
    self.lst_list:SetDataList( lst )
end


function LogicGUI.TempLadderTask:OnButtonClose()
	self:DeleteMe()
end








--道具列表
Modules = Modules or {}

Modules.TempLadderTaskItem = Modules.TempLadderTaskItem or BaseClass(GUI.ScrollItem)

function Modules.TempLadderTaskItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
	self:InitTemplate(nil)  
end

function Modules.TempLadderTaskItem:__CreateWidgetFromLuaCode()
	self.layout = Util:Layout(645,134)
	self.layout:SetClippingEnable(false)
	self.layout:SetTouchEnabled(false)
	return self.layout
end

function Modules.TempLadderTaskItem:SetData(data)
  	GUI.ScrollItem.SetData(self, data)

    self:__ShowUI( data )
end

function Modules.TempLadderTaskItem:__ShowUI(data)
    --背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_big_bg")
    local bg = Util:Sprite9(sp)
    bg:SetContentSize( self.layout:GetContentWidth() , self.layout:GetContentHeight() )
    bg:SetAnchorPoint(0.5,0.5)
    self.layout:AddChild(bg) 
    PosTool.Center( bg )  

    --内容
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_equip_attribute")
    local content = Util:Sprite9(sp)
    content:SetContentSize( 388 , 88 )
    content:SetAnchorPoint(0.5,0.5)
    content:SetPosition( 202.68 , 51.57 )
    self.layout:AddChild(content) 

    --任务
    local task = Util:Label( CPPGameLib.GetString("ladder_task_text" , data.task) , Macros.TypefaceSize.normal , Macros.Color.headline )
    task:SetAnchorPoint( 0 , 0)
    task:SetPosition( 5 , content:GetContentHeight() + 5 )
    content:AddChild( task )

    --奖励内容
    local params =  
    {
        item_class = Modules.IconItem,
        item_width = 80,
        item_height = 80,
        row = 1,
        col = 4,
        horizon = true ,
        item_space_c = 15 ,
        view_width = content:GetContentWidth() - 5 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    content:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode() )

    CPPGameLib.PrintTable("xy" , data.lst , "请求物品列表")  
    
    self.lst_list:SetDataList( data.lst )

    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
    self.button = Util:Button( sp  , function( ... )

        local info = {}
        info.win_times = data.task
        CallModuleFunc(ModuleType.LADDER, "TakeKingMatchTaskRewardReq" , info )

    end , nil , Macros.TypefaceSize.popup )

    self.button:SetAnchorPoint( 0.5, 0.5 )
    bg:AddChild(self.button)
    PosTool.Center( self.button , 200 , 0 )

    local effect
    self.effect_id , effect = self:__CreateEffect({res_name = "ui_anniuzoubian2"}, self.button )
    effect:SetPosition( self.button:GetContentWidth()/2 , self.button:GetContentHeight()/2 )


    --场次
    local appearance = CallModuleFunc(ModuleType.LADDER, "GetTaskAppearance")

    local color = Macros.Color.front_color
    if appearance < data.task then
        color = Macros.Color.red_color
    end

    local num = appearance
    if data.task < appearance then
        num = data.task
    end

    self.task = Util:Label( CPPGameLib.GetString("monster_stormcastle_total_hp" , num , data.task) , Macros.TypefaceSize.normal , color )
    self.task:SetAnchorPoint( 0.5 , 0 )
    self.task:SetPosition( self.button:GetPositionX() , self.button:GetPositionY() + 35 )
    bg:AddChild( self.task )


    --图标
    local receive_icon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "receive_icon" , true)
    self.receive_icon = Util:Sprite(receive_icon)  
    self.receive_icon:SetAnchorPoint(0.5,0.5)
    self.receive_icon:SetPosition( self.button:GetPositionX() , self.button:GetPositionY() )
    bg:AddChild( self.receive_icon )


    self:__SetShowType(data)
end

--设置状态
function Modules.TempLadderTaskItem:__SetShowType(data)

    self.task:SetVisible(false)
    self.button:SetVisible(false)
    self.receive_icon:SetVisible(false)
    GlobalEffectMgr:SetEffectVisible(self.effect_id , false)

    local num = CallModuleFunc(ModuleType.LADDER, "GetTaskAppearance")
    local type_ = CallModuleFunc(ModuleType.LADDER, "GetTaskRecordLst" , data.task )

    if num < data.task then
        --置灰
        self.task:SetVisible(true)
        self.button:SetVisible(true)
        Util:GreyButton( self.button )
        self.button:SetText( CPPGameLib.GetString("dungeon_receive") )

    elseif type_ then
        -- 已领取
        self.receive_icon:SetVisible(true)

    else
        --可领取
        local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
        self.button:SetNormalImage( imgdata )
        self.button:SetText( CPPGameLib.GetString("dungeon_receive") )

        self.task:SetVisible(true)
        self.button:SetVisible(true)
        GlobalEffectMgr:SetEffectVisible(self.effect_id , true)

        local r, g, b, a = unpack( Macros.Color.button_yellow )
        self.button:SetOutline(r, g, b, a, 2)
    end

end