
--[[
%% @module: 引用对象
%% @author: swordman sue
%% @created: 2017-3-10
--]]

RefObj = RefObj or BaseClass()

function RefObj:__init()
	self:Reset()
end

function RefObj:Retain()
	self.__ref_count = self.__ref_count + 1
end

function RefObj:Release()
	if not self:IsValid() then
		return
	end

	self.__ref_count = self.__ref_count - 1
end

function RefObj:Reset()
	self.__ref_count = 0
end

function RefObj:IsValid()
	return self.__ref_count > 0
end

function RefObj:GetRefCount()
	return self.__ref_count
end
