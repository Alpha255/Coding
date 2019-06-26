#include "Base/AssetFile.h"
#include "Colorful/Public/Geometry.h"

NamespaceBegin(gli)
	class texture;
NamespaceEnd(gli)

NamespaceBegin(Geometry)
	class Model;
NamespaceEnd(Geometry)

struct ID3D11Device;
struct ID3D11Resource;
struct ID3D11ShaderResourceView;

NamespaceBegin(AssetTool)

Export bool CompileShader(
	AssetFile &asset,
	const std::string &entryPoint,
	const uint32_t shaderStage,
	__out std::vector<uint32_t> &binary);

Export bool LoadOBJ(
	AssetFile &asset,
	__out std::vector<Geometry::Vertex> &vertices,
	__out std::vector<uint32_t> &indices,
	__out Geometry::Box &boundingBox);

Export bool LoadSDKMesh(
	AssetFile &asset,
	__out Geometry::Model &model);

Export void CreateTextureFromFile(
	ID3D11Device* pD3DDevice,
	AssetFile &asset,
	__out ID3D11Resource **pTexture,
	__out ID3D11ShaderResourceView **pTextureView);

Export gli::texture *CreateTextureFromFile(AssetFile &asset);

NamespaceEnd(AssetTool)