
#include "billboard.h"
#include "renderer.h"

#include "rendererbuffer.h"
#include "particle/technique.h"
#include "particle/particlesystem.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"
#include "particle/dynamicattribute.h"
#include "particle/simplespline.h"
#include "particle/particlefactory.h"

#include "cocos/base/ccTypes.h"
#include "cocos/2d//CCNode.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"

NS_CC_BEGIN

const std::string CCBillboardParticleRenderer::DEFAULT_TYPE = "point";
const std::string CCBillboardParticleRenderer::DEFAULT_ORIGIN = "center";
const std::string CCBillboardParticleRenderer::DEFAULT_ROT = "vertex";

CCBillboardParticleRenderer::CCBillboardParticleRenderer(CCParticleTechnique *pt, const std::string& texture_suffix):
CCParticleRenderer(pt, texture_suffix),
	m_billboard_type_str(DEFAULT_TYPE),
	m_billboard_origin(DEFAULT_ORIGIN),
	m_billboard_rot_type(DEFAULT_ROT),
	m_billboard_type(BBT_ORIENTED_COMMON),
	m_origin_type(BBO_CENTER),
	m_rotation_type(BBR_VERTEX),
	m_buffer_data(0),
	m_cur_left(0),
	m_cur_right(0),
	m_cur_top(0),
	m_cur_bottom(0),
	m_num_indices(0),
	m_num_vertices(0)
{
	m_type = "Billboard";
	SetBillboardType(DEFAULT_TYPE);
	m_common_direction.set(0, 0, 1);//UNIT_Z

	m_common_up_vector.set(0, 1, 0);	//UNIT_Y
	
	m_result_direction.set( 0, 0, 1);	//UNIT_Z
	m_result_up_vector.set(0, 1, 0);	//UNIT_Y

	if (!m_technique->IsTemplate())
	{
		m_buffer_data = new CCParticleRenderBuffer(false);
	}
}

CCBillboardParticleRenderer::~CCBillboardParticleRenderer()
{
	if (m_buffer_data != 0)
	{
		delete m_buffer_data;
		m_buffer_data = 0;
	}
}


bool CCBillboardParticleRenderer::PreDraw()
{
	if ( m_technique == 0)
	{
		return false;
	}

	m_num_vertices = 0;
	m_num_indices = 0;

	if( !IsAxisAssociateParticleData())
	{
		CalculateCurAxes(0);
	}

	return true;
}

void CCBillboardParticleRenderer::Draw(std::list<CCParticle*>& current_particles)
{
	if (m_buffer_data == 0 || m_buffer_data->getTexture() == 0)
	{
		return;
	}

	size_t size = current_particles.size();
	size = ((m_particle_quota < size) ? m_particle_quota : size);
	if (size == 0)
	{
		return;
	}

	if (!PreDraw())
	{
		return;
	}

	size_t i = 0;
	for (std::list<CCParticle*>::reverse_iterator it = current_particles.rbegin(); it != current_particles.rend() && i < size; ++it)
	{
		if ((*it)->m_type != CCParticle::Visual)
		{
			continue;
		}
		InjectBillboard(*it);
		++i;
	}

	if (m_num_indices > 0)
	{
		m_buffer_data->DrawBuffer(m_num_vertices, m_num_indices, m_add_mode_power_ratio);
	}
}

void CCBillboardParticleRenderer::InjectBillboard(CCParticle* particle)
{
	// 粒子的X轴方向和Y轴方向
	if( IsOrientPerBillboard())
	{
		CalculateCurAxes( particle);
	}

	float width = particle->m_width;
	float height = particle->m_height;

	CalculateCurOffsets(width , height);

	///////////////////////////////
	// 生成顶点并写入硬件缓冲
	// 把传入的四个坐标绕旋转轴旋转radian弧度
	///////////////////////////////
	Vec3	postions[4];

	Vec3 dir_x;
	Vec3 dir_y;
	dir_x=m_cur_x_dir;
	dir_y=m_cur_y_dir;

	if (particle->m_angle != 0.0f)
	{
		Vec3 axis;
		Vec3::cross(dir_y, dir_x,&axis);
		axis.normalize();

		Quaternion rotation;
		Quaternion::QuaternionRotationAxis(&rotation, &axis, -particle->m_angle * kmPIOver180);

		Quaternion::QuaternionMultiplyVec3(&dir_x, &rotation, &dir_x);
		Quaternion::QuaternionMultiplyVec3(&dir_y, &rotation, &dir_y);
	}

	Vec3 temp1,temp2;
	temp1 = dir_x * m_cur_left;
	temp2 = dir_y * m_cur_top;
	Vec3::add(temp1,temp2,&postions[0]);								        // l_t

	temp1 = dir_x * m_cur_right;
	temp2 = dir_y * m_cur_top;
	Vec3::add(temp1, temp2,&postions[1]);									// r_t

	temp1 = dir_x * m_cur_right;
	temp2 = dir_y * m_cur_bottom;
	Vec3::add(temp1,temp2,&postions[2]);									// r_b

	temp1 = dir_x * m_cur_left;
	temp2 = dir_y * m_cur_bottom;
	Vec3::add(temp1,temp2,&postions[3]);									// l_b

	//粒子到世界坐标的缩放和旋转
	if(!m_technique->IsLocalSpace())
	{
		Mat4 ratation_mat;
		ratation_mat.setIdentity();
		Mat4 outside_m=m_technique->GetTechToWorldMat();
		ratation_mat.m[0]=outside_m.m[0];
		ratation_mat.m[5]=outside_m.m[5];
		ratation_mat.m[1]=outside_m.m[1];
		ratation_mat.m[4]=outside_m.m[4];
		
		for(int i=0;i<4;++i)
		{
			ratation_mat.transformPoint(postions[i],&postions[i]);
		} 
	}

	// 四个uv
	Vec2 uvs[4];
	uvs[0].set(particle->m_left, particle->m_top);
	uvs[1].set(particle->m_right, particle->m_top);
	uvs[2].set(particle->m_right, particle->m_bottom);
	uvs[3].set(particle->m_left, particle->m_bottom);

	// 复制顶点数据到硬件缓冲
	V3F_C4B_T2F* cur_data = m_buffer_data->GetVertexData();
	if (cur_data == 0)
	{
		return;
	}

	// by zhuyi 2014-09-01 为与3.2版本的索引规律相同，修改矩形四个顶点排列顺序。
	Vec3::add(postions[0],particle->m_position,&postions[0]);
	cur_data[m_num_vertices].vertices.x = postions[0].x; // l_t
	cur_data[m_num_vertices].vertices.y = postions[0].y;
	cur_data[m_num_vertices].vertices.z = postions[0].z;
	cur_data[m_num_vertices].colors.r = particle->m_color.r * 255;
	cur_data[m_num_vertices].colors.g = particle->m_color.g * 255;
	cur_data[m_num_vertices].colors.b = particle->m_color.b * 255;
	cur_data[m_num_vertices].colors.a = particle->m_color.a * 255;
	cur_data[m_num_vertices].texCoords.u = uvs[0].x;
	cur_data[m_num_vertices].texCoords.v = uvs[0].y;
	m_num_vertices += 1;

	Vec3::add(postions[3],particle->m_position,&postions[3]); // l_b
	cur_data[m_num_vertices].vertices.x = postions[3].x;
	cur_data[m_num_vertices].vertices.y = postions[3].y;
	cur_data[m_num_vertices].vertices.z = postions[3].z;
	cur_data[m_num_vertices].colors.r = particle->m_color.r * 255;
	cur_data[m_num_vertices].colors.g = particle->m_color.g * 255;
	cur_data[m_num_vertices].colors.b = particle->m_color.b * 255;
	cur_data[m_num_vertices].colors.a = particle->m_color.a * 255;
	cur_data[m_num_vertices].texCoords.u = uvs[3].x;
	cur_data[m_num_vertices].texCoords.v = uvs[3].y;
	m_num_vertices += 1;

	Vec3::add(postions[1],particle->m_position,&postions[1]);
	cur_data[m_num_vertices].vertices.x = postions[1].x; // r_t
	cur_data[m_num_vertices].vertices.y = postions[1].y;
	cur_data[m_num_vertices].vertices.z = postions[1].z;
	cur_data[m_num_vertices].colors.r = particle->m_color.r * 255;
	cur_data[m_num_vertices].colors.g = particle->m_color.g * 255;
	cur_data[m_num_vertices].colors.b = particle->m_color.b * 255;
	cur_data[m_num_vertices].colors.a = particle->m_color.a * 255;
	cur_data[m_num_vertices].texCoords.u = uvs[1].x;
	cur_data[m_num_vertices].texCoords.v = uvs[1].y;
	m_num_vertices += 1;

	Vec3::add(postions[2],particle->m_position,&postions[2]);
	cur_data[m_num_vertices].vertices.x = postions[2].x; // r_b
	cur_data[m_num_vertices].vertices.y = postions[2].y;
	cur_data[m_num_vertices].vertices.z = postions[2].z;
	cur_data[m_num_vertices].colors.r = particle->m_color.r * 255;
	cur_data[m_num_vertices].colors.g = particle->m_color.g * 255;
	cur_data[m_num_vertices].colors.b = particle->m_color.b * 255;
	cur_data[m_num_vertices].colors.a = particle->m_color.a * 255;
	cur_data[m_num_vertices].texCoords.u = uvs[2].x;
	cur_data[m_num_vertices].texCoords.v = uvs[2].y;
	m_num_vertices += 1;
	//end
	
	m_num_indices += 6;
}

void CCBillboardParticleRenderer::CalculateCurOffsets( float w, float h)
{
	switch( m_origin_type)
	{
	case BBO_TOP_LEFT:
		m_cur_left	= -w;
		m_cur_right	= 0.0f;
		m_cur_top	= h;
		m_cur_bottom = 0.0f;
		break;

	case BBO_TOP_CENTER:
		m_cur_left	= -0.5f * w;
		m_cur_right = 0.5f * w;
		m_cur_top	= h;
		m_cur_bottom = 0.0f;
		break;

	case BBO_TOP_RIGHT:
		m_cur_left	= 0.0f;
		m_cur_right = w;
		m_cur_top	= h;
		m_cur_bottom = 0.0f;
		break;

	case BBO_CENTER_LEFT:
		m_cur_left	= -w;
		m_cur_right	= 0.0f;
		m_cur_top	= 0.5f * h;
		m_cur_bottom = -0.5f * h;
		break;

	case BBO_CENTER:
		m_cur_left	= -0.5f * w;
		m_cur_right = 0.5f * w;
		m_cur_top	= 0.5f * h;
		m_cur_bottom = -0.5f * h;
		break;

	case BBO_CENTER_RIGHT:
		m_cur_left	= 0.0f;
		m_cur_right = w;
		m_cur_top	= 0.5f * h;
		m_cur_bottom = -0.5f * h;
		break;

	case BBO_BOTTOM_LEFT:
		m_cur_left	= -w;
		m_cur_right	= 0.0f;
		m_cur_top	= 0;
		m_cur_bottom = -h;
		break;

	case BBO_BOTTOM_CENTER:
		m_cur_left	= -0.5f * w;
		m_cur_right = 0.5f * w;
		m_cur_top	= 0.0f;
		m_cur_bottom = -h;
		break;

	case BBO_BOTTOM_RIGHT:
		m_cur_left	= 0.0f;
		m_cur_right = w;
		m_cur_top	= 0.0f;
		m_cur_bottom = -h;
		break;
	}
}


void CCBillboardParticleRenderer::CalculateCurAxes( const CCParticle* particle)
{
	switch (m_billboard_type)
	{
	case BBT_POINT:
		m_cur_x_dir.set(1, 0, 0);
		m_cur_y_dir.set(0, 1, 0);
		break;

	case BBT_ORIENTED_COMMON:
		{
			Vec3 tmp_z (0,0,-1);
			m_cur_y_dir = m_result_direction;
			m_cur_y_dir.normalize();
			Vec3::cross(tmp_z, m_cur_y_dir,&m_cur_x_dir);
			m_cur_x_dir.normalize();
		}
		break;

	case BBT_ORIENTED_SELF:
		{
			Vec3 tmp_z (0,0,1);
			m_cur_y_dir = particle->m_direction;
			m_cur_y_dir.normalize();
			Vec3::cross(m_cur_y_dir,tmp_z,&m_cur_x_dir);
			m_cur_x_dir.normalize();
		}
		break;

	case BBT_PERPENDICULAR_COMMON:
		{
			Vec3::cross(m_result_up_vector, m_result_direction,&m_cur_x_dir);
			m_cur_x_dir.normalize();
			Vec3::cross(m_result_direction, m_cur_x_dir,&m_cur_y_dir);
			m_cur_y_dir.normalize();
		}
		break;

	case BBT_PERPENDICULAR_SELF:
		{
			Vec3::cross(m_result_up_vector, particle->m_direction,&m_cur_x_dir);
			m_cur_x_dir.normalize();
			Vec3::cross(particle->m_direction,m_cur_x_dir,&m_cur_y_dir);
			m_cur_y_dir.normalize();
		}
		break;
	}
}

bool CCBillboardParticleRenderer::IsOrientPerBillboard() const
{
	return ((m_billboard_type != BBT_PERPENDICULAR_COMMON && m_billboard_type != BBT_POINT));
}

bool CCBillboardParticleRenderer::IsOrientToCamera() const
{
	return ( m_billboard_type == BBT_POINT || 
		m_billboard_type == BBT_ORIENTED_COMMON ||
		m_billboard_type == BBT_ORIENTED_SELF);
}

bool CCBillboardParticleRenderer::IsAxisAssociateParticleData() const
{
	bool ret = m_billboard_type == BBT_ORIENTED_SELF;
	ret = ret || m_billboard_type == BBT_PERPENDICULAR_SELF;
	ret = ret || (IsOrientPerBillboard() && IsOrientToCamera());
	return ret;
}

void CCBillboardParticleRenderer::NotifyParticleQuota(size_t size)
{
	if( size == m_particle_quota)
	{
		return;
	}

	CCParticleRenderer::NotifyParticleQuota( size);

	if (!m_technique->IsTemplate() && m_buffer_data != 0)
	{
		size_t vertices_num = size * 4;
		size_t indices_num	= size * 6;

		m_buffer_data->NotitySizeChange(vertices_num, indices_num);
	}
}

void CCBillboardParticleRenderer::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleRenderer::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	if (m_billboard_type_str != DEFAULT_TYPE)
	{
		child = my_document->NewElement("billboard_type");
		content = my_document->NewText(m_billboard_type_str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_billboard_origin != DEFAULT_ORIGIN)
	{
		child = my_document->NewElement("billboard_origin");
		content = my_document->NewText(m_billboard_origin.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_billboard_rot_type != DEFAULT_ROT)
	{
		child = my_document->NewElement("billboard_rot_type");
		content = my_document->NewText(m_billboard_rot_type.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	Vec3 vec3_unit_z;
	vec3_unit_z.set(0, 0, 1);
	if (!CCParticleHelper::IsEqual(m_common_direction, vec3_unit_z))
	{
		child = my_document->NewElement("billboard_common_dir");
		content = my_document->NewText(CCParticleHelper::ToString( m_common_direction).c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	Vec3 vec3_unit_y(0, 1, 0);
	if (!CCParticleHelper::IsEqual(m_common_up_vector, vec3_unit_y))
	{
		child = my_document->NewElement("billboard_common_up_dir");
		content = my_document->NewText(CCParticleHelper::ToString( m_common_up_vector).c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}

bool CCBillboardParticleRenderer::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "billboard_type") == 0)
	{
		SetBillboardType(value);
	}
	else if (strcmp(type, "billboard_origin") == 0)
	{
		SetOrigionType(value);
	}
	else if (strcmp(type, "billboard_rot_type") == 0)
	{
		SetRotationType(value);
	}
	else if( strcmp(type, "billboard_common_dir") == 0)
	{
		Vec3 common_dir;
		CCParticleHelper::ParseVec3(value, &common_dir);
		SetCommonDirection(common_dir);
	}
	else if( strcmp( type, "billboard_common_up_dir") == 0)
	{
		Vec3 common_up_dir;
		CCParticleHelper::ParseVec3(value, &common_up_dir);
		SetCommonUpVector(common_up_dir);
	}
	else
	{
		return CCParticleRenderer::SetAttribute(type, value);
	}
	return true;
}

void CCBillboardParticleRenderer::CopyAttributesTo(CCParticleRenderer *render)
{
	CCParticleRenderer::CopyAttributesTo(render);
	CCBillboardParticleRenderer* billboard_render = static_cast<CCBillboardParticleRenderer*>(render);
	billboard_render->SetBillboardType(m_billboard_type_str);
	billboard_render->SetOrigionType(m_billboard_origin);
	billboard_render->SetRotationType(m_billboard_rot_type);
	billboard_render->SetCommonDirection(m_common_direction);
	billboard_render->SetCommonUpVector(m_common_up_vector);
}

bool CCBillboardParticleRenderer::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str;
	if (strcmp(type, "billboard_type") == 0)
	{
		str = m_billboard_type_str;
	}
	else if (strcmp(type, "billboard_origin") == 0)
	{
		str = m_billboard_origin;
	}
	else if (strcmp(type, "billboard_rot_type") == 0)
	{
		str = m_billboard_rot_type;
	}
	else if( strcmp(type, "billboard_common_dir") == 0)
	{
		str = CCParticleHelper::ToString( m_common_direction);
	}
	else if( strcmp(type, "billboard_common_up_dir") == 0)
	{
		str = CCParticleHelper::ToString( m_common_up_vector);
	}
	else
	{
		return CCParticleRenderer::GetAttribute(type, value, size);
	}
	strncpy(value, str.c_str(), size);
	return true;
}

void CCBillboardParticleRenderer::SetTexture(const std::string& name)
{
	CCParticleRenderer::SetTexture(name);
	if (m_buffer_data != 0)
	{
		m_buffer_data->SetTexturePath(m_texture_full_name);
	}
}

void CCBillboardParticleRenderer::SetOrigionType(const std::string& type)
{
	m_billboard_origin = type;
	if (type == "top_left")
	{
		m_origin_type = BBO_TOP_LEFT;
	}
	else if (type =="top_center")
	{
		m_origin_type = BBO_TOP_CENTER;
	}
	else if (type =="top_right")
	{
		m_origin_type = BBO_TOP_RIGHT;
	}
	else if (type =="center_left")
	{
		m_origin_type = BBO_CENTER_LEFT;
	}
	else if (type =="center")
	{
		m_origin_type = BBO_CENTER;
	}
	else if (type =="center_right")
	{
		m_origin_type = BBO_CENTER_RIGHT;
	}
	else if (type =="bottom_left")
	{
		m_origin_type = BBO_BOTTOM_LEFT;
	}
	else if (type =="bottom_center")
	{
		m_origin_type = BBO_BOTTOM_CENTER;
	}
	else if (type =="bottom_right")
	{
		m_origin_type = BBO_BOTTOM_RIGHT;
	}
}

void CCBillboardParticleRenderer::SetRotationType(const std::string& type)
{
	BillboardRotationType r;
	if (type == "vertex")
	{
		r = BBR_VERTEX;
	}
	else if (type == "texcoord")
	{
		r = BBR_TEXCOORD;
	}
	m_billboard_rot_type = type;
	m_rotation_type = r;
}

void CCBillboardParticleRenderer::SetBillboardType(const std::string& type)
{
	m_billboard_type_str = type;
	m_billboard_type = BBT_POINT;

	if (type == "point")
	{
		m_billboard_type = BBT_POINT;
	}
	else if (type == "oriented_common")
	{
		m_billboard_type = BBT_ORIENTED_COMMON;
	}
	else if (type == "oriented_self")
	{
		m_billboard_type = BBT_ORIENTED_SELF;
	}
	else if (type == "perpendicular_common")
	{
		m_billboard_type = BBT_PERPENDICULAR_COMMON;
	}
	else if (type == "perpendicular_self")
	{
		m_billboard_type = BBT_PERPENDICULAR_SELF;
	}
	else
	{
		m_billboard_type_str = "point";
	}
}

void CCBillboardParticleRenderer::SetMaterialType(MaterialType type)
{
	CCParticleRenderer::SetMaterialType(type);
	if (m_buffer_data != 0)
	{
		m_buffer_data->SetMaterialType(type);
	}
}

void CCBillboardParticleRenderer::MakeGrey(bool is_grey)
{
	if (m_buffer_data != nullptr)
	{
		m_buffer_data->MakeGrey(is_grey);
	}
}

void CCBillboardParticleRenderer::SetCommonUpVector(const Vec3& common_up_vec)
{
	if (m_common_up_vector == common_up_vec)
	{
		return;
	}

	m_common_up_vector.set(common_up_vec);
	// 转换到BillboardSet局部空间的旋转
	if( m_technique->IsLocalSpace())
	{
		m_result_up_vector.set(m_common_up_vector);
	}
	else
	{
		const Mat4& outside_mat = m_technique->GetTechToWorldMat();
		outside_mat.transformVector(m_common_up_vector, &m_result_up_vector);
	}

}
void CCBillboardParticleRenderer::SetCommonDirection(const Vec3& common_dir)
{
	if (m_common_direction == common_dir)
	{
		return;
	}

	m_common_direction.set(common_dir);
	if (m_technique->IsLocalSpace())
	{
		m_result_direction.set(m_common_direction);
	}
	else
	{
		const Mat4& outside_mat = m_technique->GetTechToWorldMat();
		outside_mat.transformVector(m_common_direction, &m_result_direction);
	}
}


NS_CC_END