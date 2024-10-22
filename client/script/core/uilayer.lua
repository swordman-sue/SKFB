
--[[
%% @module: UI容器基类
%% @author: swordman sue
%% @created: 2016-08-16
--]]

UILayer = UILayer or BaseClass(EventAndTimerWrapper)

function UILayer:__init()	
	self.__node = Util:Layout()

	Layer.identify_count = Layer.identify_count + 1
	self.__identify = Layer.identify_count
	Layer.layer_object_list[self.__identify] = self	
end

function UILayer:__delete(cleanup)
	--销毁托管资源
	AutoReleaseTool.DelAllObjs(self)

	--销毁特效
	if self.__effect_id_map then
		for i,effect_id in ipairs(self.__effect_id_map) do
			self:__DelEffect(effect_id)
		end
		self.__effect_id_map = nil
	end

	if self.__parent_layer then
		self.__parent_layer:RemoveChild(self)
	else
		self.__node:RemoveSelf(true)
	end
	if cleanup ~= false then
		self.__node = nil
	end
end

function UILayer:GetNode()
	return self.__node
end

function UILayer:Retain()
	CPPNodeUnity.Retain(self.__node)	
end

function UILayer:Release()
	CPPNodeUnity.Release(self.__node)
end

function UILayer:GetIdentify()
	return self.__identify
end

--设置Layer父节点
function UILayer:SetParentLayer(parent)
	self.__parent_layer = parent
end

--获取Layer父节点
function UILayer:GetParentLayer()	
	return self.__parent_layer
end

function UILayer:SetAnchorPoint(x, y)
	if self.__node then
		self.__node:SetAnchorPoint(x, y)
	end
end

function UILayer:IgnoreAnchorPoint(var)
	if self.__node then
		self.__node:SetIgnoreAnchorPoint(var)
	end
end

function UILayer:SetPosition(x, y)
	if self.__node then
		self.__node:SetPosition(x, y)
	end
end

function UILayer:GetPosition()
	if self.__node then
		return self.__node:GetPosition()
	end
end

function UILayer:GetPositionX()
	if self.__node then
		return self.__node:GetPositionX()
	end
end

function UILayer:GetPositionY()
	if self.__node then
		return self.__node:GetPositionY()
	end
end

function UILayer:SetContentSize(w, h)
	if self.__node then	
		self.__node:SetContentSize(w, h)	
	end
end

function UILayer:GetContentSize()
	if self.__node then
		return self.__node:GetContentSize()
	end
end

function UILayer:GetContentSizeWithScale()
	local size = self:GetContentSize()
	if size then
		return COCOSize(size.width * self:GetScaleFactor(), size.height * self:GetScaleFactor())
	end
end

function UILayer:SetWidth(w)
	if self.__node then
		return self.__node:SetContentWidth(w)
	end
end

function UILayer:SetOpacity(w)
	if self.__node then
		self.__node:SetOpacity(w)
	end
end

function UILayer:SetTag(tag)
	if self.__node then
		self.__node:SetTag(tag)
	end
end

function UILayer:GetWidth()
	if self.__node then
		return self.__node:GetContentWidth()
	end
end

function UILayer:SetHeight(h)
	if self.__node then
		return self.__node:SetContentHeight(h)
	end
end

function UILayer:GetHeight()
	if self.__node then
		return self.__node:GetContentHeight()
	end
end

function UILayer:SetScaleFactor(var)
	if self.__node then
		self.__node:SetScaleFactor(var)
	end
end

function UILayer:SetScaleXFactor(var)
	if self.__node then
		self.__node:SetScaleXFactor(var)
	end
end

function UILayer:SetScaleYFactor(var)
	if self.__node then
		self.__node:SetScaleYFactor(var)
	end
end

function UILayer:GetScaleFactor()
	if self.__node then
		return self.__node:GetScaleFactor()
	end
	return 1
end

function UILayer:SetZOrder(var)
	if self.__node then
		self.__node:SetZOrder(var)
	end
end

function UILayer:SetVisible(is_visible)
	if self.__node then
		self.__node:SetVisible(is_visible)
	end
end

function UILayer:IsVisible()
	if self.__node then
		return self.__node:IsVisible()	
	end
	return false
end

function UILayer:SetTouchEnabled(var)
	if self.__node then
		self.__node:SetTouchEnabled(var)
	end
end

function UILayer:SetTouchSwallow(var)
	if self.__node then
		self.__node:SetTouchSwallow(var)
	end
end

function UILayer:SetTouchFun(down_func, move_func, up_func, cancel_func)
	if self.__node then
		WidgetUnity.SetWidgetListener(self.__node, down_func, move_func, up_func, cancel_func)
	end
end

function UILayer:SetUpdateFunc(func, interval)
	if self.__node then
		self.__node:SetLuaUpdate(func and GlobalCallbackMgr:AddCallBackFunc(func) or 0, interval or 0)
	end
end

function UILayer:PauseActions()
	if self.__node then
		CPPActionManager.PauseAction(self.__node)
	end
end

function UILayer:ResumeActions()
	if self.__node then
		CPPActionManager.ResumeAction(self.__node)
	end
end

--添加子节点
function UILayer:AddChild(child, order, tag)	
	if not self.__node then
		return
	end
	if not child then
		LogTool.LogError("[UILayer:AddChild] 参数child无效")
		return
	end

	order = order or 0
	tag = tag or 0

    child = CPPGameLib.ConvertToCCNode(child)
	self.__node:AddChild(child, order, tag)
end

--移除子节点
function UILayer:RemoveChild(child, cleanup)
	if not self.__node then
		return
	end
	if not child then
		LogTool.LogError("[UILayer:AddChild] 参数child无效")
		return
	end

	cleanup = cleanup == nil and true or cleanup

    child = CPPGameLib.ConvertToCCNode(child)
	self.__node:RemoveChild(child, cleanup)
end

--移除所有子节点
function UILayer:RemoveAllChild(cleanup)
	if not self.__node then
		return
	end

	cleanup = cleanup == nil and true or cleanup

	self.__node:RemoveAllChild(cleanup)
end

--移除自身
function UILayer:RemoveSelf(cleanup)	
	if not self.__node then
		return
	end

	cleanup = cleanup == nil and true or cleanup

	if self.__parent_layer then
		self.__parent_layer:RemoveChild(self, cleanup)
	else
		self.__node:RemoveSelf(cleanup)
	end
end

function UILayer:GetChildByTag(tag)
	if not self.__node then
		return
	end

	return self.__node:GetChildByTag(tag)	
end

function UILayer:SetBackGroundColor(r, g, b)
	r = r or 0
	g = g or 0
	b = b or 0

	if self.__node then
		self.__node:SetBackGroundColor(r, g, b)
	end
end

function UILayer:SetBackGroundColorType(var)
	if self.__node then
		self.__node:SetBackGroundColorType(var)
	end
end

function UILayer:SetBackGroundColorOpacity(var)
	if self.__node then
		self.__node:SetBackGroundColorOpacity(var)
	end
end

function UILayer:SetBackGroundImage(image_data)
	if self.__node then
		self.__node:SetBackGroundImage(image_data)
	end
end

function UILayer:SetBackGroundImageScale9Enabled(var)
	if self.__node then
		self.__node:SetBackGroundImageScale9Enabled(var)
	end	
end

function UILayer:SetClippingEnable(var)
	if self.__node then
		self.__node:SetClippingEnable(var)
	end
end

function UILayer:SetClippingType(var)
	if self.__node then
		self.__node:SetClippingType(var)
	end
end

function UILayer:SetClippingSize(var)
	if self.__node then
		self.__node:SetClippingSize(var)
	end
end

--界面置顶
function UILayer:MoveToFront()
	if self.__node then
		local parent_node = self.__node:GetParent()
		if parent_node then
			self.__node:SetZOrder(parent_node:GetChildMaxZOrder() + 1)
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--[[
托管自动释放对象
@param obj                          对象(需自动释放的对象，注：1、确保支持GetNode、DeleteMe接口。2、在调用此接口前，确保obj的节点已添加到父容器)
@param obj_key[option]              对象索引
@param priority4autorelease[option] 对象自动销毁的优先级(优先销毁数值小的对象)
@return obj_key                     对象索引
]]
function UILayer:__AddAutoReleaseObj(obj, obj_key, priority4autorelease)
	return AutoReleaseTool.AddObj(self, obj, obj_key, priority4autorelease)
end

--[[
获取自动释放对象
@param obj_key      对象索引
]]
function UILayer:__GetAutoReleaseObj(obj_key)
	return AutoReleaseTool.GetObj(self, obj_key)
end

--[[
删除自动释放对象
@param obj_key      对象索引
]]
function UILayer:__DelAutoReleaseObj(obj_key)
	AutoReleaseTool.DelObj(self, obj_key)
end

--创建特效
function UILayer:__CreateEffect(params, parent_node, zorder)
	local effect_id, effect = GlobalEffectMgr:CreateUIEffect(params, parent_node, zorder)	
	self.__effect_id_map = self.__effect_id_map or {}
	self.__effect_id_map[effect_id] = true
	return effect_id, effect
end

--销毁特效
function UILayer:__DelEffect(effect_id)
	if self.__effect_id_map and self.__effect_id_map[effect_id] then
		self.__effect_id_map[effect_id] = nil
		GlobalEffectMgr:DelEffect(effect_id)
	end
end