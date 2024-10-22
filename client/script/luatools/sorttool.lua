
SortTools = SortTools or {}

--[[
从小到大排序的算子(用于表项)
@para2 sort_key_name 需要比较的表项中的key
--]]
function SortTools.KeyLowerSorter(sort_key_name)
	return function(a, b)
		if not a then
			return true
		elseif not b then
			return false
		elseif a[sort_key_name] < b[sort_key_name] then
			return true
		else
			return false
		end
	end
end

--[[
从大到小排序的算子(用于表项)
@para2 sort_key_name 需要比较的表项中的key
--]]
function SortTools.KeyUpperSorter(sort_key_name)
	return function(a, b)
		if not a then
			return true
		elseif not b then
			return false
		elseif a[sort_key_name] > b[sort_key_name] then
			return true
		else
			return false
		end
	end
end

--[[
从小到大排序的算子(用于普通项)
--]]
function SortTools.ItemLowerSorter()
	return function(a, b)
		if not a then
			return true
		elseif not b then
			return false
		else
			return a < b
		end 
	end
end

--[[
从大到小排序的算子(用于普通项)
--]]
function SortTools.ItemUpperSorter()
	return function(a, b)
		if not a then
			return true
		elseif not b then
			return false
		else
			return a > b
		end
	end
end

function SortTools.BinaryInsert(t, value, fcomp)
	-- Initialise compare function
	fcomp = fcomp or SortTools.ItemLowerSorter
	--  Initialise numbers
	local istart, iend, imid, istate = 1, #t, 1, 0
	-- Get insert position
	while istart <= iend do
		-- calculate middle
		imid = math.floor( (istart+iend)/2 )
		-- compare
		if fcomp( value, t[imid] ) then
			iend, istate = imid - 1, 0
		else
		istart, istate = imid + 1, 1
		end
	end
	table.insert(t, (imid + istate), value)
	return (imid + istate)
end

function SortTools.BinarySearch(t, value, equal_func, compare_func)
	--  Initialise numbers
	local max_t = #t
	local istart, iend, imid = 1, max_t, 0
	
	--[[
	if istart == iend then
		local find_t = nil
		if equal_func(t[istart], value) then
			find_t = {istart, iend}
		end
		return find_t
	end
	]]
	
	-- Binary Search
	while istart <= iend do
		-- calculate middle
		imid = math.floor( (istart+iend)/2 )
		-- get compare value
		local value2 = t[imid]
		-- get all values that match
		if equal_func(value, value2) then
			local tfound,num = { imid,imid },imid - 1
			while (num >= 1) and (equal_func(value, t[num])) do
				tfound[1],num = num,num - 1
			end
			num = imid + 1
			while (num <= max_t) and (equal_func(value, t[num])) do
				tfound[2],num = num,num + 1
			end
			return tfound
	 		-- keep searching
		elseif compare_func(value, value2) then
			iend = imid - 1
		else
			istart = imid + 1
		end
	end
end

--冒泡排序
--author:sunny
--list:待排序列表
--sort_word:排序元素
--descending:默认 降序，false 升序
function SortTools.BubbleSort(list,sort_word,descending)

	local comp = function(item0,item1)
	
		--将空项排在后面
		if item0[sort_word] == nil and item0[sort_word] ~= nil then
			return true
		end

		if item0[sort_word] ~= nil and item1[sort_word] ~= nil then

			if descending == false then	
				--升序
				if item0[sort_word] > item1[sort_word] then
					return true
				end
			else
				--降序
				if item0[sort_word] < item1[sort_word] then
					return true
				end
			end

		end

		return false

	end

	local length = #(list)	
	local result = false
	local temp = nil
	local flag = true

	for i=1,length - 1 do
		if not flag then
			break
		end
		flag = false
		for k=1,length - i do
			result = comp(list[k],list[k+1])
			if result then
				flag = true
				temp = list[k]
				list[k] = list[k+1]
				list[k+1] = temp
			end		
		end
	end	
end

--根据func进行冒泡
--返回true，则将后一个替换前一个
--返回false，则不作处理
function SortTools.SortListByFunc(list, func)
	local length = #(list)	
	local result = false
	local temp = nil
	local flag = true

	for i=1,length - 1 do
		if not flag then
			break
		end
		flag = false
		for k=1,length - i do
			result = func(list[k],list[k+1])
			if result then
				flag = true
				temp = list[k]
				list[k] = list[k+1]
				list[k+1] = temp
			end		
		end
	end
end

