#include "gamelayer.h"
#include <typeinfo>
#include "base/CCDirector.h"
#include "luaengine/luaengine.h"
#include "zqcommon/logutil.h"

using namespace cocos2d;

GameLayer::GameLayer()
:m_pre_point(Point::ZERO), 
m_lua_down_fun_id(0), 
m_lua_move_fun_id(0), 
m_lua_up_fun_id(0),
m_scissor_x(0),
m_scissor_y(0), 
m_scissor_width(0),
m_scissor_height(0), 
m_update_fun_id(0),
m_listener_id(0),
m_swallow(true), 
m_scissor_type(NONE_SCISSOR),
m_listener(nullptr),
m_down_callback_fun(nullptr),
m_move_callback_fun(nullptr),
m_up_callback_fun(nullptr),
m_is_scirrsor(true)
{}

//節點加入父節點
void GameLayer::onEnter()
{
	Layer::onEnter();
	setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	//setTouchEnabled(true);
}

void GameLayer::onExit()
{
	setTouchEnabled(false);
	unschedule(schedule_selector(GameLayer::Update));
	Layer::onExit();
}

GameLayer::~GameLayer()
{
	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_down_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_move_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_up_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_update_fun_id);

	m_listener = nullptr;
	m_down_callback_fun = nullptr;
	m_move_callback_fun = nullptr;
	m_up_callback_fun = nullptr;
}

//创建一个空层
GameLayer* GameLayer::Node()
{
	GameLayer* layer = new GameLayer();
	if (layer && layer->init())
	{
		layer->setContentSize(Size(0, 0));
		layer->autorelease();
		return layer;
	}
	else
	{
		CC_SAFE_DELETE(layer);
		return nullptr;
	}
}
 
//添加控件到层里
void GameLayer::AddChild(cocos2d::Node *child)
{
	if(child)
	{
		AddChild(child, 0, 0);
	}
}

void GameLayer::AddChild(cocos2d::Node * child, ZQ::INT32 order, ZQ::INT32 tag)
{
	if(child)
	{
		addChild(child, order, tag);
	}
}

//移除子控件
void GameLayer::RemoveChild(ZQ::INT32 tag, bool is_clean)
{
	Layer::removeChildByTag(tag, is_clean);
}

//移除子控件
void GameLayer::RemoveChild(cocos2d::Node *node, bool is_clean)
{
	Layer::removeChild(node, is_clean);
}

//移除全部的子控件
void GameLayer::RemoveAllChild(bool is_clean)
{
	Layer::removeAllChildrenWithCleanup(is_clean);
}

//从父层上移除自己
void GameLayer::RemoveSelf(bool is_clean)
{
	if (this->_parent)
	{
		Layer::removeFromParentAndCleanup(is_clean);
	}
}

//根据坐标对子节点进行深度排序
void GameLayer::SortAllChildByPos(ZQ::UINT8 max_scene_width_power, ZQ::UINT8 max_scene_height_power)
{
	ZQ::INT16 max_scene_width = 2 << (max_scene_width_power - 1);
	ZQ::INT16 max_scene_height = 2 << (max_scene_height_power - 1);

	ZQ::INT32 zorder;
	for (const auto child : _children)
	{
		if (child)
		{
			zorder = GetZOrderByPos(child->getPosition(), max_scene_width, max_scene_height, max_scene_width_power);
			child->setLocalZOrder(zorder);
		}
	}
}

//设置坐标
void GameLayer::SetPosition(float x, float y)
{
	setPosition(x, y);
}

//设置是否显示
void GameLayer::SetVisible(bool visible)
{
	setVisible(visible);
}

bool GameLayer::onTouchBegan(Touch* touch, Event* event)
{
	//必须在自己的有效显示区内
	if (!IsValidTouchPos(touch) || !isVisible() || !HasVisibleParents())
		return false;

	//必须点击在父窗口的有效区内才生效
	cocos2d::Node *parent = Layer::getParent();
	if(parent && typeid(*parent) == typeid(GameLayer))
	{
		GameLayer* parent_layer = dynamic_cast<GameLayer*>(parent);
		if(!parent_layer->IsValidTouchPos(touch))
		{
			return false;
		}
	}

	//从屏幕坐标转换成目前的opengl坐标
	Point current_touch_point = touch->getLocation();
	current_touch_point = convertToNodeSpace(current_touch_point);

	//查看落点是否在停留在层上
	if(!m_touch_rect.containsPoint(current_touch_point))
		return false;

	m_pre_point = current_touch_point;
	
	//lua传递
	LuaEngine::GetInstance()->CallFunction(m_lua_down_fun_id, "dd", current_touch_point.x, current_touch_point.y);

	if(m_listener && m_down_callback_fun)
	{
		(m_listener->*m_down_callback_fun)(current_touch_point.x, current_touch_point.y);
	}
	return true;
}

//触摸取消
void GameLayer::onTouchCancelled(Touch *touch, Event *event)
{
	Layer::onTouchCancelled(touch, event);
}

//触摸点拖动
void GameLayer::onTouchMoved(Touch* touch, Event* event)
{
	//从屏幕坐标转换成目前的opengl坐标
	Point current_touch_point = touch->getLocation();
	current_touch_point = convertToNodeSpace(current_touch_point);

	//查看落点是否在停留在层上
	int in_rect = m_touch_rect.containsPoint(current_touch_point) ? 1:0;

	//lua传递
	LuaEngine::GetInstance()->CallFunction(m_lua_move_fun_id, "ddi", current_touch_point.x, current_touch_point.y, in_rect);

	if(m_listener && m_move_callback_fun)
	{
		(m_listener->*m_move_callback_fun)(current_touch_point.x, current_touch_point.y);
	}
}

//拖动结束
void GameLayer::onTouchEnded(Touch* touch, Event* event)
{
	//从屏幕坐标转换成目前的opengl坐标
	Point current_touch_point = touch->getLocation();
	current_touch_point = convertToNodeSpace(current_touch_point);

	//查看落点是否在停留在层上
	int in_rect = m_touch_rect.containsPoint(current_touch_point) ? 1:0;
	m_pre_point = current_touch_point;

	//lua传递
	LuaEngine::GetInstance()->CallFunction(m_lua_up_fun_id, "ddi", current_touch_point.x , current_touch_point.y, in_rect);

	if(m_listener && m_up_callback_fun)
	{
		(m_listener->*m_up_callback_fun)(current_touch_point.x, current_touch_point.y);
	}
}

void GameLayer::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
	if (!m_is_scirrsor)
	{
		Layer::visit(renderer, transform, flags);
		return;
	}
	if (m_scissor_type == NONE_SCISSOR)
	{
		Layer::visit(renderer, transform, flags);
		return;
	}

	_beforeVisitCmdScissor.init(_globalZOrder);
	_beforeVisitCmdScissor.func = CC_CALLBACK_0(GameLayer::onBeforeVisitScissor, this);
	renderer->addCommand(&_beforeVisitCmdScissor);

	Layer::visit(renderer, transform, flags);

	_afterVisitCmdScissor.init(_globalZOrder);
	_afterVisitCmdScissor.func = CC_CALLBACK_0(GameLayer::onAfterVisitScissor, this);
	renderer->addCommand(&_afterVisitCmdScissor);
}

void GameLayer::onBeforeVisitScissor()
{
	float x_pixel = 0, y_pixel = 0, width_pixel = 0, height_pixel = 0;
	float x, y, width, height;
	switch (m_scissor_type)
	{
	case SCREEN_SCISSOR:
		x_pixel = m_scissor_x;
		y_pixel = m_scissor_y;
		width_pixel = m_scissor_width;
		height_pixel = m_scissor_height;
		break;
	case LAYER_SCISSOR:
	{
		Point cur_pt = getPosition();
		Point start_pt = getParent()->convertToWorldSpace(Point(cur_pt.x + m_scissor_x, cur_pt.y + m_scissor_y));
		Point end_pt = getParent()->convertToWorldSpace(Point(cur_pt.x + m_scissor_x + m_scissor_width, cur_pt.y + m_scissor_y + m_scissor_height));
		x_pixel = start_pt.x;
		y_pixel = start_pt.y;
		width_pixel = end_pt.x - start_pt.x;
		height_pixel = end_pt.y - start_pt.y;
	}
		break;
	default:
		break;
	}

#if defined(__APPLE__) || defined(__ANDROID__)
	x = x_pixel;
	y = y_pixel;
	width = width_pixel;
	height = height_pixel;
#elif defined(WP8)
	cocos2d::CCSize win_size = cocos2d::Director::getInstance()->getWinSize();
	x = y_pixel;
	y = win_size.width - x_pixel - width_pixel;
	width = height_pixel;
	height = width_pixel;		
#else //(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	float scale_factor = Director::getInstance()->getOpenGLView()->getFrameZoomFactor();
	x = x_pixel * scale_factor;
	y = y_pixel * scale_factor;
	width = width_pixel * scale_factor;
	height = height_pixel * scale_factor;
#endif

	auto glview = Director::getInstance()->getOpenGLView();
	m_gl_is_scissor = glview->isScissorEnabled();
	m_scissor_rect = glview->getScissorRect();

	GLint sx, sy, tx, ty;
	if (m_gl_is_scissor)
	{
		sx = m_scissor_rect.origin.x > GLint(x) ? m_scissor_rect.origin.x : GLint(x);
		sy = m_scissor_rect.origin.y > GLint(y) ? m_scissor_rect.origin.y : GLint(y);
		tx = (m_scissor_rect.origin.x + m_scissor_rect.size.width) < GLint(x + width) ? (m_scissor_rect.origin.x + m_scissor_rect.size.width) : GLint(x + width);
		ty = (m_scissor_rect.origin.y + m_scissor_rect.size.height) < GLint(y + height) ? (m_scissor_rect.origin.y + m_scissor_rect.size.height) : GLint(y + height);
	}
	else
	{
		sx = GLint(x);
		sy = GLint(y);
		tx = GLint(x + width);
		ty = GLint(y + height);
	}

	if (tx - sx > 0 && ty - sy > 0)
	{
		glEnable(GL_SCISSOR_TEST);
		glview->setScissorInPoints(sx, sy, tx - sx, ty - sy);
	}
}

void GameLayer::onAfterVisitScissor()
{
	if (m_gl_is_scissor)
	{
		glEnable(GL_SCISSOR_TEST);

		auto glview = Director::getInstance()->getOpenGLView();
		glview->setScissorInPoints(m_scissor_rect.origin.x, m_scissor_rect.origin.y, m_scissor_rect.size.width, m_scissor_rect.size.height);
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
	}
}

//设置对位点
void GameLayer::SetAnchorPoint(float x, float y)
{
	CCLayer::setAnchorPoint(cocos2d::Vec2(x, y));
}

void GameLayer::SetTouchCallBack(cocos2d::Ref *listener, LAYER_CALLBACK down_callback_fun, LAYER_CALLBACK move_callback_fun, LAYER_CALLBACK up_callback_fun)
{
	m_listener = listener;
	m_down_callback_fun = down_callback_fun;
	m_move_callback_fun = move_callback_fun;
	m_up_callback_fun = up_callback_fun;
}

//注册lua触摸的回调
void GameLayer::SetLuaCallBack(ZQ::UINT32 down_call_fun, ZQ::UINT32 move_call_fun, ZQ::UINT32 up_call_fun)
{
	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_down_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_move_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_up_fun_id);

	m_lua_down_fun_id = down_call_fun;
	m_lua_move_fun_id = move_call_fun;
	m_lua_up_fun_id = up_call_fun;
}

void GameLayer::SetLuaUpdate(ZQ::UINT32 update_fun_id, float interval)
{
	if (0 != m_update_fun_id)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(m_update_fun_id);
	}
	m_update_fun_id = update_fun_id;

	SEL_SCHEDULE selector = schedule_selector(GameLayer::Update);
	if (0 != m_update_fun_id)
	{
		if (isScheduled(selector))
		{
			unschedule(selector);
		}
		schedule(selector, interval);
	}
	else
	{
		unschedule(selector);
	}
}

//设置裁剪区
void GameLayer::SetScissorSection(float x, float y, float width, float height)
{
	m_scissor_type = SCREEN_SCISSOR;
	m_scissor_x = x;
	m_scissor_y = y;
	m_scissor_width = width;
	m_scissor_height = height;

	if(m_scissor_width < 0)
	{
		m_scissor_width = 0;
		LogUtil::LogError("GameLayer::SetScissorSection error width:%d", m_scissor_width);
	}

	if(m_scissor_height < 0)
	{
		m_scissor_height = 0;
		LogUtil::LogError("GameLayer::SetScissorSection error height:%d", m_scissor_height);
	}	
}

//设置裁剪区By层的逻辑坐标
void GameLayer::SetScissorSectionByLayer(float x, float y, float width, float height)
{
	m_scissor_type = LAYER_SCISSOR;
	m_scissor_x = x;
	m_scissor_y = y;
	m_scissor_width = width;
	m_scissor_height = height;

	if(m_scissor_width < 0)
	{
		m_scissor_width = 0;
		LogUtil::LogError("GameLayer::SetScissorSection error width:%d", m_scissor_width);
	}

	if(m_scissor_height < 0)
	{
		m_scissor_height = 0;
		LogUtil::LogError("GameLayer::SetScissorSection error height:%d", m_scissor_height);
	}
}

//设置层的触摸区
void GameLayer::SetTouchRect(float x, float y, float width, float height)
{
	m_touch_rect.origin.x = x;
	m_touch_rect.origin.y = y;
	m_touch_rect.size.width = width;
	m_touch_rect.size.height = height;
}

//重设zOrder坐标
void GameLayer::SetZOrder(ZQ::INT32 order)
{
	_localZOrder = order;
	if(Layer::getParent())
	{
		Layer::getParent()->reorderChild(this, _localZOrder);
	}
}

//设置倍数
void GameLayer::SetScaleFactor(float factor)
{
	Layer::setScale(factor);
}

void GameLayer::Update(float dt)
{
	LuaEngine::GetInstance()->CallFunction(m_update_fun_id, "d", dt);
}

//設置是否吞噬觸屏消息
void GameLayer::SetSwallow(bool swallow)
{
	if (m_swallow != swallow)
	{
		m_swallow = swallow;
		setSwallowsTouches(m_swallow);
	}
}

// 是否是有效触摸区
bool GameLayer::IsValidTouchPos(Touch *touch)
{
	//必须点击在父窗口的有效区内才生效
	cocos2d::Node *parent = Layer::getParent();
	if(parent && typeid(*parent) == typeid(GameLayer))
	{
		GameLayer* parent_layer = dynamic_cast<GameLayer*>(parent);
		if(!parent_layer->IsValidTouchPos(touch))
		{
			return false;
		}
	}

	if(!isVisible())
		return false;

	//从屏幕坐标转换成目前的opengl坐标
	Point touch_point = touch->getLocationInView();
	Point screen_touch_pos = Director::getInstance()->convertToGL(touch_point);
	Point layer_touch_pos = convertToNodeSpace(screen_touch_pos);
	Rect valid_rect = Rect(m_scissor_x, m_scissor_y, m_scissor_width, m_scissor_height);

	switch(m_scissor_type)
	{
	case NONE_SCISSOR:
		return true;
	case SCREEN_SCISSOR:
		return valid_rect.containsPoint(screen_touch_pos);
	case LAYER_SCISSOR:
		return valid_rect.containsPoint(layer_touch_pos);
	default:
		return false;
	}

	return false;
}

bool GameLayer::IsValidDrawRect(cocos2d::Rect *rect)
{
	//必须点击在父窗口的有效区内才生效
	cocos2d::Node *parent = CCLayer::getParent();
	if(parent && typeid(*parent) == typeid(GameLayer))
	{
		GameLayer* parent_layer = dynamic_cast<GameLayer*>(parent);
		if(!parent_layer->IsValidDrawRect(rect))
		{
			return false;
		}
	}

	if(!isVisible())
		return false;

	Rect valid_rect = Rect(m_scissor_x, m_scissor_y, m_scissor_width, m_scissor_height);

	switch(m_scissor_type)
	{
	case NONE_SCISSOR:
		return true;
	case SCREEN_SCISSOR:
		if(!valid_rect.intersectsRect(*rect))
			return false;
		break;
	case LAYER_SCISSOR:
		{
			Point left_bottom_pt = Point(rect->getMinX(), rect->getMinY());
			Point right_top_pt = Point(rect->getMaxX(), rect->getMaxY());
			left_bottom_pt = convertToNodeSpace(left_bottom_pt); 
			right_top_pt = convertToNodeSpace(right_top_pt);
			Rect target_rect = Rect(left_bottom_pt.x, left_bottom_pt.y, right_top_pt.x - left_bottom_pt.x, right_top_pt.y - left_bottom_pt.y);
			if(!valid_rect.intersectsRect(target_rect))
				return false;
		}
		break;
	default:
		return true;
	}

	return true;
}

float GameLayer::GetWidth()
{
	return Layer::getContentSize().width;
}

float GameLayer::GetHeight()
{
	return Layer::getContentSize().height;
}


//设置触摸等级
void GameLayer::SetTouchPriority(ZQ::INT32 priority)
{
	if(GetTouchPriority() == priority)
		return;

	//_globalZOrder = priority;
 }

//获取触摸等级
ZQ::INT32 GameLayer::GetTouchPriority()
{
	return _globalZOrder;
}

bool GameLayer::HasVisibleParents()
{
	auto parent = this->getParent();
	for (auto c = parent; c != nullptr; c = c->getParent())
	{
		if (!c->isVisible())
		{
			return false;
		}
	}
	return true;
}