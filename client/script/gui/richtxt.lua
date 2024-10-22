
GUI = GUI or {}

--富文本
GUI.RichText = GUI.RichText or {}

function GUI.RichText.GetColorWord(word, color, stroke_color, stroke_width)
	if not word then
		return "invalid"
	end
	color = color or Macros.Color.content_hex

	if stroke_color then
		stroke_width = stroke_width or 2
		return string.format([[<COLOR VALUE=%s OUTLINE=%s OUTLINE_W=%d>%s</COLOR>]], color, stroke_color, stroke_width, word)
	else
		return string.format([[<COLOR VALUE=%s>%s</COLOR>]], color, word)
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--接口重定向
--------------------------------------------------------------------------------------------------------------------------------
function GUI.RichText.RedirectAPI()
	if not GUI.RichText.SetText then
		GUI.RichText.SetText = CPPRichText.SetText
		CPPRichText.SetText = function(rich_text, content)			
			local content_converted = CPPSystemTools.ConvertUygur(content)
			GUI.RichText.SetText(rich_text, content_converted)
		end
	end

	if not GUI.RichText.SetFontSize then
		GUI.RichText.SetFontSize = CPPRichText.SetFontSize
		CPPRichText.SetFontSize = function(rich_text, font_size)
			print("skfb", "fuck SetFontSize RichText")
			font_size = StartConfig.FontSizeScaleFactor and StartConfig.FontSizeScaleFactor * font_size or font_size
			GUI.RichText.SetFontSize(rich_text, font_size)
		end
	end
end

if StartConfig.DistrictLanguage == "dl_wy" then
	GUI.RichText.RedirectAPI()
end