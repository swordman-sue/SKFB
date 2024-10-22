
--
-- @brief 成员战绩
-- @author: yjg
-- @date: 2017年5月31日15:29:31
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionTranscriptRecord = LogicGUI.TemplUnionTranscriptRecord or BaseClass(GUI.Template)

function LogicGUI.TemplUnionTranscriptRecord:__init( )
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__render_queue = Scene.RenderQueue.GRQ_UI	
	self.__is_auto_load = true
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	    self:__Server()
	end

    -- self:BindGlobalEvent(Macros.Event.UnionModule.UNION_AWARD_GET , function (_, data)
    -- 	self:__SetShowUI()
    -- end)

	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_UNION_TRANSCRIPT_RECORD_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_UNION_TRANSCRIPT_RECORD)	
end


function LogicGUI.TemplUnionTranscriptRecord:__Server()
    CallModuleFunc(ModuleType.UNION , "GuildFbDamageRecordReq")

    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_HARM_RECORD , function (_, data)
        self:__SetShowUI( data )
    end)
end

function LogicGUI.TemplUnionTranscriptRecord:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--光
	self.Image_light = self:GetWidget("Image_light")
	--关闭
	self.Button_close =  Util:WidgetButton( 
											self:GetWidget("Button_close") , 
											" " , 
											CPPGameLib.Handler(self,self.OnButtonClose) 
										)
	--内容
	self.Image_content = self:GetWidget("Image_content")
	--伤害
	self.Text_harm = Util:WidgetLabel( 
										self:GetWidget("Text_harm") , 
										CPPGameLib.GetString("union_maxHarm") ,
										Macros.TypefaceSize.normal,
										Macros.Color.keypoint
										)
	--排行
	self.Text_rank = Util:WidgetLabel( 
										self:GetWidget("Text_rank") , 
										CPPGameLib.GetString("union_rankHarm") ,
										Macros.TypefaceSize.normal,
										Macros.Color.keypoint
										 )
	--次数
	self.Text_time = Util:WidgetLabel( 
										self:GetWidget("Text_time") , 
										CPPGameLib.GetString("union_attackTime") ,
										Macros.TypefaceSize.normal,
										Macros.Color.keypoint
										)
end

function LogicGUI.TemplUnionTranscriptRecord:__ShowUI( date )
	--成员战绩
	local union_record = Util:Label( CPPGameLib.GetString("union_record") , Macros.TypefaceSize.largish , Macros.Color.lilyWhite )
	union_record:SetAnchorPoint(0.5,0.5)
	union_record:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(union_record)

	--进度
	local params = 
	{
	    item_class = Modules.TemplUnionTranscriptRecordItem,
	    item_width = 594.00,
	    item_height = 134.00,
	    row = 4,
	    col = 1,
	    -- item_space_r = 5,
	    view_height = self.Image_content:GetContentHeight() - 5 ,
	}
	self.__recommend_scrollview = GUI.ScrollView.New(params)
	self.Image_content:AddChild(self.__recommend_scrollview:GetNode())
	PosTool.Center(self.__recommend_scrollview)

	--最高伤害排名
	self.union_rankHarm = Util:Label( "0" , Macros.TypefaceSize.normal , Macros.Color.content )
	self.union_rankHarm:SetAnchorPoint(0 , 0.5)
	self.Image_bg:AddChild(self.union_rankHarm)	
	PosTool.RightTo( self.Text_rank , self.union_rankHarm )

	--最高伤害排名
	self.union_attackTime = Util:Label( "0" , Macros.TypefaceSize.normal , Macros.Color.content )
	self.union_attackTime:SetAnchorPoint(0 , 0.5)
	self.Image_bg:AddChild(self.union_attackTime)	
	PosTool.RightTo( self.Text_harm , self.union_attackTime )

	--最高伤害排名
	self.union_maxHarm = Util:Label( "0" , Macros.TypefaceSize.normal , Macros.Color.content )
	self.union_maxHarm:SetAnchorPoint(0 , 0.5)
	self.Image_bg:AddChild(self.union_maxHarm)	
	PosTool.RightTo( self.Text_time , self.union_maxHarm )

end

function LogicGUI.TemplUnionTranscriptRecord:__SetShowUI( date )
	self.__recommend_scrollview:SetDataList( date )

	local info = {}
	for i,v in ipairs(date) do
		local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
		if v.role_name == role_info.role_name then
			info = v
			break
		end
	end

	--最高伤害排名
	self.union_rankHarm:SetString( info.rank or CPPGameLib.GetString("rank_reflection") )
	--战斗次数
	self.union_attackTime:SetString( info.damage or "0")
	--最高伤害排名
	self.union_maxHarm:SetString( info.battle_times or CPPGameLib.GetString("rank_reflection") )
end

function LogicGUI.TemplUnionTranscriptRecord:OnButtonClose( date )
	self:DeleteMe()
end



Modules = Modules or {}

Modules.TemplUnionTranscriptRecordItem = Modules.TemplUnionTranscriptRecordItem or BaseClass(GUI.ScrollItem)

function Modules.TemplUnionTranscriptRecordItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_UNION_TRANSCRIPT_RECORD_ITEM)	
	self:__Getwidget()
end

function Modules.TemplUnionTranscriptRecordItem:__delete()

end

function Modules.TemplUnionTranscriptRecordItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)	
	self:__ShowUI(data)
end

function Modules.TemplUnionTranscriptRecordItem:__Getwidget()
	--条目
	self.Image_bg = self:GetWidget("Image_bg")
	--名字
	self.Text_name = self:GetWidget("Text_name")
	--图标
	self.Panel_rank = self:GetWidget("Panel_rank")
	--头像
	self.Panel_icon = self:GetWidget("Panel_icon")
	--内容
	self.Image_2 = self:GetWidget("Image_2")
end

function Modules.TemplUnionTranscriptRecordItem:__ShowUI(data)
	--名字
	local name = Util:Label( data.role_name , Macros.TypefaceSize.largish , Macros.Color.keypoint )
	name:SetAnchorPoint( 0 , 0.5 )
	name:SetPosition( 0 , self.Text_name:GetContentHeight()/2 )
	self.Text_name:AddChild( name )

	--排行
	local rank = nil
	if data.rank <= 3 then
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_rank_num_"..data.rank)
		rank = Util:Sprite( sp )
	else
		rank = Util:Label( data.rank , Macros.TypefaceSize.popup , Macros.Color.headline )
	end
	self.Panel_rank:AddChild(rank)
	PosTool.Center(rank)

	--图标
	local img_head = LogicGUI.RoleIcon.New()
	-- img_head:SetTouchCallback(function()
	-- 	-- CallModuleFunc(ModuleType.CHECKROLEINFO, "OpenView", BaseViewType.CHECKROLEINFO,data)
	-- 	LogicGUI.TemplCheckRoleInfo.New( data )
 --        end)
 	img_head:SetData(data.head_icon)
	self.Panel_icon:AddChild(img_head:GetNode())
	PosTool.Center(img_head)

	--挑战次数
	local union_attackTime = Util:Label( CPPGameLib.GetString("union_attackTime") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	union_attackTime:SetAnchorPoint( 0 , 0.5 )
	union_attackTime:SetPosition( 5 , (self.Image_2:GetContentHeight()/2) + union_attackTime:GetContentHeight() - 5 )
	self.Image_2:AddChild( union_attackTime )
	--次数
	local battle_times = Util:Label( data.battle_times , Macros.TypefaceSize.normal , Macros.Color.content )
	battle_times:SetAnchorPoint( 0 , 0.5 )
	PosTool.RightTo( union_attackTime , battle_times )
	self.Image_2:AddChild( battle_times )

	--最高伤害
	local union_maxHarm = Util:Label( CPPGameLib.GetString("union_maxHarm") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	union_maxHarm:SetAnchorPoint( 0 , 0.5 )
	union_maxHarm:SetPosition( 5 , (self.Image_2:GetContentHeight()/2) - union_maxHarm:GetContentHeight() + 5 )
	self.Image_2:AddChild( union_maxHarm )
	--伤害
	local damage = Util:Label( data.damage , Macros.TypefaceSize.normal , Macros.Color.content )
	damage:SetAnchorPoint( 0 , 0.5 )
	PosTool.RightTo( union_maxHarm , damage )
	self.Image_2:AddChild( damage )
end