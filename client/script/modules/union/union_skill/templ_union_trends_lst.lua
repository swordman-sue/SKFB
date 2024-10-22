
--
-- @brief 成员列表
-- @author: yjg
-- @date: 2016年10月19日11:26:19
--


Modules = Modules or {}

LogicGUI.TemplUnionTrendsLst = LogicGUI.TemplUnionTrendsLst or BaseClass(GUI.Template)

function LogicGUI.TemplUnionTrendsLst:__init(Panel)
	self.__layout_mode = GUI.Template.LayoutMode.CODE
	self.Panel = Panel
    self:InitTemplate(nil,Panel)

    print("----------------------------------------------------------")
	self.ShowList = self:BindGlobalEvent(Macros.Event.UnionModule.UNION_TRENDS,function ( _,info )
		self:UpShowList(info)
	end)

end

function LogicGUI.TemplUnionTrendsLst:__CreateWidgetFromLuaCode()
	local layout = Util:Layout()
	layout:SetAnchorPoint(0, 0)
	layout:SetContentSize(self.Panel:GetContentWidth(), self.Panel:GetContentHeight())

	self.list = UIListViewEx.Create()
	self.list:SetContentSize(layout:GetContentWidth() , layout:GetContentHeight() - 10 )
	self.list:SetPosition(0,5)
	self.list:SetAnchorPoint(0,0)
	self.list:SetBounceEnabled(true)
	layout:AddChild(self.list) 

	return layout
end

function LogicGUI.TemplUnionTrendsLst:UpShowList( info )
	self.list:RemoveAllItems()

	local function Cell( index , data )
		local time = Util:Label(TimeTool.GetTimeFormat(1444444444,TimeTool.defaul_format[16]),nil,Macros.Color.green)
		local text = Util:Label("data.text啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊",Macros.TypefaceSize.slightly,nil,self.list:GetContentWidth() - 20 - time:GetContentWidth() - 10)

		local layout = Util:Layout()
		layout:SetEnabled(true)
		layout:SetContentSize(text:GetContentWidth() + time:GetContentWidth() + 10 ,text:GetContentHeight() + 10 )	

		time:SetPosition(0,layout:GetContentHeight())
		time:SetAnchorPoint(0,1)
		layout:AddChild(time)

		text:SetPosition(time:GetContentWidth() + 10 ,layout:GetContentHeight())
		text:SetAnchorPoint(0,1)
		layout:AddChild(text)

		self.list:PushBackCustomItem(layout)
	end

	for i=1,10 do
		Cell(i)
	end
	
end

function LogicGUI.TemplUnionTrendsLst:__delete()

end