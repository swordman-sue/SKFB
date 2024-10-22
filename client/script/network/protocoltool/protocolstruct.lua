
--[[
%% @module: 协议编/解码器
%% @author: swordman sue
%% @created: 2016-07-26
--]]

Net = Net or {}

--------------------------------------------------------------------------------------------------------------------------------
--protobuf协议结构
--------------------------------------------------------------------------------------------------------------------------------
Net.ProtobufStruct = Net.ProtobufStruct or BaseClass()

--注册pb数据
function Net.ProtobufStruct.RegistPBData(protocol_no, pb_file_path)
	if not pb_file_path then
		pb_file_path = Net.ProtobufStruct.GetPBDataFilePath(protocol_no)
	end

	Net.ProtobufStruct.pb_data_registed_map = Net.ProtobufStruct.pb_data_registed_map or {}
	if not Net.ProtobufStruct.pb_data_registed_map[pb_file_path] then
		local file = io.open(pb_file_path, "rb")
		if not file then
			LogTool.LogError("[Net.ProtobufStruct.RegistPBData] 打开pb数据文件失败 protocol_no:%s pb_file_path:%s", protocol_no, pb_file_path)
			return
		end

		local buffer = file:read("*a")
		file:close()
		if not buffer then
			LogTool.LogError("[Net.ProtobufStruct.RegistPBData] 读取pb数据文件失败 protocol_no:%s pb_file_path:%s", protocol_no, pb_file_path)
			return
		end

		protobuf.register(buffer)
		Net.ProtobufStruct.pb_data_registed_map[pb_file_path] = true

		return buffer
	end
end

--获取PB数据文件路径
function Net.ProtobufStruct.GetPBDataFilePath(protocol_no)
	local bpos, epos = string.find(protocol_no, "%.")
	if not bpos or not epos then
		LogTool.LogError("[Net.protobufbufStruct.GetPBDataFilePath] 根据协议号无法获取pb数据文件名:%s", protocol_no)
		return 
	end	

	local pb_file_name = string.sub(protocol_no, 1, bpos - 1)
	return Resource.PathTool.GetProtobufDataFilePath(string.lower(pb_file_name))
end

function Net.ProtobufStruct:__init(protocol_no)
	self.protocol_no = protocol_no
	self.encode_data = 
	{ 
		--为了兼容采用非protobuf协议进行的网络交互
		pb_struct = self
	}
	self.__pb_file_path = Net.ProtobufStruct.GetPBDataFilePath(self.protocol_no)
end

function Net.ProtobufStruct:Encode(msg_adapter)
	--注册pb数据
	Net.ProtobufStruct.RegistPBData(self.protocol_no, self.__pb_file_path)

	msg_adapter.WriteBegin()

	--消息头编码
	local header_data = { msgtype = 1, msgname = self.protocol_no, msgret = 0}
	local ret = protobuf.encode("PbHead.MsgHead", header_data, 
		function(buffer, len)
			--写入消息头数据
			if not msg_adapter.WriteHeader(self.protocol_no, buffer, len) then
				LogTool.LogError("[Net.ProtobufStruct:Encode] 写入消息头数据失败 protocol_no:%s", self.protocol_no)
				return
			end			
			return true
		end)
	if not ret then
		return
	end

	--消息体编码
	ret = true
	if next(self.encode_data) then
		self.encode_data.pb_struct = nil
		ret = protobuf.encode(self.protocol_no, self.encode_data, 
			function(buffer, len)
				--写入消息体数据
				if not msg_adapter.WriteBytes(buffer, len) then
					LogTool.LogError("[Net.ProtobufStruct:Encode] 写入消息体数据失败 protocol_no:%s", self.protocol_no)
					return
				end		
				return true	
			end)
		self.encode_data.pb_struct = self
	end
	return ret	
end

function Net.ProtobufStruct:Decode(msg_adapter)	
	local msg_type, _, msg_ret, len = msg_adapter.ReadHeader()

	if len < 0 then
		LogTool.LogError("[Net.ProtobufStruct:Decode] len less than zero")
		return
	end
	if len == 0 then
		return true
	end

	local buffer = msg_adapter.ReadBytes(len)
	if not buffer then
		LogTool.LogError("[Net.ProtobufStruct:Decode] invalid buffer")
		return
	end

	--注册pb数据
	Net.ProtobufStruct.RegistPBData(self.protocol_no, self.__pb_file_path)

	--消息体解码
	local decode_data = protobuf.decode(self.protocol_no , buffer, len)
	decode_data.protocol_no = self.protocol_no
	return true, decode_data
end

--------------------------------------------------------------------------------------------------------------------------------
--协议结构
--------------------------------------------------------------------------------------------------------------------------------
Net.BaseProtocolStruct = Net.BaseProtocolStruct or BaseClass()

--内部类
local _AutoConstructTool
local _DefaultAnalyze
local _DefinedAnalyze
local _TableAnalyze
local _TableDefaultAnalyze
local _TableDefinedAnalyze

function Net.BaseProtocolStruct:__init(protocol_no)
	self.protocol_no = protocol_no
end

function Net.BaseProtocolStruct:AutoConstruct(analyze_rules)
	self.__orig_analyze_rules = analyze_rules

	local analyze_rules_funcs = {
		["D"] = _DefinedAnalyze,
		["T"] = _TableAnalyze,
		["default"] = _DefaultAnalyze,
	}
	local construct_tool = _AutoConstructTool.New(self.__orig_analyze_rules, analyze_rules_funcs, "__", self)
	self.__format_values = construct_tool:Handle()
end

function Net.BaseProtocolStruct:Encode(msg_adapter)
	msg_adapter.WriteBegin()
	if not msg_adapter.WriteHeader(self.protocol_no) then
		return
	end

	for _, f in ipairs(self.__format_values) do
		if not f:Encode(msg_adapter) then
			return
		end
	end

	return true
end

function Net.BaseProtocolStruct:Decode(msg_adapter)
	self.decode_data = {}
	self.decode_data.protocol_no = self.protocol_no

	local ret = true
	if not self.__format_values then
		ret = false
	else
		for _, f in ipairs(self.__format_values) do
			if not f:Decode(msg_adapter) then
				ret = false
				break
			end
		end
	end

	local protocol_data = self.decode_data
	self.decode_data = nil
	return ret, protocol_data
end

--------------------------------------------------------------------------------------------------------------------------------
--自动编/解码工具
--------------------------------------------------------------------------------------------------------------------------------
_AutoConstructTool = _AutoConstructTool or BaseClass()

function _AutoConstructTool:__init(analyze_rules, analyze_rules_funcs, splitter, value_src)
	self.__analyze_rules = analyze_rules
	self.__analyze_rules_funcs = analyze_rules_funcs
	self.__splitter = splitter
	self.__value_src = value_src
end

function _AutoConstructTool:Handle()
	self.__default_fmt = ""
	self.__default_value_names = {}
	self.__format_values = {}

	for _, d in ipairs(self.__analyze_rules) do
		local param_name, param_type, param_fields = unpack(CPPGameLib.SplitEx(d, self.__splitter))
		local label = param_type:sub(1, 1)
		CPPGameLib.SwitchEx(label, 
		{
			["D"] = function()
				self:_HandleDefinedParam(param_name, param_type, param_fields)
			end,
			["T"] = function()
				self:_HandleTableParam(param_name, param_type, param_fields)
			end,
			["default"] = function()
				self:_HandleDefaultParam(param_name, param_type, param_fields)
			end,
		})
	end

	self:_ClearDefaultValues()

	return self.__format_values
end

function _AutoConstructTool:_ClearDefaultValues()
	if self.__default_fmt ~= "" then
		local analyze_obj = self.__analyze_rules_funcs["default"].New(self.__default_fmt, self.__default_value_names, self.__value_src)
		table.insert(self.__format_values, analyze_obj)

		self.__default_fmt = ""
		self.__default_value_names = {}
	end
end

function _AutoConstructTool:_HandleDefinedParam(param_name, param_type, param_fields)
	self:_ClearDefaultValues()

	local _, defined_name = unpack(CPPGameLib.SplitEx(param_type, "|"))
	local analyze_obj = self.__analyze_rules_funcs["D"].New(defined_name, param_name, self.__value_src)
	table.insert(self.__format_values, analyze_obj)
end

function _AutoConstructTool:_HandleTableParam(param_name, param_type, param_fields)
	self:_ClearDefaultValues()

	local analyze_obj = self.__analyze_rules_funcs["T"].New(param_name, param_fields, self.__value_src)
	table.insert(self.__format_values, analyze_obj)
end

function _AutoConstructTool:_HandleDefaultParam(param_name, param_type, param_fields)
	self.__default_fmt = self.__default_fmt .. param_type
	table.insert(self.__default_value_names, param_name)
end

--------------------------------------------------------------------------------------------------------------------------------
--默认类型解析
--------------------------------------------------------------------------------------------------------------------------------
_DefaultAnalyze = _DefaultAnalyze or BaseClass()

function _DefaultAnalyze:__init(format, value_names, value_src)
	self.__format = format
	self.__value_names = value_names
	self.__value_src = value_src
	self.__values = {}
end

function _DefaultAnalyze:Encode(msg_adapter)
	self.__values = {}

	for _, n in ipairs(self.__value_names) do
		table.insert(self.__values, self.__value_src[n])
	end

	return msg_adapter.WriteFMT(self.__format, unpack(self.__values))
end

function _DefaultAnalyze:Decode(msg_adapter)
	self.__values = {msg_adapter.ReadFMT(self.__format)}

	for i, n in ipairs(self.__value_names) do
		self.__value_src.decode_data[n] = self.__values[i]
	end

	return msg_adapter.ReadResult()
end

--------------------------------------------------------------------------------------------------------------------------------
--自定义类型解析
--------------------------------------------------------------------------------------------------------------------------------
_DefinedAnalyze = _DefinedAnalyze or BaseClass()

function _DefinedAnalyze:__init(defined_name, value_name, value_src)
	self.__defined_name = defined_name
	self.__value_name = value_name
	self.__value_src = value_src

	self.__defined_class = Net.CSStruct[self.__defined_name]
    if not self.__defined_class then
        LogTool.LogError("[_DefinedAnalyze:__init] 找不到对应的协议自定义类(class_name:%s)", self.__defined_name)
    end
end

function _DefinedAnalyze:Encode(msg_adapter)
	if self.__defined_class then
		return self.__value_src[self.__value_name]:Encode(msg_adapter)
	end
	return false
end

function _DefinedAnalyze:Decode(msg_adapter)
	if self.__defined_class then
		local defined_obj = self.__defined_class.New()
		self.__value_src.decode_data[self.__value_name] = defined_obj
		return defined_obj:Decode(msg_adapter)
	end
	return false
end

--------------------------------------------------------------------------------------------------------------------------------
--Table默认类型解析
--------------------------------------------------------------------------------------------------------------------------------
_TableDefaultAnalyze = _TableDefaultAnalyze or BaseClass(_DefaultAnalyze)

function _TableDefaultAnalyze:Encode(msg_adapter, table_name, table_index)
	self.__values = {}

	for _, n in ipairs(self.__value_names) do
		table.insert(self.__values, self.__value_src[table_name][table_index][n])
	end

	return msg_adapter.WriteFMT(self.__format, unpack(self.__values))
end

function _TableDefaultAnalyze:Decode(msg_adapter, table_name, table_index)
	self.__values = {msg_adapter.ReadFMT(self.__format)}

	for i, n in ipairs(self.__value_names) do
		self.__value_src.decode_data[table_name][table_index][n] = self.__values[i]
	end

	return msg_adapter.ReadResult()
end

--------------------------------------------------------------------------------------------------------------------------------
--Table自定义类型解析
--------------------------------------------------------------------------------------------------------------------------------
_TableDefinedAnalyze = _TableDefinedAnalyze or BaseClass(_DefinedAnalyze)

function _TableDefinedAnalyze:Encode(msg_adapter, table_name, table_index)
	if self.__defined_class then
		return self.__value_src[table_name][table_index][self.__value_name]:Encode(msg_adapter)
	end
	return false
end

function _TableDefinedAnalyze:Decode(msg_adapter, table_name, table_index)
	if self.__defined_class then
		local defined_obj = self.__defined_class.New()
		self.__value_src.decode_data[table_name][table_index][self.__value_name] = defined_obj
		return defined_obj:Decode(msg_adapter)
	end
	return false
end

--------------------------------------------------------------------------------------------------------------------------------
--Table解析
--------------------------------------------------------------------------------------------------------------------------------
_TableAnalyze = _TableAnalyze or BaseClass()

function _TableAnalyze:__init(value_name, value_fields, value_src)
	self.__value_name = value_name
	self.__analyze_rules = CPPGameLib.SplitEx(value_fields, "##")
	self.__value_src = value_src

	local analyze_rules_funcs = {
		["D"] = _TableDefinedAnalyze,
		["default"] = _TableDefaultAnalyze,
	}
	local construct_tool = _AutoConstructTool.New(self.__analyze_rules, analyze_rules_funcs, "@", self.__value_src)
	self.__format_values = construct_tool:Handle()
end

function _TableAnalyze:Encode(msg_adapter)
	local table_len = #self.__value_src[self.__value_name]
	msg_adapter.WriteUShort(table_len)

	for i = 1, table_len do
		for _, f in ipairs(self.__format_values) do
			if not f:Encode(msg_adapter, self.__value_name, i) then
				return false
			end
		end
	end

	return true
end

function _TableAnalyze:Decode(msg_adapter)
	self.__value_src.decode_data[self.__value_name] = {}

	local table_len = msg_adapter.ReadUShort()	
	for i = 1, table_len do
		self.__value_src.decode_data[self.__value_name][i] = {}
		for _, f in ipairs(self.__format_values) do
			if not f:Decode(msg_adapter, self.__value_name, i) then
				return false
			end
		end
	end

	return true
end
