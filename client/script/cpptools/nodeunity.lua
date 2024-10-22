
NodeUnity = NodeUnity or {}

function NodeUnity.GetWorldPosition(node)
	local point = {x = 0, y = 0}
	point.x, point.y = CPPNodeUnity.GetWorldPosition(node)
	return point
end

function NodeUnity.ConvertToNodePositon(node, pos)
	local tmp_pos = pos
	if type(pos) ~= 'userdata' then
		tmp_pos = COCOPoint()
		tmp_pos.x, tmp_pos.y = pos.x, pos.y
	end

	local point = COCOPoint()
	point.x, point.y = CPPNodeUnity.ConvertToNodePosition(node, tmp_pos)
	return point
end

function NodeUnity.ConvertToWorldPosition(node, pos)
	local tmp_pos = pos
	if type(pos) ~= 'userdata' then
		tmp_pos = COCOPoint()
		tmp_pos.x, tmp_pos.y = pos.x, pos.y
	end

	local point = COCOPoint()
	point.x, point.y = CPPNodeUnity.ConvertToWorldPosition(node, tmp_pos)
	return point
end

--将container_node中的pos转换为new_container_node中的new_pos
function NodeUnity.ConvertPosBetweenTwoContainer(container_node, new_container_node, pos)
	--世界坐标
	local world_pos = NodeUnity.ConvertToWorldPosition(container_node, pos)
	--新节点坐标
	local new_pos = NodeUnity.ConvertToNodePositon(new_container_node, world_pos)
	return new_pos
end

--检测pos是否在container_node
function NodeUnity.HitTest(container_node, pos)
	pos = NodeUnity.ConvertToNodePositon(container_node, pos)

	local size = container_node:GetContentSize()
	if pos.x >= 0 and pos.x < size.width and pos.y >= 0 and pos.y < size.height then
		return true
	end
end