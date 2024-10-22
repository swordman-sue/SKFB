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
	std::vector<char*>	m_vertex_buffs;	// *.mesh文件中，每个SubMesh可能有多个VertexStream，就分别保存在这里
	std::vector<unsigned short>	m_vertex_sizes;	// 每个VertexStream中的VertexElement占用的字节数
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
		Mesh读取完成后，把multi-stream合成single-stream，把multi-submesh合成single-mesh，
		计算Mesh的包围盒,在显存中创建顶点缓冲，并且最后清理临时数据
		@param Mesh * pMesh 
		@returns void 
	*/
	void	FinishLoad( CCMyMesh* pMesh);

	void ClearTempData();

	SubMeshData*	AddSubMeshData();

protected:
	std::vector<SubMeshData*>	m_submesh_datas;	// 在importMesh时临时存储顶点信息的容器
};

NS_CC_END

#endif