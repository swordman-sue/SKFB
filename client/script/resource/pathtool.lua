
--[[
%% @module: 资源路径工具 
%% @author: swordman sue
%% @created: 2016-07-06
--]]

Resource = Resource or {}

Resource.PathTool = Resource.PathTool or {}

--资源根目录
Resource.PathTool.Root = "resource/"

--渠道资源根目录
Resource.PathTool.RootTrd = "trdres/"

--UI
Resource.PathTool.UILayout = Resource.PathTool.Root .. "ui_layout/"
Resource.PathTool.UIRes = Resource.PathTool.Root .. "ui_res/"

--地图
Resource.PathTool.SceneMapCnf = Resource.PathTool.Root .. "map/scenemapcnf/"

--模型
Resource.PathTool.ModelRes = Resource.PathTool.Root .. "model/"

--特效
Resource.PathTool.SkeletonEffectPath = Resource.PathTool.Root .. "effects/skeleton/"
Resource.PathTool.ParticleEffectCnfPath = Resource.PathTool.Root .. "effects/particle/config/"
Resource.PathTool.ParticleEffectTexturePath = Resource.PathTool.Root .. "effects/particle/texture/"

--物品
Resource.PathTool.Items = Resource.PathTool.Root .. "items/"

--大图标
Resource.PathTool.BigIcon = Resource.PathTool.Root .. "bigicon/"

--章节图标
Resource.PathTool.ChapterIcon = Resource.PathTool.UIRes .. "dungeon_chapter_icon/"

--关卡线路
Resource.PathTool.DungeonLevelLine = Resource.PathTool.UIRes .. "dungeon_gamelevel_line/"

--头像
Resource.PathTool.HeroHead = Resource.PathTool.Root .. "heads/hero/"
Resource.PathTool.JointHead = Resource.PathTool.Root .. "heads/joint/"

--精灵头像
Resource.PathTool.SpiritHead = Resource.PathTool.UIRes .. "spirit_headicon/"

--精灵技能图标
Resource.PathTool.SpiritSkillIcon = Resource.PathTool.UIRes .. "spirit_skillicon/"

--精灵图鉴图标
Resource.PathTool.SpiritHandBookIcon = Resource.PathTool.UIRes .. "spirit_handbook_icon/"

--公会技能图标
Resource.PathTool.UnionSkillIcon = Resource.PathTool.UIRes .. "unionskill/"

--BGM & 音效
Resource.PathTool.BGM = Resource.PathTool.Root .. "sound/bgm/"
Resource.PathTool.SoundEffect = Resource.PathTool.Root .. "sound/effect/"

--章节图标
Resource.PathTool.UnionTranscriptChapterIcon = Resource.PathTool.UIRes .. "uniontranscriptchaptericon/"

--protobuf数据文件
Resource.PathTool.ProtobufData = Resource.PathTool.Root .. "pb/"

--获取文件名
function Resource.PathTool.GetFileName(path)
    local idx = path:match(".+()%.%w+$")
    if(idx) then
        return path:sub(1, idx-1)
    else
        return path
    end
end

--获取文件扩展名
function Resource.PathTool.GetExtension(path)
    return path:match(".+%.(%w+)$")
end

function Resource.PathTool.GetResPath(res_name)
	return Resource.PathTool.Root .. res_name
end

--渠道资源
function Resource.PathTool.GetTrdResPath(res_name)
	return Resource.PathTool.RootTrd .. res_name
end

--获取骨骼默认路径
function Resource.PathTool.GetSkeletonDefaultPath(name)
	name = name or "skeleton"
	return Resource.PathTool.Root .. string.format("default/%s.ssdz", name)
end

--获取UI布局文件路径
function Resource.PathTool.GetUILayoutPath(layout_name)
	return string.format("%s%s.csb", Resource.PathTool.UILayout, layout_name)
end

--获取UI资源路径
function Resource.PathTool.GetUIResPath(res_name, language_related, suffix)
	suffix = suffix or "sdz"

	local path
	if language_related then
		path = string.format("%slanguage/%s/%s", Resource.PathTool.UIRes, LanguageHelper.language, res_name)
	else
		path = string.format("%s%s", Resource.PathTool.UIRes, res_name)
	end
	path = Resource.PathTool.ConvertPathInExamine(path)
	return string.format("%s.%s", path, suffix)
end

--获取UI贴图集配置路径
function Resource.PathTool.GetUICnfPath(cnf_name, language_related)
	local path
	if language_related then
		path = string.format("%slanguage/%s/%s", Resource.PathTool.UIRes, LanguageHelper.language, cnf_name)
	else
		path = Resource.PathTool.UIRes .. cnf_name
	end
	return Resource.PathTool.ConvertPathInExamine(path)
end

--获取配置路径
function Resource.PathTool.GetCnfPath(cnf_name)
	if not Resource.PathTool.CnfPath and not Resource.PathTool.ZCnfPath then
		if StartConfig.IsBetaConfig then
			Resource.PathTool.CnfPath = string.format("%slanguage/%s/~config_beta/", Resource.PathTool.Root, LanguageHelper.district_language)
		else
			Resource.PathTool.CnfPath = string.format("%slanguage/%s/~config/", Resource.PathTool.Root, LanguageHelper.district_language)
		end
		Resource.PathTool.ZCnfPath = string.format("%slanguage/%s/config/", Resource.PathTool.Root, LanguageHelper.district_language)
	end

	if StartConfig.IsLocalRes == "true" and CPPSystemTools.system == CPPSystemTools.win32 then
		return Resource.PathTool.CnfPath .. cnf_name
	else
		return Resource.PathTool.ZCnfPath .. cnf_name
	end
end

--获取地图配置路径
function Resource.PathTool.GetMapCnfPath(map_id)
	local path = string.format("%s%d", Resource.PathTool.SceneMapCnf, map_id)
	return Resource.PathTool.ConvertPathInExamine(path)
end

--获取关卡路线配置路径
function Resource.PathTool.GetGameLevelLinePath(line_id)
	return string.format("%s%d.sdz", Resource.PathTool.DungeonLevelLine, line_id)
end

--获取地图Tile路径
function Resource.PathTool.GetMapSlicePath(slice_path, slice_name)
	--TODO: 使用正式地图资源
	local path = string.format("%s%s%s.sdz", Resource.PathTool.Root, slice_path, slice_name)
	return Resource.PathTool.ConvertPathInExamine(path)
end

--获取模型路径
function Resource.PathTool.GetModelPath(model_type, model_id)
	return string.format("%s%s/%d.ssdz", Resource.PathTool.ModelRes, model_type, model_id)
end

--获取模型路径
function Resource.PathTool.GetModelPathEx(id, is_monster)
	if is_monster then
		--怪物、BOSS
		local config = CPPGameLib.GetConfig("Monster", id)
		if not config then
			return ""
		end

		if config.hero_id and config.hero_id > 0 then
			return Resource.PathTool.GetModelPathEx(config.hero_id)
		else
			return Resource.PathTool.GetModelPath(config.model_type, config.model_id)
		end
	else		
		local type, config = ObjIDTool.GetInfo(id)
		if not config then
			return ""
		end
		return Resource.PathTool.GetModelPath(config.model_type, config.model_id)
	end
end

--获取模型影子路径
function Resource.PathTool.GetModelShadowPath(res_name)
	res_name = res_name or "default"
	return string.format("%sshadow/%s.sdz", Resource.PathTool.ModelRes, res_name)	
end

--获取骨骼特效路径
function Resource.PathTool.GetSkeletonEffectPath(res_name)
	local path = string.format("%s%s", Resource.PathTool.SkeletonEffectPath, res_name)
	return Resource.PathTool.ConvertPathInExamine(path)	
end

--获取物品图标路径
function Resource.PathTool.GetItemPath(res_id)
	res_id = res_id or 0
	--多语言相关
	return string.format("%s%s/%d.sdz", Resource.PathTool.Items, LanguageHelper.language, res_id)
end

-- --获取装备图标路径
-- function Resource.PathTool.GetEquipPath(res_id)
-- 	res_id = res_id or 0
-- 	return string.format("%s%d.sdz", Resource.PathTool.Equip , res_id)
-- end

--获取大图标路径
function Resource.PathTool.GetBigIconPath(res_id)
	res_id = res_id or 0
	return string.format("%s%d.sdz", Resource.PathTool.BigIcon, res_id)	
end

--获取章节路径
function Resource.PathTool.GetChapterIconPath(res_id)
	res_id = res_id or 0
	return string.format("%s%d.sdz", Resource.PathTool.ChapterIcon, res_id)	
end

--获取头像路径
function Resource.PathTool.GetHeroHeadPath(res_id)
	res_id = res_id or 0
	if CPPSdkTools.IsInExamine() then
		local safe_hero_list = ConfigAdapter.Common.GetSafeHeroListForExamine() or Macros.Game.SafeHeroListForExamine

		--审核状态下，指向不侵权的英雄
		res_id = safe_hero_list[MathTool.GetRandom(#safe_hero_list)]
	end
	
	return string.format("%s%d.sdz", Resource.PathTool.HeroHead, res_id)
end
function Resource.PathTool.GetRoleHeadPath(res_id)
	return Resource.PathTool.GetHeadPathByObjID(res_id)
end
function Resource.PathTool.GetJointHeadPath(res_id)
	res_id = res_id or 0
	return string.format("%s%s.sdz", Resource.PathTool.JointHead, res_id)
end

--获取精灵头像路径
function Resource.PathTool.GetSpiritHeadPath(res_id , path )
	res_id = res_id or 0
	return string.format("%s%d.sdz", Resource.PathTool.SpiritHead .. (path or "") , res_id)
end

--获取精灵技能图标路径
function Resource.PathTool.GetSpiritSkillIconPath(res_id)
	res_id = res_id or 0
	return string.format("%s%d.sdz", Resource.PathTool.SpiritSkillIcon, res_id)
end

--获取精灵图鉴图标路径
function Resource.PathTool.GetSpiritHandBookIconPath(res_id)
	res_id = res_id or 0
	return string.format("%s%d.sdz", Resource.PathTool.SpiritHandBookIcon, res_id)
end

--获取公会技能图标
function Resource.PathTool.GetUnionSkillIconPath(res_id)
	res_id = res_id or 0
	return string.format("%s%d.sdz", Resource.PathTool.UnionSkillIcon, res_id)
end

--根据对象ID获取头像路径(只对英雄、怪物、BOSS生效)
function Resource.PathTool.GetHeadPathByObjID(id, is_monster)
	if is_monster then
		--怪物、BOSS
		local config = CPPGameLib.GetConfig("Monster", id)
		if not config then
			return ""
		end
		if config.hero_id and config.hero_id > 0 then
			return Resource.PathTool.GetHeadPathByObjID(config.hero_id)
		else
			return Resource.PathTool.GetHeroHeadPath(config.head_icon)
		end
	else		
		local type, config = ObjIDTool.GetInfo(id)
		if not config then
			return ""
		end
		if type == Macros.Game.ObjType.HERO then
			--英雄
			return Resource.PathTool.GetHeroHeadPath(config.head_icon)
		end
	end

	return ""
end

--获取系统该图标路径
function Resource.PathTool.GetSystemEntryIconPath(res_name)
	return Resource.PathTool.GetUIResPath(string.format("cityentry/%s", res_name))
end

--获取半身像图标路径
function Resource.PathTool.GetHalfHeadPath(res_name)
	return Resource.PathTool.GetResPath(string.format("half_heads/%s.sdz", res_name))
end

--获取剧情精灵路径
function Resource.PathTool.GetStorySpritePath(res_name)
	return Resource.PathTool.GetUIResPath(string.format("story_sprite/%s", res_name), true)	
end

--获取合击技能名图片路径
function Resource.PathTool.GetJointSkilNamePath(skill_id)
	return Resource.PathTool.GetUIResPath(string.format("skillname/%d", skill_id), true)
end

--获取背景音乐路径
function Resource.PathTool.GetBGMusicPath(res_name)
	return string.format("%s%s.sdzm", Resource.PathTool.BGM, res_name)
end

--获取音效路径
function Resource.PathTool.GetSoundEffectPath(res_name)
	return string.format("%s%s.sdzs", Resource.PathTool.SoundEffect, res_name)
end

--获取主线副本名字路径
function Resource.PathTool.GetMainDungeonNamePath(res_name)
	return Resource.PathTool.GetUIResPath(string.format("dungeon_main_name/%s", res_name), true)	
end

--获取英雄副本名字路径
function Resource.PathTool.GetHeroDungeonNamePath(res_name)
	return Resource.PathTool.GetUIResPath(string.format("dungeon_hero_name/%s", res_name), true)	
end

--获取技能图
function Resource.PathTool.GetBattleSpeakPath(res_name)
	return Resource.PathTool.GetUIResPath(string.format("battlespeak/%s", res_name), true)	
end

--获取军团副本名字路径
function Resource.PathTool.GetUnionTranscriptNamePath(res_name)
	return Resource.PathTool.GetUIResPath(string.format("uniontranscriptname/%s", res_name), true)	
end

--公会副本章节
function Resource.PathTool.GetUnionTranscriptChapterIconPath(res_id)
	res_id = res_id or 0
	return string.format("%s%d.sdz", Resource.PathTool.UnionTranscriptChapterIcon, res_id)	
end

--获取合击背景路径
function Resource.PathTool.GetSkillJointBGPath(res_name)
	return Resource.PathTool.GetUIResPath(string.format("skill_joint_bg/%s", res_name))
end

--获取称号图标
function Resource.PathTool.GetTitleIconPath(res_name , type_ )
	return Resource.PathTool.GetUIResPath( string.format("titleicon/%s", res_name) ,type_ )	
end

--获取限时整合
function Resource.PathTool.GetPromptPath(res_name)
	return Resource.PathTool.GetUIResPath( string.format("prompt/%s", res_name) ,true )	
end

--获取次日登陆
function Resource.PathTool.GetMorrowPath(res_name)
	return Resource.PathTool.GetUIResPath( string.format("morrow/%s", res_name) )	
end

--获取段位图标
function Resource.PathTool.GetLadderIconPath(res_name , type_ )
	return Resource.PathTool.GetUIResPath( string.format("laddericon/sp_season_rank_big_%s", res_name) , type_ )	
end

--获取段位图标
function Resource.PathTool.GetLadderWordartPath(res_name , type_ )
	return Resource.PathTool.GetUIResPath( string.format("laddericon/%s", res_name) , type_ )	
end

--获取protobuf数据文件路径
function Resource.PathTool.GetProtobufDataFilePath(file_name)
	return CPPViewHelper.GetResoursePath(string.format("%s%s.pb", Resource.PathTool.ProtobufData, file_name))
end

--审核状态下，转换文件路径
function Resource.PathTool.ConvertPathInExamine(path)
	if CPPSdkTools.IsInExamine() then
		local config_channel_examine_file = CPPGameLib.GetConfig("ChannelExamineFiles", CPPSdkTools.GetCPChannelType(), false) 
		if config_channel_examine_file then
			local file_map = config_channel_examine_file.file_map
			if not file_map then
				file_map = {}
				for _, v in ipairs(config_channel_examine_file.file_list) do
					file_map[v.src_file] = v.dst_file
				end
				config_channel_examine_file.file_map = file_map
			end

			return file_map[path] or path			
		end
	end
	return path
end

