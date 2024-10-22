
--[[
%% @module: 资源加载工具
%% @author: swordman sue
%% @created: 2016-07-06
--]]

Resource = Resource or {}

Resource.LoadTool = Resource.LoadTool or {}

local load_widget_image
local load_widget_image_real
local release_widget_image

--加载UI贴图集
Resource.LoadTool.LoadUITextureAtlas = function(tex_atlas_info, finished_callback)
	local function require_finished()
		local config_res_compose = GameConfig.ResCompose[tex_atlas_info.config_key]
		if not config_res_compose then
			LogTool.LogError("[Resource.LoadTool.LoadUITextureAtlas] 找不到对应资源配置 GameConfig.ResCompose[%s]", tostring(tex_atlas_info.config_key))
			return
		end

		local image_path = config_res_compose.Path
		image_path = string.gsub(image_path, "%.png", Macros.Global.image_suffix)
		image_path = string.gsub(image_path, "/language/.+/", "/language/"..LanguageHelper.language.."/")

		--兼容布局文件中贴图路径
		local clip_image_path = string.gsub(image_path, "/language/"..LanguageHelper.language.."/", "/language/chs/")

		local imagedata_list = {}
		for k, v in pairs(config_res_compose) do
			if(type(v) == "table") then
				local imagedata = CPPImageData()
				imagedata.image_path = tex_atlas_info.texture_path
				imagedata.clip_image_name = string.gsub(clip_image_path, Macros.Global.image_suffix, "/"..k..".png")
				imagedata.x = v[1]
				imagedata.y = v[2]
				imagedata.width = v[3]
				imagedata.height = v[4]
				table.insert(imagedata_list, imagedata)
			end
		end		

		--加载贴图和贴图集信息
		load_widget_image(tex_atlas_info.texture_path, imagedata_list, finished_callback)
	end
	--加载配置文件
	GlobalRequireToolBatch:LoadFiles(
		{file_list = {tex_atlas_info.config_path}, callback = require_finished})
end

--卸载UI贴图集
Resource.LoadTool.UnloadUITextureAtlas = function(tex_atlas_info)
	release_widget_image(tex_atlas_info.texture_path)
end

--获取贴图数据
Resource.LoadTool.GetImageData = function(tex_atlas_key, ImageDataName, language_related)
	local imagedata = CPPImageData()

	local tex_atlas_info = Resource.UITextureAtlas.GetTexAtlasInfo(tex_atlas_key, language_related)
	if not tex_atlas_info then		
		return imagedata
	end

	local config_key = tex_atlas_info.config_key
	if GameConfig.ResCompose[config_key] == nil or GameConfig.ResCompose[config_key][ImageDataName] == nil then
		LogTool.LogError("[Resource.LoadTool.GetImageData] 找不到对应资源配置 GameConfig.ResCompose[%s][%s]", tostring(config_key), tostring(ImageDataName))
		return imagedata
	end

	local config = GameConfig.ResCompose[config_key]
	local image_path = string.gsub(config.Path, "resource/", Resource.PathTool.Root)
	image_path = string.gsub(image_path, "%.png", Macros.Global.image_suffix)
	image_path = string.gsub(image_path, "/language/.+/", "/language/"..LanguageHelper.language.."/")

	--兼容布局文件中贴图路径
	local clip_image_path = string.gsub(image_path, "/language/"..LanguageHelper.language.."/", "/language/chs/")

	imagedata.image_path = tex_atlas_info.texture_path
	imagedata.clip_image_name = string.gsub(clip_image_path, Macros.Global.image_suffix, "/"..ImageDataName..".png")
	imagedata.scale_size = config.ScaleSize or 1.0
	imagedata.x = config[ImageDataName][1]
	imagedata.y = config[ImageDataName][2]
	imagedata.width = config[ImageDataName][3]
	imagedata.height = config[ImageDataName][4]
	imagedata.offset_x = config[ImageDataName][5] / 2
	imagedata.offset_y = -config[ImageDataName][6] / 2
	
	return imagedata
end

--异步加载骨骼资源
Resource.LoadTool.LoadSkeletonAsync = function(skeleton_path, skeleton_node, finish_load_func)
	AsyncLoadMgr.GetInstance():LoadSsdzSkeleton(skeleton_path, skeleton_node, finish_load_func)
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
load_widget_image = function(image_path, imagedata_list, finished_callback)
	Resource.LoadTool.widget_image_map = Resource.LoadTool.widget_image_map or {}

	local retain_info = Resource.LoadTool.widget_image_map[image_path]
	if retain_info then
		retain_info.count = retain_info.count + 1

		if finished_callback then
			if retain_info.is_finished then
				--加载完成回调
				finished_callback(image_path)
			else
				--缓存加载完成回调信息
				table.insert(retain_info.finished_callback_list, {func = finished_callback, param = image_path})
			end
		end
		return
	end

	retain_info = {count = 1, finished_callback_list = {{func = finished_callback, param = image_path}}}
	Resource.LoadTool.widget_image_map[image_path] = retain_info

	--异步加载
	CPPViewHelper.LoadAsyncImageResource(image_path, GlobalCallbackMgr:AddCallBackFunc(function()
		load_widget_image_real(image_path, imagedata_list)
	end))

	--超时处理
	retain_info.load_timeout_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()
			load_widget_image_real(image_path, imagedata_list)
		end), 4)

	-- LogTool.LogInfo("[Resource.LoadTool.LoadUITextureAtlas] load %s", image_path)
end

load_widget_image_real = function(image_path, imagedata_list)
	local retain_info = Resource.LoadTool.widget_image_map[image_path]
	if not retain_info or retain_info.is_finished then
		return
	end

	CPPViewHelper.LoadWidgetImageResource(image_path, imagedata_list)

	retain_info.is_finished = true

	if retain_info.load_timeout_timer then
		GlobalTimerQuest:CancelQuest(retain_info.load_timeout_timer)
		retain_info.load_timeout_timer = nil
	end

	--加载完成回调
	if retain_info.finished_callback_list then
		for _, callback_info in ipairs(retain_info.finished_callback_list) do
			if callback_info.func then
				callback_info.func(callback_info.param)
			end
		end
		retain_info.finished_callback_list = nil
	end
end

release_widget_image = function(image_path)
	Resource.LoadTool.widget_image_map = Resource.LoadTool.widget_image_map or {}

	local retain_info = Resource.LoadTool.widget_image_map[image_path]
	if not retain_info then
		return
	end
	retain_info.count = retain_info.count - 1

	if retain_info.count <= 0 then
		Resource.LoadTool.widget_image_map[image_path] = nil

		CPPViewHelper.ReleaseWidgetImageResource(image_path)

		-- LogTool.LogInfo("[Resource.LoadTool.UnloadUITextureAtlas] unload %s", image_path)
	end
end
