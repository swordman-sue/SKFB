
StringTool = StringTool or {}

--[[
    UTF-8首字节：
        [0, 128)    单字节编码ASCII
        [128, 192)  非UTF-8编码
        [192, 224)  双字节编码
        [224, 240)  三字节编码
        [240, 248)  四字节编码
        [248, 252)  五字节编码
        [252, 254)  六字节编码
]]
--[[
功能：字符串计数（ASCII按1算，其余多字节编码按2算）
@@return: 1.字节数 2.字符数I(数字、英文、中文都视作1个) 3.字符数II(数字、英文视作1个，中文视作2个)
@@param1: 字符串
]]
function StringTool.CountUTF8String(s)
    if type(s) ~= "string" then
        return
    end
    local i = 1
    local len = string.len(s)
    local single_count = 0
    local multi_count = 0
    while i <= len do
        local byte = string.byte(s, i)
        if byte < 128 then
            single_count = single_count + 1
            i = i + 1
        elseif byte < 192 then
            error("[StringTool.CountUTF8String] error first-byte in [128, 192) ")
        elseif byte < 224 then
            multi_count = multi_count + 1
            i = i + 2
        elseif byte < 240 then
            multi_count = multi_count + 1
            i = i + 3
        elseif byte < 248 then
            multi_count = multi_count + 1
            i = i + 4
        elseif byte < 252 then
            multi_count = multi_count + 1
            i = i + 5
        elseif byte < 254 then
            multi_count = multi_count + 1
            i = i + 6
        else
            error("[StringTool.CountUTF8String] unknown code range[254, 255) ")
        end
    end
    return i - 1, single_count + multi_count, single_count + multi_count * 2
end

--[[
    UTF-8首字节：
        [0, 128)    单字节编码ASCII
        [128, 192)  非UTF-8编码
        [192, 224)  双字节编码
        [224, 240)  三字节编码
        [240, 248)  四字节编码
        [248, 252)  五字节编码
        [252, 254)  六字节编码
]]
--[[
功能：正确截取字符串，防止将多byte字符只截取掉一部分。起始位置为1
@@param1: 字符串
@@param2: 终点位置
]]
function StringTool.TruncateUTF8String(s, n)
    local dropping = string.byte(s, n + 1)
    if not dropping then
        return s
    end
    if dropping >= 128 and dropping < 192 then
        return StringTool.TruncateUTF8String(s, n - 1)
    end
    return string.sub(s, 1, n)
end

function StringTool.TruncateUTF8StringByWordCount(s, count)
    if type(s) ~= "string" or count < 0 then
        return
    end

    local len = #s
    local multi_count = 0

    local i = 1
    while i <= len do
        local byte = string.byte(s, i)
        if byte < 128 then
            multi_count = multi_count + 1
            i = i + 1
        elseif byte < 192 then
            error("[StringTool.CountUTF8String] error first-byte in [128, 192) ")
        elseif byte < 224 then
            multi_count = multi_count + 1
            i = i + 2
        elseif byte < 240 then
            multi_count = multi_count + 1
            i = i + 3
        elseif byte < 248 then
            multi_count = multi_count + 1
            i = i + 4
        elseif byte < 252 then
            multi_count = multi_count + 1
            i = i + 5
        elseif byte < 254 then
            multi_count = multi_count + 1
            i = i + 6
        else
            error("[StringTool.CountUTF8String] unknown code range[254, 255) ")
        end

        count = count - 1
        if count <= 0 then
            break
        end
    end
    return string.sub(s, 1, i - 1)
end

function StringTool.Number( text )
    local n1 , n2 , n3 = StringTool.CountUTF8String( text )

    local num = 0
    local letter = 0
    local slip = 0

    for i = 1 , n2 do
        local t = string.sub( text , i , i+1 )
        local text = string.sub( t , 0 , 1 )

        if string.find(text, "%a") then
            letter = letter + 1
        elseif string.find(text, "%d") then
            num = num + 1
        else
            slip = slip + 1
        end
    end

    return num , letter , slip
end
