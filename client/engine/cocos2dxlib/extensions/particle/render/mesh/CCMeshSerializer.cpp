#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "CCMyMesh.h"
#include "bytebuffer.h"
#include "CCMeshSerializer.h"
#include "CCMeshTrunkDef.h"
#include <assert.h>

NS_CC_BEGIN

const unsigned short HEADER_CHUNK_ID = 0x1000;

MeshSerializer::MeshSerializer()
{
	mVersion = "[MeshSerializer_v1.41]";
}

MeshSerializer::~MeshSerializer()
{
	ClearTempData();
}

bool MeshSerializer::importMesh(ByteBuffer* stream, CCMyMesh* pDest )
{
	ClearTempData();

	if( !readFileHeader( stream))
	{
		return false;
	}

	unsigned short streamID;
	while(!eof(stream))
	{
		bool mesh_readed = false;
		streamID = readChunk( stream);
		switch( streamID)
		{
		case M_MESH:
			readMesh( stream, pDest);				
			mesh_readed = true;
			break;
		}
		if( mesh_readed)
		{
			break;
		}
	}
	return true;

}

void MeshSerializer::readMesh(ByteBuffer* stream, CCMyMesh* pMesh )
{
	skip(stream, sizeof( bool));	// 跳过是否有骨骼动画的标志

	unsigned short streamID;
	// Find all substreams
	if (!eof(stream))
	{
		streamID = readChunk(stream);
		while(!eof(stream) &&
			(streamID == M_GEOMETRY ||
			streamID == M_SUBMESH ||
			streamID == M_MESH_SKELETON_LINK ||
			streamID == M_MESH_BONE_ASSIGNMENT ||
			streamID == M_MESH_BOUNDS ||
			streamID == M_SUBMESH_NAME_TABLE))
		{
			switch(streamID)
			{
			case M_GEOMETRY:
			case M_MESH_BONE_ASSIGNMENT:
			case M_MESH_BOUNDS:
			case M_SUBMESH_NAME_TABLE:
				skip(stream, mCurrentstreamLen - STREAM_OVERHEAD_SIZE);					
				break;

			case M_SUBMESH:
				readSubMesh(stream, pMesh);	
				break;
			case M_MESH_SKELETON_LINK:
				readSkeletonLink(stream, pMesh);
				break;
			}

			if (!eof(stream))
			{
				streamID = readChunk(stream);
			}
		}
		if (!eof(stream))
		{
			skip(stream, -STREAM_OVERHEAD_SIZE);
		}
	}

	FinishLoad( pMesh);
}

void MeshSerializer::readSubMesh(ByteBuffer* stream, CCMyMesh* pMesh )
{
	unsigned short streamID;

	SubMeshData* sub_mesh_data = AddSubMeshData();

	std::string material = readString(stream);

	// 跳过是否使用sharedVertices的标志，我们不用
	bool use_vertices;
	readBools( stream, &use_vertices, 1);

	unsigned int indexCount = 0;
	readInts(stream, &indexCount, 1);

	// 是否使用32位索引缓冲
	bool index_32bit;
	readBools( stream, &index_32bit, 1);

	assert(index_32bit == false && "stream index must 16 bit!");

	if (indexCount > 0)
	{
		pMesh->m_index_size = sizeof(unsigned short);
		unsigned short* pIdx = static_cast<unsigned short*>( sub_mesh_data->CreateTmpIndexBuffer( indexCount, sizeof(unsigned short)));
		readShorts( stream, pIdx, indexCount);	
	}

	streamID = readChunk(stream);
	if (streamID != M_GEOMETRY)
	{
		assert(false && "Missing geometry data in mesh file MeshSerializerImpl::readSubMesh");
	}
	readGeometry(stream, pMesh, sub_mesh_data);

	// Find all bone assignments, submesh operation, and texture aliases (if present)
	if (!eof(stream))
	{
		streamID = readChunk(stream);
		while(!eof(stream) &&
			(streamID == M_SUBMESH_BONE_ASSIGNMENT ||
			streamID == M_SUBMESH_OPERATION))
		{
			switch( streamID)
			{
			case M_SUBMESH_BONE_ASSIGNMENT:
				readSubMeshBoneAssignment( stream, pMesh, sub_mesh_data);
				break;
			default:
				skip(stream, mCurrentstreamLen - STREAM_OVERHEAD_SIZE);
				break;
			}

			if (!eof(stream))
			{
				streamID = readChunk(stream);
			}
		}

		if (!eof(stream))
		{
			skip(stream, -STREAM_OVERHEAD_SIZE);
		}
	}
}

void MeshSerializer::readGeometry(ByteBuffer* stream, CCMyMesh* pMesh, SubMeshData* temp_data )
{
	unsigned int vertexCount = 0;
	readInts(stream, &vertexCount, 1);

	// Find optional geometry streams
	if (!eof(stream))
	{
		unsigned short streamID = readChunk(stream);
		while(!eof(stream) &&
			(streamID == M_GEOMETRY_VERTEX_DECLARATION ||
			streamID == M_GEOMETRY_VERTEX_BUFFER ))
		{
			switch (streamID)
			{
			case M_GEOMETRY_VERTEX_DECLARATION:
				readGeometryVertexDeclaration(stream, pMesh);
				break;
			case M_GEOMETRY_VERTEX_BUFFER:
				readGeometryVertexBuffer(stream, pMesh, temp_data, vertexCount);
				break;
			}
			// Get next stream
			if (!eof(stream))
			{
				streamID = readChunk(stream);
			}
		}
		if (!eof(stream))
		{
			skip(stream, -STREAM_OVERHEAD_SIZE);
		}
	}
}

void MeshSerializer::readGeometryVertexDeclaration(ByteBuffer* stream, CCMyMesh* pMesh )
{
	if (!eof(stream))
	{
		unsigned short streamID = readChunk(stream);
		bool all_skip = ( pMesh->GetVertexElementCount() > 0);

		while(!eof(stream) &&	(streamID == M_GEOMETRY_VERTEX_ELEMENT ))
		{
			if( !all_skip)
			{
				unsigned short source, offset, index, tmp;
				VertexElementType vType;
				VertexElementSemantic vSemantic;
				// unsigned short source;  	// buffer bind source
				readShorts(stream, &source, 1);
				// unsigned short type;    	// VertexElementType
				readShorts(stream, &tmp, 1);
				vType = static_cast<VertexElementType>(tmp);
				// unsigned short semantic; // VertexElementSemantic
				readShorts(stream, &tmp, 1);
				vSemantic = static_cast<VertexElementSemantic>(tmp);
				// unsigned short offset;	// start offset in buffer in bytes
				readShorts(stream, &offset, 1);
				// unsigned short index;	// index of the semantic
				readShorts(stream, &index, 1);

				pMesh->AddVertexElement(source, offset, vType, vSemantic, (unsigned char)index);
			}
			else
			{
				skip(stream, mCurrentstreamLen - STREAM_OVERHEAD_SIZE);
			}

			if (!eof(stream))
			{
				streamID = readChunk(stream);
			}
		}
		if (!eof(stream))
		{
			skip(stream, -STREAM_OVERHEAD_SIZE);
		}
	}
}

void MeshSerializer::readGeometryVertexBuffer(ByteBuffer* stream, CCMyMesh* pMesh, SubMeshData* temp_data, size_t vertex_count )
{
	unsigned short stream_index, vertex_size;
	// unsigned short bindIndex;	// Index to bind this buffer to
	readShorts(stream, &stream_index, 1);
	// unsigned short vertexSize;	// Per-vertex size, must agree with declaration at this index
	readShorts(stream, &vertex_size, 1);

	// Check for vertex data header
	unsigned short headerID;
	headerID = readChunk(stream);
	if (headerID != M_GEOMETRY_VERTEX_BUFFER_DATA)
	{
		assert(false && "Can't find vertex buffer data area MeshSerializerImpl::readGeometryVertexBuffer");
	}

	if ( pMesh->GetVertexSize( stream_index ) != vertex_size)
	{
		assert(false && "Buffer vertex size does not agree with vertex declaration MeshSerializerImpl::readGeometryVertexBuffer");
	}

	void* buffer = temp_data->CreatTmpVertexBuffer( vertex_count, vertex_size, stream_index );
	stream->read((unsigned char*)buffer, vertex_count * vertex_size);
}

void MeshSerializer::ClearTempData()
{
	for( size_t i = 0; i< m_submesh_datas.size(); ++i)
	{
		if( m_submesh_datas[i])
		{
			delete m_submesh_datas[i];
		}
	}
	m_submesh_datas.clear();
}

SubMeshData* MeshSerializer::AddSubMeshData()
{
	SubMeshData* temp_data = new SubMeshData();
	m_submesh_datas.push_back( temp_data);
	return temp_data;
}

void MeshSerializer::FinishLoad( CCMyMesh* pMesh )
{
	// 1.查看所有临时数据，算出总的顶点数和索引数
	unsigned short vertex_size = pMesh->GetVertexSize();
	unsigned short index_size = pMesh->GetIndexSize();
	size_t vertex_count = 0;
	size_t index_count = 0; 
	for( size_t i = 0; i < m_submesh_datas.size(); ++i)
	{
		SubMeshData* submesh_data = m_submesh_datas[i];
		vertex_count += submesh_data->m_vertex_count;
		index_count += submesh_data->m_index_count;
	}
	pMesh->m_vertex_count = vertex_count;
	pMesh->m_index_count = index_count;

	//---------------------------------------------------------------
	// 2.创建硬件缓冲和内存缓冲
	if( vertex_count > 0)
	{
		pMesh->CreateVeretexMemBuffer( vertex_count * vertex_size);
		pMesh->m_vertex_buffer_id = -1;
	}
	if( index_count > 0)
	{
		pMesh->CreateIndexMemBuffer( index_count * index_size);
		pMesh->m_index_buffer_id = -1;
	}
		
	//---------------------------------------------------------------
	// 3.处理临时数据，整合到内存缓冲中去
	const unsigned char* vertex_membuffer	= pMesh->m_vertex_membuffer_ptr;
	const unsigned char* index_membuffer		= pMesh->m_index_membuffer_ptr; 
	size_t index_offset = 0;
	for( size_t i = 0; i < m_submesh_datas.size(); ++i)
	{
		SubMeshData* submesh_data = m_submesh_datas[i];
		if( vertex_membuffer != 0)
		{
			size_t buff_size = submesh_data->m_vertex_count * vertex_size;
			submesh_data->HandleTempVertexBuffer( (void*)vertex_membuffer, pMesh);
			vertex_membuffer += submesh_data->m_vertex_count * vertex_size;
		}
		if( index_membuffer != 0)
		{
			// 所以从第二SubMesh开始，它们的索引值都要加上一个偏移值（即已经添加过的SubMesh的顶点总数）
			submesh_data->HandleTempIndexBuffer( (void*)index_membuffer, index_size, index_offset);
			index_membuffer += submesh_data->m_index_count * index_size;
		}
			
		submesh_data->HandleVertexWeights( pMesh, index_offset);
		index_offset += submesh_data->m_vertex_count;
	}

	//---------------------------------------------------------------
	// 4.把multi-stream强制转成single-stream，注意，必须在第3步之后操作
	unsigned short used_verties_size = 0;
	for( size_t i = 0; i < pMesh->m_vertex_elements.size(); ++i)
	{
		VertexElement& ele = pMesh->m_vertex_elements[i];
		ele.stream_id = 0;	// 全部强制设成0
		ele.offset = used_verties_size;

		used_verties_size += VertexElement::GetElementSize( ele.element_type);
	}

	// 5.把内存缓冲复制到硬件缓冲中去
	pMesh->OnSerializFinished();

	// 6.清空临时数据
	ClearTempData();
}

void MeshSerializer::readSubMeshBoneAssignment(ByteBuffer*stream, CCMyMesh* pMesh, SubMeshData* temp_data )
{
	unsigned int vertex_idx = 0;
	unsigned short bone_idx = 0;
	float weight = 0.0f;
	// unsigned int vertexIndex;
	readInts(stream, &(vertex_idx),1);
	// unsigned short boneIndex;
	readShorts(stream, &(bone_idx),1);
	// float weight;
	readFloats(stream, &(weight), 1);
	temp_data->AddWeight( vertex_idx, bone_idx, weight);
}

void MeshSerializer::readSkeletonLink(ByteBuffer* stream, CCMyMesh* pMesh )
{
	std::string skelName = readString(stream);
	pMesh->SetSkeletonName(skelName);
}

//---------------------------------------------------------------
//---------------------------------------------------------------
SubMeshData::SubMeshData() 
	: m_vertex_count(0)
	, m_index_buff(0)
	, m_index_count(0)
{
}

SubMeshData::~SubMeshData()
{
	for( size_t i = 0; i < m_vertex_buffs.size(); ++i)
	{
		free( m_vertex_buffs[i]);
		m_vertex_buffs[i] = 0;
	}
	m_vertex_buffs.clear();
	m_vertex_count = 0;
	m_vertex_sizes.clear();

	if( m_index_buff != 0)
	{
		free( m_index_buff);
	}
	m_index_buff = 0;
	m_index_count = 0;
}

void* SubMeshData::CreatTmpVertexBuffer(size_t v_count, size_t vertex_size, size_t stream_id )
{
	// 如果创建多个stream的时候，传进来的顶点数量不一致，就肯定是出错了
	if( m_vertex_buffs.size() > 0 && v_count != m_vertex_count)
	{
		assert(false && "VertexCounts are not equal in different streams SubMeshData::CreatTmpVertexBuffer");
	}

	if( stream_id >= m_vertex_buffs.size())
	{
		m_vertex_buffs.resize( stream_id + 1);
		m_vertex_sizes.resize( stream_id + 1);
	}
	char* buffer = 0;
	if( v_count > 0)
	{
		buffer = (char*)malloc( v_count * vertex_size);
		m_vertex_buffs[stream_id] = buffer;
		m_vertex_sizes[stream_id] = vertex_size;
		m_vertex_count = v_count;
	}
	return buffer;
}

void* SubMeshData::CreateTmpIndexBuffer( size_t i_count, size_t index_size )
{
	if( i_count > 0)
	{
		m_index_buff = (char*)malloc( i_count * index_size);
		m_index_count = i_count;
	}
	return m_index_buff;
}

void SubMeshData::HandleTempVertexBuffer( void* hardware_buffer, CCMyMesh* mesh )
{
	// 各个stream的指针
	std::vector<char*> buffers = m_vertex_buffs;

	const VertexElement* pos_ele = mesh->GetVertexElement(VES_POSITION);
		
	// 依次处理每个顶点
	for( size_t i = 0; i< m_vertex_count; ++i)
	{
		// 每个顶点可能被拆分在多个stream里，这里依次合并
		for( size_t j = 0; j < buffers.size(); ++j)
		{
			// 如果该stream包含POSITION信息，就把该POSITION合并到包围盒里去 
			if( pos_ele != 0 && pos_ele->stream_id == j)
			{
				float* x = (float*)((char*)buffers[j] + pos_ele->offset);
				float* y = x + 1;
				float* z = y + 1;
				mesh->MergeVertexIntoAABB( *x, *y, *z);
			}

			unsigned short vertex_size = m_vertex_sizes[j];
			memcpy( hardware_buffer, buffers[j], vertex_size);
			hardware_buffer = (void*)((char*)(hardware_buffer) + vertex_size);
			buffers[j] += vertex_size;
		}
	}
}

void SubMeshData::HandleTempIndexBuffer( void* hardware_buffer, size_t index_size, size_t offset )
{
	// 不同SubMesh的索引是各自独立的，但现在要把多SubMesh合成一个,
	// 所以从第二SubMesh开始，它们的索引值都要加上一个偏移值（即已经添加过的SubMesh的顶点总数）
	if( offset == 0)
	{
		memcpy( hardware_buffer, (void*)m_index_buff, index_size * m_index_count);
	}
	else
	{
		void *buffer = (void*)m_index_buff;
		for( size_t i = 0; i < m_index_count; ++i)
		{
			if( index_size == sizeof(unsigned short))
			{
				unsigned short* u16 = (unsigned short*)buffer;
				*u16 += (unsigned short)offset;
				memcpy( hardware_buffer, buffer, sizeof(unsigned short));
				buffer = (void*)(++u16);
			}
		}
	}

}

bool SubMeshData::AddWeight( size_t vertex_idx, size_t bone_idx, float weight )
{
	VertexWeightsMap::iterator it = m_vertex_weights_map.find( vertex_idx);
	if( it == m_vertex_weights_map.end())
	{
		m_vertex_weights_map[vertex_idx] = VertexWeights();
		it = m_vertex_weights_map.find( vertex_idx);
	}

	VertexWeights& vw = it->second;
		
	return vw.AddWeight(bone_idx, weight);
}

void SubMeshData::HandleVertexWeights( CCMyMesh* mesh, size_t offset )
{
	VertexWeightsMap::iterator it_end = m_vertex_weights_map.end();
	for( VertexWeightsMap::iterator it = m_vertex_weights_map.begin();
			it != it_end; ++it)
	{
		size_t vertex_idx = it->first + offset;
			
		VertexWeights& vw = it->second;
		for( size_t i = 0; i < MAX_BLEND_WEIGHTS; ++i)
		{
			if( vw.bone_weights[i] < 1e-6f)
			{
				break;
			}
				
			mesh->AddBoneAssignment(vertex_idx, vw.bone_indices[i], vw.bone_weights[i]);
		}
	}
}

NS_CC_END