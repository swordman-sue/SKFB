#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "CCMyMesh.h"
#include "CCMeshSerializer.h"
#include "bytebuffer.h"

NS_CC_BEGIN

CCMyMesh::CCMyMesh( const std::string& res_name)
	: BaseObj(res_name)
	, m_vertex_count(0)
	, m_index_count(0)
	, m_vertex_buffer_size(0)
	, m_vertex_membuffer_ptr(0)
	, m_index_buffer_size(0)
	, m_index_membuffer_ptr(0)
	, m_has_color(false)
	, m_vertex_size(0)
{
}

CCMyMesh::~CCMyMesh()
{
	m_vertex_elements.clear();
	m_vertex_weights_array.clear();

	DestroyVertexMemBuffer();
	DestroyIndexMemBuffer();
	Unload();
}



void CCMyMesh::OnCreated()
{
	if( m_res_stream->size() > 0)
	{
		MeshSerializer serilizer;
		serilizer.importMesh(m_res_stream, this);
	}
}


void CCMyMesh::AddVertexElement( unsigned short source, unsigned short offset, VertexElementType ele_type, VertexElementSemantic ele_usage, unsigned char usage_index )
{
	m_vertex_elements.push_back( VertexElement( source, offset, ele_type, ele_usage, usage_index));
	m_vertex_size += VertexElement::GetElementSize(ele_type);
	if (ele_usage == VES_DIFFUSE)
	{
		m_has_color = true;
	}
}

unsigned short CCMyMesh::GetVertexSize( unsigned short stream_index) const
{
	unsigned short vertex_size = 0;
	for( size_t i = 0; i< m_vertex_elements.size(); ++i)
	{
		if( m_vertex_elements[i].stream_id == stream_index)
		{
			vertex_size += VertexElement::GetElementSize( m_vertex_elements[i].element_type);
		}
	}
	return vertex_size;
}

void CCMyMesh::MergeVertexIntoAABB( float x, float y, float z )
{
	//m_aabb.merge( Vector3( x, y, z));
}

const VertexElement* CCMyMesh::GetVertexElement( VertexElementSemantic usage, unsigned char usage_index/*=0*/ ) const
{
	for( size_t i = 0; i < m_vertex_elements.size(); ++i)
	{
		const VertexElement& ele = m_vertex_elements[i];
		if( ele.element_usage == VES_POSITION && ele.usage_index == usage_index)
		{
			return &ele;
		}
	}
	return 0;
}

void CCMyMesh::OnSerializFinished()
{
// 	m_vertex_buffer_id = RenderService::getSingleton().VertexBufferCreate(m_vertex_count, GetVertexSize(), true);
// 	RenderService::getSingleton().VertexBufferUpdate(m_vertex_buffer_id, m_vertex_membuffer_ptr, m_vertex_buffer_size);
// 
// 	m_index_buffer_id = RenderService::getSingleton().IndexBufferCreate(m_index_count, GetIndexSize(), true);
// 	RenderService::getSingleton().IndexBufferUpdate(m_index_buffer_id, m_index_membuffer_ptr, m_index_buffer_size);
}

void CCMyMesh::CreateVeretexMemBuffer( unsigned int buffer_bytes )
{
	DestroyVertexMemBuffer();
	m_vertex_membuffer_ptr = (unsigned char*)malloc(buffer_bytes);
	m_vertex_buffer_size = buffer_bytes;
}

void CCMyMesh::CreateIndexMemBuffer( unsigned int buffer_bytes )
{
	DestroyIndexMemBuffer();
	m_index_membuffer_ptr = (unsigned char*)malloc(buffer_bytes);
	m_index_buffer_size = buffer_bytes;
}

void CCMyMesh::DestroyVertexMemBuffer()
{
	if( m_vertex_membuffer_ptr != 0)
	{
		free( m_vertex_membuffer_ptr);
		m_vertex_membuffer_ptr = 0;
		m_vertex_buffer_size = 0;
	}
}

void CCMyMesh::DestroyIndexMemBuffer()
{
	if( m_index_membuffer_ptr != 0)
	{
		free(m_index_membuffer_ptr);
		m_index_membuffer_ptr = 0;
		m_index_buffer_size = 0;
	}
}

void CCMyMesh::AddBoneAssignment( unsigned int vertex_idx, unsigned int bone_idx, float weight)
{
	if( vertex_idx >= m_vertex_weights_array.size())
	{
		m_vertex_weights_array.reserve( vertex_idx + 1);
		m_vertex_weights_array.resize( vertex_idx + 1);
	}
	VertexWeights& vw = m_vertex_weights_array[vertex_idx];
	vw.AddWeight( bone_idx, weight);
}

void CCMyMesh::SetSkeletonName( const std::string& skel_name )
{
	size_t ls_pos = m_res_name.find_last_of('/');
	if( ls_pos == std::string::npos)
	{
		return;
	}
	std::string full_skel_name = m_res_name.substr(0, ls_pos + 1) + skel_name;
	m_skeleton_name = full_skel_name;
}

bool CCMyMesh::HasSkeleton() const
{
	return !m_skeleton_name.empty();
}

bool CCMyMesh::AnimateVertices( const Mat4* bone_mats, size_t mats_num, unsigned char* out_buffer, size_t buffer_sz )
{
	if( buffer_sz != m_vertex_buffer_size)
	{
		return false;
	}
		
	size_t vertex_sz = GetVertexSize();
	unsigned char* src_buffer = m_vertex_membuffer_ptr;
	unsigned char* dst_buffer = out_buffer;
	for( size_t i = 0; i < m_vertex_count; ++i)
	{
		float* pos_buff = (float*)(void*)src_buffer;
		if( i < m_vertex_weights_array.size())
		{
			Vec3 src_pos;
			src_pos.x = *pos_buff++;
			src_pos.y = *pos_buff++;
			src_pos.z = *pos_buff++;
			Vec3 dst_pos (0, 0, 0);
			Vec3 result_pos(0, 0, 0);

			// 计算受骨骼影响后的顶点坐标  
			// Ogre::OptimisedUtilGeneral::softwareVertexSkinning
			const VertexWeights& vw = m_vertex_weights_array[i];
			for( size_t j = 0; j < MAX_BLEND_WEIGHTS; ++j)
			{
				unsigned int bone_idx = vw.bone_indices[j];
				float bone_weit = vw.bone_weights[j];

				if( bone_idx < mats_num && bone_weit > 1e-6f)
				{
					dst_pos.set(src_pos);
					bone_mats[bone_idx].transformPoint(&dst_pos);
					dst_pos.scale(bone_weit);
					Vec3::add(result_pos, dst_pos,&result_pos);
				}
				else
				{
					break;
				}
			}

			// 把新顶点写入dst_buffer
			float* dst_p = (float*)(void*)dst_buffer;
			*dst_p++ = result_pos.x;
			*dst_p++ = result_pos.y;
			*dst_p++ = result_pos.z;
		}

		src_buffer += vertex_sz;
		dst_buffer += vertex_sz;
	}
	return true;
}

NS_CC_END