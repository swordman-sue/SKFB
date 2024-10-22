#ifndef DreamMeshSerializer_h__
#define DreamMeshSerializer_h__

#include <vector>
#include <map>
#include "CCSerializer.h"
using namespace std;

NS_CC_BEGIN

struct VertexWeights;
class CCMyMesh;
class ByteBuffer;

class  SubMeshData
{
public:
	std::vector<char*>	m_vertex_buffs;	// *.mesh�ļ��У�ÿ��SubMesh�����ж��VertexStream���ͷֱ𱣴�������
	std::vector<unsigned short>	m_vertex_sizes;	// ÿ��VertexStream�е�VertexElementռ�õ��ֽ���
	unsigned int	m_vertex_count;

	char*	m_index_buff;
	size_t	m_index_count;

	typedef std::map<size_t, VertexWeights> VertexWeightsMap;
	VertexWeightsMap	m_vertex_weights_map;

	SubMeshData();
	~SubMeshData();

	void* CreatTmpVertexBuffer(size_t v_count, size_t vertex_size, size_t stream_id);
	void* CreateTmpIndexBuffer( size_t i_count, size_t index_size);

	void HandleTempVertexBuffer( void* hardware_buffer, CCMyMesh* mesh);
	void HandleTempIndexBuffer( void* hardware_buffer, size_t index_size, size_t offset);

	bool AddWeight( size_t vertex_idx, size_t bone_idx, float weight);
	void HandleVertexWeights(CCMyMesh* mesh, size_t offset);
};

class MeshSerializer : public Serializer
{
public:
	MeshSerializer();
	virtual ~MeshSerializer();

    bool importMesh(ByteBuffer* stream, CCMyMesh* pDest);

protected:
	virtual void readMesh(ByteBuffer* stream, CCMyMesh* pMesh);

	virtual void readSubMesh(ByteBuffer* stream, CCMyMesh* pMesh);

	virtual void readSubMeshBoneAssignment(ByteBuffer* stream, CCMyMesh* pMesh, SubMeshData* temp_data);

	virtual void readGeometry(ByteBuffer* stream, CCMyMesh* pMesh, SubMeshData* temp_data);

	virtual void readGeometryVertexDeclaration(ByteBuffer* stream, CCMyMesh* pMesh);

	virtual void readGeometryVertexBuffer(ByteBuffer* stream, CCMyMesh* pMesh, SubMeshData* temp_data, size_t vertex_count);

	virtual void readSkeletonLink(ByteBuffer* stream, CCMyMesh* pMesh);
		
	/*
		Mesh��ȡ��ɺ󣬰�multi-stream�ϳ�single-stream����multi-submesh�ϳ�single-mesh��
		����Mesh�İ�Χ��,���Դ��д������㻺�壬�������������ʱ����
		@param Mesh * pMesh 
		@returns void 
	*/
	void	FinishLoad( CCMyMesh* pMesh);

	void ClearTempData();

	SubMeshData*	AddSubMeshData();

protected:
	std::vector<SubMeshData*>	m_submesh_datas;	// ��importMeshʱ��ʱ�洢������Ϣ������
};

NS_CC_END

#endif