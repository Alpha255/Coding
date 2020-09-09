#pragma once

#include "Colorful/Public/GfxResource.h"
#include "AssetTool/Asset.h"
#include "ThirdParty/json/single_include/nlohmann/json.hpp"

DeclareShared(Material)
class Material : public Gear::File
{
public:
	Material(const std::string& name);
	Material() = default;

	bool8_t isDirty() const
	{
		return false;
	}

	void test();
protected:
	class IAttribute
	{
		virtual const char8_t* const name() const = 0;
		virtual void serialize(nlohmann::json& json) = 0;
		virtual void deserialize(nlohmann::json& json) = 0;
	};

	struct InputLayout : IAttribute
	{
		std::vector<std::pair<eRVertexUsage, eRFormat>> Attributes;

		const char8_t* const name() const override
		{
			return "InputLayout";
		}

		void serialize(nlohmann::json& json) override
		{
			auto& target = json[name()];
			for (auto& attr : Attributes)
			{
			}
		}

		void deserialize(nlohmann::json& json) override
		{

		}
	};

	struct UniformBuffer
	{
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
		}_Resource;
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
	nlohmann::json m_Json;

	std::vector<std::shared_ptr<IAttribute>> m_Attributes;
};
