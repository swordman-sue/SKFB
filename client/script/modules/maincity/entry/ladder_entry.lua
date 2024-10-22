--[[
%% @module: 天梯赛入口
%% @author: yjg
%% @created: 2019年1月5日15:55:52
--]]

Modules = Modules or {}

Modules.LadderEntry = Modules.LadderEntry or BaseClass(Modules.BaseMainCityEntry)

function Modules.LadderEntry:__init()
	--天梯赛
    if self.__entry_info.system_id == Macros.Game.SystemID.LADDER then
	    local seasonInfo = CallModuleFunc(ModuleType.LADDER , "GetSeasonInfo" )
	    if seasonInfo and seasonInfo.countDown then
	    	if self.sp_ladder_game then
	    		self.sp_ladder_game:SetVisible(false)
	    	end
	    else
	    	if self.sp_ladder_game then
	    		self.sp_ladder_game:SetVisible(true)
	    	else
	    		local sp_ladder_game = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"sp_ladder_game",true)
	    		self.sp_ladder_game = Util:Sprite( sp_ladder_game )
	    		self.sp_ladder_game:SetAnchorPoint( 0.5 , 0.5 )
	    		self.sp_ladder_game:SetPosition(self.__entry_btn:GetContentWidth() - 20 , 50 )
	    		self.__entry_btn:AddChild( self.sp_ladder_game )
	    	end
	    end
	end	
end

