
-----------------------------------Table相关-----------------------------------
-------------------------------------------------------------------------------

--unpack({1,1,nil,1,nil})，这种情况下，结果为1,1，所以重写
unpack_sys = unpack
unpack = function(t, i, l)
    local count = table.maxn(t)
    return unpack_sys(t, 1, count)
end

function CPPGameLib.TableIsEmpty(t)
    return _G.next(t) == nil
end

-- 将table里面的key和value都转成字符串
function CPPGameLib.ToStringTable(tb)
	if tb == nil then
		return nil
	end

	if type(tb) ~= "table" then
		return tostring(tb)
	end

	local tmp_tb = {}
	for k,v in pairs(tb) do
		local tmp_value = CPPGameLib.ToStringTable(v)
		tmp_tb[tostring(k)] = tmp_value
	end
	return tmp_tb
end

--将表转化成字符串
function CPPGameLib.VarDump(var,tab)
	local get_tab_string = function (tab_num)
		local tab_str = ""
		if tab_num ~= nil then
			for i = 1,tab_num do
				tab_str = tab_str .. "\t"
			end
		end
		return tab_str
	end

	local head_tab_str= get_tab_string(tab)
	local body_tab_str= head_tab_str.."\t"

	if tab == nil then tab = 0 end

	if type(var) ~= "table" then
		return head_tab_str..tostring(var).."\r\n"
	else
		local str=""
		for k,v in pairs(var) do
			local tmp = CPPGameLib.VarDump(v, tab+1)
			str = str .. string.format("%s%s:\r\n%s{\r\n%s%s}\r\n", head_tab_str, tostring(k), head_tab_str, tmp, head_tab_str)
		end
		return str
	end
end

function CPPGameLib.CopyTblShallow(object)
    local new_table = {}
    for index, value in pairs(object) do
        new_table[index] = value
    end
    return new_table
end

function CPPGameLib.CopyListShallow(list)
    local new_list = {}
    for k, v in ipairs(list) do
        table.insert(new_list, v)
    end
    return new_list
end

function CPPGameLib.CopyTbl(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end

        local new_table = {}
        lookup_table[object] = new_table
        for index, value in pairs(object) do
            new_table[_copy(index)] = _copy(value)
        end
        return new_table
    end

    return _copy(object)
end

function CPPGameLib.CopyTblDeep(object)
    if not object then
        return nil
    end
    
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end

        local new_table = {}
        lookup_table[object] = new_table
        for index, value in pairs(object) do
            new_table[_copy(index)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(object))
    end

    return _copy(object)
end

--得到hash表的长度
function CPPGameLib.GetTableLength(temp_table)
    if not temp_table then
        return 0
    end
    
	local length = 0
	if type(temp_table) == "table" then
		for _, v in pairs(temp_table) do
			length = length + 1
		end
	end
	return length
end

--解析Table
function CPPGameLib.ParseTable(tbl, split, extend)
    split = split.."  "

    local result = "{\n"

    for k, v in pairs(tbl) do
        local cur = split
        local lvalue_split = type(v) == "string" and "\"" or ""
        local rvalue_split = type(v) == "string" and "\"" or ""
		-- 不显示无关内容
        local flag = true
        if type(k) == "number" then
            if type(v) == "table" then
                cur = cur.."["..k.."] = "..CPPGameLib.ParseTable(v, split, extend)
            elseif type(v) == "function" or type(v) == "userdata" then
                flag = false
            else
                cur = cur.."["..k.."] = "..lvalue_split..tostring(v)..rvalue_split
            end            

        elseif type(k) == "table" then
            if type(v) == "table" then
                cur = cur.."["..CPPGameLib.ParseTable(k, split, extend).."] = "..CPPGameLib.ParseTable(v, split, extend)
            elseif type(v) == "function" or type(v) == "userdata" then
                flag = false
            else
                cur = cur.."["..CPPGameLib.ParseTable(k, split, extend).."] = "..lvalue_split..tostring(v)..rvalue_split
            end                        

        elseif k == "__class_type" or k == "DeleteMe" then
            flag = false
        else
            local lkey_split = extend and "[\"" or ""
            local rkey_split = extend and "\"]" or ""
            if type(v) == "table" then
                cur = cur..lkey_split..k..rkey_split.." = "..CPPGameLib.ParseTable(v, split, extend)
            elseif type(v) == "function" or type(v) == "userdata" then
                flag = false
            else
                cur = cur..lkey_split..k..rkey_split.." = "..lvalue_split..tostring(v)..rvalue_split
            end
        end
        if flag then
            result = result..cur..",\n"
        end
    end

    split = string.sub(split, 1, -3)

    result = result..split.."}"

    return result
end

--获取Table字符串
function CPPGameLib.GetTableStr(tbl, tblName)
    tblName = tblName or "TABLE"

    local str = ""
    if tbl == nil then
        str = "nil"
    elseif tbl == false then
        str = "false"
    else
        --只有在将tbl放入table中，才能准确获取type(tbl)
        local tempTable = {}
        table.insert(tempTable, tbl)

        for _, v in pairs(tempTable) do
            if type(v) == "table" then
                str = CPPGameLib.ParseTable(tbl, "", true)
            else
                str = tostring(v)
            end
        end
    end
    return tblName.." = "..str
end

--打印Table字符串
function CPPGameLib.PrintTable(print_tag, tbl, tblName)
    tblName = tblName or "TABLE"

    local str = ""
    if tbl == nil then
        str = "nil"
    elseif tbl == false then
        str = "false"
    else
        --只有在将tbl放入table中，才能准确获取type(tbl)
        local tempTable = {}
        table.insert(tempTable, tbl)

        for _, v in pairs(tempTable) do
            if type(v) == "table" then
                str = CPPGameLib.ParseTable(tbl, "")
            else
                str = tostring(v)
            end
        end
    end

    print(print_tag, "****************PTINT TABLE***********************")
    print(print_tag, tblName .. " = " .. str)
    print(print_tag, "**************************************************")
end

--合并table为list
function CPPGameLib.MergeTable2List(...)
    local list = {}

    local org_tbl
    local tbl_num = select('#', ...)
    for i = 1, tbl_num do
        org_tbl = select(i, ...)
        if org_tbl then
            for _, v in pairs(org_tbl) do
                table.insert(list, v)
            end
        end
    end

    return list
end

--将map转为list
function CPPGameLib.ConvertMap2List(map, extract_key)
    if not map then
        return
    end

    local list = {}
    for k, v in pairs(map) do
        if extract_key then
            table.insert(list, k)            
        else
            table.insert(list, v)
        end
    end
    return list
end

-----------------------------------String相关-----------------------------------
--------------------------------------------------------------------------------

---是否以xxx结尾
function CPPGameLib.StrEndWith(str, substr)
	local str_len = string.len(str)
	local substr_len = string.len(substr)                                           

	if(str_len < substr_len) then
		return false
	end

	local target_str = string.sub(str, -substr_len)
	if(target_str ~= substr) then
		return false
	end

    return true
end

---替换结尾字符串
function CPPGameLib.StrEndReplaceWith(str, suffix_src, suffix_tgt)
    local str_len = string.len(str)
    local suffix_src_len = string.len(suffix_src)
    if str_len <= suffix_src_len then
        return nil, str
    end 

    local suffix_str = string.sub(str, -suffix_src_len)
    if suffix_str == suffix_src then
        --替换结尾
        local new_str = string.sub(str, 1, str_len - suffix_src_len)..suffix_tgt
        return true, new_str
    else
        return nil, str
    end    
end

---获取短名称
function CPPGameLib.GetShortName(name_str, max_len)
	local short_name = ""
	max_len = max_len or 10
	local str_len = tonumber(CPPStringConvHelper.unicode_length(name_str))
	for index = 1, str_len do
		local strcode = CPPStringConvHelper.unicode_at(name_str, index-1)
		if(CPPStringConvHelper.unicode_length(short_name) + CPPStringConvHelper.unicode_length(strcode) <= max_len) then
			short_name = short_name..strcode
		else
			short_name = short_name..".."
			break
		end
	end
	
	return short_name
end

---判断字符串是否全部为数字
function CPPGameLib.IsDigit(str)
	if not str then
        return false
    end
    local len = string.len(str)
    if len <= 0 then
    	return false
    end
    for i = 1, len do
	    local ch = string.sub(str, i, i)
	    if ch < '0' or ch > '9' then
	        return false
	    end
    end
    return true
end

--切割字符串
function CPPGameLib.Split(s, p)
	local rt = {}
	if (not s) or (not p) then
		return rt
	end
    string.gsub(s, '[^'..p..']+', function(w) table.insert(rt, w) end)
    return rt
end

-- 以某个分隔符为标准，分割字符串
-- @param split_string 需要分割的字符串
-- @param splitter 分隔符
-- @return 用分隔符分隔好的table
function CPPGameLib.SplitEx(split_string, splitter)
    local split_result = {}
    local search_pos_begin = 1
    local find_pos_begin, find_pos_end

    while true do
        find_pos_begin, find_pos_end = string.find(split_string, splitter, search_pos_begin)
        if not find_pos_begin then
            break
        end

        -- 处理以分隔符开头的情况
        if search_pos_begin <= find_pos_begin - 1 then
            split_result[#split_result + 1] = string.sub(split_string, search_pos_begin, find_pos_begin - 1)
        end
        search_pos_begin = find_pos_end + 1
    end

    -- 处理不以分隔符结束的情况
    if search_pos_begin <= string.len(split_string) then
        split_result[#split_result + 1] = string.sub(split_string, search_pos_begin)
    end

    return split_result
end

---是否包含非法字符
function CPPGameLib.CheckContainForbidWord(word, forbid_four_word, interrupt)
    local config_forbid_words = CPPGameLib.GetConfig("Forbid_Word", nil, nil, true)

	--其他语言屏蔽函数
	if not config_forbid_words then
		return word or ""
	end

	local checkedcode = {}
	local str_len = tonumber(CPPStringConvHelper.unicode_length(word))
	for index = 1, str_len do
	  local strcode = CPPStringConvHelper.unicode_at(word, index-1)
	  local forbidwords = {}
	  --检查3字节以上utf8
	  if(forbid_four_word and string.len(strcode) > 3) then
		if(interrupt) then
			return true
		else
			word = word:gsub(strcode,"*")
		end
	  end
	  
	  --检查屏蔽字
	  if not checkedcode[strcode] then
	    forbidwords = config_forbid_words[strcode] or {}
	    --保存已检查过key
	    checkedcode[strcode] = 1
	  end
	 
	  for _,forbidword in ipairs(forbidwords) do
		local s,e = string.find(word, forbidword)
		
		if(s and e) then
			if interrupt then
			   return true
			end
			
			local replace = ""
			
			local forbidword_len = CPPStringConvHelper.unicode_length(forbidword)
			for i = 1, forbidword_len do
			   replace = replace .. "*"
			end
			
			word = word:gsub(forbidword,replace)
		end
	  end
	
	end
	
	return word
end

--屏蔽表情符号
function CPPGameLib.CheckContainFaceWord(word)
	local word_temp = word
	word_temp = CPPFaceConvHelper:convertToFaceStr(word_temp)
	return (word_temp ~= word)
end

--检查角色非法字符
function CPPGameLib.CheckContainRoleWord(word)
	local checkedcode = {}
	local str_len = tonumber(CPPStringConvHelper.unicode_length(word))
	for index = 1, str_len do
		local strcode = CPPStringConvHelper.unicode_at(word, index-1)
	  	local forbidwords = {}
	  	--检查屏蔽字
	  	if not checkedcode[strcode] then
	    	forbidwords = Macros.ForbidRoleWord[strcode] or {}
	    	--保存已检查过key
	    	checkedcode[strcode] = 1
	 	 end
	 
	  	for _,forbidword in ipairs(forbidwords) do
			local s,e = string.find(word, forbidword)
			if(s and e) then
			  	return true
			end
	 	end
	end
	return false	
end 

---获取字符串
function CPPGameLib.GetString(index, ...)
    local str = GameConfig.StringMap[index]
	if str then
        local arg_num = select('#', ...)
        if arg_num > 0 then
            return string.format(str, ...)
        end
		return GameConfig.StringMap[index]
	else
		return "invalid"
	end
end

--获取字符串
function CPPGameLib.GetStartString(index)
    local config_string_map = StartStringMap[LanguageHelper.language]
    if not config_string_map then
        return "invalid"
    end
    return config_string_map[index] or "invalid"
end

-------------------------------------杂项--------------------------------------
-------------------------------------------------------------------------------

CPPGameLib.JSON = require("luatools/luajson")

Default, Nil = {},function () end -- for uniqueness
function CPPGameLib.Switch(i)
  return setmetatable({ i }, {
    __call = function (t, cases)
      local item = #t == 0 and Nil or t[1]
      return (cases[item] or cases[Default] or Nil)(item)
    end
  })
end

function CPPGameLib.SwitchEx(var, tbl, ...)
    if not tbl then
        return
    end

    tbl["default"] = tbl["default"] or function() end

    if not var or not tbl[var] then
        tbl["default"](unpack({...}))
        return
    end
    tbl[var](unpack({...}))
end

function CPPGameLib.SwitchFuncs(t)
    t.Case = function (self, x)
        return self[x] or (self.default or function() end)
    end
    return t
end

--[[
获取某个配置表的索引项内容，过程有进行错误判断并给出出错信息
@@config_name:配置表名字，如Goods, System
@@index:索引
@@show_error:假如业务逻辑中允许无效配置，则传入false
@@return:索引项内容
]]            
function CPPGameLib.GetConfig(config_name, index, show_error, all)
    if GameConfig[config_name] == nil then
        if not ConfigAdapter or not ConfigAdapter.Map or not ConfigAdapter.Map.MapPath[config_name] then
            if show_error ~= false then
                LogTool.LogError("[CPPGameLib.GetConfig] 找不到映射配置. config_name %s", config_name)
            end
            return 
        end

        GlobalRequireToolBatch:LoadFilesSync( 
        {
            file_list = { ConfigAdapter.Map.MapPath[config_name] }
        })

        if not GameConfig[config_name] then
            if show_error ~= false then
                LogTool.LogError("[CPPGameLib.GetConfig] 找不到对应配置. config_name %s", config_name)
            end
            return
        end
    end
    
    if all then
        --返回整个表
        return GameConfig[config_name]
    end

    if not index or type(index) ~= "number" then
        if show_error ~= false then
        	LogTool.LogError("[CPPGameLib.GetConfig] 参数index无效")
        end
        return nil
    end
    local config = GameConfig[config_name][index]
    if not config then
        if show_error ~= false then
        	LogTool.LogError("[CPPGameLib.GetConfig] 配置表：%s 找不到该索引：%d", config_name, index)
        end
    end

    CPPSdkTools.HandleSystemEntryConfig(config_name, config)

    return config
end

function CPPGameLib.Sleep(time)
    local tm = TimeTool.GetTime()
    while TimeTool.GetTime() - tm < time do
    end
end

function CPPGameLib.Handler(obj_self, func)
    return function(...)
        if obj_self and func then
            func(obj_self, ...)
        end
    end
end

function CPPGameLib.ClearObjList(list)
    if not list then
        return
    end

    for _, obj in pairs(list) do
        obj:DeleteMe()
    end
end

function CPPGameLib.UnbindEvent(event_handle)
    if event_handle then
        GlobalEventSystem:UnBind(event_handle)
    end
end

function CPPGameLib.DeleteObj(obj)
    if not obj then
        return
    end
    if type(obj) ~= "userdata" then
        obj:DeleteMe()
    else
        obj:RemoveSelf(true)
    end    
end

function CPPGameLib.ConvertToCCNode(node)
    if not node then
        return nil
    end

    if type(node) ~= "userdata" then
        if node.GetNode then
            node = node:GetNode()
        else
            LogTool.LogError("[CPPGameLib.ConvertToCCNode] 参数node无效")
        end
    end
    return node
end

--根据后缀名获取mime type
function CPPGameLib.GetMimeTypeByExt(path)
    local ext = Resource.PathTool.GetExtension(path)
    if ext == "jpg" or ext == "jpeg" then
        return "image/jpeg"

    elseif ext == "png" then
        return "image/png"
    end
end
