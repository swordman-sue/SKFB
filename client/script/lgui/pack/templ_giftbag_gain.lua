
--
-- @brief 礼包使用获得弹窗
-- @author: yjg
-- @date: 2016年10月31日20:01:23
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplGiftbagGain = LogicGUI.TemplGiftbagGain or BaseClass(GUI.Template)

function LogicGUI.TemplGiftbagGain:__init( data , num , func )
	self.__is_auto_load = true

	self.__open_callback = function()
		--遮罩层
		self.__shield_layer = LogicGUI.ShieldLayer.New(function()
			-- self:OnBtnClose()
		end)
		self.__shield_layer:SetAnchorPoint(0.5,0.5)
		self.__root_widget:AddChild(self.__shield_layer:GetNode(),-1)
		PosTool.Center(self.__shield_layer)
		self.num = num
		self.func = func

		self:__Getwidget()
		self:__SettleData(data)
		-- self:Effects()
	end

	self:InitTemplate(GUI.TemplateRegistry.TEMPL_GIFTBAG_GAIN)	
end

function LogicGUI.TemplGiftbagGain:__delete()
  	if self.__zones_scrollview then
  		self.__zones_scrollview:DeleteMe()
  		self.__zones_scrollview = nil
  	end
end

function LogicGUI.TemplGiftbagGain:__Getwidget()
	--背景
    self.img_selectBG = self:GetWidget("img_selectBG") 
    --关闭按钮
    self.btn_close =  Util:WidgetButton( self:GetWidget("btn_close") , nil ,CPPGameLib.Handler(self,self.OnBtnClose) )
    --确定按钮
    self.btn_ok = Util:WidgetButton( 
    									self:GetWidget("btn_ok") , 
    									CPPGameLib.GetString("btn_useend") , 
    									CPPGameLib.Handler(self,self.OnBtnClose) , 
    									nil , nil , Macros.Color.button_yellow )
    --光
    self.img_headline = self:GetWidget("img_headline") 
    --内容背景
    self.img_dataBG = self:GetWidget("img_dataBG") 
    --提示
    self.Text_tips = self:GetWidget("Text_tips") 
end

function LogicGUI.TemplGiftbagGain:__SettleData( data )
	local t1 = {}
	for i,v in ipairs(data) do
		if t1[v.item_id] then
			t1[v.item_id] = t1[v.item_id] + v.item_num
		else
			t1[v.item_id] = v.item_num
		end
	end
	local t2 = {}
	for i,v in pairs(t1) do
		local type_ , info_ = ObjIDTool.GetInfo(i)
		local quality = info_.quality or info_.init_quality
		table.insert(t2,{id = i , value = v , quality = quality })
	end

	table.sort( t2, function ( a , b )
		if a.quality > b.quality then
			return a.quality > b.quality
		elseif a.quality == b.quality then
			return a.id < b.id
		end
	end )

	self:__ShowUI(t2)
end

function LogicGUI.TemplGiftbagGain:__ShowUI(data)
	--标题
	self.headline = Util:Label( CPPGameLib.GetString("pack_oneuseHeadline") , Macros.TypefaceSize.largish , Macros.Color.lilyWhite )
	self.headline:SetPosition(self.img_headline:GetContentWidth()/2 ,self.img_headline:GetContentHeight()/2 )
	self.img_headline:AddChild(self.headline)

	--提示
	self.pack_useTips = Util:Label( CPPGameLib.GetString("pack_useTips",self.num) ,Macros.TypefaceSize.slightly, Macros.Color.content )
	self.pack_useTips:SetPosition(self.Text_tips:GetContentWidth()/2 , self.Text_tips:GetContentHeight()/2 )
	self.pack_useTips:SetAnchorPoint(0,0.5)
	self.Text_tips:AddChild(self.pack_useTips)

    local lst = data

    local info = Util:SegmentationTab( lst , 4 )

	self.lst_list = UIListViewEx.Create()
    self.lst_list:SetContentSize( self.img_dataBG:GetContentWidth() - 2 , self.img_dataBG:GetContentHeight() - 2 )
    self.lst_list:SetAnchorPoint( 0.5 , 0.5 )
	self.img_dataBG:AddChild(self.lst_list)
	PosTool.Center( self.lst_list )

	local func = nil
	local maxIndex = #info
	local nowIndex = 1

	local function newitem ( ... )
		if nowIndex <= maxIndex then
			local layou = Util:Layout( self.img_dataBG:GetContentWidth() - 2 , 140 )
			self.lst_list:PushBackCustomItem(layou)
			self.lst_list:ScrollToBottom()
			
			local icon = {}

			for i , v in ipairs( info[nowIndex] ) do

				icon[i] = LogicGUI.IconLayer.New(false)
				icon[i]:SetData({ id = v.id , num = v.value})
				icon[i]:SetAnchorPoint(0.5,0.5)
				icon[i]:SetTouchSwallow(false)
				icon[i]:SetPosition( 53.5 + ( (i-1)*107 ) , layou:GetContentHeight()/2 + 10 )
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

			nowIndex = nowIndex + 1
			CPPActionManager.DelayTo( self.img_dataBG , 1.1 , func )
		end
	end

	func = GlobalCallbackMgr:AddCallBackFunc( newitem )
	newitem()
end


function LogicGUI.TemplGiftbagGain:OnBtnClose()
	if self.func then
		self.func()
	end
	self:DeleteMe()
end

--设置按钮文字
function LogicGUI.TemplGiftbagGain:SetBtnText( text )
	self.btn_ok:SetText( text )
end

--设置标题文字
function LogicGUI.TemplGiftbagGain:SetHeadlineText( text )
	self.headline:SetString( text )
end

--设置提示文字
function LogicGUI.TemplGiftbagGain:SetTipsText( text )
	self.pack_useTips:SetString( text )
end

Modules = Modules or {}

Modules.TemplGiftbagGainItem = Modules.TemplGiftbagGainItem or BaseClass(GUI.ScrollItem)

function Modules.TemplGiftbagGainItem:__init(data)
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)
end

function Modules.TemplGiftbagGainItem:__CreateWidgetFromLuaCode(...)
	self.layout = Util:Layout(107, 140)
	self.layout:SetAnchorPoint(0.5, 0.5)
	return self.layout
end


function Modules.TemplGiftbagGainItem:__delete(  )
end

function Modules.TemplGiftbagGainItem:SetData( data )

	local icon = LogicGUI.IconLayer.New(false)
	icon:SetData({ id = data.id , num = data.value})
	icon:SetAnchorPoint(0.5,0.5)
	icon:SetTouchSwallow(false)
	self.layout:AddChild(icon:GetNode())
	PosTool.Center(icon,0,25)

	local type_,info = ObjIDTool.GetInfo(data.id)
	local name = Util:Name(info.name , Macros.TypefaceSize.slightly , info.quality , nil , icon:GetNode():GetContentWidth() )
	name:SetPosition( icon:GetNode():GetContentWidth()/2 , -5 )
	name:SetAnchorPoint(0.5,1)
	icon:AddChild(name)

end

function Modules.TemplGiftbagGainItem:__Getwidget( info )
end

function Modules.TemplGiftbagGainItem:__ShowUI( data )

end