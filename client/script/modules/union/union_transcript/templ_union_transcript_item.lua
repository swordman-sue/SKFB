--
-- @brief 公会副本条目
-- @author: yjg
-- @date: 2017年5月19日15:10:03
--

Modules = Modules or {}

--副本章节模板
Modules.TemplUnionTranscriptItem = Modules.TemplUnionTranscriptItem or BaseClass(GUI.ScrollItem)

function Modules.TemplUnionTranscriptItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_UNION_TRANSCRIPT_ITEM)	
	self:__Getwidget()
end

function Modules.TemplUnionTranscriptItem:__delete()
end  

function Modules.TemplUnionTranscriptItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self:__ShowUI()
end

function Modules.TemplUnionTranscriptItem:__Getwidget()
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")
	--特效
	self.Panel_combat = self:GetWidget("Panel_combat")
	self.Panel_combat:SetPosition( self.Panel_combat:GetPositionX()  , self.Panel_combat:GetPositionY() + 130 )
	--名字
	self.Panel_name = self:GetWidget("Panel_name")
	self.Panel_name:SetPosition( self.Panel_name:GetPositionX()  , self.Panel_name:GetPositionY() + 50 )
	--名字
	self.Panel_bar = self:GetWidget("Panel_bar")
	self.Panel_bar:SetPosition( self.Panel_bar:GetPositionX()  , self.Panel_bar:GetPositionY() + 50 )
	--红点
	self.Panel_Box = self:GetWidget("Panel_Box")
	self.Panel_Box:SetPosition( self.Panel_Box:GetPositionX()  , self.Panel_Box:GetPositionY() - 100 )
	self.Panel_Box:SetVisible(false)
end

function Modules.TemplUnionTranscriptItem:__ShowUI()
	-- if self.__chapter_skeleton_effectid then
	-- 	self:__DelEffect(self.__chapter_skeleton_effectid)
	-- 	self.__chapter_skeleton_effectid = nil
	-- end

	self.data = CPPGameLib.GetConfig("GuildFB", self.__data.id )

	-- self.__chapter_skeleton_effectid, effect = self:__CreateEffect({res_name = "UI_chapter_icon"..(self.data.chapter_bg_id or 1) , type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE}, self:GetWidget("Panel_Chapters"),-1)
	-- effect:SetPosition(107,188)
	-- self.__chapter_skeleton_root = GlobalEffectMgr:GetInnerEffect(self.__chapter_skeleton_effectid)


	local path = Resource.PathTool.GetUnionTranscriptChapterIconPath( self.__data.chapter_bg_id or 1 )
	self.Panel_icon:SetBackGroundImage( path,TextureResType.LOCAL )

	for i , v in ipairs( self.data.gamelevel_list ) do
		local level = CPPGameLib.GetConfig("GameLevel" , v )
        local monter = CPPGameLib.GetConfig( "Monster" , level.icon_id )

		local sc = nil
		local dir = nil
		local x = nil
		local y = nil
		local zorder = nil

		if i == 1 then
			zorder = 1
			sc = 0.8 * ( monter.show_scale or 1 )
			dir = MathTool.HORZ_LEFT
			x = ( self.Panel_icon:GetContentWidth()/2 ) - 70
			y = ( self.Panel_icon:GetContentHeight()/2 ) + 75 
		elseif i == 2 then
			zorder = 2
			sc = 0.9 * ( monter.show_scale or 1 )
			dir = MathTool.HORZ_LEFT
			x = ( self.Panel_icon:GetContentWidth()/2 )
			y = ( self.Panel_icon:GetContentHeight()/2 ) + 60 
		elseif i == 3 then
			zorder = 1
			sc = 0.8 * ( monter.show_scale or 1 )
			dir = MathTool.HORZ_RIGHT
			x = ( self.Panel_icon:GetContentWidth()/2 ) + 70
			y = ( self.Panel_icon:GetContentHeight()/2 ) + 75 
		end


        local bg_hero_shadow = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_shadow"))
        bg_hero_shadow:SetPosition( x  , y )
        self.Panel_icon:AddChild( bg_hero_shadow )

        --模型
        local monster = LogicGUI.Model.New({info_id = level.icon_id , model_type = LogicGUI.ModelType.MONSTER , parent = self.Panel_icon , dir = dir , zorder = zorder })
        monster:SetScaleFactor( sc )
        monster:SetPosition( x  , y )
	end

	--章节名
	local chapter_res = Resource.PathTool.GetUnionTranscriptNamePath(tostring(3000 + self.__data.id ))
	local chapter_name = Util:Sprite(chapter_res)
	chapter_name:SetAnchorPoint(0.5, 0.5)
	self.Panel_name:AddChild(chapter_name)
	PosTool.Center(chapter_name , 0 , 15 )

	--章节
	local chapter_num = GUI.ImgFont.New( Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.UNION, "chapter_num", true ) , GUI.ImgFont.ImgType.Num1)
	chapter_num:SetAnchorPoint(0.5,0.5)
	self.Panel_name:AddChild(chapter_num:GetNode())
	chapter_num:SetText(tostring( self.__data.id ))
	PosTool.CenterBottom(chapter_num , 0 , -30 )

	local text_di = Util:Sprite( Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.UNION, "text_di", true ) )
	self.Panel_name:AddChild( text_di )

	local text_chapter = Util:Sprite( Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.UNION, "text_chapter", true ) )
	self.Panel_name:AddChild( text_chapter )

	--调整文字位置
	local width = chapter_num:GetNode():GetContentWidth()
	PosTool.Center(text_di,-(width/2 + (text_di:GetContentWidth()/2 + 5) ) , chapter_num:GetPositionY() )
	PosTool.Center(text_chapter ,(width/2 + (text_chapter:GetContentWidth()/2 + 5) ) , chapter_num:GetPositionY() )

	self.Panel_bar:SetPosition( self.Panel_bar:GetPositionX() , self.Panel_bar:GetPositionY() - 30 )

	local section = CallModuleFunc(ModuleType.UNION , "GetNowAttackSection")
	if self.__data.id == section then

		local progressBar_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_main_bg")
		local progressBar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_preview")

		--血条
		local bar = GUI.ProgressBar9Layer.New( progressBar , progressBar_bg , self.Panel_bar:GetContentWidth() - 3 , self.Panel_bar:GetContentHeight() - 3 , self.Panel_bar:GetContentWidth() , self.Panel_bar:GetContentHeight() , 0 , 100) 
		bar:SetAnchorPoint(0.5,0.5)
		self.Panel_bar:AddChild( bar:GetNode() )
		PosTool.Center( bar )

		local hp , max = CallModuleFunc(ModuleType.UNION , "GetOutsideHP")
		bar:SetProgressMax(max)
		bar:SetProgress(hp)

	    --字
	    local barText = Util:Name( hp.."%" , Macros.TypefaceSize.slightly )
	    barText:SetAnchorPoint( 0.5 ,0.5 )
	    self.Panel_bar:AddChild(barText)
	    PosTool.Center( barText , 0 , -1 )
	
	    --可攻打
		local effect_id, effect = self:__CreateEffect({res_name = "UI_xiaodao",scale = 0.8} , self.Panel_combat)
		effect:SetAnchorPoint(0,0.5)
		PosTool.Center( effect )

	elseif self.__data.id < section then
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "passed" , true)
		local passed = Util:Sprite(sp)
		self.Panel_name:AddChild( passed , 999 )
		PosTool.Center( passed )

	elseif self.__data.id > section then

		local cnf = CPPGameLib.GetConfig("GuildFB", self.__data.id - 1 )
		local dungeon_open_after = Util:Name( CPPGameLib.GetString("dungeon_open_after" , cnf.name ) )
		dungeon_open_after:SetAnchorPoint( 0.5 , 0.5 )
		self.Panel_bar:AddChild( dungeon_open_after )
		PosTool.Center( dungeon_open_after )
	end

	self:SetRed( self.__data.id )
end

function Modules.TemplUnionTranscriptItem:IsTouchEnable()
	if not GUI.ScrollItem.IsTouchEnable(self) then
		return false
	end

	local section = CallModuleFunc(ModuleType.UNION , "GetNowAttackSection")
	if self.__data.id > section then
		return false
	else
		return true
	end
end

--获取
function Modules.TemplUnionTranscriptItem:GetImgNormal()
	return self.Panel_icon
end

--红点
function Modules.TemplUnionTranscriptItem:SetRed( id )

	local lst = CallModuleFunc(ModuleType.UNION, "GetMyMapReward" , id)
	if not lst then return end

	local section = CallModuleFunc(ModuleType.UNION, "GetNowAttackSection")	
	local getType_ = true
	for i , v in ipairs( lst ) do
		if v.is_take_reward == 0 then
			if section == id then
				local campHP = CallModuleFunc(ModuleType.UNION , "GetMonsterData" , i )
				if campHP.killer_name ~= "" then
					getType_ = false
				end
			else
				getType_ = false
			end
		end
	end

	if getType_ == false then
		self.Panel_Box:SetVisible(true)
		self.Panel_combat:SetVisible(false)
	else
		self.Panel_combat:SetVisible(true)
	end
end