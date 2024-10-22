--[[
%% @module: 主界面背景
--]]
Modules.MainCityBG = Modules.MainCityBG or BaseClass(UILayer)

--[[
@ params：
    必须：
    image_size			[cocosize]背景图原尺寸
    image_anchor 		[cocopoint]背景图锚点
    touch_func			[function]点击回调
--]]
function Modules.MainCityBG:__init(params, parent_view)
	self.parent_view = parent_view

	self.image_size = params.image_size or Macros.Global.DesignSize
	self.image_anchor = params.image_anchor or COCOPoint(0.5, 0)
	self.layer_anchor = params.layer_anchor or COCOPoint(0.5, 0.5)
	self:SetAnchorPoint(self.layer_anchor.x, self.layer_anchor.y)
	self:SetContentSize(self.image_size.width, self.image_size.height)

	if params.touch_func then
		self:SetTouchEnabled(true)
		self:SetTouchFun(nil, nil, params.touch_func)
	end
end

--设置背景
function Modules.MainCityBG:SetBG(bg_res_list)
	if not bg_res_list then
		return
	end

	self.__bg_cache_list = self.__bg_cache_list or {}

	for _,del_bg in ipairs(self.__bg_cache_list) do
		del_bg:RemoveSelf(true)
	end
	self.__bg_cache_list = {}

	local bg_res_list = bg_res_list or {}

	local tmp_bg, pos_x, pos_y
	for _,bg_info in ipairs(bg_res_list) do
		tmp_bg = Util:Sprite( Resource.PathTool.GetUIResPath("bigbg/" .. bg_info.city_bg, nil, bg_info.suffix))
		tmp_bg:SetAnchorPoint(self.image_anchor.x,self.image_anchor.y)
		self:AddChild(tmp_bg,bg_info.zorder)
		pos_x = bg_info.pos and bg_info.pos[1] or 0
		pos_y = bg_info.pos and bg_info.pos[2] or 0
		PosTool.CenterBottom(tmp_bg, pos_x, pos_y)
		table.insert(self.__bg_cache_list,tmp_bg)
	end
	local scale_x = Macros.Global.DesignSize.width / self.image_size.width
	local scale_y = Macros.Global.DesignSize.height / self.image_size.height
	self:SetScaleFactor(math.max(scale_x, scale_y))		

	--多分辨率适配
	GUI.MultiResolutionAdapter.AdaptForBG(
		self, self.parent_view:GetNode(), self.parent_view:IsForceStretch(), self.parent_view:IsIgnoreIPhoneSafeArea())	
end

--设置特效
function Modules.MainCityBG:SetEffects(eff_res_list)
	if not eff_res_list then
		return
	end

	self.__effects_cache_list = self.__effects_cache_list or {}

	for _,del_eff in ipairs(self.__effects_cache_list) do
		self:__DelEffect(del_eff)
	end
	self.__effects_cache_list = {}

	local eff_res_list = eff_res_list or {}

	local tmp_effect, tmp_effid, pos_x, pos_y
	for _, eff_info in ipairs(eff_res_list) do
		tmp_effid, tmp_effect = self:__CreateEffect({id = eff_info.effects}, self:GetNode(), eff_info.zorder)
		tmp_effect:SetAnchorPoint(0.5,0.5)
		pos_x = eff_info.pos and eff_info.pos[1] or 0
		pos_y = eff_info.pos and eff_info.pos[2] or 0
		PosTool.Center(tmp_effect, pos_x, pos_y)
		table.insert(self.__effects_cache_list,tmp_effid)
	end
end

--设置骨骼
function Modules.MainCityBG:SetAction(act_res_list)
	if not act_res_list then
		return
	end

	self.__action_cache_list = self.__action_cache_list or {}

	for _,del_act in ipairs(self.__action_cache_list) do
		self:__DelEffect(del_act)
	end
	self.__action_cache_list = {}

	local act_res_list = act_res_list or {}

	local tmp_act, tmp_actid, pos_x, pos_y
	for i , act_info in ipairs(act_res_list) do
		tmp_actid, tmp_act = self:__CreateEffect({res_name = act_info.action , type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE}, self:GetNode(), act_info.zorder)
		tmp_act:SetAnchorPoint(0.5, 0.5)
		pos_x = act_info.pos and act_info.pos[1] or 0
		pos_y = act_info.pos and act_info.pos[2] or 0
		PosTool.Center(tmp_act, pos_x, pos_y)
		table.insert(self.__action_cache_list, tmp_actid)
	end
end

function Modules.MainCityBG:IsFixedSize()
	return self.is_fixed_size
end


Modules.MainCityLayerBG = Modules.MainCityLayerBG or BaseClass(Modules.MainCityBG)

--[[
@ params：
	必须：
    image_path_list		[string]贴图路径
    image_size			[cocosize]背景图原尺寸
    image_anchor 		[cocopoint]背景图锚点
    func				[function]点击回调
--]]
function Modules.MainCityLayerBG:__init(params,parent_view)
	self.parent_view = parent_view
	self.image_size = params.image_size or Macros.Global.BGDefaultSize
	self.image_anchor = params.image_anchor or COCOPoint(0, 0)
	self:SetContentSize(self.image_size.width, self.image_size.height)
	if params.touch_func then
		self:SetTouchEnabled(true)
		self:SetTouchFun(params.bg_func, params.touch_func,params.ed_func)
	end

	--贴图路径
	self.image_path_list = params.image_path_list	
end

function Modules.MainCityLayerBG:SetBG()
	if self.image_path_list then
		for i,image_path in ipairs(self.image_path_list) do
			local image_node = Util:Sprite(Resource.PathTool.GetUIResPath("bigbg/"..image_path))
			image_node:SetAnchorPoint(self.image_anchor.x, self.image_anchor.y)
			self:AddChild(image_node,i*2)

			local scale_x = Macros.Global.DesignSize.width / self.image_size.width
			local scale_y = Macros.Global.DesignSize.height / self.image_size.height
			self:SetScaleFactor(math.max(scale_x, scale_y))	
		end
	end

	--多分辨率适配
	GUI.MultiResolutionAdapter.AdaptForBG(
		self, self.parent_view:GetNode(), self.parent_view:IsForceStretch(), self.parent_view:IsIgnoreIPhoneSafeArea())
end