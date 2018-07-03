#pragma once

#include "D3DMath.h"
#include "D3DBuffer.h"
#include "D3DShader.h"

#include <DirectXTK/Inc/Model.h>
#include <DirectXTK/Inc/CommonStates.h>

class Camera;
class D3DEngine;
struct Material;

NamespaceBegin(Geometry)

struct Vertex
{
	Vec3 Position = {};
	Vec3 Normal = {};
	Vec3 Tangent = {};
	Vec2 UV = {};

	Vertex() = default;

	Vertex(const Vec3 &pos, const Vec3 &normal, const Vec3 &tangent, const Vec2 &uv)
		: Position(pos)
		, Normal(normal)
		, Tangent(tangent)
		, UV(uv)
	{
	}

	Vertex(float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		: Position(px, py, pz)
		, Normal(nx, ny, nz)
		, Tangent(tx, ty, tz)
		, UV(u, v)
	{
	}
};

struct Mesh
{
public:
	void CreateAsBox(float width, float height, float depth);
	void CreateAsCube(float width);
	void CreateAsSphere(float radius, uint32_t slice, uint32_t stack);
	void CreateAsGeoSphere(float radius, uint32_t subDivisions);
	void CreateAsFlatGeoSphere(float radius, uint32_t subDivisions);
	void CreateAsCylinder(float bottomRadius, float topRadius, float height, uint32_t slice, uint32_t stack);
	void CreateAsGrid(float width, float depth, uint32_t m, uint32_t n);
	void CreateAsQuad(float length);
	void CreateAsQuad(float left, float top, float width, float height);
	void SaveAsObjFile(const char *pObjFileName);

	void DrawNormal(const Camera &cam);

	void Bind(const Material *pMaterial = nullptr);

	void Draw(const Material *pMaterial = nullptr, uint32_t startIndex = 0U, int32_t vertexOffset = 0);

	inline bool IsCreated() const
	{
		return m_Created;
	}

	D3DInputLayout VertexLayout;
	D3DBuffer VertexBuffer;
	D3DBuffer IndexBuffer;
	uint32_t IndexCount = 0U;
	uint32_t VertexCount = 0U;
protected:
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	void CreateRenderResource();

	void SubDivide();
	void MakeCylinderTopBottomCap(bool bTop, float bottomRadius, float topRadius, float height, uint32_t slice);

	void ApplyMaterial(const Material *pMaterial);
private:
	bool m_Created = false;
};

struct ObjMesh : public Mesh
{
public:
	void Create(const char *pFileName);

	struct ObjIndex
	{
		int32_t i = 0U;
		int32_t t = 0U;
		int32_t n = 0U;
	};
protected:
	///void TokenizeNextLine(std::ifstream &fs, std::vector<std::string> &tokens);

	void CreateVertexData(
		const std::vector<Vec3> &srcVertices,
		const std::vector<std::vector<ObjIndex>> &indexCollector,
		const std::vector<Vec3> &normals,
		const std::vector<Vec2> &uvs);

	void ComputeTangent();
};

struct SDKMesh
{
public:
	void Create(const char *pMeshName, bool bLoadVertexLayout = true);
	void CreateFromFile(const char *pMeshName);

	void Draw(bool bAlphaParts, bool bDisableMaterial = false);

	inline void SetInputLayout(const D3DInputLayout &layout)
	{
		m_VertexLayout = layout;
	}
protected:
	enum eFileInfo
	{
		eSDKMeshFileVersion = 101U,
		eSDKMeshMaxVertexElements = 32U,
		eSDKMeshIndexType_16Bit = 0U,
		eSDKMeshIndexType_32Bit = 1U,
		eSDKMeshMaxMeshName = 100U,
		eSDKMeshMaxVertexStreams = 16U,
	};

	struct SDKMeshVertexElement
	{
		uint16_t Stream;     // Stream index
		uint16_t Offset;     // Offset in the stream in bytes
		uint8_t  Type;       // Data type
		uint8_t  Method;     // Processing method
		uint8_t  Usage;      // Semantics
		uint8_t  UsageIndex; // Semantic index
	};

	struct SDKMeshHeader
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

	struct SDKMeshVertexBufferHeader
	{
		uint64_t NumVertices;
		uint64_t SizeBytes;
		uint64_t StrideBytes;
		SDKMeshVertexElement Decl[eSDKMeshMaxVertexElements];
		union
		{
			uint64_t DataOffset;
		};
	};

	struct SDKMeshIndexBufferHeader
	{
		uint64_t NumIndices;
		uint64_t SizeBytes;
		uint32_t IndexType;
		union
		{
			uint64_t DataOffset;
		};
	};

	struct SDKMeshMesh
	{
		char Name[eSDKMeshMaxMeshName];
		uint8_t NumVertexBuffers;
		uint32_t VertexBuffers[eSDKMeshMaxVertexStreams];
		uint32_t IndexBuffer;
		uint32_t NumSubsets;
		uint32_t NumFrameInfluences; //aka bones

		Vec3 BoundingBoxCenter;
		Vec3 BoundingBoxExtents;

		union
		{
			uint64_t SubsetOffset;
			int32_t *Subsets;
		};
		union
		{
			uint64_t FrameInfluenceOffset;
			uint32_t *FrameInfluences;
		};
	};

	void SetupVertexIndex(const DirectX::ModelMeshPart *pModelPart);
	void SetupMaterial(const DirectX::ModelMeshPart *pModelPart, bool bDisableMaterial);

	SDKMeshHeader LoadHeader(const uint8_t *pData, size_t dataSize);
	void LoadVertexBuffers(const uint8_t *pData, size_t dataSize, const SDKMeshHeader &header);
	void LoadIndexBuffers(const uint8_t *pData, size_t dataSize, const SDKMeshHeader &header);
	void LoadMeshs(const uint8_t *pData, size_t dataSize, const SDKMeshHeader &header);
private:
	std::unique_ptr<DirectX::Model> m_Model;
	std::unique_ptr<DirectX::CommonStates> m_States;

	std::vector<D3DBuffer> m_VertexBuffers;
	std::vector<D3DBuffer> m_IndexBuffers;

	D3DInputLayout m_VertexLayout;

	bool m_Created = false;
	bool m_ExtVertexLayout = false;
};

NamespaceEnd(Geometry)
