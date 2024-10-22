
GUI = GUI or {}

-- 进度条
-- img_bar -- 进度条
-- img_bk -- 底层背景
-- max_pos -- 最大值
-- cur_pos -- 当前值
GUI.ProgressBarLayer = GUI.ProgressBarLayer or BaseClass(UILayer)

function GUI.ProgressBarLayer:__init(img_bar, img_bk, cur_pos, max_pos)
	self.cur_pos = cur_pos or 0
	self.max_pos = max_pos or 100

	-- 进度条
	self.__bar = CPPProgressControl.CreateWithImage(img_bar, img_bk or CPPImageData())
	self.__bar:SetPosition(0, 0)
	self:AddChild(self.__bar)

	self:__SetProgressMax(self.max_pos)
	self:__SetProgress(self.cur_pos)
	self:SetSpeed(self.max_pos)

	self:SetUpdateFunc(CPPGameLib.Handler(self, self.Update))	
end

function GUI.ProgressBarLayer:__delete()
end

-- 更新动态进度条长度变化
function GUI.ProgressBarLayer:Update(elapse_time)
	if self.__smooth_end_time and self.__smooth_end_time > 0 then
		self.__smooth_elapse_time = self.__smooth_elapse_time + elapse_time
		self.cur_pos = self.__smooth_beg_pos + self.__smooth_cur_speed * self.__smooth_elapse_time

		if CPPGameEngine:GetNowTime() - self.__smooth_end_time >= 0 then
			self.__smooth_end_time = 0
			self.__smooth_cur_speed = 0
			self.cur_pos = self.tgt_pos

			if self.progress_end_callback then
				self.progress_end_callback()
			end			
		end

		self:__SetProgress(self.cur_pos)
	end
end

-- 设置最大值
function GUI.ProgressBarLayer:SetProgressMax(max_pos)
	self.cur_pos = 0
	self.tgt_pos = 0
	self.max_pos = max_pos
	self.__smooth_end_time = 0

	self:__SetProgressMax(self.max_pos)
	self:__SetProgress(self.cur_pos)
	self:SetSpeed(self.max_pos)
end

-- 设置进度
function GUI.ProgressBarLayer:SetProgress(pos)
	if self.max_pos == 0 then
		return
	end

	self.cur_pos = pos
	self.__smooth_end_time = 0
	self:__SetProgress(pos)
end

-- 设置移动到某个进度
function GUI.ProgressBarLayer:SetToProgress(pos)
	if self.max_pos == 0 then
		return
	end

	-- 计算动画数据
	local diff = pos - self.cur_pos
	if math.abs(diff) > 0 then
		if diff < 0 then
			self.__smooth_cur_speed = -self.__smooth_abs_sub_speed
		else
			self.__smooth_cur_speed = self.__smooth_abs_add_speed
		end
		self.__smooth_beg_pos = self.cur_pos
		self.__smooth_elapse_time = 0		
		self.__smooth_end_time = CPPGameEngine:GetNowTime() + diff / self.__smooth_cur_speed
	end

	-- 赋值
	self.tgt_pos = pos
end

--绝对速度(单位/秒)
function GUI.ProgressBarLayer:SetSpeed(add_speed, sub_speed)
	add_speed = add_speed or 1	
	self.__smooth_abs_add_speed = add_speed
	self.__smooth_abs_sub_speed = sub_speed or add_speed
end

function GUI.ProgressBarLayer:SetFlipX(is_flip)
	if is_flip then
		self.__bar:SetScaleXFactor(-1)
	else
		self.__bar:SetScaleXFactor(1)
	end
end

function GUI.ProgressBarLayer:SetProgressEndCallback(callback)
	self.progress_end_callback = callback
end

function GUI.ProgressBarLayer:GetBar()
	return self.__bar
end

function GUI.ProgressBarLayer:GetBarBG()
	return self.__bar_bg
end

function GUI.ProgressBarLayer:__SetProgressMax(pos)
	self.__bar:SetProgressMax(pos)
end

function GUI.ProgressBarLayer:__SetProgress(pos)
	self.__bar:SetProgress(math.min(self.max_pos, pos))
end

----------------------------------------------------------------------
-- 进度条(9宫格拉伸)
--[[
@ params：
	必须：
	cur - 初始进度
	max - 最大进度(默认=宽度)
	img_bar - 进度图片
	img_bg - 背景图片
	bar_width - 进度条宽度
	bar_height - 进度条高度

	可选：
	bk_width - 背景宽度
	bk_height - 背景高度
--]]
GUI.ProgressBar9Layer = GUI.ProgressBar9Layer or BaseClass(UILayer)

function GUI.ProgressBar9Layer:__init(img_bar, img_bg, bar_width, bar_height, bk_width, bk_height, cur, max)
	self.cur_pos = cur or 0
	self.max_pos = max or 100

	bk_width = bk_width or bar_width
	bk_height = bk_height or bar_height
	self:SetContentSize(bk_width, bk_height)

	-- 背景框
	if img_bg ~= nil then
		self.__bar_bg = Util:Sprite9(img_bg, {10, 10})
		self.__bar_bg:SetContentWidth(bk_width)
		self.__bar_bg:SetContentHeight(bk_height)
		self:AddChild(self.__bar_bg)
		PosTool.Center(self.__bar_bg)
	end

	-- 进度条
	self.__bar = CPPProgress9Control.CreateWithImage(img_bar)
	self.__bar:SetContentWidth(bar_width)
	self.__bar:SetContentHeight(bar_height)
	self.__bar:SetPosition((bk_width - bar_width) / 2, (bk_height - bar_height) / 2)
	self:AddChild(self.__bar)

	self:__SetProgressMax(self.max_pos)
	self:__SetProgress(self.cur_pos)
	self:SetSpeed(self.max_pos)

	self:SetUpdateFunc(CPPGameLib.Handler(self, self.Update))	
end

function GUI.ProgressBar9Layer:__delete()
end

-- 更新动态进度条长度变化
function GUI.ProgressBar9Layer:Update(elapse_time)
	if self.__smooth_end_time and self.__smooth_end_time > 0 then
		self.__smooth_elapse_time = self.__smooth_elapse_time + elapse_time
		self.cur_pos = self.__smooth_beg_pos + self.__smooth_cur_speed * self.__smooth_elapse_time

		if CPPGameEngine:GetNowTime() - self.__smooth_end_time >= 0 then
			self.__smooth_end_time = 0
			self.__smooth_cur_speed = 0
			self.cur_pos = self.tgt_pos

			if self.progress_end_callback then
				self.progress_end_callback()
			end
		end
		
		self:__SetProgress(self.cur_pos)
	end
end

-- 设置进度(0-100)
function GUI.ProgressBar9Layer:SetProgress(cur)
	if self.max_pos == 0 then
		return
	end

	self.cur_pos = cur or 0
	self.__smooth_end_time = 0
	self:__SetProgress(self.cur_pos)
end

-- 设置最大值
function GUI.ProgressBar9Layer:SetProgressMax(max_pos)
	self.cur_pos = 0
	self.tgt_pos = 0
	self.max_pos = max_pos or 100
	self.__smooth_end_time = 0

	self:__SetProgressMax(self.max_pos)
	self:__SetProgress(self.cur_pos)
	self:SetSpeed(self.max_pos)
end

-- 设置移动到某个进度
function GUI.ProgressBar9Layer:SetToProgress(pos)
	pos = pos or 0

	if self.max_pos == 0 then
		return
	end

	-- 计算动画数据
	local diff = pos - self.cur_pos
	if math.abs(diff) > 0 then
		if diff < 0 then
			self.__smooth_cur_speed = -self.__smooth_abs_sub_speed
		else
			self.__smooth_cur_speed = self.__smooth_abs_add_speed
		end
		self.__smooth_beg_pos = self.cur_pos
		self.__smooth_elapse_time = 0		
		self.__smooth_end_time = CPPGameEngine:GetNowTime() + diff / self.__smooth_cur_speed
	end

	-- 赋值
	self.tgt_pos = pos
end

--绝对速度(单位/秒)
function GUI.ProgressBar9Layer:SetSpeed(add_speed, sub_speed)
	add_speed = add_speed or 1
	self.__smooth_abs_add_speed = add_speed
	self.__smooth_abs_sub_speed = sub_speed or add_speed
end

function GUI.ProgressBar9Layer:SetProgressEndCallback(callback)
	self.progress_end_callback = callback
end

function GUI.ProgressBar9Layer:SetFlipX(is_flip)
	if is_flip then
		self.__bar:SetScaleXFactor(-1)
	else
		self.__bar:SetScaleXFactor(1)
	end
end

function GUI.ProgressBar9Layer:GetBar()
	return self.__bar
end

function GUI.ProgressBar9Layer:GetBarBG()
	return self.__bar_bg
end

function GUI.ProgressBar9Layer:StartPreviewEffect(preview_progress, preview_imgdata)
	if not self.__bar_blink then
		local bk_size = self:GetContentSize()
		local bar_size = self.__bar:GetContentSize()
	    self.__bar_blink = Util:Sprite9(preview_imgdata)
		self.__bar_blink:SetContentWidth(bar_size.width)
		self.__bar_blink:SetContentHeight(bar_size.height)
		self.__bar_blink:SetAnchorPoint(0,0.5)		
		self:AddChild(self.__bar_blink, 0)
		PosTool.LeftCenter(self.__bar_blink, self.__bar:GetPositionX())	

		if self.__bar then
			self.__bar:SetZOrder(2)	
		end
	end
	self.__bar_blink:SetScaleXFactor(math.min(preview_progress, self.max_pos) / self.max_pos)

	ActionManager.GetInstance():CycleBlinkEffect(self.__bar_blink, 0.5)
end

function GUI.ProgressBar9Layer:StopPreviewEffect()
	if self.__bar_blink then
		self.__bar_blink:RemoveSelf(true)
		self.__bar_blink = nil
	end
end

function GUI.ProgressBar9Layer:SetBarOffset(offset_x, offset_y)
	offset_x = offset_x or 0
	offset_y = offset_y or 0
	
	local pos_x, pos_y = self.__bar:GetPosition()
	self.__bar:SetPosition(pos_x + offset_x, pos_y + offset_y)
end

function GUI.ProgressBar9Layer:__SetProgressMax(pos)
	self.__bar:SetProgressMax(pos)
end

function GUI.ProgressBar9Layer:__SetProgress(pos)
	self.__bar:SetProgress(math.min(self.max_pos, pos))
end

----------------------------------------------------------------------
-- 特效进度条
--[[
@ params：
	必须：
	cur - 初始进度
	max - 最大进度(默认=宽度)
	img_bar - 进度图片
	img_bg - 背景图片
	bar_width - 进度条宽度
	bar_height - 进度条高度

	可选：
	bk_width - 背景宽度
	bk_height - 背景高度
--]]
GUI.EffectsProgressBarLayer = GUI.EffectsProgressBarLayer or BaseClass(UILayer)

function GUI.EffectsProgressBarLayer:__init( effects , bgImg , width , height , cur , max )

	--背景
	self.bg = Util:Sprite( bgImg )

	self.progress_num = cur or 0
	self.max = max or 100
	self.width = width 
	self.height = height

	--背景大小
	self:SetContentSize( self.bg:GetContentWidth() , self.bg:GetContentHeight() )
	self:SetAnchorPoint( 0.5 , 0.5 )
	self.bg:SetAnchorPoint( 0.5 , 0.5  )
	self:AddChild( self.bg )
	PosTool.Center( self.bg )

	--容器
	self.bgLayer = Util:Layout( self.width , self.height )
	self.bgLayer:SetAnchorPoint( 0 , 0.5 )
	self.bgLayer:SetClippingEnable( true )
	self.bg:AddChild( self.bgLayer )
	PosTool.Center( self.bgLayer , 0 - (self.bgLayer:GetContentWidth()/2) )

    --特效进度条
	local params = {}
	if type(effects) == "string" then
		params = {res_name = effects }
	elseif  type(effects) == "number" then
		params = {id = effects }
	end

	local effectsCon = nil
	self.effectsId , effectsCon = GlobalEffectMgr:CreateUIEffect( params, self.bgLayer )
	effectsCon:SetPosition( self.bgLayer:GetContentWidth()/2 , self.bgLayer:GetContentHeight()/2 )
	
	self:SetProgress( self.progress_num )
end

function GUI.EffectsProgressBarLayer:__delete()
	if self.effectsId then
		GlobalEffectMgr:DelEffect(self.effectsId)
		self.effectsId = nil
	end
end

-- 设置进度(0-100)
function GUI.EffectsProgressBarLayer:SetProgress(cur)
	cur = cur or 0

	if not self.max or self.max == 0 then
		return
	end
	--特效长度
	local maxSize = self.width
	--百分比
	local percent = cur / self.max * maxSize
	self.bgLayer:SetContentSize( percent , self.bgLayer:GetContentHeight() )

	self.progress_num = cur
end

-- 设置最大值
function GUI.EffectsProgressBarLayer:SetProgressMax(max_pos)
	self.cur_pos = 0
	self.max = max_pos or 100
end

-- 设置移动到某个进度
function GUI.EffectsProgressBarLayer:SetToProgress(cur)
	cur = cur or 0
	
	self.bgLayer:SetContentSize( percent , self.bgLayer:GetContentHeight() )
	--是大于0还是少于0
	self.toProgressType = cur - self.progress_num
	--保存自己
	self.toProgress = cur
	--特效长度
	local maxSize = self.width
	--应该要移动到的百分比
	self.percent = self.toProgress  / self.max * 100
	print("xy" , "percent" , self.percent )
	--获取现在的进度的百分比
	self.nowPercent = self.bgLayer:GetContentWidth() / self.width * 100
	print("xy" , "nowPercent" , self.nowPercent )
	--获得实际上应该要移动的百分比
	local truePercent = self.percent - self.nowPercent
	--每帧移动
	self.division = 1200 / truePercent
	self:SetUpdateFunc(CPPGameLib.Handler(self, self.Update))	
end

function GUI.EffectsProgressBarLayer:Update(  )
	--如果一样了
	if self.percent <= self.nowPercent then
		self:SetUpdateFunc()
		return 
	end

	--增加的 
	if self.toProgressType > 0 then
		self.bgLayer:SetContentSize( self.bgLayer:GetContentWidth() + self.division  , self.bgLayer:GetContentHeight() )
	else --减少的 
		self.bgLayer:SetContentSize( self.bgLayer:GetContentWidth() - self.division  , self.bgLayer:GetContentHeight() )
	end
	self.nowPercent = self.bgLayer:GetContentWidth() / self.width * 100
end

function GUI.EffectsProgressBarLayer:SetBarOffset(offset_x, offset_y)
	offset_x = offset_x or 0
	offset_y = offset_y or 0

	local pos_x, pos_y = self.bgLayer:GetPosition()
	self.bgLayer:SetPosition(pos_x + offset_x, pos_y + offset_y)
end
