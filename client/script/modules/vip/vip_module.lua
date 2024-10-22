--[[
%% @module: vip
%% @author: yjg
%% @created: 2016年10月25日09:33:40
--]]


Modules = Modules or {}

Modules.VIPModule = Modules.VIPModule or BaseClass(BaseModule)

function Modules.VIPModule:__init()
    self.shopBuy = nil
end

function Modules.VIPModule:__delete()
end

function Modules.VIPModule:GetVipLV()
    local cnf = CPPGameLib.GetConfig("Vip", nil, nil , true )
    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    return userDate.vip_level , #cnf
end

function Modules.VIPModule:GetVipExp( level )
    local lv =  level or self:GetVipLV()
    local cnf = CPPGameLib.GetConfig("Vip", lv)
    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    return userDate.vip_exp , cnf.need_vip_exp
end

--处理提示表
function Modules.VIPModule:GetVipTipsCnf()
    local t1 = {}
    for k,v in pairs(CPPGameLib.GetConfig("vipTips", nil, nil , true )) do
        table.insert(t1,v)
    end

    table.sort( t1, function ( a , b )
        return a.level < b.level
    end )

    local t2 = {}
    for i,v in ipairs(t1) do
        local cnf = CPPGameLib.GetConfig("Vip", v.level)
        if cnf.show_vip_level <= self:GetVipLV() then
            local lst = CPPGameLib.SplitEx(v.privilege,"<GH>")
            table.insert( t2 , { level = v.level , lst = lst })
        end
    end

    return t2
end

--获得礼包物品
function Modules.VIPModule:GetVipPackCnf(lv)
    local itemId = CPPGameLib.GetConfig("Vip", lv or self:GetVipLV() ).getgoods
    local itemLst = CPPGameLib.GetConfig("Item", itemId)

    local t1 = {}
    for i,v in ipairs(itemLst.item_id_list) do
        table.insert(t1,{id = v , num = itemLst.item_min_num_list[i] })
    end
    return t1
end

--权限整理
function Modules.VIPModule:GetPrivilege(type , level )
    local lv = level or self:GetVipLV()
    local cnf = CPPGameLib.GetConfig("Vip", lv)
    return cnf and cnf["value"..type]
end

--vip开启等级
function Modules.VIPModule:GetToVipOpen(type)
    for i = 0 , #CPPGameLib.GetConfig("Vip", nil, nil , true ) do
        local info = self:GetPrivilege( type , i )
        if info and info ~= 0 then
            return i
        end
    end
    return 0
end

--礼包购买记录请求
function Modules.VIPModule:ShopBuyRecordRequest()
    -- print("礼包购买记录请求")
    CallModuleFunc(ModuleType.SHOP,"ShopBuyRecordRequest",Macros.Game.SHOP.STOREOEM,true)
end

--礼包购买记录回调
function Modules.VIPModule:ShopBuyRecordRequestRqsp(data)
    CPPGameLib.PrintTable("xy" , data,"礼包购买记录回调")  
    self.shopBuy = data
end

--礼包购买记录
function Modules.VIPModule:GetShopBuy(data)
    return self.shopBuy
end

--Get礼包购买整理
function Modules.VIPModule:GetShopBuyRecordRequest(lv)
    local itemId = CPPGameLib.GetConfig("Vip", lv).getgoods
    local index = nil
    for i,itemData in pairs( CPPGameLib.GetConfig( "GiftShop" ,nil,nil,true ) ) do
        if itemData.item_id == itemId then
            index = i
            break
        end
    end
    if not self.shopBuy or #self.shopBuy <= 0 then
        return nil
    end

    local t1 = {}
    for i,v in ipairs(self.shopBuy) do
        t1[v.goods_id] = v
    end

    if t1[index] then
        return true
    else
        return false
    end
end

--重置次数计算
function Modules.VIPModule:ToTypeGetResetItem( type_ )
    if not type_ then
        return false
    end
    local num = self:GetPrivilege( type_ )

    local item = false 
    local viplevel = self:GetVipLV()

    for i = self:GetVipLV() , #CPPGameLib.GetConfig("Vip", nil, nil , true )  do
        local value = self:GetPrivilege( type_ , i )
        if num ~= value then
            item = true
            viplevel = i
            break         
        end
    end

    return item , viplevel 
end

--商店购买次数计算
function Modules.VIPModule:ToTypeGetShopBuyResetItem( type_ )
    local item , viplevel = self:ToTypeGetResetItem( type_ )

    --目标等级
    -- viplevel
    print("xy" , "viplevel=============" , viplevel )

    --上限增加
    local nexNum = self:GetPrivilege( type_ , viplevel )
    if not nexNum then
        return item , viplevel , 0 , 0
    end
    local now = self:GetPrivilege( type_ )
    local addnum = nexNum - now
    print("xy" , "addnum=============" , addnum )

    --再充值
    local exp = 0
    for i = self:GetVipLV() , viplevel do
        if i == viplevel then
            break
        end

        local cnf = CPPGameLib.GetConfig("Vip", i )
        exp = exp + cnf.need_vip_exp
    end
    exp = exp - self:GetVipExp()

    return item , viplevel , addnum , exp
end

--扩展背包
--鬼知道会发生什么的预留
function Modules.VIPModule:ExtendPack( type )

    local nowinfo = self:GetPrivilege( type , self:GetVipLV() )
    local index = 0
    local lv = nil
    local max = nil
    for i = self:GetVipLV() , 16 do
        index = index + 1
        local nexinfo = self:GetPrivilege( type , self:GetVipLV() + index )
        if nexinfo and nowinfo ~= nexinfo then
            lv = self:GetVipLV() + index 
            max = nexinfo
            break
        end
    end

    return lv , max
end