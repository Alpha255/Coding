#include "Colorful/Public/GfxMaterial.h"
#include "Colorful/Public/GfxEngine.h"
#include <ThirdParty/rttr/src/rttr/registration.h>

#define RAPIDJSON_HAS_STDSTRING 1
#include <ThirdParty/rttr/3rd_party/rapidjson-1.1.0/rapidjson/prettywriter.h>
#include <ThirdParty/rttr/3rd_party/rapidjson-1.1.0/rapidjson/document.h>

using namespace rttr;
using namespace rapidjson;

RTTR_REGISTRATION
{
	rttr::registration::enumeration<eRVertexUsage>("eRVertexUsage")
	(
		value("ePosition", eRVertexUsage::ePosition),
		value("eNormal", eRVertexUsage::eNormal),
		value("eBinNormal", eRVertexUsage::eBinNormal),
		value("eTangent", eRVertexUsage::eTangent),
		value("eBiTangent", eRVertexUsage::eBiTangent),
		value("eTexcoord", eRVertexUsage::eTexcoord),
		value("eColor", eRVertexUsage::eColor)
	);

	rttr::registration::enumeration<eRFormat>("eRFormat")
	(
		value("eUnknownFormat", eRFormat::eUnknownFormat)
	);

	rttr::registration::class_<GfxVertexAttributes>("GfxVertexAttributes")
		.constructor()(rttr::policy::ctor::as_object)
		.property("Usage", &GfxVertexAttributes::Usage)
		.property("Format", &GfxVertexAttributes::Format);

	rttr::registration::class_<GfxMaterial>("GfxMaterial")
		.property("InputLayout", &GfxMaterial::InputLayout)
		.property("ShaderNames", &GfxMaterial::ShaderNames)
		.property("Shaders", &GfxMaterial::Shaders)
		(
			metadata("NoSerialize", true)
		)
		.property("Name", &GfxMaterial::Name);
}

namespaceStart(Json)

void serializeToJson(rttr::instance obj, PrettyWriter<StringBuffer>& writer);
bool8_t writeAtomicTypes(const type& t, const variant& var, PrettyWriter<StringBuffer>& writer);
void writeArray(const variant_sequential_view& view, PrettyWriter<StringBuffer>& writer);
void writeAssociativeContainer(const variant_associative_view& view, PrettyWriter<StringBuffer>& writer);
bool8_t writeVariant(const variant& var, PrettyWriter<StringBuffer>& writer);

bool8_t writeAtomicTypes(const type& t, const variant& var, PrettyWriter<StringBuffer>& writer)
{
	if (t.is_arithmetic())
	{
		if (t == type::get<bool8_t>()) writer.Bool(var.to_bool());
		else if (t == type::get<char8_t>()) writer.Bool(var.to_bool());
		else if (t == type::get<int8_t>()) writer.Int(var.to_int8());
		else if (t == type::get<int16_t>()) writer.Int(var.to_int16());
		else if (t == type::get<int32_t>()) writer.Int(var.to_int32());
		else if (t == type::get<int64_t>()) writer.Int64(var.to_int64());
		else if (t == type::get<uint8_t>()) writer.Uint(var.to_uint8());
		else if (t == type::get<uint16_t>()) writer.Uint(var.to_uint16());
		else if (t == type::get<uint32_t>()) writer.Uint(var.to_uint32());
		else if (t == type::get<uint64_t>()) writer.Uint64(var.to_uint64());
		else if (t == type::get<float32_t>()) writer.Double(var.to_double());
		else if (t == type::get<double64_t>()) writer.Double(var.to_double());
		return true;
	}
	else if (t.is_enumeration())
	{
		bool8_t ok = false;
		auto result = var.to_string(&ok);
		if (ok)
		{
			writer.String(var.to_string());
		}
		else
		{
			ok = false;
			auto value = var.to_uint64(&ok);
			if (ok)
			{
				writer.Uint64(value);
			}
			else
			{
				writer.Null();
			}
		}

		return true;
	}
	else if (t == type::get<std::string>())
	{
		writer.String(var.to_string());
		return true;
	}

	return false;
}

void writeArray(const variant_sequential_view& view, PrettyWriter<StringBuffer>& writer)
{
	writer.StartArray();
	for (const auto& item : view)
	{
		if (item.is_sequential_container())
		{
			writeArray(item.create_sequential_view(), writer);
		}
		else
		{
			auto wrappedVar = item.extract_wrapped_value();
			auto valueType = wrappedVar.get_type();
			if (valueType.is_arithmetic() || valueType == type::get<std::string>() || valueType.is_enumeration())
			{
				writeAtomicTypes(valueType, wrappedVar, writer);
			}
			else
			{
				serializeToJson(wrappedVar, writer);
			}
		}
	}
	writer.EndArray();
}

void writeAssociativeContainer(const variant_associative_view& view, PrettyWriter<StringBuffer>& writer)
{
	static const string_view key("key");
	static const string_view value("value");

	writer.StartArray();

	if (view.is_key_only_type())
	{
		for (auto& item : view)
		{
			writeVariant(item.first, writer);
		}
	}
	else
	{
		for (auto& item : view)
		{
			writer.StartObject();
			writer.String(key.data(), static_cast<rapidjson::SizeType>(key.length()), false);

			writeVariant(item.first, writer);

			writer.String(value.data(), static_cast<rapidjson::SizeType>(value.length()), false);

			writeVariant(item.second, writer);

			writer.EndObject();
		}
	}

	writer.EndArray();
}

bool8_t writeVariant(const variant& var, PrettyWriter<StringBuffer>& writer)
{
	auto valueType = var.get_type();
	auto wrappedType = valueType.is_wrapper() ? valueType.get_wrapped_type() : valueType;
	bool8_t isWrapper = wrappedType != valueType;
	if (writeAtomicTypes(
		isWrapper ? wrappedType : valueType, 
		isWrapper ? var.extract_wrapped_value() : var, 
		writer))
	{
	}
	else if (var.is_sequential_container())
	{
		writeArray(var.create_sequential_view(), writer);
	}
	else if (var.is_associative_container())
	{
		writeAssociativeContainer(var.create_associative_view(), writer);
	}
	else
	{
		auto child_props = isWrapper ? wrappedType.get_properties() : valueType.get_properties();
		if (!child_props.empty())
		{
			serializeToJson(var, writer);
		}
		else
		{
			bool ok = false;
			auto text = var.to_string(&ok);
			if (!ok)
			{
				writer.String(text);
				return false;
			}

			writer.String(text);
		}
	}

	return true;
}

void serializeToJson(rttr::instance obj, PrettyWriter<StringBuffer>& writer)
{
	writer.StartObject();

	auto realObj = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
	auto propList = realObj.get_derived_type().get_properties();
	for (auto prop : propList)
	{
		if (prop.get_metadata("NoSerialize"))
		{
			continue;
		}

		auto propValue = prop.get_value(obj);
		if (!propValue)
		{
			continue;
		}

		const auto name = prop.get_name();
		writer.String(name.data(), static_cast<SizeType>(name.length()), false);
		verify(writeVariant(propValue, writer));
	}

	writer.EndObject();
}

namespaceEnd(Json)

void GfxMaterial::serialize()
{
	rttr::instance obj(*this);
	assert(obj.is_valid());

	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);

	Json::serializeToJson(*this, writer);

	auto json = buffer.GetString();
	assert(Name.length() > 0u);
}

void GfxMaterial::deserialize()
{
}
