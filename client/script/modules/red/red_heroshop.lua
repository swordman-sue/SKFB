
--[[
%% @module: 英雄商店红点
--]]
Modules.Red.HeroShop = Modules.Red.HeroShop or BaseClass( Modules.Red.BaseRed )

function Modules.Red.HeroShop:__init( ... )
end

--红点统计逻辑
function Modules.Red.HeroShop:__Statistics( sub_system_id )
    self.red = 0

    local free_refresh_times = CallModuleFunc(ModuleType.HERO_SHOP,"GetFreeRefreshTimes")
    if not free_refresh_times then
        return self.red
    else
        if GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_SHOP) then
            local HeroShopFreeRefresh = CallModuleFunc(ModuleType.HERO_SHOP,"GetHeroShopFreeRefresh")
            if HeroShopFreeRefresh == ConfigAdapter.Common.GetHeroShopFreeNum() then
                self.red = self.red + 1
            end
        end
        if GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_AWAKEN_SHOP) then
            local HeroShopFreeRefresh = CallModuleFunc(ModuleType.HERO_SHOP,"GetAwakenShopFreeRefresh")
            if self:GetAwakenShopFreeRefresh() == ConfigAdapter.Common.GetAwakenShopFreeNum() then
                self.red = self.red + 1
            end
        end
        return self.red
    end

end