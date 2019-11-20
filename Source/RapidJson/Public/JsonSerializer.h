#pragma once

#include "CoreMinimal.h"
#include "JsonFwd.h"

namespace Json
{
namespace Serializer
{
namespace Detail
{
struct ToJsonFunctor
{
	template <typename DataType, typename WriterType>
	void operator()(WriterType& Writer, DataType&& Data) const
	{
		ToJson(Writer, Forward<DataType>(Data));
	}
};

// Template variables are required to have external linkage per the Standard.
template <typename DataType> constexpr DataType MakeOdrSafe{};
} // namespace Detail

namespace
{
// Variables declared at global scope will have external linkage, so we'll need to use an anonymous
// namespace to keep the enclosed reference "itself from being multiply defined." This works
// because anonymous namespaces behave as if a unique identifier were chosen for each translation
// unit in which it appears. As a result, the reference has internal linkage. However, since the
// reference below refers to a variable template (which is required to have external linkage), "all
// translation units [will] refer to the same entity," and therefore "there is no ODR violation."
//
// Source: Suggested Design for Customization Points
// [http://ericniebler.github.io/std/wg21/D4381.html]
//
// @note Use an `inline constexpr` variable when upgrading to C++17.
constexpr const auto& ToJson = Detail::MakeOdrSafe<Detail::ToJsonFunctor>;
} // namespace


namespace Detail
{
template <typename CharacterType> struct KeyHolder
{
};

template <> struct KeyHolder<ANSICHAR>
{
	template <typename DataType>
	static ANSICHAR* GenerateKey(const DataType& data)
	{
		return TCHAR_TO_UTF8(*data);
	}
};

template <> struct KeyHolder<TCHAR>
{
	template <typename DataType>
	static TCHAR* GenerateKey(const DataType& data)
	{
		return *data;
	}
};

template <typename Writer, typename KeyType, typename ValueType>
void InsertKeyValuePair(Writer& writer, const KeyType& key, const ValueType& value)
{
	writer.Key(KeyHolder<typename Writer::Ch>::GenerateKey(key));
	Serializer::ToJson(writer, value);
}

template <typename WriterType> void ToJson(WriterType& writer, bool data)
{
	writer.Bool(data);
}

template <typename WriterType> void ToJson(WriterType& writer, int32 data)
{
	writer.Int(data);
}

template <typename WriterType> void ToJson(WriterType& writer, uint32 data)
{
	writer.Uint(data);
}

template <typename WriterType> void ToJson(WriterType& writer, int64 data)
{
	writer.Int64(data);
}

template <typename WriterType> void ToJson(WriterType& writer, uint64 data)
{
	writer.Uint64(data);
}

template <typename WriterType, typename DataType>
auto ToJson(WriterType& writer, DataType data) ->
typename TEnableIf<TIsFloatingPoint<DataType>::Value>::Type
{
	writer.Double(data);
}

template <typename WriterType>
void ToJson(WriterType& writer, const FString& data)
{
	writer.String(TCHAR_TO_UTF8(*data));
}

template <typename WriterType>
auto ToJson(WriterType& writer, const ANSICHAR* data)
{
	if (data == nullptr)
	{
		writer.Null();
		return;
	}

	writer.String(data);
}

template <typename WriterType>
auto ToJson(WriterType& writer, const TCHAR* data)
{
	if (data == nullptr)
	{
		writer.Null();
		return;
	}

	writer.String(data);
}

template <typename WriterType, typename DataType>
void ToJson(WriterType& writer, const TSharedPtr<DataType>& pointer)
{
	if (pointer == nullptr)
	{
		writer.Null();
		return;
	}

	ToJson(writer, *pointer);
}

template <typename WriterType, typename DataType>
void ToJson(WriterType& writer, const TUniquePtr<DataType>& pointer)
{
	if (pointer == nullptr)
	{
		writer.Null();
		return;
	}

	ToJson(writer, *pointer);
}

template <typename WriterType, typename DataType>
void ToJson(WriterType& writer, const TWeakPtr<DataType>& weakPointer)
{
	const auto strongPointer = weakPointer.Pin();
	if (strongPointer == nullptr)
	{
		writer.Null();
		return;
	}

	ToJson(writer, *strongPointer);
}

template <typename WriterType, typename ContainerType>
auto ToJson(WriterType& writer, const ContainerType& container) ->
typename TEnableIf<Traits::TreatAsArray<ContainerType>::Value>::Type
{
	writer.StartArray();

	for (const auto& item : container)
	{
		ToJson(writer, item);
	}

	writer.EndArray();
}

template <typename WriterType, typename ContainerType>
auto ToJson(WriterType& writer, const ContainerType& container) ->
typename TEnableIf<Traits::TreatAsObject<ContainerType>::Value>::Type
{
	writer.StartObject();

	for (const auto& item : container)
	{
		ToJson(writer, item);
	}

	writer.EndObject();
}

template <typename WriterType, typename FirstType, typename SecondType>
void ToJson(WriterType& writer, const TPair<FirstType, SecondType>& pair)
{
	InsertKeyValuePair(writer, pair.Key, pair.Value);
}

template <typename WriterType, typename DataType>
void ToJson(WriterType& writer, const TOptional<DataType>& data)
{
	if (!data.IsSet())
	{
		writer.Null();
		return;
	}

	ToJson(writer, data.GetValue());
}

template <typename WriterType, typename DataType>
auto ToJson(WriterType& writer, const DataType& data) ->
typename TEnableIf<Traits::HasToJson<WriterType, DataType>::Value>::Type
{
	data.ToJson(writer);
}


} // namespace Detail
} // namespace Serializer
} // namespace Json
