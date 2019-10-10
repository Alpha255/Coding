#include "Base/AssetFile.h"
#include "Colorful/Public/Geometry.h"
#include "gear/gear.h"

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

Export std::vector<uint32_t> compileShader(const std::string &fileName, const std::string &entryName, uint32_t shaderStage, bool8_t bUsingParser = false);

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

NamespaceEnd(AssetTool)