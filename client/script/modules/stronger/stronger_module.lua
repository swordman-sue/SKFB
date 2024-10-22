Modules = Modules or {}

Modules.StrongerModule = Modules.StrongerModule or BaseClass(BaseModule)


function Modules.StrongerModule:__init()
	self.isFirstOpen = true
end

function Modules.StrongerModule:__delete()
	
end

--是不是第一次打开
function Modules.StrongerModule:SetisFirstOpen( bol )
	self.isFirstOpen = bol

	self:FireNextFrame(Macros.Event.RedModule.RED_UPDATE, Macros.Game.SystemID.STRONGER)
end

--是不是第一次打开
function Modules.StrongerModule:GetisFirstOpen( )
	return self.isFirstOpen
end
