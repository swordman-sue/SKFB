
GUI = GUI or {}
GUI.FeignCheckBoxOfList = GUI.FeignCheckBoxOfList or BaseClass(UILayer)

--创建
-- 创建的时候            self 间隔 (默认是竖的
-- SetContentSize  		--设置大小
-- SetData 				--tab
-- SetItemAlign 		--图片的锚点 用于细胞和那宽不一样大

--显示的方式 挑一个喜欢的
--这个是页签的
-- SetOnCeil 			--两个图片 第一个是选中的 第二个是没选中的  
--这个是原味的
-- SetCeil 				--普通的

--外面需要的函数
-- Ceil 				--细胞/条目 长这样 ↓

-- function Modules.MaincityView:Ceil( index , data )
-- 	local Layout = Util:Layout( 100  , 100 )
-- 	控件或者逻辑什么的
-- 	return Layout
-- end

-- OnCallBack 			--条目的点击回调 长这样 ↓
-- function Modules.MaincityView:OnCallBack( 编号 , 内容 )
-- end

function GUI.FeignCheckBoxOfList:__init( father , interval , is_vert )

	self.father = father
	--条目
	self.item = {}
	--选择的条目
	self.onItem = {}
	--细胞
	self.ceil = {}

	self.key = nil

    self.lst_list = UIListViewEx.Create()
    self.lst_list:SetAnchorPoint( 0.5 , 0.5 )
    self.lst_list:SetItemsMargin( interval or 0 )
    self:SetBounceEnabled( true )
    self:AddChild(self.lst_list)
    PosTool.Center( self.lst_list )

    if is_vert then
	    self.lst_list:SetDirection(UIListViewEx.ScrollDirHorizontal)
	end

	self.align_ = 0.5
end

--设置内容
function GUI.FeignCheckBoxOfList:SetData( tab )
	if not self.father.Ceil then
		return 
	end
	self.tab = tab
	if self.key == "SetCeil" then
		self:SetCeil()
	elseif self.key == "SetOnCeil" then
		self:SetOnCeil()
	end
end

--设置子控件对齐方向
function GUI.FeignCheckBoxOfList:SetItemAlign( ind , ind_)
	self.align = ind or 0.5
	self.align_ = ind_ or 0.5
	if self.key == "SetCeil" then
		self:SetCeil( )
	elseif self.key == "SetOnCeil" then
		self:SetOnCeil( )
	end
end

function GUI.FeignCheckBoxOfList:SetContentSize( w , h )
	self.lst_list:SetContentSize(w , h)
	if self.key == "SetCeil" then
		self:SetCeil()
	elseif self.key == "SetOnCeil" then
		self:SetOnCeil()
	end
end

function GUI.FeignCheckBoxOfList:SetCeil( x , y )
	if not self.tab then
		return 
	end

	self.lst_list:RemoveAllItems()
	self.key = "SetCeil"

	x = x or 0
	y = y or 0

	for i,v in ipairs(self.tab) do
		self.ceil[i] = self.father:Ceil( i , v )
		self.item[i] = Util:Layout( self.ceil[i]:GetContentWidth() , self.ceil[i]:GetContentHeight() )

		if self.align == 0 then
			self.ceil[i]:SetAnchorPoint( 0 , self.align_ )
			self.ceil[i]:SetPosition( 0 + x , self.item[i]:GetContentHeight()/2 + y )
		elseif self.align == 1 then
			self.ceil[i]:SetAnchorPoint( 1 , self.align_ )
			self.ceil[i]:SetPosition( self.item[i]:GetContentWidth() + x , self.item[i]:GetContentHeight()/2 + y )
		else
			self.ceil[i]:SetAnchorPoint( 0.5 , self.align_ )
			self.ceil[i]:SetPosition( self.item[i]:GetContentWidth()/2 + x , self.item[i]:GetContentHeight()/2 + y )
		end
		self.item[i]:AddChild( self.ceil[i] )

		self.item[i]:SetTouchEnabled(true)
		self.item[i]:SetTouchSwallow(true)
		WidgetUnity.SetWidgetListener(self.item[i], nil, nil, function ( ... )
			if self.father.OnCallBack then
				self:On( i , v ,nil ,true )
			end
		end)

		self.lst_list:PushBackCustomItem( self.item[i] )
	end
	
	--指引中，立即刷新
	if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
		self.lst_list:ForceDoLayout()	
	end

	self:On( 1 , self.tab[1] , nil , true )
end

function GUI.FeignCheckBoxOfList:SetOnCeil( Onimg , Noimg , x , y )
	if not self.tab then
		return 
	end

    self.Onimg = Onimg
    self.Noimg = Noimg
	
	x = x or 0
	y = y or 0

	self.lst_list:RemoveAllItems()
	self.key = "SetOnCeil"

	for i,v in ipairs(self.tab) do
		self.onItem[i] = Util:Sprite( self.Noimg )
		self.item[i] = Util:Layout( self.onItem[i]:GetContentWidth() , self.onItem[i]:GetContentHeight() )
		self.ceil[i] = self.father:Ceil( i , v )

		if self.align == 0 then

			self.ceil[i]:SetAnchorPoint( 0 , self.align_ )
			self.ceil[i]:SetPosition( 0 + x , self.item[i]:GetContentHeight()/2 + y )

			self.onItem[i]:SetAnchorPoint( 0 , self.align_ )
			self.onItem[i]:SetPosition( 0 + x , self.item[i]:GetContentHeight()/2 + y )

		elseif self.align == 1 then

			self.ceil[i]:SetAnchorPoint( 1 , self.align_ )
			self.ceil[i]:SetPosition( self.item[i]:GetContentWidth() + x , self.item[i]:GetContentHeight()/2 + y )

			self.onItem[i]:SetAnchorPoint( 1 , self.align_)
			self.onItem[i]:SetPosition( self.item[i]:GetContentWidth() + x , self.item[i]:GetContentHeight()/2 + y )

		else

			self.ceil[i]:SetAnchorPoint( 0.5 , self.align_ )
			self.ceil[i]:SetPosition( self.item[i]:GetContentWidth()/2 + x , self.item[i]:GetContentHeight()/2 + y )

			self.onItem[i]:SetAnchorPoint( 0.5 , self.align_ )
			self.onItem[i]:SetPosition( self.item[i]:GetContentWidth()/2 + x , self.item[i]:GetContentHeight()/2 + y )

		end
		self.item[i]:AddChild( self.onItem[i] )
		self.item[i]:AddChild( self.ceil[i] )

		self.item[i]:SetTouchEnabled(true)
		self.item[i]:SetTouchSwallow(true)
		WidgetUnity.SetWidgetListener(self.item[i], 
			function ( ... )
				self.item[i]:SetScaleFactor(1.01)
			end , 
			function ( ... )
				local touch = self.item[i]:IsHighlighted()
				if not touch then
					self.item[i]:SetScaleFactor(1)
				end
			end, 
			function ( ... )
				self.item[i]:SetScaleFactor(1)
				self:On( i , v , nil , true )
			end)
		self.lst_list:PushBackCustomItem( self.item[i] )
	end

	--指引中，立即刷新
	if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
		self.lst_list:ForceDoLayout()	
	end

	self:SetTouchEnabled( false )
	if not self.noOn then
		self:On( 1 , self.tab[1] ,nil , true )
	end
end

function GUI.FeignCheckBoxOfList:On( index , data , func , abort )

	if self.__sp_call_back then
		if self.__sp_call_back(index) then
			return
		end
	end

	if self.key == "SetOnCeil" then
		for i = 1 , #self.onItem do
			self.onItem[i]:SetImage( self.Noimg )
		end
		self.onItem[index]:SetImage( self.Onimg )
	end

	if abort ~= nil then
		if self.index == index then
			return 
		end
	end

	self.index = index
	if self.father.OnCallBack then
		self.father:OnCallBack( index , data )
	end
	
	if func then
		func()
	end
end

--获得单个条目
function GUI.FeignCheckBoxOfList:GetBtn( index )
	return self.item[index] , self.tab[index]
end

function GUI.FeignCheckBoxOfList:GetBtn_( index )
	return self.onItem[index] , self.tab[index]
end

--获得挂件
function GUI.FeignCheckBoxOfList:SetRed( index )
	return self.__pendant[index] 
end

--获得挂件
function GUI.FeignCheckBoxOfList:SetTouchEnabled( bool )
	self.lst_list:SetTouchEnabled( bool )
end

--获得选中
function GUI.FeignCheckBoxOfList:GetOnBtn_()
	return self.onItem[self.index] , self.index
end

--滚到对应的页签
function GUI.FeignCheckBoxOfList:LocateToItemByIndex(index)
	if not index then 
		return
	end	
	self.lst_list:ScrollToItem(index - 1, COCOPoint(1,1), COCOPoint(1,1), 0.1)
end

--设置判断
function GUI.FeignCheckBoxOfList:SetHandleSpCallBack(sp_call_back)
	self.__sp_call_back = sp_call_back
end

--设置不默认调用
function GUI.FeignCheckBoxOfList:SetNoOn(noOn)
	self.noOn = noOn
end

--回弹开关
function GUI.FeignCheckBoxOfList:SetBounceEnabled(bool)
	self.lst_list:SetBounceEnabled(bool)
end
