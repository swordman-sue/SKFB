
--
-- @brief 获得弹窗
-- @author: yjg
-- @date: 2016年10月28日17:51:53
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplAcquireTips = LogicGUI.TemplAcquireTips or BaseClass(GUI.Template)

LogicGUI.TemplAcquireTips.MAX = 5

function LogicGUI.TemplAcquireTips:__init(data , fun , hideHero , effects,is_separation)
	self.__is_auto_load = true
	self.__fun = fun
	self.hideHero = hideHero
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self:__Getwidget()
		self:__SettleData(data,is_separation)
		self:Effects(effects)
	end


	self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS )	
end

function LogicGUI.TemplAcquireTips:__delete()
end

function LogicGUI.TemplAcquireTips:__Getwidget(type,finished_callback)
	self.index = 0
	self.showIndex = 0
	self.iconLst = {}
	self.key = false
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--特效
	self.Panel_effects = self:GetWidget("Panel_effects")
	self.Panel_effects:SetPosition(self.Panel_effects:GetPositionX() , self.Panel_effects:GetPositionY() + 10 )
	--特效文字
	self.Panel_effectText = self:GetWidget("Panel_effectText")

	--确定按钮
	self.Button_ok = Util:WidgetButton( 
										self:GetWidget("Button_ok") , 
										CPPGameLib.GetString("common_confirm") , 
										CPPGameLib.Handler(self,self.OnBtnClose) 
										,nil ,Macros.Color.white ,Macros.Color.button_yellow
										)
end

function LogicGUI.TemplAcquireTips:__SettleData( data,is_separation)
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
			-- local type_ , info = ObjIDTool.GetInfo( i )
			-- if type_ == Macros.Game.ObjType.ITEM and info.sub_type == Macros.Game.NormalItemSubType.CURRENCY then
		 --     	if num then
		 --        	num = num * info.param2
			-- 	end
			-- end
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
	local t3 = {}
	local t4 = {}
	local max = 5
	local index = 0
	for i,v in ipairs(t2) do
		index = index + 1 
		table.insert(t3,v)
		if index == max then
			table.insert(t4,t3)
			t3 = {}
			index = 0
		end
	end
	if #t3 ~= 0 then
		table.insert(t4,t3)
	end
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
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.HERO_SINGLE_RECRUIT_VIEW, false, t5[1].id , Macros.Game.HERO_RECRUIT.MORE_RECRUIT ,function ( ... )
				table.remove(t5,1)
				if t5[1] then
					Count()
				else
					self:__ShowUI(t4)
				end
			end)
		else
			self:__ShowUI(t4)
		end
	end
	Count()
end

function LogicGUI.TemplAcquireTips:__ShowUI(data)
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
			for i,v in ipairs(data[self.index]) do
				CPPActionManager.DelayTo(self.Image_bg , 0.2 * (i), func )
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
	self.iconLst = {}

	func = GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		layou:SetContentSize(#data[self.index] * 110 , 90 )
		local interval = layou:GetContentWidth() / #data[self.index]

		self.showIndex = self.showIndex + 1 
		self.iconLst[self.showIndex] = LogicGUI.IconLayer.New(false)
		self.iconLst[self.showIndex]:SetPosition( 55 + interval * (self.showIndex - 1)  , layou:GetContentHeight()/2 + 20)
		self.iconLst[self.showIndex]:SetData({ id = data[self.index][self.showIndex].id , num = data[self.index][self.showIndex].value,is_extra = data[self.index][self.showIndex].is_extra})
		self.iconLst[self.showIndex]:SetAnchorPoint(0.5,0.5)
		layou:AddChild(self.iconLst[self.showIndex]:GetNode())

		local type_,info_ = ObjIDTool.GetInfo(data[self.index][self.showIndex].id)
		local name = Util:Name(info_.name,Macros.TypefaceSize.slightly , info_.quality or info_.init_quality , nil , self.iconLst[self.showIndex]:GetNode():GetContentWidth() + 20)
		name:SetPosition(self.iconLst[self.showIndex]:GetNode():GetContentWidth()/2,0 - 10)
		name:SetAnchorPoint(0.5,1)
		self.iconLst[self.showIndex]:GetNode():AddChild(name)

	end)
	
	judge()
end

function LogicGUI.TemplAcquireTips:OnBtnClose()
	CPPActionManager.StopAllActions(self.Image_bg)
	if self.key == false then
		self.showIndex = 0
		self:__ShowUI(self.allData)
	else
		if self.__fun then
			self.__fun()
		end
		self:DeleteMe()
	end
end

--特效
function LogicGUI.TemplAcquireTips:Effects( effects )
	--特效
	local effect_id, effect = self:__CreateEffect({id = 100033}, self.Panel_effects)
	PosTool.CenterBottom(effect)	

	local effect_id, effect = self:__CreateEffect({id = effects or 100011}, self.Panel_effectText)
	PosTool.CenterBottom(effect)
end

--
-- @brief 小号渐升获得弹窗
-- @author: yjg
-- @date: 2017年3月24日16:08:56
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplLittleAcquireTips = LogicGUI.TemplLittleAcquireTips or BaseClass(GUI.Template)

LogicGUI.TemplLittleAcquireTips.MAX = 5

function LogicGUI.TemplLittleAcquireTips:__init(data,fun)
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil) 

	self.index = 0
	self.showIndex = 0
	self.iconLst = {}
	self.key = false

	self:__Getwidget()
    self:__SettleData(data)
    self:Effects()
    
end

function LogicGUI.TemplLittleAcquireTips:__delete()
	for i=1,10 do
		if self.iconLst[i] then
			self.iconLst[i]:RemoveSelf(true)
		end
	end
end

function LogicGUI.TemplLittleAcquireTips:__CreateWidgetFromLuaCode()
    self.sp_bg = Util:Sprite( Resource.PathTool.GetUIResPath("citycommonslices/bg_discoveryarmy") )
    self.sp_bg:SetAnchorPoint(0.5, 0.5)

    self.layout = Util:Layout(self.sp_bg:GetContentWidth() , self.sp_bg:GetContentHeight())
    self.layout:SetTouchEnabled(false)
    self.layout:AddChild(self.sp_bg, 0)
    PosTool.Center(self.sp_bg)

    return self.layout
end

function LogicGUI.TemplLittleAcquireTips:__Getwidget(type,finished_callback)

	--特效
	local effect_id, effect = self:__CreateEffect({id = 100033}, self.sp_bg)
	effect:SetPosition( self.sp_bg:GetContentWidth()/2 , self.sp_bg:GetContentHeight() )

	local gxhd_1 = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "gxhd_1" , true ) )
	gxhd_1:SetPosition( self.sp_bg:GetContentWidth()/2 , self.sp_bg:GetContentHeight() )
	gxhd_1:SetAnchorPoint( 0.5,0.5 )
	self.sp_bg:AddChild( gxhd_1 , 99 )
end

function LogicGUI.TemplLittleAcquireTips:__SettleData( data )
	local t1 = {}
	for i,v in ipairs(data) do
		if t1[v.type or v.id] then
			t1[v.type or v.id] = t1[v.type or v.id] + (v.value or v.num)
		else
			t1[v.type or v.id] = v.value or v.num
		end
	end
	local t2 = {}
	for i,v in pairs(t1) do
		table.insert(t2,{id = i , value = v})
	end
	table.sort( t2, function ( a , b )
		return a.id < b.id
	end )
	local t3 = {}
	local t4 = {}
	local max = 5
	local index = 0
	for i,v in ipairs(t2) do
		index = index + 1 
		table.insert(t3,v)
		if index == max then
			table.insert(t4,t3)
			t3 = {}
			index = 0
		end
	end
	if #t3 ~= 0 then
		table.insert(t4,t3)
	end
	self.allData = t4

	self:__ShowUI(t4)
end

function LogicGUI.TemplLittleAcquireTips:__ShowUI(data)
	local layou = Util:Layout()
	layou:SetAnchorPoint(0.5,0.5)
	layou:SetPosition(self.sp_bg:GetContentWidth()/2 ,self.sp_bg:GetContentHeight()/2 )
	self.sp_bg:AddChild(layou)

	--突破
	local func
	--找到这一块里面有没有
	local function judge( ... )
		self.index = self.index + 1
		if data[self.index] then
			for i,v in ipairs(data[self.index]) do
				CPPActionManager.DelayTo(self.sp_bg , 0.01 * (i), func )
			end
		end
		if self.index == #data then
			self.key = true
		end
	end

	for i=1,10 do
		if self.iconLst[i] then
			self.iconLst[i]:RemoveSelf(true)
		end
	end
	self.iconLst = {}

	func = GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		layou:SetContentSize(#data[self.index] * 110 , 90 )
		local interval = layou:GetContentWidth() / #data[self.index]

		self.showIndex = self.showIndex + 1 
		self.iconLst[self.showIndex] = LogicGUI.IconLayer.New(false)
		self.iconLst[self.showIndex]:SetPosition( 55 + interval * (self.showIndex - 1)  , layou:GetContentHeight()/2 + 10 )
		self.iconLst[self.showIndex]:SetData({ id = data[self.index][self.showIndex].id , num = data[self.index][self.showIndex].value})
		self.iconLst[self.showIndex]:SetAnchorPoint(0.5,0.5)
		layou:AddChild(self.iconLst[self.showIndex]:GetNode())

		local type_,info_ = ObjIDTool.GetInfo(data[self.index][self.showIndex].id)
		local name = Util:Name(info_.name,Macros.TypefaceSize.slightly,info_.quality,nil,self.iconLst[self.showIndex]:GetNode():GetContentWidth() + 20)
		name:SetPosition(self.iconLst[self.showIndex]:GetNode():GetContentWidth()/2 , 0 - 10)
		name:SetAnchorPoint(0.5,1)
		self.iconLst[self.showIndex]:GetNode():AddChild(name)

	end)
	
	judge()
end

--特效
function LogicGUI.TemplLittleAcquireTips:Effects()

										--延时
	CPPActionManager.DelayTo( self.sp_bg , 0.9 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
	
		--移动和渐隐是同时的
										--移动
	    CPPActionManager.MoveTo( self.sp_bg , 2 , self.sp_bg:GetContentWidth()/2 , self.layout:GetContentHeight()/2 + 400 , GlobalCallbackMgr:AddCallBackFunc(function ()
	    end))
	    								--渐隐
		CPPActionManager.FadeOut( self.sp_bg , 1 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			self:DeleteMe()
		end) )


	end)  )


end