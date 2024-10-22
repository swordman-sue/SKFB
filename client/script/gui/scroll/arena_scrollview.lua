

GUI = GUI or {}

GUI.ArenaScrollView = GUI.ArenaScrollView or BaseClass(GUI.ScrollView)

function GUI.ArenaScrollView:__init()
	self.__touch_controller:SetMaxScrollSpeed(800)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--列表滚动回调
function GUI.ArenaScrollView:__ScrollCallBack()
	GUI.ScrollView.__ScrollCallBack(self, true)

	self:MyScrollCallBack()
end

--设置条目位置
function GUI.ArenaScrollView:MyScrollCallBack(layer1,layer2)	
	self.__close_layer1 = self.__close_layer1 or layer1
	self.__close_layer2 = self.__close_layer2 or layer2
	self:ItemIter(function (index,item)
		local pos_x, pos_y = self:__CalcPositionByIndex(index)
		local panel_pos = item:GetPanelPos()
		--print("ldx","self:GetScrollX()self:GetScrollX(): ",self:GetScrollX(),pos_x)
		panel_pos:SetPosition(0,0.07 * math.min((self:GetScrollX() + pos_x - 520),0))
	end)
	--滚动前景
	if self.__close_layer1 and self.__close_layer2 then
		for i=1,2 do
			if self["__close_layer"..i]:GetPositionX() <= (-Macros.Global.DesignSize.width/2) then
				local close_layer_position
				if i == 1 then
					close_layer_position = self.__close_layer2:GetPositionX() + Macros.Global.DesignSize.width
				else
					close_layer_position = self.__close_layer1:GetPositionX() + Macros.Global.DesignSize.width
				end			
				self["__close_layer"..i]:SetPosition(close_layer_position,self["__close_layer"..i]:GetPositionY())
			elseif self["__close_layer"..i]:GetPositionX() > (3* Macros.Global.DesignSize.width/2) then
				local close_layer_position
				if i == 1 then
					close_layer_position = self.__close_layer2:GetPositionX() - Macros.Global.DesignSize.width
				else
					close_layer_position = self.__close_layer1:GetPositionX() - Macros.Global.DesignSize.width
				end
				self["__close_layer"..i]:SetPosition(close_layer_position,self["__close_layer"..i]:GetPositionY())
			end
		end
		local scroll_x = math.min(self:GetScrollX(),0) 
		scroll_x = math.max(-(self.__touch_controller:GetNodeWidth() - Macros.Global.DesignSize.width),scroll_x)
		self.__cur_scroll_x = self.__cur_scroll_x or scroll_x
		self.__close_layer1:SetPosition(self.__close_layer1:GetPositionX() + (scroll_x - self.__cur_scroll_x) * 0.4,self.__close_layer1:GetPositionY())
		self.__close_layer2:SetPosition(self.__close_layer2:GetPositionX() + (scroll_x - self.__cur_scroll_x) * 0.4,self.__close_layer2:GetPositionY())
		self.__cur_scroll_x = scroll_x
	end
end
