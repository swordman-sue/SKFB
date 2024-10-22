
#include "setting.h"
#include "filesystem/filesystemex.h"
#include "zqcommon/xmlhelper.h"
#include "common/tinyxml/tinyxml.h"


bool Setting::Load()
{
	std::string file_full_name = FileSystemEx::GetInstance()->GetDownLoadPath() + "setting.xml";
	TiXmlDocument document;
	if (!document.LoadFile(file_full_name))
	{
		return false;
	}

	TiXmlElement* root_elem = document.RootElement();
	if (0 == root_elem)
	{
		return false;
	}

	TiXmlElement *cur_elem = root_elem->FirstChildElement("setting");
	if (0 == cur_elem)
	{
		return false;
	}

	while(0 != cur_elem)
	{
		std::string setting_name;
		std::string setting_value;
		if (!GetNodeAttr(cur_elem, "name", setting_name))
		{
			return false;
		}
		if (!GetNodeValue(cur_elem, setting_value))
		{
			cur_elem = cur_elem->NextSiblingElement();
			continue;
		}
		cur_elem = cur_elem->NextSiblingElement();
		m_setting_map.insert(std::make_pair(setting_name, setting_value));
	}

	return true;
}

std::string Setting::Get(const std::string &key) const
{
	GameType::StrKVMap_CIter iter = m_setting_map.find(key);
	if (iter != m_setting_map.end())
	{
		return iter->second;
	}
	return "";
}

void Setting::Set(const std::string &key, const std::string &value)
{
	GameType::StrKVMap_CIter iter = m_setting_map.find(key);
	if (iter == m_setting_map.end() || iter->second != value)
	{
		m_setting_map[key] = value;
		m_is_changed = true;
	}
}

void Setting::Save(unsigned int now_time)
{
	if (!m_is_changed || m_last_save_time+1000>now_time)
	{
		return;
	}

	FileSystemEx::GetInstance()->WriteSettingFileAsync("setting.xml", m_setting_map);

	m_is_changed = false;
	m_last_save_time = now_time;
	return;
}