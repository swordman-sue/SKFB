
require "macros/setting"

Macros = Macros or {}

Macros.Global = Macros.Global or {}

--布尔值
Macros.Global.TRUE = 1
Macros.Global.FALSE = 0

--运行帧率
Macros.Global.FPS_Default = 30
Macros.Global.FPS_MainCity = 30
Macros.Global.FPS_Dungeon = 60

--普通背景图默认放大系数
Macros.Global.BG_SCALE_COE = 1.6

--默认设计分辨率
Macros.Global.DesignSizeDef = nil
--实际设计分辨率
Macros.Global.DesignSize = COCOSize()
--屏幕尺寸
Macros.Global.ScreenSize = COCOSize()
--IPhone安全区域
Macros.Global.IphoneSafeAreaRect = nil
--通用背景默认尺寸
Macros.Global.BGDefaultSize = COCOSize(768, 433)

--Z轴顶点位置
Macros.Global.z_order_top = 1024			
Macros.Global.z_order_bottom = -1024			

--超时时间
Macros.Global.HttpOutTime = 60*60000
Macros.Global.HttpOutTimeII = 15000
Macros.Global.HttpSuccessCode = 200

--压缩文件后缀名
Macros.Global.image_suffix = CPPViewHelper.GetImageSuffix()
Macros.Global.music_suffix = CPPViewHelper.GetMusicSuffix()
Macros.Global.sound_suffix = CPPViewHelper.GetSoundSuffix()
Macros.Global.skeleton_suffix = CPPViewHelper.GetSkeletonSuffix()

--配置表和资源表
GameConfig = GameConfig or {}
GameConfig.ResCompose = GameConfig.ResCompose or {}

--活动公告URL
Macros.Global.NOTICEURL	= nil
