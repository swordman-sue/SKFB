#include "particlesystem.h"
#include "particledatamanager.h"

#include "render/renderer.h"
#include "render/emittedps.h"
#include "render/entity.h"

#include "particlefactory.h"
#include "particlehelper.h"
#include "particle.h"
#include "attribute.h"
#include "dynamicattribute.h"
#include "technique.h"
#include "simplespline.h"

#include "cocos/base/CCDirector.h"
#include "cocos/2d/CCActionCamera.h"
#include "cocos/2d/CCDrawingPrimitives.h"
#include "cocos/2d/CCNode.h"

#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "cocos/math/TransformUtils.h"

#define TIME_OF_EACH_FRAME 0.033333f 

NS_CC_BEGIN

const float MyParticleSystem::DEFAULT_CYCLETIME = 3.0;

MyParticleSystem::MyParticleSystem(const std::string& name):
m_ps_name(name),
m_is_template(false),
m_is_cycle(true),
m_cycle_total_time(DEFAULT_CYCLETIME),
m_listener(0),
m_selector(0),
m_fScaleZ(1.0f),
m_scale_time(1.0f),
m_scale_speed(1.0f),
m_enabled(true),
m_is_flip_x(false),
m_is_flip_y(false),
m_live_time(0.0f),
m_pre_time(0.0f),
m_skip_time(0),
m_is_need_pre_time(false),
m_show_aabb(false),
m_manual_update(false),
m_screen_short(false),
m_screen_short_num(0),
m_screen_short_cur_num(0),
m_inverted(true),
m_update_delta_time(TIME_OF_EACH_FRAME),
m_curr_delta_time(0.0f),
m_frame_pre_second(30),
m_async_load_id(0)
{
	m_custom_rect.setRect(-64, -64, 128, 128);
	setAnchorPoint(CCPoint(0.5,0.5));
	m_clipper_rect.size.width = 28;
	m_clipper_rect.size.height = 28;
}

MyParticleSystem::~MyParticleSystem()
{
	m_listener = 0;
	m_selector = 0;

	if (!m_is_template)
	{
		unscheduleUpdate();
	}
	DestoryClippingNode();
	DestroyAllTechniques();

	if (0 != m_async_load_id)
	{
		PSManager::GetInstance()->AbortTemplateAsync(m_async_load_id);
	}
}

MyParticleSystem& MyParticleSystem::operator=(const MyParticleSystem& ps)
{
	DestoryClippingNode();
	DestroyAllTechniques();
	
	if (ps.m_clippers.size() > 0)
	{
		m_clipper_rect = ps.m_clipper_rect;
		m_clipper_pos = ps.m_clipper_pos;
		m_clipper_technique_list = ps.m_clipper_technique_list;
		m_inverted = ps.m_inverted;
	}
	
	for (size_t i = 0 ; i < ps.GetNumTechniques(); ++i)
	{
		CCParticleTechnique *technique = ps.GetTechnique(i);
		CreateTechnique(technique);
	}

	m_ps_name = ps.m_ps_name;
	SetCycle(ps.m_is_cycle);
	SetCycleTotalTime(ps.m_cycle_total_time);
	SetScale(ps._scaleX, ps._scaleY, ps.m_fScaleZ);
	setPosition(CCPoint(ps._position.x,ps._position.y));
	SetPreTime(ps.m_pre_time);
	SetFramePreSecond(ps.m_frame_pre_second);

	m_show_aabb = ps.m_show_aabb;
	m_custom_rect = ps.m_custom_rect;
	m_scale_speed = ps.m_scale_speed;
	m_scale_time = ps.m_scale_time;
	m_manual_update = ps.m_manual_update;

	m_live_time = 0.0f;

	if (!m_is_template)
	{
		scheduleUpdateWithPriority(1);
	}

	return *this;
}

MyParticleSystem* MyParticleSystem::Create()
{
	MyParticleSystem* ps = new MyParticleSystem(DEFAULT_EMPTY_PARTICLE_SYSTEM_NAME);
	ps->autorelease();
	return ps;
}

MyParticleSystem* MyParticleSystem::Create(const char* template_name)
{
	MyParticleSystem* template_ps = PSManager::GetInstance()->CreateTemplate(template_name);
	if (nullptr == template_ps)
	{
		log("[MyParticleSystem::Create] Error %s", template_name);
		return nullptr;
	}
	return CopyData(template_ps);
}

MyParticleSystem* MyParticleSystem::CreateAsync(const char* template_name)
{
	MyParticleSystem* new_ps;
	MyParticleSystem* template_ps = PSManager::GetInstance()->GetTemplate(template_name);
	if (nullptr == template_ps)
	{
		template_ps = PSManager::GetInstance()->CreateTemplate(EMPTY_PARTICLE);
		if (nullptr == template_ps)
		{
			log("Create CCMyParticleSystem Error %s", EMPTY_PARTICLE);
			return nullptr;
		}
		new_ps = CopyData(template_ps);

		//异步加载特效模板
		PSManager::GetInstance()->CreateTemplateAsync(template_name, new_ps);
	}
	else
	{
		new_ps = CopyData(template_ps);
	}
	return new_ps;
}

MyParticleSystem* MyParticleSystem::CopyData(MyParticleSystem* ps)
{
	if (nullptr == ps)
	{
		return nullptr;
	}

	std::string name = ps->GetTemplateName();
	MyParticleSystem *sys = new MyParticleSystem(ps->GetTemplateName());
	*sys = *ps;
	sys->autorelease();
	return sys;
}

bool MyParticleSystem::InitWithTemplateName(const char* template_name)
{
	MyParticleSystem *ps = PSManager::GetInstance()->CreateTemplate(template_name);
	if (nullptr == ps)
	{
		log("[MyParticleSystem::InitWithTemplateName] Error %s", template_name);
		return false;
	}

	if (!m_is_template)
	{
		unscheduleUpdate();
	}
	*this = *ps;
	return true;
}

void MyParticleSystem::OnTemplateAsyncLoadFinished(const char* template_name)
{
	MyParticleSystem* templ_ps = PSManager::GetInstance()->GetTemplate(template_name);
	if (nullptr == templ_ps)
	{
		log("[MyParticleSystem::OnTemplateAsyncLoadFinished] Error %s", template_name);
		return;
	}

	if (!m_is_template)
	{
		unscheduleUpdate();
	}
	*this = *templ_ps;

	if (!m_async_cache_set.empty())
	{
		ParticleSysAsyncCacheSetCIter it;
		ParticleSysAsyncCacheSetCIter ie = m_async_cache_set.end();
		for (it = m_async_cache_set.begin(); it != ie; ++it)
		{
			if (*it == "is_cycle")
			{
				SetCycle(m_async_cache_info.is_cycle);
			}
			else if (*it == "scale_time")
			{
				SetScaleTime(m_async_cache_info.scale_time);
			}
			else if (*it == "scale")
			{
				SetScale(m_async_cache_info.scale_x, m_async_cache_info.scale_y, m_async_cache_info.scale_z);
			}
			else if (*it == "flip_x")
			{
				SetFlipX(m_async_cache_info.flip_x);
			}
			else if (*it == "flip_y")
			{
				SetFlipY(m_async_cache_info.flip_y);
			}
			else if (*it == "manual_update")
			{
				SetManualUpdate(m_async_cache_info.manual_update);
			}
			else if (*it == "frame_num")
			{
				SetFramePreSecond(m_async_cache_info.frame_num);
			}
			else if (*it == "is_grey")
			{
				MakeGrey(m_async_cache_info.is_grey);
			}
		}
		m_async_cache_set.clear();
	}
}

void MyParticleSystem::SetPosition(float x, float y, float z)
{
	setPosition(Vec2(x, y));
	_positionZ = z;
}

void MyParticleSystem::SetScale(float scale_x, float scale_y, float scale_z)
{
	if (0 != m_async_load_id)
	{
		m_async_cache_set.insert("scale");
		m_async_cache_info.scale_x = scale_x;
		m_async_cache_info.scale_y = scale_y;
		m_async_cache_info.scale_z = scale_z;
		return;
	}

	float to_scale_x = scale_x;
	float to_scale_y = scale_y;
	if (m_is_flip_x)
	{
		to_scale_x = -scale_x;
	}
	if (m_is_flip_y)
	{
		to_scale_y = -scale_y;
	}
	m_fScaleZ = scale_z;
	setScaleX(to_scale_x);
	setScaleY(to_scale_y);
}

void MyParticleSystem::SetCycle(bool cycle)
{ 
	if (0 != m_async_load_id)
	{
		m_async_cache_set.insert("is_cycle");
		m_async_cache_info.is_cycle = cycle;
		return;
	}

	m_is_cycle = cycle; 
	if (cycle)
	{
		SetEnabled(true);
	}
}

void MyParticleSystem::SetFlipX(bool is_flip_x)
{
	if (0 != m_async_load_id)
	{
		m_async_cache_set.insert("flip_x");
		m_async_cache_info.flip_x = is_flip_x;
		return;
	}

	if (m_is_flip_x == is_flip_x){ return; }
	m_is_flip_x = is_flip_x;
	float to_scale_x = -_scaleX;
	setScaleX(to_scale_x);
}

void MyParticleSystem::SetFlipY(bool is_flip_y)
{
	if (0 != m_async_load_id)
	{
		m_async_cache_set.insert("flip_y");
		m_async_cache_info.flip_y = is_flip_y;
		return;
	}

	if (m_is_flip_y == is_flip_y){ return; }
	m_is_flip_y = is_flip_y;
	float to_scale_y = -_scaleY;
	setScaleY(to_scale_y);
}

void MyParticleSystem::SetPreTime(float time)
{
	m_skip_time = m_pre_time = time;
	if (time > 0.0001f)
	{
		m_skip_time =m_pre_time = time;
		m_is_need_pre_time = true;
	}
	else
	{
		m_skip_time =m_pre_time = 0;
		m_is_need_pre_time = false;
	}
}

void MyParticleSystem::SetFramePreSecond(int frame_num)
{
	if (0 != m_async_load_id)
	{
		m_async_cache_set.insert("frame_num");
		m_async_cache_info.frame_num = frame_num;
		return;
	}

	if (frame_num > 60)
	{
		frame_num = 60;
	}
	else if (frame_num <= 0)
	{
		frame_num = 30;
	}

	m_frame_pre_second = frame_num;
	m_update_delta_time = 1.0 / m_frame_pre_second;
}

int MyParticleSystem::GetFramePreSecond()
{
	return m_frame_pre_second;
}

void MyParticleSystem::SetScaleSpeed(float speed)
{
	m_scale_speed = speed;
	if (m_scale_speed < 0.0001f)
	{
		m_scale_speed = 0.0001f;
	}
}
void MyParticleSystem::SetScaleTime(float time)
{
	if (0 != m_async_load_id)
	{
		m_async_cache_set.insert("scale_time");
		m_async_cache_info.scale_time = time;
		return;
	}

	m_scale_time = time;
	if (m_scale_time < 0.00001f)
	{
		m_scale_time = 0.00001f;
	}
}

void MyParticleSystem::SetCycleCallBack(Ref* select_target, SEL_CallFunc selector)
{ 
	m_listener = select_target; 
	m_selector = selector;
}

void MyParticleSystem::SetManualUpdate(bool var)
{
	if (0 != m_async_load_id)
	{
		m_async_cache_set.insert("manual_update");
		m_async_cache_info.manual_update = var;
		return;
	}

	m_manual_update = var;
}

bool MyParticleSystem::SetAttribute(const char* name, const std::string& value)
{
	if (strcmp(name, "cycle_total_time") == 0)
	{
		SetCycleTotalTime(CCParticleHelper::ParseFloat(value));
	}
	else if (strcmp(name, "is_cycle") == 0)
	{
		SetCycle(CCParticleHelper::ParseBool(value));
	}
	else if (strcmp(name, "template_name") == 0)
	{
		m_ps_name = value;
	}
	else if (strcmp(name, "ps_scale") == 0)
	{
		Vec3 scale (1, 1, 1);
		CCParticleHelper::ParseVec3(value, &scale);
		SetScale(scale.x, scale.y, scale.z);
	}
	else if (strcmp(name, "scale_speed") == 0)
	{
		SetScaleSpeed(CCParticleHelper::ParseFloat(value));
	}
	else if (strcmp(name, "scale_time") == 0)
	{
		SetScaleTime(CCParticleHelper::ParseFloat(value));
	}
	else if (strcmp(name, "pre_time") == 0)
	{
		SetPreTime(CCParticleHelper::ParseFloat(value));
	}
	else if (strcmp(name, "frame_pre_second") == 0)
	{
		SetFramePreSecond(CCParticleHelper::ParseInt(value));
	}
	else if (strcmp(name, "bound") == 0)
	{
		CCParticleHelper::ParseRect(value, &m_custom_rect);
	}
	else if (strcmp(name, "clipper_width") == 0)
	{
		float width = CCParticleHelper::ParseFloat(value);
		if (m_clipper_rect.size.width != width)
		{
			m_clipper_rect.size.width = width;
			SetClippersRect();
		}
	}
	else if (strcmp(name, "clipper_height") == 0)
	{
		float height = CCParticleHelper::ParseFloat(value);
		if (m_clipper_rect.size.height != height)
		{
			m_clipper_rect.size.height = height;
			SetClippersRect();
		}
	}
	else if (strcmp(name, "clipper_pos") == 0)
	{
		Vec3 pos(0, 0, 0);
		CCParticleHelper::ParseVec3(value, &pos);
		SetClipperPosition(Vec2(pos.x, pos.y));
	}
	else if (strcmp(name, "clipper_technique_list") == 0)
	{
		SetClipperExcludeList(value);
	}
	else if (strcmp(name, "clipper_inverted") == 0)
	{
		SetInverted(CCParticleHelper::ParseBool(value));
	}
	else
	{
		log("[ParticleSystem::SetAttribute TypeNoExist][Type:%s]",name);
		return false;
	}
	return true;
}

bool MyParticleSystem::GetAttribute(const char* name, std::string& value)
{
	value = "";
	if (strcmp(name, "cycle_total_time") == 0)
	{
		value = CCParticleHelper::ToString(m_cycle_total_time);
	}
	else if (strcmp(name, "is_cycle") == 0)
	{
		value = CCParticleHelper::ToString(m_is_cycle);
	}
	else if (strcmp(name, "ps_scale") == 0)
	{
		Vec3 scale (_scaleX, _scaleY, m_fScaleZ);
		value = CCParticleHelper::ToString(scale);
	}
	else if (strcmp(name, "template_name") == 0)
	{
		value = m_ps_name;
	}
	else if (strcmp(name, "scale_speed") == 0)
	{
		value = CCParticleHelper::ToString(m_scale_speed);
	}
	else if (strcmp(name, "scale_time") == 0)
	{
		value = CCParticleHelper::ToString(m_scale_time);
	}
	else if (strcmp(name, "pre_time") == 0)
	{
		value = CCParticleHelper::ToString(m_pre_time);
	}
	else if (strcmp(name, "frame_pre_second") == 0)
	{
		value = CCParticleHelper::ToString(GetFramePreSecond());
	}
	else if(strcmp(name, "m_screen_short_cur_num") == 0)
	{
		value = CCParticleHelper::ToString(m_screen_short_cur_num);
	}
	else if(strcmp(name, "m_screen_short") == 0)
	{
		value = CCParticleHelper::ToString(m_screen_short);
	}
	else if (strcmp(name, "texture_list") == 0)
	{
		for (ParticleTechniqueList::iterator it = m_techniques.begin(); it != m_techniques.end(); ++it)
		{
			CCParticleRenderer* render = (*it)->GetRender();
			if (render == 0)
			{
				continue;
			}
			value += render->GetTextureName() + "|";
		}
	}
	else if (strcmp(name, "mesh_list") == 0)
	{
		for (ParticleTechniqueList::iterator it = m_techniques.begin(); it != m_techniques.end(); ++it)
		{
			CCParticleRenderer* render = (*it)->GetRender();
			if (render && render->GetRendererType() == "Entity")
			{
				CCEntityParticleRenderer* entity = static_cast<CCEntityParticleRenderer*>(render);
				value += entity->GetMeshName() + "|";
			}
		}
	}
	else if (strcmp(name, "emitted_ps_list") == 0)
	{
		for (ParticleTechniqueList::iterator it = m_techniques.begin(); it != m_techniques.end(); ++it)
		{
			CCParticleRenderer* render = (*it)->GetRender();
			if (render && render->GetRendererType() == "EmittedPS")
			{
				CCEmittedPSParticleRenderer* eps = static_cast<CCEmittedPSParticleRenderer*>(render);
				value += eps->GetEmittedPSName() + "|";
			}
		}
	}
	else if (strcmp(name, "tech_name_list") == 0)
	{
		for (ParticleTechniqueList::iterator it = m_techniques.begin(); it != m_techniques.end(); ++it)
		{
			value += (*it)->GetName() + "|";
		}
	}
	else if (strcmp(name, "clipper_width") == 0)
	{
		if (m_clippers.size() == 0)
		{
			return false;
		}
		value = CCParticleHelper::ToString(m_clipper_rect.size.width);
	}
	else if (strcmp(name, "clipper_height") == 0)
	{
		if (m_clippers.size() == 0)
		{
			return false;
		}
		value = CCParticleHelper::ToString(m_clipper_rect.size.height);
	}
	else if (strcmp(name, "clipper_pos") == 0)
	{
		if (m_clippers.size() == 0)
		{
			return false;
		}
		value = CCParticleHelper::ToString(Vec3(m_clipper_pos.x, m_clipper_pos.y, 0));
	}
	else if (strcmp(name, "clipper_technique_list") == 0)
	{
		value = m_clipper_technique_list;
	}
	else if (strcmp(name, "clipper_inverted") == 0)
	{
		value = CCParticleHelper::ToString(m_inverted);
	}
	else
	{
		log("[ParticleSystem::GetAttribute TypeNoExist][Type:%s]",name);
		return false;
	}

	return true;
}

void MyParticleSystem::SetEnabled(bool enabled)
{
	m_enabled = enabled;
	if (m_enabled)
	{
		m_live_time = 0.0;
	}
}

size_t MyParticleSystem::GetNumTechniques() const
{
	return m_techniques.size();
}

ClippingNode* MyParticleSystem::CreateClippingNode(const char* type)
{
	ClippingNode* clipper = ClippingNode::create();
	clipper->setInverted(m_inverted);				//设置底板可见

	return clipper;
}

bool MyParticleSystem::DestoryClippingNode()
{
	this->removeAllChildren();
	for (auto it = m_clippers.begin(); it != m_clippers.end(); it++)
	{
		ClippingNode* clipper = it->second;
		clipper->removeAllChildren();
		clipper->release();
	}
	m_clippers.clear();
	m_clipper_technique_list = "";
	m_clipper_rect.size.width = 28;
	m_clipper_rect.size.height = 28;
	m_clipper_pos = Vec2(0, 0);
	m_inverted = true;

	return true;
}

CCParticleTechnique* MyParticleSystem::CreateTechnique(CCParticleTechnique* template_tech)
{
	CCParticleTechnique *pt = new CCParticleTechnique(this);
	m_techniques.push_back(pt);

	if (template_tech!=0)
	{
		template_tech->CopyAttributesTo( pt );
	}

	if (!m_is_template)
	{
		pt->autorelease();
		pt->retain();

		if (pt->GetName() != "" && IsClipperContain(pt->GetName()))
		{
			AddClipper(pt);
		}
		else
		{
			addChild(pt);
		}
	}

	return pt;
}

bool MyParticleSystem::DestoryTechnique (size_t index)
{
	if( index < m_techniques.size())
	{
		if (m_is_template)
		{
			delete m_techniques[index];
			m_techniques[index] = 0;
		}
		else
		{
			if( m_techniques[index] != 0)
			{
				const std::string& tech_name = m_techniques[index]->GetName();
				if (m_clippers.size() > 0 && tech_name != "" && IsClipperContain(tech_name))
				{
					auto it = m_clippers.find(tech_name);
					if (it != m_clippers.end())
					{
						removeChild(it->second);
						it->second->release();
						m_clippers.erase(it);
					}
				}
				else
				{
					removeChild(m_techniques[index]);
				}
				m_techniques[index]->release();
			}
		}
		m_techniques.erase( m_techniques.begin() + index);
		return true;
	}
	return false;
}
void MyParticleSystem::DestroyAllTechniques (void)
{
	if (m_is_template)
	{
		for (ParticleTechniqueList::iterator it = m_techniques.begin(); it != m_techniques.end(); ++it)
		{
			delete *it;
			*it = 0;
		}
	}
	else
	{
		for (int i = 0; i < m_techniques.size(); i++)
		{
			CCParticleTechnique* pt = m_techniques.at(i);
			pt->release();
		}
		this->removeAllChildren();
	}

	m_techniques.clear();
}

bool MyParticleSystem::IsClipperContain(std::string technique_name)
{
	if (m_clipper_technique_list == technique_name)
	{
		return true;
	}

	std::string technique_list = m_clipper_technique_list;
	std::string::size_type pos = technique_list.find('|');

	std::string sub_string = "";
	while (pos != std::string::npos)
	{
		sub_string = technique_list.substr(0, pos);
		technique_list = technique_list.substr(pos+1, technique_list.length());
		if (sub_string == technique_name || technique_list == technique_name)
		{
			return true;
		}
		pos = technique_list.find('|');
	}

	return false;
}

int MyParticleSystem::GetIndexTechnique(const std::string& name)
{
	size_t size = m_techniques.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (strcmp(m_techniques[i]->GetName().c_str(), name.c_str()) == 0)
		{
			return i;
		}
	}
	return -1;
}

CCParticleTechnique* MyParticleSystem::GetTechnique (size_t index) const
{
	if (index >= m_techniques.size())
	{
		return 0;
	}
	return m_techniques[index];
}
bool MyParticleSystem::MoveTechnique(unsigned int move_index, unsigned int target_index)
{
	if (m_clippers.size() > 0)
	{
		move_index -= 1;
		target_index -= 1;
	}

	if (move_index == target_index
		|| move_index >= m_techniques.size()
		|| target_index >= m_techniques.size())
	{
		return false;
	}

	CCParticleTechnique* temp = m_techniques[move_index];
	if (move_index > target_index)
	{
		m_techniques.insert(m_techniques.begin() + target_index, temp);
		m_techniques.erase(m_techniques.begin() + move_index + 1);
	}
	else
	{
		m_techniques.insert(m_techniques.begin() + target_index+1, temp);
		m_techniques.erase(m_techniques.begin() + move_index);
	}
	
	return true;
}

void MyParticleSystem::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	if (!_visible || !m_enabled)
	{
		return;
	}

	uint32_t flags = processParentFlags(parentTransform, parentFlags);

	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

	int i = 0;
	if (!_children.empty())
	{
		sortAllChildren();
		for (; i < _children.size(); i++)
		{
			auto node = _children.at(i);

			if (node && node->getLocalZOrder() < 0)
			{
				node->visit(renderer, _modelViewTransform, flags);
			}
			else
			{
				break;
			}
		}

		this->draw(renderer, _modelViewTransform, flags);

		for (auto it = _children.cbegin() + i; it != _children.cend(); ++it)
		{
			(*it)->visit(renderer, _modelViewTransform, flags);
		}
	}
	else
	{
		this->draw(renderer, _modelViewTransform, flags);
	}

	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

	if (m_show_aabb)
	{
		director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
		Mat4 tmp_matrix;
		tmp_matrix.setIdentity();
		AffineTransform tmp_transform = getNodeToParentAffineTransform();
		CGAffineToGL(tmp_transform, tmp_matrix.m);
		director->multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW,tmp_matrix);

		GL::blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		DrawPrimitives::setDrawColor4F(1.0f, 0.0f, 0.0f,0.5f);
		GLfloat old_value;
		glGetFloatv(GL_LINE_WIDTH,&old_value);
		glLineWidth(2.0f);
		DrawPrimitives::drawRect(m_custom_rect.origin, CCPoint(m_custom_rect.getMaxX(), m_custom_rect.getMaxY()));
		glLineWidth(old_value);
		director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	}
}

void MyParticleSystem::update(float fDelta)
{
	if (0 != m_async_load_id)
	{
		return;
	}

	if (m_skip_time >= TIME_OF_EACH_FRAME)
	{
		for(m_skip_time; m_skip_time >= TIME_OF_EACH_FRAME; m_skip_time-=TIME_OF_EACH_FRAME)
		{
			onUpdate(TIME_OF_EACH_FRAME);
		}
		if (m_skip_time > 0)
		{
			onUpdate(m_skip_time);
			m_skip_time = 0;
		}
	}
	else
	{
		onUpdate(fDelta);
	}
}

void MyParticleSystem::onUpdate(float fDelta)
{
	if (!m_enabled)
	{
		return;
	}

	if (m_manual_update)
	{
		fDelta = 0;
	}
	else
	{
		fDelta *= m_scale_time;
	}

	int scale = (int)(fDelta/m_cycle_total_time);
	fDelta-=m_cycle_total_time*scale;

	//降低粒子系统的update频率
	m_curr_delta_time += fDelta;
	if (m_curr_delta_time < m_update_delta_time)
	{
		return;
	}
	fDelta = m_curr_delta_time;
	m_curr_delta_time = 0.0f;

	m_live_time += fDelta;
	if (m_live_time >= m_cycle_total_time)
	{
		if (!m_is_cycle)
		{
			m_enabled = false;
		}
		m_live_time = 0.0f;

		if (m_listener!=0 && m_selector!=0)
		{
			(m_listener->*m_selector)();
			return;
		}
	}

	if(m_screen_short)
	{
		fDelta = CCDirector::getInstance()->getAnimationInterval();
	}
	
	if(m_techniques.size()<=0)
		return ;

	for (ParticleTechniqueList::iterator it = m_techniques.begin(); it != m_techniques.end(); ++it)
	{
		(*it)->UpdateFromPS(fDelta);
	}

	if(m_screen_short)
	{
		DoScreenShort();
	}
}

void MyParticleSystem::DoScreenShort()
{
	if(m_screen_short_cur_num<m_screen_short_num)
	{
		char image_name[256] = {0};
		sprintf(image_name,"%s_%d.png",m_ps_name.c_str(),m_screen_short_cur_num++);
		utils::captureScreen(nullptr, m_screen_path + "\\" + image_name);
	}
	else
	{
		m_screen_short = false;
		m_screen_short_cur_num = 0;
		m_screen_short_num = 0;
	}
}

void MyParticleSystem::ResetData()
{
	for (ParticleTechniqueList::iterator it = m_techniques.begin(); it != m_techniques.end(); ++it)
	{
		(*it)->ResetData();
	}

	m_live_time = 0.0f;
}


void MyParticleSystem::ChangeTexExtension(const std::string& ext)
{
	std::string name = "";
	for (ParticleTechniqueList::iterator it = m_techniques.begin(); it != m_techniques.end(); ++it)
	{
		CCParticleRenderer* render = (*it)->GetRender();
		if (render == 0)
		{
			continue;
		}
		name = render->GetTextureName();
		if (name != "")
		{
			size_t pos = name.find_last_of('.');
			if (pos != std::string::npos && pos != 0)
			{
				name = name.substr(0, pos) + "." + ext;
				render->SetTexture(name);
			}
		}
	}
}

size_t MyParticleSystem::GetTotalParticleNum()
{
	size_t total = 0;
	for (ParticleTechniqueList::iterator it = m_techniques.begin(); it != m_techniques.end(); ++it)
	{
		total += (*it)->GetVisualNum();
	}
	return total;
}

size_t MyParticleSystem::GetTotalActiveParticleNum()
{
	size_t total = 0;
	for (ParticleTechniqueList::iterator it = m_techniques.begin(); it != m_techniques.end(); ++it)
	{
		total += (*it)->GetActiveNum();
	}
	return total;
}


void MyParticleSystem::setVisible( bool visible )
{
	if (_visible == visible)
	{
		return;
	}

	_visible = visible;
	for (ParticleTechniqueList::iterator it = m_techniques.begin(); it != m_techniques.end(); ++it)
	{
		(*it)->setVisible(visible);
	}
}

void MyParticleSystem::SetScreenShotsState(bool screen_short,int num, std::string path)
{ 
	if(screen_short)
	{
		m_screen_short_num=num;
		m_screen_short_cur_num=0;
		m_screen_path = path;
	}
	m_screen_short = screen_short;
}

void MyParticleSystem::SetClipperExcludeList(std::string exclude_list)
{
	if (m_clipper_technique_list == exclude_list)
	{
		return;
	}

	m_clipper_technique_list = exclude_list;
	for (auto it = m_clippers.begin(); it != m_clippers.end(); it++)
	{
		ClippingNode* node = it->second;
		node->removeAllChildren();
		node->release();
	}
	m_clippers.clear();
	this->removeAllChildren();

	for (int i = 0; i < m_techniques.size(); i++)
	{
		CCParticleTechnique *pt = m_techniques.at(i);

		if (IsClipperContain(pt->GetName()))
		{
			AddClipper(pt);
		}
		else
		{
			this->addChild(pt);
		}
	}
}

std::string MyParticleSystem::GetClipperExcludeList()
{
	return m_clipper_technique_list;
}

void MyParticleSystem::AddClipper(CCParticleTechnique* pt)
{
	ClippingNode* clipper = CreateClippingNode("");
	if (clipper == nullptr)
	{
		return;
	}
	Sprite* stencil = Sprite::create(PSManager::GetInstance()->GetTexturePath() + "clipper.png");
	stencil->setTextureRect(m_clipper_rect);
	stencil->setPosition(m_clipper_pos);
	clipper->setStencil(stencil);			//设置裁剪模板 
	clipper->addChild(pt);
	this->addChild(clipper);
	clipper->retain();
	m_clippers[pt->GetName()] = clipper;
}

void MyParticleSystem::SetInverted(bool inverted)
{
	m_inverted = inverted;
	for (auto it = m_clippers.begin(); it != m_clippers.end(); it++)
	{
		it->second->setInverted(m_inverted);
	}
}

bool MyParticleSystem::GetInverted()
{
	return m_inverted;
}

std::map<std::string, ClippingNode*>& MyParticleSystem::GetClippers()
{
	return m_clippers;
}

Rect& MyParticleSystem::GetClipperRect()
{
	return m_clipper_rect;
}

Vec2& MyParticleSystem::GetClipperPos()
{
	return m_clipper_pos;
}

void MyParticleSystem::SetClippersRect()
{
	for (auto it = m_clippers.begin(); it != m_clippers.end(); it++)
	{
		ClippingNode* node = it->second;
		Sprite* stencil = static_cast<Sprite*>(node->getStencil());
		stencil->setTextureRect(m_clipper_rect);
	}
}

void MyParticleSystem::SetClipperPosition(const Vec2& pos)
{
	if (m_clipper_pos == pos)
	{
		return;
	}

	m_clipper_pos = pos;
	for (auto it = m_clippers.begin(); it != m_clippers.end(); it++)
	{
		ClippingNode* node = it->second;
		Node* stencil = node->getStencil();
		stencil->setPosition(m_clipper_pos);
	}
}

void MyParticleSystem::MakeGrey(bool is_grey)
{
	if (0 != m_async_load_id)
	{
		m_async_cache_set.insert("is_grey");
		m_async_cache_info.is_grey = is_grey;
		return;
	}

	if (m_is_grey == is_grey)
	{
		return;
	}
	m_is_grey = is_grey;

	for (CCParticleTechnique* technique : m_techniques)
	{
		technique->MakeGrey(is_grey);
	}
}

NS_CC_END
