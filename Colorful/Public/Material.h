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
			auto& value = json[name()];
			for (int i = 0u; i < Attributes.size(); ++i)
			{
				value[i]["Usage"] = Attributes[i].first;
				value[i]["Format"] = Attributes[i].second;
			}
		}

		void deserialize(nlohmann::json& json) override
		{
			auto& value = json[name()];
			assert(value.is_array());
			for (auto& item : value)
			{
				Attributes.emplace_back(std::make_pair(static_cast<eRVertexUsage>(item["Usage"]), 
					static_cast<eRFormat>(item["Format"])));
			}
		}
	};

	struct Shader final : public IAttribute
	{
		eRShaderUsage Usage = eRShaderUsage_MaxEnum;
		std::string Target;
		std::string Source;
		std::shared_ptr<byte8_t> Binary = nullptr;

		const char8_t* const name() const override
		{
			assert(Usage < eRShaderUsage_MaxEnum);
			return usageNames[Usage];
		}

		void serialize(nlohmann::json& json) override
		{
			assert(Source.length());
			auto& value = json[name()];
			value["Target"] = Target;
			value["Source"] = Source;
			value["Binary"] = std::string(reinterpret_cast<char8_t*>(Binary.get()));
		}

		void deserialize(nlohmann::json& json) override
		{

		}

		static constexpr const char8_t* const usageNames[]
		{
			"VertexShader",
			"HullShader",
			"DomainShader",
			"GeometryShader",
			"FragmentShader",
			"ComputeShader"
		};
	};

	void reload();
private:
	nlohmann::json m_Json;

	std::vector<std::shared_ptr<IAttribute>> m_Attributes;
};
