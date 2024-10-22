--[[
用于呈现一个自动排序的Map
note:该Map允许key为空的项存在!
--]]
SortMap = SortMap or BaseClass()

--[[
构造函数
@para1 key_compare_func in 用于对Key进行比较的函数(可选)
@author swordman sue
@create 5/18/2012
--]]
function SortMap:__init(key_compare_func)
	self.compare_func = key_compare_func or SortTools.ItemLowerSorter()
	self.equal_func = function(a, b)
		return (not self.compare_func(a, b)) and (not self.compare_func(b,a))
	end
	self.value_list = {}
	self.key_list = {}
end

--[[
判断SortMap是否为空
@return SortMap为空返回true
@author swordman sue
@create 5/18/2012
--]]
function SortMap:Empty()
	return #self.key_list == 0
end

--[[
返回SortMap包含的项数
@return 包含的项数
@author swordman sue
@create 5/18/2012
--]]
function SortMap:Size()
	return #self.key_list
end

--[[
向SortMap插入一项
@para1 key in key
@para2 value in 值项
@return 为true表示插入成功,为false表示插入失败
@author swordman sue
@create 5/18/2012
--]]
function SortMap:Insert(key, value)
	local find_result = SortTools.BinarySearch(self.key_list, key, self.equal_func, self.compare_func)
	if find_result == nil then
		SortTools.BinaryInsert(self.key_list, key, self.compare_func)
		self.value_list[key] = value
		return true
	else
		return false
	end
end

--[[
遍历SortMap
@para1 oper_func in 遍历会挨项调用此函数,此函数接受两个参数,oper_func(key, value)
@author swordman sue
@create 5/18/2012
--]]
function SortMap:ForEach(oper_func)
	for i = 1, #self.key_list do
		local key = self.key_list[i]
		local val = self.value_list[key]
		oper_func(key, val)
	end
end

--[[
转换SortMap中的某一项, 已经存在对应项则直接替换
@para1 key in key
@para2 value in 值项
@author swordman sue
@create 5/18/2012
--]]
function SortMap:Replace(key, value)
	local find_result = SortTools.BinarySearch(self.key_list, key, self.equal_func, self.compare_func)
	if find_result == nil then
		SortTools.BinaryInsert(self.key_list, key, self.compare_func)
		self.value_list[key] = value
	else
		self.value_list[key] = value
	end
end

--[[
判断SortMap是否包含某个key值
@para1 key in key
@return 为true表示SortMap已经包含对应项
@author swordman sue
@create 5/18/2012
--]]
function SortMap:ContainKey(key)
	local find_result = SortTools.BinarySearch(self.key_list, key, self.equal_func, self.compare_func)
	return find_result ~= nil
end

--[[
在SortMap中查找某一Key对应的Value
@para1 key in key
@return 返回找到的项
@author swordman sue
@create 5/18/2012
@note 注意,该处不能以返回值为nil来判断SortMap是否包含对应的项
--]]
function SortMap:Find(key)
	return self.value_list[key]
end

--[[
从SortMap中删除某个Key对应的项
@para1 key in key
@author swordman sue
@create 5/18/2012
--]]
function SortMap:Erase(key)
	local find_result = SortTools.BinarySearch(self.key_list, key, self.equal_func, self.compare_func)
	if find_result ~= nil then
		table.remove(self.key_list, find_result[1])
		self.value_list[key] = nil
	end
end

--[[
以索引方式获取SortMap中的某一项
@para1 index in 从零开始的索引
@return 返回两项,分别为: key, value
@author swordman sue
@create 5/18/2012
--]]
function SortMap:GetByIndex(index)
	local real_index = index + 1
	if real_index <= self:Size() then
		local tmp_key = self.key_list[real_index]
		return tmp_key, self.value_list[tmp_key]
	end
	return nil, nil
end

