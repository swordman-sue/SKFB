#ifndef IMAGE_SPRITE_H
#define IMAGE_SPRITE_H

#include "cocos2d.h"
#include <map>
#include <deque>
#include "zqcommon/typedef.h"
#include "view/struct.h"

class ImageData;

#define DEFAULT_SHADOW_MAX		8		//光影帧数
#define DEFAULT_SHADOW_TIME		1.0		//光影时间

//原点偏移
typedef struct origin_offset
{
	float offset_x;
	float offset_y;
	float scale_size;
	origin_offset():offset_x(0.0), offset_y(0.0), scale_size(1.0){}
}ORIGINOFFSET;

//精灵动画结构
typedef struct sprite_state 
{
	ZQ::INT32 frame_count;												//动画帧数
	float delay;														//每帧间隔
	cocos2d::Vector<cocos2d::SpriteFrame*> frame_array;										//动画数组
	std::vector<ORIGINOFFSET> offset_array;								//动画帧偏移数组
	sprite_state():frame_count(0),delay(0.0){} 
}SPRITESTATE;

//光影帧的动作
typedef struct shadow_infomation
{
	float					x, y;			//坐标
	ZQ::UINT32				sprite_state;	//精灵状态
	ZQ::UINT32				index;			//帧
	cocos2d::SpriteFrame	*frame;			//动画帧
	bool					flip;			//是否翻转
}SHADOWINFO;

//游戏中的精灵
class ImageSprite : public cocos2d::Sprite
{
public:
	enum SpriteEffect
	{
		EFFECT_NONE = 0,		//无效果
		EFFECT_HIGHLIGHT,		//高亮效果
		EFFECT_GREY,			//暗光效果
		EFFECT_SHADOW,			//光影效果
	};

	~ImageSprite();

	//  创建空的精灵
	static ImageSprite* Node();

	// 	使用图片创建精灵
	// 	In: image_file 图片路径 path 图片文件夹路径 ImageData 裁剪图数据
	//	In: state_index 状态标识符（一个精灵可能有各种状态 例如行走 站立等) 
	// 	In: frame_count 帧数
	// 	In：每帧播放间隔时间 1.0为一秒
	//从整张图片读取
	static ImageSprite* CreateWithImage(const std::string &image_file, ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);
	//从文件夹读取
	static ImageSprite* CreateWithImagePath(const std::string &path, ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);
	//从拼图读取
	static ImageSprite* CreateWithImageData(const ImageData &image_data, ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);

	//	为精灵增加一个状态
	// 	In: image_file 图片路径 image_data 图片区域数据 path 图片文件路径 
	//	In: state_index 状态标识符（一个精灵可能有各种状态 例如行走 站立等) 
	// 	In: frame_count 帧数
	// 	In：每帧播放间隔时间 1.0为一秒
	bool AddSpriteStateImage(const std::string &image_file, ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);
	bool AddSpriteStatePath(const std::string &path, ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);
	bool AddSpriteStateData(const ImageData &image_data, ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);

	//增加一个空状态并使用拼图加入状态帧
	bool AddSpriteState(ZQ::INT32 state_index, ZQ::INT32 frame_count = 1, float delay = 0.0);
	bool AddSpriteStateIndex(const ImageData &image_data, ZQ::INT32 state_index);

	//	设置精灵的状态
	//	In: state_index 状态标识符（一个精灵可能有各种状态 例如行走 站立等) 
	void SetSpriteState(ZQ::INT32 state_index);
	//	获取精灵的状态
	ZQ::INT32 GetSpriteState();

	// 设置当前的播放帧
	void SetFrameIndex(unsigned int frame_index);

	//	获取当前的播放帧
	int GetFrameIndex(){return m_frame_index;}

	virtual void setPosition(const cocos2d::Vec2& pos);

	//设置精灵的位置
	//In: x, y 控件的坐标
	void SetPosition(float x, float y);

	//获取是否播放过一遍
	bool IsPlayOnce(){return m_is_play_once;}

	//设置是否水平翻转
	//In: flip 是否水平翻转
	void SetFlipX(bool flip);

	//设置是否水平翻转
	//In: flip 是否纵向翻转
	void SetFlipY(bool flip);

	//设置是否可见
	//visible 精灵动画是否可见
	void SetVisible(bool visible);

	//设置ZOrder
	//In:order 控件遮掩排序的编号
	void SetZOrder(ZQ::INT32 order);
	
	//设置加亮效果
	void SetHighLight(bool high_light);
	void SetBright(float value);

	//设置变暗效果
	void SetGray(bool is_grey);

	//设置光影效果
	void SetShadow(bool shadow);
	//设置光影效果拖影长度和间隔
	//shadow_time 光影时间
	//frame_count 光影帧数
	void SetShadowInfo(float shadow_time, float frame_count);
	//增加光影偏移量
	void AddShadowOffset(float x, float y);

	//设置是否自动更新
	void SetUpdate(bool is_update){ m_is_update = is_update;}

	//设置更新
	virtual void visit();

	//设置对位点
	void SetAnchorPoint(float x, float y);

	//设置缩放倍数
	virtual void setScale(float scale_size);
	virtual void setScaleX(float x_scale_size);
	virtual void setScaleY(float y_scale_size);
	void ResetStateScaleFactor(float state_scale);

	//获取宽高
	float GetWidth();
	float GetHeight();

	//设置播放完一遍后的回调
	void SetLuaPlayOnceFun(ZQ::UINT32 lua_fun_id){m_lua_playonce_fun_id = lua_fun_id;}

	//查询精灵的状态是否存在
	bool IsSpriteStateExist(ZQ::INT32 state_index);

	// 清空某一个状态下的动画帧
	void ClearSpriteStateIndex(ZQ::INT32 state_index);

	// 重设动作频率
	void ResetUpdateDelay(ZQ::INT32 state_index, float delay_time);

	virtual void onDraw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

protected:
	void Update(float time);
	void TrackShadow(float time);

protected:
	std::map<ZQ::INT32, SPRITESTATE>		m_sprite_map;				//存放各种状态动画的映射表
	ZQ::INT32								m_state;					//精灵的状态
	ZQ::UINT32								m_frame_index;				//当前播放帧
	SPRITESTATE								m_sprite_date;				//精灵状态数据
	bool									m_is_play_once;				//是否播放过一次
	bool									m_is_update;				//是否自动更新
	bool									m_is_filp_x;				//是否水平翻转
	bool									m_is_flip_y;				//是否纵向翻转
	ZQ::UINT32								m_lua_playonce_fun_id;		//播放一遍后调用的函数
	float									m_x, m_y;					//当前位置
	float									m_dx, m_dy;					//偏移位置
	SpriteEffect							m_effect;					//特效
	std::deque<SHADOWINFO>					m_shadow_queue;				//光影列表
	float									m_shadow_max;				//光影帧数量
	float									m_shadow_time;				//光影帧密度
	float									m_scale_x;					//x轴缩放倍数
	float									m_scale_y;					//y轴缩放倍数
	float									m_state_scale_size;			//当前状态的缩放帧
	bool									m_highlight_enable;
	float									m_highlight_value;
	cocos2d::GLProgram*						m_shader_program;

private:
	ImageSprite();
	void ResetOffsetPos();
};

#endif

