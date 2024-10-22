
--
-- @brief 获得弹窗
-- @author: yjg
-- @date: 2016年10月28日17:51:53
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplAcquireTips_1 = LogicGUI.TemplAcquireTips_1 or BaseClass(GUI.Template)

LogicGUI.TemplAcquireTips_1.MAX = 5

function LogicGUI.TemplAcquireTips_1:__init(data , fun , hideHero , effects,is_separation)
	self.__is_auto_load = true
	self.__fun = fun
	self.hideHero = hideHero
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self:__Getwidget( data )
		self:__SettleData(data,is_separation)
		self:Effects(effects)
	end


	self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS )	
end

function LogicGUI.TemplAcquireTips_1:__delete()
end

function LogicGUI.TemplAcquireTips_1:__Getwidget( data )
	self.index = 0
	self.showIndex = 0
	self.iconLst = {}
	self.key = false
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	self.Image_bg:SetContentSize( self.Image_bg:GetContentWidth() , self.Image_bg:GetContentHeight() )
	--特效
	self.Panel_effects = self:GetWidget("Panel_effects")
	self.Panel_effects:SetPosition(self.Panel_effects:GetPositionX() , self.Panel_effects:GetPositionY() + 10 )
	--特效文字
	self.Panel_effectText = self:GetWidget("Panel_effectText")

	if #data > 5 then
		--背景
		self.Image_bg:SetContentSize( self.Image_bg:GetContentWidth() , self.Image_bg:GetContentHeight() + 80 )
		--特效
		self.Panel_effects:SetPosition(self.Panel_effects:GetPositionX() , self.Panel_effects:GetPositionY() + 80 )
		--特效文字
		self.Panel_effectText:SetPosition(self.Panel_effectText:GetPositionX() , self.Panel_effectText:GetPositionY() + 80 )
	end

	--确定按钮
	self.Button_ok = Util:WidgetButton( 
										self:GetWidget("Button_ok") , 
										CPPGameLib.GetString("common_confirm") , 
										CPPGameLib.Handler(self,self.OnBtnClose) 
										,nil ,Macros.Color.white ,Macros.Color.button_yellow
										)

	self.lst_list = UIListViewEx.Create()
    self.lst_list:SetContentSize( 5 * 110 , self.Image_bg:GetContentHeight() - 40 )
    self.lst_list:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_bg:AddChild(self.lst_list)
	PosTool.Center( self.lst_list , 0 , -5 )

end

function LogicGUI.TemplAcquireTips_1:__SettleData( data,is_separation)
	local t1 = {}
	for i,v in ipairs(data) do
		if t1[v.type or v.id] then
			t1[v.type or v.id] = t1[v.type or v.id] + (v.value or v.num)
		else
			t1[v.type or v.id] = v.value or v.num
		end
	end
	local t2 = {}
	if not is_separation then
		for i,v in pairs(t1) do
			local num = v
			table.insert(t2,{id = i , value = num })
		end
		table.sort( t2, function ( a , b )
			return a.id < b.id
		end )
	else
		for i,v in ipairs(data) do
			table.insert(t2,{id = v.type or v.id , value = v.value or v.num ,is_extra = v.is_extra})
		end
	end

    local t4 = Util:SegmentationTab( t2 , 5 )

	self.allData = t4
	local t5 = {}
	for i,v in ipairs(data) do
		local type_ , info = ObjIDTool.GetInfo(v.id or v.type )
		if type_ == Macros.Game.ObjType.HERO and info.type ~= Macros.Game.HeroType.EXP and info.init_quality > Macros.Game.QualityType.BLUE then
			table.insert( t5 , v )
		end
	end

	local function Count( )
		if #t5 ~= 0 and self.hideHero ~= true then
			self:__ShowUI(t4)
		else
			self:__ShowUI(t4)
		end
	end
	Count()
end

function LogicGUI.TemplAcquireTips_1:__ShowUI(data)
	local layou = Util:Layout()
	layou:SetAnchorPoint(0.5,0.5)
	layou:SetPosition(self.Image_bg:GetContentWidth()/2 ,self.Image_bg:GetContentHeight()/2 )
	self.Image_bg:AddChild(layou)

	--突破
	local func
	--找到这一块里面有没有
	local function judge( ... )
		self.index = self.index + 1
		if data[self.index] then
			if #data <= 5 then
				CPPActionManager.DelayTo(self.Image_bg , 0.2 , func )
			else
				CPPActionManager.DelayTo(self.Image_bg , 1.1 , func )
			end
		end
		if self.index >= #data then
			self.key = true
		end
	end

	for i=1,10 do
		if self.iconLst[i] then
			self.iconLst[i]:RemoveSelf(true)
		end
	end

	if not self.iconLst then
		self.iconLst = {}
	end

	func = GlobalCallbackMgr:AddCallBackFunc(function ( ... )

		local layou = Util:Layout( self.Image_bg:GetContentWidth() - 2 , 140 )

		local icon = {}

		local interval = self.lst_list:GetContentWidth() / #data[self.index]

		for i , v in ipairs( data[self.index] ) do
			icon[i] = LogicGUI.IconLayer.New(false)
			icon[i]:SetData({ id = v.id , num = v.value})
			icon[i]:SetAnchorPoint(0.5,0.5)
			icon[i]:SetTouchSwallow(false)
			-- icon[i]:SetPosition( 53.5 + ( (i-1)*107 ) , layou:GetContentHeight()/2 + 10 )
			icon[i]:SetPosition( 55 + interval * (i - 1) , layou:GetContentHeight()/2 + 10 )
			icon[i]:SetVisible( false )
			layou:AddChild(icon[i]:GetNode())

			local type_,info = ObjIDTool.GetInfo(v.id)
			local name = Util:Name(info.name , Macros.TypefaceSize.slightly , info.quality , nil , icon[i]:GetNode():GetContentWidth() )
			name:SetPosition( icon[i]:GetNode():GetContentWidth()/2 , -5 )
			name:SetAnchorPoint(0.5,1)
			icon[i]:AddChild(name)

			CPPActionManager.DelayTo( icon[i]:GetNode() , 0.2 * i , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
				icon[i]:SetVisible(true)
			end ) )
		end

		self.lst_list:PushBackCustomItem(layou)
		self.lst_list:ScrollToBottom()
			

		if self.key == false then
			self:__ShowUI(self.allData)
		end
	end)
	
	judge()
end

function LogicGUI.TemplAcquireTips_1:OnBtnClose()
	CPPActionManager.StopAllActions(self.Image_bg)
	self:DeleteMe()
end

--特效
function LogicGUI.TemplAcquireTips_1:Effects( effects )
	--特效
	local effect_id, effect = self:__CreateEffect({id = 100033}, self.Panel_effects)
	PosTool.CenterBottom(effect)	

	local effect_id, effect = self:__CreateEffect({id = effects or 100011}, self.Panel_effectText)
	PosTool.CenterBottom(effect)
end