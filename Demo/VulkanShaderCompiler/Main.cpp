#include <Windows.h>
#include <stdint.h>
#include <vector>
#include <assert.h>
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>
#include "../Util/ResourceFile.h"

std::vector<uint32_t> CompileShader(const std::string &code, VkShaderStageFlagBits stage, const char *pSourceName)
{
	shaderc_shader_kind shaderKind = shaderc_vertex_shader;
	switch (stage)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		shaderKind = shaderc_vertex_shader;
		break;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		shaderKind = shaderc_fragment_shader;
		break;
	}

	shaderc::Compiler compiler;
	shaderc::CompileOptions compileOptions;
	shaderc::SpvCompilationResult compile = compiler.CompileGlslToSpv(code, shaderKind, pSourceName);

	if (compile.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		OutputDebugStringA(compile.GetErrorMessage().c_str());
		assert(0);
	}

	std::vector<uint32_t> result(compile.cbegin(), compile.cend());
	return result;
}

int main()
{
	ResourceFile shader("UIVertexShader.shader");
	uint8_t *pCode = shader.Load();

	auto result = CompileShader(std::string((char *)pCode), VK_SHADER_STAGE_VERTEX_BIT, "UIVertexShader.shader").data();

	return 0;
}