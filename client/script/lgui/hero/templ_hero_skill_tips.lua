
--[[
%% @module：英雄技能Tips
%% @author: swordman sue
%% @created: 2016-12-5
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroSkillTips = LogicGUI.TemplHeroSkillTips or BaseClass(UILayer)

function LogicGUI.TemplHeroSkillTips:__init()
	self:SetContentSize(402, 50)
	self:SetBackGroundImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_pack_tips_2"))
	self:SetBackGroundImageScale9Enabled(true)

	self.__sp_arrow = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "clicktips_arrow"))
	self.__sp_arrow:SetAnchorPoint(1, 0.5)
	self.__sp_arrow:SetFlipX(true)
	self:AddChild(self.__sp_arrow)
end

function LogicGUI.TemplHeroSkillTips:__delete()
end

function LogicGUI.TemplHeroSkillTips:SetData(hero_id, skill_id, skill_lev)
	local tips_info = ConfigAdapter.Skill.GetSkillTips(hero_id, skill_id, skill_lev)

	--技能描述
	if not self.__rt_des then
		self.__rt_des = Util:RichText(nil, nil, 370)
		self.__rt_des:SetAnchorPoint(0.5, 1)
		self:AddChild(self.__rt_des)
	end
	self.__rt_des:SetText(tips_info.des)

	--重置背景尺寸
	self:SetHeight(41 + self.__rt_des:GetContentHeight() + (tips_info.joint_des and 41 or 10))
	PosTool.RightCenter(self.__sp_arrow, 10)	

	--技能描述
	PosTool.CenterTop(self.__rt_des, 0, -41)

	--技能名
	if not self.__lab_name then
		self.__lab_name = Util:Label(nil, nil, Macros.Color.golden)
		self.__lab_name:SetAnchorPoint(0, 1)
		self:AddChild(self.__lab_name)
	end
	self.__lab_name:SetString(tips_info.name)
	PosTool.LeftTop(self.__lab_name, 12, -10)

	--技能CD
	if tips_info.cd then
		if not self.__lab_cd then
			self.__lab_cd = Util:Label(nil, nil, Macros.Color.btn)
			self.__lab_cd:SetAnchorPoint(1, 1)
			self:AddChild(self.__lab_cd)
		end
		self.__lab_cd:SetString(tips_info.cd)	
		PosTool.RightTop(self.__lab_cd, -20, -10)

	elseif self.__lab_cd then
		self.__lab_cd:SetVisible(false)
	end

	--合击技能
	if tips_info.joint_des then
		if not self.__rt_joint_des then
			self.__rt_joint_des = Util:RichText()
			self.__rt_joint_des:SetAnchorPoint(1, 0)
			self:AddChild(self.__rt_joint_des)		
		end
		self.__rt_joint_des:SetText(tips_info.joint_des)
		PosTool.RightBottom(self.__rt_joint_des, -22, 10)
	end
end
