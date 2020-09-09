#pragma once

#include "Colorful/Public/GfxResource.h"
#include "AssetTool/Asset.h"
#include "ThirdParty/json/single_include/nlohmann/json.hpp"

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
	template<class T>
	class TAttribute : public nlohmann::json
	{
		virtual const char8_t* const name() const = 0;
		virtual void serialize() = 0;
		virtual void deserialize() = 0;
	};

	struct InputLayout : TAttribute<InputLayout>
	{
		std::vector<std::pair<eRVertexUsage, eRFormat>> Attributes;

		///NLOHMANN_DEFINE_TYPE_INTRUSIVE;

		NLOHMANN_JSON_SERIALIZE_ENUM(eRVertexUsage, 
			{
				{ ePosition,  "Position"  },
				{ eNormal,    "Normal"    },
				{ eTangent,   "Tangent"   },
				{ eBiNormal,  "BiNormal"  },
				{ eBiTangent, "BiTangent" },
				{ eTexcoord0, "Texcoord0" },
				{ eTexcoord1, "Texcoord1" },
				{ eTexcoord2, "Texcoord2" },
				{ eTexcoord3, "Texcoord3" },
				{ eTexcoord4, "Texcoord4" },
				{ eTexcoord5, "Texcoord5" },
				{ eTexcoord6, "Texcoord6" },
				{ eTexcoord7, "Texcoord7" },
				{ eColor0,    "Color0"    },
				{ eColor1,    "Color1"    },
				{ eColor2,    "Color2"    },
				{ eColor3,    "Color3"    },
				{ eWeight,    "Weight"    },
			})

		const char8_t* const name() const override
		{
			return "InputLayout";
		}

		void serialize() override
		{
			auto& json = *static_cast<nlohmann::json*>(this);
		}

		void deserialize() override
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
};
