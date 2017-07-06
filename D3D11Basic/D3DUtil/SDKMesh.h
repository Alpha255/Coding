#pragma once

#include <d3d9.h>
#include "D3DMath.h"

class SDKMesh
{
public:
	SDKMesh(const char* filePath)
	{
		LoadFromFile(filePath);
	}
	~SDKMesh();

protected:
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
		static const uint32_t s_MaxVertexElements = 32;

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
		std::string Name;
		uint32_t MaterialID;
		uint32_t PrimitiveType;
		uint64_t IndexStart;
		uint64_t IndexCount;
		uint64_t VertexStart;
		uint64_t VertexCount;
	};

	struct Frame
	{
		std::string Name;
		uint32_t Mesh;
		uint32_t ParentFrame;
		uint32_t ChildFrame;
		uint32_t SiblingFrame;
		DirectX::XMFLOAT4X4 Matrix;
		uint32_t AnimationDataIndex;		/// Used to index which set of keyframes transforms this frame
	};

	struct Material
	{
		std::string Name;

		/// Use MaterialInstancePath
		std::string MaterialInstancePath;

		/// Or fall back to d3d8-type materials
		std::string DiffuseTexture;
		std::string NormalTexture;
		std::string SpecularTexture;

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

	void LoadFromFile(const char* filePath);
	void LoadFromMemory(const uint8_t* const pMeshData, size_t meshDataSize);
	bool IsValid(const uint8_t* const pMeshData, size_t meshDataSize);
private:
	static const uint32_t s_MaxVertexStreams = 16;
	static const uint32_t s_Version = 101;

	std::string m_Name;
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
