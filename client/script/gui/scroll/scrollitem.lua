
--[[
%% @module: 滚动项基类
%% @author: swordman sue
%% @created: 2016-08-08
--]]

GUI = GUI or {}

GUI.ScrollItem = GUI.ScrollItem or BaseClass(GUI.Template)

function GUI.ScrollItem:__init()
end

function GUI.ScrollItem:__delete()
end

function GUI.ScrollItem:InitTemplate(template_name, parent_node, fill_parent)
	GUI.Template.InitTemplate(self, template_name, parent_node, fill_parent)

	self:SetAnchorPoint(0.5, 0.5)
end

function GUI.ScrollItem:SetIndex(var)
	self.__index = var
end

function GUI.ScrollItem:GetIndex()
	return self.__index
end

function GUI.ScrollItem:SetViewIndex(var)
	self.__view_index = var
end

function GUI.ScrollItem:GetViewIndex()
	return self.__view_index
end

function GUI.ScrollItem:SetSelected(var)
	--override me
end

function GUI.ScrollItem:GetTouchNode()
	return self.__root_widget
end

function GUI.ScrollItem:SetTouchFilter(var)
	self.__touch_filter = var
end

function GUI.ScrollItem:IsTouchEnable()
	return not self.__touch_filter()
end

function GUI.ScrollItem:__AddToParent(parent_node, fill_parent)
end

function GUI.ScrollItem:SetOffsetX(var)
	self.__offset_x = var
end

function GUI.ScrollItem:SetOffsetY(var)
	self.__offset_y = var
end

function GUI.ScrollItem:GetOffsetX()
	return self.__offset_x or 0
end

function GUI.ScrollItem:GetOffsetY()
	return self.__offset_y or 0
end

function GUI.ScrollItem:TouchBegin()
end

function GUI.ScrollItem:TouchMoved()
end

function GUI.ScrollItem:TouchEnd()
end

function GUI.ScrollItem:TouchCancel()
end
