--
-- @brief 公会创建界面
-- @author: yjg
-- @date: 2016年10月17日19:40:59
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionEstablish = LogicGUI.TemplUnionEstablish or BaseClass(GUI.Template)

function LogicGUI.TemplUnionEstablish:__init(data)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self:__Getwidget()
        self:__ShowUI()
    end

    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_DATE , function (_, data)
        CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION)
        self:OnButtonClose()
    end)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION,true)
    self:InitTemplate(GUI.TemplateRegistry.UNION_ESTABLISH) 
end

function LogicGUI.TemplUnionEstablish:__delete()
	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end
end

function LogicGUI.TemplUnionEstablish:__Getwidget( ... )
    --背景框
    self.Image_Bg = self:GetWidget("Image_Bg")
    --光
    self.Image_light = self:GetWidget("Image_light")
    --关闭按钮
    self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") ,nil , CPPGameLib.Handler(self,self.OnButtonClose) )
    --背景框
    self.Image_content = self:GetWidget("Image_content")
    --输入背景
    self.Image_import = self:GetWidget("Image_import")
    --输入
    self.node_TextField = Util:WidgetTextField(self:GetWidget("node_TextField"))
    --军团
    self.Text_corps = self:GetWidget("Text_corps")
    --消耗
    self.Panel_expend = self:GetWidget("Panel_expend")
    --确定
    self.Button_establish = Util:WidgetButton( 
                                                self:GetWidget("Button_establish") , CPPGameLib.GetString("common_confirm") , 
                                                CPPGameLib.Handler(self,self.OnButtonEstablish) ,
                                                nil,nil,Macros.Color.button_yellow
                                            )

end

function LogicGUI.TemplUnionEstablish:__ShowUI( ... )
    --标题
    local name = Util:Label(CPPGameLib.GetString("union_establishUnion") , Macros.TypefaceSize.popup , Macros.Color.white)
    name:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
    self.Image_light:AddChild(name)

    --公会名称
    local union_name = Util:Label(CPPGameLib.GetString("union_name") , Macros.TypefaceSize.popup , Macros.Color.keypoint )
    union_name:SetPosition(self.Text_corps:GetContentWidth() + 10 , self.Text_corps:GetContentHeight()/2 )
    union_name:SetAnchorPoint(1 , 0.5)
    self.Text_corps:AddChild(union_name)

    --输入框
    self.node_TextField:SetMaxLengthEnabled(true)
    self.node_TextField:SetMaxLength(6)
    self.node_TextField:SetTextColor(Macros.Color.keypoint)
    self.node_TextField:SetPlaceHolderText(CPPGameLib.GetString("union_perch"))

    --创建消耗
    local tab = {
                    { zi = CPPGameLib.GetString("rebel_consume"), ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
                    { tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.DIAMOND ) , dx = 0.8 },
                    { zi = ConfigAdapter.Common.GetUnionEstablishConsume() , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal , jg = 5 },
                }
    local expend = Util:LabCenter( tab )
    self.Panel_expend:AddChild( expend )    
    PosTool.Center( expend , - 100 )


    local Viplv = CallModuleFunc(ModuleType.VIP , "GetVipLV")
    local Viplv_ = ConfigAdapter.Common.GetUnionEstablishDesiredVip()

    local color = Macros.Color.front_color
    if Viplv < Viplv_ then
        color = Macros.Color.red_color
    end

    --创建消耗
    local tab 
    if CPPSdkTools.GetCPChannelType() == Macros.SDK.CPChannelType.NNS_BT then
        tab = {
            { zi = CPPGameLib.GetString("union_desiredGMLevel"), ys = color , dx = Macros.TypefaceSize.normal },
        }
    else
        tab = {
            { zi = CPPGameLib.GetString("union_desiredVip" , ConfigAdapter.Common.GetUnionEstablishDesiredVip()), ys = color , dx = Macros.TypefaceSize.normal },
        }
    end
    local union_desiredVip = Util:LabCenter( tab )
    self.Panel_expend:AddChild( union_desiredVip )    
    PosTool.Center( union_desiredVip , 100 )

end
    
--取消
function LogicGUI.TemplUnionEstablish:OnButtonClose()
	self:DeleteMe()
end

-- --确定
function LogicGUI.TemplUnionEstablish:OnButtonEstablish( ... )
    local info = {}
    --公会名
    info.guild_name = self.node_TextField:GetText()
    --图标id 默认 1
    info.icon_id = 1

    local name , isop = LanguageFilter.NameFilter(info.guild_name)

    local diamond = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
    local diamond_ = ConfigAdapter.Common.GetUnionEstablishConsume()

    local Viplv = CallModuleFunc(ModuleType.VIP , "GetVipLV")
    local Viplv_ = ConfigAdapter.Common.GetUnionEstablishDesiredVip()

    if isop and diamond >= diamond_ and Viplv >= Viplv_ then
        if info.guild_name == "" then
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_perch") )
            return
        end
        if not GlobalCustomizedLogicAdapterMgr:Call("CheckCreateGuildEnabled") then
            return
        end
        
        CallModuleFunc(ModuleType.UNION, "GuildCreateReq",info)

    elseif Viplv < Viplv_ then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_desiredNoVip") )

    elseif diamond < diamond_ then 
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_NoDiamond") )

    else
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_name_invalid") )
    end
    
end