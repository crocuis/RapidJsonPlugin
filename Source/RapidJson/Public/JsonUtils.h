#pragma once

#include "RapidJsonPCH.h"
#include "JsonDeserializer.h"
#include "JsonSerializer.h"
#include "FutureStd.h"

namespace Json
{
namespace Detail
{
template <typename ContainerType, typename EncodingType, typename StreamType>
ContainerType Deserialize(StreamType& stream)
{
    rapidjson::GenericDocument<EncodingType> document;
    document.ParseStream(stream);

    if (document.HasParseError())
	{
		throw std::invalid_argument("Could not parse JSON document.");
	}

    static_assert(
		TIsConstructible<ContainerType>::Value,
        "The container must have a default constructible.");

    ContainerType container;
    Deserializer::FromJson(document, container);

    return container;
}
} // namespace Detail

template <
    typename InputEncodingType = rapidjson::UTF8<>, typename OutputEncodingType = rapidjson::UTF8<>,
    typename DataType>
JSON_UTILS_NODISCARD FString SerializeToJson(const DataType& data)
{
    rapidjson::GenericStringBuffer<OutputEncodingType> buffer;
    rapidjson::Writer<decltype(buffer), InputEncodingType, OutputEncodingType> writer{ buffer };

	Serializer::ToJson(writer, data);

    return buffer.GetString();
}

template <
    typename InputEncodingType = rapidjson::UTF8<>, typename OutputEncodingType = rapidjson::UTF8<>,
    typename DataType>
JSON_UTILS_NODISCARD FString SerializeToPrettyJson(const DataType& data)
{
    rapidjson::GenericStringBuffer<OutputEncodingType> buffer;
    rapidjson::PrettyWriter<decltype(buffer), InputEncodingType, OutputEncodingType> writer{
        buffer
    };

    Serializer::ToJson(writer, data);

    return buffer.GetString();
}

template <typename ContainerType>
JSON_UTILS_NODISCARD ContainerType DeserializeFromJson(const ANSICHAR* const json)
{
    using EncodingType = rapidjson::UTF8<>;

    rapidjson::GenericStringStream<EncodingType> string_stream{ json };
    return Detail::Deserialize<ContainerType, EncodingType>(string_stream);
}

template <typename ContainerType>
JSON_UTILS_NODISCARD ContainerType DeserializeFromJson(const TCHAR* const json)
{
	using EncodingType = rapidjson::UTF16<>;

	rapidjson::GenericStringStream<EncodingType> string_stream{ json };
	return Detail::Deserialize<ContainerType, EncodingType>(string_stream);
}

template <typename ContainerType>
JSON_UTILS_NODISCARD ContainerType DeserializeFromJson(const FString& json)
{
    return DeserializeFromJson<ContainerType>(*json);
}

} // namespace Json
