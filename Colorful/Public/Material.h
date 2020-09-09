#pragma once

#include "Colorful/Public/GfxResource.h"
#include "AssetTool/Asset.h"

#define MaterialAttribute(ClassName)       \
const char8_t* const name() const override \
{                                          \
	return #ClassName;                     \
}

DeclareShared(Material)
class Material : public Gear::File
{
public:
	Material(const std::string& name);

	bool8_t isDirty() const
	{
		return false;
	}
protected:
	struct IAttribute
	{
		virtual const char8_t* const name() const = 0;
	};

	struct InputLayout : IAttribute
	{
		MaterialAttribute(InputLayout)
	};

	struct UniformBuffer : IAttribute
	{
		MaterialAttribute(UniformBuffer)
	};

	struct RWBuffer
	{
	};

	struct Sampler
	{
	};

	struct Texture
	{
		std::string FilePath;
	};

	struct TextureSampler
	{
		Texture TargetTexture;
		Sampler TargetSampler;
	};

	struct ShaderResource
	{
		enum EGpuResourceType
		{
			ETexture,
			EUniformBuffer,
			ERWBuffer,
			ESampler
		};

		uint32_t Slot = 0u;
		EGpuResourceType Type;

		union Resource
		{
			TextureSampler CombinedTextureSampler;
			Texture CommonTexture;
			Sampler CommonSampler;
			UniformBuffer ConstantsBuffer;
			RWBuffer ReadWriteBuffer;
		};
	};

	struct Shader
	{
		eRShaderUsage Usage;
		std::string Source;
		std::shared_ptr<byte8_t> Binary;

		static constexpr std::array<const char8_t* const, eRShaderUsage_MaxEnum> usageNames
		{
			"VertexShader",
			"HullShader",
			"DomainShader",
			"GeometryShader",
			"FragmentShader",
			"ComputeShader"
		};

		static constexpr std::array<const char8_t* const, eRShaderCode_MaxEnum> shaderCodeTypeNames
		{
			"GLSL",
			"HLSL"
		};
	};

	void reload();
private:
};
