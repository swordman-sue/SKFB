
--
-- @brief 选择界面
-- @author: yjg
-- @date: 2016年11月2日20:14:27
--
Modules = Modules or {}

Modules.TemplDebrisellScreen = Modules.TemplDebrisellScreen or BaseClass(GUI.Template)

function Modules.TemplDebrisellScreen:__init( lst , func )

    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
        
    self.__open_callback = function ()
        self.lst = lst
        self.func = func
        self:__Getwidget()
        self:__ShowUI()
    end 

    self:InitTemplate(GUI.TemplateRegistry.SALE_SCREEN)
end

function Modules.TemplDebrisellScreen:__delete()

end

function Modules.TemplDebrisellScreen:__Getwidget()
    --背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --内容框
    self.listview_content = self:GetWidget("ListView_content") 
    --标题
    self.Image_headline = self:GetWidget("Image_headline") 
    --关闭
    self.Button_close =  Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose))
    --取消
    self.Button_cancel =  Util:WidgetButton( 
                                                self:GetWidget("Button_cancel") , 
                                                CPPGameLib.GetString("common_cancel") , 
                                                CPPGameLib.Handler(self,self.OnButtonClose) , 
                                                nil,nil, Macros.Color.button_red
                                            )
    --确定
    self.Button_confirm =  Util:WidgetButton( 
                                                self:GetWidget("Button_confirm") , 
                                                CPPGameLib.GetString("common_confirm") , 
                                                CPPGameLib.Handler(self,self.OnButtonConfirm) , 
                                                nil,nil, Macros.Color.button_yellow
                                            )

end
-- GetContentWidth
-- GetContentHeight()
function Modules.TemplDebrisellScreen:__ShowUI()
    --标题
    local lab = Util:Label(CPPGameLib.GetString("sell_pinzhishaixuan"), Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
    self.Image_headline:AddChild(lab)  
    PosTool.Center(lab)

    local bg_sp9_get_approach = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_get_approach")
    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_no")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes")

    self.onCbx = {}
    for i,v in ipairs( self.lst ) do
        local layout = Util:Layout(self.listview_content:GetContentWidth() - 5 , 66)
        layout:SetAnchorPoint(0.5, 1)
        self.listview_content:PushBackCustomItem(layout)

        local sp_bg = Util:Sprite9( bg_sp9_get_approach )
        sp_bg:SetContentSize( self.listview_content:GetContentWidth() - 5 , 66 )
        sp_bg:SetAnchorPoint(0.5,0.5)
        layout:AddChild(sp_bg)
        PosTool.Center(sp_bg)

        local text = ""
        local quality = 0
        if type( v ) == "table" then
            text = v.str
            quality = v.quality
        else
            text = CPPGameLib.GetString( "sell_quality" .. v )
            quality = v
        end

        local lab = Util:Label(     
                                    text , 
                                    Macros.TypefaceSize.normal , 
                                    Macros.Quality[ quality ] ,
                                    nil,nil,Macros.Quality_Stroke[ quality ]
                                    )
        lab:SetAnchorPoint(0.5,0.5)
        lab:SetPosition(sp_bg:GetContentWidth()/2 - 50 , sp_bg:GetContentHeight()/2)
        sp_bg:AddChild(lab)

        local cbx = UICheckBoxEx.CreateWithImage(sp_1,sp_2)
        -- PosTool.RightTo(lab,cbx)
        cbx:SetPosition( sp_bg:GetContentWidth() - cbx:GetContentWidth() - 5 , sp_bg:GetContentHeight()/2 )
        sp_bg:AddChild(cbx)
        cbx:SetEventListener(
             GlobalCallbackMgr:AddCallBackFunc(function()
                self.onCbx[ v ] = i
                 -- CallModuleFunc(ModuleType.DEBRISELL, "SaveFiltrationData", self.type , v)
             end),
             GlobalCallbackMgr:AddCallBackFunc(function()
                self.onCbx[ v ] = nil
                 -- CallModuleFunc(ModuleType.DEBRISELL, "RemoveFiltrationData", self.type , v)
             end)
        )  
    end

    self.listview_content:SetTouchEnabled(#self.lst > 3)
end

function Modules.TemplDebrisellScreen:OnButtonClose()
    self:DeleteMe()
end

function Modules.TemplDebrisellScreen:OnButtonCancel()
    self:DeleteMe()
end

function Modules.TemplDebrisellScreen:OnButtonConfirm()
    if self.func then
        self.func( self.onCbx )
    end
    self:DeleteMe()
end