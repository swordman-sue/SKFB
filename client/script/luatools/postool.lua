
PosTool = PosTool or {}

function PosTool.CenterTop(node, offset_x, offset_y, parent, only_calc_pos)
    if not node then
        return
    end

    offset_x = offset_x or 0
    offset_y = offset_y or 0

    node = CPPGameLib.ConvertToCCNode(node)

    local x, y
    parent = parent or node:GetParent()
    if not parent then
        x, y = Macros.Global.DesignSize.width / 2, Macros.Global.DesignSize.height
    else
        x, y = parent:GetContentWidth() / 2, parent:GetContentHeight()
    end

    if only_calc_pos then
        return x + offset_x, y + offset_y
    end

    node:SetPosition(x + offset_x, y + offset_y)
end

function PosTool.Center(node, offset_x, offset_y, parent, only_calc_pos)
    if not node then
        return
    end

    offset_x = offset_x or 0
    offset_y = offset_y or 0

    node = CPPGameLib.ConvertToCCNode(node)

    local x, y
    parent = parent or node:GetParent()
    if not parent then
        x, y = Macros.Global.DesignSize.width / 2, Macros.Global.DesignSize.height / 2
    else
        x, y = parent:GetContentWidth() / 2, parent:GetContentHeight() / 2
    end

    if only_calc_pos then
        return x + offset_x, y + offset_y
    end

    node:SetPosition(x + offset_x, y + offset_y)
end

function PosTool.CenterBottom(node, offset_x, offset_y, parent, only_calc_pos)
    if not node then
        return
    end

    offset_x = offset_x or 0
    offset_y = offset_y or 0

    node = CPPGameLib.ConvertToCCNode(node)

    local x, y
    parent = parent or node:GetParent()
    if not parent then
        x, y = Macros.Global.DesignSize.width / 2, 0
    else
        x, y = parent:GetContentWidth() / 2, 0
    end

    if only_calc_pos then
        return x + offset_x, y + offset_y
    end

    node:SetPosition(x + offset_x, y + offset_y)
end

function PosTool.LeftTop(node, offset_x, offset_y, parent, only_calc_pos)
    if not node then
        return
    end

    offset_x = offset_x or 0
    offset_y = offset_y or 0

    node = CPPGameLib.ConvertToCCNode(node)

    local x, y
    parent = parent or node:GetParent()
    if not parent then
        x, y = 0, Macros.Global.DesignSize.height
    else
        x, y = 0, parent:GetContentHeight()
    end

    if only_calc_pos then
        return x + offset_x, y + offset_y
    end

    node:SetPosition(x + offset_x, y + offset_y)
end

function PosTool.LeftCenter(node, offset_x, offset_y, parent, only_calc_pos)
    if not node then
        return
    end

    offset_x = offset_x or 0
    offset_y = offset_y or 0

    node = CPPGameLib.ConvertToCCNode(node)

    local x, y
    parent = parent or node:GetParent()
    if not parent then
        x, y = 0, Macros.Global.DesignSize.height / 2
    else
        x, y = 0, parent:GetContentHeight() / 2
    end    

    if only_calc_pos then
        return x + offset_x, y + offset_y
    end

    node:SetPosition(x + offset_x, y + offset_y)
end

function PosTool.RightTop(node, offset_x, offset_y, parent, only_calc_pos)
    if not node then
        return
    end

    offset_x = offset_x or 0
    offset_y = offset_y or 0

    node = CPPGameLib.ConvertToCCNode(node)

    local x, y
    parent = parent or node:GetParent()
    if not parent then
        x, y = Macros.Global.DesignSize.width, Macros.Global.DesignSize.height
    else
        x, y = parent:GetContentWidth(), parent:GetContentHeight()
    end

    if only_calc_pos then
        return x + offset_x, y + offset_y
    end

    node:SetPosition(x + offset_x, y + offset_y)
end

function PosTool.RightCenter(node, offset_x, offset_y, parent, only_calc_pos)
    if not node then
        return
    end

    offset_x = offset_x or 0
    offset_y = offset_y or 0

    node = CPPGameLib.ConvertToCCNode(node)

    local x, y
    parent = parent or node:GetParent()
    if not parent then
        x, y = Macros.Global.DesignSize.width, Macros.Global.DesignSize.height / 2
    else
        x, y = parent:GetContentWidth(), parent:GetContentHeight() / 2
    end

    if only_calc_pos then
        return x + offset_x, y + offset_y
    end

    node:SetPosition(x + offset_x, y + offset_y)
end

function PosTool.RightBottom(node, offset_x, offset_y, parent, only_calc_pos)
    if not node then
        return
    end

    offset_x = offset_x or 0
    offset_y = offset_y or 0

    node = CPPGameLib.ConvertToCCNode(node)

    local x, y
    parent = parent or node:GetParent()
    if not parent then
        x, y = Macros.Global.DesignSize.width, 0
    else
        x, y = parent:GetContentWidth(), 0
    end

    if only_calc_pos then
        return x + offset_x, y + offset_y
    end

    node:SetPosition(x + offset_x, y + offset_y)
end

function PosTool.RightTo(node_lt, node_rt, offset_x, offset_y, anchor_x, anchor_y)
    if not node_lt then
        return
    end
    if not node_rt then
        return
    end    

    offset_x = offset_x or 0
    offset_y = offset_y or 0

    node_lt = CPPGameLib.ConvertToCCNode(node_lt)
    node_rt = CPPGameLib.ConvertToCCNode(node_rt)    

    anchor_x = anchor_x or 0
    anchor_y = anchor_y or 0.5
    node_rt:SetAnchorPoint(anchor_x, anchor_y)
    local x = node_lt:GetPositionX() + node_lt:GetContentWidth()
    local y = node_lt:GetPositionY()

    node_rt:SetPosition(x + offset_x, y + offset_y)
end

function PosTool.MoveBy(node, offset_x, offset_y)
    local pos_x, pos_y = node:GetPosition()
    node:SetPosition(pos_x + offset_x, pos_y + offset_y)

end
