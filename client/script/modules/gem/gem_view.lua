--[[
%% @module: 宝石
%% @author: yjg
%% @created: 2017年9月20日16:43:36
--]]


Modules = Modules or {} 


Modules.GemView = Modules.GemView or BaseClass(BaseView)

function Modules.GemView:__init()
	self.__layout_name = "gem"	
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_gem")}
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("gem"),
		show_power = true,
		show_gem = true,
		close_func = function()
			CallModuleFunc(ModuleType.GEM, "SetOpenType" )
			self:CloseManual()
		end,
	}

	self.__open_callback = function()   	
    	self:__OpenCallback()
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.GEM)
end

function Modules.GemView:__OpenCallback()
	self.index = 1

	local lineup_list = CallModuleFunc(ModuleType.HERO, "GetLineupList")
	CallModuleFunc(ModuleType.GEM, "SetOnHero", unpack( self.__open_data ) or lineup_list[1].uid )

	self:__Getwidget()
	self:__ShowUI() 
	self:__Server() 
end

function Modules.GemView:__Dispose() 
	if self.heroLst then
		self.heroLst:DeleteMe()
		self.heroLst = nil
	end

	if self.packLst then
		self.packLst:DeleteMe()
		self.packLst = nil
	end

	if self.beijing_Id then
		self:__DelEffect(self.beijing_Id)
		self.beijing_Id = nil
	end	

	if self.renwu_Id then
		self:__DelEffect(self.renwu_Id)
		self.renwu_Id = nil
	end	

	if self.kaikeng_Id then
		self:__DelEffect(self.kaikeng_Id)
		self.kaikeng_Id = nil
	end	

	if self.fazhen then
		self:__DelEffect(self.fazhen)
		self.fazhen = nil
	end	
	
	CallModuleFunc(ModuleType.GEM, "SetNoeOpen" , nil )

end

function Modules.GemView:__Getwidget()
	--人物
	self.Image_1 = self:GetWidget("Image_1")


    --英雄列表
    self.Panel_icon = self:GetWidget("Panel_icon")
    self.Panel_icon_pos = { 
		    				[1] = self.Panel_icon:GetPositionX(),
		    				[2] = self.Panel_icon:GetPositionY()
					    }

    --宝石背包
    self.Panel_pack = self:GetWidget("Panel_pack")
    self.Panel_pack_pos = { 
		    				[1] = self.Panel_pack:GetPositionX(),
		    				[2] = self.Panel_pack:GetPositionY()
					    }

    --镶嵌
    self.Panel_inlay = self:GetWidget("Panel_inlay")

    local t1 = {}
	local t2 = {}
    local radius = 220
    local beg_degree = ( 360 / 10 ) + 90 
    local step_degree = ( 360 / 10 ) 

    local x, y, degree
    for i = 1 , 10 do

        degree = beg_degree + (i - 1) * step_degree
    	if i >= 5 then
    		degree = degree + step_degree
    	end

        x = (self.Panel_inlay:GetContentWidth()/2) + math.cos(math.rad(degree)) * radius
        y = (self.Panel_inlay:GetContentHeight()/2) + math.sin(math.rad(degree)) * radius

        if i < 5 then
	        table.insert( t1 , { x = x , y = y } )
       	else
       		table.insert( t2 , { x = x , y = y } )
	    end
    end

	local t3 = {}
	for i = 1 , 10 do
		if t1[i] then
			table.insert( t3 , t1[i] )
		end

		if t2[i] then
			table.insert( t3 , t2[ #t2 - i - 1] )
		end
	end

    self.Image_inlay = {}
    self.Button_inlay = {}
    self.icon_touch = {}
    for i = 1 , 8 do

    	self.Image_inlay[i] = self:GetWidget("Image_inlay_"..i)
    	self.Image_inlay[i]:SetVisible(false)
    	self.Image_inlay[i]:SetPosition( t3[i].x , t3[i].y )

	    --添加
	    self.Button_inlay[i] = Util:WidgetButton( self:GetWidget("Button_inlay_"..i) )

	    self.icon_touch[i] = Util:Layout( self.Image_inlay[i]:GetContentWidth() , self.Image_inlay[i]:GetContentHeight() ,function ( ... )
	    	self:OnButtonInlay( i )
	    end )
	    self.icon_touch[i]:SetAnchorPoint(0.5 , 0.5)
	    self.Image_inlay[i]:AddChild( self.icon_touch[i] ,9)
	    PosTool.Center( self.icon_touch[i] )
    end

    --一键添加
    self.Button_oneKey = Util:WidgetButton(  
                                            self:GetWidget("Button_oneKey") , 
                                            CPPGameLib.GetString("gem_ongkeyinlay") , 
                                            CPPGameLib.Handler(self,self.OnButtonOneKey) ,
                                            nil,nil,Macros.Color.button_yellow 
                                            ) 

    --属性
    self.Button_property = Util:WidgetButton(  
                                            self:GetWidget("Button_property") , 
                                            CPPGameLib.GetString("gem_inlayproperty") , 
                                            CPPGameLib.Handler(self,self.OnButtonProperty) ,
                                            nil,nil,Macros.Color.button_yellow 
                                            ) 

	--规则按钮
	local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Gem")
        end )
	end )
	Button_rule:SetZOrder(100)
end

function Modules.GemView:__ShowUI()

    local Effect_con = nil
    self.beijing_Id , Effect_con = self:__CreateEffect( { res_name = "UI_baoshibeijing" } , self.__root_widget , -1)
    Effect_con:SetPosition( self.__root_widget:GetContentWidth()/2 , self.__root_widget:GetContentHeight()/2 )

    local Effect_con = nil
    self.renwu_Id , Effect_con = self:__CreateEffect( { res_name = "UI_baoshirenwu" } , self.Image_1 )
    Effect_con:SetPosition( self.Image_1:GetContentWidth()/2 + 10 , self.Image_1:GetContentHeight()/2 )
    
    local Effect_con = nil
    self.fazhen , Effect_con = self:__CreateEffect( { res_name = "UI_baoshibeijing_fazhen" } , self.__root_widget , -1 )
    Effect_con:SetPosition( self.Panel_inlay:GetPositionX() , self.Panel_inlay:GetPositionY() )

	--英雄列表
	self.heroLst = Modules.GemIconLayer.New( self.Panel_icon )
	self.Panel_icon:AddChild( self.heroLst:GetNode() )
	PosTool.Center( self.heroLst )

								--头像列表往一开始往右挪两百个像素↓
	self.Panel_icon:SetPosition( self.Panel_icon:GetPositionX() - 200 , self.Panel_icon:GetPositionY() )
				   --用一秒的时间挪到正常的位置↓		
	CPPActionManager.MoveTo( self.Panel_icon , 0.2 , self.Panel_icon_pos[1] , self.Panel_icon:GetPositionY() )

	--宝石背包
	self.packLst = Modules.GemPackLayer.New( self.Panel_pack )
	self.Panel_pack:AddChild( self.packLst:GetNode() )
	PosTool.Center( self.packLst )

								--背包列表往一开始往左挪两百个像素↓
	self.Panel_pack:SetPosition( self.Panel_pack:GetPositionX() + 200 , self.Panel_pack:GetPositionY() )
				   --用一秒的时间挪到正常的位置↓	
	CPPActionManager.MoveTo( self.Panel_pack , 0.2 , self.Panel_pack_pos[1] , self.Panel_pack:GetPositionY() )

	--宝石位置刷新
	self.gemLst = {}
	self.levelLst = {}
	for i = 1 , 8 do
	    self.gemLst[i] = LogicGUI.IconLayer.New( false )
	    -- self.gemLst[i]:SetData({id = id})
	    self.Image_inlay[i]:AddChild(self.gemLst[i]:GetNode())
	    PosTool.Center(self.gemLst[i])   

	    self.levelLst[i] = Util:Name( CPPGameLib.GetString("common_open_lev") , Macros.TypefaceSize.slightly , Macros.Game.QualityType.RED )
	    self.levelLst[i]:SetPosition( self.Image_inlay[i]:GetPositionX() , self.Image_inlay[i]:GetPositionY() - 40 )
	    self.Panel_inlay:AddChild(self.levelLst[i] , 999 )
		self.levelLst[i]:SetVisible(false)
	end

	self.sp_on = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.GEM, "sp_on" ) )
	self.sp_on:SetAnchorPoint( 0.5 , 0.5 )
	self.sp_on:SetPosition( self.Image_inlay[1]:GetPositionX() , self.Image_inlay[1]:GetPositionY() )
	self.Panel_inlay:AddChild( self.sp_on )
	self.sp_on:SetVisible(false)

end

function Modules.GemView:__Server()
	--英雄选择
	self:BindGlobalEvent(Macros.Event.GemModule.SELECT_HERO , function( ... )
		self:__SetShowUI( nil , "xuanze" )
		self:OnButtonInlay( )
	end)

	self:BindGlobalEvent(Macros.Event.GemModule.INLAY , function( ... )
		local inlayindex = CallModuleFunc(ModuleType.GEM, "GetOnVacancy")

		if self.oneKey ~= true then
			self:__SetShowUI( inlayindex , "xiangqian" )
		else
			self.oneKey = nil
			self:__SetShowUI( nil , "xiangqian" )
		end

		self.sp_on:SetPosition( self.Image_inlay[inlayindex]:GetPositionX() , self.Image_inlay[inlayindex]:GetPositionY() )
		self:SelectEffects(  )

	end)

	self:BindGlobalEvent(Macros.Event.GemModule.DISCHARGE , function( ... )
		self:__SetShowUI(nil , "xiexia")
	end)

	self:__SetShowUI()
	self:OnButtonInlay( )
end


function Modules.GemView:__SetShowUI( inlayindex  , type_ )

	CallModuleFunc(ModuleType.GEM, "SetOpenType" , nil )

	--按照等级的显示格子
	local holeLst = CallModuleFunc(ModuleType.GEM, "GetNowLevelShow")
	for i , v in ipairs( holeLst ) do
		self.Image_inlay[i]:SetVisible(true)

		local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
		local cnd = CallModuleFunc(ModuleType.GEM, "GetGemHoleCnf" , i )
		if roleLevel < cnd.role_level then
			self.levelLst[i]:SetString( CPPGameLib.GetString("common_open_lev" , cnd.role_level ) )
			self.levelLst[i]:SetVisible(true)
		else
			self.levelLst[i]:SetVisible(false)
		end
	end

	--按照开了孔的显示状态
	local gemLst , onIndex = CallModuleFunc(ModuleType.GEM, "GetOnGemLocationLst")


	local openType = CallModuleFunc(ModuleType.GEM, "GetOpenLevel" , onIndex )

	for i = 1 , #self.Button_inlay do
		if openType and i <= openType then

			local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.GEM, "sp_gem_open")
			self.Image_inlay[ i ]:SetImage( imgdata )

			local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.GEM, "sp_gem_add")
			self.Button_inlay[i]:SetNormalImage( imgdata )
			self.Button_inlay[i]:SetScale9Enabled( false )
			self.Button_inlay[i]:IgnoreContentAdaptWithSize( true )

			-- self.Button_inlay[i]:SetScaleFactor( 1 )
		else
			-- local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.GEM, "sp_gem_open")
			-- self.Image_inlay[ i ]:SetImage( imgdata )

			local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.GEM, "sp_gem_lock")
			self.Button_inlay[i]:SetNormalImage( imgdata )
			self.Button_inlay[i]:SetScale9Enabled( false )
			self.Button_inlay[i]:IgnoreContentAdaptWithSize( true )
			-- self.Button_inlay[i]:SetScaleFactor( 0.8 )
		end

		local openType = CallModuleFunc(ModuleType.GEM, "GetNoeOpen" )
		if openType ~= nil and type_ == "xuanze" then
			ActionManager.GetInstance():Stamp( self.Image_inlay[ i ] , function( ... )
			end , 0.8 , 1 , nil , true )
		else
			if i == #self.Button_inlay then
				CallModuleFunc(ModuleType.GEM, "SetNoeOpen",true )
			end
		end
	end

	--装备宝石显示
	local gemLst = CallModuleFunc(ModuleType.GEM, "GetOnGemLocationLst")
	for i , v in ipairs( gemLst ) do
		if v ~= 0 then
			--设置图标
			self.gemLst[i]:SetData({id = v})
		    self.gemLst[i]:ShowIconQuality(false)
		    self.gemLst[i]:ShowIconShade(false)
		    self.gemLst[i]:ShowIconBG(false)
		    self.gemLst[i]:SetScaleFactor( ConfigAdapter.Common.GetGemScaleFactor() )
		end

		if inlayindex and inlayindex == i and v ~= 0  then
			--隐藏物品图标
			self.gemLst[i]:SetVisible( v ~= 0 )
			ActionManager.GetInstance():Stamp( self.gemLst[i]:GetNode() , function( ... )
				local xiangqian_Id , Effect_con = self:__CreateEffect( { res_name = "UI_baoshixiangqian" } , self.Panel_inlay )
				Effect_con:SetPosition( self.Image_inlay[ i ]:GetPositionX() , self.Image_inlay[ i ]:GetPositionY() )
				--按钮状态设置
				self.Button_inlay[i]:SetVisible( v == 0 )
			end ,nil,nil,nil,true )

		else
			--隐藏物品图标
			self.gemLst[i]:SetVisible( v ~= 0 )
			--按钮状态设置
			self.Button_inlay[i]:SetVisible( v == 0 )
		end
	end
end

-- 选中按钮
-- 1962 宝石默认选中效果优化
-- 触发时机：刚进入界面未进行其他操作或者刚切换英雄未进行其他操作的时候，
-- 默认选中规则：只选中已经开坑但未镶嵌的坑位，如果没有上述条件的坑位，则没有默认选中框，等玩家点击之后才有选中框
-- 是选中已经开坑但未镶嵌的坑位 
-- 加一个条件：如果有多个的话，就按照坑位id从小到大

function Modules.GemView:OnButtonInlay( index )

	-- CPPGameLib.PrintTable("xy" , index ,"只要有 就选中")   
	--只要有 就选中
	if index then
		CallModuleFunc(ModuleType.GEM, "SetOnVacancy" , index )
		self.sp_on:SetPosition( self.Image_inlay[ index ]:GetPositionX() , self.Image_inlay[ index ]:GetPositionY() )
		self:SelectEffects(  )

		local gemLst , onIndex = CallModuleFunc(ModuleType.GEM, "GetOnGemLocationLst")
		local openType = CallModuleFunc(ModuleType.GEM, "GetOpenLevel" , onIndex )

		if index > openType then
			local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
			local cnd = CallModuleFunc(ModuleType.GEM, "GetGemHoleCnf" , index )
			if roleLevel < cnd.role_level then
			    local tips = LogicGUI.TemplAffirmTips.New( { 
			                                                    type = LogicGUI.TemplAffirmTipsType.YES,
			                                                    content =  CPPGameLib.GetString("gem_no_level" , cnd.role_level ) ,
			                                                    confirm_func = function ( ... )
			                                                    end
			                                                } )
			else
				CallModuleFunc(ModuleType.GEM, "SetOpenHole" , index )
				if index == openType + 1 then
					self.index = index
					self:OpenHole()
				else
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("gem_order"))
				end
			end
		else
			self.index = index
			if gemLst[self.index] ~= 0 then
				GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_GEM_INFO , false , gemLst[self.index] , "discharge" )
			end
		end

	else
		--没有安装宝石的列表
		local gemLst = CallModuleFunc(ModuleType.GEM, "GetNoGemLocationLst")
		if #gemLst ~= 0 then
			self.index = gemLst[1].noGemPos
			CallModuleFunc(ModuleType.GEM, "SetOnVacancy" , self.index )
			self.sp_on:SetPosition( self.Image_inlay[ self.index ]:GetPositionX() , self.Image_inlay[ self.index ]:GetPositionY() )
			self:SelectEffects()
		else
			self.index = nil
			CallModuleFunc(ModuleType.GEM, "SetOnVacancy")
			self.sp_on:SetVisible(false)
		end
	end
end

--一键镶嵌
function Modules.GemView:OnButtonOneKey( index )

	-- 2581 【优化】宝石一键镶嵌功能

	--背包所有的宝石
	local packLst = CallModuleFunc(ModuleType.GEM , "GetGemLit" )
	CPPGameLib.PrintTable("xy", packLst ,"背包所有的宝石") 

	--阵位上装备的宝石
	local inlayLst , inlayindex = CallModuleFunc(ModuleType.GEM, "GetOnGemLocationLst")
	CPPGameLib.PrintTable("xy", inlayLst ,"阵位上装备的宝石") 
	--清算阵位宝石数据
	local t1 = {}
	for i , v in ipairs( inlayLst ) do
		if v ~= 0 then
			if not t1[v] then
				t1[v] = 0
			end
			t1[v] = t1[v] + 1
		end
	end

	--阵位宝石加入背包
	for i , v in ipairs(packLst) do
		if t1[v.gem_id] then
			v.num = v.num + t1[v.gem_id]
			t1[v.gem_id] = nil
		end
	end

 	for k , v in pairs( t1 ) do
		local gemCnf = CallModuleFunc(ModuleType.GEM, "GetGemCnf" , k )
	 	local inlayCnf = CallModuleFunc(ModuleType.GEM, "GetGemInlayCnf" , gemCnf.type )	
 		table.insert( packLst , { gem_id = k , num = v , max = inlayCnf.inlay_num_limit , type = gemCnf.type } )
 	end

	--持有的孔位
	local gemLst = CallModuleFunc(ModuleType.GEM, "GetNoGemLocationLst")

	--当前开启
	local openLevel = CallModuleFunc(ModuleType.GEM, "GetOpenLevel" , inlayindex )
	-- CPPGameLib.PrintTable("xy", openLevel ,"当前开启") 

 	if #packLst <= 0 then
 		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("gem_no_gem"))
 	end

 	--将背包所有宝石分开并排序
 	local t3 = {}
 	local index = 1
 	for i , v in ipairs( packLst ) do
 		for j = 1 , v.num do
 			local gemCnf = CallModuleFunc(ModuleType.GEM, "GetGemCnf" , v.gem_id )
 			t3[index] = { gem_id = v.gem_id , type = v.type , max = v.max , power = gemCnf.power }
 			index = index + 1
 		end
 	end
 	table.sort( t3, function ( a , b )
 		return a.power > b.power
 	end )

 	--上限清算
 	local t4 = {}
 	for i , v in ipairs( t3 ) do
 		if not t4[ v.type ] then
 			t4[ v.type  ] = {}
 		end
 		local inlayCnf = CallModuleFunc(ModuleType.GEM, "GetGemInlayCnf" , v.type )
 		if #t4[ v.type  ] < inlayCnf.inlay_num_limit then
	 		table.insert( t4[ v.type  ] , v )
	 	end
 	end

 	--宝石拆封
 	local t5 = {}
 	for type_ , v in pairs( t4 ) do
 		for i , vv in ipairs(v) do
	 		table.insert( t5 , vv )
 		end
 	end
  	table.sort( t5, function ( a , b )
 		return a.power > b.power
 	end )

	local t6 = {}
	for i = 1 , openLevel do
		if t5[i] then
			table.insert( t6 , { id = t5[i].gem_id , hole = i } )
		end
	end

	if #t6 == 0 then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("gem_no_gem"))
		return 
	end

	self.oneKey = true

    local info = {}
    info.formation_slot = inlayindex
    info.gem_inlay_list = t6
    CallModuleFunc(ModuleType.GEM, "GemInlayReq" , info )

    for i = 1 , openLevel do
		local xiangqian_Id , Effect_con = self:__CreateEffect( { res_name = "UI_baoshixiangqian" } , self.Panel_inlay )
		Effect_con:SetPosition( self.Image_inlay[ i ]:GetPositionX() , self.Image_inlay[ i ]:GetPositionY() )
    end
end

--镶嵌属性
function Modules.GemView:OnButtonProperty( index )
	LogicGUI.TemplGemIproperty.New()
end

--开孔
function Modules.GemView:OpenHole( index )

	local id = CallModuleFunc(ModuleType.GEM , "GetOpenHole" )

	local gemCnf = CallModuleFunc(ModuleType.GEM , "GetGemHoleCnf", id )
	-- CPPGameLib.PrintTable("xy", gemCnf ,"GetGemHoleCnf")   

	local itemCnf = CPPGameLib.GetConfig("Item", gemCnf.cost_item_id)
	
	local itemNum= CallModuleFunc(ModuleType.Pack, "GetNumByItemID" , gemCnf.cost_item_id)

	local color = Macros.Color.headline
	if gemCnf.cost_item_num > itemNum then
		color = Macros.Color.red
	end
    local tips = LogicGUI.TemplAffirmTips.New( { 
                                                    type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                    content = " ",
                                                    confirm_func = function ( ... )
                                                    	if color == Macros.Color.red then
                                                    		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_potential_tips2" , itemCnf.name ))
                                                    	else
                                                    		local _ , index = CallModuleFunc(ModuleType.GEM , "GetOnGemLocationLst" )
                                                    		local info = {}
                                                    		info.formation_slot = index
                                                    		info.gem_hole = self.index
                                                    		CallModuleFunc(ModuleType.GEM , "GemHoleOpenReq" , info )

															local Effect_con = nil
															self.kaikeng_Id , Effect_con = self:__CreateEffect( { res_name = "UI_baoshikaikeng" } , self.Image_inlay[self.index] )
															Effect_con:SetPosition( self.Image_inlay[self.index]:GetContentWidth()/2 , self.Image_inlay[self.index]:GetContentHeight()/2 )
                                                    	end
                                                    end
                                                } )
    tips:SetLoadedCallback( function ( ... )
		local tab = {
						{ zi = CPPGameLib.GetString("gem_open_levelTips") , ys = Macros.Color.content , jg = 5, dx = Macros.TypefaceSize.largish },
						{ zi = itemCnf.name .. "X" .. gemCnf.cost_item_num , ys = color , jg = 5, dx = Macros.TypefaceSize.largish },
					}
		--用作好几个组件拼起来居中显示的强迫症控件 2.0版
		local lab = Util:LabCenter_3( tab , nil , 400 )
        lab:SetAnchorPoint(0.5,0.5)
        lab:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 )
        tips:GetNode():AddChild(lab)
    end )
end

--选中缩放
function Modules.GemView:SelectEffects()


	self.sp_on:SetScaleFactor( 0.8 )
	self.sp_on:SetVisible(true)
	local _ , openType = CallModuleFunc(ModuleType.GEM, "GetNoeOpen" )
	if openType ~= nil then
		ActionManager.GetInstance():Stamp( self.sp_on , function( ... )
		end , 0.8 , 1 , nil , nil )
		CallModuleFunc(ModuleType.GEM, "SetNoeOpen" , _ , true )
	else
		self.sp_on:SetScaleFactor( 1 )
		CallModuleFunc(ModuleType.GEM, "SetNoeOpen" , _ , true )
	end
end