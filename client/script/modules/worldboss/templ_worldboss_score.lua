--
-- @brief 世界boss战报
-- @author: yjg
-- @date: 2016年11月21日16:52:23
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplWorldbossScore = LogicGUI.TemplWorldbossScore or BaseClass(GUI.Template)

function LogicGUI.TemplWorldbossScore:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
		self:OnFlee()
	end
    CallModuleFunc(ModuleType.WORLDBOSS, "WorldBossBattleRecordReq")

	self.event_rank = self:BindGlobalEvent(Macros.Event.WorldBossModule.WORLDBOSS_SCORE,function()
    	local lst = CallModuleFunc(ModuleType.WORLDBOSS, "GetBossScore")
    	for i,v in ipairs(lst) do
		    self.listView:PushBackCustomItem( Modules.TemplWorldbossScoreItem.New( self.Image_content:GetContentWidth() , v ):GetNode() )
    	end
	end)

	self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_SCORE)	
end

function LogicGUI.TemplWorldbossScore:__delete()

end

function LogicGUI.TemplWorldbossScore:__Getwidget()
	--背景框
	self.Image_bg = self:GetWidget("Image_bg")
	--光
	self.Image_light = self:GetWidget("Image_light")
	--背景框
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") ,nil , CPPGameLib.Handler(self,self.OnButtonClose) )
	--背景框
	self.Image_content = self:GetWidget("Image_content")
	--背景框
	self.Text_tips = self:GetWidget("Text_tips")
end

function LogicGUI.TemplWorldbossScore:__ShowUI()

    --列表容器
    self.listView = UIListViewEx.Create()
    self.listView:SetAnchorPoint(0.5,0.5)
    self.listView:SetContentSize(self.Image_content:GetContentWidth() ,self.Image_content:GetContentHeight() - 10 )
    self.Image_content:AddChild(self.listView)
    PosTool.Center(self.listView)
    self.listView:SetItemsMargin(5)
    self.listView:SetBounceEnabled(true)

	--奖励
	local score = Util:Label( CPPGameLib.GetString("worldBoss_BossScore") , Macros.TypefaceSize.popup , Macros.Color.white )
	score:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
	score:SetAnchorPoint(0.5,0.5)
	self.Image_light:AddChild(score)
end

--关闭按钮
function LogicGUI.TemplWorldbossScore:OnButtonClose()
	print("关闭按钮")
	self:DeleteMe()
end

--逃跑倒计时
function LogicGUI.TemplWorldbossScore:OnFlee(index)
    local func = nil
    local function moveAnimate( ... )
      local runTime = CallModuleFunc(ModuleType.WORLDBOSS, "isOpen")
      print(runTime)
      self:__UpShowUI(runTime)
      CPPActionManager.DelayTo(self.Image_bg,1,func)
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end


function LogicGUI.TemplWorldbossScore:__UpShowUI(time)
	if self.tab then
		self.tab:RemoveSelf(true)
		self.tab = nil
	end
	if not time then return end	
	local lab = "HH"..CPPGameLib.GetString("common_hour").."MM"..CPPGameLib.GetString("common_minute").."SS"..CPPGameLib.GetString("common_second")
	self.tab = Util:LabCenter( {
								{zi = TimeTool.TransTimeStamp(lab , time) , dx = Macros.TypefaceSize.normal , ys = Macros.Color.keypoint },
								{zi = CPPGameLib.GetString("worldBoss_runTime") , dx = Macros.TypefaceSize.normal , ys = Macros.Color.content }
							} )
	self.tab:SetAnchorPoint(0.5,0.5)
	self.tab:SetPosition(self.Text_tips:GetContentWidth()/2 , self.Text_tips:GetContentHeight()/2)
	self.Text_tips:AddChild(self.tab)
end


Modules = Modules or {}
Modules.TemplWorldbossScoreItem = Modules.TemplWorldbossScoreItem or BaseClass( UILayer )

function Modules.TemplWorldbossScoreItem:__init( white , data )
	self:SetAnchorPoint( 0.5 , 0.5 )
	self:SetContentSize( white , 160 )

    if data.luck_attack_name ~= "" and data. kill_name ~= "" then
    	self:SetContentSize( white , 160 )
    else
    	self:SetContentSize( white , 110 )
    end

	self:__ShowUI(data)
end

function Modules.TemplWorldbossScoreItem:__ShowUI( data )
 	local bg_sp9_get_approach = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_get_approach")
	self.approach = Util:Sprite9( bg_sp9_get_approach )
	self.approach:SetContentSize( self:GetWidth() - 5 , self:GetHeight() )
	self.approach:SetAnchorPoint(0.5,0.5)
	self.approach:SetPosition( self:GetWidth()/2 , self:GetHeight()/2 )
	self:AddChild(self.approach)

    local bossCnf = CPPGameLib.GetConfig("WorldBoss", data.boss_lv )
    local cnf = CPPGameLib.GetConfig("GameLevel", tonumber(bossCnf.map_id))
	local name = Util:Label( cnf.name .. " " .. CPPGameLib.GetString("common_lev6" ,data.boss_lv )  , Macros.TypefaceSize.largish , Macros.Color.headline )
	name:SetAnchorPoint( 0 , 0.5 )
	name:SetPosition( 20 , self.approach:GetContentHeight() - name:GetContentHeight() )
	self.approach:AddChild( name )

	local cut_off_line = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "cut_off_line")
	local line = Util:Sprite9( cut_off_line )
	line:SetAnchorPoint(0.5,0.5)
	line:SetContentSize( self.approach:GetContentWidth() - 30 , line:GetContentHeight() )
	line:SetPosition( self.approach:GetContentWidth()/2 , name:GetPositionY() - (name:GetContentHeight()/2) - 5 )
	self.approach:AddChild(line)


	local posY = 0
	if data.luck_attack_name ~= "" then
		local kill_time = TimeTool.GetTimeFormat( data.luck_attack_time , TimeTool.defaul_format[15])
		local time = Util:Label( kill_time , Macros.TypefaceSize.normal , Macros.Color.keypoint )

		local type_ , info_ = ObjIDTool.GetInfo(bossCnf.killer_reward_id)
		local tab = {}
		if info_.sub_type == Macros.Game.NormalItemSubType.CURRENCY then
			local quality = 1
			if info_.quality == 0 then
				quality = 1
			end
			tab = 
				{
					{ zi = kill_time, ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
					{ zi = CPPGameLib.GetString( "common_brackets" , data.luck_attack_name ) , ys = Macros.Color.front_color , dx = Macros.TypefaceSize.normal },
					{ zi = CPPGameLib.GetString("worldBoss_BossScoreLuck") , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },
					{ tp = Util:GetMoneyIconImgData(info_.param1) , dx = 1.5 , gs = "X".. bossCnf.killer_reward_num , ys = Macros.Quality[quality] , mb = Macros.Quality_Hex_Stroke[quality] , gsdx = Macros.TypefaceSize.normal },
				}
		else
			tab = 
				{
					{ zi = kill_time , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
					{ zi = CPPGameLib.GetString( "common_brackets" , data.luck_attack_name ) , ys = Macros.Color.front_color , dx = Macros.TypefaceSize.normal },
					{ zi = CPPGameLib.GetString("worldBoss_BossScoreLuck") , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },quality
					{ zi = info_.name , ys = Macros.Quality[info_.quality] , mb = Macros.Quality_Stroke[info_.quality] , dx = Macros.TypefaceSize.normal },
				}
		end

		local item = Util:LabCenter_3( tab , Macros.TypefaceSize.normal, self.approach:GetContentWidth() - 20 )
		item:SetAnchorPoint( 0 , 1 )
		item:SetPosition( 10 , line:GetPositionY() - ( line:GetContentHeight()/2 ) - 5 )
		self.approach:AddChild(item)

		posY = item:GetContentHeight()

	end

	if data.kill_name ~= "" then
		local kill_time = TimeTool.GetTimeFormat( data.kill_time , TimeTool.defaul_format[15])
		local time = Util:Label( kill_time , Macros.TypefaceSize.normal , Macros.Color.keypoint )
		local type_ , info_ = ObjIDTool.GetInfo(bossCnf.killer_reward_id)
		local tab = {}   
		if info_.sub_type == Macros.Game.NormalItemSubType.CURRENCY then
			local quality = 1
			if info_.quality == 0 then
				quality = 1
			end
			tab = 
				{
					{ zi = kill_time, ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
					{ zi = CPPGameLib.GetString( "common_brackets" , data.kill_name ) , ys = Macros.Color.front_color , dx = Macros.TypefaceSize.normal },
					{ zi = CPPGameLib.GetString("worldBoss_BossScoreKill") , ys = Macros.Color.content  , dx = Macros.TypefaceSize.normal },
					{ tp = Util:GetMoneyIconImgData(info_.param1) , dx = 1.5 , gs = "X".. bossCnf.killer_reward_num , ys = Macros.Quality[quality] , mb = Macros.Quality_Hex_Stroke[quality] , gsdx = Macros.TypefaceSize.normal },
				}
		else
			tab = 
				{
					{ zi = kill_time, ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
					{ zi = CPPGameLib.GetString( "common_brackets" , data.kill_name ) , ys = Macros.Color.front_color , dx = Macros.TypefaceSize.normal },
					{ zi = CPPGameLib.GetString("worldBoss_BossScoreKill") , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },quality
					{ zi = info_.name , ys = Macros.Quality[info_.quality] , mb = Macros.Quality_Stroke[info_.quality] , dx = Macros.TypefaceSize.normal },
				}
		end

		local item = Util:LabCenter_3( tab , Macros.TypefaceSize.normal, self.approach:GetContentWidth() - 20 )
		item:SetAnchorPoint( 0 , 1 )
		item:SetPosition( 10 , line:GetPositionY() - ( line:GetContentHeight()/2 ) - 5 - posY )
		self.approach:AddChild(item)
	end

end