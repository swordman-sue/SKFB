

Macros = Macros or {}

Macros.Color = {}

--------------------------------------------------------------------------------------------------------------------------------
--颜色
--------------------------------------------------------------------------------------------------------------------------------
--白色
Macros.Color.white    		  = {0xfe, 0xf4, 0xe3}
Macros.Color.white_hex 		  = "FFFEF4E3"
Macros.Color.white_rt  		  = "<COLOR VALUE=FFFEF4E3>%s</COLOR>"
Macros.Color.white_stroke	  = {0x57, 0x45, 0x29, 0xff}
Macros.Color.white_stroke_hex = "FF574529"
Macros.Color.white_stroke_rt  = "<COLOR VALUE=FFFEF4E3 OUTLINE=FF7A3A26 OUTLINE_W=2>%s</COLOR>"

--绿色
Macros.Color.green 			  = {0x6e,0xf1,0x2d}
Macros.Color.green_hex 		  = "FF6EF12D"
Macros.Color.green_rt 		  = "<COLOR VALUE=FF6EF12D>%s</COLOR>"
Macros.Color.green_stroke 	  = {0x2D,0x3F,0x0C,0xff}
Macros.Color.green_stroke_hex = "FF2D3F0C"
Macros.Color.green_stroke_rt  = "<COLOR VALUE=FF6EF12D OUTLINE=FF2D3F0C OUTLINE_W=2>%s</COLOR>"

--蓝色
Macros.Color.blue 			  = {0x00,0xb4,0xff}
Macros.Color.blue_hex 		  = "FF00B4FF"
Macros.Color.blue_rt 		  = "<COLOR VALUE=FF00B4FF>%s</COLOR>"
Macros.Color.blue_stroke 	  = {0x0b,0x3b,0x66,0xff}
Macros.Color.blue_stroke_hex  = "FF0B3B26"
Macros.Color.blue_stroke_rt   = "<COLOR VALUE=FF00B4FF OUTLINE=FF0B3B26 OUTLINE_W=2>%s</COLOR>"

--紫色
Macros.Color.purple 		  = {0xf0,0x41,0xdd}
Macros.Color.purple_hex 	  = "FFF041DD"
Macros.Color.purple_rt 	      = "<COLOR VALUE=FFF041DD>%s</COLOR>"
Macros.Color.purple_stroke 	  = {0x3d,0x0a,0x43,0xff}
Macros.Color.purple_stroke_hex= "FF3D0A43"
Macros.Color.purple_stroke_rt = "<COLOR VALUE=FFF041DD OUTLINE=FF3D0A43 OUTLINE_W=2>%s</COLOR>"

--橙色
Macros.Color.orange   		  = {0xff,0x99,0x3f}
Macros.Color.orange_hex 	  = "FFFF993F"
Macros.Color.orange_rt 	      = "<COLOR VALUE=FFFF993F>%s</COLOR>"
Macros.Color.orange_stroke    = {0x56,0x23,0x07,0xff}
Macros.Color.orange_stroke_hex= "FF562307"
Macros.Color.orange_stroke_rt = "<COLOR VALUE=FFFF993F OUTLINE=FF562307 OUTLINE_W=2>%s</COLOR>"

--红色
Macros.Color.red 			  = {0xff,0x4c,0x34}
Macros.Color.red_hex 	  	  = "FFFF4C34"
Macros.Color.red_rt 	  	  = "<COLOR VALUE=FFFF4C34>%s</COLOR>"
Macros.Color.red_stroke   	  = {0x49,0x0a,0x08,0xff}
Macros.Color.red_stroke_1  	  = {0xd4,0x27,0x10,0xff}
Macros.Color.red_stroke_hex   = "FF490A08"
Macros.Color.red_stroke_rt 	  = "<COLOR VALUE=FFFF4C34 OUTLINE=FF490A08 OUTLINE_W=2>%s</COLOR>"

--金色
Macros.Color.golden 			= {0xff,0xff,0x00}
Macros.Color.golden_hex			= "FFFFFF00"
Macros.Color.golden_rt			= "<COLOR VALUE=FFFF00>%s</COLOR>"
Macros.Color.golden_stroke_rt	= "<COLOR VALUE=FFFF00 OUTLINE=FF8b4815 OUTLINE_W=2>%s</COLOR>"
Macros.Color.golden_stroke 		= {0x8b, 0x48, 0x15, 0xff}

--深橙
Macros.Color.darkOrange = {0xcf,0x58,0x00}

--纯白
Macros.Color.lilyWhite = {0xff,0xff,0xff}
Macros.Color.lilyWhite_hex = "FFFFFFFF"

--纯黑
Macros.Color.lilyBlack = {0x00, 0x00, 0x00}
Macros.Color.lilyBlack_stroke = {0x00, 0x00, 0x00,0xff}
Macros.Color.lilyBlack_hex = "FF000000"

--灰度黑
Macros.Color.GrayBlack = {0x44, 0x44, 0x44}
Macros.Color.GrayBlack_stroke = {0x44, 0x44, 0x44,0xff}

--灰度黑
Macros.Color.GrayBlackII = {0x22, 0x22, 0x22}

--灰色
Macros.Color.Gray = {0x96, 0x96, 0x96}

--标题色
Macros.Color.headline			= {0xc4,0x64,0x1a}
Macros.Color.headline_stroke 	= {0x57, 0x45, 0x29, 0xff}
Macros.Color.headline_rt		= "<COLOR VALUE=FFC4641A>%s</COLOR>"
Macros.Color.headline_hex   	= "FFC4641A"
Macros.Color.headline_stroke_rt	= "<COLOR VALUE=FFC4641A OUTLINE=FF553923 OUTLINE_W=2>%s</COLOR>"

--描述色
Macros.Color.content			= {0x55, 0x39, 0x23}
Macros.Color.content_rt			= "<COLOR VALUE=FF553923>%s</COLOR>"
Macros.Color.content_hex		= "FF553923"
Macros.Color.content_stroke_rt	= "<COLOR VALUE=FF553923 OUTLINE=FF553923 OUTLINE_W=2>%s</COLOR>"
Macros.Color.content_stroke     = {0x55, 0x39, 0x23,0xff}

--描述色(高亮)
Macros.Color.blinkcontent 			= {0xf7, 0xdd, 0xb0}
Macros.Color.blinkcontent_rt 		= "<COLOR VALUE=FFF7DDB0>%s</COLOR>"
Macros.Color.blinkcontent_hex		= "FFF7DDB0"
Macros.Color.blinkcontent_stroke_hex= "FF553923"
Macros.Color.blinkcontent_stroke_rt	= "<COLOR VALUE=FFF7DDB0 OUTLINE=FF553923 OUTLINE_W=2>%s</COLOR>"
Macros.Color.blinkcontent_stroke   	= {0x55, 0x39, 0x23,0xff}

--重点色
Macros.Color.keypoint			= {0x83, 0x58, 0x31}
Macros.Color.keypoint_rt		= "<COLOR VALUE=FF835831>%s</COLOR>"
Macros.Color.keypoint_hex		= "FF835831"
Macros.Color.keypoint_stroke_rt	= "<COLOR VALUE=FF835831 OUTLINE=FF553923 OUTLINE_W=2>%s</COLOR>"

--通用描边色
Macros.Color.common_stroke 		= {0x7a, 0x3a, 0x26, 0xff}
Macros.Color.common_stroke_hex 	= "FF7a3a26"

--字体描边（红）
Macros.Color.lab_red 		= { 0x91, 0x26, 0x2c, 0xff}

--按钮(普通字色)
Macros.Color.btn 		= {0xfe,0xf4,0xe3}
Macros.Color.btn_rt		= "<COLOR VALUE=FFFEF4E3>%s</COLOR>"
Macros.Color.btn_hex 	= "FFFEF4E3"
--按钮描边（黄）
Macros.Color.button_yellow 		= { 0x9b, 0x68, 0x2c, 0xff}
--按钮描边（红）
Macros.Color.button_red 		= { 0x9b, 0x4b, 0x2c, 0xff}
--按钮(棕底按钮)
Macros.Color.btn_1 		= {0xf5,0xdf,0xc3}
--按钮描边（棕）
Macros.Color.button_palm 		= { 0x7b, 0x49, 0x10, 0xff}

--通用阴影色
Macros.Color.shadow_color 		= {0x00, 0x00, 0x00, 0xff}
Macros.Color.shadow_color_hex 	= "FF000000"

--正面（绿色）
Macros.Color.front_color = {0x28,0x87,0x1e}
Macros.Color.front_color_hex = "FF28871e"
Macros.Color.front_color_rt  = "<COLOR VALUE=FF28871e>%s</COLOR>"

--负面（红色）
Macros.Color.red_color = {0xc4,0x36,0x23}
Macros.Color.red_color_hex = "FFC43623"
Macros.Color.red_color_rt  = "<COLOR VALUE=FFC43623>%s</COLOR>"

--系统标题渐变色(富文本)
Macros.Color.system_headline_stroke = {0x5b,0x5b,0x54,0xff}
Macros.Color.system_headline_stroke_rt = "<COLOR VALUE={FFFFF2DC,FFFFF2DC,FFA48554,FFA48554}OUTLINE=FF421C03 OUTLINE_W=2>%s</COLOR>"

--主城界面系统字体渐变色(富文本)
Macros.Color.city_headline_rt = "<COLOR VALUE={FFFFFEF4,FFFFFEF4,FFF9D087,FFF9D087} OUTLINE=FF421C03 OUTLINE_W=2>%s</COLOR>"

--主城字体描边
Macros.Color.cityblack_stroke = {0x42, 0x1c, 0x03,0xff}

--排行榜
Macros.Color.tangerine_color = {0xff,0x99,0x3f}

--道具名
Macros.Color.pack_color = {0x9b,0x68,0x2c}

--禁用
Macros.Color.forbidden 				= {0xfe,0xf4,0xe3}
Macros.Color.forbidden_stroke 		= {0x5b, 0x5b, 0x54, 0xff}

--公会
Macros.Color.union 		= {0x2b,0xc5,0xc0}
Macros.Color.union_rt		= "<COLOR VALUE=FF2BC5C0>%s</COLOR>"

--私聊
Macros.Color.pm 		= {0xf6,0x0e,0xf9}
Macros.Color.pm_rt		= "<COLOR VALUE=FFF60EF9>%s</COLOR>"

--世界
Macros.Color.world 		= {0xff,0x99,0x3f}
Macros.Color.world_rt		= "<COLOR VALUE=FFFF993F>%s</COLOR>"

--棕色
Macros.Color.brown 		= {0x55,0x39,0x23}

--条目字体名字颜色
Macros.Color.itemName 	= {0xfe,0xc8,0x82}

--条目字体颜色
Macros.Color.itemText 	= {0xf2,0xd1,0xa8}

--功能预告
Macros.Color.system_foreshow_bg_stroke_rt = "<COLOR VALUE={FFFFCA12,FFFFCA12,FFFFFDC3,FFFFFDC3}OUTLINE=FF89400B OUTLINE_W=2>%s</COLOR>"
Macros.Color.system_foreshow_sm_stroke_rt = "<COLOR VALUE={FFFFCF23,FFFFCF23,FFFFFCC0,FFFFFCC0}OUTLINE=FF89400B OUTLINE_W=2>%s</COLOR>"

--特殊字色
Macros.Color.Special 	= {0xfc,0xb5,0x44}
Macros.Color.Special_hex  = "FFFCB544"
Macros.Color.Special_rt		= "<COLOR VALUE=FFFCB544>%s</COLOR>"

--限时活动描边
Macros.Color.limit_activity_stroke = {0x76,0x06,0x06,0xff}
Macros.Color.limit_activity_stroke_rt = "<COLOR VALUE=FFFF00 OUTLINE=FF760606 OUTLINE_W=2>%s</COLOR>"

--副本关卡界面的章节名富文本渐变色
Macros.Color.dungeon_chapter_name_rt_gradient = "<COLOR VALUE={FFFDEDA3,FFFDEDA3,FFF7B031,FFF7B031}>%s</COLOR>"

--剧情、指引默认颜色
Macros.Color.lead_story_def_hex = "FFA57F5C"

--打折
Macros.Color.blue_discount =  { 0x15, 0x69, 0x94, 0xff }

Macros.Color.red_discount  =  { 0x9a, 0x27, 0x27, 0xff }

--登陆界面
Macros.Color.register  =  { 0x3e, 0x6c, 0xa8 }
Macros.Color.registerImport  =  { 0xb8, 0xb8, 0xb8 }
Macros.Color.register_rt		= "<COLOR VALUE=FF3E6CA8>%s</COLOR>"

--vip礼包标签
Macros.Color.vipgift  =  { 0x90, 0x3c, 0x00 }

--怪物攻城名字颜色
Macros.Color.monster_stormcastle  =  { 0x00, 0xff, 0xff }

--怪物攻城名字描边颜色
Macros.Color.monster_stormcastle_stroke  =  { 0x0c, 0x6a, 0x6a, 0xff}

--主界面角色名字颜色
Macros.Color.maincity_rolename = {0xfc, 0xb5, 0x44} 

--主界面角色名字描边颜色
Macros.Color.maincity_rolename_stroke = {0x42, 0x1c, 0x03, 0xff} 

Macros.Color.team_btn_join = {0x8c,0x3d,0x1b}

Macros.Color.team_btn_reject = {0x88,0x54,0x17}

--组队界面系统字体渐变色(富文本)
Macros.Color.team_headline_rt = "<COLOR VALUE={FFFFE6A4,FFFFE6A4,FFFFB400,FFFFB400} OUTLINE=FF421C03 OUTLINE_W=2>%s</COLOR>"
Macros.Color.team_headline 	= {0xFF,0xe6,0xa4}
Macros.Color.team_stroke = {0x42,0x1c,0x03,0xff}
Macros.Color.team_stroke_hex = "FF421C03"

--组队扭蛋机按钮描边
Macros.Color.gashaponmachine_stroke 		= {0xb1, 0x1f, 0x0c, 0xff}
Macros.Color.gashaponmachine_stroke_hex 	= "FFB11F0C"

--组队超链接
Macros.Color.team_link = "<LINK TYPE=%d><COLOR VALUE=FF6EF12D>%s</COLOR></LINK>"

--天梯主界面
Macros.Color.ladder = {0xff,0xf7,0x99}
--天梯排行榜
Macros.Color.ladderRank = {0xce,0xbd,0x5d}

--天梯匹配颜色
Macros.Color.ladderVs_1 = {0xa1,0xc8,0xf8}
--天梯排行榜
Macros.Color.ladderVs_2 = {0xf9,0xa7,0xa7}

--------------------------------------------------------------------------------------------------------------------------------
--字号
--------------------------------------------------------------------------------------------------------------------------------
Macros.TypefaceSize = {}

--最小
Macros.TypefaceSize.minimum 	= 16

--描述
Macros.TypefaceSize.slightly 	= 18

--正常
Macros.TypefaceSize.normal 		= 20

--按钮
Macros.TypefaceSize.largish  	= 22

--弹窗标题
Macros.TypefaceSize.popup 		= 24

--通用标签
Macros.TypefaceSize.tab 		= 26

--通用按钮
Macros.TypefaceSize.button 		= 24

--标题
Macros.TypefaceSize.headline  	= 28

--标题
Macros.TypefaceSize.headlineII  = 30

--------------------------------------------------------------------------------------------------------------------------------
-- 品质颜色
--------------------------------------------------------------------------------------------------------------------------------
Macros.Quality = {
	[0] = Macros.Color.btn,
	[1] = Macros.Color.green,
	[2] = Macros.Color.blue,
	[3] = Macros.Color.purple,
	[4] = Macros.Color.orange,
	[5] = Macros.Color.red,
	[100] = Macros.Color.golden,
	[101] = Macros.Color.btn,
	[102] = Macros.Color.btn,
	[103] = Macros.Color.btn,
	[104] = Macros.Color.btn,
}

Macros.Quality_Stroke = {
	[0] = Macros.Color.common_stroke,
	[1] = Macros.Color.green_stroke,
	[2] = Macros.Color.blue_stroke,
	[3] = Macros.Color.purple_stroke,
	[4] = Macros.Color.orange_stroke,
	[5] = Macros.Color.red_stroke,
	[100] = Macros.Color.golden_stroke,
	[101] = Macros.Color.button_red,
	[102] = Macros.Color.lab_red,
	[103] = Macros.Color.button_yellow,
	[104] = Macros.Color.GrayBlack_stroke,
}

Macros.Quality_RT = {
	[0] = Macros.Color.white_rt,
	[1] = Macros.Color.green_rt,
	[2] = Macros.Color.blue_rt,
	[3] = Macros.Color.purple_rt,
	[4] = Macros.Color.orange_rt,
	[5] = Macros.Color.red_rt,
}

Macros.Quality_Stroke_RT = {
	[0] = Macros.Color.white_stroke_rt,
	[1] = Macros.Color.green_stroke_rt,
	[2] = Macros.Color.blue_stroke_rt,
	[3] = Macros.Color.purple_stroke_rt,
	[4] = Macros.Color.orange_stroke_rt,
	[5] = Macros.Color.red_stroke_rt,
}

Macros.Quality_HEX = {
	[0] = Macros.Color.white_hex,
	[1] = Macros.Color.green_hex,
	[2] = Macros.Color.blue_hex,
	[3] = Macros.Color.purple_hex,
	[4] = Macros.Color.orange_hex,
	[5] = Macros.Color.red_hex,
}

Macros.Quality_Hex_Stroke = {
	[0] = Macros.Color.white_stroke_hex,
	[1] = Macros.Color.green_stroke_hex,
	[2] = Macros.Color.blue_stroke_hex,
	[3] = Macros.Color.purple_stroke_hex,
	[4] = Macros.Color.orange_stroke_hex,
	[5] = Macros.Color.red_stroke_hex,
}

--获取品质颜色
function Macros.QualityColor(quality)
	local color = Macros.Quality[quality] or Macros.Quality[0]
	return unpack(color)
end

function Macros.QualityColorStroke(quality)
	local color = Macros.Quality_Stroke[quality] or Macros.Quality_Stroke[0]
	return unpack(color)
end

--------------------------------------------------------------------------------------------------------------------------------
-- 渐变色
--------------------------------------------------------------------------------------------------------------------------------
--渐变色表（1黄 2绿 3蓝 4紫）
Macros.Color.GradientMap = 
{
	[1] = "<COLOR VALUE={FFF4EC2D,FFF4EC2D,FFFF6B00,FFFF6B00}>%s</COLOR>",
	[2] = "<COLOR VALUE={FF4CFFC6,FF4CFFC6,FF18FF00,FF18FF00}>%s</COLOR>",
	[3] = "<COLOR VALUE={FF4CFFF0,FF4CFFF0,FF00A2FF,FF00A2FF}>%s</COLOR>",
	[4] = "<COLOR VALUE={FFFFA0C6,FFFFA0C6,FFC500EF,FFC500EF}>%s</COLOR>",
}

function Macros.GradientColor(type)
	type = type or 1
	return Macros.Color.GradientMap[type] or Macros.Color.GradientMap[1]
end

