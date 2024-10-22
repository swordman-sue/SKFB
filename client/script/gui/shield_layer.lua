LogicGUI = LogicGUI or {}

LogicGUI.ShieldLayer = LogicGUI.ShieldLayer or BaseClass(UILayer)

function LogicGUI.ShieldLayer:__init(callback)
    self:SetContentSize(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
    self:SetAnchorPoint(0,0)
    self:SetPosition(0,0)
    self:SetBackGroundColor(0, 0, 0)
    self:SetBackGroundColorType(1)
    self:SetBackGroundColorOpacity(120)
    self:SetTouchEnabled(true)

    self:SetTouchFun(nil, nil, function()
      if callback then
        callback()
      end
    end)
end

function LogicGUI.ShieldLayer:__delete()
end