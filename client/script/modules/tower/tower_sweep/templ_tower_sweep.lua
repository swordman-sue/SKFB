
--
-- @brief 扫荡
-- @author: yjg
-- @date: 2016年12月7日09:47:07
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplTowerSweep = LogicGUI.TemplTowerSweep or BaseClass(GUI.Template)


function LogicGUI.TemplTowerSweep:__init(fun)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self.fun = fun
		self:__Getwidget()
		self:__ShowUI()
	end

	-- 副本图片加载
	-- self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CLEAN)
	--条目
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TOWER_RESULT)
	--结算
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TOWER_RESULT_TOTAL)
	--框
	self:InitTemplate(GUI.TemplateRegistry.TOWER_CLEAR)	
end

function LogicGUI.TemplTowerSweep:__delete()

end

function LogicGUI.TemplTowerSweep:SetData(data)
end

function LogicGUI.TemplTowerSweep:__Getwidget( ... )
	--居中层
	self.Image_center = self:GetWidget("Image_center")
    --光
    self.Image_light = self:GetWidget("Image_light") 
    --关闭
    self.Button_Close = Util:WidgetButton(self:GetWidget("Button_Close"), nil , CPPGameLib.Handler( self,self.OnBtnExit))  
    self.Button_Close:SetVisible(false)
    --内容层
    self.Image_content = self:GetWidget("Image_content") 
    --确定
    self.Button_CleanDone = Util:WidgetButton(	
    											self:GetWidget("Button_CleanDone"), 
    											CPPGameLib.GetString("common_confirm") , 
    											CPPGameLib.Handler( self,self.OnBtnExit),
    											nil,nil,Macros.Color.button_yellow
    											)  
    self.Button_CleanDone:SetEnabled(false)
end

function LogicGUI.TemplTowerSweep:__ShowUI( ... )	
	--标题
	local headline = Util:Label( CPPGameLib.GetString("dungeon_clean_title") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	headline:SetAnchorPoint(0.5,0.5)
	headline:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(headline)

	self.listView = UIListViewEx.Create()
	self.listView:SetContentSize(self.Image_content:GetContentWidth() ,self.Image_content:GetContentHeight())
	self.Image_content:AddChild(self.listView)

	local lst = CallModuleFunc(ModuleType.TOWER , "GetOneKeyCleanInfo")

	local index = 0
	local function Sweep( ... )
		index = index + 1
		if index > #lst then

			local layout = LogicGUI.TemplTowerResultTotal.New( lst )
			self.listView:PushBackCustomItem(layout:GetNode())

			self.Button_CleanDone:SetEnabled(true)
		elseif lst[index] then

			local info = ConfigAdapter.GameLevel.GetRewardCurrencyMap(lst[index].map)

		    local t1 = {}
		    for k,v in pairs(info) do
		    	table.insert( t1 , {type = k , value = v} )
		    end
		    table.sort( t1, function ( a , b )
		    	return a.type < b.type
		    end )

			local layout = LogicGUI.TemplTowerResult.New(index , t1)
			self.listView:PushBackCustomItem(layout:GetNode())

		    CPPActionManager.DelayTo(headline, 0.5 , GlobalCallbackMgr:AddCallBackFunc(function()
		    	Sweep()
		    end))
		    self.Button_CleanDone:SetEnabled(false)
		end
		self.listView:ScrollToBottom()
	end

    CPPActionManager.DelayTo(headline, 0.5 , GlobalCallbackMgr:AddCallBackFunc(function()
    	Sweep()
    end))

-- --获取可能掉落的对象列表
-- ConfigAdapter.GameLevel.GetDropList(gamelevel_id)
-- --获取消耗货币表
-- ConfigAdapter.GameLevel.GetCostCurrencyMap(gamelevel_id)
-- --获取奖励货币表
-- ConfigAdapter.GameLevel.GetRewardCurrencyMap(gamelevel_id)
-- --获取奖励道具表
-- ConfigAdapter.GameLevel.GetRewardObjList(gamelevel_id)


end

--关闭
function LogicGUI.TemplTowerSweep:OnBtnExit( ... )
	self.fun()
	self:DeleteMe()
end




LogicGUI = LogicGUI or {}

LogicGUI.TemplTowerResult = LogicGUI.TemplTowerResult or BaseClass(GUI.Template)

function LogicGUI.TemplTowerResult:__init(index , data )
    self.index = index
    self.data = data
	self:InitTemplate(GUI.TemplateRegistry.TOWER_RESULT)	
    self:__Getwidget()

    self:__ShowUI()
end

function LogicGUI.TemplTowerResult:__AddToParent()
end

function LogicGUI.TemplTowerResult:__Getwidget()
	--条目
	self.Panel_item = self:GetWidget("Panel_item")
	--标题
	self.Text_headline1 = self:GetWidget("Text_headline1")
	--类型1
	self.Text_gainType1 = self:GetWidget("Text_gainType1")
	--类型2
	self.Text_gainType2 = self:GetWidget("Text_gainType2")
end
  
function LogicGUI.TemplTowerResult:__ShowUI()
	--标题
	local tower_battle = Util:Label( CPPGameLib.GetString("tower_battle" , self.index) , nil , Macros.Color.keypoint )
	tower_battle:SetAnchorPoint(0,0.5)
	tower_battle:SetPosition(0,self.Text_headline1:GetContentHeight()/2)
	self.Text_headline1:AddChild(tower_battle)

	--类型1
	local num_1 = "0"
	if self.data[1] then
		num_1 = self.data[1].value
	end
	local tab = {
					{ zi = CPPGameLib.GetString("tower_getGold") , ys = Macros.Color.keypoint },
					{ tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.GOLD ) , dx = 0.7 , jg = 3  } ,
					{ zi = num_1, ys = Macros.Color.content , jg = 10 },
				}
	local tower_getGold =  Util:LabCenter( tab )
	tower_getGold:SetAnchorPoint(0,0.5)
	tower_getGold:SetPosition( 0 , self.Text_gainType1:GetContentHeight()/2 )
	self.Text_gainType1:AddChild(tower_getGold)

	--类型2
	local num_2 = "0"
	if self.data[2] then
		num_2 = self.data[2].value
	end
	local tab = {
					{ zi = CPPGameLib.GetString("tower_gainRenown") , ys = Macros.Color.keypoint },
					{ tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.TOWER_PRESTIGE ) , dx = 0.7 , jg = 3  } ,
					{ zi = num_2, ys = Macros.Color.content , jg = 10 },
				}
	local tower_gainRenown =  Util:LabCenter( tab )
	tower_gainRenown:SetAnchorPoint(0,0.5)
	tower_gainRenown:SetPosition( 0 , self.Text_gainType2:GetContentHeight()/2 )
	self.Text_gainType2:AddChild(tower_gainRenown)
end 



LogicGUI = LogicGUI or {}

LogicGUI.TemplTowerResultTotal = LogicGUI.TemplTowerResultTotal or BaseClass(GUI.Template)

function LogicGUI.TemplTowerResultTotal:__init(data)
    self.lst = data
	self:InitTemplate(GUI.TemplateRegistry.TOWER_RESULT_TOTAL)	

    self:__Getwidget()
    self:__ShowUI()
end

function LogicGUI.TemplTowerResultTotal:__AddToParent()
end

function LogicGUI.TemplTowerResultTotal:__Getwidget()
	--条目
	self.Image_item = self:GetWidget("Image_item")
	--标题
	self.Text_Title = self:GetWidget("Text_Title")
	--提示
	self.Text_tips = self:GetWidget("Text_tips")
	--图标
	self.Panel_5 = self:GetWidget("Panel_5")
end
  
function LogicGUI.TemplTowerResultTotal:__ShowUI()
	local star = CallModuleFunc(ModuleType.TOWER , "GetNowHoldStar")

	local lv = CallModuleFunc(ModuleType.TOWER , "GetCurMapLevel")
	local chapter = CPPGameLib.GetConfig("TowerGameLevel",lv).chapter
	local chapter = CPPGameLib.GetConfig("TowerChapter",chapter)
	local t1 = {}
	local lst = {}
	local index = 0
	for i,v in ipairs(chapter.star_award_id_list) do
		for j,vv in ipairs(v) do
			index = index + 1
			table.insert( t1 , {id = vv , num = chapter.star_award_num_list[i][j] , star = chapter.star_list[i] })
			if j == #v then
				table.insert(lst,t1)
				t1 = {}
				index = 0
			end
		end
	end
	if #t1 ~= 0 then
		table.insert(lst,t1)
	end
	
	local t2 = {}
	for i,v in ipairs(lst) do
		if v[1].star <= star then
			table.insert(t2,v)
		end
	end

	for i,v in ipairs(t2[#t2]) do
		local icon = LogicGUI.IconLayer.New(true)
		local item = CPPGameLib.GetConfig("Item", v.id or 13999 )
		icon:SetData({id = v.id , obj_info = item , num = v.num})
		icon:SetPosition( 40 + (i-1) * 95 , self.Panel_5:GetContentHeight()/2 )
		icon:SetAnchorPoint(0.5,0.5)
		self.Panel_5:AddChild(icon:GetNode())
	end

	--标题
	local headline = Util:Label( CPPGameLib.GetString("tower_congrats") , nil , Macros.Color.lilyWhite )
	headline:SetAnchorPoint(0.5,0.5)
	headline:SetPosition(self.Text_Title:GetContentWidth()/2 , self.Text_Title:GetContentHeight()/2 )
	self.Text_Title:AddChild(headline)

	--提示
	local mainLab = CPPGameLib.GetString( "tower_sweepTips" , star )
	local tab = {  
					{zi = self.lst[1].lv or "????" .. "~" ..self.lst[#self.lst].lv or "???" , ys = Macros.Color.golden },
				}

	local echo = Util:Echo( mainLab , tab , Macros.Color.content )
	echo:SetAnchorPoint(0.5,0.5)
	echo:SetPosition( self.Text_tips:GetContentWidth()/2 , self.Text_tips:GetContentHeight()/2 )
	self.Text_tips:AddChild(echo)
end 
