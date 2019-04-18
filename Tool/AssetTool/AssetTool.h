#include "Colorful/Public/Geometry.h"

NamespaceBegin(AssetTool)

Export bool CompileShader(
	const std::string &fileName,
	const std::string &entryPoint,
	const uint32_t shaderStage,
	__out std::vector<uint32_t> &binary);

Export bool LoadOBJ(
	const std::string &filePath,
	__out std::vector<Geometry::Vertex> &vertices,
	__out std::vector<uint32_t> &indices);

NamespaceEnd(AssetTool)