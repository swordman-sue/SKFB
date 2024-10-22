
LanguageHelper = LanguageHelper or {}

--自动识别区域语言
function LanguageHelper.AutoDistrictLanguage()
	if StartConfig.AutoDistrictLanguage ~= "true" then
		return
	end

    local country_code = CPPSystemTools.GetCountryCodeEx()
    local district_language_map = 
    {
        ["CN"] = "dl_chs",
        ["SG"] = "dl_chs",
        ["MY"] = "dl_chs",
        ["HK"] = "gt_cht",
        ["TW"] = "gt_cht",
        ["MO"] = "gt_cht",
    }
    local district_language = district_language_map[country_code] or StartConfig.DistrictLanguage
    CPPResManager:SetSetting(Macros.Setting.DistrictLanguage, district_language)
end

--初始化区域语言
function LanguageHelper.InitLanguage()	
	LanguageHelper.AutoDistrictLanguage()

	LanguageHelper.district_language = CPPResManager:GetSetting(Macros.Setting.DistrictLanguage)
	local list = CPPGameLib.Split(LanguageHelper.district_language, "_")
	LanguageHelper.district = list[1]
	LanguageHelper.language = list[2]

	--语言相关文件的后缀（部分文件：粒子特效、骨骼特效）
	LanguageHelper.language_chs = "_language_chs"
	LanguageHelper.language_tag = "_language_"..LanguageHelper.language

	--语言文件转换表
	LanguageHelper.language_trans_file_map = {}

    --TEMP：统一缩放文字
    if LanguageHelper.district_language == "dl_wy" then
        StartConfig.FontSizeScaleFactor = 0.8
    end
end

--是否简体中文
function LanguageHelper.IsChs()
    return LanguageHelper.language == "chs"
end

--转换语言文件名
function LanguageHelper.TransFileName(res_name, suffix)
	if not CPPGameLib.StrEndWith(res_name, suffix) then
		res_name = res_name..suffix
	end

	if LanguageHelper.IsChs() then
		return res_name
	end

	if LanguageHelper.language_trans_file_map[res_name] then
		return LanguageHelper.language_trans_file_map[res_name]
	end

	local src = LanguageHelper.language_chs..suffix
	local tgt = LanguageHelper.language_tag..suffix
	local ret, new_res_name = CPPGameLib.StrEndReplaceWith(res_name, src, tgt)
	if ret then
		LanguageHelper.language_trans_file_map[res_name] = new_res_name
	end
	return new_res_name
end

