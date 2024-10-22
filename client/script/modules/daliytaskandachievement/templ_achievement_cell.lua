Modules = Modules or {}

Modules.TemplAchievementCell = Modules.TemplAchievementCell or BaseClass(GUI.ScrollItem)

function Modules.TemplAchievementCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.DALIYTASK_AND_ACHIEVEMENT_CELL)
end

function Modules.TemplAchievementCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	self.__bigdata = CPPGameLib.GetConfig("Achievement",self.__data.achievement_id)
	self.__achievementInfo = self.__data.HaveGetThrough and self.__bigdata.level_list[self.__data.cur_reward_lv] or self.__bigdata.level_list[self.__data.cur_reward_lv+1]
	--名字
	self.__itemname = self:GetWidget("Text_title")	
	Util:WidgetLabel(self.__itemname,"",Macros.TypefaceSize.largish, Macros.Color.headline )
	--进度
	self.__text_progress = self:GetWidget("Text_progress")
	Util:WidgetLabel(self.__text_progress,CPPGameLib.GetString("progress_Text"),Macros.TypefaceSize.normal, Macros.Color.keypoint )
	--进度数值
	self.__text_progressnum = self:GetWidget("Text_progressNum")
	Util:WidgetLabel(self.__text_progressnum,"",Macros.TypefaceSize.normal, Macros.Color.content )
	--奖励的文字 
	self.__encourage_text = CPPGameLib.GetString("encourage_Richtext")
	--奖励列表的文字	
	self.__rich_text = Util:RichText("",Macros.TypefaceSize.normal,350,0)
	self.__rich_text:SetAnchorPoint(0,1)
	self:GetNode():AddChild(self.__rich_text)
	self.__rich_text:SetPosition(135,76)
	--任务头像	
	--self:GetWidget("Image_icon"):IgnoreContentAdaptWithSize(true)	
	self:GetWidget("Image_icon"):SetImage(Resource.PathTool.GetSystemEntryIconPath(self.__achievementInfo.icon_id),TextureResType.LOCAL)	
	self:GetWidget("Image_icon"):SetScaleFactor(1.2)

	local function checkTreasureNum()
		local is_treasurebag_no_full,is_herobag_no_full = true,true
		local type,itemInfo = ObjIDTool.GetInfo(self.__achievementInfo.award_item_id)
		if type == Macros.Game.ObjType.TREASURE then
			CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.TREASURE, self.__achievementInfo.award_item_num,false,function ( sum )
	        	is_treasurebag_no_full = sum >= self.__achievementInfo.award_item_num
	        	if not is_treasurebag_no_full then
	        		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("daliyMission_treasuretips"))
	        	end
	    	end)
	    elseif type == Macros.Game.ObjType.HERO then
	    	CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.HERO, self.__achievementInfo.award_item_num,false,function ( sum )
	        	is_herobag_no_full = sum >= self.__achievementInfo.award_item_num
	        	if not is_herobag_no_full then
	        		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("daliyMission_herotips"))
	        	end
	    	end)
		end	
    	return is_treasurebag_no_full,is_herobag_no_full
	end

	self:SetContent()
	local function getEncourage()
		local is_treasurebag_no_full,is_herobag_no_full = checkTreasureNum()
		local is_no_full = is_herobag_no_full and is_treasurebag_no_full or false
		--如果背包满了
	 	if not is_no_full then
	 		return
	 	end
		self.__button_moretype:SetEnabled(false)
		CallModuleFunc(ModuleType.DALIYTASK_AND_ACHIEVEMENT,"AchievementAwardReq",self.__data.achievement_id,self.__data.cur_reward_lv+1,self.__index)				
	end
	local function moveTo()
		GlobalEventSystem:FireNextFrame(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW)
		GlobalModulesMgr:OpenSystemModule(self.__achievementInfo.systemid)
	end
	self.__button_moretype = self:GetWidget("Button_moreType")
	self.haveDoneImage = self:GetWidget("Image_haveDone")
	if data.HaveGetThrough then
		self.__button_moretype:SetVisible(false)
		self.__text_progress:SetVisible(false)
		self.__text_progressnum:SetVisible(false)
	elseif data.CouldGetReward then
		self.haveDoneImage:SetVisible(false)
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_blue")
		self.__button_moretype:SetNormalImage(sp)		
		Util:WidgetButton(self.__button_moretype,CPPGameLib.GetString("get_Award_Text"),getEncourage,Macros.TypefaceSize.largish,nil , Macros.Color.button_red )
		self.__button_moretype:SetTouchSoundInfo(Macros.Game.SystemID.DALIYTASK_AND_ACHIEVEMENT,1)
		local effect
		self.__button_zoubian_effect,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.__button_moretype)
		PosTool.Center(effect)
		self.__text_progress:SetVisible(false)
		self.__text_progressnum:SetVisible(false)
	else
		self.haveDoneImage:SetVisible(false)
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
		self.__button_moretype:SetNormalImage(sp)
		Util:WidgetButton(self.__button_moretype,CPPGameLib.GetString("move_To_Text"),moveTo,Macros.TypefaceSize.largish,nil , Macros.Color.button_yellow )
		self.__text_progress:SetVisible(true)
		self.__text_progressnum:SetVisible(true)		
	end
end

function Modules.TemplAchievementCell:RefrashData()
	self.__button_moretype:SetEnabled(true)
	self.__data.cur_reward_lv = self.__data.cur_reward_lv + 1
	if self.__data.cur_reward_lv == self.__data.level and #self.__bigdata.level_list == self.__data.cur_reward_lv then --已满级
		self.__data.CouldGetReward = false
		self.__data.HaveGetThrough = true
		self.__achievementInfo = self.__bigdata.level_list[self.__data.cur_reward_lv]
		self:SetContent()
		self.haveDoneImage:SetVisible(true)
		self.__button_moretype:SetVisible(false)
		if self.__button_zoubian_effect then
			self:__DelEffect(self.__button_zoubian_effect)
			self.__button_zoubian_effect = nil
		end
		self.__text_progress:SetVisible(false)
		self.__text_progressnum:SetVisible(false)
		GlobalEventSystem:FireNextFrame(Macros.Event.DaliyMissionAndAchievementModule.REFRASH_ACHIEVEMENT_LIST,self.__index,true)
	elseif self.__data.cur_reward_lv < self.__data.level then
		self.__achievementInfo = self.__bigdata.level_list[self.__data.cur_reward_lv+1]
		self:SetContent()
	elseif self.__data.cur_reward_lv == self.__data.level then
		self.__data.CouldGetReward = false
		self.__achievementInfo = self.__bigdata.level_list[self.__data.cur_reward_lv+1]
		self:SetContent()
		self.haveDoneImage:SetVisible(false)
		if self.__button_zoubian_effect then
			self:__DelEffect(self.__button_zoubian_effect)
			self.__button_zoubian_effect = nil
		end
		self.__button_moretype:SetVisible(true)
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
		self.__button_moretype:SetNormalImage(sp)
		Util:WidgetButton(self.__button_moretype,CPPGameLib.GetString("move_To_Text"),moveTo,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)
		self.__text_progress:SetVisible(true)
		self.__text_progressnum:SetVisible(true)
		GlobalEventSystem:FireNextFrame(Macros.Event.DaliyMissionAndAchievementModule.REFRASH_ACHIEVEMENT_LIST,self.__index)
	else

	end
end

function Modules.TemplAchievementCell:SetContent()
	self.__itemname:SetText(self.__achievementInfo.desc)	
	local encourage_String = ""
	local type,itemInfo = ObjIDTool.GetInfo(self.__achievementInfo.award_item_id)
	encourage_String = encourage_String..itemInfo.name.."*"..self.__achievementInfo.award_item_num
	local award_Text = CPPGameLib.GetString("encourage_Award_Richtext",encourage_String)
	self.__rich_text:SetText(self.__encourage_text..award_Text) 
	self.__text_progressnum:SetText( Util:NumTransform( self.__data.value ).."/"..Util:NumTransform( self.__achievementInfo.value ) )
	self.__text_progress:SetPosition(self.__text_progressnum:GetPositionX() - self.__text_progressnum:GetContentWidth(),self.__text_progress:GetPositionY())
end

--新手指引特殊操作检测
function Modules.TemplAchievementCell:CheckSectionOperateSP()
	--日常成就前往（成就ID）
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.ACHIEVEMENT_GOTO, self.__button_moretype, self.__data.achievement_id)                	

	--日常成就领奖（成就ID）
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.ACHIEVEMENT_GET_REWARD, self.__button_moretype, self.__data.achievement_id)                				
end

function Modules.TemplAchievementCell:__delete()
	if self.__button_zoubian_effect then
		self:__DelEffect(self.__button_zoubian_effect)
		self.__button_zoubian_effect = nil
	end
end