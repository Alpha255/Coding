#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(Gfx)

#define SHADER_CACHE_NAME "ShaderCache.tmp"

class ShaderCache
{
public:
	struct CompiledShaderStorage
	{
		char8_t Path[MAX_PATH]{};
		FileTime LastWriteTime{};
		EShaderLanguage Language = EShaderLanguage::GLSL;
		EShaderStage Stage = EShaderStage::ShaderStageCount;
		uint32_t Hash = 0u;
		uint32_t BinarySize = 0u;
	};

	ShaderCache();
	std::vector<uint32_t> tryToGetCompiledBinary(EShaderLanguage language, EShaderStage stage, const std::string& materialPath);
	void updateShader(
		EShaderLanguage language, 
		EShaderStage stage, 
		const std::string& materialPath,
		const char8_t* const code);
protected:
private:
	std::unordered_map<uint32_t, CompiledShaderStorage> m_Storages{};
};
DECLARE_UNIQUE_PTR(ShaderCache)

NAMESPACE_END(Gfx)