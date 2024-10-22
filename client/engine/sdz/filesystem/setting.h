
#ifndef SETTING_H
#define SETTING_H

#include "fileasync.h"
#include "zqcommon/typedef.h"
#include <string>

#define SETTING_DEFAULT_FONT_NAME "DefaultFontName"

class Setting
{
public:
	static Setting *GetInstance()
	{
		static Setting instance;
		return &instance;
	}
	~Setting(){};

private:
	Setting():m_is_changed(false),m_last_save_time(0)
	{
		Load();
	}

public:
	void Save(unsigned int now_time);
	std::string Get(const std::string &key) const;
	void Set(const std::string &key, const std::string &value);

private:
	bool Load();

private:
	bool m_is_changed;									// 是否变更，作为判断是否保存的脏位
	unsigned int m_last_save_time;						// 上次保存时间

	GameType::StrKVMap m_setting_map;
};

#endif
