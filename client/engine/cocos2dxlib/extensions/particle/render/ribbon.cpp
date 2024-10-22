
#include "ribbon.h"
#include "renderer.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"
#include "particle/dynamicattribute.h"

#include "particle/simplespline.h"
#include "particle/particlefactory.h"
#include "rendererbuffer.h"
#include "particle/technique.h"

#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"
#include "cocos/2d/CCNode.h"


NS_CC_BEGIN

const float CCRibbonParticleRenderer::DEFAULT_WIDTH = 16.0f;
const float CCRibbonParticleRenderer::DEFAULT_CHANGE_WIDTH = 0.0f;
const float CCRibbonParticleRenderer::DEFAULT_TRAIL_LEN = 128.0f;
const size_t CCRibbonParticleRenderer::DEFAULT_MAX_ELEMENT = 50;
const Color4F CCRibbonParticleRenderer::DEFAULT_COLOR (1, 1, 1, 1);
const Color4F CCRibbonParticleRenderer::DEFAULT_CHANGE_COLOR (0, 0, 0, 1);
const RibbonUVDirection CCRibbonParticleRenderer::DEFAULT_UV_DIR = RUVD_U;
const size_t CCRibbonParticleRenderer::SEGMENT_EMPTY = 0xffffffffu;


CCRibbonParticleRenderer::CCRibbonParticleRenderer(CCParticleTechnique *pt, const std::string& texture_suffix) :
	CCParticleRenderer(pt, texture_suffix),
	m_uv_dir(DEFAULT_UV_DIR),
	m_color(DEFAULT_COLOR),
	m_change_color(DEFAULT_CHANGE_COLOR),
	m_width(DEFAULT_WIDTH),
	m_change_width(DEFAULT_CHANGE_WIDTH),
	m_trail_length(DEFAULT_TRAIL_LEN),
	m_max_element(DEFAULT_MAX_ELEMENT),
	m_chain_count(1),
	m_max_vertex_count(0),
	m_max_index_count(0),
	m_num_indices(0),
	m_dirty_element(true),
	m_buffer_data(0),
	m_speed_is_zero(false),
	m_posR(Vec3::ZERO)
{
	m_type = "RibbonTrail";
	m_elem_length = m_trail_length / (float)m_max_element;

	if (!m_technique->IsTemplate())
	{
		m_buffer_data = new CCParticleRenderBuffer(true);
	}
}

CCRibbonParticleRenderer::~CCRibbonParticleRenderer()
{
	ElementList::iterator it, ie = m_element_list.end();
	for (it = m_element_list.begin(); it != ie; ++it)
	{
		delete (*it);
		(*it) = 0;
	}

	m_element_list.clear();

	if (m_buffer_data != 0)
	{
		delete m_buffer_data;
		m_buffer_data = 0;
	}
}

void CCRibbonParticleRenderer::Draw(std::list<CCParticle*>& current_particles)
{
	if (m_buffer_data == 0 || m_buffer_data->getTexture() == 0)
	{
		return;
	}
	m_speed_is_zero = true;
	std::list<CCParticle*>::iterator it;
	std::list<CCParticle*>::iterator ie = current_particles.end();

	m_pos_map.clear();

	for (it = current_particles.begin(); it != ie; ++it)
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

			if (index >= m_chain_count)
			{
				continue;
			}

			(*it)->m_index = index;
		}

		if ((*it)->m_index != -1 && (*it)->m_index < (int)m_chain_count)
		{
			// 在拖尾粒子速度为0的时候，设置它的位置与particlesystem位置相同，实现拖尾跟随particlesystem效果
			if((*it)->m_direction.isZero())
			{
				(*it)->m_position = m_technique->GetParticleSystem()->getPosition3D();
			}
			else
			{
				m_speed_is_zero = false;
			}
			m_pos_map[(*it)->m_index] = (*it)->m_position;
		}
	}

	ClearAllNodeVisible();

	if (UpdateData(m_technique->GetUpdateLastTime()) && m_num_vertex > 4)
	{
		Mat4 temp_mat1,temp_mat2 ;

		if(m_speed_is_zero && m_technique->IsLocalSpace())
		{
			temp_mat1 = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
			Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
			temp_mat2 = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
			Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
		}
		m_buffer_data->DrawBuffer(m_num_vertex, m_num_indices, m_add_mode_power_ratio);
		if(m_speed_is_zero && m_technique->IsLocalSpace())
		{
			Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
			Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW,temp_mat2);
			Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
			Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW,temp_mat1);
		}
	}
}

void CCRibbonParticleRenderer::ResetData()
{
	m_pos_map.clear();
}

void CCRibbonParticleRenderer::NotifyParticleQuota(size_t size)
{
	if (size > 50)
	{
		size = 50;
	}
	if (size == m_particle_quota)
	{
		return;
	}

	CCParticleRenderer::NotifyParticleQuota( size);

	m_chain_count = size;
	m_dirty_element = true;
}

inline void CCRibbonParticleRenderer::SetRibbonUVDirection(RibbonUVDirection uv_dir)
{
	m_uv_dir = uv_dir;
}

inline RibbonUVDirection CCRibbonParticleRenderer::GetRibbonUVDirection()
{
	return m_uv_dir;
}


void CCRibbonParticleRenderer::SetTexture(const std::string& name)
{
	CCParticleRenderer::SetTexture(name);
	if (m_buffer_data != 0)
	{
		m_buffer_data->SetTexturePath(m_texture_full_name);
	}
}

void CCRibbonParticleRenderer::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleRenderer::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	string str;

	if (m_width != DEFAULT_WIDTH)
	{
		str = CCParticleHelper::ToString(m_width);
		child = my_document->NewElement("ribbon_width");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_trail_length != DEFAULT_TRAIL_LEN)
	{
		str = CCParticleHelper::ToString(m_trail_length);
		child = my_document->NewElement("ribbon_length");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}


	if (m_max_element != DEFAULT_MAX_ELEMENT)
	{
		str = CCParticleHelper::ToString((int)m_max_element);
		child = my_document->NewElement("ribbon_max_element");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_uv_dir != DEFAULT_UV_DIR)
	{
		str = CCParticleHelper::ToString((int)m_uv_dir);
		child = my_document->NewElement("ribbon_uv_dir");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_color, DEFAULT_COLOR))
	{
		str = CCParticleHelper::ToString(m_color);
		child = my_document->NewElement("ribbon_colour");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_change_color, DEFAULT_COLOR))
	{
		str = CCParticleHelper::ToString(m_change_color);
		child = my_document->NewElement("ribbon_change_color");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_change_width != DEFAULT_CHANGE_WIDTH)
	{
		str = CCParticleHelper::ToString(m_change_width);
		child = my_document->NewElement("ribbon_change_width");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}

bool CCRibbonParticleRenderer::SetAttribute(const char* type, const char* value)
{
	static Color4F color;
	if (strcmp(type, "ribbon_width") == 0)
	{
		SetWidth(CCParticleHelper::ParseFloat(value));
	}
	else if (strcmp(type, "ribbon_length") == 0)
	{
		SetTrailLength(CCParticleHelper::ParseFloat(value));
	}
	else if (strcmp(type, "ribbon_max_element") == 0)
	{
		SetMaxElement(CCParticleHelper::ParseInt(value));
	}
	else if (strcmp(type, "ribbon_uv_dir") == 0)
	{
		SetRibbonUVDirection((RibbonUVDirection)CCParticleHelper::ParseInt(value));
	}
	else if (strcmp(type, "ribbon_colour") == 0)
	{
		CCParticleHelper::ParseColor(value, &m_color);
	}
	else if (strcmp(type, "ribbon_change_color") == 0)
	{
		CCParticleHelper::ParseColor(value, &m_change_color);
	}
	else if (strcmp(type, "ribbon_change_width") == 0)
	{
		SetChangeWidth(CCParticleHelper::ParseFloat(value));
	}
	else
	{
		return CCParticleRenderer::SetAttribute(type, value);
	}
	return true;
}

bool CCRibbonParticleRenderer::GetAttribute(const char* type, char* value, size_t size)
{
	string str;
	if (strcmp(type, "ribbon_width") == 0)
	{
		str = CCParticleHelper::ToString(GetWidth());
	}
	else if (strcmp(type, "ribbon_length") == 0)
	{
		str =  CCParticleHelper::ToString(GetTrailLength());
	}
	else if (strcmp(type, "ribbon_max_element") == 0)
	{
		str =  CCParticleHelper::ToString((int)GetMaxElement());
	}
	else if (strcmp(type, "ribbon_uv_dir") == 0)
	{
		str =  CCParticleHelper::ToString((int)GetRibbonUVDirection());
	}
	else if (strcmp(type, "ribbon_colour") == 0)
	{
		str = CCParticleHelper::ToString(m_color);
	}
	else if (strcmp(type, "ribbon_change_color") == 0)
	{
		str = CCParticleHelper::ToString(m_change_color);
	}
	else if (strcmp(type, "ribbon_change_width") == 0)
	{
		str = CCParticleHelper::ToString(GetChangeWidth());
	}
	else
	{
		return CCParticleRenderer::GetAttribute(type, value, size);
	}
	strncpy(value, str.c_str(), size);
	return true;
}

void CCRibbonParticleRenderer::CopyAttributesTo(CCParticleRenderer *render)
{
	CCParticleRenderer::CopyAttributesTo(render);
	CCRibbonParticleRenderer* ribbon_render = static_cast<CCRibbonParticleRenderer*>(render);
	ribbon_render->SetWidth(GetWidth());
	ribbon_render->SetTrailLength(GetTrailLength());
	ribbon_render->SetColour(GetColour());
	ribbon_render->SetMaxElement(GetMaxElement());
	ribbon_render->SetChangeColour(GetChangeColour());
	ribbon_render->SetChangeWidth(GetChangeWidth());
	ribbon_render->SetRibbonUVDirection(GetRibbonUVDirection());
}

void CCRibbonParticleRenderer::SetMaterialType(MaterialType type)
{
	CCParticleRenderer::SetMaterialType(type);
	if (m_buffer_data != 0)
	{
		m_buffer_data->SetMaterialType(type);
	}
}

void CCRibbonParticleRenderer::MakeGrey(bool is_grey)
{
	if (m_buffer_data != nullptr)
	{
		m_buffer_data->MakeGrey(is_grey);
	}
}

inline void CCRibbonParticleRenderer::SetWidth(float w)
{
	m_width = w;
}

inline float CCRibbonParticleRenderer::GetWidth()
{
	return m_width;
}

inline void CCRibbonParticleRenderer::SetTrailLength(float len)
{
	m_trail_length = len;
	if (m_trail_length < 32.0f)
	{
		m_trail_length = 32.0f;
	}

	m_elem_length = m_trail_length / (float)m_max_element;
}

inline float CCRibbonParticleRenderer::GetTrailLength()
{
	return m_trail_length;
}

inline void CCRibbonParticleRenderer::SetColour(const Color4F& color)
{
	m_color = color;
}

inline const Color4F& CCRibbonParticleRenderer::GetColour()
{
	return m_color;
}

inline void CCRibbonParticleRenderer::SetChangeColour(const Color4F& color)
{
	m_change_color = color;
}

inline const Color4F& CCRibbonParticleRenderer::GetChangeColour()
{
	return m_change_color;
}

inline void CCRibbonParticleRenderer::SetChangeWidth(float w)
{
	m_change_width = w;
}

inline const float CCRibbonParticleRenderer::GetChangeWidth()
{
	return m_change_width;
}

inline void CCRibbonParticleRenderer::SetMaxElement(size_t max_num)
{
	if (max_num < 5)
	{
		max_num = 5;
	}

	if (max_num > 200)
	{
		max_num = 200;
	}

	m_max_element = max_num;
	m_elem_length = m_trail_length / (float)m_max_element;
	m_dirty_element = true;
}

inline size_t CCRibbonParticleRenderer::GetMaxElement()
{
	return m_max_element;
}

void CCRibbonParticleRenderer::ParticleExpired(CCParticle* particle)
{
	if (particle != 0 && particle->m_index >= 0)
	{
		ClearChain(particle->m_index);
		particle->m_index = -1;
	}
}

bool CCRibbonParticleRenderer::FadeOutParticle(CCParticle* particle, float timeElapsed)
{
	return false;
	/*static float time = 0.3f;

	int count = (int)((float)m_max_element_count * timeElapsed / time);
	RemoveCountChian(particle->m_index, count);

	particle->m_time_live -= timeElapsed;
	if (particle->m_time_live < -time)
	{
	return false;
	}

	return true;*/
}


/////////////////////////////////////////////////////////////////////////////////////////
void CCRibbonParticleRenderer::AddChainElement(size_t chainIndex, const Vec3& pos)
{
	if (chainIndex >= m_chain_count)
	{
		return;
	}

	ChainSegment& seg = m_chain_list[chainIndex];
	if (seg.head == SEGMENT_EMPTY)
	{
		seg.tail = m_max_element - 1;
		seg.head = seg.tail;
	}
	else
	{
		if (seg.head == 0)
		{
			seg.head = m_max_element - 1;
		}
		else
		{
			--seg.head;
		}
		// Run out of elements?
		if (seg.head == seg.tail)
		{
			if (seg.tail == 0)
				seg.tail = m_max_element - 1;
			else
				--seg.tail;
		}
	}

	m_element_list[seg.start + seg.head]->pos = pos;
	m_element_list[seg.start + seg.head]->color = m_color;
	m_element_list[seg.start + seg.head]->uv0.set(0.0f, 0.0f);
	m_element_list[seg.start + seg.head]->uv1.set(0.0f, 0.0f);
	m_element_list[seg.start + seg.head]->width = m_width;
}

bool CCRibbonParticleRenderer::RemoveChainElement(size_t chainIndex)
{
	if (chainIndex >= m_chain_count)
	{
		return false;
	}

	ChainSegment& seg = m_chain_list[chainIndex];
	if (seg.tail == SEGMENT_EMPTY || seg.head == seg.tail)
	{
		return false;
	}

	if (seg.tail == 0)
		seg.tail = m_max_element - 1;
	else
		--seg.tail;

	return true;
}

size_t CCRibbonParticleRenderer::GetNumChainElements(size_t chainIndex) const
{
	if (chainIndex >= m_chain_count)
	{
		return 0;
	}
	const ChainSegment& seg = m_chain_list[chainIndex];

	if (seg.head == SEGMENT_EMPTY)
	{
		return 0;
	}

	if( seg.tail < seg.head )
	{
		return seg.tail - seg.head + m_max_element + 1;
	}
	else
	{
		return seg.tail - seg.head + 1;
	}
}

void CCRibbonParticleRenderer::RemoveCountChian(size_t chainIndex, int count)
{
	for (int i = 0; i < count; ++i)
	{
		if (!RemoveChainElement(chainIndex))
		{
			break;
		}
	}
}

void CCRibbonParticleRenderer::UpdateAllTrail()
{
	map<size_t, Vec3>::iterator it;
	for (size_t s = 0; s < m_chain_list.size(); ++s)
	{
		it = m_pos_map.find(s);
		if (it == m_pos_map.end())
		{
			continue;
		}
		UpdateTrail(s);
	}
}

void CCRibbonParticleRenderer::UpdateColour(float t)
{
	map<size_t, Vec3>::iterator it;
	for (size_t s = 0; s < m_chain_list.size(); ++s)
	{
		it = m_pos_map.find(s);
		ChainSegment& seg = m_chain_list[s];
		if (it == m_pos_map.end())
		{
			continue;
		}

		if (seg.head != SEGMENT_EMPTY && seg.head != seg.tail)
		{
			for(size_t e = seg.head; ; ++e)
			{
				e = e % m_max_element;
				RibbonElement* elem = m_element_list[seg.start + e];
				Color4F temp (m_change_color.r * t, m_change_color.g * t, m_change_color.b * t, m_change_color.a * t);
				CCParticleHelper::SubColour(&elem->color, &elem->color, &temp);
				CCParticleHelper::SaturateColor(&elem->color);
				elem->width = elem->width - (m_change_width * t);
				if (e == seg.tail)
					break;
			}
		}
	}
}

void CCRibbonParticleRenderer::UpdateUV()
{
	static vector<float> dis_list;

	dis_list.clear();
	dis_list.reserve(m_max_element);
	dis_list.resize(m_max_element);

	map<size_t, Vec3>::iterator it;

	Vec3 diff;
	for (size_t s = 0; s < m_chain_list.size(); ++s)
	{
		it = m_pos_map.find(s);
		ChainSegment& seg = m_chain_list[s];
		size_t size = GetNumChainElements(s);
		if (size < 2 || it == m_pos_map.end())
		{
			continue;
		}

		if (seg.head != SEGMENT_EMPTY && seg.head != seg.tail)
		{
			size_t i =0 ;
			float totalDist = 0.0f;
			size_t last = 0;

			for(size_t e = seg.head; ; ++e)
			{
				e = e % m_max_element;
				RibbonElement* elem = m_element_list[seg.start + e];

				if(i == 0)
				{
					dis_list[i] = 0.0f;
				}
				else
				{
					RibbonElement* last_element = m_element_list[seg.start + last];
					Vec3::subtract(elem->pos, last_element->pos,&diff);
					dis_list[i] = dis_list[i-1] + diff.length();
				}

				++i;
				last = e;
				if (e == seg.tail)
					break;
			}

			totalDist = dis_list[i-1];

			if(totalDist > 1e-3f)
				totalDist = 1.f / totalDist;

			i = 0;
			float d;
			for(size_t e = seg.head; ; ++e)
			{
				e = e % m_max_element;
				RibbonElement* elem = m_element_list[seg.start + e];
				d = dis_list[i] * totalDist;

				if (m_uv_dir == RUVD_U)
				{
					elem->uv0.set(0.0f, d);
					elem->uv1.set(1.0f, d);
				}
				else if (m_uv_dir == RUVD_V)
				{
					elem->uv0.set(d, 0.0f);
					elem->uv1.set(d, 1.0f);
				}

				++i;
				if (e == seg.tail)
					break;
			}
		}
	}
}

void CCRibbonParticleRenderer::UpdateElementCount()
{
	size_t old_size = m_element_list.size();

	size_t size = m_chain_count * m_max_element;
	m_element_list.reserve(size);
	m_element_list.resize(size);

	for (size_t i = old_size; i < size; ++i)
	{
		m_element_list[i] = new RibbonElement();
	}

	m_chain_list.resize(m_chain_count);
	for (size_t i = 0; i < m_chain_count; ++i)
	{
		ChainSegment& seg = m_chain_list[i];
		seg.start = i * m_max_element;
		seg.tail = seg.head = SEGMENT_EMPTY;
	}

	if (m_max_vertex_count != 2 * size || m_max_index_count != 6 * size)
	{
		m_max_vertex_count = 4 * size;
		m_max_index_count = 6 * size;
		m_buffer_data->NotitySizeChange(m_max_vertex_count, m_max_index_count);
	}

	m_dirty_element = false;
}

bool CCRibbonParticleRenderer::UpdateData(float t)
{
	if (m_dirty_element || m_technique->GetEmitterDirty())
	{
		UpdateElementCount();
		m_technique->SetEmitterDirty(false);
	}

	UpdateAllTrail();
	UpdateUV();
	UpdateColour(t);
	if(UpdateBuffer())
	{
		return true;
	}

	return false;
}

void CCRibbonParticleRenderer::ClearChain(size_t chainIndex)
{
	if (chainIndex >= m_chain_count)
	{
		return;
	}
	ChainSegment& seg = m_chain_list[chainIndex];
	seg.tail = seg.head = SEGMENT_EMPTY;
}

void CCRibbonParticleRenderer::UpdateTrail(size_t index)
{
	ChainSegment& seg = m_chain_list[index];
	map<size_t, Vec3>::iterator it = m_pos_map.find(index);
	if (it == m_pos_map.end())
	{
		return;
	}

	Vec3 node_pos = it->second;

	if (seg.head == SEGMENT_EMPTY)
	{
		AddChainElement(index, node_pos);
		seg.posR = node_pos;
		return;
	}

	if(IsReduceChainLength(index))
	{
		ReduceChainLength(index);
		return;
	}

	if(seg.posR == node_pos)
	{
		return;
	}

	RibbonElement* headElem = m_element_list[seg.start + seg.head];

	Vec3 diff (0, 0, 0);
	Vec3::subtract(node_pos,headElem->pos,&diff);
	float len = diff.length();
	if (len >= m_elem_length)
	{
		Vec3 n_diff (0.0f, 0.0f, 0.0f);
		n_diff = diff.getNormalized();
		int size = (int)(len / m_elem_length) - 1;

		if (size >= (int)m_max_element)
		{
			size = m_max_element - 1;
		}

		for (int i = 0; i < size; ++i)
		{
			Vec3 pos = n_diff * m_elem_length;
			RibbonElement* headElem = m_element_list[seg.start + seg.head];
			pos = headElem->pos + pos;
			AddChainElement(index, pos);
		}
		seg.posR = node_pos;
	} 
}

void CCRibbonParticleRenderer::ClearAllNodeVisible()
{
	map<size_t, Vec3>::iterator it;
	for (size_t s = 0; s < m_chain_list.size(); ++s)
	{
		it = m_pos_map.find(s);
		if(it == m_pos_map.end())
		{
			ClearChain(s);
		}
	}
}

int CCRibbonParticleRenderer::FindFirstNoneVisible()
{
	map<size_t, Vec3>::iterator it;
	for (size_t s = 0; s < m_chain_list.size(); ++s)
	{
		it = m_pos_map.find(s);
		if(it == m_pos_map.end())
		{
			m_chain_list[s].head = m_chain_list[s].tail = SEGMENT_EMPTY;
			return s;
		}
			
	}

	return -1;
}

bool CCRibbonParticleRenderer::UpdateBuffer()
{
	static Vec3 unit_z (0, 0, 1);
	Vec3 chainTangent;
	map<size_t, Vec3>::iterator it;

	m_num_indices = 0;
	m_num_vertex = 0;

	V3F_C4B_T2F *pQuads = m_buffer_data->GetVertexData();
	if (pQuads == 0)
	{
		return false;
	}

	for (size_t s = 0; s < m_chain_list.size(); ++s)
	{
		ChainSegment& seg = m_chain_list[s];
		it = m_pos_map.find(s);
		if (it == m_pos_map.end())
		{
			continue;
		}
		if (seg.head != SEGMENT_EMPTY && seg.head != seg.tail)
		{
			size_t index = 0;
			size_t size = GetNumChainElements(s);

			size_t laste = seg.head;
			for (size_t e = seg.head; ; ++e)
			{
				e = e % m_max_element;
				RibbonElement* elem = m_element_list[seg.start + e];
				Color4B color((GLubyte)(elem->color.r*255), (GLubyte)(elem->color.g*255), (GLubyte)(elem->color.b*255), (GLubyte)(elem->color.a*255));

				size_t nexte = e + 1;
				if (nexte == m_max_element)
					nexte = 0;

				Vec3 vPerpendicular;
				vPerpendicular.set(0.0, 0.0, 0.0);

				if (GetNumChainElements(s) >= 3)
				{
					if (e == seg.head)
					{
						Vec3::subtract(m_element_list[nexte + seg.start]->pos,elem->pos,&chainTangent);
					}
					else if (e == seg.tail)
					{
						Vec3::subtract(elem->pos,m_element_list[laste + seg.start]->pos,&chainTangent);
					}
					else
					{
						Vec3::subtract(m_element_list[nexte + seg.start]->pos,m_element_list[laste + seg.start]->pos,&chainTangent);
					}
					Vec3::cross(chainTangent,unit_z,&vPerpendicular);
					vPerpendicular.normalize();
				}
				else
				{
					chainTangent.set(0.0, 0.0, 0.0);
				}
				vPerpendicular = vPerpendicular * (elem->width * 0.5f);

				if(!m_technique->IsLocalSpace())
				{
					Mat4 ralation_mat;
					ralation_mat.setIdentity();

					Mat4 outside_mat;
					outside_mat.setIdentity();
					outside_mat=m_technique->GetTechToWorldMat();

					ralation_mat.m[0]=outside_mat.m[0];
					ralation_mat.m[1]=outside_mat.m[1];
					ralation_mat.m[4]=outside_mat.m[4];
					ralation_mat.m[5]=outside_mat.m[5];
					ralation_mat.transformPoint(vPerpendicular,&vPerpendicular);
				}

				Vec3 pos0;
				Vec3::subtract(elem->pos,vPerpendicular,&pos0);
				Vec3 pos1;

				Vec3::add(elem->pos,vPerpendicular,&pos1);
				
				// 升级到3.2 add by zhuyi 2014-09-01
				if(index>1 && m_num_vertex>=2)
				{
					pQuads[m_num_vertex].vertices.x = pQuads[m_num_vertex-2].vertices.x;
					pQuads[m_num_vertex].vertices.y =pQuads[m_num_vertex-2].vertices.y;
					pQuads[m_num_vertex].vertices.z = pQuads[m_num_vertex-2].vertices.z;
					pQuads[m_num_vertex].colors = pQuads[m_num_vertex-2].colors;
					pQuads[m_num_vertex].texCoords.u =pQuads[m_num_vertex-2].texCoords.u;
					pQuads[m_num_vertex].texCoords.v = pQuads[m_num_vertex-2].texCoords.v ;
					m_num_vertex+= 1;

					pQuads[m_num_vertex].vertices.x = pQuads[m_num_vertex-2].vertices.x;
					pQuads[m_num_vertex].vertices.y =pQuads[m_num_vertex-2].vertices.y;
					pQuads[m_num_vertex].vertices.z = pQuads[m_num_vertex-2].vertices.z;
					pQuads[m_num_vertex].colors = pQuads[m_num_vertex-2].colors;
					pQuads[m_num_vertex].texCoords.u =pQuads[m_num_vertex-2].texCoords.u;
					pQuads[m_num_vertex].texCoords.v = pQuads[m_num_vertex-2].texCoords.v ;
					m_num_vertex+= 1;

				}
				//end
					
				pQuads[m_num_vertex].vertices.x = pos0.x;
				pQuads[m_num_vertex].vertices.y = pos0.y;
				pQuads[m_num_vertex].vertices.z = pos0.z;
				pQuads[m_num_vertex].colors = color;
				pQuads[m_num_vertex].texCoords.u = elem->uv0.x;
				pQuads[m_num_vertex].texCoords.v = elem->uv0.y;
				m_num_vertex+= 1;

				pQuads[m_num_vertex].vertices.x = pos1.x;
				pQuads[m_num_vertex].vertices.y = pos1.y;
				pQuads[m_num_vertex].vertices.z = pos1.z;
				pQuads[m_num_vertex].colors = color;
				pQuads[m_num_vertex].texCoords.u = elem->uv1.x;
				pQuads[m_num_vertex].texCoords.v = elem->uv1.y;
				m_num_vertex += 1;

				++index;
				if (e == seg.tail)
					break;
				laste = e;
			}
		}
	}
	return true;
}

//by ZhuYi 2015-1-4 实现拖尾渲染器粒子长度逐渐变短，直到消失
bool CCRibbonParticleRenderer::IsReduceChainLength(int index)
{
	std::list<CCParticle*> particle_list = *m_technique->GetActiveList();
	std::list<CCParticle*>::iterator iter = particle_list.begin();
	float scale=1;

	for(iter;iter!=particle_list.end();++iter)
	{
		if((*iter)->m_index == index && m_chain_list[(*iter)->m_index].head!=SEGMENT_EMPTY && m_chain_list[(*iter)->m_index].tail!=SEGMENT_EMPTY)
		{
			scale = (*iter)->m_time_live / (*iter)->m_total_live;
			break;
		}
	}
	return scale<0.1?true:false;
}

void CCRibbonParticleRenderer::ReduceChainLength(int index)
{
	ChainSegment& seg = m_chain_list[index];

	if (index >= m_chain_count || GetNumChainElements(index) < 2)
	{
		return;
	}

	for(int i=0;i<5;++i)
	{
		if(seg.tail == 0)
		{
			seg.tail = m_max_element-1;

			if(seg.head==m_max_element-1)
				seg.tail=0;
		}
		else
		{
			--seg.tail;
		}

		if(seg.tail == seg.head)
		{
			++seg.tail;
			break;
		}
	}
}
//end

NS_CC_END