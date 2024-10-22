
--[[
%% @module: 系统公告
%% @author: yjg
%% @created: 2017年1月12日11:48:10
--]]

----------------------系统公告提示框----------------------
--系统公告
Modules = Modules or {}

Modules.TemplSystemNotice = Modules.TemplSystemNotice or BaseClass(GUI.Template)

function Modules.TemplSystemNotice:__init( data , fun )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self.__render_queue = Scene.RenderQueue.GRQ_UI_UP_II
    self.is_auto_close = false
    self:InitTemplate(nil)
    PosTool.CenterTop(self, 0, -130)
    
    self.fun = fun
    self:__ShowUI( data )
end

function Modules.TemplSystemNotice:__delete()
end

function Modules.TemplSystemNotice:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(638.00,95)
    self.layout:SetClippingEnable(false)
    self.layout:SetAnchorPoint(0.5,1)
    return self.layout
end

function Modules.TemplSystemNotice:__ShowUI( data )
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp_notice")
    self.sp_bg = Util:Sprite9( sp )
    self.sp_bg:SetContentSize( 450 , self.sp_bg:GetContentHeight() )
    self.sp_bg:SetAnchorPoint(0.5, 0.5)
    self.layout:AddChild(self.sp_bg, 0)
    PosTool.Center(self.sp_bg)

    --裁切层
    self.lsyer = Util:Layout(self.sp_bg:GetContentWidth() , self.sp_bg:GetContentHeight() )
    self.lsyer:SetAnchorPoint(0.5,0.5)
    self.lsyer:SetClippingEnable( true )
    self.sp_bg:AddChild(self.lsyer)
    PosTool.Center(self.lsyer)

    --字
    self.text = Util:Echo( data.text , data.tab )
    self.text:SetAnchorPoint( 0 , 0.5 )
    self.text:SetPosition(self.lsyer:GetContentWidth() , self.lsyer:GetContentHeight()/2 )
    self.lsyer:AddChild(self.text)

    --计算时间
    local move_time
    local move_speed = -100
    local delta_world_vec = -self.text:GetContentWidth() - self.text:GetPositionX()
    move_time = delta_world_vec / move_speed

    --设置移动结束的回调
    CPPActionManager.DelayTo(self:GetNode(), move_time, GlobalCallbackMgr:AddCallBackFunc(function ()
        if self.fun then
            self.fun()
            self.fun = nil
        end
    end))

    local offset_x, offset_y = self.text:GetPosition()
    self.__elapse_time = 0
    self:SetUpdateFunc(function(elapse_time)
        self.__elapse_time = self.__elapse_time + elapse_time / CPPSceneManager:GetDeltaSpeedTime()
        self.text:SetPosition(offset_x + self.__elapse_time * move_speed, offset_y)
    end)
end
