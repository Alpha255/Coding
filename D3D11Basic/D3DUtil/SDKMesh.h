#pragma once

#include <d3d9.h>
#include <d3d11.h>
#include "D3DMath.h"

class SDKMesh
{
public:
	SDKMesh() {}
	~SDKMesh();

	void LoadFromFile(const char* pFileName);
protected:
	static const uint32_t s_MaxVertexStreams = 16U;
	static const uint32_t s_MaxVertexElements = 32;
	static const uint32_t s_Version = 101U;
	static const uint32_t s_NameLength = 100U;
	static const uint32_t s_PathLength = MAX_PATH;

	enum eDeclType
	{
		eFloat1 = D3DDECLTYPE_FLOAT1,  /// 1D float expanded to (value, 0., 0., 1.)
		eFloat2 = D3DDECLTYPE_FLOAT2,  /// 2D float expanded to (value, value, 0., 1.)
		eFloat3 = D3DDECLTYPE_FLOAT3,  /// 3D float expanded to (value, value, value, 1.)
		eFloat4 = D3DDECLTYPE_FLOAT4,  /// 4D float
		eColor = D3DDECLTYPE_D3DCOLOR,  /// 4D packed unsigned bytes mapped to 0. to 1. range
	    
        /// Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
	    eUByte4 = D3DDECLTYPE_UBYTE4,  /// 4D unsigned uint8_t
	    eUByte4N = D3DDECLTYPE_UBYTE4N,  /// Each of 4 bytes is normalized by dividing to 255.0
	    eShort4N = D3DDECLTYPE_SHORT4N,  /// 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
	    
        /// Note: There is no equivalent to D3DDECLTYPE_DEC3N (14) as a DXGI_FORMAT
	    eFloat16_2 = D3DDECLTYPE_FLOAT16_2,  /// Two 16-bit floating point values, expanded to (value, value, 0, 1)
	    eFloat16_4 = D3DDECLTYPE_FLOAT16_4,  /// Four 16-bit floating point values

	    eUnUsed = D3DDECLTYPE_UNUSED,  /// When the type field in a decl is unused.

		/// These are extensions for DXGI-based versions of Direct3D
		eDXGI_R10G10B10A2_UNorm = 32 + DXGI_FORMAT_R10G10B10A2_UNORM,
		eDXGI_R11G11B10_Float = 32 + DXGI_FORMAT_R11G11B10_FLOAT,
		eDXGI_R8G8B8A8_SNorm = 32 + DXGI_FORMAT_R8G8B8A8_SNORM,
	};

	enum eIndexType
	{
		eIT_16BIT = 0,
		eIT_32BIT,
	};

	struct Header
	{
		/// Basic Info and sizes
		uint32_t Version;
		uint8_t  IsBigEndian;
		uint64_t HeaderSize;
		uint64_t NonBufferDataSize;
		uint64_t BufferDataSize;

		/// Stats
		uint32_t NumVertexBuffers;
		uint32_t NumIndexBuffers;
		uint32_t NumMeshes;
		uint32_t NumTotalSubsets;
		uint32_t NumFrames;
		uint32_t NumMaterials;

		/// Offsets to Data
		uint64_t VertexStreamHeadersOffset;
		uint64_t IndexStreamHeadersOffset;
		uint64_t MeshDataOffset;
		uint64_t SubsetDataOffset;
		uint64_t FrameDataOffset;
		uint64_t MaterialDataOffset;
	};

	struct VertexBufferHeader
	{
		uint64_t NumVertices;
		uint64_t SizeBytes;
		uint64_t StrideBytes;

		D3DVERTEXELEMENT9 Decl[s_MaxVertexElements];
		union
		{
			uint64_t DataOffset;
		};
	};

	struct IndexBufferHeader
	{
		uint64_t NumIndices;
		uint64_t SizeBytes;
		uint32_t IndexType;
		union
		{
			uint64_t DataOffset;
		};
	};

	struct SubSet
	{
		char Name[s_NameLength];
		uint32_t MaterialID;
		uint32_t PrimitiveType;
		uint64_t IndexStart;
		uint64_t IndexCount;
		uint64_t VertexStart;
		uint64_t VertexCount;
	};

	struct Frame
	{
		char Name[s_NameLength];
		uint32_t Mesh;
		uint32_t ParentFrame;
		uint32_t ChildFrame;
		uint32_t SiblingFrame;
		DirectX::XMFLOAT4X4 Matrix;
		uint32_t AnimationDataIndex;		/// Used to index which set of keyframes transforms this frame
	};

	struct Material
	{
		enum eFlags
		{
			ePerVertexClr = 0x1,
			eSkinning = 0x2,
			eDualTexture = 0x4,
			eNormalMaps = 0x8,
			eBiasedVertexNormals = 0x10,
		};

		char Name[s_NameLength];

		/// Use MaterialInstancePath
		char MaterialInstancePath[s_PathLength];

		/// Or fall back to d3d8-type materials
		char DiffuseTexture[s_PathLength];
		char NormalTexture[s_PathLength];
		char SpecularTexture[s_PathLength];

		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT4 Ambient;
		DirectX::XMFLOAT4 Specular;
		DirectX::XMFLOAT4 Emissive;
		float Power;

		union
		{
			uint64_t Force64_1;			/// Force the union to 64bits
		};
		union
		{
			uint64_t Force64_2;			/// Force the union to 64bits
		};
		union
		{
			uint64_t Force64_3;			/// Force the union to 64bits
		};

		union
		{
			uint64_t Force64_4;			/// Force the union to 64bits
		};
		union
		{
			uint64_t Force64_5;		    /// Force the union to 64bits
		};
		union
		{
			uint64_t Force64_6;			/// Force the union to 64bits
		};
	};

	struct Mesh
	{
		char Name[s_NameLength];
		uint8_t m_NumVertexBuffers;
		uint32_t m_VertexBuffers[s_MaxVertexStreams];
		uint32_t m_IndexBuffer;
		uint32_t m_NumSubsets;
		uint32_t m_NumFrameInfluences; /// aka bones

		DirectX::XMFLOAT3 m_BoundingBoxCenter;
		DirectX::XMFLOAT3 m_BoundingBoxExtents;

		union
		{
			uint64_t u_SubsetOffset;
			INT* u_Subsets;
		};
		union
		{
			uint64_t u_FrameInfluenceOffset;
			uint32_t* u_FrameInfluences;
		};
	};

	void LoadFromMemory(const uint8_t* const pMeshData, size_t meshDataSize);
	bool IsValid(const uint8_t* const pMeshData, size_t meshDataSize);

	uint32_t GetInputLayoutDesc(_In_reads_(32) const D3DVERTEXELEMENT9 decl[], std::vector<D3D11_INPUT_ELEMENT_DESC>& inputDesc);
private:
};
