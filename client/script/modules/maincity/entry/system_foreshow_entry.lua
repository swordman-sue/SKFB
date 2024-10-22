--[[
%% @module: 功能预告入口
%% @author: ldx
%% @created: 2018-12-21
--]]

Modules = Modules or {}

Modules.SystemForeshowEntry = Modules.SystemForeshowEntry or BaseClass(Modules.BaseMainCityEntry)

function Modules.SystemForeshowEntry:__init()
	self.__entry_info.module_txt = ""

	self.__entry_btn:IgnoreContentAdaptWithSize(false)

	--背景图标
	local sp =  Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY, "bg_system_foreshow") 
	local icon_bg = SpriteEx.CreateWithImageFile(sp)
	icon_bg:SetAnchorPoint(1, 0.5)
	icon_bg:SetScaleFactor(self.__entry_info.icon_size or 1)
	self:AddChild(icon_bg,-1)
	PosTool.Center(icon_bg, -45, 30)

	--功能名
	self.__system_foreshow_label = Util:RichText("", Macros.TypefaceSize.largish)
	self.__system_foreshow_label:SetAnchorPoint(0.5,0.5)
	self.__system_foreshow_label:SetScaleFactor(self.__entry_info.icon_size or 1)
	self:AddChild(self.__system_foreshow_label)
	PosTool.Center(self.__system_foreshow_label, -95, 43)		

	--开放等级
	self.__system_foreshow_level_label = Util:RichText("", Macros.TypefaceSize.largish)
	self.__system_foreshow_level_label:SetAnchorPoint(0.5,0.5)
	self.__system_foreshow_level_label:SetScaleFactor(self.__entry_info.icon_size or 1)
	self:AddChild(self.__system_foreshow_level_label)
	PosTool.Center(self.__system_foreshow_level_label, -95, 17)

	self:__UpdateSystemForeshow()
end

function Modules.SystemForeshowEntry:RegistAllEvents()
	Modules.BaseMainCityEntry.RegistAllEvents(self)

	--阵容推荐特殊处理
	self:BindGlobalEvent(Macros.Event.StoryModule.RECOMMEND_LINEUP, function(_,is_start)
		self:__HandleRecommendLineupStory(is_start)
	end)
end

--设置功能预告等级图标
function Modules.SystemForeshowEntry:__UpdateSystemForeshow()
	--排列
	local sort_list,is_no_foreshow = CallModuleFunc(ModuleType.RECOMMEND_LINEUP, "SortSystemForeshowList")
	self:SetVisible(not is_no_foreshow)
	if not is_no_foreshow then
		--功能名
		local str = string.format(Macros.Color.city_headline_rt, sort_list[1].system_name)
		self.__system_foreshow_label:SetText(GUI.RichText.GetColorWord(str))

		--开放等级
		str = CPPGameLib.GetString("common_open_lev",tostring(sort_list[1].open_level))
		self.__system_foreshow_level_label:SetText(GUI.RichText.GetColorWord(str, Macros.Color.golden_hex)) 

		--入口按钮
		self.__entry_btn:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath(sort_list[1].icon_id), TextureResType.LOCAL)
	end
end

--处理推荐阵容剧情
function Modules.SystemForeshowEntry:__HandleRecommendLineupStory(is_start)
	--剧情开始
	if is_start then
		self.__camp_img_list = {}
		CPPActionManager.DelayTo(self:GetNode(),0.5,GlobalCallbackMgr:AddCallBackFunc( function ( ... )
        	--创建阵营图标
			for i=0,2 do
				local scene = GlobalScene:GetRenderGroup(Scene.RenderQueue.GRQ_UI_TOP,true)
				local image_path = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "camp_icon_big_"..i,true)
				local camp_img = Util:Sprite(image_path)
				scene:AddChild(camp_img,1000)
				camp_img:SetScaleFactor(5)
				PosTool.Center(camp_img,-360 + 360*i,100)
				CPPActionManager.ScaleTo(camp_img,0.15,1)			
				table.insert(self.__camp_img_list,camp_img)
			end
    	end))		
	else
		local del_func = function ()
			for _,camp_img in ipairs(self.__camp_img_list) do
				camp_img:RemoveSelf(true)
			end
			self.__camp_img_list = {}
		end			
		--获取入口控件
		if self.__region_panel then
			local scene = GlobalScene:GetRenderGroup(Scene.RenderQueue.GRQ_UI_TOP)
			local pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__region_panel,scene:GetNode(),COCOPoint(self:GetPositionX()+40,self:GetPositionY()-60))
			--飞过去
			for index,camp_img in ipairs(self.__camp_img_list) do
				ActionManager.GetInstance():RunScaleToAndMoveToEffectEx(camp_img, 0.5,pos.x,pos.y,0,index == 2 and function ()
					del_func()
					--特效										
					local effectid,effect = self:__CreateEffect({id = 100028,loop = false,time_scale = 1,sacle = 0.2},self:GetNode(),100)
				    PosTool.Center(effect,0,-20)
				    --闪过
					CPPActionManager.ScaleTo(effect, 0.2, 0.4, GlobalCallbackMgr:AddCallBackFunc(function()
						CPPActionManager.ScaleTo(effect, 0.2, 0, GlobalCallbackMgr:AddCallBackFunc(function()							
						end))
					end))			    
				end)
			end			
		elseif self.__camp_img_list then
			del_func()
		end
	end
end

--等级变化相关处理
function Modules.SystemForeshowEntry:__HandleLevelChange()
	self:__UpdateSystemForeshow()
end

