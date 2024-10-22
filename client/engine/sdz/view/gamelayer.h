#ifndef GAME_LAYER_H
#define GAME_LAYER_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"

#define LAYER_TOUCH_PRIORITY	-99

// 按钮下按回调函数 
// out: 触摸时的屏幕坐标
typedef void (cocos2d::Object::*LAYER_CALLBACK)(float, float);

class GameLayer : public cocos2d::Layer
{
public:
	enum ScissorType{
		NONE_SCISSOR = 0,			//无裁剪区
		SCREEN_SCISSOR,				//相对于屏幕裁剪
		LAYER_SCISSOR				//相对于层裁剪				
	};

	~GameLayer();

	//创建一个空层
	static GameLayer* Node();

	//设置是否显示
	//In: visible 是否显示本层
	void SetVisible(bool visible);

	//设置坐标
	//In: x y控件位置 以左下角对位
	void SetPosition(float x, float y);

	//设置层的触摸区
	void SetTouchRect(float x, float y, float width, float height);

	//设置裁剪是否生效
	void SetIsScissor(bool is_scissor){m_is_scirrsor = is_scissor;}
	bool GetIsScissor() {return m_is_scirrsor;}
	//设置裁剪区
	void SetScissorSection(float x, float y, float width, float height);
	//设置裁剪区By层的逻辑坐标
	void SetScissorSectionByLayer(float x, float y, float width, float height);

	//添加控件到层里
	//In: child新增到层的控件
	//order:层的绘制顺序，order大的会挡住order小的
	//tag:层的标示号
	void AddChild(cocos2d::Node *child);
	void AddChild(cocos2d::Node *child, ZQ::INT32 order, ZQ::INT32 tag);

	//移除子控件
	//tag：子控件的标识
	//is_clean: 是否清除
	void RemoveChild(ZQ::INT32 tag, bool is_clean = true);
	void RemoveChild(cocos2d::Node *node, bool is_clean = true);

	//移除全部的子控件
	void RemoveAllChild(bool is_clean = true);

	//从父层上移除自己
	void RemoveSelf(bool is_clean = true);

	//根据坐标对子节点进行深度排序
	void SortAllChildByPos(ZQ::UINT8 max_scene_width_power, ZQ::UINT8 max_scene_height_power);

	//设置ZOrder
	//In:order 控件遮掩排序的编号
	void SetZOrder(ZQ::INT32 order);

	//设置倍数
	void SetScaleFactor(float factor);

	//父节点每帧自动调用，用以遍历它的子节点并绘制
	virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);

	void Update(float dt);

	//加入父節點時調用
	virtual void onEnter();
	virtual void onExit();

	//设置对位点
	void SetAnchorPoint(float x, float y);

	//设置是否吞噬触屏消息
	void SetSwallow(bool swallow);
	void SetTouchCallBack(cocos2d::Ref *listener, LAYER_CALLBACK down_callback_fun, LAYER_CALLBACK move_callback_fun, LAYER_CALLBACK m_up_callback_fun);

	//  Lua版本的触摸回调函数
	void SetLuaTouchDown(ZQ::UINT32 down_call_fun_id){m_lua_down_fun_id = down_call_fun_id;}
	void SetLuaTouchMove(ZQ::UINT32 move_call_fun_id){m_lua_move_fun_id = move_call_fun_id;}
	void SetLuaTouchUp(ZQ::UINT32 up_call_fun_id){m_lua_up_fun_id = up_call_fun_id;}
	void SetLuaCallBack(ZQ::UINT32 down_call_fun_id, ZQ::UINT32 move_call_fun_id, ZQ::UINT32 up_call_fun_id);
	void SetLuaUpdate(ZQ::UINT32 update_fun_id, float interval = 0.0f);

	//是否是有效触摸区
	bool IsValidTouchPos(cocos2d::Touch* touch);

	//是否是有效绘制区
	bool IsValidDrawRect(cocos2d::Rect *rect);
	void SetOnListener(ZQ::UINT32 listener_id){ m_listener_id = listener_id; };

	//获取层的宽高
	float GetWidth();
	float GetHeight();

	//设置触摸等级
	void SetTouchPriority(ZQ::INT32 priority);
	//获取触摸等级
	ZQ::INT32 GetTouchPriority();

	//是否有可见父节点
	bool HasVisibleParents();

protected:
	void onBeforeVisitScissor();
	void onAfterVisitScissor();
	//继承自CCLayer, 处理触摸事件
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);

protected:
	GameLayer();
	bool IsListenerTouch();
	bool IsContainTouchLayer();
	inline ZQ::INT32 GetZOrderByPos(
		const cocos2d::Vec2& pos, ZQ::INT16 max_scene_width, ZQ::INT16 max_scene_height, ZQ::UINT8 max_scene_width_power)
	{
		float x = MIN(pos.x, max_scene_width); 
		x = MAX(-max_scene_width, x);
		float y = MIN(pos.y, max_scene_height); 
		y = MAX(-max_scene_height, y);
		ZQ::INT32 zorder = ((max_scene_height - (int)y) << max_scene_width_power) + (max_scene_width - (int)x);
		return zorder;
	}

private:
	cocos2d::Point		m_pre_point;			//触摸屏拖动的上一个位置
	ZQ::UINT32			m_lua_down_fun_id;		//触摸点停留在在层上时的回调
	ZQ::UINT32			m_lua_move_fun_id;		//触摸点在层上滑动的回调
	ZQ::UINT32			m_lua_up_fun_id;		//触摸点离开层时的回调
	ZQ::UINT32			m_update_fun_id;		//lua调用的更新函数
	ZQ::UINT32			m_listener_id;			//lua调用的更新函数
	cocos2d::Rect		m_touch_rect;			//触摸区
	ScissorType			m_scissor_type;			//裁剪的类型
	float				m_scissor_x;			//层的裁剪区
	float				m_scissor_y;			
	float				m_scissor_width;
	float				m_scissor_height;
	cocos2d::Rect m_scissor_rect;
	GLboolean m_gl_is_scissor;
	cocos2d::CustomCommand _beforeVisitCmdScissor;
	cocos2d::CustomCommand _afterVisitCmdScissor;

	bool				m_swallow;					//是否吞噬消息
	bool				m_is_scirrsor;				//裁剪区是否生效
	cocos2d::Ref*		m_listener;					//监听事件对象
	LAYER_CALLBACK		m_down_callback_fun;		//下按后的回调函数
	LAYER_CALLBACK		m_move_callback_fun;		//移动时的回调函数
	LAYER_CALLBACK		m_up_callback_fun;			//按住松开后的回调函数
};

#endif
