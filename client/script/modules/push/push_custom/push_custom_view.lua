--
-- @brief 自定义推送
-- @author: yjg
-- @date: 2017年12月13日10:53:36

Modules = Modules or {}

Modules.PushCustomView = Modules.PushCustomView or BaseClass(BaseView)

function Modules.PushCustomView:__init( )
    self.__layout_name = "push_custom"  
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self:__OpenCallback( id )
    end
end

function Modules.PushCustomView:__OpenCallback()
    self:__ShowUI()
end

function Modules.PushCustomView:__ShowUI( ... )
    
    local cnf = unpack(self.__open_data)
    CPPGameLib.PrintTable("xy" , cnf , "cnf------------")  
    --背景
    if cnf.bg then
        local bg = Util:Sprite( Resource.PathTool.GetUIResPath("push/".. cnf.bg , true) )
        bg:SetAnchorPoint( 0.5 , 0.5 )
        self.__root_widget:AddChild( bg )
        local x , y
        if cnf.bg_pos then
            x = cnf.bg_pos[1]
            y = cnf.bg_pos[2]
        end
        PosTool.Center( bg , x , y )
    end

    --按钮
    if cnf.btn_lst then
        --按钮
        for i , v in ipairs(cnf.btn_lst) do

            local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, v )

            --字色
            local color = Macros.Color.button_yellow
            if v == "btn_btn_bigred" then
                color = Macros.Color.button_red
            end

            --文字
            local text = ""
            if cnf.btn_text and cnf.btn_text[i] then
                text = cnf.btn_text[i]
            end 

            local btn = Util:Button( imgdata , 
                                            function ( ... )
                                                if cnf.btn_jump_lst and cnf.btn_jump_lst[i] then
                                                    local system_info = CPPGameLib.GetConfig("SystemEntry" , cnf.btn_jump_lst[i][1] )
                                                    local openType = GlobalModulesMgr:IsOpen( cnf.btn_jump_lst[i][1] , true )
                                                    if openType then
                                                        CallModuleFunc( system_info.module_type ,"OpenView", system_info.view_type or system_info.module_type , cnf.btn_jump_lst[i][2]  )
                                                    end
                                                else
                                                    self:OnButtonClose()
                                                end
                                            end , 
                                            text , 
                                            Macros.TypefaceSize.button , 
                                            Macros.Color.btn , color
                                        )
            btn:SetAnchorPoint( 0.5 , 0.5 )
            self.__root_widget:AddChild( btn )

            local x , y
            if cnf.btn_pos_lst then
                x = cnf.btn_pos_lst[i][1]
                y = cnf.btn_pos_lst[i][2]
            end
            PosTool.Center( btn , x , y )

        end
    end

    --特效
    if cnf.effects_lst then
        --特效
        for i , v in ipairs(cnf.effects_lst) do
            local id = tonumber(v)
            local tab = {}
            if id then
                tab = { id = id }
            else
                tab = { res_name = v }
            end

            local effectid,effect = self:__CreateEffect( tab , self.__root_widget , cnf.effects_zob_lst[i] )
            effect:SetAnchorPoint(0.5,0.5)
            local x , y
            if cnf.effects_pos_lst then
                x = tonumber(cnf.effects_pos_lst[i][1])
                y = tonumber(cnf.effects_pos_lst[i][2])
            end

            PosTool.Center( effect , x , y )
        end
    end

    --模型
    if cnf.modes_lst then
        --特效
        for i , v in ipairs(cnf.modes_lst) do
            local icon = LogicGUI.Model.New({ path = Resource.PathTool.GetModelPath( "hero" , v ) , parent = self.__root_widget , zorder = cnf.modes_zob_lst[i] })
            
            local x , y
            if cnf.modes_pos_lst then
                x = tonumber(cnf.modes_pos_lst[i][1])
                y = tonumber(cnf.modes_pos_lst[i][2])
            end

            PosTool.Center( icon , x , y )
        end
    end

    --文字
    if cnf.text_lst then
        --文字
        for i , v in ipairs(cnf.text_lst) do
            local text = Util:RichText( v )
            local x , y
            if cnf.text_pos_lst then
                x = tonumber(cnf.text_pos_lst[i][1])
                y = tonumber(cnf.text_pos_lst[i][2])
            end
            text:SetAnchorPoint( 0.5 , 0.5 )
            self.__root_widget:AddChild( text )
            PosTool.Center( text , x , y )
        end
    end

end

function Modules.PushCustomView:OnButtonClose()
    self:CloseManual(true)
    CallModuleFunc(ModuleType.PUSH,"Transfer" , self.__open_data[2] )
end