Modules = Modules or {}

--副本通关领取宝箱信息模板
Modules.TemplDungeonPassedAllTip = Modules.TemplDungeonPassedAllTip or BaseClass(GUI.Template)

function Modules.TemplDungeonPassedAllTip:__init(chapter_info,chapter_index,fb_type)

	self.__is_auto_load = true
	self.__open_callback = function()
		fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON
		local gamelevel_box_num = 0
		local  panel_part1 = self:GetWidget("Panel_part1")
		local  panel_part2 = self:GetWidget("Panel_part2")
		--精英宝箱
		for game_index, gamelevel_info in ipairs(chapter_info.gamelevel_list) do
			if gamelevel_info.box_award_flag == Modules.DungeonModule.BoxStatus.CAN_RECEIVE then
				gamelevel_box_num = gamelevel_box_num + 1
				local image_box = WidgetUnity.GetWidgetByName(panel_part1, "Image_box"..gamelevel_box_num)
				image_box:SetVisible(true)
				local box_imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL, "elite_box_close")
				image_box:IgnoreContentAdaptWithSize(true)
				image_box:SetImage(box_imgdata)
				WidgetUnity.GetWidgetByName(image_box, "Image_star"..gamelevel_box_num):SetVisible(false)
				local text_star = WidgetUnity.GetWidgetByName(image_box, "Text_star"..gamelevel_box_num)
				text_star:SetVisible(true)
				local config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(chapter_index, fb_type)
				local gamelevel_detail_info = CPPGameLib.GetConfig("GameLevel",config_chapter.gamelevel_list[game_index])
				Util:WidgetLabel(text_star,gamelevel_detail_info.name, Macros.TypefaceSize.popup, Macros.Color.content)
			end
		end
		local panel
		local Text_box_title
		if gamelevel_box_num > 0 then
			Text_box_title = WidgetUnity.GetWidgetByName(panel_part1, "Text_box_title")
			Util:WidgetLabel(Text_box_title, CPPGameLib.GetString("dungeon_gamelevel_tip"), Macros.TypefaceSize.normal, Macros.Color.content)
			Text_box_title = WidgetUnity.GetWidgetByName(panel_part2, "Text_box_title")
			Util:WidgetLabel(Text_box_title, CPPGameLib.GetString("dungeon_boxinfo_tip"), Macros.TypefaceSize.normal, Macros.Color.content)
			panel = panel_part2
		else
			Text_box_title = WidgetUnity.GetWidgetByName(panel_part1, "Text_box_title")
			Util:WidgetLabel(Text_box_title, CPPGameLib.GetString("dungeon_boxinfo_tip"), Macros.TypefaceSize.normal, Macros.Color.content)
			panel = panel_part1
			panel_part2:SetVisible(false)
		end
		gamelevel_box_num = 0
		--星级宝箱
		local config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(chapter_index, fb_type)
		local starbox_img_map = {"low", "middle", "high"}
		if config_chapter then
			for index, tgt_star in ipairs(config_chapter.star_list) do
				--初始化星级宝箱状态
				if chapter_info.star_award_list[index] == Modules.DungeonModule.BoxStatus.CAN_RECEIVE then
					gamelevel_box_num = gamelevel_box_num + 1
					local image_box = WidgetUnity.GetWidgetByName(panel, "Image_box"..gamelevel_box_num)
					image_box:SetVisible(true)
					local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL, starbox_img_map[index].."_box_close")
					image_box:IgnoreContentAdaptWithSize(true)
					image_box:SetImage(imgdata)
					local text_star = WidgetUnity.GetWidgetByName(image_box, "Text_star"..gamelevel_box_num)
					Util:WidgetLabel(text_star,chapter_info.cur_star.."/"..tgt_star, Macros.TypefaceSize.popup, Macros.Color.content)
				end
			end
		end
		--标题		
		Util:WidgetLabel(self:GetWidget("Text_title"), CPPGameLib.GetString("dungeon_gamelevel_passall_tips_title"), Macros.TypefaceSize.popup, Macros.Color.btn)
		--取消
		Util:WidgetButton(self:GetWidget("Button_cancel"),CPPGameLib.GetString("common_cancel"),function ()
			self:FireNextFrame(Macros.Event.DungeonModule.DUNGEON_ONEKEY_AWARD_GET_RESP,false)
		end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)

		--关闭
		Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
			self:FireNextFrame(Macros.Event.DungeonModule.DUNGEON_ONEKEY_AWARD_GET_RESP,false)
		end)
		
		--一键领取
		Util:WidgetButton(self:GetWidget("Button_onekeyget"),CPPGameLib.GetString("friend_Button_oneUseGet"),function ()
			CallModuleFunc(ModuleType.DUNGEON,"FBPassedAllReq",chapter_index,fb_type)
		end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_red)
	end
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_PASSEDALL_BOXTIP)	
end