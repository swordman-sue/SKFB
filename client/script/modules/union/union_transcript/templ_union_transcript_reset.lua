
--
-- @brief 副本重置
-- @author: yjg
-- @date: 2017年6月1日15:39:50
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionTranscriptReset = LogicGUI.TemplUnionTranscriptReset or BaseClass(GUI.Template)

function LogicGUI.TemplUnionTranscriptReset:__init( )
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__render_queue = Scene.RenderQueue.GRQ_UI	
	self.__is_auto_load = true
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	    self:__Server()
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_UNION_TRANSCRIPT_RESET)	
end

function LogicGUI.TemplUnionTranscriptReset:__Server()
end

function LogicGUI.TemplUnionTranscriptReset:__Getwidget()
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
	--提示
	self.Text_tips = Util:WidgetLabel( 
										self:GetWidget("Text_tips") , 
										CPPGameLib.GetString("union_resetTips") ,
										Macros.TypefaceSize.normal,
										Macros.Color.keypoint
										)
	--最高提示
	self.Text_maxTips = self:GetWidget("Text_maxTips")

	--确定
	self.Button_confirm =  Util:WidgetButton( 
											self:GetWidget("Button_confirm") , 
											CPPGameLib.GetString("hero_awaken_buttonText4") , 
											CPPGameLib.Handler(self,self.OnButtonConfirm) ,
											nil,nil,Macros.Color.button_yellow
										)

	self.Image_item = {}
	self.Image_content = {}
	self.Text_name = {}
	self.Text_censorship = {}
	self.CheckBox = {}
	for i = 1 , 2 do
		--条目
		self.Image_item[i] = self:GetWidget("Image_item"..i)
		--内容
		self.Image_content[i] = self:GetWidget("Image_content"..i)
		--名字
		self.Text_name[i] = Util:WidgetLabel( 
							self:GetWidget("Text_name"..i) ,
							"" , 
							Macros.TypefaceSize.normal, 
							Macros.Color.keypoint
						)

		--关卡名
		self.Text_censorship[i] = Util:WidgetLabel( 
							self:GetWidget("Text_censorship"..i) ,
							"" , 
							Macros.TypefaceSize.normal, 
							Macros.Color.red
						)

		--选中
		self.CheckBox[i] = self:GetWidget("CheckBox"..i)
        self.CheckBox[i]:SetEventListener(
            GlobalCallbackMgr:AddCallBackFunc(function()
                self:Onbox(i)
            end)
        )  
        self.CheckBox[i]:SetSelectedState( false )
	end
end

function LogicGUI.TemplUnionTranscriptReset:__ShowUI( date )
	--成员战绩
	local union_record = Util:Label( CPPGameLib.GetString("union_reset") , Macros.TypefaceSize.largish , Macros.Color.lilyWhite )
	union_record:SetAnchorPoint(0.5,0.5)
	union_record:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(union_record)

	local text = {
			[1] = CPPGameLib.GetString("union_normal"),
			[2] = CPPGameLib.GetString("union_sendBack")
	}

	local tips = {
			[1] = CPPGameLib.GetString("union_resetTyps1") ,
			[2] = CPPGameLib.GetString("union_resetTyps2") ,
	}

	local item = {}
	for i , v in ipairs( CallModuleFunc(ModuleType.UNION , "GetResetLst") ) do
		--章节名
		self.Text_censorship[i]:SetText( CPPGameLib.GetString("union_transcriptSectionReset" , v.section ))
		--名字
		self.Text_name[i]:SetText(text[i])

		local tips = Util:RichText( string.format(Macros.Color.content_rt , tips[i] ) , Macros.TypefaceSize.normal , self.Image_content[i]:GetContentWidth() - 10  )
		tips:SetAnchorPoint( 0 , 1 )
		tips:SetPosition( 5 , self.Image_content[i]:GetContentHeight() - 10 )
		self.Image_content[i]:AddChild( tips )
	end

	local tab = {
					{ zi = CPPGameLib.GetString("union_historyMaxPlan") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
					{ zi = CallModuleFunc(ModuleType.UNION , "GetResetLst")[1].name , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },
				}
	local test = Util:LabCenter( tab )
	test:SetAnchorPoint( 0.5 , 0.5 )
	self.Text_maxTips:AddChild( test )
	PosTool.Center( test )
	
	local type_ = CallModuleFunc(ModuleType.UNION , "GetUnionTranscriptResettingType")

	CPPGameLib.PrintTable("xy" , type_ ,"GetUnionTranscriptResettingType")  



	if type_ == 0 then
		self:Onbox(2)
	else
		self:Onbox(1)
	end
end

function LogicGUI.TemplUnionTranscriptReset:Onbox( index )
	self.index = index
	for i = 1 , 2 do
		self.CheckBox[i]:SetSelectedState( false )
	end
	self.CheckBox[index]:SetSelectedState(true)
end

function LogicGUI.TemplUnionTranscriptReset:OnButtonClose( date )
	self:DeleteMe()
end

function LogicGUI.TemplUnionTranscriptReset:OnButtonConfirm( date )

	local privilege = CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.RESET )
	if privilege ~= 1 then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_noRights_") )
		return
	end

	local info = {}
	info.is_history_record = 0
	if self.index == 1 then
		info.is_history_record = 1
	end

	-- --如果是退回
	-- if info.is_history_record == 1 then
	-- 	local lst = CallModuleFunc(ModuleType.UNION , "GetResetLst")
	-- 	if lst[1].section == lst[2].section then
	-- 		info.is_history_record = 0
	-- 	end
	-- end

	CallModuleFunc(ModuleType.UNION , "GuildFbChapterResetReq",info)
end
