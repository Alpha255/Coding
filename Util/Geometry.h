#pragma once

#include "Definitions.h"
#include "Lighting.h"
#include "Camera.h"
#include <DirectXCollision.h>

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

	RInputLayout InputLayout;
	RBuffer VertexBuffer;
	RBuffer IndexBuffer;
	uint32_t IndexCount = 0U;
	uint32_t VertexCount = 0U;
public:
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

struct SDKMesh : public Mesh
{
public:
	void Create(const char *pMeshName, bool ccw = false, bool alpha = false);

	void Draw(bool bAlphaParts, bool bDisableMaterial = false);

	inline void SetInputLayout(const RInputLayout &layout)
	{
		m_VertexLayout = layout;
	}
protected:
	enum eSDKMeshFileInfo
	{
		eFileVersion = 101U,
		eMaxVertexElements = 32U,
		eMaxMeshName = 100U,
		eMaxSubsetName = 100U,
		eMaxMaterialName = 100U,
		eMaxMaterialPath = 260U, 
		eMaxTextureName = 260U,
		eMaxVertexStreams = 16U,
	};

	enum eSDKMeshIndexType
	{
		eIndexType_16Bit = 0U,
		eIndexType_32Bit = 1U,
	};

	enum eSDKMeshPrimitiveType
	{
		eTriangle_List = 0,
		eTriangle_Strip,
		eLine_List,
		eLine_Strip,
		ePoint_List,
		eTriangle_List_Adj,
		eTriangle_Strip_Adj,
		eLine_List_Adj,
		eLine_Strip_Adj
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
		SDKMeshVertexElement Decl[eMaxVertexElements];
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
		char Name[eMaxMeshName];
		uint8_t NumVertexBuffers;
		uint32_t VertexBuffers[eMaxVertexStreams];
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

	struct SDKMeshSubset
	{
		char Name[eMaxSubsetName];
		uint32_t MaterialID;
		uint32_t PrimitiveType;
		uint64_t IndexStart;
		uint64_t IndexCount;
		uint64_t VertexStart;
		uint64_t VertexCount;
	};

	struct ModelMesh
	{
		struct MeshPart
		{
			uint32_t  IndexCount;
			uint32_t  StartIndex;
			uint32_t  VertexOffset;
			uint32_t  VertexStride;
			eRPrimitiveTopology PrimitiveType;
			uint32_t IndexFormat;
			RBuffer IndexBuffer;
			RBuffer VertexBuffer;
			bool IsAlpha;

			Material *Mt = nullptr;

			~MeshPart();
		};

		DirectX::BoundingBox MeshBoundingBox;

		bool CCW;
		bool Alpha;

		std::vector<std::unique_ptr<MeshPart>> MeshParts;
	};

	struct SDKMeshMaterial
	{
		char    Name[eMaxMaterialName];

		/// Use MaterialInstancePath
		char    MaterialInstancePath[eMaxMaterialPath];

		/// Or fall back to d3d8-type materials
		char    DiffuseTexture[eMaxTextureName];
		char    NormalTexture[eMaxTextureName];
		char    SpecularTexture[eMaxTextureName];

		Vec4 Diffuse;
		Vec4 Ambient;
		Vec4 Specular;
		Vec4 Emissive;
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

	SDKMeshHeader LoadHeader(const uint8_t *pData, size_t dataSize);
	void LoadVertexBuffers(const uint8_t *pData, size_t dataSize, const SDKMeshHeader &header);
	void LoadIndexBuffers(const uint8_t *pData, size_t dataSize, const SDKMeshHeader &header);
	void LoadMeshs(const uint8_t *pData, size_t dataSize, const SDKMeshHeader &header, bool ccw, bool alpha);
	bool LoadMaterial(const SDKMeshMaterial &sdkmt, Material &mt);

private:
	std::vector<std::shared_ptr<ModelMesh>> m_Meshs;

	std::vector<RBuffer> m_VertexBuffers;
	std::vector<RBuffer> m_IndexBuffers;

	RInputLayout m_VertexLayout;

	bool m_Created = false;
};

NamespaceEnd(Geometry)
