--
-- @brief 冲榜活动 
-- @author: yjg
-- @date: 2017年9月6日18:00:33
--

Modules = Modules or {}

Modules.RankActivityModule = Modules.RankActivityModule or BaseClass(BaseModule)

function Modules.RankActivityModule:__init()

	self.rank = {}
	local cnf = CPPGameLib.GetConfig("SevenDayRankReward", nil , nil, true)
	if not cnf then return end
	for _ , v in ipairs( cnf ) do
		if not self.rank[v.rank_type] then
			self.rank[v.rank_type] = {}
		end

		--区间
		local section = v.rank_min
		if v.rank_min ~= v.rank_max then
			section = v.rank_min .. "~" .. v.rank_max
		end

		--奖励
		local award = {}
		for i , vv in ipairs( v.item_id_list ) do
			table.insert( award , { id = vv , num = v.item_num_list[i] } )
		end

		table.insert( self.rank[v.rank_type] , { 
													rank = v.rank_min , 
													section = section ,
													award = award
												} )
	end


	--竞技
	table.sort( self.rank[ Macros.Game.RankType.ARENA ], function ( a , b )
		return a.rank < b.rank
	end )

	for i , v in ipairs( self.rank[ Macros.Game.RankType.ARENA ] ) do
		if i%2 ~= 0 then
			v.show = true
		else
			v.show = false
		end
	end

	--主线
	table.sort( self.rank[ Macros.Game.RankType.MAIN_DUNGEON_STAR ], function ( a , b )
		return a.rank < b.rank
	end )

	for i , v in ipairs( self.rank[ Macros.Game.RankType.MAIN_DUNGEON_STAR ] ) do
		if i%2 ~= 0 then
			v.show = true
		else
			v.show = false
		end
	end

	--战力
	table.sort( self.rank[ Macros.Game.RankType.BATTLE ], function ( a , b )
		return a.rank < b.rank
	end )

	for i , v in ipairs( self.rank[ Macros.Game.RankType.BATTLE ] ) do
		if i%2 ~= 0 then
			v.show = true
		else
			v.show = false
		end
	end

end

function Modules.RankActivityModule:__delete()
end

-- 消息说明: 请求我的排名 消息Id: 20811
function Modules.RankActivityModule:MYRankReq( index )

	local info = {}
	if index == 1 then
		info.rank_type = Macros.Game.RankType.ARENA
	elseif index == 2 then
		info.rank_type = Macros.Game.RankType.MAIN_DUNGEON_STAR 
	elseif index == 3 then
		info.rank_type = Macros.Game.RankType.BATTLE
	end

	CallModuleFunc(ModuleType.RANK, "MYRankReq" , info )
end


function Modules.RankActivityModule:SetArenaRank( int )
    local view = self:GetView()
    if view and view:IsOpen() then
        view:SetMyRank( int )
    end   
end

function Modules.RankActivityModule:SetManinDungeonStarRank( int )
    local view = self:GetView()
    if view and view:IsOpen() then
        view:SetMyRank( int )
    end   
end

function Modules.RankActivityModule:SetBattleRank( int )
    local view = self:GetView()
    if view and view:IsOpen() then
        view:SetMyRank( int )
    end   
end

--竞技
function Modules.RankActivityModule:GetAthleticsCnf()
	if self.rank[ Macros.Game.RankType.ARENA ][ #self.rank[ Macros.Game.RankType.ARENA ] ].show == false then
		table.insert( self.rank[ Macros.Game.RankType.ARENA ] , { show = true } )
	end
	return self.rank[ Macros.Game.RankType.ARENA ]
end
	
--主线
function Modules.RankActivityModule:GetMainlinesCnf()
	if self.rank[ Macros.Game.RankType.MAIN_DUNGEON_STAR ][ #self.rank[ Macros.Game.RankType.MAIN_DUNGEON_STAR ] ].show == false then
		table.insert( self.rank[ Macros.Game.RankType.MAIN_DUNGEON_STAR ] , { show = true } )
	end
	return self.rank[ Macros.Game.RankType.MAIN_DUNGEON_STAR ]
end

--战力
function Modules.RankActivityModule:GetPowerCnf()
	if self.rank[ Macros.Game.RankType.BATTLE ][ #self.rank[ Macros.Game.RankType.BATTLE ] ].show == false then
		table.insert( self.rank[ Macros.Game.RankType.BATTLE ] , { show = true } )
	end
	return self.rank[ Macros.Game.RankType.BATTLE ]
end


