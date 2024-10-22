#ifndef DreamMeshTrunkDef_h__
#define DreamMeshTrunkDef_h__


NS_CC_BEGIN

enum MeshChunkID {
    M_HEADER                = 0x1000,
        // char*          version           : Version number check
    M_MESH                = 0x3000,
		// bool skeletallyAnimated   // important flag which affects h/w buffer policies
        // Optional M_GEOMETRY chunk
        M_SUBMESH             = 0x4000, 
            // char* materialName
            // bool useSharedVertices
            // unsigned int indexCount
            // bool indexes32Bit
            // unsigned int* faceVertexIndices (indexCount)
            // OR
            // unsigned short* faceVertexIndices (indexCount)
            // M_GEOMETRY chunk (Optional: present only if useSharedVertices = false)
            M_SUBMESH_OPERATION = 0x4010, // optional, trilist assumed if missing
                // unsigned short operationType
            M_SUBMESH_BONE_ASSIGNMENT = 0x4100,
                // Optional bone weights (repeating section)
                // unsigned int vertexIndex;
                // unsigned short boneIndex;
                // float weight;
    		// Optional chunk that matches a texture name to an alias
            // a texture alias is sent to the submesh material to use this texture name
            // instead of the one in the texture unit with a matching alias name
            M_SUBMESH_TEXTURE_ALIAS = 0x4200, // Repeating section
                // char* aliasName;
                // char* textureName;

        M_GEOMETRY          = 0x5000, // NB this chunk is embedded within M_MESH and M_SUBMESH
            // unsigned int vertexCount
			M_GEOMETRY_VERTEX_DECLARATION = 0x5100,
				M_GEOMETRY_VERTEX_ELEMENT = 0x5110, // Repeating section
					// unsigned short source;  	// buffer bind source
					// unsigned short type;    	// VertexElementType
					// unsigned short semantic; // VertexElementSemantic
					// unsigned short offset;	// start offset in buffer in bytes
					// unsigned short index;	// index of the semantic (for colours and texture coords)
			M_GEOMETRY_VERTEX_BUFFER = 0x5200, // Repeating section
				// unsigned short bindIndex;	// Index to bind this buffer to
				// unsigned short vertexSize;	// Per-vertex size, must agree with declaration at this index
				M_GEOMETRY_VERTEX_BUFFER_DATA = 0x5210,
					// raw buffer data
        M_MESH_SKELETON_LINK = 0x6000,
            // Optional link to skeleton
            // char* skeletonName           : name of .skeleton to use
        M_MESH_BONE_ASSIGNMENT = 0x7000,
            // Optional bone weights (repeating section)
            // unsigned int vertexIndex;
            // unsigned short boneIndex;
            // float weight;
        M_MESH_BOUNDS = 0x9000,
            // float minx, miny, minz
            // float maxx, maxy, maxz
            // float radius
                    
		// Added By DrEvil
		// optional chunk that contains a table of submesh indexes and the names of
		// the sub-meshes.
		M_SUBMESH_NAME_TABLE = 0xA000,
			// Subchunks of the name table. Each chunk contains an index & string
			M_SUBMESH_NAME_TABLE_ELEMENT = 0xA100,

            M_GEOMETRY_NORMALS = 0x5100,    //(Optional)
                // float* pNormals (x, y, z order x numVertices)
            M_GEOMETRY_COLOURS = 0x5200,    //(Optional)
                // unsigned long* pColours (RGBA 8888 format x numVertices)
            M_GEOMETRY_TEXCOORDS = 0x5300    //(Optional, REPEATABLE, each one adds an extra set)
 
};

NS_CC_END

#endif
