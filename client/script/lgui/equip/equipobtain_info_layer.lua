
--
-- @brief 装备获取信息界面
-- @author: yjg
-- @date: 2016年8月18日18:02:34
--

LogicGUI = LogicGUI or {}

LogicGUI.EquipObtainInfoLayer = LogicGUI.EquipObtainInfoLayer or BaseClass(UILayer)
function LogicGUI.EquipObtainInfoLayer:__init(layer , info , come)

	self.layer = layer
	self.info = info
	self.come = come

	self:__Getwidget()
	self:__ShowUI()
end

function LogicGUI.EquipObtainInfoLayer:__delete()
end

--[[
@ data：
	index
	name
--]]
function LogicGUI.EquipObtainInfoLayer:SetData(data)

end

function LogicGUI.EquipObtainInfoLayer:__Getwidget( ... )
    --六芒星
  	self.img_xing = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.EQUIPPACK, "zb_fazheng"))
	self.img_xing:SetPosition(160.42 ,278.30)
	self.img_xing:SetContentSize(278.00,278.00)
	self.img_xing:SetAnchorPoint(0.5,0.5)
	self.layer:AddChild(self.img_xing)	

    --滚动列表
    self.img_lstBG = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_lst_layer1"))
	self.img_lstBG:SetPosition(322.38 ,70.30 - 14)
	self.img_lstBG:SetContentSize(379.00,343)
	self.img_lstBG:SetAnchorPoint(0,0)
	self.layer:AddChild(self.img_lstBG)	

    --说明
    self.img_labBg = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.EQUIPPACK, "zb_name"))
	self.img_labBg:SetPosition(160.42 , 82.56 )
	self.img_labBg:SetContentSize(275.00,34.00)
	self.img_labBg:SetAnchorPoint(0.5,0.5)
	self.layer:AddChild(self.img_labBg)	
end

function LogicGUI.EquipObtainInfoLayer:__ShowUI( ... )
	--装备说明
    local lab = LabelEx.CreateWithString(self.info.name, Util.FontType, 20)
    lab:SetPosition(self.img_labBg:GetContentWidth()/2 , self.img_labBg:GetContentHeight()/2 )
    lab:SetAnchorPoint(0.5, 0.5)
    self.img_labBg:AddChild(lab)    

    --list
	self.lst_list = UIListViewEx.Create()
    self.lst_list:SetContentSize(374,338)
    self.lst_list:SetAnchorPoint(0,0)
    self.lst_list:SetPosition(5.00,5.00)
	self.img_lstBG:AddChild(self.lst_list)

	self:Intensify()
end

function LogicGUI.EquipObtainInfoLayer:Intensify(data)
  	local layou = Util:Layout()
  	layou:SetContentSize(370,110)
  	layou:SetAnchorPoint(0.5,0.5)
 	layou:SetTouchEnabled(true)

	local bg = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_lst_layer1"))
	bg:SetPosition(layou:GetContentWidth()/2 ,layou:GetContentHeight()/2)
	bg:SetContentSize(370,105)
	bg:SetAnchorPoint(0.5,0.5)
	layou:AddChild(bg)	

	local headBg = self:HeadBG(bg,layou)
	bg:AddChild(headBg)
	--名称
	local head = LabelEx.CreateWithString(CPPGameLib.GetString("Equip_intensify"), Util.FontType, 20)
    head:SetPosition(25, headBg:GetContentHeight()/2 )
    head:SetAnchorPoint(0, 0.5)
    headBg:AddChild(head) 

	self.lst_list:PushBackCustomItem(layou)
 	self:Refine()
end

--精炼
function LogicGUI.EquipObtainInfoLayer:Refine(data)
  	local layou = Util:Layout()
  	layou:SetContentSize(370,110)
  	layou:SetAnchorPoint(0.5,0.5)
 	layou:SetTouchEnabled(true)

	local bg = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_lst_layer1"))
	bg:SetPosition(layou:GetContentWidth()/2 ,layou:GetContentHeight()/2)
	bg:SetContentSize(370,105)
	bg:SetAnchorPoint(0.5,0.5)
	layou:AddChild(bg)

	local headBg = self:HeadBG(bg,layou)
	bg:AddChild(headBg)
	--名称
	local head = LabelEx.CreateWithString(CPPGameLib.GetString("Equip_refine"), Util.FontType, 20)
    head:SetPosition(25, headBg:GetContentHeight()/2 )
    head:SetAnchorPoint(0, 0.5)
    headBg:AddChild(head) 

	self.lst_list:PushBackCustomItem(layou)
 	self:Suit()
end

--套装
function LogicGUI.EquipObtainInfoLayer:Suit(data)
  	local layou = Util:Layout()
  	layou:SetContentSize(370,209)
  	layou:SetAnchorPoint(0.5,0.5)
 	layou:SetTouchEnabled(true)

	local bg = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_lst_layer1"))
	bg:SetPosition(layou:GetContentWidth()/2 ,layou:GetContentHeight()/2)
	bg:SetContentSize(370,204)
	bg:SetAnchorPoint(0.5,0.5)
	layou:AddChild(bg)

	local headBg = self:HeadBG(bg,layou)
	bg:AddChild(headBg)
	--名称
	local head = LabelEx.CreateWithString(CPPGameLib.GetString("Equip_suit"), Util.FontType, 20)
    head:SetPosition(25 , headBg:GetContentHeight()/2 )
    head:SetAnchorPoint(0, 0.5)
    headBg:AddChild(head) 

	self.lst_list:PushBackCustomItem(layou)
end

--标题背景
function LogicGUI.EquipObtainInfoLayer:HeadBG(bg,layou)
	local headBg = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bb_biaoti"))
	headBg:SetPosition(bg:GetContentWidth()/2 - 25  ,layou:GetContentHeight() - 7.5 )
	headBg:SetAnchorPoint(0.5,1)
	return headBg
end

--取消
function LogicGUI.EquipObtainInfoLayer:Onbtn_no()
	if self.come then
		self.come()
	end
	self:DeleteMe()
end