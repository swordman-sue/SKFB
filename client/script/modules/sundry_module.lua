--[[
%% @module: 杂项模块
%% @author: yjg
%% @created: 2017年4月12日16:51:05
--]]


Modules = Modules or {}

Modules.SundryModule = Modules.SundryModule or BaseClass(BaseModule)

function Modules.SundryModule:__init()
end

--判断背包上限
-- type↓
-- Macros.Game.ObjType.HERO
-- Macros.Game.ObjType.EQUIP
-- Macros.Game.ObjType.TREASURE
-- need 需要的格子
-- text 要不要飘字
-- func
-- CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.HERO , 0 , true , function ( sum )
-- end )
function Modules.SundryModule:PackVacancy( type , need , show , func ) 
	need = need or 1
	local num , max = 0 , 999999
	local text = ""
	--如果传进来的是个英雄
	if type == Macros.Game.ObjType.HERO then
		num , max = CallModuleFunc(ModuleType.HERO, "GetHeroSumAndMax")
		text = CPPGameLib.GetString("pack_heroMax")
	--如果传进来的是个装备
	elseif type == Macros.Game.ObjType.EQUIP then
		num , max = CallModuleFunc(ModuleType.EQUIP, "GetEquipSumAndMax")
		text = CPPGameLib.GetString("pack_equipMax")
	--如果传进来的是个圣器
	elseif type == Macros.Game.ObjType.TREASURE then
		num , max = CallModuleFunc(ModuleType.TREASURE, "GetTreasureSumAndMax")
		text = CPPGameLib.GetString("pack_TreasureMax")
	--如果传进来的是个圣器
	elseif type == Macros.Game.ObjType.FAIRY then
		num , max = CallModuleFunc(ModuleType.DIVINATION, "GetDivinationSumAndMax")
		text = CPPGameLib.GetString("pack_DivinationMax")
	end

	local sum = max - num
	if sum <= need then
		if show then 
			GlobalTipMsg.GetInstance():Show( text )
		end
	end
	if func then
		func( sum , text )
	end
end

--礼包的内容要多少空位
--CallModuleFunc(ModuleType.SUNDRY, "GiftBagPackVacancy", self.id , self.pitchNum , true , function ( text )
--end )
function Modules.SundryModule:GiftBagPackVacancy( id , num , show , func ) 
	local type_ , info_	= ObjIDTool.GetInfo( id )
	if type_ ~= Macros.Game.ObjType.ITEM and info_.type ~= Macros.Game.ItemBigType.GIFT then
		return
	end

    local t1 = {}
    num = num or 1

    if info_.need_hero_bag_num then
        self:PackVacancy( Macros.Game.ObjType.HERO , nil , nil, function ( sum , text )
            if sum < info_.need_hero_bag_num * num then
                table.insert( t1 , text )
            end  
        end)
    end

    if info_.need_equipment_bag_num then
        self:PackVacancy( Macros.Game.ObjType.EQUIP , nil , nil, function ( sum , text )
            if sum < info_.need_equipment_bag_num * num then
                table.insert( t1 , text )
            end  
        end)
    end

    if info_.need_treasure_bag_num then
        self:PackVacancy( Macros.Game.ObjType.TREASURE , nil , nil, function ( sum , text )
            if sum < info_.need_treasure_bag_num * num then
                table.insert( t1 , text )
            end  
        end)
    end

    if info_.need_card_bag_num then
        self:PackVacancy( Macros.Game.ObjType.FAIRY , nil , nil, function ( sum , text )
            if sum < info_.need_card_bag_num * num then
                table.insert( t1 , text )
            end  
        end)
    end

    if show then
	    if #t1 ~= 0 then
	        GlobalTipMsg.GetInstance():Show( t1[1] )
	    end
	end

	if func then
		func(t1)
	end
end

--判断列表背包上限
-- local t1 = { 
-- 		[1] = { id = 40201 , num = 1 },
-- 		[2] = { id = 20303 , num = 1 },
-- 		[3] = { id = 30301 , num = 1 },
-- 		[4] = { id = 13311 , num = 1 },
-- }
-- CallModuleFunc(ModuleType.SUNDRY, "LstPackVacancy", t1 , true , function ( text )
-- end)
function Modules.SundryModule:LstPackVacancy( lst , show , func ) 	
	local dataLst = {}
	for k,v in pairs(lst) do
		local type_ , info_ = ObjIDTool.GetInfo( v.id )
		if type_ ~= Macros.Game.ObjType.ITEM then
			if dataLst[type_] then
				dataLst[type_] = dataLst[type_] + v.num
			else
				dataLst[type_] = v.num
			end
		end
	end

	local t1 = {}
	for type , num in pairs(dataLst) do
		self:PackVacancy( type , nil , true , function ( sum , text )
		    if sum <= num then
		        table.insert( t1 , text )
		    end  
		end )
	end

    if show then
	    if #t1 ~= 0 then
	        GlobalTipMsg.GetInstance():Show( t1[1] )
	    end
	end

	if func then
		func(t1)
	end
end

--联系GM
function Modules.SundryModule:ConnectGM()
    if CPPSdkTools.IsSelfLoginSystem() then
        self:ConnectGM_CP()
        return
    end

    CPPSdkTools.EnterCustomerService()    
end

--联系GM
function Modules.SundryModule:ConnectGM_CP()
    local tips = LogicGUI.TemplAffirmTips.New( { 
                                                type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                content = "" ,
                                                cancel_text = CPPGameLib.GetString("common_cancel") ,
                                                confirm_func = function ( ... )
                                                end,
                                                cancel_func = function ( ... )
                                                end,
                                            } )
    tips:SetLoadedCallback(function()

		local channel_type = CPPSdkTools.GetCPChannelType()
		local config_channel_setting = CPPGameLib.GetConfig("ChannelSetting", channel_type , false )

		local text = CPPGameLib.GetString("common_callGM")
		if channel_type and config_channel_setting and config_channel_setting.connect_gm_tips then
			text = config_channel_setting.connect_gm_tips
		end

        local dungeon_resetMax_w = Util:RichText( text , Macros.TypefaceSize.largish , 0 , 0 )

        local w = dungeon_resetMax_w:GetContentWidth()
        if w >= 400 then
            w = 400
        end
        local dungeon_resetMax = Util:RichText( text , Macros.TypefaceSize.largish , 400 , 0 )
        dungeon_resetMax:SetPosition( (tips:GetNode():GetContentWidth()/2) + ( (400 - w) /2 )  , tips:GetNode():GetContentHeight()/2 )
        dungeon_resetMax:SetAnchorPoint( 0.5 , 0.5 )
        tips:GetNode():AddChild(dungeon_resetMax)
	    local OnLinkClicked = function (x,y,link_type,user_data,str_url)
			if str_url and str_url ~= "" then
		    	CPPSdkTools.GotoBrowser( str_url )
			end
	    end  
	    local link_func = GlobalCallbackMgr:AddCallBackFunc(OnLinkClicked)
	    dungeon_resetMax:SetLuaTouchLink(link_func)
    end)
end

function Modules.SundryModule:ToIdGetPath( id , func )
	local type_ , info_ = ObjIDTool.GetInfo( id )
	local itemId = id
	if type_ == Macros.Game.ObjType.CURRENCY then
		itemId = info_.big_icon
	end

	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, itemId , nil , function ( ... )
		if func then
			func()
		end
	end)
	return true
end

--货币调整
function Modules.SundryModule:GetCurrencyText( data,is_mycard)
	--货币数量
	local text, money

    local config_recharge = CPPGameLib.GetConfig(is_mycard and "RechargeMyCard" or "Recharge", data , false )
    local config_month_card = CPPGameLib.GetConfig("MonthCard", data , false )
    if config_recharge then
        --充值表数据
        money = Modules.ChargeModule.GetMoney(config_recharge, "money")

    elseif config_month_card then
        --月卡表数据
        money = Modules.ChargeModule.GetMoney(config_month_card, "sell_price")
    
    elseif type(data) == "number" then
        --金额数据
        money = data
    end

	if money then
        local currency_type = CPPSystemTools.GetCurrencyType()
        text = CPPGameLib.GetString("Currency_"..currency_type, money)
	else
        text = data
    end

    --钻石
   	local layout , layoutLst = Util:LabCenter({
   		{tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.DIAMOND ) , dx = 0.8 },
   		{zi = text , mb = true , ys = 103 , dx = Macros.TypefaceSize.popup , jg = 5 },
	})	

	return text , layout , layoutLst , money
end

--客服建议
function Modules.SundryModule:ServiceIssueSubmit( question , img , img2 )
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")

	local osType = CPPSystemTools.IsAndroid()
	local os = "android"
	if not osType then
	    os = "ios"
	end

	local key = "bab3817fbca54db2"

	local post_data = {}
	post_data.userId = role_info.role_id
	post_data.roleName = role_info.role_name
	post_data.sid = role_info.server_id
	post_data.gameId = "4"
	post_data.os = "ios"
	post_data.vip = role_info.vip_level
	post_data.sign = CPPGameLib.Md5( post_data.os .. post_data.roleName .. post_data.sid .. post_data.userId .. post_data.vip .. key )
	post_data.question = question
	post_data.img = img
	post_data.img2 = img2

	local url = "http://faq.vanggame.com:8083/vgFaq/cs"
	local urlmsg_data = {}
	urlmsg_data.url = url
	urlmsg_data.post_param = post_data
	urlmsg_data.handle_func = function (_,resp_post_data)
	    self:ServiceIssueHistory()
	    GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("service_succeed") )
	end
	urlmsg_data.forbid_retry = true
	SendUrlMsg(urlmsg_data)
end

--建议历史
function Modules.SundryModule:ServiceIssueHistory(  )
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")

	local key = "bab3817fbca54db2"

	local post_data = {}
	post_data.userId = tostring(role_info.role_id)
	post_data.sid = tostring(role_info.server_id)
	post_data.gameId = "4"
	post_data.sign = CPPGameLib.Md5( post_data.sid .. post_data.userId .. key )

	local url = "http://faq.vanggame.com:8083/vgFaq/ history"
	local urlmsg_data = {}
	urlmsg_data.url = url
	urlmsg_data.post_param = post_data
	urlmsg_data.handle_func = function (_,resp_post_data)
	    if resp_post_data.status == 0 then
		    self:FireNextFrame(Macros.Event.SundryModule.ISSUE_HISTORY , resp_post_data.data )
		end
	end
	urlmsg_data.forbid_retry = true
	SendUrlMsg(urlmsg_data)
end

--选择的路径
function Modules.SundryModule:ServiceSelectPath( img_url )
	if not self.selectPath then
		self.selectPath = {}
	end

	if img_url then
		if self.setSelectFrame then
			self.selectPath[ self.setSelectFrame ] = img_url
		else
			table.insert( self.selectPath , img_url )
		end
	else
		self.selectPath = nil
	end 

	self:FireNextFrame(Macros.Event.SundryModule.SELECT_PATH)
end

--选择的路径
function Modules.SundryModule:GetServiceSelectPath( )
	return self.selectPath or {}
end

--选择的框
function Modules.SundryModule:SetSelectFrame( index )
	self.setSelectFrame = index
end

--上传选中的相册图片
function Modules.SundryModule:UploadAlbumImage(data)
    SendFile2Server({
        url = StartConfig.UploadImageURL or "http://39.108.124.78:8888/skfb/develop/upload_image",
        file_path = data.path,
        mime_type = CPPGameLib.GetMimeTypeByExt(data.path),
        forbid_retry = true,        
        handle_func = function(_, json_data)
            LogTool.LogInfo("[Modules.SundryModule:UploadAlbumImage] upload image result")
            if json_data.image_url then
                LogTool.LogInfo("[Modules.SundryModule:UploadAlbumImage] upload image success")
                CallModuleFunc(ModuleType.SUNDRY, "ServiceSelectPath", json_data.image_url) 
            end
        end
    })
end

--获取随机角色名
function Modules.SundryModule:GetRandomRoleName()
    --表
    local lst = CPPGameLib.GetConfig("NameList", nil, nil, true)

    local function Group( ... )
        local key = {}
        local part = MathTool.GetRandom( 1 , #lst )
        if lst[part].first_name then
            key[1] = lst[part].first_name
        else
            key[1] = ""
        end

        local part = MathTool.GetRandom( 1 , #lst )
        if lst[part].middle_name then
            key[2] = lst[part].middle_name
        else
            key[2] = ""
        end

        local part = MathTool.GetRandom( 1 , #lst )
        if lst[part].last_name then
            key[3] = lst[part].last_name
        else
            key[3] = ""
        end
        return key
    end

    local num = 2
    local text
    while num >= 2 do
        num = 0
        text = ""
        local lst = Group()
        for __ , v in ipairs(lst) do
            if v == "" then
                num = num + 1
            else
                text = text .. v
            end
        end

        local _, is_pass = LanguageFilter.NameFilter(text)
        if not is_pass then
            num = 2
        end
    end

    return text
end

--过滤竞技场机器人名字
function Modules.SundryModule:IsNameConflitWithArenaRobot( name )
    local lst = {}
    for k,v in pairs( CPPGameLib.GetConfig("ArenaRobot", nil, nil , true ) ) do
        lst[v.name] = true
    end
    return lst[name]
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 新一天的通知 消息Id: 20022
function Modules.SundryModule:NewDayNotifyResp( protocol_data )
	self:FireNextFrame(Macros.Event.SundryModule.NEW_DAY_NOTIFY)
end