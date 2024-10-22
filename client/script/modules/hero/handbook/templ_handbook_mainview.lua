GUI = GUI or {}
GUI.TemplHandBookMainView = GUI.TemplHandBookMainView or BaseClass(GUI.Template)

function GUI.TemplHandBookMainView:__init(data,quality)
	self:InitTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_MAINVIEW)
	self:createMainUI(data,quality)
end

function GUI.TemplHandBookMainView:__AddToParent()

end
function GUI.TemplHandBookMainView:createMainUI(data,quality)
	if not data then
		return
	end
	--获取收集列表
	self.CollectionList = CallModuleFunc(ModuleType.HERO,"GetCollectionList")
	local panel_top = self:GetWidget("Panel_top") 
	self.rData = {}
	local alreadyHaveNum = 0
	for _,hero_id in ipairs(data) do
		local dt = {}
		dt.hero_id = hero_id
		dt.quality = quality		
		if self.CollectionList[hero_id] then
			alreadyHaveNum = alreadyHaveNum + 1
			dt.IsGray = false
		else
			dt.IsGray = true
		end
		table.insert(self.rData,dt)
	end
	local heroNum = #data
	local labex = LabelEx.CreateWithString(CPPGameLib.GetString("hero_handbook_qualityHeroNum_"..quality), Util.FontType, Macros.TypefaceSize.largish )
	labex:SetColor(unpack(Macros.Quality[quality]))
	labex:SetTextWidthAndHeight(0, 0)
    labex:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
	labex:SetAnchorPoint(0.5,0.5)
	labex:SetPosition(58,16)
	panel_top:AddChild(labex)
	--数量文字
	Util:WidgetLabel(self:GetWidget("Text_numtext"),CPPGameLib.GetString("arena_numText"),Macros.TypefaceSize.largish, Macros.Color.keypoint )
	--数量
	Util:WidgetLabel(self:GetWidget("Text_num"),alreadyHaveNum.."/"..heroNum,Macros.TypefaceSize.normal, Macros.Color.content )
	--计算行数
	local rowNum = 0
	if heroNum%6 == 0 then
		rowNum = math.floor(heroNum/6)
	else
		rowNum = math.ceil(heroNum/6)
	end
	local scrollviewparams = {
		item_class = Modules.TemplHandBookHeroIcon,
    	item_width = 120,
    	item_height	= 150,
    	item_space_c = 4,
    	item_space_r = 5,
    	row	= rowNum,
    	col	= 6,
    	-- touchable = true,
    	-- touch_callback = function (index,data)
    	-- 	CallModuleFunc(ModuleType.HERO,"OpenView",BaseViewType.HERO_HANDBOOK_DETAIL,data.hero_id)
    	-- end
	}

	local scrollviewPanel = self:GetWidget("Panel_scrollview")
	self.scrollview = GUI.ScrollView.New(scrollviewparams)					
	scrollviewPanel:AddChild(self.scrollview:GetNode())			
	self.scrollview:SetDataList(self.rData)
	--根据高度适配
	local height = self.scrollview:GetViewHeight()
	scrollviewPanel:SetContentSize(740,height)	
	PosTool.Center(self.scrollview)	
	self:GetNode():SetContentSize(773,height+40)

	scrollviewPanel:SetPosition(15,0)
	PosTool.LeftTop(panel_top)	
end

function GUI.TemplHandBookMainView:__delete()
	if self.scrollview then
		self.scrollview:DeleteMe()
		self.scrollview = nil
	end
end