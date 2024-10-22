--
-- @brief 飘字特效
-- @author: ldx
-- @date: 2016年12月28日16:54:47
--

WindWordsEffects = WindWordsEffects or BaseClass()

function WindWordsEffects:__init()

end

function WindWordsEffects:__delete()
	--self:__RestoreStatus()
	if self.layou then
		self.layou:RemoveSelf(true)
		self.layou = nil
	end

end

--[[
-- tbl 每个info里都有节点,飘字类型,目标位置
-- info.node 
-- info.anim_type game里面 WIND_WORDS_TYPE
-- info.tgt_pos  {x = 1,y = 1}
-- fun 回调函数
]]

--飘字效果
function WindWordsEffects:Wind(tbl,fun)
	if not tbl then
		return
	end
	self.__funId =  GlobalCallbackMgr:AddCallBackFunc(function ()
		self:__RestoreStatus(fun)
	end)
	for i,node_info in ipairs(tbl) do
		node_info.original_pos = {x = node_info.node:GetPositionX(),y = node_info.node:GetPositionY()}
		if i == #tbl then
			self:__RunActionByMoveType(node_info,true)
		else
			self:__RunActionByMoveType(node_info)
		end		
	end
	self.__node_info_tbl = tbl	
end

--根据类型来移动
function WindWordsEffects:__RunActionByMoveType(node_info,is_callback)
	node_info.node:SetOpacity(255)
	CPPActionManager.StopAllActions(node_info.node)
	node_info.node:SetScaleFactor(0.8)
	node_info.node:SetVisible(true)
	local funid
	if is_callback then
		funid = self.__funId
	end
	if node_info.anim_type == Macros.Game.WIND_WORDS_TYPE.UPSLOPE then
		ActionManager.GetInstance():WindWordsUpslopeEffect(node_info.node,node_info.tgt_pos,funid)
	else
		ActionManager.GetInstance():WindWordsToLocationEffect(node_info.node,node_info.tgt_pos,funid)
	end
end

--还原原来的状态
function WindWordsEffects:__RestoreStatus(fun)
	for i,node_info in ipairs(self.__node_info_tbl) do
		node_info.node:SetScaleFactor(1)
		node_info.node:SetVisible(false)
		node_info.node:SetPosition(node_info.original_pos.x,node_info.original_pos.y)
		if node_info.anim_type == Macros.Game.WIND_WORDS_TYPE.UPSLOPE then
			CPPActionManager.FadeIn(node_info.node,0)
		end
	end	
	if fun then
		fun()
	end
	self.__is_restore = true
end

--分段创建
function WindWordsEffects:Subsection( selfNode , posNode , tab , interval , fun )
	local time = 0.2
	interval = interval or 0
	self.layou = Util:Layout( 200 , 100 )
	self.layou:SetAnchorPoint(0.5,0.5)
	selfNode:AddChild(self.layou)
	PosTool.Center(self.layou)

	local ii = 1
	local layer = {}
	for big_i , tab_v in ipairs( tab ) do
		--创建板子
		layer[big_i] = Util:Layout( 100 , 100 )
	    layer[big_i]:SetAnchorPoint(0.5 , 0.5 )
		self.layou:AddChild(layer[big_i])

		--设置板子大小
		local height = 0
		for i,v in ipairs(tab_v.lst) do
			height = height + v.node:GetContentHeight() + v.interval
		end
		layer[big_i]:SetContentSize( 100 , height )

		--设置控件
		for i,v in ipairs(tab_v.lst) do
			local y = layer[big_i]:GetContentHeight() - ( v.node:GetContentHeight() / 2 )
			if tab_v.lst[i-1] then
				y = tab_v.lst[i-1].node:GetPositionY() - ( tab_v.lst[i].node:GetContentHeight() ) - tab_v.lst[i-1].interval
			end
			v.node:SetAnchorPoint(0.5,0.5)
			v.node:SetPosition( layer[big_i]:GetContentWidth()/2 , y )
			v.node:SetTag(ii)
			layer[big_i]:AddChild(v.node)
			ii = ii + 1
		end
	end

	local tall = 0
	for i=1 , #layer do
		tall = tall + layer[i]:GetContentHeight()
	end

	self.layou:SetContentSize( 200 , tall + ( interval * (#layer - 1) ) )
	local interval_ = self.layou:GetContentHeight() / #layer
	local y = self.layou:GetContentHeight()
	for i=1 , #layer do
		if layer[i-1] then
			y = layer[i-1]:GetPositionY() - (layer[i-1]:GetContentHeight()/2) - interval 
		end
		layer[i]:SetPosition( self.layou:GetContentWidth()/2 , y - layer[i]:GetContentHeight()/2)
	end

	local key = true

	local function moveAnimate_( ... )
		if fun then
			fun()
		end
		self.layou:RemoveSelf(true)
		self.layou = nil
	end

	--动作组
    local function moveAnimate(tab_i)
    	-- 大模板
    	-- for tab_i , tab_v in ipairs(tab) do
    	local tab_v = tab[tab_i]
    		--向上
    		if tab_v.type == Macros.Game.WIND_WORDS_TYPE.UPSLOPE then
    			key = true
    			--大模板
    			--移动
    			CPPActionManager.MoveBy( layer[tab_i] , time , 0 , 50 )
    			--消失
    			CPPActionManager.FadeOut( layer[tab_i] , time)
    		--自定义
    		elseif tab_v.type == Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION then
    			key = true
    			--如果有坐标
    			if tab_v.pos then
    				key = false
	    			local pos = NodeUnity.ConvertPosBetweenTwoContainer( selfNode , layer[tab_i] ,  COCOPoint( tab_v.pos.x , tab_v.pos.y ) )
	    			CPPActionManager.MoveTo( layer[tab_i] , time , pos.x , pos.y )
	    		end
    		end
    		if key == true then
	    		--小模板
	    		for i,v in ipairs(tab_v.lst) do
	    			if v.pos then
						local pos = NodeUnity.ConvertPosBetweenTwoContainer( posNode ,  layer[tab_i], COCOPoint( v.pos.x , v.pos.y )  )
		    			CPPActionManager.MoveTo( v.node , time , pos.x , pos.y )
		    			CPPActionManager.ScaleTo( v.node , time , 0 )
		    			CPPActionManager.FadeOut( v.node , time , GlobalCallbackMgr:AddCallBackFunc(moveAnimate_) )	
		    		else
		    			-- 移动
		    			CPPActionManager.MoveBy( v.node , time , 0 , 50 )
		    			-- 消失
		    			CPPActionManager.FadeOut( v.node , time , GlobalCallbackMgr:AddCallBackFunc(moveAnimate_) )
	    			end
	    		end
	    	end
    	-- end
    end

	for i=1,#layer do
    	CPPActionManager.ScaleTo(layer[i],0.3,1.1,GlobalCallbackMgr:AddCallBackFunc(function ()
			CPPActionManager.ScaleTo(layer[i],0.2,1,GlobalCallbackMgr:AddCallBackFunc(function ()
				CPPActionManager.DelayTo(layer[i],1,GlobalCallbackMgr:AddCallBackFunc(function ()
					moveAnimate(i)
				end))
			end))
    	end))
    end
end
