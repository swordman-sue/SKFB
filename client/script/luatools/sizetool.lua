
SizeTool = SizeTool or {}

function SizeTool.PercentSize(node, percent_w, percent_h, delta_w, delta_h, parent)
    if not node then
        return
    end

    percent_w = percent_w or 0
    percent_h = percent_h or 0
    delta_w = delta_w or 1
    delta_h = delta_h or 1

    node = CPPGameLib.ConvertToCCNode(node)

    local w, h
    parent = parent or node:GetParent()
    if not parent then
        w, h = Macros.Global.DesignSize.width * percent_w + delta_w, Macros.Global.DesignSize.height * percent_h + delta_h
    else
        w, h = parent:GetContentWidth() * percent_w + delta_w, parent:GetContentHeight() * percent_h + delta_h
    end

    node:SetContentSize(w, h)
end
