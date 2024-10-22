
--[[
%% @module: 容器基类
%% @author: swordman sue
%% @created: 2016-07-07
%% @description: 
	逻辑层使用的容器基类，界面、场景对象、场景....继承此类。
	注：1、一般情况下，Layer.New后，需手动调用Layer.DeleteMe
		2、Layer.DeleteMe时，将调用所有子Layer的DeleteMe
--]]

Layer = Layer or BaseClass()

Layer.identify_count = Layer.identify_count or 0
Layer.layer_object_list = Layer.layer_object_list or {}
setmetatable(Layer.layer_object_list, {__mode = "kv"})

--检查一个Layer对象是否存在
Layer.IsLayerExist = function(identify)
	if not Layer.layer_object_list[identify] then
		return false
	end
	return true
end

local function is_layer_valid(layer)
	if not layer.__identify or not Layer.IsLayerExist(layer.__identify) then
		LogTool.LogError("layer已经销毁")
		return false
	end
	return true
end

local function is_child_valid(child)
	if not child then
		LogTool.LogError("参数child无效")		
		return false
	end
	if type(child) ~= "userdata" and 
		not IsClassOrInheritClass(child, Layer) and
		not IsClassOrInheritClass(child, UILayer) then
		LogTool.LogError("参数child的类型错误")
		return false
	end
	return true
end

function Layer:__init()	
	self.__node = CPPGameLayer.Node()
	self.__node:SetCascadeOpacityEnabled(true)
	CPPNodeUnity.Retain(self.__node)

	Layer.identify_count = Layer.identify_count + 1
	self.__identify = Layer.identify_count

	self.__parent_layer = nil
		
	--Layer子节点列表
	self.__child_layer_list = {}
	-- setmetatable(self.__child_layer_list, {__mode = "kv"})

	Layer.layer_object_list[self.__identify] = self
end

function Layer:__delete(clean_child_mark)
	if not is_layer_valid(self) then return end

	--删除Layer子节点
	for _, child_layer in pairs(self.__child_layer_list) do
		child_layer:DeleteMe(false)
	end
	self.__child_layer_list = nil
	--移除其他子节点
	self.__node:RemoveAllChild(true)
	--移除自身
	self:RemoveSelf(nil, clean_child_mark)

	CPPNodeUnity.Release(self.__node)
	self.__node = nil

	Layer.layer_object_list[self.__identify] = nil
		
	for k, v in pairs(self) do
		self[k] = nil
	end
end

--每帧更新
function Layer:Update(elapse_time)
end

--添加子节点
function Layer:AddChild(child, order, tag)	
	if not is_layer_valid(self) then return end
	if not is_child_valid(child) then return end

	order = order or 0
	tag = tag or 0

	if IsClassOrInheritClass(child, Layer) or IsClassOrInheritClass(child, UILayer) then
		child:SetParentLayer(self)
		self.__child_layer_list[child:GetIdentify()] = child
		self.__node:AddChild(child:GetNode(), order, tag)
	else
		self.__node:AddChild(child, order, tag)
	end
end

--移除子节点
function Layer:RemoveChild(child, clean_up, clean_child_mark)
	if not is_layer_valid(self) then return end
	if not is_child_valid(child) then return end

	clean_up = clean_up == nil and true or clean_up

	if IsClassOrInheritClass(child, Layer) or IsClassOrInheritClass(child, UILayer) then
		if child:GetParentLayer() ~= self then
			LogTool.LogError("[Layer:RemoveChild] 参数child不是本layer的孩子")
			return
		end
		child:SetParentLayer(nil)
		if false ~= clean_child_mark then
			self.__child_layer_list[child:GetIdentify()] = nil
		end
		self.__node:RemoveChild(child:GetNode(), clean_up)		
	else
		self.__node:RemoveChild(child, clean_up)
	end
end

--移除所有子节点
function Layer:RemoveAllChild(clean_up)
	if not is_layer_valid(self) then return end

	clean_up = clean_up == nil and true or clean_up

	for _, v in pairs(self.__child_layer_list) do
		self:RemoveChild(v, clean_up, false)
	end
	self.__child_layer_list = {}

	self.__node:RemoveAllChild(clean_up)
end

--移除自身
function Layer:RemoveSelf(clean_up, clean_child_mark)
	if not is_layer_valid(self) then return end
	
	clean_up = clean_up == nil and true or clean_up

	if self.__parent_layer then
		self.__parent_layer:RemoveChild(self, clean_up, clean_child_mark)
	else
		self.__node:RemoveSelf(clean_up)
	end
end

function Layer:GetChildCount()
	if not is_layer_valid(self) then return 0 end

	return self.__node:GetChildCount()
end

function Layer:GetChildByTag(tag)
	if not is_layer_valid(self) then return nil end

	return self.__node:GetChildByTag(tag)	
end

function Layer:GetChildLayerByTag(tag)
	if not is_layer_valid(self) then return nil end
	
	for _, child_layer in pairs(self.__child_layer_list) do
		if child_layer:GetNode():GetTag() == tag then
			return child_layer
		end
	end
end

--根据坐标对子节点进行深度排序
function Layer:SortAllChildByPos(max_scene_size_power)
	if not is_layer_valid(self) then return nil end

	self.__node:SortAllChildByPos(max_scene_size_power[1], max_scene_size_power[2])
end

--获取Layer节点
function Layer:GetNode()
	if not is_layer_valid(self) then return nil end
	
	return self.__node
end

function Layer:GetIdentify()
	return self.__identify
end

--设置Layer父节点
function Layer:SetParentLayer(parent)
	self.__parent_layer = parent
end

--获取Layer父节点
function Layer:GetParentLayer()	
	return self.__parent_layer
end

--获取Layer子节点列表
function Layer:GetChildLayerList()
	if not is_layer_valid(self) then return nil end

	local child_layer_list = {}
	setmetatable(child_layer_list, {__mode = "kv"})
	for k, v in pairs(self.__child_layer_list) do
		child_layer_list[k] = v
	end
	return child_layer_list
end

function Layer:SetPosition(x, y)
	if not is_layer_valid(self) then return end

	self.__node:SetPosition(x, y)
end

function Layer:GetPosition()
	if not is_layer_valid(self) then return end

	return self.__node:GetPosition()
end

function Layer:SetContentSize(w, h)
	if not is_layer_valid(self) then return end

	self.__node:SetContentSize(w, h)	
end

function Layer:GetContentSize()
	if not is_layer_valid(self) then return end

	return self.__node:GetContentSize()
end

function Layer:SetWidth(w)
	if not is_layer_valid(self) then return end

	return self.__node:SetContentWidth(w)
end

function Layer:GetWidth()
	if not is_layer_valid(self) then return end

	return self.__node:GetContentWidth()
end

function Layer:SetHeight(h)
	if not is_layer_valid(self) then return end

	return self.__node:SetContentHeight(h)
end

function Layer:GetHeight()
	if not is_layer_valid(self) then return end

	return self.__node:GetContentHeight()
end

function Layer:SetScaleFactor(var)
	if not is_layer_valid(self) then return end

	self.__node:SetScaleFactor(var)
end

function Layer:GetScaleFactor()
	return self.__node:GetScaleFactor()
end

function Layer:SetZOrder(var)
	if not is_layer_valid(self) then return end

	self.__node:SetZOrder(var)
end

function Layer:GetZOrder()
	if not is_layer_valid(self) then return end

	return self.__node:GetZOrder()
end

function Layer:SetVisible(is_visible)
	if not is_layer_valid(self) then return end

	self.__node:SetVisible(is_visible)
end

function Layer:IsVisible()
	if not is_layer_valid(self) then return false end

	return self.__node:IsVisible()	
end

--设置裁剪(相对层)
function Layer:SetScissorSectionByLayer(x, y, width, height)
	if not is_layer_valid(self) then return end

	self.__node:SetScissorSectionByLayer(x, y, width, height)
end

--设置裁剪(相对屏幕)
function Layer:SetScissorSection(x, y, width, height)
	if not is_layer_valid(self) then return end

	self.__node:SetScissorSection(x, y, width, height)
end

function Layer:SetTouchEnabled(var)
	if not is_layer_valid(self) then return end

	self.__node:SetTouchEnabled(var)
end

function Layer:SetTouchSwallow(var)
	if not is_layer_valid(self) then return end

	self.__node:SetTouchSwallow(var)
end

--设置触碰回调
function Layer:SetTouchFun(touch_down, touch_move, touch_up, cd_time, cd_control)
	if not is_layer_valid(self) then return end

	cd_time = cd_time or 0
	local touch_down_fun_id = touch_down and GlobalCallbackMgr:AddBtnCallBackFunc(touch_down, cd_time, nil, cd_control) or 0
	local touch_move_fun_id = touch_move and GlobalCallbackMgr:AddBtnCallBackFunc(touch_move, cd_time, nil, cd_control) or 0
	local touch_up_fun_id = touch_up and GlobalCallbackMgr:AddBtnCallBackFunc(touch_up, cd_time, nil, cd_control) or 0
	self.__node:SetEventListener(touch_down_fun_id, touch_move_fun_id, touch_up_fun_id)
end

function Layer:SetTouchRect(x, y, width, height)
	if not is_layer_valid(self) then return end

	if width > 0 and height > 0 then
		self.__node:SetTouchEnabled(true)
	else
		self.__node:SetTouchEnabled(false)
	end
	self.__node:SetTouchRect(x, y, width, height)
end

function Layer:SetFullLayerTouch()
	if not is_layer_valid(self) then return end

	local size = self:GetContentSize()
	self:SetTouchRect(0, 0, size.width, size.height)	
end

--设置全屏触碰
function Layer:SetFullScreenTouch()
	if not is_layer_valid(self) then return end

	local spt = COCOPoint(0, 0)
	local ept = COCOPoint(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
	spt = NodeUnity.ConvertToNodePositon(self.__node, spt)
	ept = NodeUnity.ConvertToNodePositon(self.__node, ept)		

	self.__node:SetTouchEnabled(true)
	self.__node:SetTouchRect(spt, ept, ept.x - spt.x + 1, ept.y - ept.y + 1)	
end

--[[
@ params：
	func 
	interval
--]]
function Layer:SetUpdateFunc(func, interval)
	if not is_layer_valid(self) then return end

	self.__node:SetLuaUpdate(func and GlobalCallbackMgr:AddCallBackFunc(func) or 0, interval or 0)
end

function Layer:PauseActions()
	if not is_layer_valid(self) then return end

	CPPActionManager.PauseAction(self.__node)
end

function Layer:ResumeActions()
	if not is_layer_valid(self) then return end

	CPPActionManager.ResumeAction(self.__node)
end

function Layer:SetAnchorPoint(x, y)
	if not is_layer_valid(self) then return end

	self.__node:SetIgnoreAnchorPoint(false)
	self.__node:SetAnchorPoint(x, y)
end

function Layer:IgnoreAnchorPoint(var)
	if not is_layer_valid(self) then return end

	self.__node:SetIgnoreAnchorPoint(var)
end

function Layer:SetRotation(rotation)
	if not is_layer_valid(self) then return end

	self.__node:SetRotation(rotation)
end
