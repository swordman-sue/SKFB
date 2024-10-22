--
-- @brief 控件
-- @author: yjg
-- @date: 2016年8月9日17:23:47
--

Util = Util or {}

Util.FontType = StartConfig.DefaultFontName or CPPResManager:GetSetting(Macros.Setting.DefaultFontName)

Util.BackGroundColorType = {
	NONE = 0,
	SOLID = 1,
	GRADIENT = 2,
}

--是否UITextEx
function Util:IsText(widget)
	if not widget then 
		return false
	end
	return widget:getDescription() == "UITextEx" or widget:getDescription() == "CPPRichText"
end

--是否UIWidget或者继承自UIWidget
function Util:IsWidgetOrInheritWidget(widget)
	if not widget then 
		return false
	end

	local desc = widget:getDescription()
	return desc == "Widget" or desc == "UILayoutEx" or 
	desc == "UIScrollViewEx" or desc == "UIListViewEx" or 
	desc == "UIPageViewEx" or desc == "UIButtonEx" or 
	desc == "UITextEx" or desc == "UIImageViewEx" or 
	desc == "UILoadingBarEx" or desc == "UITextFieldEx" or
	desc == "UICheckBoxEx" or desc == "UITextAtlasEx" or 
	desc == "UISliderEx"
end

--获取Widget缩放后的尺寸
function Util:GetWidgetSizeWithScale(widget)
	if not widget then
		return COCOSize()
	end

	local widget_size = widget:GetContentSize()
	widget_size.width = widget_size.width * widget:GetScaleFactor()
	widget_size.height = widget_size.height * widget:GetScaleFactor()	
	return widget_size
end

--创建Layout
function Util:Layout(width, height, fun)
	local layout = UILayoutEx.Create()
	layout:SetContentSize(width,height)
	layout:SetCascadeOpacityEnabled(true)
	if fun then
		layout:SetTouchEnabled(true)
		WidgetUnity.SetWidgetListener(layout, nil, nil, fun)
	end
	return layout
end

--设置Layout纯色背景
function Util:SetLayoutBGColor(layout, color, opacity)
	if not layout or not color then
		return
	end

	opacity = opacity or 255

	layout:SetBackGroundColorType(1)
	layout:SetBackGroundColor(unpack(color))
	layout:SetBackGroundColorOpacity(opacity)
end

--创建Label
function Util:Label(lab, font_size, color, width, height, stroke_color, stroke_width)
	lab = (not lab or lab == "") and " " or lab

	font_size = font_size or Macros.TypefaceSize.normal
	font_size = StartConfig.FontSizeScaleFactor and StartConfig.FontSizeScaleFactor * font_size or font_size

	color = color or Macros.Color.white

	width = width or 0

	height = height or 0
	
	local labex = LabelEx.CreateWithString(lab, Util.FontType, font_size)
	labex:SetColor(unpack(color))
	labex:SetTextWidthAndHeight(width,height)
	labex:SetTextHorizontalAlign(LabelEx.ALIGIN_LEFT)
	labex:SetLineSpacing(3)
	labex:SetCascadeOpacityEnabled(true)	

	if stroke_color then
		stroke_width = stroke_width or 2
		labex:SetStrokeColor(unpack(stroke_color))
		labex:SetStrokeWidth(stroke_width)
	end
	return labex
end

--设置Label的默认属性
function Util:WidgetLabel(node, lab, font_size, labcolor, width, height, stroke_color, shadow_color)
	lab = (not lab or lab == "") and "" or lab

	font_size = font_size or Macros.TypefaceSize.normal
	font_size = StartConfig.FontSizeScaleFactor and StartConfig.FontSizeScaleFactor * font_size or font_size

	labcolor = labcolor or Macros.Color.white

	width = width or 0

	height = height or 0

	node:SetText(lab)
	node:SetFontName(Util.FontType)
	node:SetFontSize(font_size)
	node:SetColor(unpack(labcolor))
	node:SetTextAreaSize(width, height)

	if stroke_color then
		node:SetStrokeColor(unpack(stroke_color))
	end

	return node
end

--创建Button
function Util:Button(img, fun, lab, font_size, labcolor, stroke_color, stroke_width , time)
	lab = lab or ""
	
	font_size = font_size or Macros.TypefaceSize.normal

	labcolor = labcolor or Macros.Color.blinkcontent

	time = time or 0.5

	local btn = type(img) == "userdata" and UIButtonEx.CreateWithImage(img) or UIButtonEx.CreateWithImage(img, TextureResType.LOCAL)
	if not btn then
		local error_msg = img
		if type(img) == "userdata" then
			error_msg = string.format("img_path:%s img_name:%s", img.image_path, img.clip_image_name)
		end
		LogTool.LogError("[Util:Button] 创建ButtonEx失败 %s", error_msg)		
		return
	end
	btn:SetText(lab)
	btn:SetFontName(Util.FontType)
	btn:SetFontSize(font_size)
	btn:SetTextColor(unpack(labcolor))
	btn:SetCascadeOpacityEnabled(true)
	btn:SetTouchSoundInfo(Macros.Game.SystemIDForSound.COMMON, 1)

	--按钮默认阴影
	-- btn:SetShadow(0, 0, 0, 255, 2, -2, 0)

	--按钮文字描边
	if stroke_color then
		local r, g, b, a = unpack( stroke_color )
		stroke_width = stroke_width or 2
		btn:SetOutline(r, g, b, a, stroke_width)
	end

	WidgetUnity.SetWidgetListener(btn, nil, nil, fun, nil, time*1000)
	return btn
end

--设置Button的默认属性
function Util:WidgetButton(btn, lab, fun, font_size, labcolor, stroke_color, stroke_width, time)
	lab = lab or ""

	font_size = font_size or Macros.TypefaceSize.button

	labcolor = labcolor or Macros.Color.blinkcontent

	time = time or 0.5

	btn:SetText(lab)
	btn:SetFontName(Util.FontType)
	btn:SetFontSize(font_size)
	btn:SetTextColor(unpack(labcolor))
	btn:SetTouchSoundInfo(Macros.Game.SystemIDForSound.COMMON, 1)

	-- --按钮默认阴影
	-- btn:SetShadow(0, 0, 0, 255, 2, -2, 0)

	--按钮文字描边
	if stroke_color then		
		local r, g, b, a = unpack(stroke_color)
		stroke_width = stroke_width or 2
		btn:SetOutline(r, g, b, a, stroke_width)
	end	
	WidgetUnity.SetWidgetListener(btn, nil, nil, function()
		if fun then
			fun()
		end
	end, nil, time * 1000)

	return btn
end

--置灰按钮
function Util:GreyButton( btn )
	btn:SetEnabled( false )
	btn:SetTextColor(unpack(Macros.Color.forbidden))
	local r, g, b, a = unpack(Macros.Color.forbidden_stroke)
	local stroke_width = 2
	btn:SetOutline(r, g, b, a, stroke_width)
end

--创建富文本
function Util:RichText(lab, font_size, width, height, labcolor, stroke, stroke_width)
	lab = (not lab or lab == "") and "" or lab
	labcolor = labcolor or Macros.Color.white_hex
	lab = GUI.RichText.GetColorWord(lab, labcolor, stroke, stroke_width)

	font_size = font_size or Macros.TypefaceSize.normal
	font_size = StartConfig.FontSizeScaleFactor and StartConfig.FontSizeScaleFactor * font_size or font_size

	width = width or 0
	height = height or 0

	local richText = CPPRichText.CreateRichText(width, height, Util.FontType, font_size)
	if StartConfig.RTReadingOrder then
		richText:SetReadingOrder(StartConfig.RTReadingOrder)
	end	
	richText:SetText(lab)
	richText:SetCascadeOpacityEnabled(true)
	
	return richText
end

--创建Sprite
function Util:Sprite(img, touch_func, touch_swallow)
	if not img then
		LogTool.LogError("[Util:Sprite] 参数img无效")
		return
	end

	local sp = SpriteEx.CreateWithImageFile(img)
	if not sp then
		local error_msg = img
		if type(img) == "userdata" then
			error_msg = string.format("img_path:%s img_name:%s", img.image_path, img.clip_image_name)
		end
		LogTool.LogError("[Util:Sprite] 创建SpriteEx失败 %s", error_msg)
		return
	end
	sp:SetCascadeOpacityEnabled(true)

	if touch_func then
		local layou = self:Layout(sp:GetContentWidth(), sp:GetContentHeight(), touch_func)
		layou:SetAnchorPoint(0.5,0.5)
		layou:SetTouchSwallow(touch_swallow == nil and true or touch_swallow)
		sp:AddChild(layou)
		PosTool.Center(layou)
	end	

	return sp, sp:GetContentWidth() > 0
end

--九宫格的sprite
--@param inner_rect：中心区域的尺寸(默认为{10, 10})
function Util:Sprite9(img, inner_rect, fun)
	if not img then
		LogTool.LogError("[Util:Sprite9] 参数img无效")
		return
	end

	local sp = Scale9SpriteEx.CreateWithImageFile(img)
	if not sp then
		local error_msg = img
		if type(img) == "userdata" then
			error_msg = string.format("img_path:%s img_name:%s", img.image_path, img.clip_image_name)
		end
		LogTool.LogError("[Util:Sprite9] 创建Scale9SpriteEx失败 %s", error_msg)
		return
	end
	sp:SetCascadeOpacityEnabled(true)

	--中心区域
	inner_rect = inner_rect or {10, 10}
	local org_w = sp:GetContentWidth()
	local org_h = sp:GetContentHeight()
	local inner_w = math.min(inner_rect[1], org_w)
	local inner_h = math.min(inner_rect[2], org_h)
	sp:SetInnerRect((org_w - inner_w) / 2 / org_w, (org_h - inner_h) / 2 / org_h, inner_w / org_w, inner_h / org_h)

	--点击回调
	if fun then
		local layou = self:Layout(sp:GetContentWidth(), sp:GetContentHeight(), fun)
		layou:SetAnchorPoint(0.5,0.5)
		sp:AddChild(layou)
		PosTool.Center(layou)
	end

	return sp
end

--创建进度条
function Util:LoadingBar(img)
	local bar = UILoadingBarEx.CreateWithImage(img)
	if not bar then
		LogTool.LogError("[Util:LoadingBar] 创建UILoadingBarEx失败")		
		return
	end
	bar:SetCascadeOpacityEnabled(true)
	return bar
end

--创建可触摸文本
function Util:Text(lab, font_size, color, fun , mb)
	local labex = nil
	if not mb then
		labex = Util:Label( lab , font_size , color )
	else
		labex = Util:Name( lab , font_size , color )
	end

	if fun then
		local layout = Util:Layout( labex:GetContentWidth() , labex:GetContentHeight() , function ( ... )
			fun()
		end )
		layout:SetAnchorPoint( 0.5 , 0.5 )
		labex:AddChild( layout )
		PosTool.Center( layout )
	end

	return labex
end

--创建输入框
function Util:TextField(place,max)
	place = place or " "

	local TextField = UITextFieldEx.Create()
	TextField:SetAnchorPoint(0.5, 0.5)
	TextField:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_LEFT)
	TextField:SetFontName(Util.FontType)
	TextField:SetFontSize(Macros.TypefaceSize.normal)
	TextField:SetCascadeOpacityEnabled(true)
	TextField:SetTouchSoundInfo(Macros.Game.SystemIDForSound.COMMON, 2)
	
	if place then
		TextField:SetPlaceHolderText(place)
	end

	if max then
		TextField:SetMaxLengthEnabled(true)
		TextField:SetMaxLength(max)
	end

	return TextField
end

--设置输入框的默认属性
function Util:WidgetTextField(widget)
	if not widget then
		return
	end

	widget:SetTouchSoundInfo(Macros.Game.SystemIDForSound.COMMON, 2)
	widget:SetPlaceHolderColor(Macros.Color.content)
	widget:SetFontName(Util.FontType)
	
	return widget
end

--创建输入框
function Util:EditText(width, height, holder_text, holder_text_color, font_size)
	holder_text = holder_text or "import"
	holder_text_color = holder_text_color or Macros.Color.white
	font_size = font_size or Macros.TypefaceSize.normal

	local edittext = CPPEditText.CreateEditText("", width, height, Util.FontType, font_size)
	edittext:SetHolderText(holder_text)
	edittext:SetPlaceHolderColor(holder_text_color)
	edittext:SetCascadeOpacityEnabled(true)
	return edittext
end

--创建实心圆遮罩
function Util:CircleClippingNode(center_x, center_y, radius)
	center_x = center_x or 0
	center_y = center_y or 0

	local stencil_node = CPPGraphicLabel.Node()
	stencil_node:DrawSolidCircle(center_x, center_y, radius, 0, 0, 0, 0xff)

	local clipping_node = ClippingNodeEx.Node()
	clipping_node:SetStencilMask(stencil_node)

	return clipping_node
end

--名字Label(品质、描边)
function Util:Name(lab, font_size, quality, stroke_width, width, height)
	lab = (not lab or lab == "") and "" or lab

	font_size = font_size or Macros.TypefaceSize.normal

	quality = quality or 0

	stroke_width = stroke_width or 2

	width = width or 0

	height = height or 0

	local labex = Util:Label(lab, font_size)
	labex:SetColor(unpack(Macros.Quality[quality]))
	labex:SetStrokeColor(unpack(Macros.Quality_Stroke[quality]))
	labex:SetStrokeWidth(stroke_width)
	labex:SetTextWidthAndHeight(width, height)
    labex:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
    labex:SetCascadeOpacityEnabled(true)

	return labex
end

--用作返回属性类型和数值
--类型 值 一个值 一个不带加号的值 类型+值
function Util:Nature(type, num, part, ignore_plus, ignore_colon)
	local worth = ""
	local key , worth = Util:GroupType(type , num)
	if key == true then
		if not ignore_plus then
			worth = "+"..worth
		end
	else
		worth = num
		if not ignore_plus then
			worth = "+"..worth
		end
	end

	local name = Macros.Game.BattlePropName[type] or ""
	if part then
		return name, worth
	elseif ignore_colon then
		return name..worth
	else
		return name ..CPPGameLib.GetString("common_colon")..worth
	end
end

--组合类型
function Util:GroupType(type , num)
	local value = {
		[Macros.Game.BattleProp.DAMAGE_DECREASE_RATE]	   = "%s%%",
		[Macros.Game.BattleProp.DAMAGE_INCREASE_RATE] 	   = "%s%%",
		[Macros.Game.BattleProp.PENETRATION_RATE] 	  	   = "%s%%",

		[Macros.Game.BattleProp.HIT_RATE] 			 	   = "%s%%",
		[Macros.Game.BattleProp.DODGE_RATE] 		 	   = "%s%%",
		[Macros.Game.BattleProp.CRIT_RATE] 			  	   = "%s%%",
		[Macros.Game.BattleProp.CRIT_DEFENCE_RATE] 	  	   = "%s%%",
		[Macros.Game.BattleProp.CRIT_RATE_CORRECT]    	   = "%s%%",
		[Macros.Game.BattleProp.CRIT_DEFENCE_RATE_CORRECT] = "%s%%",
		[Macros.Game.BattleProp.BLOCK_RATE] 			   = "%s%%",
		[Macros.Game.BattleProp.BLOCK_RATE_CORRECT] 	   = "%s%%",
		[Macros.Game.BattleProp.BEAT_BACK_CORRECT] 		   = "%s%%",
		[Macros.Game.BattleProp.BEAT_BACK_DEFENCE_CORRECT] = "%s%%",

		[Macros.Game.BattleProp.ATTACK_RATE] 			   = "%s%%",
		[Macros.Game.BattleProp.PHYSICAL_DEFENCE_RATE] 	   = "%s%%",
		[Macros.Game.BattleProp.MAGIC_DEFENCE_RATE] 	   = "%s%%",
		[Macros.Game.BattleProp.HP_RATE] 	  		   = "%s%%",

		[Macros.Game.BattleProp.ATTACK_RATE_FINAL] 	  		   = "%s%%",
		[Macros.Game.BattleProp.PHYSICAL_DEFENCE_RATE_FINAL]   = "%s%%",
		[Macros.Game.BattleProp.MAGIC_DEFENCE_RATE_FINAL] 	   = "%s%%",
		[Macros.Game.BattleProp.HP_RATE_FINAL] 	  		       = "%s%%",
	}

	local key = false
	local worth = " "
	if value[type] then
		num = MathTool.TransProbCoeffPercentage(num)
		worth = string.format(value[type],tostring(num))
		key = true
	end
	return key , worth 
end

--tab
-- {
-- 	{ zi = 字, ys = 颜色, jg = 间隔, mb = 描边, dx = 大小 yy = 阴影},
-- 	{ tp = 图片, jg = 间隔, dx = 大小},
-- }
--用作好几个组件拼起来居中显示的强迫症控件 2.0版
function Util:LabCenter(tab)
	local layout_width = 0
	local layout_height
	local widget_list = {}
	local layout = self:Layout(layout_width, 0)
	layout:SetAnchorPoint(0.5,0.5)

	local widget, widget_width, widget_height
	for i,v in ipairs(tab) do
		if v.zi then
			if v.mb then
				widget = self:Name(v.zi,v.dx,v.ys)
			else
				widget = self:Label(v.zi,v.dx,v.ys)
				if v.yy then
					widget:SetShadow(true)
					widget:SetShadowColor(0, 0, 0, 255)
				end
			end				

			widget_width = widget:GetContentWidth()
			widget_height = widget:GetContentHeight()

		elseif v.tp then
			v.dx = v.dx or 1
			widget = self:Sprite(v.tp , v.hz )
			widget:SetScaleFactor(v.dx)
			widget_width = widget:GetContentWidth() * v.dx
			widget_height = widget:GetContentHeight() * v.dx		
		elseif v.num then
			v.dx = v.dx or 1
			local num_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, v.num , true)
			widget = GUI.ImgFont.New( num_sp )
			widget:SetText(tostring( v.value or 0 ))

			widget_width = widget:GetNode():GetContentWidth() * v.dx
			widget_height = widget:GetNode():GetContentHeight() * v.dx		

		end
		widget:SetAnchorPoint(0, 0.5)

		if not layout_height then
			layout_height = widget_height
			layout:SetContentHeight(layout_height)
		end

		local jg = v.jg or 0

		--设置位置
		if v.num then
			layout:AddChild(widget:GetNode())
		else
			layout:AddChild(widget)
		end

		widget:SetTag(i)
		table.insert(widget_list,widget)
		PosTool.RightCenter(widget, jg + (v.x or 0) , v.y or 0 )

		--重设尺寸
		layout_width = layout_width + widget_width + jg
		layout:SetContentWidth(layout_width)
	end
	return layout,widget_list
end

--用作好几个组件拼起来居中显示的强迫症控件 3.0版 (支持换行)
-- local tab = 
-- 	{
-- 		{ zi = "一二三四 %s 五六七八", ys = Macros.Color.headline , mb = Macros.Color.orange_stroke_hex , dx = 20},
-- 		{ tp = sp, dx = 1 , gs = "一二三四 %s 五六七八", ys = Macros.Color.headline , mb = Macros.Color.orange_stroke_hex , dx = 20 },
-- 		{ zi = "二二三四 %s 五六七八", ys = Macros.Color.red , dx = 50},
-- 	}
function Util:LabCenter_3(tab , font_size, width, height)
	local function PingZhuangYanSe(ys)
		local txt =  "<COLOR VALUE=FF%s>%s</COLOR>"
		local ysl = ""
		for i,vv in ipairs(ys) do
			local sl = tostring(string.upper(string.format("%0#x",vv)))
			if string.len(sl) < 4 then
				sl = string.sub(sl,-1)
				sl = "0"..sl
			else
				sl = string.sub(sl,-2)
			end
			ysl = ysl .. sl
		end
		return ysl
	end

    local text = " %s "
    for i,v in ipairs(tab) do
    	--如果是字
    	if v.zi then
    		local zi = string.format("<FONT SIZE=%s>%s</FONT>", v.dx or Macros.TypefaceSize.slightly , v.zi)
    		text = string.format(text , zi )
    		if v.mb then
    			local ys = PingZhuangYanSe(v.ys )
    			text = string.format("<COLOR VALUE=FF%s OUTLINE=%s OUTLINE_W=2>%s</COLOR>" , ys , v.mb , text )  
    		else
	    		if v.ys then
	    			local ys = PingZhuangYanSe(v.ys )
	    			text = string.format("<COLOR VALUE=FF%s>%s</COLOR>" , ys , text )  
	    		end
	    	end
    	--如果是图
    	elseif v.tp then
    		local tp = string.format("<FACEIMG IMG=%s TEXRECT={%d,%d,%d,%d} SCALE=%f>", v.tp.image_path, v.tp.x, v.tp.y, v.tp.width, v.tp.height, v.dx or 1 )
    		if v.gs then
	    		tp = tp .. "%s"
	    		local zi = string.format("<FONT SIZE=%s>%s</FONT>", v.gsdx or Macros.TypefaceSize.slightly , v.gs)
	    		tp = string.format(tp , zi )
	    		if v.mb then
	    			local ys = PingZhuangYanSe(v.ys )
	    			tp = string.format("<COLOR VALUE=FF%s OUTLINE=%s OUTLINE_W=2>%s</COLOR>" , ys , v.mb , tp )  
	    		else
		    		if v.ys then
		    			local ys = PingZhuangYanSe(v.ys )
		    			tp = string.format("<COLOR VALUE=FF%s>%s</COLOR>" , ys , tp )  
		    		end
		    	end
	    	end

    		text = string.format(text , tp , "" )
    	end

    	if i ~= #tab then
    		text = text .. "%s"
    	end
    end
    
    return self:RichText(text, font_size, width, height)
end

--读表式富文本
-- mainLab = "一二三四五六七{1}9，10，100，{2},B,C,D,E,F,G{3}"
-- tab = {  
-- 		{zi = "八", dx = 30, ys = Macros.Color.green },
-- 		{zi = "A", dx = 20, ys = Macros.Color.blue },
-- 		{zi = "?", dx = 22, ys = Macros.Color.purple },
-- 	}
function Util:Echo(mainLab, tab, ys, font_size, width, height)
	
	mainLab = self:EchoText(mainLab, tab, ys )

	return self:RichText(mainLab, font_size, width, height)
end

function Util:EchoText(mainLab, tab, ys )
	local function PingZhuangYanSe(ys, lab)
		local txt =  "<COLOR VALUE=FF%s>%s</COLOR>"
		local ysl = ""
		for i,vv in ipairs(ys) do
			local sl = tostring(string.upper(string.format("%0#x",vv)))
			if string.len(sl) < 4 then
				sl = string.sub(sl,-1)
				sl = "0"..sl
			else
				sl = string.sub(sl,-2)
			end
			ysl = ysl .. sl
		end
		lab = string.format(txt, ysl, lab)
		return lab
	end

	if ys then
		mainLab = PingZhuangYanSe(ys,mainLab)
	end

	local t1 = {}
	for _,v in ipairs(tab) do
		if v.ys then
			txt = PingZhuangYanSe(v.ys,v.zi)
			table.insert(t1,txt)
		else
			local zi = string.format("<FONT SIZE=%s>%s</FONT>",v.dx or Macros.TypefaceSize.normal, v.zi)
			table.insert(t1,zi)			
		end
	end

	for i,v in ipairs(t1) do
		local t = "{"..i.."}"
		mainLab = string.gsub(mainLab, t, v)
	end

	return mainLab 
end


-- 货币类型 图片大小  --可选 显示文字 字体大小
function Util:MoneyControl(type, size, lab, font_size, color, jg)
	local layou = nil
	size = size or 1
	local sp = self:GetMoneyIconImgData(type)
	if lab then
		layou = self:LabCenter({
									{ tp = sp, dx = size},
									{ zi = lab, dx = font_size, ys = color , jg = jg},
								})
	else
		layou = self:Sprite(sp)
		layou:SetScaleFactor(size)
	end
	return layou
end

--获得货币图片路径
function Util:GetMoneyIconImgData(type_)

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_MONEYICON, "type_"..type_)
	if type_ == Macros.Game.RolePropType.BATTLE_VALUE then
		sp = Resource.PathTool.GetUIResPath( "commonmoneyicon/type_" .. Macros.Game.RolePropType.BATTLE_VALUE , true )
	end

	return sp
end

--分割tab
function Util:SegmentationTab(tab,num)
    local t1 = {}
    local t2 = {}
    local idnex = 0
    for i,v in ipairs(tab) do
        idnex = idnex + 1
        table.insert(t2,v)
        if idnex == num then
			table.insert(t1,t2)
			idnex = 0
			t2 = {}
        end
    end
    if #t2 ~= 0 then
    	table.insert(t1,t2)
    end
    return t1
end

--数字转换
function Util:NumTransform( num )
	local num_ = num
	-- if num >= 1000 and num < 10000 then
	-- 	num_ = CPPGameLib.GetString( "common_qian" , math.ceil( num / 1000 ) ) 
	-- else	
	if num >= 10000000 then
		--达到千万只显示万
		num_ = CPPGameLib.GetString( "common_wan" , math.floor( tonumber( num ) / 10000 )) 
	elseif num >= 1000000 then
		--达到百万保留一位小数
		num_ = CPPGameLib.GetString( "common_wan" , math.floor(tonumber( num )/ 1000 )/10) 
	end
	return num_ , num
end

--关卡点连接
function Util:GameLevelConnect(node,posInfo_1,posInfo_2,is_boss,is_runAction,fun)
	--BOSS框120,普通框80
    local posInfo_2_y = is_boss and posInfo_2[2] - 50 or posInfo_2[2] - 40
    --哪个距离远
    local is_x_shorter = math.abs(posInfo_2[1] - posInfo_1[1]) <= math.abs(posInfo_2_y - posInfo_1[2] + 40) and true or false
   	--长度
    local shorter_distance = is_x_shorter and posInfo_2[1] - posInfo_1[1] or posInfo_2_y - posInfo_1[2] + 40
    local longer_distance =	is_x_shorter and posInfo_2_y - posInfo_1[2] + 40 or posInfo_2[1] - posInfo_1[1]
   	local is_x_shorter_add = is_x_shorter and shorter_distance >= 0 and 1 or not is_x_shorter and longer_distance >= 0 and 1 or -1
   	local is_y_shorter_add = is_x_shorter and longer_distance >= 0 and 1 or not is_x_shorter and shorter_distance >= 0 and 1 or -1
   	shorter_distance = math.abs(shorter_distance)
   	longer_distance = math.abs(longer_distance)
    --点的数量
    local point_num = math.floor(shorter_distance/14)
	point_num = point_num <= 2 and 2 or point_num >= 5 and 5 or point_num
    --x间隙
    local x_gap = is_x_shorter and (shorter_distance - 14 * point_num)/(point_num + 1) or (longer_distance - 14 * point_num )/(point_num + 1)
    x_gap = x_gap * is_x_shorter_add
    --y间隙
    local y_gap = is_x_shorter and (longer_distance - 14 * point_num )/(point_num + 1) or (shorter_distance - 14 * point_num)/(point_num + 1)
    y_gap = y_gap * is_y_shorter_add
    --计算两个关卡之间的点     
    local image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL, "path_point")
    for i=1,point_num do
    	local sprite = Util:Sprite(image_data)
    	node:AddChild(sprite,-1)
    	sprite:SetTag(i)
    	sprite:SetPosition(34 + i * x_gap + is_x_shorter_add *(14*i - 7), - 10 + i * y_gap + is_y_shorter_add *(14*i - 7))
    	if is_runAction then
    		sprite:SetVisible(false)
    	end
    end
    local path_point_count = 0
    local replayCount
    local function visiblePathPoint()
    	path_point_count = path_point_count + 1
    	if path_point_count <= point_num then
    		if node:GetChildByTag(path_point_count) then
    			local sprite = node:GetChildByTag(path_point_count)
    			sprite:SetVisible(true)   			
    		end
    		if path_point_count == point_num then
	    		fun()
	    		return
	    	end 
	    	replayCount()
    	end    	
    end
    --动态创建
    if is_runAction then
    	if point_num >= 1 then
			replayCount = function ()
				CPPActionManager.DelayTo(node,0.3,GlobalCallbackMgr:AddCallBackFunc(function ()
				visiblePathPoint()
				end))	
			end			
			visiblePathPoint()	
    	end
    end
end

--创建天梯赛段位名图片
function Util:Dan(id)
	local cnf = CPPGameLib.GetConfig("LadderDan", id )
	if cnf then
		local tab = {}
		if cnf.star then
			tab = {
						{ tp = Resource.PathTool.GetLadderWordartPath("sp_level_" .. cnf.type , true )},
						{ tp = Resource.PathTool.GetLadderWordartPath("sp_level_" .. cnf.type .. "_" .. cnf.star , true )},
					}
		else
			tab = {
						{ tp = Resource.PathTool.GetLadderWordartPath("sp_level_" .. cnf.type , true )},
					}
		end

		return self:LabCenter(tab)
	end
end

--获取天梯赛段位名tbl
function Util:DanTbl(id)
	local tab	
	local cnf = CPPGameLib.GetConfig("LadderDan", id)
	if cnf then
		if cnf.star then
			tab = {
						{ tp = Resource.PathTool.GetLadderWordartPath("sp_level_" .. cnf.type , true )},
						{ tp = Resource.PathTool.GetLadderWordartPath("sp_level_" .. cnf.type .. "_" .. cnf.star , true )},
					}
		else
			tab = {
						{ tp = Resource.PathTool.GetLadderWordartPath("sp_level_" .. cnf.type , true )},
					}
		end
	end
	return tab
end

--创建称号、昵称图片
function Util:TitleAndNickNameSpite(title_id , nickname_id , nickname_interval , nickname_interval_ )

	local nameWidth = nickname_interval or 50
	local nameHeight = nickname_interval_ or 25

	local layer = self:Layout( nameWidth , nameHeight )
	layer:SetAnchorPoint( 0.5 , 0.5 )	

	local t1 = {}

	--称号
	if title_id then 
		local cnf = CPPGameLib.GetConfig( "Title" , title_id , false )
		if cnf and cnf.type == Macros.Game.TITLE_TYPE.TITLE then
			local sp = Resource.PathTool.GetTitleIconPath( "sp_title_".. title_id , true )
			t1[ #t1 + 1 ] = self:Sprite( sp )
			t1[#t1]:SetAnchorPoint( 0.5 , 0 )
			t1[#t1]:SetPosition( layer:GetContentWidth()/2 , layer:GetContentHeight() + 5 )
			layer:AddChild( t1[#t1] )
		end
	end

	if nickname_id then 
		local cnf = CPPGameLib.GetConfig( "Title" , nickname_id , false )
		if cnf and cnf.type == Macros.Game.TITLE_TYPE.PREFIX then
			local sp = Resource.PathTool.GetTitleIconPath( "sp_prefix_".. nickname_id , true )
			t1[ #t1 + 1 ] = self:Sprite( sp )
			t1[ #t1 ]:SetAnchorPoint( 1 , 0.5 )
			t1[ #t1 ]:SetPosition( -5 , layer:GetContentHeight()/2 )
			layer:AddChild( t1[ #t1 ] )

			if cnf.postfix == 1 then
				local sp = Resource.PathTool.GetTitleIconPath( "sp_suffix_".. nickname_id , true )
				t1[ #t1 + 1 ] = self:Sprite( sp )
				t1[ #t1 ]:SetAnchorPoint( 0 , 0.5 )
				t1[ #t1 ]:SetPosition( layer:GetContentWidth() + 5 , layer:GetContentHeight()/2 )
				layer:AddChild( t1[ #t1 ] )
			end
		end
	end

	return layer , t1
end

--------------------------------------------------------------------------------------------------------------------------------
--接口重定向
--------------------------------------------------------------------------------------------------------------------------------
function Util:RedirectAPI()
	if not Util.LabelEx_SetFontSize then
		Util.LabelEx_SetFontSize = LabelEx.SetFontSize
		LabelEx.SetFontSize = function(label, font_size)
			font_size = StartConfig.FontSizeScaleFactor and StartConfig.FontSizeScaleFactor * font_size or font_size
			Util.LabelEx_SetFontSize(label, font_size)
		end
	end

	if not Util.UITextEx_SetFontSize then
		Util.UITextEx_SetFontSize = UITextEx.SetFontSize
		UITextEx.SetFontSize = function(ui_text, font_size)
			font_size = StartConfig.FontSizeScaleFactor and StartConfig.FontSizeScaleFactor * font_size or font_size
			Util.UITextEx_SetFontSize(ui_text, font_size)
		end
	end	

	if not Util.UIButtonEx_SetFontSize then
		Util.UIButtonEx_SetFontSize = UIButtonEx.SetFontSize
		UIButtonEx.SetFontSize = function(ui_button, font_size)
			font_size = StartConfig.FontSizeScaleFactor and StartConfig.FontSizeScaleFactor * font_size or font_size
			Util.UIButtonEx_SetFontSize(ui_button, font_size)
		end
	end		
end

if StartConfig.DistrictLanguage == "dl_wy" then
	Util:RedirectAPI()
end