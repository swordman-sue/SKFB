
--
-- @brief 碎片获取信息界面
-- @author: yjg
-- @date: 2016年8月18日18:02:34
--

LogicGUI = LogicGUI or {}

LogicGUI.DebrisObtainInfoLayer = LogicGUI.DebrisObtainInfoLayer or BaseClass(UILayer)

function LogicGUI.DebrisObtainInfoLayer:__init(img_BG,data,info,target)
	--条目背景
	local bg = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_lst_layer1"))
	bg:SetPosition(img_BG:GetContentWidth()/2 , img_BG:GetContentHeight()/2 + 120 )
	bg:SetContentSize(695.00,129.00)
	bg:SetAnchorPoint(0.5,0.5)
	img_BG:AddChild(bg)	

	--icon
	local icon_layer = LogicGUI.IconLayer.New()
	icon_layer:SetData({})
	icon_layer:SetScaleFactor(0.86)
	icon_layer:SetPosition( 55 , bg:GetContentHeight()/2)
	bg:AddChild(icon_layer:GetNode())

	-- --标题BG
	local headBg = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bb_biaoti"))
	headBg:SetPosition( 105 ,bg:GetContentHeight() - 11.5 )
	headBg:SetAnchorPoint(0,1)	
	bg:AddChild(headBg)

	--标题
    local lab = LabelEx.CreateWithString(info.name, Util.FontType, 20)
    lab:SetPosition(0,headBg:GetContentHeight()/2)
    lab:SetAnchorPoint(0, 0.5)
    headBg:AddChild(lab)  

	--属性BG
	local propertyBG = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_lst_layer1"))
	propertyBG:SetAnchorPoint(0, 0)
	propertyBG:SetPosition(105,15)
	propertyBG:SetContentSize(573,64)
	bg:AddChild(propertyBG)	

	--属性
	local property = Util:RichText(info.description, 20, 573, 0)
    property:SetPosition(2,propertyBG:GetContentHeight() - 2)
    property:SetAnchorPoint(0,1)
    propertyBG:AddChild(property)

    --数量
    local numText = LabelEx.CreateWithString(CPPGameLib.GetString("pack_num"), Util.FontType, 20)
    numText:SetPosition(560,104)
    numText:SetAnchorPoint(0, 0.5)
    bg:AddChild(numText) 

    --数量
    local num = LabelEx.CreateWithString(data.item_num .. "/" .. target.fragment_num, Util.FontType, 20)
    bg:AddChild(num)
    PosTool.RightTo(numText,num)

	--listBg
	local listBg = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_lst_layer1"))
	listBg:SetAnchorPoint(0.5, 0.5)
	listBg:SetPosition(img_BG:GetContentWidth()/2,159)
	listBg:SetContentSize(690,200)
	img_BG:AddChild(listBg)	

	self.lst_list = UIListViewEx.Create()
	self.lst_list:SetPosition(listBg:GetContentWidth()/2,listBg:GetContentHeight()/2)
	self.lst_list:SetAnchorPoint(0.5,0.5)
	self.lst_list:SetContentSize(listBg:GetContentWidth() - 5,listBg:GetContentHeight() - 5)
 	listBg:AddChild(self.lst_list)

 	self:CellTop()
 	for i=1,3 do
	 	self:Cell()
 	end
end

function LogicGUI.DebrisObtainInfoLayer:__delete()
	
end

function LogicGUI.DebrisObtainInfoLayer:SetData(data)
end

function LogicGUI.DebrisObtainInfoLayer:CellTop()
  	local layou = Util:Layout()
  	layou:SetContentSize(self.lst_list:GetContentWidth(),30)
  	layou:SetAnchorPoint(0.5,0.5)
  	layou:SetTouchEnabled(true)

	local numText = LabelEx.CreateWithString(CPPGameLib.GetString("Equip_getway"), Util.FontType, 20)
	numText:SetPosition(layou:GetContentWidth()/2,layou:GetContentHeight()/2)
	numText:SetAnchorPoint(0.5, 0.5)
	layou:AddChild(numText) 

	self.lst_list:PushBackCustomItem(layou)
end

function LogicGUI.DebrisObtainInfoLayer:Cell()
  	local layou = Util:Layout()
  	layou:SetContentSize(self.lst_list:GetContentWidth(),81)
  	layou:SetAnchorPoint(0.5,0.5)
  	layou:SetTouchEnabled(true)

	--条目背景
	local bg = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_lst_layer1"))
	bg:SetPosition(layou:GetContentWidth()/2 , layou:GetContentHeight()/2 )
	bg:SetContentSize(layou:GetContentWidth() - 1 ,layou:GetContentHeight())
	bg:SetAnchorPoint(0.5,0.5)
	layou:AddChild(bg)	

	self.lst_list:PushBackCustomItem(layou)
end
