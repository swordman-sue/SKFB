
GUI = GUI or {}

-- 图片字体
-- 注: 暂时只支持单行字体图片
GUI.ImgFont = GUI.ImgFont or BaseClass(UILayer)

GUI.ImgFont.ImgType =
{
	Num1 = 1,		-- 数字类型 0123456789
	Num2 = 2,		-- 数字类型 0123456789+-.X
	Num3 = 3,		-- 数字类型 0123456789+-
	Num4 = 4,		-- 数字类型 0123456789:
}

--[[
注：已有类型不能随意修改！
	若需修改，请修改所有关联的地方
--]]
GUI.ImgFont.ImgFontMap =
{
	[GUI.ImgFont.ImgType.Num1] = "0123456789",
	[GUI.ImgFont.ImgType.Num2] = "0123456789+-.X",
	[GUI.ImgFont.ImgType.Num3] = "0123456789+-",
	[GUI.ImgFont.ImgType.Num4] = "0123456789:",
}

-- 参数
-- img - 图片路径,
-- img_type - 图片类型(解释时用到), 见枚举 GUI.ImgFont.ImgFontMap
function GUI.ImgFont:__init(image_data, img_type)
	self.image_data = image_data
	self.img_type = img_type or GUI.ImgFont.ImgType.Num1
	self.str = ""
	self.text = ""	
	self.span = 0

	local base_str_table = GUI.ImgFont.ImgFontMap[self.img_type]
	if base_str_table == nil then
		LogTool.LogError("[GUI.ImgFont:__init] 找不到对应的图字类型(img_type:%d)", self.img_type)
		return
	end

	self.img_table = {}
	self.label_list = {}
	self.unit_width = self.image_data.width / (string.len(base_str_table))
	self.unit_height = self.image_data.height
	for i=1,string.len(base_str_table) do
		local img_data = CPPImageData()
		img_data.image_path = self.image_data.image_path
		img_data.width = self.unit_width
		img_data.height = self.unit_height
		img_data.x = self.image_data.x + (i-1) * self.unit_width
		img_data.y = self.image_data.y
		self.img_table[string.byte(base_str_table, i)] = img_data
	end
end

-- 设置间距
function GUI.ImgFont:SetSpan(span)
	self.span = span
end

-- 设置颜色
function GUI.ImgFont:SetColor(color_r, color_g, color_b, front, back)
	for i, label in ipairs(self.label_list) do
		if (not front or not back) or (i >= front and i <= back) then
			label:SetColor(color_r, color_g, color_b)
		end
	end
end

-- 设置灰化
function GUI.ImgFont:SetGray(grey)
	for i, label in ipairs(self.label_list) do
		CPPNodeUnity.SetGrayShader(label, grey)
	end
end

--设置前缀
function GUI.ImgFont:SetPrefixEnabled(var, prefix_image_data)
	self.prefix_enabled = var
	self.prefix_image_data = prefix_image_data	
end

--设置后缀
function GUI.ImgFont:SetSuffixEnabled(var, suffix_image_data)
	self.suffix_enabled = var
	self.suffix_image_data = suffix_image_data
end

-- 设置显示文字
function GUI.ImgFont:SetText(txt)
	if self.text == txt then
		return
	end
	self.text = txt

	self:ParseText()
end

function GUI.ImgFont:GetText()
	return self.text
end

function GUI.ImgFont:ParseText()
	self.label_list = {}
	self:RemoveAllChild(true)
	self:SetContentSize(0, 0)

	if not self.text then
		LogTool.LogError("[GUI.ImgFont:ParseText] 参数str为空")
		return
	elseif type(self.text) ~= "string" then
		LogTool.LogError("[GUI.ImgFont:ParseText] 参数str不是字符串")
		return
	end

	local char
	local char_num = string.len(self.text)
	local image_data, pos_x, label_prefix, label_suffix
	local prefix_space = 0
	local suffix_space = 0
	local total_span = self.unit_width + self.span

	--有前缀
	if self.prefix_enabled then
		label_prefix = Util:Sprite(self.prefix_image_data)
		label_prefix:SetAnchorPoint(0, 0.5)
		prefix_space = label_prefix:GetContentWidth() + self.span
	end

	--有后缀
	if self.suffix_enabled then
		label_suffix = Util:Sprite(self.suffix_image_data)
		label_suffix:SetAnchorPoint(0, 0.5)
		suffix_space = label_suffix:GetContentWidth() + self.span
	end

	self:SetContentSize(char_num * total_span - self.span + prefix_space + suffix_space, self.unit_height)

	--阿拉伯数字
	local sp
	for i = 1, char_num do
		char = string.byte(self.text, i)
		image_data = self.img_table[char]
		if image_data ~= nil then
			pos_x = prefix_space + (i-1) * total_span
			sp = Util:Sprite(image_data)
			sp:SetAnchorPoint(0, 0.5)
			self:AddChild(sp)
			PosTool.LeftCenter(sp, pos_x)
			table.insert(self.label_list, sp)
		end
	end

	--前缀
	if self.prefix_enabled then
		self:AddChild(label_prefix)		
		PosTool.LeftCenter(label_prefix)		
		table.insert(self.label_list, 1, label_prefix)		
	end

	--后缀
	if self.suffix_enabled then
		pos_x = pos_x + total_span
		self:AddChild(label_suffix)		
		PosTool.LeftCenter(label_suffix, pos_x)		
		table.insert(self.label_list, label_suffix)
	end
end
