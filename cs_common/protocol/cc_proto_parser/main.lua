
parser = require "parser"
require "config"

--解析Table
local function ParseTable(tbl, split, extend)
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
                cur = cur.."["..k.."] = "..ParseTable(v, split, extend)
            elseif type(v) == "function" or type(v) == "userdata" then
                flag = false
            else
                cur = cur.."["..k.."] = "..lvalue_split..tostring(v)..rvalue_split
            end            

        elseif type(k) == "table" then
            if type(v) == "table" then
                cur = cur.."["..ParseTable(k, split, extend).."] = "..ParseTable(v, split, extend)
            elseif type(v) == "function" or type(v) == "userdata" then
                flag = false
            else
                cur = cur.."["..ParseTable(k, split, extend).."] = "..lvalue_split..tostring(v)..rvalue_split
            end                        

        elseif k == "__class_type" or k == "DeleteMe" then
            flag = false
        else
            local lkey_split = extend and "[\"" or ""
            local rkey_split = extend and "\"]" or ""
            if type(v) == "table" then
                cur = cur..lkey_split..k..rkey_split.." = "..ParseTable(v, split, extend)
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
local function GetTableStr(tbl, tblName)
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
                str = ParseTable(tbl, "", true)
            else
                str = tostring(v)
            end
        end
    end
    return tblName.." = "..str
end

--打印Table字符串
local function PrintTable(tbl, tblName)
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
                str = ParseTable(tbl, "")
            else
                str = tostring(v)
            end
        end
    end

    print("****************PTINT TABLE***********************")
    print(tblName .. " = " .. str)
    print("**************************************************")
end

--生成枚举定义文件
function GenerateEnumDefineFile()
    local all, data_map = parser.register("cs_enum_define.proto", "../cs_proto")
    if data_map then
        local file = io.open("../../../client/script/macros/game_cs.lua", "w+")
        if file then        
            local code_str = "\nMacros = Macros or {}\n\nMacros.GameCS = {\n"
            local enum_type_list = data_map[1].enum_type
            for _, enum_info in ipairs(enum_type_list) do
                code_str = string.format("%s\t%s = {\n", code_str, enum_info.name)
                for _, value_info in ipairs(enum_info.value) do
                    code_str = string.format("%s\t\t%s = %d,\n", code_str, value_info.name, value_info.number)
                end
                code_str = code_str .. "\t},\n"
            end
            code_str = code_str .. "}"

            file:write(code_str)
            file:close()
    
            print("\ngenerate enum define file success")
            return
        end
    end
    print("\ngenerate enum define file failed") 
end

--生成协议号定义文件
function GenerateProtoNoDefineFile()
    local all, data_map = parser.register(PROTO_FILE_LIST, "../cs_proto")
    if data_map then
        local file = io.open("../../../client/script/network/protocols/protocols_no_pb.lua", "w+")
        if file then        
            local code_str = "\nNet = Net or {}\n\nNet.ProtocolNo = Net.ProtocolNo or {\n"
            local package, message_type_list
            local proto_list_len = #PROTO_FILE_LIST
            for i = 1, proto_list_len do
                package = data_map[i].package
                message_type_list = data_map[i].message_type
                for _, message_info in ipairs(message_type_list) do
                    code_str = string.format("%s\t%s_%s = \"%s.%s\",\n", code_str, package, message_info.name, package, message_info.name)          
                end
                if i < proto_list_len then
                    code_str = code_str .. "\n"
                end
            end
            code_str = code_str .. "}"      

            file:write(code_str)
            file:close()

            print("\ngenerate proto number define file success")
            return
        end
    end
    print("\ngenerate proto number define file failed")
end

function Generate(mode)
    if mode == 1 then
        GenerateEnumDefineFile()

    elseif mode == 2 then
        GenerateProtoNoDefineFile()
    
    else
        print(mode)
        print("\ninvalid generation mode, please input 1: generate enum define file. 2: generate proto number define file.")
    end
end