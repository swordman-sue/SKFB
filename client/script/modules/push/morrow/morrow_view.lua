--
-- @brief 次日登陆
-- @author: yjg
-- @date: 2017年7月15日15:36:49
--

Modules = Modules or {}

Modules.MorrowView = Modules.MorrowView or BaseClass(BaseView)

function Modules.MorrowView:__init( )
    self.__layout_name = "templ_morrow"  
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self:__Getwidget()
        self:__ShowUI( id )
        self:CountDown( )
    end

    self.chat_cd =  self:BindGlobalEvent(Macros.Event.MorrowModule.MORROW_AWARD,function (_,data)
        -- self:OnButtonClose()
    end)

    -- self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.MORROW)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.MORROW,true)
end

function Modules.MorrowView:__Dispose()
    if self.chongzhiEffect_id then
        self:__DelEffect(self.chongzhiEffect_id)
        self.chongzhiEffect_id = nil
    end
end

function Modules.MorrowView:__Getwidget( ... )
    -- 内容背景
    self.Image_morrow = self:GetWidget("Image_morrow") 

    --时间框
    self.Image_frame = self:GetWidget("Image_frame")

    --按钮
    self.Button_btn = Util:WidgetButton(  
                                            self:GetWidget("Button_btn") , 
                                            "" , 
                                            CPPGameLib.Handler(self , self.OnButtonClose) ,
                                            nil,nil,Macros.Color.button_yellow
                                            ) 

    self.Image_btn = self:GetWidget("Image_btn")

    -- self.Panel_balance = {}
    -- self.Panel_attack = {}
    -- self.Panel_defense = {}
    -- for i = 1 , 4 do

    --     self.Panel_balance[i] = self:GetWidget("Panel_balance_"..i)
    --     ActionManager.GetInstance():UpDownMoveAnimation(self.Panel_balance[i], MathTool.GetRandom(2, 5) , MathTool.GetRandom(5, 10) )

    --     self.Panel_attack[i] = self:GetWidget("Panel_attack_"..i)
    --     ActionManager.GetInstance():UpDownMoveAnimation(self.Panel_attack[i], MathTool.GetRandom(2, 5) , MathTool.GetRandom(5, 10) )

    --     self.Panel_defense[i] = self:GetWidget("Panel_defense_"..i)
    --     ActionManager.GetInstance():UpDownMoveAnimation(self.Panel_defense[i], MathTool.GetRandom(2, 5) , MathTool.GetRandom(5, 10) )

    -- end
end

function Modules.MorrowView:__ShowUI(  )


    CallModuleFunc(ModuleType.MORROW,"GetEndTime")
    self.endTime = Util:Label( CPPGameLib.GetString("morrow_time" , "00:00:00") , Macros.TypefaceSize.normal , Macros.Color.white )
    self.endTime:SetPosition( self.Image_frame:GetContentWidth()/2 , self.Image_frame:GetContentHeight()/2 )
    self.Image_frame:AddChild( self.endTime )

    local BtnEffect_con = nil
    self.chongzhiEffect_id , BtnEffect_con = self:__CreateEffect({res_name = "UI_chongzhitongyong"},self.Button_btn,999)
    BtnEffect_con:SetPosition(self.Button_btn:GetContentWidth()/2 , self.Button_btn:GetContentHeight()/2 )
    GlobalEffectMgr:SetEffectVisible(self.chongzhiEffect_id , false)
end

function Modules.MorrowView:OnButtonClose()
    if unpack(self.__open_data) == "push" then
        self:CloseManual(true)
        CallModuleFunc(ModuleType.PUSH,"Transfer",Macros.Game.SystemID.MORROW)
    else
        self:CloseManual()
    end
end

function Modules.MorrowView:CountDown()
    local endTime = CallModuleFunc(ModuleType.MORROW,"GetEndTime")
    
    local func = nil
    local function moveAnimate( ... )
        --现在时间
        local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        local time = endTime - server_time
        if time <= 0 then
            local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "draw" , true)
            self.Image_btn:SetImage( sp )
            self.Image_btn:SetContentSize( 77 , 31 )
            self.endTime:SetString( CPPGameLib.GetString("morrow_time" , TimeTool.TransTimeStamp( "HH:MM:SS" , 0 ) ) )
            self.Image_frame:SetVisible(false)

            WidgetUnity.SetWidgetListener( self.Button_btn , nil, nil, function()
                self:OnButtonBtn()
            end, nil, 0.5 * 1000)

            GlobalEffectMgr:SetEffectVisible(self.chongzhiEffect_id , false)

        else
            self.endTime:SetString( CPPGameLib.GetString("morrow_time" , TimeTool.TransTimeStamp( "HH:MM:SS" , time ) ) )
            CPPActionManager.DelayTo(self.Image_morrow , 1 , func)
        end
    end      
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end

function Modules.MorrowView:OnButtonBtn()
    self:OnButtonClose()
    CallModuleFunc(ModuleType.MORROW,"NextDayRewardTakeReq")
end