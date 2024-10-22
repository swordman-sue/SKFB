#ifndef DreamMesh_h__
#define DreamMesh_h__

#include "CCResObjManager.h"
#include "math/CCMath.h"

#include <vector>
#include <map>

NS_CC_BEGIN

enum VertexElementType
{
	VET_FLOAT1 = 0,
	VET_FLOAT2 = 1,
	VET_FLOAT3 = 2,
	VET_FLOAT4 = 3,
	/// alias to more specific colour type - use the current rendersystem's colour packing
	VET_COLOUR = 4,
	VET_SHORT1 = 5,
	VET_SHORT2 = 6,
	VET_SHORT3 = 7,
	VET_SHORT4 = 8,
	VET_UBYTE4 = 9,
	/// D3D style compact colour
	VET_COLOUR_ARGB = 10,
	/// GL style compact colour
	VET_COLOUR_ABGR = 11,

	VET_COUNT
};


// 和_D3DDECLUSAGE一一对应，为VertexDecl::Usage的可选项
/// Vertex element semantics, used to identify the meaning of vertex buffer contents
enum VertexElementSemantic {
	/// Position, 3 reals per vertex
	VES_POSITION = 1,
	/// Blending weights
	VES_BLEND_WEIGHTS = 2,
	/// Blending indices
	VES_BLEND_INDICES = 3,
	/// Normal, 3 reals per vertex
	VES_NORMAL = 4,
	/// Diffuse colours
	VES_DIFFUSE = 5,
	/// Specular colours
	VES_SPECULAR = 6,
	/// Texture coordinates
	VES_TEXTURE_COORDINATES = 7,
	/// Binormal (Y axis if normal is Z)
	VES_BINORMAL = 8,
	/// Tangent (X axis if normal is Z)
	VES_TANGENT = 9,

	VES_COUNT

};

struct VertexElement
{
	VertexElement( unsigned short _source, unsigned short _offset, VertexElementType _type, 
		VertexElementSemantic _usage, unsigned char _usage_index)
		: stream_id(_source), offset(_offset), element_type(_type)
		, element_usage( _usage), usage_index(_usage_index)
	{
	}

	static unsigned short GetElementSize(VertexElementType ele_type)
	{
		switch( ele_type)
		{
		case VET_FLOAT1:
			return sizeof(float);
		case VET_FLOAT2:
			return sizeof(float) * 2;
		case VET_FLOAT3:
			return sizeof(float) * 3;
		case VET_FLOAT4:
			return sizeof(float) * 4;
		case VET_COLOUR:
		case VET_COLOUR_ARGB:
		case VET_COLOUR_ABGR:
			return sizeof(unsigned int);
		case VET_SHORT1:
			return sizeof(short);
		case VET_SHORT2:
			return sizeof(short) * 2;
		case VET_SHORT3:
			return sizeof(short) * 3;
		case VET_SHORT4:
			return sizeof(short) * 4;
		case VET_UBYTE4:
			return sizeof(unsigned char) * 4;
		}
		// 
		return 0;
	}

	unsigned short    stream_id;     // Stream index，可理解为BufferID，一般是0
	unsigned short    offset;     // Offset in the stream in bytes

	VertexElementType		element_type;
	VertexElementSemantic   element_usage;

	unsigned char    usage_index; // Semantic index，如果有相同Usage，则此值递增，
};

typedef std::vector<VertexElement>	VertexElementArray;

enum
{
	MAX_BLEND_WEIGHTS = 4,		// 一个顶点最多受几个骨骼影响
};

struct VertexWeights
{
	VertexWeights()
	{
		memset(bone_indices, 0, sizeof(bone_indices));
		memset(bone_weights, 0, sizeof(bone_weights));
	}
	unsigned int bone_indices[MAX_BLEND_WEIGHTS];	
	float bone_weights[MAX_BLEND_WEIGHTS];

	bool AddWeight(unsigned int bone_idx, float weight)
	{
		for( size_t i = 0; i < MAX_BLEND_WEIGHTS; ++i)
		{
			if (bone_idx == bone_indices[i] && bone_weights[i] > 1e-6f)
			{
				break;
			}

			if( bone_weights[i] < 1e-6f)
			{
				bone_weights[i] = weight;
				bone_indices[i] = bone_idx;
				return true;
			}
		}
		return false;
	}
};

class CCMyMesh : public BaseObj
{
	friend class MeshSerializer;
public:

	CCMyMesh( const std::string& res_name);
	virtual ~CCMyMesh();

	void	MergeVertexIntoAABB( float x, float y, float z);

	/*
		添加一条顶点格式描述信息
	*/
	void AddVertexElement( unsigned short source, unsigned short offset, VertexElementType ele_type, VertexElementSemantic ele_usage, unsigned char usage_index);
		
	/*
		获取顶点格式描述信息的数量
		@returns unsigned int 
	*/
	unsigned short GetVertexElementCount() const
	{
		return m_vertex_elements.size();
	}

	void AddBoneAssignment( unsigned int vertex_idx, unsigned int bone_idx, float weight);

	/*
		获取顶点格式中的指定描述信息
	*/
	const VertexElement* GetVertexElement( VertexElementSemantic usage, unsigned char usage_index=0) const;

	/*
		获取每个顶点占用的字节数
		@returns unsigned int 
	*/
	unsigned short GetVertexSize( unsigned short stream_index) const;
	unsigned short GetVertexSize() const {return m_vertex_size;}

	unsigned short GetIndexSize() const { return m_index_size; }
	unsigned int GetVertexCount() const { return m_vertex_count;	}
	unsigned int GetVertexBufferSize() const { return m_vertex_buffer_size;	}
	unsigned char* GetVertexBuffer() { return m_vertex_membuffer_ptr; }

	unsigned int GetIndexBufferSize() const { return m_index_buffer_size; }
	unsigned int GetIndexCount() const { return m_index_count; }
	unsigned char* GetIndexBuffer() const { return m_index_membuffer_ptr; }

	bool HasDiffuse() { return m_has_color;}

	void OnSerializFinished();
		
	void SetSkeletonName( const std::string& skel_name);

	bool HasSkeleton() const;

	const std::string& GetSkeletonName() const { return m_skeleton_name;	}
		
	bool AnimateVertices( const Mat4* bone_mats, size_t mats_num, unsigned char* out_buffer, size_t buffer_sz);



public:
	virtual void OnCreated();


protected:
	void CreateVeretexMemBuffer( unsigned int buffer_bytes);
	void CreateIndexMemBuffer( unsigned int buffer_bytes);
	void DestroyVertexMemBuffer();
	void DestroyIndexMemBuffer();

protected:
	int		m_vertex_buffer_id;
	unsigned int m_vertex_count;
	unsigned short m_vertex_size;
	unsigned int m_vertex_buffer_size;		// 顶点缓冲的总字节数
	unsigned char*	m_vertex_membuffer_ptr;		// 内存里保存的顶点缓冲指针
	VertexElementArray	m_vertex_elements;

	int		m_index_buffer_id;
	unsigned int m_index_count;
	unsigned short	m_index_size;
	unsigned int m_index_buffer_size;		// 索引缓冲的总字节数
	unsigned char*	m_index_membuffer_ptr;		// 内存里的索引缓冲指针

	//------------------------------------------------------------
	// 骨骼动画相关
	typedef std::vector<VertexWeights>	VertexWeightsArray;
	VertexWeightsArray	m_vertex_weights_array;

	std::string		m_skeleton_name;

	bool m_has_color;
};

NS_CC_END

#endif