
Modules = Modules or {}

--服务器区模板
Modules.LoginServerZone = Modules.LoginServerZone or BaseClass(GUI.ScrollItem)

function Modules.LoginServerZone:__init()
	self:InitTemplate(GUI.TemplateRegistry.LOGIN_SERVER_ZONE)	

	self.cbox_zone = self:GetWidget("CheckBox_Zone")
end

function Modules.LoginServerZone:__delete()
end

--[[
@ data：
	index
	name
--]]
function Modules.LoginServerZone:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	
	Util:WidgetLabel( 
						self:GetWidget("Text_Zone") ,
						data.name, 
						Macros.TypefaceSize.normal, 
						Macros.Color.keypoint
					)
end

function Modules.LoginServerZone:SetSelected(var)
	self.cbox_zone:SetSelectedState(var)
end

