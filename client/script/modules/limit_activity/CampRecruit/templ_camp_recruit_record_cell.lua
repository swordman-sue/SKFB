Modules = Modules or {}

Modules.TemplCampRecruitRecordCell = Modules.TemplCampRecruitRecordCell or BaseClass(GUI.ScrollItem)

function Modules.TemplCampRecruitRecordCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_CAMPRECRUIT_RECORD_CELL)
end

function Modules.TemplCampRecruitRecordCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	local type,config_hero = ObjIDTool.GetInfo(self.__data.hero_id)
	local quality 
	if type == Macros.Game.ObjType.HERO then
		quality = config_hero.init_quality
	else
		quality = config_hero.quality
	end
	--信息 
	local rt_txt = CPPGameLib.GetString("camp_recruit_record_cell_rt2",config_hero.name,self.__data.hero_num)
	self.__rich_txt = Util:RichText(CPPGameLib.GetString("camp_recruit_record_cell_rt",self.__data.role_name,string.format(Macros.Quality_Stroke_RT[quality],rt_txt)), Macros.TypefaceSize.slightly, 310, 45, Macros.Color.content_hex)
	self.__rich_txt:SetAnchorPoint(0,1)
	self:GetNode():AddChild(self.__rich_txt)
	self.__rich_txt:SetPosition(34,47)
end