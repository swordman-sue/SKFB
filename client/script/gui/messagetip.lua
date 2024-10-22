
GUI = GUI or {}

-- 弹出提示框
GUI.TipMsgLayer = GUI.TipMsgLayer or BaseClass(UILayer)

function GUI.TipMsgLayer:__init(str, show_bg, quality , path )	
	if show_bg ~= false then

		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp_labTips")
		if path then
			sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, path )
		end

		self.sp_bg = Util:Sprite9( sp )
		self.sp_bg:SetAnchorPoint(0.5, 0.5)
		self:AddChild(self.sp_bg, 0)
	end

	self.error_label = Util:Name(str , Macros.TypefaceSize.largish,quality)
	self.error_label:SetAnchorPoint(0.5, 0.5)
	self:AddChild(self.error_label, 1)

	if self.sp_bg then
		self.sp_bg:SetContentWidth(self.error_label:GetContentWidth() + 80)
	end

	self.run_time = 0
	self.is_fade = false

	self:SetUpdateFunc(CPPGameLib.Handler(self, self.Update))	
end

function GUI.TipMsgLayer:__delete()
end

function GUI.TipMsgLayer:Update(elapse_time)
	local factor = CPPSceneManager:GetDeltaSpeedTime()

	self.run_time = self.run_time + elapse_time / factor
	if self.run_time > 1 and not self.is_fade then
		self.is_fade = true
		CPPActionManager.MoveBy(self:GetNode(), 1.0 / factor, 0, 128)
		CPPActionManager.FadeOut(self:GetNode(), 1.0 / factor)
	end

	if self.run_time >= 3 then
		GlobalTipMsg.GetInstance():DelMsgNode()
	end
end

function GUI.TipMsgLayer:Show( x , y )  
	PosTool.Center(self , x or 0 , y or 0)
	GlobalScene:AddToRenderGroup(self, Scene.RenderQueue.GRQ_UI_MAX_II)
	self:MoveToFront()
end


-- 弹出提示框
GUI.TipMarginLayer = GUI.TipMarginLayer or BaseClass(UILayer)

function GUI.TipMarginLayer:__init(node, show_bg , is_move , is_remove , posY , posX )
	if show_bg ~= false then
		self.sp_bg = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp_labTips"))
		self.sp_bg:SetAnchorPoint(0.5, 0.5)
		self:AddChild(self.sp_bg, 0)
	end
	
	self.error_label = node
	self.error_label:SetAnchorPoint(0.5, 0.5)
	self:AddChild(self.error_label, 1)

	self.run_time = 0
	self.is_fade = false
	self.is_move = is_move
	self.is_remove = is_remove
	self.posY = posY
	self.posX = posX

	self:SetUpdateFunc(CPPGameLib.Handler(self, self.Update))	
end

function GUI.TipMarginLayer:__delete()
end

function GUI.TipMarginLayer:Update(elapse_time)
	local factor = CPPSceneManager:GetDeltaSpeedTime()	
	
	self.run_time = self.run_time + elapse_time / factor
	if self.run_time > 1 and not self.is_fade then
		self.is_fade = true
		if self.is_move ~= false then
			CPPActionManager.MoveBy(self:GetNode(), 1.0 / factor, 0, 128)
		end
		if self.is_remove ~= false then
			if self.sp_bg then	
				CPPActionManager.FadeOut(self.sp_bg, 1.0 / factor)
			end
			CPPActionManager.FadeOut(self.error_label, 1.0 / factor)
		end
	end

	if self.run_time >= 3 then
		if self.is_remove ~= false then
			GlobalTipMsg.GetInstance():DelMsgNode()
		end
	end
end

function GUI.TipMarginLayer:Show()
	PosTool.Center(self , self.posX , self.posY )
	GlobalScene:AddToRenderGroup(self, Scene.RenderQueue.GRQ_UI_MAX_II)
	self:MoveToFront()
end




----------------------单例提示框----------------------
---弹出提示框
GlobalTipMsg = GlobalTipMsg or BaseClass()

function GlobalTipMsg.GetInstance()
	if not GlobalTipMsg.instance then
		GlobalTipMsg.instance = GlobalTipMsg.New()
	end
	return GlobalTipMsg.instance
end

function GlobalTipMsg.DelInstance()
	if GlobalTipMsg.instance then
		GlobalTipMsg.instance:DeleteMe()
		GlobalTipMsg.instance = nil
	end
end

function GlobalTipMsg:__init()
end

function GlobalTipMsg:__delete()
	self:DelMsgNode()
end

function GlobalTipMsg:Show(str, callback,quality)
	self:DelMsgNode()
	self.msg_node = GUI.TipMsgLayer.New(str, callback,quality)
	self.msg_node:Show()
end

function GlobalTipMsg:ShowLayer(node, callback , is_move , is_remove)
	self:DelMsgNode()
	self.msg_node = GUI.TipMarginLayer.New(node, callback , is_move , is_remove )
	self.msg_node:Show()
end

function GlobalTipMsg:ShowGetLayer(str, callback , color , path , y )
	self:DelMsgNode()
	self.msg_node = GUI.TipMsgLayer.New(str, callback , color , path )
	self.msg_node:Show( 0 , y or 0 )
end

function GlobalTipMsg:DelMsgNode()
	if self.msg_node then
		self.msg_node:DeleteMe()
		self.msg_node = nil
	end	
end

----------------------系统模式框----------------------
SysDlgLayer = SysDlgLayer or BaseClass(UILayer)

function SysDlgLayer:__init(title, confirm_text, confirm_fun, cancel_text, cancel_fun)
	confirm_text = confirm_text or CPPGameLib.GetString("common_confirm")
	if cancel_text ~= false then
		cancel_text = cancel_text or CPPGameLib.GetString("common_cancel")
	end

	self:SetAnchorPoint(0.5, 0.5)
	self:SetTouchEnabled(true)
	self:SetTouchSwallow(true)
	self:SetContentSize(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
	Util:SetLayoutBGColor(self, Macros.Color.lilyBlack, 204)

	--背景
	self.sp_bg = Scale9SpriteEx.CreateWithImageFile(Resource.PathTool.Root.."start/tipbk.png")
	self.sp_bg:SetContentSize(465, 180)
	self:AddChild(self.sp_bg, 0, 0)
	PosTool.Center(self.sp_bg)

	--标题
	self.lab_title = Util:Label(title, nil, nil, 400)
	self.lab_title:SetAnchorPoint(0.5, 1)
	self.lab_title:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
	self.sp_bg:AddChild(self.lab_title)
	PosTool.CenterTop(self.lab_title, 0, -10)

	--确定按钮
	self.btn_confirm = Util:Button(Resource.PathTool.GetResPath("start/small_btn.png"), function()
		if confirm_fun then
			confirm_fun()
		end		
		GlobalSysDlg.GetInstance():DelDlgNode()
	end, confirm_text, Macros.TypefaceSize.button, Macros.Color.content)
	self.btn_confirm:SetAnchorPoint(0.5, 0)

	self.sp_bg:AddChild(self.btn_confirm)
	PosTool.CenterBottom(self.btn_confirm, cancel_text and -80 or 0, 10)
	
	--取消按钮
	if cancel_text then
		self.btn_cancel = Util:Button(Resource.PathTool.GetResPath("start/small_btn.png"), function()
			if cancel_fun then
				cancel_fun()
			end		
			GlobalSysDlg.GetInstance():DelDlgNode()
		end, cancel_text, Macros.TypefaceSize.button, Macros.Color.content)
		self.btn_cancel:SetAnchorPoint(0.5, 0)

		self.sp_bg:AddChild(self.btn_cancel)
		PosTool.CenterBottom(self.btn_cancel, 80, 10)
	end
end

function SysDlgLayer:Show()
	PosTool.Center(self)
	GlobalScene:AddToRenderGroup(self, Scene.RenderQueue.GRQ_UI_MAX_II)
end

----------------------单例系统模式框----------------------
GlobalSysDlg = GlobalSysDlg or BaseClass()

function GlobalSysDlg.GetInstance()
	if not GlobalSysDlg.instance then
		GlobalSysDlg.instance = GlobalSysDlg.New()
	end
	return GlobalSysDlg.instance
end

function GlobalSysDlg.DelInstance()
	if GlobalSysDlg.instance then
		GlobalSysDlg.instance:DeleteMe()
		GlobalSysDlg.instance = nil
	end
end

function GlobalSysDlg:__init()
	self.dlg_node = nil
end

function GlobalSysDlg:__delete()
	self:DelDlgNode()
end

function GlobalSysDlg:Show(title, confirm_text, confirm_fun, cancel_text, cancel_fun)
	self:DelDlgNode()

	self.dlg_node = SysDlgLayer.New(title, confirm_text, confirm_fun, cancel_text, cancel_fun)
	self.dlg_node:Show()
end

function GlobalSysDlg:DelDlgNode()
	if self.dlg_node then
		self.dlg_node:DeleteMe()
		self.dlg_node = nil
	end	
end