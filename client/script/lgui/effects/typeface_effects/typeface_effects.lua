--
-- @brief 字体特效
-- @author: yjg
-- @date: 2016年8月9日17:23:47
--

TypefaceEffects = TypefaceEffects or BaseClass()

function TypefaceEffects:__init()

end

function TypefaceEffects:__delete()
	if self.onlyOne then		
		GlobalTimerQuest:CancelQuest(self.onlyOne)
		self.onlyOne = nil
	end

	if self.rollex_timer then
		GlobalTimerQuest:CancelQuest(self.rollex_timer)
		self.rollex_timer = nil
	end
end

function TypefaceEffects:Roll(node, nexLab, biko, fun, size, speed, is_front, txt_color, txt_color2)
	CPPActionManager.ScaleTo(node, 0 , 1)
	
	size = size or 1.6

	speed = speed or 0.001

	if self.onlyOne then
		GlobalTimerQuest:CancelQuest(self.onlyOne)
		self.onlyOne = nil
	end
	CPPActionManager.ScaleTo( node, 0.2 , size )
	--现在的Lab
	local nowLab = Util:IsText(node) and node:GetText() or node:GetString()
	--先搞成字符串的
	nowLab = tostring( nowLab )
	nexLab = tostring( nexLab )

	--把初始搞成tab
	local nowTab = {}
	for i = 1 , string.len(nowLab) do
		local t = string.sub(nowLab, i , i+1 )
		table.insert(nowTab , 1 , { num = string.sub(t,0,1) , type = "num"} )
	end
	
	--把目标搞成tab
	local nexTab = {}
	for i = 1 , string.len(nexLab) do
		local t = string.sub(nexLab, i , i+1 )
		table.insert(nexTab , 1 , { num = string.sub(t,0,1) , type = "num"} )
	end

	--把这初始成数字的
	local t1 = nowTab
	for i,v in ipairs(t1) do
		nowTab[i].num = tonumber(v.num) or v.num
		nowTab[i].type = v.type
	end
	--把这目标成数字的
	local t1 = nexTab
	for i,v in ipairs(nexTab) do
		nexTab[i].num = tonumber(v.num) or v.num
		nexTab[i].type = v.type
	end

	--循环次数
	local sum = #nexTab
	--如果现在小于未来
	if #nowTab > #nexTab then
		--把总数设成现在
		sum = #nowTab 
		local t1 = nexTab
		for i=1,#nowTab - #t1 do
			table.insert(nexTab , { num = 0 , type = "false"} )
		end
	end

	--计数器
	local count = 0
	local function SecondsCounter( ... )
		local key = false

		for i = 1 , sum do
			if nowTab[i] and nexTab[i] and nowTab[i].type == nexTab[i].type and nowTab[i].num == nexTab[i].num then
			else
				key = true
				break
			end
		end

		--如果现在不是未来
		if key == true then
			--每次加1
			--内置循环
			self.onlyOne = GlobalTimerQuest:AddPeriodQuest(
				GlobalCallbackMgr:AddCallBackFunc(function()					
					for count = 1 , sum do
						--如果有现在 而且有未来
						if nowTab[count] and nowTab[count].num and nexTab[count] and nexTab[count].num then
							--不管
						end
						if not nowTab[count] and nexTab[count] and nexTab[count].num then
							nowTab[count] = {}
							nowTab[count].num = 0
							nowTab[count].type = "num"
						end

						--如果现在不是数字 而未来是数字
						if nowTab[count] and (type( nowTab[count].num ) ~= "number") and nexTab[count] and (type( nexTab[count].num ) == "number") then
							--把现在搞成数字
							nowTab[count].num = 0
						--如果现在是数字 而未来不是数字
						elseif nowTab[count] and (type( nowTab[count].num ) == "number") and nexTab[count] and (type( nexTab[count].num ) ~= "number") then
							nowTab[count].num = nexTab[count].num
						end

						--如果现在是数字 而且未来也是数字
						if nowTab[count] and (type( nowTab[count].num ) == "number") and nexTab[count] and (type( nexTab[count].num ) == "number") then
							--如果现在大过未来 
							if nowTab[count].num > nexTab[count].num then
								nowTab[count].num = nowTab[count].num - 1
							--如果现在小过未来 
							elseif nowTab[count].num < nexTab[count].num then
								nowTab[count].num = nowTab[count].num + 1
							--如果一模一样
							elseif nowTab[count].num == nexTab[count].num then
								nowTab[count].type = nexTab[count].type
								GlobalTimerQuest:CancelQuest(self.onlyOne)
								self.onlyOne = nil
								SecondsCounter()
							end
						else
							nowTab[count].num = nexTab[count].num 
							nowTab[count].type = nexTab[count].type

							GlobalTimerQuest:CancelQuest(self.onlyOne)
							self.onlyOne = nil
							SecondsCounter()
						end

						local lab = ""
						for i,v in ipairs(nowTab) do
							if v.type ~= "false" then
								lab = v.num .. lab
							end
						end
						local str
						if Util:IsText(node) then														
							if is_front then
								if txt_color then
									local str_prop_value,str_prop_type
									str_prop_type = GUI.RichText.GetColorWord(( biko or "" ), txt_color)
									str_prop_value = GUI.RichText.GetColorWord(lab,txt_color2)
									str = string.format("%s%s", str_prop_type, str_prop_value)
								else
									str = ( biko or "" )..lab
								end
								node:SetText(str)
							else
								node:SetText(lab .. ( biko or "" ) )
							end							
						else
							if is_front then
								node:SetString(( biko or "" )..lab)
							else
								node:SetString(lab .. ( biko or "" ) )
							end
						end
					end
			end) , speed , -1)
		else

			CPPActionManager.ScaleTo(node, 0.2 , 1 )
			if fun then
				fun()
			end
		end
	end 
	SecondsCounter()

	return false
end

--滚字特效(old_value和new_value只支持整数)
function TypefaceEffects:RollEx(node, old_value, new_value, total_time, update_interval, assemble_func, finished_callback)
	update_interval = update_interval or 1 / Macros.Global.FPS_Default

	if not node or not old_value or not new_value or not total_time or old_value == new_value or not assemble_func then
		if finished_callback then
			finished_callback()
		end
		return
	end

	local pass_time = 0
	local cur_value = 0
	--变化方向
	local change_dir = new_value - old_value
	--是否使用SetText
	local is_text_widget = Util:IsText(node)

	--组装文本&更新控件
	local function update_text_func(var)
		local text = assemble_func(var)
		if is_text_widget then
			node:SetText(text or "")
		else
			node:SetString(text or "")
		end
	end

	if self.rollex_timer then
		GlobalTimerQuest:CancelQuest(self.rollex_timer)
	end	
	self.rollex_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(function()
		local change_var = new_value - old_value
		pass_time = math.min(pass_time + update_interval, total_time)
		cur_value = old_value + pass_time / total_time * change_var
		cur_value = change_var > 0 and math.ceil(cur_value) or math.floor(cur_value)

		--更新控件
		update_text_func(cur_value)

		--检测结束
		if pass_time >= total_time or 
			(change_dir > 0 and cur_value >= new_value) or
			(change_dir < 0 and cur_value <= new_value) then

			--更新控件
			update_text_func(new_value)

			GlobalTimerQuest:CancelQuest(self.rollex_timer)
			self.rollex_timer = nil
			if finished_callback then
				finished_callback()
			end
		end 

	end), update_interval, -1)
end

--小小的飘字 +1s这种
function TypefaceEffects:LittleFloat( text , color , time , pox , poy , fun , apx,is_fadeout,text_size)
	local lab = Util:RichText( text , text_size or Macros.TypefaceSize.minimum ,0,0, color or Macros.Color.green_hex )
	lab:SetAnchorPoint(apx or 0.5,0.5)

    local function moveAnimate( ... )
    	lab:RemoveSelf(true)
    	lab = nil
    	if fun then
    		fun()
		end
    end	    

	local func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
	CPPActionManager.MoveBy(lab, time , pox , poy ,func)
	if is_fadeout then
		CPPActionManager.FadeOut(lab,time)
	end
	return lab
end

--装备或者宝具的公用地光
function TypefaceEffects:Spotlight( father , node , quality )
	local text = {
		[1] = "UI_zhuangbeiqianghua_lvse",
		[2] = "UI_zhuangbeiqianghua_lanse",
		[3] = "UI_zhuangbeiqianghua_zise",
		[4] = "UI_zhuangbeiqianghua_chense",
		[5] = "UI_zhuangbeiqianghua_hongse",
	}
	local effect_id , effect = father:__CreateEffect({res_name = text[quality] }, node)
	effect:SetScaleFactor(1.2)
	effect:SetPosition( node:GetContentWidth()/2 , node:GetContentHeight()/2 - 110 )

	return effect_id , effect
end

--停止滚字
function TypefaceEffects:StopAllRolling()
	if self.rollex_timer then
		GlobalTimerQuest:CancelQuest(self.rollex_timer)
		self.rollex_timer = nil
	end
end

--[[
%% @module: 战力变化特效工具
--]]
BattlePowerChangedEffectTool = BattlePowerChangedEffectTool or BaseClass()

function BattlePowerChangedEffectTool:__init( now , new )
end

--处理战力变化
function BattlePowerChangedEffectTool:HandleBattlePowerChanged( now , new )
	if not now or not new then return end
	self.now = self.now or now
	self.new = new

	if self.__in_delay_show_status then 
		return
	end

	self:__ShowBattlePowerChangedEffect()
end

--记录战力变化前后
function BattlePowerChangedEffectTool:RecordBattlePowerChanged( type, is_changed )
	if not is_changed then
		--进入延时展示战力变化特效状态
		self.__in_delay_show_status = true

		--容错处理(4秒后解除此状态)
		if self.__cancel_delay_show_mode_timer then
			GlobalTimerQuest:CancelQuest(self.__cancel_delay_show_mode_timer)
		end
		self.__cancel_delay_show_mode_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
			self.__cancel_delay_show_mode_timer = nil
			self.__in_delay_show_status = nil
		end), 4)
		
		return 
	end

	self:__ShowBattlePowerChangedEffect()
end

--展示战力变化特效
function BattlePowerChangedEffectTool:__ShowBattlePowerChangedEffect()
	if self.now and self.new and self.now < self.new then
		LogicGUI.BattlePowerChangedEffect.New(self.now , self.new)	
	end
	
	self.now = nil
	self.new = nil
	self.__in_delay_show_status = nil

	if self.__cancel_delay_show_mode_timer then
		GlobalTimerQuest:CancelQuest(self.__cancel_delay_show_mode_timer)
		self.__cancel_delay_show_mode_timer = nil
	end
end

--删除
function BattlePowerChangedEffectTool:__delete()
	if self.__cancel_delay_show_mode_timer then
		GlobalTimerQuest:CancelQuest(self.__cancel_delay_show_mode_timer)
		self.__cancel_delay_show_mode_timer = nil
	end
end

--[[
%% @module: 战力变化特效
--]]
LogicGUI = LogicGUI or {}

LogicGUI.BattlePowerChangedEffect = LogicGUI.BattlePowerChangedEffect or BaseClass(GUI.Template)

function LogicGUI.BattlePowerChangedEffect:__init( now , new , fun )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self.__render_queue = Scene.RenderQueue.GRQ_UI_UP_II
    self:InitTemplate(nil)  

	self:__ShowUI(now, new, fun)
end

function LogicGUI.BattlePowerChangedEffect:__delete()
end

function LogicGUI.BattlePowerChangedEffect:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( Macros.Global.DesignSize.width , Macros.Global.DesignSize.height )
    self.layout:SetClippingEnable(false)
    self.layout:SetAnchorPoint(0.5,0.5)
    return self.layout
end

function LogicGUI.BattlePowerChangedEffect:__ShowUI( now , new  , fun )
	local second = 2.5
	CPPActionManager.DelayTo(self.layout , second , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		self:DeleteMe()
		if fun then
			fun()
		end
	end))

	local effectid,effect = self:__CreateEffect( {id = 100048 ,loop = false} , self.layout)
	effect:SetAnchorPoint(0.5,0.5)

	local viewInfo = GlobalViewMgr:GetCurViewInfo()
	local x , y = self.layout:GetContentWidth()/2 , self.layout :GetContentHeight()/2

	if viewInfo then		

		if viewInfo.view_type == BaseViewType.HERO_LINEUP then
			x , y = self.layout:GetContentWidth()/2 , self.layout :GetContentHeight()/2 + 236

		elseif viewInfo.view_type == BaseViewType.HERO_PROMOTE then
			x , y = self.layout:GetContentWidth()/2 , self.layout :GetContentHeight()/2 + 175

		elseif viewInfo.view_type == BaseViewType.EQUIPINTENSIFY then
			x , y = self.layout:GetContentWidth()/2  + 65 , self.layout :GetContentHeight()/2 + 170

		elseif viewInfo.view_type == BaseViewType.TREASURE_LIFT then
			x , y = self.layout:GetContentWidth()/2  + 65 , self.layout :GetContentHeight()/2 + 170

		elseif viewInfo.view_type == BaseViewType.WORLD_HEART then
			x , y = self.layout:GetContentWidth()/2 , self.layout :GetContentHeight()/2 + 170

		elseif viewInfo.view_type == BaseViewType.UNION_SKILL then
			x , y = self.layout:GetContentWidth()/2 , self.layout :GetContentHeight()/2 + 170

		elseif viewInfo.view_type == BaseViewType.SPIRIT then
			x , y = self.layout:GetContentWidth()/2 , self.layout :GetContentHeight()/2 + 170

		elseif viewInfo.view_type == BaseViewType.KNIGHTHOOD then
			x , y = self.layout:GetContentWidth()/2 , self.layout :GetContentHeight()/2 + 170

		elseif viewInfo.view_type == BaseViewType.GEM then
			x , y = self.layout:GetContentWidth()/2 , self.layout :GetContentHeight()/2 + 170

		elseif viewInfo.view_type == BaseViewType.DIVINATION_COLLECT then
			x , y = self.layout:GetContentWidth()/2 , self.layout :GetContentHeight()/2 + 170

		elseif viewInfo.view_type == BaseViewType.DIVINATION_BOOK then
			x , y = self.layout:GetContentWidth()/2 , self.layout :GetContentHeight()/2 + 170
			
		elseif viewInfo.view_type == BaseViewType.DIVINATION_INTENSIFY then
			x , y = self.layout:GetContentWidth()/2 , self.layout :GetContentHeight()/2 + 170

		elseif viewInfo.view_type == BaseViewType.EQUIP_UP_STAR then
			x , y = self.layout:GetContentWidth()/2 , self.layout :GetContentHeight()/2 + 170

			
		end
	end

	effect:SetPosition( x , y )

	self.layer = Util:Layout( 0 , self.layout:GetContentWidth() )
	self.layer:SetAnchorPoint( 0.5 , 0.5 )
	self.layer:SetPosition( 0 , y)
	self.layout:AddChild(self.layer)

	--战斗力图样
    local battle_powerSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "battle_value_big_1", true)
    local battle_power = Util:Sprite(battle_powerSp)

    --第一次设置大小
    self.layer:SetContentSize( self.layer:GetContentWidth() + battle_power:GetContentWidth() , self.layer:GetContentHeight() )

    --战斗力
	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    local power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "battle_num_1", true))
    power_font:SetText(tostring(userDate.battle_value))

    --第二次设置大小
    self.layer:SetContentSize( self.layer:GetContentWidth() + power_font:GetNode():GetContentWidth() + 5 , self.layer:GetContentHeight() )

	local effectid,effect = self:__CreateEffect({res_name = "UI_zhanli2"} , self.layer , 999 )
	effect:SetAnchorPoint(0.5,0.5)
	PosTool.Center( effect )
	
    --战斗图样
    battle_power:SetAnchorPoint(0,0.5)
    battle_power:SetPosition(0,self.layer:GetContentHeight()/2)
    self.layer:AddChild(battle_power)

    --战斗力
    self.layer:AddChild(power_font:GetNode())
    PosTool.RightTo( battle_power , power_font , 5 )


	local icon_upgrade_tips = Util:Layout( 0 , self.layout:GetContentWidth() )
	icon_upgrade_tips:SetAnchorPoint( 0 , 0 )
	icon_upgrade_tips:SetPosition( self.layer:GetContentWidth() + 5 , .0 )
	self.layer:AddChild(icon_upgrade_tips , 10 )
	icon_upgrade_tips:SetOpacity( 0 )

	local num = 0
	-- local img = "icon_upgrade_tips"
	local imgType = GUI.ImgFont.ImgType.Num3
	local numImg = "battle_num_4"
	if now > new then
		local big = now
		local tiny = new
		local index = now
		num = big - tiny
		local time = num/(second*10)

		num = "-"..num
		local function moveAnimate( ... )
			index = index - time
			if index > new then
				power_font:SetText(tostring( math.floor( index ) ))
			else
				power_font:SetText(tostring( math.floor( new ) ))
				self:SetUpdateFunc( nil )
		    end
		end
		self:SetUpdateFunc( moveAnimate )
		imgType = GUI.ImgFont.ImgType.Num3
		numImg = "battle_num_3"
	else
		local big = new
		local tiny = now
		local index = now
		num = big - tiny
		local time = num/(second*10)
		num = "+"..num
		local function moveAnimate( ... )
			index = index + time
			if index < new then
				power_font:SetText(tostring( math.floor( index ) ))
			else
				power_font:SetText(tostring( math.floor( new ) ))
				self:SetUpdateFunc( nil )
		    end
		end
		self:SetUpdateFunc( moveAnimate )
	end

    local battle_up_num = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, numImg , true) , imgType )
    battle_up_num:SetAnchorPoint(0, 0.5)
    battle_up_num:SetText(tostring(num))
    battle_up_num:SetPosition( icon_upgrade_tips:GetContentWidth() , icon_upgrade_tips:GetContentHeight()/2 )
    icon_upgrade_tips:AddChild(battle_up_num:GetNode())

	CPPActionManager.MoveTo( self.layer , 0.2 , x , y, GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		ActionManager.GetInstance():BlinkEffect( icon_upgrade_tips , 0.2 , 0.6 )
	end) )
end