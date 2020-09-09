#include "Colorful/Public/GfxMaterial.h"
#include "Colorful/Public/GfxEngine.h"
#include "AssetTool/AssetDatabase.h"
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
		value("eBinNormal", eRVertexUsage::eBiNormal),
		value("eTangent", eRVertexUsage::eTangent),
		value("eBiTangent", eRVertexUsage::eBiTangent),
		value("eTexcoord0", eRVertexUsage::eTexcoord0),
		value("eColor0", eRVertexUsage::eColor0)
	);

	rttr::registration::enumeration<eRFormat>("eRFormat")
	(
		value("eUnknownFormat", eRFormat::eUnknownFormat),
		value("eD16_UNorm", eRFormat::eD16_UNorm),
		value("eD32_Float", eRFormat::eD32_Float),
		value("eD32_Float_S8X24_Uint", eRFormat::eD32_Float_S8X24_Uint),
		value("eD24_UNorm_S8_UInt", eRFormat::eD24_UNorm_S8_UInt),
		value("eRGBA32_Typeless", eRFormat::eRGBA32_Typeless),
		value("eRGBA32_Float", eRFormat::eRGBA32_Float),
		value("eRGBA32_UInt", eRFormat::eRGBA32_UInt),
		value("eRGBA32_SInt", eRFormat::eRGBA32_SInt),
		value("eRGB32_Typeless", eRFormat::eRGB32_Typeless),
		value("eRGB32_Float", eRFormat::eRGB32_Float),
		value("eRGB32_UInt", eRFormat::eRGB32_UInt),
		value("eRGB32_SInt", eRFormat::eRGB32_SInt),
		value("eRGBA16_Typeless", eRFormat::eRGBA16_Typeless),
		value("eRGBA16_Float", eRFormat::eRGBA16_Float),
		value("eRGBA16_UNorm", eRFormat::eRGBA16_UNorm),
		value("eRGBA16_UInt", eRFormat::eRGBA16_UInt),
		value("eRGBA16_SNorm", eRFormat::eRGBA16_SNorm),
		value("eRGBA16_SInt", eRFormat::eRGBA16_SInt),
		value("eRG32_Typeless", eRFormat::eRG32_Typeless),
		value("eRG32_Float", eRFormat::eRG32_Float),
		value("eRG32_UInt", eRFormat::eRG32_UInt),
		value("eRG32_SInt", eRFormat::eRG32_SInt),
		value("eR32G8X24_Typeless", eRFormat::eR32G8X24_Typeless),
		value("eR32_Float_X8X24_Typeless", eRFormat::eR32_Float_X8X24_Typeless),
		value("eX32_Typeless_G8X24_UInt", eRFormat::eX32_Typeless_G8X24_UInt),
		value("eRGB10A2_Typeless", eRFormat::eRGB10A2_Typeless),
		value("eRGB10A2_UNorm", eRFormat::eRGB10A2_UNorm),
		value("eRGB10A2_UInt", eRFormat::eRGB10A2_UInt),
		value("eRG11B10_Float", eRFormat::eRG11B10_Float),
		value("eRGBA8_Typeless", eRFormat::eRGBA8_Typeless),
		value("eRGBA8_UNorm", eRFormat::eRGBA8_UNorm),
		value("eRGBA8_UNorm_SRGB", eRFormat::eRGBA8_UNorm_SRGB),
		value("eRGBA8_UInt", eRFormat::eRGBA8_UInt),
		value("eRGBA8_SNorm", eRFormat::eRGBA8_SNorm),
		value("eRGBA8_SInt", eRFormat::eRGBA8_SInt),
		value("eRG16_Typeless", eRFormat::eRG16_Typeless),
		value("eRG16_Float", eRFormat::eRG16_Float),
		value("eRG16_UNorm", eRFormat::eRG16_UNorm),
		value("eRG16_UInt", eRFormat::eRG16_UInt),
		value("eRG16_SNorm", eRFormat::eRG16_SNorm),
		value("eRG16_SInt", eRFormat::eRG16_SInt),
		value("eR32_Typeless", eRFormat::eR32_Typeless),
		value("eR32_Float", eRFormat::eR32_Float),
		value("eR32_UInt", eRFormat::eR32_UInt),
		value("eR32_SInt", eRFormat::eR32_SInt),
		value("eR24G8_Typeless", eRFormat::eR24G8_Typeless),
		value("eR24_UNorm_X8_Typeless", eRFormat::eR24_UNorm_X8_Typeless),
		value("eX24_Typelesss_G8_UInt", eRFormat::eX24_Typelesss_G8_UInt),
		value("eRG8_Typeless", eRFormat::eRG8_Typeless),
		value("eRG8_UNorm", eRFormat::eRG8_UNorm),
		value("eRG8_UInt", eRFormat::eRG8_UInt),
		value("eRG8_SNorm", eRFormat::eRG8_SNorm),
		value("eRG8_SInt", eRFormat::eRG8_SInt),
		value("eR16_Typeless", eRFormat::eR16_Typeless),
		value("eR16_Float", eRFormat::eR16_Float),
		value("eR16_UNorm", eRFormat::eR16_UNorm),
		value("eR16_UInt", eRFormat::eR16_UInt),
		value("eR16_SNorm", eRFormat::eR16_SNorm),
		value("eR16_SInt", eRFormat::eR16_SInt),
		value("eR8_Typeless", eRFormat::eR8_Typeless),
		value("eR8_UNorm", eRFormat::eR8_UNorm),
		value("eR8_UInt", eRFormat::eR8_UInt),
		value("eR8_SNorm", eRFormat::eR8_SNorm),
		value("eR8_SInt", eRFormat::eR8_SInt),
		value("eA8_UNorm", eRFormat::eA8_UNorm),
		value("eR1_UNorm", eRFormat::eR1_UNorm),
		value("eRGB9E5_SharedXP", eRFormat::eRGB9E5_SharedXP),
		value("eRG8_BG8_UNorm", eRFormat::eRG8_BG8_UNorm),
		value("eGR8_GB8_UNorm", eRFormat::eGR8_GB8_UNorm),
		value("eBC1_Typeless", eRFormat::eBC1_Typeless),
		value("eBC1_UNorm", eRFormat::eBC1_UNorm),
		value("eBC1_UNorm_SRGB", eRFormat::eBC1_UNorm_SRGB),
		value("eBC2_Typeless", eRFormat::eBC2_Typeless),
		value("eBC2_UNorm", eRFormat::eBC2_UNorm),
		value("eBC2_UNorm_SRGB", eRFormat::eBC2_UNorm_SRGB),
		value("eBC3_Typeless", eRFormat::eBC3_Typeless),
		value("eBC3_UNorm", eRFormat::eBC3_UNorm),
		value("eBC3_UNorm_SRGB", eRFormat::eBC3_UNorm_SRGB),
		value("eBC4_Typeless", eRFormat::eBC4_Typeless),
		value("eBC4_UNorm", eRFormat::eBC4_UNorm),
		value("eBC4_SNorm", eRFormat::eBC4_SNorm),
		value("eBC5_Typeless", eRFormat::eBC5_Typeless),
		value("eBC5_UNorm", eRFormat::eBC5_UNorm),
		value("eBC5_SNorm", eRFormat::eBC5_SNorm),
		value("eB5G6R5_UNorm", eRFormat::eB5G6R5_UNorm),
		value("eBGR5A1_UNorm", eRFormat::eBGR5A1_UNorm),
		value("eBGRA8_UNorm", eRFormat::eBGRA8_UNorm),
		value("eBGRX8_UNorm", eRFormat::eBGRX8_UNorm),
		value("eRGB10_XR_Bias_A2_UNorm", eRFormat::eRGB10_XR_Bias_A2_UNorm),
		value("eBGRA8_Typeless", eRFormat::eBGRA8_Typeless),
		value("eBGRA8_UNorm_SRGB", eRFormat::eBGRA8_UNorm_SRGB),
		value("eBGRX8_Typeless", eRFormat::eBGRX8_Typeless),
		value("eBGRX8_UNorm_SRGB", eRFormat::eBGRX8_UNorm_SRGB),
		value("eBC6H_Typeless", eRFormat::eBC6H_Typeless),
		value("eBC6H_UF16", eRFormat::eBC6H_UF16),
		value("eBC6H_SF16", eRFormat::eBC6H_SF16),
		value("eBC7_Typeless", eRFormat::eBC7_Typeless),
		value("eBC7_UNorm", eRFormat::eBC7_UNorm),
		value("eBC7_UNorm_SRGB", eRFormat::eBC7_UNorm_SRGB),
		value("eAYUV", eRFormat::eAYUV),
		value("eAYUV", eRFormat::eAYUV),
		value("eY416", eRFormat::eY416),
		value("eNV12", eRFormat::eNV12),
		value("eP010", eRFormat::eP010),
		value("eP016", eRFormat::eP016),
		value("e420_Opaque", eRFormat::e420_Opaque),
		value("eYUY2", eRFormat::eYUY2),
		value("eY210", eRFormat::eY210),
		value("eY216", eRFormat::eY216),
		value("eNV11", eRFormat::eNV11),
		value("eAI44", eRFormat::eAI44),
		value("eIA44", eRFormat::eIA44),
		value("eP8", eRFormat::eP8),
		value("eAP8", eRFormat::eAP8),
		value("eBGRA4_UNorm", eRFormat::eBGRA4_UNorm),
		value("eP208", eRFormat::eP208),
		value("eV208", eRFormat::eV208),
		value("eV408", eRFormat::eV408),
		value("eForce_UInt", eRFormat::eForce_UInt)
	);

	rttr::registration::class_<GfxVertexAttributes>("GfxVertexAttributes")
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

void serializeFromjson(instance obj, Value& jsonObject);
variant extractBasicTypes(Value& jsonValue);
void writeArray(variant_sequential_view& view, Value& jsonValue);
variant extractValue(Value::MemberIterator& itr, const type& t);
void writeAssociativeView(variant_associative_view& view, Value& json_array_value);

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

variant extractBasicTypes(Value& jsonValue)
{
	switch (jsonValue.GetType())
	{
	case kStringType:
		return std::string(jsonValue.GetString());
	case kNullType:     
		break;
	case kFalseType:
	case kTrueType:
		return jsonValue.GetBool();
	case kNumberType:
		if (jsonValue.IsInt()) return jsonValue.GetInt();
		else if (jsonValue.IsDouble()) return jsonValue.GetDouble();
		else if (jsonValue.IsUint()) return jsonValue.GetUint();
		else if (jsonValue.IsInt64()) return jsonValue.GetInt64();
		else if (jsonValue.IsUint64()) return jsonValue.GetUint64();
	// we handle only the basic types here
	case kObjectType:
	case kArrayType: 
		return variant();
	}

	return variant();
}

void writeArray(variant_sequential_view& view, Value& jsonValue)
{
	view.set_size(jsonValue.Size());
	const type array_value_type = view.get_rank_type(1);

	for (SizeType i = 0; i < jsonValue.Size(); ++i)
	{
		auto& json_index_value = jsonValue[i];
		if (json_index_value.IsArray())
		{
			auto sub_array_view = view.get_value(i).create_sequential_view();
			writeArray(sub_array_view, json_index_value);
		}
		else if (json_index_value.IsObject())
		{
			variant var_tmp = view.get_value(i);
			variant wrapped_var = var_tmp.extract_wrapped_value();
			serializeFromjson(wrapped_var, json_index_value);
			view.set_value(i, wrapped_var);
		}
		else
		{
			variant extracted_value = extractBasicTypes(json_index_value);
			if (extracted_value.convert(array_value_type))
			{
				view.set_value(i, extracted_value);
			}
		}
	}
}

variant extractValue(Value::MemberIterator& itr, const type& t)
{
	auto& json_value = itr->value;
	variant extracted_value = extractBasicTypes(json_value);
	const bool could_convert = extracted_value.convert(t);
	if (!could_convert)
	{
		if (json_value.IsObject())
		{
			constructor ctor = t.get_constructor();
			for (auto& item : t.get_constructors())
			{
				if (item.get_instantiated_type() == t)
					ctor = item;
			}
			extracted_value = ctor.invoke();
			serializeFromjson(extracted_value, json_value);
		}
	}

	return extracted_value;
}

void writeAssociativeView(variant_associative_view& view, Value& json_array_value)
{
	for (SizeType i = 0; i < json_array_value.Size(); ++i)
	{
		auto& json_index_value = json_array_value[i];
		if (json_index_value.IsObject()) // a key-value associative view
		{
			Value::MemberIterator key_itr = json_index_value.FindMember("key");
			Value::MemberIterator value_itr = json_index_value.FindMember("value");

			if (key_itr != json_index_value.MemberEnd() &&
				value_itr != json_index_value.MemberEnd())
			{
				auto key_var = extractValue(key_itr, view.get_key_type());
				auto value_var = extractValue(value_itr, view.get_value_type());
				if (key_var && value_var)
				{
					view.insert(key_var, value_var);
				}
			}
		}
		else // a key-only associative view
		{
			variant extracted_value = extractBasicTypes(json_index_value);
			if (extracted_value && extracted_value.convert(view.get_key_type()))
			{
				view.insert(extracted_value);
			}
		}
	}
}

void serializeFromjson(instance obj, Value& jsonObject)
{
	auto realObj = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
	const auto propList = realObj.get_derived_type().get_properties();

	for (auto prop : propList)
	{
		Value::MemberIterator ret = jsonObject.FindMember(prop.get_name().data());
		if (ret == jsonObject.MemberEnd())
		{
			continue;
		}

		const type valueType = prop.get_type();

		auto& jsonValue = ret->value;
		switch (jsonValue.GetType())
		{
		case kArrayType:
		{
			variant var;
			if (valueType.is_sequential_container())
			{
				var = prop.get_value(realObj);
				auto view = var.create_sequential_view();
				writeArray(view, jsonValue);
			}
			else if (valueType.is_associative_container())
			{
				var = prop.get_value(realObj);
				auto associativeView = var.create_associative_view();
				writeAssociativeView(associativeView, jsonValue);
			}

			prop.set_value(realObj, var);
			break;
		}
		case kObjectType:
		{
			auto var = prop.get_value(realObj);
			serializeFromjson(var, jsonValue);
			prop.set_value(realObj, var);
			break;
		}
		default:
		{
			auto extractedValue = extractBasicTypes(jsonValue);
			if (extractedValue.convert(valueType))
			{
				prop.set_value(realObj, extractedValue);
			}
		}
		}
	}
}

void deserializeFromJson(rttr::instance obj, const std::string& json)
{
	Document doc;
	verify(!doc.Parse(json.c_str()).HasParseError());

	serializeFromjson(obj, doc);
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

	auto path = AssetTool::AssetDatabase::instance().assetBasePath() + "\\Materials\\" + Name;
	File::write(File::eText, path, (const byte8_t*)json, strlen(json));
}

void GfxMaterial::deserialize()
{
	if (!isValid())
	{
		auto asset = AssetTool::AssetDatabase::instance().tryToGetAsset(Name);
		assert(asset);

		std::string json((const char8_t*)asset->data().get());
		Json::deserializeFromJson(*this, json);

		for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
		{
			switch (i)
			{
			case eVertexShader:
				Shaders[i] = g_GfxEngine->createVertexShader(ShaderNames[i]);
				break;
			case eHullShader:
				break;
			case eDomainShader:
				break;
			case eGeometryShader:
				break;
			case eFragmentShader:
				Shaders[i] = g_GfxEngine->createFragmentShader(ShaderNames[i]);
				break;
			case eComputeShader:
				break;
			}
		}
	}
}
