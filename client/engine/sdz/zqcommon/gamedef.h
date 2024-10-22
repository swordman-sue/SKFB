
#ifndef GAMEDEF_H
#define GAMEDEF_H

namespace GameType
{
	// 默认屏幕尺寸(win32)
	// 横屏:1136x640，竖屏：533x946
	const float SCREEN_WIDTH_DEF = 1136;
	const float SCREEN_HEIGHT_DEF = 640;

	// 默认设计分辨率尺寸
	// 横屏：1136x640，竖屏：640x1136
	const float SCREEN_DESIGN_WIDTH_DEF = 960;
	const float SCREEN_DESIGN_HEIGHT_DEF = 640;

	// 渲染层次
	enum RenderQueue
	{
		GRQ_MAX = 8888,				//层次上限
	};
}

#endif
