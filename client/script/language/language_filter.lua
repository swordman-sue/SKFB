
LanguageFilter = LanguageFilter or {}

LanguageFilter.WhiteSpaceSymbol = LanguageFilter.WhiteSpaceSymbol or {
	" ", " ", "	", "	", "\n", "\r", "\t"
}

LanguageFilter.NameFilterSymbol = LanguageFilter.NameFilterSymbol or {
	",", "，", "%.", "。", "%(", 
	"%)", "%+", "%-", "%?", "%[", 
	"%]", "%^", "%&", "%$", "【", 	
	"】", "（", "）", "#", "@", 
	"!", "-", "_", "——", "=", 
	"\"", "\'", "“", "”", "‘",
	"’", ":", "：", "<", ">", 
	"《", "》", ";", "；", "{", 
	"}", "%%", "~", "`", "·", 
	"/", "、", "\\", "|", "？",
	" ", " ", "	", "	", "/", 
	"\\", "\n", "\r", "\t",
}

LanguageFilter.ChatFilterSymbol = LanguageFilter.ChatFilterSymbol or {
	"br", "\n", "\r"
}

function LanguageFilter.ConvPartten(partten_str)
	partten_str = string.gsub(partten_str, "%.", "%%.")
	partten_str = string.gsub(partten_str, "%(", "%%(")
	partten_str = string.gsub(partten_str, "%)", "%%)")
	partten_str = string.gsub(partten_str, "%+", "%%+")
	partten_str = string.gsub(partten_str, "%-", "%%-")
	partten_str = string.gsub(partten_str, "%?", "%%?")
	partten_str = string.gsub(partten_str, "%[", "%%[")
	partten_str = string.gsub(partten_str, "%]", "%%]")
	partten_str = string.gsub(partten_str, "%^", "%%^")
	partten_str = string.gsub(partten_str, "%&", "%%&")
	partten_str = string.gsub(partten_str, "%$", "%%$")
	return partten_str
end

function LanguageFilter.InitMaskData()
	if LanguageFilter.InitMaskDataFinished then
		return
	end
	LanguageFilter.InitMaskDataFinished = true

	LanguageFilter.Mask = {}

	for index, partten in pairs( CPPGameLib.GetConfig("Shield", nil, nil , true ) ) do
		local conv_partten = LanguageFilter.ConvPartten(partten.content)
		LanguageFilter.Mask[index] = conv_partten
	end
end

function LanguageFilter.FindAll(message, partten, result_table)
	if not message or not partten or not result_table or string.len(message) == 0 then
		return
	end

	local partten_table = type(partten) == "table" and partten or {partten}
	for _, real_partten in pairs(partten_table) do
		local cur_start_index = 0
		local cur_end_index = 0
		while true do
			cur_start_index, cur_end_index = string.find(message, real_partten, cur_end_index + 1)
			if not cur_start_index or not cur_end_index then
				break
			end
			table.insert(result_table, {start_index = cur_start_index, end_index = cur_end_index})
		end
	end
end

function LanguageFilter.ReplaceAll(message, partten, dest)
	local index_table = {}

	LanguageFilter.FindAll(message, partten, index_table)

	local message_start, message_end
	for _, index_pair in pairs(index_table) do
		message_start = index_pair.start_index == 0 and "" or string.sub(message, 1, index_pair.start_index - 1)
		message_end = string.sub(message, index_pair.end_index + 1)
		message = message_start .. string.rep("\*" , index_pair.end_index - index_pair.start_index + 1) .. message_end
	end	

    if #index_table ~= 0 then
        message = string.gsub(message, "%*+", dest)
    end	

    return message
end

function LanguageFilter.FilterMsg(message, is_only_check)
	local index_table = {}

	LanguageFilter.FindAll(message, LanguageFilter.Mask, index_table)

	if not is_only_check then
		local message_start, message_end
		for _, index_pair in pairs(index_table) do
			message_start = index_pair.start_index == 0 and "" or string.sub(message, 1, index_pair.start_index - 1)
			message_end = string.sub(message, index_pair.end_index + 1)
			message = message_start .. string.rep("\*" , index_pair.end_index - index_pair.start_index + 1) .. message_end
		end

	    if #index_table ~= 0 then
	        message = string.gsub(message, "%*+", "**")
	    end
	end

	return message, #index_table == 0
end

--聊天过滤
function LanguageFilter.ChatFilter(message)
	local is_pass = false

	if not message then
		return nil, is_pass
	end

	local message_no_white_space = LanguageFilter.ReplaceAll(message, LanguageFilter.WhiteSpaceSymbol, "")
	if string.len(message_no_white_space) == 0 then
		return nil, is_pass
	end	

	message = LanguageFilter.ReplaceAll(message, LanguageFilter.ChatFilterSymbol, "")

	local now_message, is_pass = LanguageFilter.FilterMsg(message)

	return now_message, is_pass
end

--名字过滤
function LanguageFilter.NameFilter(message)
	local is_pass = false

	if not message or string.len(message) == 0 then
		return nil, is_pass
	end

	message = LanguageFilter.ReplaceAll(message, LanguageFilter.NameFilterSymbol, "")
	if string.len(message) == 0 then
		return nil, is_pass
	end

    local _, is_pass = LanguageFilter.FilterMsg(message, true)
	return message, is_pass
end


