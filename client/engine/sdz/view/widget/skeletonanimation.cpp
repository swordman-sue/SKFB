#include "skeletonanimation.h"
#include "filesystem/filesystemex.h"
#include "zqcommon/logutil.h"
#include "luaengine/luaengine.h"

#include "view/skeletondatacache.h"

using namespace cocos2d;

float SkeletonAnimationEx::fps_time = 1.0f / 30.0f;

SkeletonAnimationEx::SkeletonAnimationEx(SkeletonData *skeleton_data)
:spine::SkeletonAnimation(skeleton_data->GetSkeletonData()), m_skeleton_data(skeleton_data),
m_flip_x(false),
m_flip_y(false),
m_sp_animation_state_data(NULL),
m_run_animation_ex(true),
m_run_animation(true),
m_animation_listener_fun_id(0),
m_highlight_enable(false),
m_grey_enable(false),
m_delta_time(fps_time),
m_shader_program(nullptr),
m_state_dirty(false)
{
	m_skeleton_data->retain();
}

SkeletonAnimationEx::~SkeletonAnimationEx()
{
	if (m_sp_animation_state_data)
	{
		spAnimationStateData_dispose(m_sp_animation_state_data);
	}

	if (m_skeleton_data)
	{
		m_skeleton_data->release();
	}

	ClearAnimationListener();
}

void SkeletonAnimationEx::update(float delta_time)
{
	if (m_run_animation && m_run_animation_ex)
	{
		m_delta_time += delta_time;
		
		if (m_delta_time < fps_time && !m_state_dirty)
			return;

		SkeletonRenderer::update(m_delta_time);

		m_delta_time *= _timeScale;
		spAnimationState_update(_state, m_delta_time);

		if (m_flip_x)
			_skeleton->flipX = !_skeleton->flipX;
		if (m_flip_y)
			_skeleton->flipY = !_skeleton->flipY;

		spAnimationState_apply(_state, _skeleton);
		spSkeleton_updateWorldTransform(_skeleton);

		if (m_flip_x)
			_skeleton->flipX = !_skeleton->flipX;
		if (m_flip_y)
			_skeleton->flipY = !_skeleton->flipY;

		m_delta_time = 0.0f;

		m_state_dirty = false;
	}

	UpdateEvent();
}

void SkeletonAnimationEx::UpdateEvent()
{
	if (m_animation_listener_fun_id == 0)
		return;

	if (m_callback_list.empty())
		return;

	EventInfoList tmp_list;
	m_callback_list.swap(tmp_list);
	for (EventInfoList::iterator iter = tmp_list.begin(); iter != tmp_list.end(); ++iter)
	{	
		if (iter->event_type == SP_ANIMATION_EVENT)
		{
			LuaEngine::GetInstance()->CallFunction(m_animation_listener_fun_id, "sisids", iter->animation_name.c_str(), iter->event_type, iter->event_name.c_str(), iter->int_value, iter->float_value, iter->string_value.c_str());
		}
		else
		{
			LuaEngine::GetInstance()->CallFunction(m_animation_listener_fun_id, "si", iter->animation_name.c_str(), iter->event_type);
		}
	}
}

void SkeletonAnimationEx::onEnter()
{
	spine::SkeletonAnimation::onEnter();
}

SkeletonAnimationEx* SkeletonAnimationEx::CreateWithFile(const char* skeletonDataFile, const char* atlasFile)
{
	SkeletonData *skeleton_data = SkeletonDataCache::GetInstance()->LoadSkeletonData(skeletonDataFile, atlasFile);

	if (skeleton_data == NULL || skeleton_data->GetSkeletonData() == NULL)
	{
		LogUtil::LogError("[SkeletonAnimationEx::CreateWithFile] error path %s, %s", skeletonDataFile, atlasFile);
		return NULL;
	}

	SkeletonAnimationEx* node = new SkeletonAnimationEx(skeleton_data);
	node->autorelease();

	return node;
}

SkeletonAnimationEx* SkeletonAnimationEx::CreateWithFile(const char* skeletonZipFile)
{
	SkeletonData *skeleton_data = SkeletonDataCache::GetInstance()->LoadSsdzSkeletonData(skeletonZipFile);

	if (skeleton_data == NULL || skeleton_data->GetSkeletonData() == NULL)
	{
		LogUtil::LogError("[SkeletonAnimationEx::CreateWithFile] error path %s", skeletonZipFile);
		return NULL;
	}

	SkeletonAnimationEx* node = new SkeletonAnimationEx(skeleton_data);
	node->autorelease();

	return node;
}

void SkeletonAnimationEx::ResetSkeletonData(SkeletonData *skeleton_data)
{
	if (skeleton_data == NULL)
		return;

	if (skeleton_data == m_skeleton_data)
	{
		return;
	}

	//记录替代动画前的执行动作和时间
	//std::string animation_name;
	//float animation_time;
	//bool loop;
	//spTrackEntry *track_entry = getCurrent();
	//if (track_entry)
	//{
	//	animation_name = track_entry->animation->name;
	//	animation_time = track_entry->time;
	//	loop = (bool)(track_entry->loop);
	//}

	spSkeletonData *sp_skeleton_data = skeleton_data->GetSkeletonData();
	if (SkeletonRenderer::_skeleton)
	{
		spSkeleton_dispose(SkeletonRenderer::_skeleton);
	}
	setSkeletonData(sp_skeleton_data, false);

	spAnimationStateData *animation_state_data = spAnimationStateData_create(sp_skeleton_data);
	setAnimationStateData(animation_state_data);
	if (m_sp_animation_state_data)
	{
		spAnimationStateData_dispose(m_sp_animation_state_data);
	}
	m_sp_animation_state_data = animation_state_data;

	if (m_skeleton_data)
	{
		m_skeleton_data->release();
	}
	m_skeleton_data = skeleton_data;
	m_skeleton_data->retain();

	//尝试恢复之前动画的执行动作和时间
	//if (track_entry)
	//{
	//	SetAnimation(0, animation_name.c_str(), loop);
	//	update(animation_time);
	//}
}

void SkeletonAnimationEx::ResetSkeleton(const char* skeletonDataFile, const char* atlasFile)
{
	SkeletonData *skeleton_data = SkeletonDataCache::GetInstance()->LoadSkeletonData(skeletonDataFile, atlasFile);

	if (skeleton_data == NULL || skeleton_data->GetSkeletonData() == NULL)
	{
		LogUtil::LogError("[SkeletonAnimationEx::SetSkeleton] error path %s, %s", skeletonDataFile, atlasFile);
		return;
	}

	ResetSkeletonData(skeleton_data);
}

void SkeletonAnimationEx::ResetSkeleton(const char* ssdz_file)
{
	SkeletonData *skeleton_data = SkeletonDataCache::GetInstance()->LoadSsdzSkeletonData(ssdz_file);

	if (skeleton_data == NULL || skeleton_data->GetSkeletonData() == NULL)
	{
		LogUtil::LogError("[SkeletonAnimationEx::ResetSsdzSkeleton] error path %s", ssdz_file);
		return;
	}

	ResetSkeletonData(skeleton_data);
}

void SkeletonAnimationEx::SetAnimation(int trackIndex, const char* name, bool loop)
{
	// 初始化数据层(spAnimationState_setAnimation)
	setAnimation(trackIndex, name, loop);

	// 初始化表现层(spSkeleton_setBonesToSetupPose、spSkeleton_setSlotsToSetupPose)
	setToSetupPose();

	m_state_dirty = true;
}

void SkeletonAnimationEx::AddAnimation(int trackIndex, const char* name, bool loop, float delay)
{
	addAnimation(trackIndex, name, loop, delay);
}

void SkeletonAnimationEx::SetMix(const char* fromAnimation, const char* toAnimation, float duration)
{
	setMix(fromAnimation, toAnimation, duration);
}

void SkeletonAnimationEx::SetToSetupPose()
{
	setToSetupPose();
}

void SkeletonAnimationEx::SetTimeScale(float var)
{
	setTimeScale(var);
}

void SkeletonAnimationEx::SetHighLight(bool high_light)
{
	if (m_highlight_enable == high_light)
		return;

	m_highlight_enable = high_light;
	
	if (m_highlight_enable)
	{
		if (!m_shader_program)
		{
			m_shader_program = getGLProgram();
		}
		setGLProgram(cocos2d::GLProgramCache::getInstance()->getGLProgram(cocos2d::GLProgram::SHADER_POSITION_TEXTURE_COLOR_BRIGHT_NO_MVP));
	}
	else
	{
		setGLProgram(m_shader_program);
		m_shader_program = nullptr;
	}
}

void SkeletonAnimationEx::SetGray(bool graying)
{
	if (m_grey_enable == graying)
		return;

	m_grey_enable = graying;

	if (m_grey_enable)
	{
		if (!m_shader_program)
		{
			m_shader_program = getGLProgram();
		}
		setGLProgram(cocos2d::GLProgramCache::getInstance()->getGLProgram(cocos2d::GLProgram::SHADER_POSITION_TEXTURE_COLOR_GRAY_NO_MVP));
	}
	else
	{
		setGLProgram(m_shader_program);
		m_shader_program = nullptr;
	}
}

void SkeletonAnimationEx::SetColor(unsigned char r, unsigned char g, unsigned char b)
{
	setColor(Color3B(r, g, b));
}

//设置翻转X
void SkeletonAnimationEx::SetFlipX(bool flip_x)
{
	m_flip_x = flip_x;
}

//设置翻转Y
void SkeletonAnimationEx::SetFlipY(bool flip_y)
{
	m_flip_y = flip_y;
}

//动作是否存在
bool SkeletonAnimationEx::IsAnimationExist(const char* name)
{
	spAnimation* animation = spSkeletonData_findAnimation(_skeleton->data, name);
	if (!animation)
		return false;

	return true;
}

//获取当前正在播放的动画
std::string SkeletonAnimationEx::GetPlayAnimationName()
{
	spTrackEntry* track_entry = getCurrent();
	if (track_entry == NULL || track_entry->animation == NULL)
		return "";

	return track_entry->animation->name;
}

//獲取當前正在播放動畫的時間
float SkeletonAnimationEx::GetPlayAnimationTime()
{
	spTrackEntry* track_entry = getCurrent();
	if (track_entry == NULL || track_entry->animation == NULL)
		return 0;

	return track_entry->animation->duration;
}

//强制定位到动画的播放时间
void SkeletonAnimationEx::SetToPlayTime(float play_time)
{
	update(play_time);
}

void SkeletonAnimationEx::SetSkin(const char* name)
{
	setSkin(name);
}

void SkeletonAnimationEx::SetAttachment(const char* slotName, const char* attachmentName)
{
	setAttachment(slotName, attachmentName);
}

float SkeletonAnimationEx::GetHeight()
{
	float height = getBoundingBox().size.height;
	return height;
}

float SkeletonAnimationEx::GetWidth()
{
	float width = getBoundingBox().size.width;
	return width;
}

float SkeletonAnimationEx::GetOriginX()
{
	return getBoundingBox().origin.x;
}

float SkeletonAnimationEx::GetOriginY()
{
	return getBoundingBox().origin.y;
}

void SkeletonAnimationEx::SetAnimationListener(GameType::UINT32 listener_fun_id)
{
	this->ClearAnimationListener();
	m_animation_listener_fun_id = listener_fun_id;
}

void SkeletonAnimationEx::ClearAnimationListener()
{
	if (m_animation_listener_fun_id != 0)
	{
		m_callback_list.clear();
		LuaEngine::GetInstance()->DelLuaFuncID(m_animation_listener_fun_id);
		m_animation_listener_fun_id = 0;
	}
}

cocos2d::Node* SkeletonAnimationEx::GetNodeBySlotName(const char* name)
{
	return this->getNodeBySlotName(name);
}

void SkeletonAnimationEx::SetSlotNodeOffSet(float x, float y)
{
	this->SetOffSet(cocos2d::Vec2(x, y));
}

void SkeletonAnimationEx::onAnimationStateEvent(int trackIndex, spEventType type, spEvent* event, int loopCount) {
	if (m_animation_listener_fun_id == 0)
		return;
	
	spTrackEntry* entry = spAnimationState_getCurrent(_state, trackIndex);
	const char* animationName = (entry && entry->animation) ? entry->animation->name : 0;
	GameType::UINT32 eventType = (GameType::UINT32) type;

	SkeletonAnimationEx::EventInfo vi;
	std::string animation_name(animationName);
	vi.animation_name = animation_name;
	vi.event_type = eventType;
	if (eventType == SP_ANIMATION_EVENT)
	{
		vi.int_value = event->data->intValue;
		vi.float_value = event->data->floatValue;
		std::string event_name(event->data->name);
		vi.event_name = event_name;
		if (event->data->stringValue != nullptr)
		{
			std::string string_value(event->data->stringValue);
			vi.string_value = string_value;
		}
	}
	m_callback_list.push_back(vi);
}
