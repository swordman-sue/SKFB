
-- @brief 奖励预览 
-- @author: yjg
-- @date: 2018年11月10日10:05:29
--
LogicGUI = LogicGUI or {}

LogicGUI.TempSuperDiscountAward = LogicGUI.TempSuperDiscountAward or BaseClass(GUI.Template)

function LogicGUI.TempSuperDiscountAward:__init(cnf , func , text )
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self.cnf = cnf
        self:__Getwidget( )
        self:__ShowUI()
    end
    self:InitTemplate(GUI.TemplateRegistry.DUNGEON_BOXINFO)
end

function LogicGUI.TempSuperDiscountAward:__delete()
    if self.lst_list then 
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

function LogicGUI.TempSuperDiscountAward:__Getwidget( text )

	self.Panel_boxInfo = self:GetWidget("Panel_boxInfo") 
    --标题
    self.Text_Title = Util:WidgetLabel(     
                                        self:GetWidget("Text_Title"), 
                                        CPPGameLib.GetString("union_constructionPreview"), 
                                        Macros.TypefaceSize.popup, 
                                        Macros.Color.white
                                    )
    --不要的
    self.Text_GameLevelCondition = self:GetWidget("Text_GameLevelCondition") 
    self.Text_GameLevelCondition:SetVisible(false)
    self.Panel_StarCondition = self:GetWidget("Panel_StarCondition") 
    self.Panel_StarCondition:SetVisible(false)
    self.ScrollView_Rewards = self:GetWidget("ScrollView_Rewards") 
    self.ScrollView_Rewards:SetVisible(false)

    --内容
    self.Image_9 = self:GetWidget("Image_9") 
    
    self.Button_Close =  Util:WidgetButton( self:GetWidget("Button_Close") , " " , CPPGameLib.Handler(self,self.OnBtnClose) )
    self.Button_Receive =  Util:WidgetButton( 
                                                self:GetWidget("Button_Receive") , 
                                                CPPGameLib.GetString("common_confirm") , 
                                                CPPGameLib.Handler(self,self.OnBtnClose) ,
                                                nil,nil,Macros.Color.button_red
                                                )
    self:SetBtn()
end

--按钮设置
function LogicGUI.TempSuperDiscountAward:SetBtn()
	local buyTime = CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW, "GetCurBuyTime" )
    local rewardTimes = CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW, "GetRewardTimes" , self.cnf.times )
    if self.cnf.need_buy_times <= buyTime and rewardTimes == 0 then
        self.Button_Receive:SetText( CPPGameLib.GetString("manor_get") )
        WidgetUnity.SetWidgetListener( self.Button_Receive , nil, nil, function( ... )
            self:OnButtonReceive()
        end, nil, 0.5*1000)
    end
end

-- 2017年5月8日16:55:09
-- 大硕硕说最多四个
function LogicGUI.TempSuperDiscountAward:__ShowUI()
	
	local SuperDiscountTimeBuy = Util:Label( CPPGameLib.GetString("SuperDiscountTimeBuy" , self.cnf.need_buy_times ) , nil , Macros.Color.content  )
	SuperDiscountTimeBuy:SetPosition( self.Text_GameLevelCondition:GetPositionX() , self.Text_GameLevelCondition:GetPositionY() )
	self.Panel_boxInfo:AddChild( SuperDiscountTimeBuy )

    local layou = Util:Layout( self.Image_9:GetContentWidth() - 20 , self.Image_9:GetContentHeight() )
    layou:SetAnchorPoint(0.5,0.5)
    layou:SetPosition(self.Image_9:GetContentWidth()/2 ,self.Image_9:GetContentHeight()/2 )
    self.Image_9:AddChild(layou)

    local interval = layou:GetContentWidth() / 4
    --图标
    for i,v in ipairs(self.cnf.item_id_list) do
        local icon = LogicGUI.IconLayer.New(true)
        icon:SetData({ id = v , num = self.cnf.item_num_list[i] })
        icon:SetPosition( (interval/2) + interval * (i - 1)  , layou:GetContentHeight()/2 + 20 )
        icon:SetAnchorPoint(0.5,0.5)
        layou:AddChild(icon:GetNode())

        local type_ , info_ = ObjIDTool.GetInfo(v)
        local name = Util:Name( info_.name , Macros.TypefaceSize.slightly ,info_.quality , nil , interval )
        name:SetAnchorPoint( 0.5 , 1 )
        name:SetPosition( icon:GetNode():GetPositionX() , 55 )
        layou:AddChild(name)
    end
end

function LogicGUI.TempSuperDiscountAward:OnBtnClose()
    self:DeleteMe()
end

function LogicGUI.TempSuperDiscountAward:OnButtonReceive()
    local info = {}
    info.times = self.cnf.times
    CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW , "SuperDiscounitAccumulateRewardReq" , info )
    self:OnBtnClose()
end