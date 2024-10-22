Modules = Modules or {}

Modules.TemplDaliyTaskCell = Modules.TemplDaliyTaskCell or BaseClass(GUI.ScrollItem)

function Modules.TemplDaliyTaskCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.DALIYTASK_AND_ACHIEVEMENT_CELL)
end

function Modules.TemplDaliyTaskCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	self.mission_id = data.mission_id
	self.__data.CouldGetReward = false
	local missionInfo = CPPGameLib.GetConfig("DailyMission",data.mission_id)
	--名字
	Util:WidgetLabel(self:GetWidget("Text_title"),missionInfo.desc,Macros.TypefaceSize.largish, Macros.Color.headline  )
	--进度
	self.Text_progress = self:GetWidget("Text_progress")
	Util:WidgetLabel(self.Text_progress,CPPGameLib.GetString("progress_Text"),Macros.TypefaceSize.normal, Macros.Color.keypoint )
	--进度数值
	self.Text_progressNum = self:GetWidget("Text_progressNum")
	Util:WidgetLabel(self.Text_progressNum,data.value.."/"..missionInfo.value,Macros.TypefaceSize.normal, Macros.Color.content )
	self.Text_progress:SetPosition(self.Text_progressNum:GetPositionX() - self.Text_progressNum:GetContentWidth(),self.Text_progress:GetPositionY())
	
	--奖励的文字 
	self:ShowRichText()

	--任务头像		
	self:GetWidget("Image_icon"):SetImage(Resource.PathTool.GetSystemEntryIconPath(missionInfo.icon_id),TextureResType.LOCAL)	
	self:GetWidget("Image_icon"):SetScaleFactor(1.2)
	
	local function checkTreasureNum()
		local is_treasurebag_no_full,is_herobag_no_full = true,true
		for i,award_item_id in ipairs(missionInfo.award_item_id) do
			local type,itemInfo = ObjIDTool.GetInfo(award_item_id)
			if type == Macros.Game.ObjType.TREASURE then
				CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.TREASURE, missionInfo.award_item_num[i],false,function ( sum )
		        	is_treasurebag_no_full = sum >= missionInfo.award_item_num[i]
		        	if not is_treasurebag_no_full then
		        		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("daliyMission_treasuretips"))
		        	end
		    	end)
		    	if not is_treasurebag_no_full then
		    		break
		    	end
		    elseif type == Macros.Game.ObjType.HERO then
		    	CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.HERO, missionInfo.award_item_num[i],false,function ( sum )
		        	is_herobag_no_full = sum >= self.__achievementInfo.award_item_num
		        	if not is_herobag_no_full then
		        		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("daliyMission_herotips"))
		        	end
		    	end)
		    	if not is_herobag_no_full then
		    		break
		    	end
			end
		end		
    	return is_treasurebag_no_full,is_herobag_no_full
	end
	--TODO
	local function getEncourage()
		local is_treasurebag_no_full,is_herobag_no_full = checkTreasureNum()
		local is_no_full = is_herobag_no_full and is_treasurebag_no_full or false
		--如果背包满了
	 	if not is_no_full then
	 		return
	 	end
		CallModuleFunc(ModuleType.DALIYTASK_AND_ACHIEVEMENT,"DaliyMissionAwardReq",self.mission_id,self.__base_exp)
		self.__button_moretype:SetVisible(false)
		self.__data.CouldGetReward = false
		self.__data.is_get_reward = true
		if self.__button_zoubian_effect then
			self:__DelEffect(self.__button_zoubian_effect)
			self.__button_zoubian_effect = nil
		end
		self.haveDoneImage:SetVisible(true)
		GlobalEventSystem:FireNextFrame(Macros.Event.DaliyMissionAndAchievementModule.REFRASH_DALIYMISSION_LIST,self.__index,self.mission_id)	
	end

	local function moveTo()	
		GlobalEventSystem:FireNextFrame(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW)
		GlobalModulesMgr:OpenSystemModule(missionInfo.systemid)
	end
	self.__button_moretype = self:GetWidget("Button_moreType")	
	self.haveDoneImage = self:GetWidget("Image_haveDone")
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DALIY_AND_ACHIEVEMENT, "have_get",true)
	self.haveDoneImage:SetImage(sp)
	if data.is_get_reward then
		self.__button_moretype:SetVisible(false)
		self.Text_progress:SetVisible(false)
		self.Text_progressNum:SetVisible(false)
		self.haveDoneImage:SetVisible(true)
	elseif missionInfo.value == data.value then
		self.__data.CouldGetReward = true
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_blue")
		self.__button_moretype:SetNormalImage(sp)		
		Util:WidgetButton(self.__button_moretype,CPPGameLib.GetString("get_Award_Text"),getEncourage,Macros.TypefaceSize.largish, nil , Macros.Color.button_red )
		self.__button_moretype:SetTouchSoundInfo(Macros.Game.SystemID.DALIYTASK_AND_ACHIEVEMENT,1)
		local effect
		self.__button_zoubian_effect,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.__button_moretype)
		PosTool.Center(effect)
		self.Text_progress:SetVisible(false)
		self.Text_progressNum:SetVisible(false)
		self.__button_moretype:SetVisible(true)
		self.haveDoneImage:SetVisible(false)
	else
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
		self.__button_moretype:SetNormalImage(sp)
		Util:WidgetButton(self.__button_moretype,CPPGameLib.GetString("move_To_Text"),moveTo,Macros.TypefaceSize.largish, nil , Macros.Color.button_yellow )
		self.__button_moretype:SetVisible(true)
		self.haveDoneImage:SetVisible(false)
	end
end

function Modules.TemplDaliyTaskCell:__delete()
	if self.__button_zoubian_effect then
		self:__DelEffect(self.__button_zoubian_effect)
		self.__button_zoubian_effect = nil
	end
end

function Modules.TemplDaliyTaskCell:ShowRichText()
	local missionInfo = CPPGameLib.GetConfig("DailyMission",self.__data.mission_id)
	--奖励的文字 
	local encourage_Text = CPPGameLib.GetString("encourage_Richtext")

	--奖励列表的文字
	local encourage_String = ""
	for i,award_item_id in ipairs(missionInfo.award_item_id) do
		local type,itemInfo = ObjIDTool.GetInfo(award_item_id)
		encourage_String = encourage_String..itemInfo.name.."*"..missionInfo.award_item_num[i].."  "
	end
	encourage_String = encourage_String..CPPGameLib.GetString("DaliyMissionAwardPoint").." *"..missionInfo.point
	--经验
	if missionInfo.base_exp then
		local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
		self.__base_exp = missionInfo.base_exp*role_level
		encourage_String = encourage_String.."<BR>    "..CPPGameLib.GetString("daliyMission_exp",self.__base_exp)
	end
	local award_Text = CPPGameLib.GetString("encourage_Award_Richtext",encourage_String)
	if not self.__rich_text then
		self.__rich_text = Util:RichText(encourage_Text..award_Text,Macros.TypefaceSize.normal,350,0)
		self.__rich_text:SetAnchorPoint(0,1)
		self:GetNode():AddChild(self.__rich_text)
		self.__rich_text:SetPosition(135,76)
	else
		local lab = GUI.RichText.GetColorWord(encourage_Text..award_Text, Macros.Color.white_hex)
		self.__rich_text:SetText(lab)
	end
end