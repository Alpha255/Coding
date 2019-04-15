#include "Base/Base.h"

NamespaceBegin(AssetTool)

Export bool CompileShader(
	const std::string &fileName,
	const std::string &entryPoint,
	const uint32_t shaderStage,
	__out std::vector<uint32_t> &binary);

NamespaceEnd(AssetTool)