
#include "entity.h"
#include "renderer.h"

#include "particle/simplespline.h"
#include "particle/particlefactory.h"
#include "rendererbuffer.h"
#include "particle/technique.h"
#include "particle/particlesystem.h"
#include "particle/particledatamanager.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"
#include "particle/dynamicattribute.h"

#include "mesh/CCEnt3D.h"
#include "mesh/CCMyMesh.h"
#include "mesh/CCMeshManager.h"

#include "cocos/base/ccTypes.h"
#include "cocos/2d/CCNode.h"
#include "math/CCMath.h"
//#include "kazmath/GL/matrix.h"
#include "tinyxml2/tinyxml2.h"

NS_CC_BEGIN

const Vec3 CCEntityParticleRenderer::DEFAULT_AXIS (0, -1, 0);

CCEntityParticleRenderer::CCEntityParticleRenderer(CCParticleTechnique *pt, const std::string& texture_suffix)
	:CCParticleRenderer(pt, texture_suffix)
	,m_scroll_speedu(0)
	,m_scroll_speedv(0)
	,m_u_cycle(true)
	,m_v_cycle(true)
	,m_offset_u(0.0f)
	,m_offset_v(0.0f)
	,m_scale_u(1.0f)
	,m_scale_v(1.0f)
	,m_angle(0)
	,m_last_index(0)
	,m_axis(DEFAULT_AXIS)
	,m_anim_name("action")
	,m_action_loop(false)
	,m_anim_dirty(true)
	,m_pTexture(0)
{
	m_type = "Entity";

	m_scroll_speedu = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_scroll_speedu)->SetValue(0.0f);

	m_scroll_speedv = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_scroll_speedv)->SetValue(0.0f);
}

CCEntityParticleRenderer::~CCEntityParticleRenderer()
{
	Clear();

	if (m_scroll_speedu != 0)
	{
		delete m_scroll_speedu;
		m_scroll_speedu = 0;
	}

	if (m_scroll_speedv != 0)
	{
		delete m_scroll_speedv;
		m_scroll_speedv = 0;
	}
	CC_SAFE_RELEASE(m_pTexture);
	m_pTexture = NULL;
}

void CCEntityParticleRenderer::SetOffsetU(float u)
{
	m_offset_u = u;
}

void CCEntityParticleRenderer::SetOffsetV(float v)
{
	m_offset_v = v;
}

void CCEntityParticleRenderer::SetScaleU(float scale_u)
{
	m_scale_u = scale_u;
}

void CCEntityParticleRenderer::SetScaleV(float scale_v)
{
	m_scale_v = scale_v;
}

void CCEntityParticleRenderer::SetAngle(int angle)
{
	m_angle = angle;
}

void CCEntityParticleRenderer::Draw(std::list<CCParticle*>& current_particles)
{
	if (!CanRender())
	{
		return;
	}

	std::list<CCParticle*>::iterator it;
	std::list<CCParticle*>::iterator ie = current_particles.end();
	size_t size = m_entity_list.size();

	for (it = current_particles.begin(); it != ie; ++it)
	{
		if ((*it)->m_type != CCParticle::Visual)
		{
			continue;
		}

		if ((*it)->m_index != -1 && (*it)->m_index < (int)size)
		{
			CCEnt3D* ent = m_entity_list[(*it)->m_index]->ent;
			m_entity_list[(*it)->m_index]->visible = true;

			bool m_is_local_space = m_technique->IsLocalSpace();
			Mat4 scale_mat;
			Mat4::createScale((*it)->m_width / m_technique->GetDefaultWidth(), (*it)->m_height / m_technique->GetDefaultHeight(), (*it)->m_depth / m_technique->GetDefaultDepth(), &scale_mat);
			Mat4 rotation_mat;
			Mat4::createRotation(m_axis, (*it)->m_angle * kmPIOver180, &rotation_mat);
			Mat4 mat = rotation_mat * scale_mat;
			const Vec3& position = m_is_local_space ? (*it)->m_position : Vec3(0,0,0);
			mat.m[12] = position.x;
			mat.m[13] = position.y;
			mat.m[14] = position.z;

			ent->SetBlendColor((*it)->m_color);

			Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
			if (!m_is_local_space)
			{
				Mat4 derived_not_tran = m_technique->GetTechToWorldMat();
				derived_not_tran.m[12] = derived_not_tran.m[13] = derived_not_tran.m[14] = 0;
				Mat4::multiply(derived_not_tran,mat,&mat);

				mat.m[12] += (*it)->m_position.x;
				mat.m[13] += (*it)->m_position.y;
				mat.m[14] += (*it)->m_position.z;
			}
			Director::getInstance()->multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW,mat);
			if (ent->HaveMesh())
			{
				ent->Draw(m_add_mode_power_ratio);
			}
			Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
		}
	}
}

void  CCEntityParticleRenderer::UpdateFromTechnique(std::list<CCParticle*>& current_particles, float time_elapsed)
{
	size_t size = m_entity_list.size();
	for (std::list<CCParticle*>::iterator it = current_particles.begin(); it != current_particles.end(); ++it)
	{
		if ((*it)->m_type != CCParticle::Visual)
		{
			continue;
		}

		if ((*it)->m_index < 0)
		{
			int index = FindFirstNoneVisible();
			if (index == -1)
			{
				break;
			}
			
			if (index >= size)
			{
				continue;
			}

			(*it)->m_index = index;
			CCEnt3D* ent = m_entity_list[(*it)->m_index]->ent;
			ent->SetTextureOffsetUV(m_offset_u, m_offset_v);
			ent->SetTextureScaleUV(m_scale_u, m_scale_v);
			ent->SetTextureRotateUV(m_angle);
		}

		if ((*it)->m_index != -1 && (*it)->m_index < (int)size)
		{
			float t = ((*it)->m_total_live - (*it)->m_time_live) / (*it)->m_total_live;
			UpdateMeshUVOffset((*it)->m_index, t, time_elapsed);

			CCEnt3D* ent = m_entity_list[(*it)->m_index]->ent;
			m_entity_list[(*it)->m_index]->visible = true;

			ent->PreDraw(time_elapsed);
		}
	}
}

void CCEntityParticleRenderer::UpdateMeshUVOffset(size_t index, float t, float time_elapsed)
{
	if (index >= 0 && index < m_entity_list.size())
	{
		Entity* entity = m_entity_list[index];
		if (entity != 0 && entity->ent != 0)
		{
			CCEnt3D* ent = entity->ent;
			entity->u_offset = entity->u_offset + AttributeCalculate(m_scroll_speedu, t, 0.0f) * time_elapsed;
			entity->v_offset = entity->v_offset + AttributeCalculate(m_scroll_speedv, t, 0.0f) * time_elapsed;
			ent->SetTextureOffsetUV(entity->u_offset + m_offset_u, entity->v_offset + m_offset_v);
		}
		
	}
}

bool CCEntityParticleRenderer::CanRender()
{
	if (m_entity_list.size() > 0)
	{
		CCEnt3D* ent = m_entity_list[0]->ent;
		if (ent == 0 || ent->getTexture() == 0)
		{
			return false;
		}

		if (CCMeshManager::sharedCCMeshManger()->GetByName(m_mesh_full_name) == 0)
		{
			return false;
		}

		UpdateAnimData();
		return true;
	}

	return false;
}

void CCEntityParticleRenderer::ParticleExpired(CCParticle* particle)
{
	if (particle != 0 && particle->m_index >= 0 && particle->m_index < (int)m_entity_list.size())
	{
		CCEnt3D* ent = m_entity_list[particle->m_index]->ent;
		if (ent == 0)
		{
			return;
		}
		m_entity_list[particle->m_index]->visible = false;
		ent->SetTextureOffsetUV(0.0f, 0.0f);
		ent->SetTextureScaleUV(1.0f, 1.0f);
		ent->SetTextureRotateUV(0);
		m_entity_list[particle->m_index]->u_offset = 0;
		m_entity_list[particle->m_index]->v_offset = 0;
		particle->m_index = -1;
	}
}


int CCEntityParticleRenderer::FindFirstNoneVisible()
{
	size_t size = m_entity_list.size();
	size_t index  = 0;
	while(index < size)
	{
		if (!m_entity_list[m_last_index]->visible)
		{
			return m_last_index;
		}
		m_last_index = (m_last_index + 1) % size;
		++index;
	}

	return -1;
}

void CCEntityParticleRenderer::NotifyParticleQuota(size_t size)
{
	if (size == m_particle_quota)
	{
		return;
	}

	CCParticleRenderer::NotifyParticleQuota( size);

	if (!m_technique->IsTemplate())
	{
		Clear();
		Init();
	}
}


void CCEntityParticleRenderer::SetMesh(const std::string& name)
{
	m_mesh_name = name;
	m_mesh_full_name = PSManager::GetInstance()->GetMeshPath() + m_mesh_name;

	for (std::vector<Entity*>::iterator it = m_entity_list.begin(); it != m_entity_list.end(); ++it)
	{
		if ((*it)->ent != 0)
		{
			(*it)->ent->SetMesh(m_mesh_full_name, m_mat_type);
		}
	}
}

void CCEntityParticleRenderer::ResetData()
{
	m_last_index = 0;
	for (std::vector<Entity*>::iterator it = m_entity_list.begin(); it != m_entity_list.end(); ++it)
	{
		(*it)->visible = false;
		(*it)->u_offset = 0;
		(*it)->v_offset = 0;
		(*it)->ent->SetTextureOffsetUV(0.0f, 0.0f);
		(*it)->ent->SetTextureScaleUV(1.0f, 1.0f);
		(*it)->ent->SetTextureRotateUV(0);
	}
}

void CCEntityParticleRenderer::Init()
{
	m_mesh_full_name = PSManager::GetInstance()->GetMeshPath() + m_mesh_name;
	m_anim_dirty = true;

	for (size_t i = 0; i < m_particle_quota; ++i)
	{
		CCEnt3D* ent = new CCEnt3D();
		Entity* entity = new Entity();

		if (m_mesh_name != "" && ent != 0)
		{
			ent->SetMesh(m_mesh_full_name, m_mat_type);
		}
		entity->u_offset = 0.0f;
		entity->v_offset = 0.0f;
		entity->visible = false;
		entity->ent = ent;

		m_entity_list.push_back(entity);
	}

	UpdateTexture();
	UpdateMaterialType();
	UpdateTexWrap();
	UpdateAnimData();
	m_last_index = 0;
}

void CCEntityParticleRenderer::Clear()
{
	size_t size = m_entity_list.size();
	for (size_t index = 0; index < size; ++index)
	{
		CCEnt3D* ent = m_entity_list[index]->ent;
		if (ent != 0)
		{
			delete ent;
			ent = 0;
		}
		delete m_entity_list[index];
	}
	m_entity_list.clear();
}

void CCEntityParticleRenderer::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleRenderer::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	if (m_mesh_name != "")
	{
		child = my_document->NewElement("mesh_name");
		content = my_document->NewText(m_mesh_name.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!(m_scroll_speedu->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_scroll_speedu)->GetValue(), 0.0f)))
	{
		child = my_document->NewElement("dyn");
		child->SetAttribute("type", "scroll_u");
		m_scroll_speedu->SaveScript(my_document, child);
		element->LinkEndChild(child);
	}

	if (!(m_scroll_speedv->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_scroll_speedv)->GetValue(), 0.0f)))
	{
		child = my_document->NewElement("dyn");
		child->SetAttribute("type", "scroll_v");
		m_scroll_speedv->SaveScript(my_document, child);
		element->LinkEndChild(child);
	}

	if (!CCParticleHelper::IsEqual(m_axis, DEFAULT_AXIS))
	{
		str = CCParticleHelper::ToString( m_axis);
		child = my_document->NewElement("entity_axis");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!m_u_cycle)
	{
		str = CCParticleHelper::ToString( m_u_cycle);
		child = my_document->NewElement("u_cycle");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!m_v_cycle)
	{
		str = CCParticleHelper::ToString( m_v_cycle);
		child = my_document->NewElement("v_cycle");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_offset_u, 0.0f))
	{
		str = CCParticleHelper::ToString( m_offset_u);
		child = my_document->NewElement("offset_u");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_offset_v, 0.0f))
	{
		str = CCParticleHelper::ToString( m_offset_v);
		child = my_document->NewElement("offset_v");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_scale_u, 1.0f))
	{
		str = CCParticleHelper::ToString(m_scale_u);
		child = my_document->NewElement("scale_u");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_scale_v, 1.0f))
	{
		str = CCParticleHelper::ToString(m_scale_v);
		child = my_document->NewElement("scale_v");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_angle, 0))
	{
		str = CCParticleHelper::ToString(m_angle);
		child = my_document->NewElement("angle");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_action_loop)
	{
		str = CCParticleHelper::ToString( m_action_loop);
		child = my_document->NewElement("anim_loop");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}

bool CCEntityParticleRenderer::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "mesh_name") == 0)
	{
		if (m_technique->IsTemplate())
		{
			m_mesh_name = value;
		}
		else
		{
			SetMesh(value);
		}
	}
	else if (strcmp(type, "scroll_u") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetTextureScrollSpeedU(dyn);
	}
	else if (strcmp(type, "scroll_v") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetTextureScrollSpeedV(dyn);
	}
	else if (strcmp(type, "entity_axis") == 0)
	{
		Vec3 axis (1, 0, 0);
		CCParticleHelper::ParseVec3(value, &axis);
		SetAxis(axis);
	}
	else if (strcmp(type, "u_cycle") == 0)
	{
		SetULoop(CCParticleHelper::ParseBool(value));
	}
	else if (strcmp(type, "v_cycle") == 0)
	{
		SetVLoop(CCParticleHelper::ParseBool(value));
	}
	else if (strcmp(type, "offset_u") == 0)
	{
		m_offset_u = CCParticleHelper::ParseFloat(value);
	}
	else if (strcmp(type, "offset_v") == 0)
	{
		m_offset_v = CCParticleHelper::ParseFloat(value);
	}
	else if (strcmp(type, "scale_u") == 0)
	{
		m_scale_u = CCParticleHelper::ParseFloat(value);
	}
	else if (strcmp(type, "scale_v") == 0)
	{
		m_scale_v = CCParticleHelper::ParseFloat(value);
	}
	else if (strcmp(type, "angle") == 0)
	{
		m_angle = CCParticleHelper::ParseInt(value);
	}
	else if (strcmp(type, "anim_loop") == 0)
	{
		SetAnimLoop(CCParticleHelper::ParseBool(value));
	}
	else
	{
		return CCParticleRenderer::SetAttribute(type, value);
	}
	return true;
}

void CCEntityParticleRenderer::CopyAttributesTo(CCParticleRenderer *render)
{
	CCParticleRenderer::CopyAttributesTo(render);
	CCEntityParticleRenderer* entity_render = static_cast<CCEntityParticleRenderer*>(render);
	entity_render->SetMesh(m_mesh_name);

	entity_render->SetTextureScrollSpeedU(CloneDynamicAttribute(m_scroll_speedu));
	entity_render->SetTextureScrollSpeedV(CloneDynamicAttribute(m_scroll_speedv));
	entity_render->SetAxis(m_axis);
	entity_render->SetULoop(m_u_cycle);
	entity_render->SetVLoop(m_v_cycle);

	entity_render->SetOffsetU(m_offset_u);
	entity_render->SetOffsetV(m_offset_v);

	entity_render->SetScaleU(m_scale_u);
	entity_render->SetScaleV(m_scale_v);

	entity_render->SetAngle(m_angle);

	entity_render->SetAnimLoop(m_action_loop);
	entity_render->SetAnimName(m_anim_name);
}

bool CCEntityParticleRenderer::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str;
	if (strcmp(type, "mesh_name") == 0)
	{
		str = m_mesh_name;
	}
	else if (strcmp(type, "scroll_u") == 0)
	{
		GetDynAttribute(m_scroll_speedu, str);
	}
	else if (strcmp(type, "scroll_v") == 0)
	{
		GetDynAttribute(m_scroll_speedv, str);
	}
	else if (strcmp(type, "entity_axis") == 0)
	{
		str = CCParticleHelper::ToString(m_axis);
	}
	else if (strcmp(type, "u_cycle") == 0)
	{
		str = CCParticleHelper::ToString(m_u_cycle);
	}
	else if (strcmp(type, "v_cycle") == 0)
	{
		str = CCParticleHelper::ToString(m_v_cycle);
	}
	else if (strcmp(type, "offset_u") == 0)
	{
		str = CCParticleHelper::ToString(m_offset_u);
	}
	else if (strcmp(type, "offset_v") == 0)
	{
		str = CCParticleHelper::ToString(m_offset_v);
	}
	else if (strcmp(type, "scale_u") == 0)
	{
		str = CCParticleHelper::ToString(m_scale_u);
	}
	else if (strcmp(type, "scale_v") == 0)
	{
		str = CCParticleHelper::ToString(m_scale_v);
	}
	else if (strcmp(type, "angle") == 0)
	{
		str = CCParticleHelper::ToString(m_angle);
	}
	else if (strcmp(type, "anim_loop") == 0)
	{
		str = CCParticleHelper::ToString(m_action_loop);
	}
	else if (strcmp(type, "anim_name") == 0)
	{
		str = m_anim_name;
	}
	else
	{
		return CCParticleRenderer::GetAttribute(type, value, size);;
	}

	strncpy(value, str.c_str(), size);
	return true;
}

void CCEntityParticleRenderer::SetTextureScrollSpeedU(DynamicAttribute* dyn)
{
	if (m_scroll_speedu != 0)
	{
		delete m_scroll_speedu;
		m_scroll_speedu = 0;
	}

	m_scroll_speedu = dyn;
}

void CCEntityParticleRenderer::SetTextureScrollSpeedV(DynamicAttribute* dyn)
{
	if (m_scroll_speedv != 0)
	{
		delete m_scroll_speedv;
		m_scroll_speedv = 0;
	}

	m_scroll_speedv = dyn;
}

bool CCEntityParticleRenderer::SetDynAttribute(const char* type, DynamicAttribute *dyn)
{
	if (strcmp(type, "scroll_u") == 0)
	{
		SetTextureScrollSpeedU(dyn);
	}
	else if (strcmp(type, "scroll_v") == 0)
	{
		SetTextureScrollSpeedV(dyn);
	}
	else
	{
		return false;
	}

	return true;
}

void CCEntityParticleRenderer::SetULoop(bool loop)
{
	m_u_cycle = loop;
	UpdateTexWrap();
}

void CCEntityParticleRenderer::SetVLoop(bool loop)
{
	m_v_cycle = loop;
	UpdateTexWrap();
}

void CCEntityParticleRenderer::UpdateTexWrap()
{
	std::vector<Entity*>::iterator it, ie = m_entity_list.end();
	for (it = m_entity_list.begin(); it != ie; ++it)
	{
		if ((*it)->ent == 0)
		{
			continue;
		}

		GLint uaddr_mode = GL_REPEAT;
		GLint vaddr_mode = GL_REPEAT;

		if(!m_u_cycle)
		{
			uaddr_mode = GL_CLAMP_TO_EDGE;
		}
		if (!m_v_cycle)
		{
			vaddr_mode = GL_CLAMP_TO_EDGE;
		}

		(*it)->ent->SetWrapS(uaddr_mode);
		(*it)->ent->SetWrapT(vaddr_mode);
	}
}

void CCEntityParticleRenderer::UpdateTexture()
{
	if (m_pTexture == 0)
	{
		return;
	}

	for (std::vector<Entity*>::iterator it = m_entity_list.begin(); it != m_entity_list.end(); ++it)
	{
		if ((*it)->ent != 0)
		{
			(*it)->ent->setTexture(m_pTexture);
		}
	}
}

void CCEntityParticleRenderer::SetTexture(Texture2D* texture)
{
	if (texture != nullptr)
	{
		CC_SAFE_RELEASE(m_pTexture);
		m_pTexture = texture;
		CC_SAFE_RETAIN(m_pTexture);
		UpdateTexture();
	}
}

void CCEntityParticleRenderer::OnPreparedCompleted(void* callback_data)
{
	if (callback_data == NULL)
		return;

	auto particle_render = static_cast<CCEntityParticleRenderer*>(callback_data);
	char tmp[256];
	particle_render->GetAttribute("full_tex_name", tmp, 256);
	std::string full_tex_name = tmp;
	auto texture = Director::getInstance()->getTextureCache()->getTextureForKey(full_tex_name);
	particle_render->SetTexture(texture);
	CC_SAFE_RELEASE(particle_render);
}

void CCEntityParticleRenderer::OnTextureCacheCallBack(Texture2D* texture)
{
	if (texture == nullptr)
	{
		return;
	}
	SetTexture(texture);
}

void CCEntityParticleRenderer::SetTexture(const std::string& name)
{
	CCParticleRenderer::SetTexture(name);
	auto texture = Director::getInstance()->getTextureCache()->getTextureForKey(m_texture_full_name);
	if (texture != nullptr)
	{
		SetTexture(texture);
	}
	else
	{
		CC_SAFE_RETAIN(this);
		TextureCache::getInstance()->addImageAsync(m_texture_full_name, CC_CALLBACK_1(CCEntityParticleRenderer::OnTextureCacheCallBack, this));
	}
}

void CCEntityParticleRenderer::UpdateMaterialType()
{
	for (std::vector<Entity*>::iterator it = m_entity_list.begin(); it != m_entity_list.end(); ++it)
	{
		if ((*it)->ent != 0)
		{
			(*it)->ent->SetMaterialType(m_mat_type);
		}
	}
}

void CCEntityParticleRenderer::SetAxis(const Vec3& vec)
{
	static Vec3 zero (0, 0, 0);
	m_axis = vec;
	if (CCParticleHelper::IsEqual(m_axis, zero))
	{
		m_axis = DEFAULT_AXIS;
	}
	m_axis.normalize();
}

void CCEntityParticleRenderer::SetMaterialType(MaterialType type)
{
	CCParticleRenderer::SetMaterialType(type);
	UpdateMaterialType();
}

void CCEntityParticleRenderer::SetAnimName(const std::string& name)
{
	m_anim_name = name;
	m_anim_dirty = true;
	UpdateAnimData();
}

void CCEntityParticleRenderer::SetAnimLoop(bool loop)
{
	m_action_loop = loop;
	m_anim_dirty = true;
	UpdateAnimData();
}

void CCEntityParticleRenderer::UpdateAnimData()
{
	if (!m_anim_dirty)
	{
		return;
	}
	bool has_skeleton = false;
	CCMyMesh* mesh = static_cast<CCMyMesh*>(CCMeshManager::sharedCCMeshManger()->GetByName(m_mesh_full_name));
	if (mesh != 0)
	{
		has_skeleton = mesh->HasSkeleton();
	}

	if (!has_skeleton)
	{
		return;
	}

	std::vector<Entity*>::iterator it, ie = m_entity_list.end();
	for (it = m_entity_list.begin(); it != ie; ++it)
	{
		if ((*it)->ent != 0)
		{
			(*it)->ent->SetAnimationEnable(m_anim_name, true, m_action_loop);
		}
	}

	m_anim_dirty = false;
}

void CCEntityParticleRenderer::MakeGrey(bool is_grey)
{
	for(Entity* entity : m_entity_list)
	{
		entity->ent->MakeGrey(is_grey, m_mat_type);
	}
}

NS_CC_END
