
--
-- @brief 叛军发现界面
-- @author: yjg
-- @date: 2016年9月24日18:28:48
--
LogicGUI.RebelLayer = LogicGUI.RebelLayer or BaseClass(GUI.Template)

function LogicGUI.RebelLayer:__init(callback)
	self.__callback = callback

	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__is_auto_load = true
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.REBEL)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.REBEL,true)
	self:InitTemplate(GUI.TemplateRegistry.REBEL_RANK_FIND)	
end

function LogicGUI.RebelLayer:__Getwidget()
	--居中层
	self.Panel_1 = self:GetWidget("Panel_1")
	--背景
	self.Image_1 = self:GetWidget("Image_1")
	--右背景
	-- self.Image_right = self:GetWidget("Image_right")
	--模型层
	self.Panel_hero = self:GetWidget("Panel_hero")
	--名字
	self.Text_name = self:GetWidget("Text_name")
	--对话版
	self.Image_dialogue = self:GetWidget("Image_dialogue")

	--延迟按钮
	self.Button_delay = Util:WidgetButton( 
											self:GetWidget("Button_delay"), 
											CPPGameLib.GetString("rebel_delay") , 
											CPPGameLib.Handler( self,self.OnBtnDelay ) , 
											nil,nil,Macros.Color.button_red )  
	--立刻按钮
	self.Button_immediately = Util:WidgetButton( 
													self:GetWidget("Button_immediately"), 
													CPPGameLib.GetString("rebel_immediately") , 
													CPPGameLib.Handler( self,self.OnBtnImmediately ) , 
													nil,nil,Macros.Color.button_yellow )  
end

function LogicGUI.RebelLayer:__delete()
	if self.modes then
		self.modes:DeleteMe()
		self.modes = nil
	end
end

function LogicGUI.RebelLayer:__ShowUI()
	local data = CallModuleFunc(ModuleType.REBEL, "GetCanHitRebelFindData",true)

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.REBEL, "enrty",true)
	local text = Util:Sprite(sp)
	text:SetPosition(self.Text_name:GetContentWidth()/2,self.Text_name:GetContentHeight()/2)
	text:SetAnchorPoint(0.5,0.5)
	self.Text_name:AddChild(text)

	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    local lab = Util:LabCenter({
							    { zi = CPPGameLib.GetString("rebel_discover1"), dx = Macros.TypefaceSize.normal , mb = true },

							    { zi =  data.name , ys = data.quality , dx = Macros.TypefaceSize.normal , mb = true },

							    { zi =  " (" .. CPPGameLib.GetString("common_lev6" , data.level ) .. ") " , dx = Macros.TypefaceSize.normal , mb = true },

							    { zi = CPPGameLib.GetString("rebel_discover2") , dx = Macros.TypefaceSize.normal , mb = true },
							})
	lab:SetPosition(self.Image_1:GetContentWidth()/2+40 , self.Image_1:GetContentHeight()/2 + 30)
	lab:SetAnchorPoint(0.5,0.5)
	self.Image_1:AddChild(lab)


    CPPGameLib.PrintTable("xy" , data , "data.modes_id")

	self.modes = LogicGUI.Model.New({info_id = data.monster_id, parent = self.Panel_hero  , model_type = data.model_type })
	self.modes:SetPosition(self.Panel_hero:GetContentWidth() - 60 , 0)
	self.modes:SetScaleFactor(1.2)
    local sneer = CPPGameLib.GetConfig("RebelSneer",MathTool.GetRandom(1 , #CPPGameLib.GetConfig("RebelSneer", nil, nil , true )))
	if not sneer then
		return
	end
	self.tips = Util:Label( sneer.talk , Macros.TypefaceSize.slightly ,Macros.Color.brown , 190 )
	self.tips:SetAnchorPoint(0,0.5)
	self.tips:SetPosition( 26 , self.Image_dialogue:GetContentHeight()/2 )
	self.Image_dialogue:AddChild(self.tips)

    local image_1 = self:GetWidget("Image_1")
    image_1:SetImage(Resource.PathTool.GetUIResPath("citycommonslices/bg_discoveryarmy"), TextureResType.LOCAL)
end

--延迟按钮
function LogicGUI.RebelLayer:OnBtnDelay( ... )
	if self.__callback then
		self.__callback(false)
	end

	self:DeleteMe()
end

--立刻按钮
function LogicGUI.RebelLayer:OnBtnImmediately( ... )	
	CallModuleFunc(ModuleType.REBEL, "OpenView", BaseViewType.REBEL)

	if self.__callback then
		self.__callback(true)
	end

	self:DeleteMe()
end
