LogicGUI = LogicGUI or {}

LogicGUI.TemplChooseCard = LogicGUI.TemplChooseCard or BaseClass(GUI.Template)
local flip_time = 0.3
local skew_du = 8
function LogicGUI.TemplChooseCard:__init(type,finished_callback)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__fb_type = type
	self.__open_callback = function ()
		self:createUI(type)
		CallModuleFunc(ModuleType.Pack,"ChooseCardReq",type)		
	end	
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_CHOOSE_CARD)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_CHOOSE_CARD,true)
	self.__finished_callback = finished_callback
	
	self.__getChooseCardDataEvent = self:BindGlobalEvent(Macros.Event.PackModule.GET_CHOOSECARD_DATA, function(_,data)
        self.chooseCardData = {}
        if data.create_item_id ~= 0 then
        	self.chooseCardData.CardId = data.create_item_id
        	self.chooseCardData.CardNum = data.create_item_num
        else
        	local money_Config = CPPGameLib.GetConfig("HuoBi",data.currency_type)
			self.chooseCardData.CardId = money_Config.big_icon
        	self.chooseCardData.CardNum = data.currency_value
        end  
        --翻牌翻到所要碎片
        if self.__fb_type == Macros.Game.FBType.PLUNDERED_TREASURES then 
        	self.__selected_treasuresId,self.__selected_treasuresPieceId = CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"GetCurtreasureInfoId")
       		self.__is_choosecard_getpiece = self.__selected_treasuresPieceId == self.chooseCardData.CardId
       	end          	
		self:SetCallBackData(self.chooseCardData.CardId)
    end)

	self:InitTemplate(GUI.TemplateRegistry.BATTLE_CHOOSE_CARD) 
end

function LogicGUI.TemplChooseCard:createUI(type)
	local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local info = nil
	local cardInfo = nil
	--根据类型获取抽卡礼包的信息
	if type == Macros.Game.FBType.PLUNDERED_TREASURES then
		info = CPPGameLib.GetConfig("DuobaoMap",roleLevel)
		cardInfo = CPPGameLib.GetConfig("Item",info.fp_gift_id)
	else
		info = CPPGameLib.GetConfig("ArenaAward",roleLevel)
		cardInfo = CPPGameLib.GetConfig("Item",info.fp_item_id)
	end	
	--获取权值
	local weightNum = 0
	for i,value in ipairs(cardInfo.item_rate_list) do
		weightNum = weightNum + value
	end	
	--图片适配
	self:GetWidget("Image_3"):IgnoreContentAdaptWithSize(true)
	--self.randomSuccess = false
	--[[while(not self.randomSuccess) do
		
	end--]]
	--随机另外两个物品
	self:RandomCardIndex(weightNum,cardInfo)
	
	--点击抽取文字
	self.Panel_chooseencourage = self:GetWidget("Panel_chooseencourage")
	--点击抽取
	Util:WidgetLabel(self:GetWidget("Text_5"),CPPGameLib.GetString("plunderedtreasures_ChooseTextOne"),Macros.TypefaceSize.largish, Macros.Color.btn)
	--3选1
	local text_6 = self:GetWidget("Text_6")
	Util:WidgetLabel(text_6,CPPGameLib.GetString("plunderedtreasures_ChooseTextTwo") ,Macros.TypefaceSize.largish, Macros.Color.orange)
	--抽取文字
	self.Panel_encourage = self:GetWidget("Panel_encourage")
	--恭喜获得
	Util:WidgetLabel(self:GetWidget("Text_congratruation"),CPPGameLib.GetString("plunderedtreasures_ChooseFinshTextOne"),Macros.TypefaceSize.largish, Macros.Color.btn)	
	--点击屏幕继续
	self.Text_continute = self:GetWidget("Text_continute")
	self.Text_continute:SetVisible(false)
	Util:WidgetLabel(self.Text_continute,CPPGameLib.GetString("plunderedtreasures_TouchScreenTip"), Macros.TypefaceSize.popup, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
end

function LogicGUI.TemplChooseCard:SetCallBackData(cardid)
	for i=1,3 do
		self["ImageBackCard_"..i] = self:GetWidget("ImageBackCard_"..i)
		local function callBackFunc()
			self.curIndex = i	
			for i=1,3 do
				self["ImageBackCard_"..i]:SetTouchEnabled(false)
			end
			table.insert(self.cardTable,i,self.chooseCardData)	
			self:SetCardData()
			self:RunCardAction(i)
		end
		WidgetUnity.SetWidgetListener(self["ImageBackCard_"..i],callBackFunc,nil,nil,nil)
		self["ImageFrontCard_"..i] = self:GetWidget("ImageFrontCard_"..i)
		self["ImageFrontCard_"..i]:SetVisible(false)
		self["PanelCardIcon"..i] = WidgetUnity.GetWidgetByName(self["ImageFrontCard_"..i],"Panel_icon")
		self["PanelCardIconName"..i] = WidgetUnity.GetWidgetByName(self["ImageFrontCard_"..i],"Text_itemname")
		Util:WidgetLabel(self["PanelCardIconName"..i],"",Macros.TypefaceSize.normal,nil,150,55)
	end
	--获得物品
	local info = CPPGameLib.GetConfig("Item",cardid)
	Util:WidgetLabel(self:GetWidget("Text_encourageitem"),info.name,Macros.TypefaceSize.largish, Macros.Quality[info.quality])
	self.Panel_encourage:SetVisible(false)
end

function LogicGUI.TemplChooseCard:SetCardData()
	for i,v in ipairs(self.cardTable) do
		local icon = LogicGUI.IconLayer.New()
		self["PanelCardIcon"..i]:AddChild(icon:GetNode())
		icon:SetData({id = v.CardId,num = v.CardNum})
		PosTool.Center(icon)
		icon:adaptParentContentSize()
		local info = CPPGameLib.GetConfig("Item",v.CardId)
		self["PanelCardIconName"..i]:SetText(info.name)
		self["PanelCardIconName"..i]:SetColor(Macros.QualityColor(info.quality))
		self["PanelCardIconName"..i]:SetStrokeColor(Macros.QualityColorStroke(info.quality))		
	end
end

function LogicGUI.TemplChooseCard:RandomCardIndex(weightNum,cardInfo)
	self.cardTable = {}
	for k=1,2 do
		local randomNum = MathTool.GetRandom(1,weightNum)
		local num_1 = 0
		local num_2 = 0
		for i,v in ipairs(cardInfo.item_rate_list) do			
			num_1 = num_2
			num_2 = num_2 + v
			if randomNum > num_1 and randomNum <= num_2 then
				local data = {}
				--self["CardIndex"..k] = i
				data.CardId = cardInfo.item_id_list[i]
				data.CardNum = MathTool.GetRandom(cardInfo.item_min_num_list[i],cardInfo.item_max_num_list[i])
				table.insert(self.cardTable,data)
				break
			end
		end
		--[[
		if k == 2 then
			if (self.CardIndex1 ~= self.CardIndex2)  then
				self.randomSuccess = true
			else
				self.randomSuccess = false
			end
		end --]]
	end
end

function LogicGUI.TemplChooseCard:RunCardAction(k)
	local function touchScreenClose()
		self.Text_continute:SetVisible(true)
		WidgetUnity.SetWidgetListener(self:GetNode(),nil,nil,function ()
			if self.__finished_callback then
				self.__finished_callback(self.__is_choosecard_getpiece)
			end
			self:DeleteMe()
		end)	
	end
	local function callTenTimes()
		for i=1,3 do
			if i ~= self.curIndex then
				self:RunCardAction(i)
			end	
		end
	end
	local function finalCallBack()		
		self["PanelCardIcon"..k]:SetVisible(true)
		self["PanelCardIconName"..k]:SetVisible(true)		
		if k ~= self.curIndex then
			self.Panel_chooseencourage:SetVisible(false)
			self.Panel_encourage:SetVisible(true)
			local funId = GlobalCallbackMgr:AddCallBackFunc(touchScreenClose)
			CPPActionManager.DelayTo(self.Panel_encourage,0.2,funId)
		else
			local effectid,effect = self:__CreateEffect({id = 100034}, self["ImageFrontCard_"..k],2)
			PosTool.Center(effect)
			local funId = GlobalCallbackMgr:AddCallBackFunc(callTenTimes)
			CPPActionManager.DelayTo(self["ImageFrontCard_"..k],0.2,funId)
		end
	end
	
	local function zhengmiancardStartRotate()
		local funId = GlobalCallbackMgr:AddCallBackFunc(finalCallBack)
		self["ImageFrontCard_"..k]:SetVisible(true)
		self["ImageFrontCard_"..k]:SetScaleXFactor(0)
		CPPActionManager.ScaleToXY(self["ImageFrontCard_"..k],flip_time,1,1,funId)
		self["ImageFrontCard_"..k]:SetSkewY(-skew_du)
		CPPActionManager.SkewToXY(self["ImageFrontCard_"..k],flip_time,0,0)
	end
	
	local funId = GlobalCallbackMgr:AddCallBackFunc(zhengmiancardStartRotate)
	CPPActionManager.ScaleToXY(self["ImageBackCard_"..k],flip_time,0,1,funId)
	CPPActionManager.SkewByXY(self["ImageBackCard_"..k],flip_time,0,skew_du)	
end

function LogicGUI.TemplChooseCard:__delete()
	if self.__getChooseCardDataEvent then
		self:UnBind(self.__getChooseCardDataEvent)
    	self.__getChooseCardDataEvent = nil
	end	
end