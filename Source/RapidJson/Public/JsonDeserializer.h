#pragma once

#include "RapidJsonLog.h"
#include "JsonFwd.h"

namespace Json
{
namespace Deserializer
{
namespace Detail
{
struct FromJsonFunctor
{
	template <typename ContainerType, typename EncodingType, typename AllocatorType>
	void operator()(
		const rapidjson::GenericValue<EncodingType, AllocatorType>& json_value,
		ContainerType& container) const
	{
		FromJson(json_value, container);
	}

	template <typename ContainerType, typename EncodingType, typename AllocatorType>
	void operator()(
		const rapidjson::GenericMember<EncodingType, AllocatorType>& json_value,
		ContainerType& container) const
	{
		FromJson(json_value, container);
	}
};

// Template variables are required to have external linkage per the Standard.
template <typename DataType> constexpr DataType MakeOdrSafe{};
} // namespace detail

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
constexpr const auto& FromJson = Detail::MakeOdrSafe<Detail::FromJsonFunctor>;
}

struct DefaultInsertionPolicy
{
	template <typename DataType, typename ContainerType>
	static void Insert(DataType&& data, ContainerType& container)
	{
		container.Emplace(Forward<DataType>(data));
	}
};

struct BackInsertionPolicy
{
	template <typename DataType, typename ContainerType>
	static void Insert(DataType&& data, ContainerType& container)
	{
		container.Add(MoveTemp(data));
	}
};

namespace Detail
{
template <typename JsonValueType> FString TypeToString(const JsonValueType& value)
{
	switch (value.GetType())
	{
	case rapidjson::Type::kArrayType:
		return "an array";
	case rapidjson::Type::kTrueType:
	case rapidjson::Type::kFalseType:
		return "a boolean";
	case rapidjson::Type::kNullType:
		return "null";
	case rapidjson::Type::kNumberType:
		return "a numeric type";
	case rapidjson::Type::kObjectType:
		return "an object";
	case rapidjson::Type::kStringType:
		return "a string";
	}

	return "an unknown type";
}

template <
	typename StringType, typename InputEncodingType, typename OutputEncodingType,
	typename EncodingType, typename AllocatorType>
	StringType TransCode(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
{
	assert(value.IsString());

	rapidjson::GenericStringStream<InputEncodingType> source{ value.GetString() };
	rapidjson::GenericStringBuffer<OutputEncodingType> target;

	using TranscoderType = rapidjson::Transcoder<InputEncodingType, OutputEncodingType>;

	bool successfully_transcoded = true;
	while (source.Peek() != '\0' && successfully_transcoded)
	{
		successfully_transcoded = TranscoderType::Transcode(source, target);
	}

	if (!successfully_transcoded)
	{
		UE_LOG(LogRapidJson, Error, TEXT("Failed to transcode strings."));
		return "";
	}

	return target.GetString();
}

template <typename DataType> struct ValueExtractor
{
	template <typename EncodingType, typename AllocatorType>
	static bool
		ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
	{
		UE_LOG(LogRapidJson, Error, TEXT("Cannot extract unsupported type %s."), *TypeToString(value));
		return false;
	}
};

template <> struct ValueExtractor<bool>
{
	template <typename EncodingType, typename AllocatorType>
	static bool ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
	{
		if (!value.IsBool())
		{
			UE_LOG(LogRapidJson, Error, TEXT("Expected a bool, got %s."), *TypeToString(value));
			return false;
		}

		return value.GetBool();
	}
};

template <> struct ValueExtractor<int32>
{
	template <typename EncodingType, typename AllocatorType>
	static int32 ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
	{
		if (!value.IsInt())
		{
			UE_LOG(LogRapidJson, Error, TEXT("Expected a 32-bit integer, got %s."), *TypeToString(value));
			return 0;
		}

		return value.GetInt();
	}
};

template <> struct ValueExtractor<uint32>
{
	template <typename EncodingType, typename AllocatorType>
	static uint32 ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
	{
		if (!value.IsUint())
		{
			UE_LOG(LogRapidJson, Error, TEXT("Expected an unsigned, 32-bit integer, got %s."), *TypeToString(value));
			return 0;
		}

		return value.GetUint();
	}
};

template <> struct ValueExtractor<int64>
{
	template <typename EncodingType, typename AllocatorType>
	static int64 ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
	{
		if (!value.IsInt64())
		{
			UE_LOG(LogRapidJson, Error, TEXT("Expected a 64-bit integer, got %s."), *TypeToString(value));
			return 0;
		}

		return value.GetInt64();
	}
};

template <> struct ValueExtractor<uint64>
{
	template <typename EncodingType, typename AllocatorType>
	static uint64 ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
	{
		if (!value.IsUint64())
		{
			UE_LOG(LogRapidJson, Error, TEXT("Expected an unsigned, 64-bit integer, got %s."), *TypeToString(value));
			return 0;
		}

		return value.GetUint64();
	}
};

template <> struct ValueExtractor<float>
{
	template <typename EncodingType, typename AllocatorType>
	static float ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
	{
		if (!value.IsDouble())
		{
			UE_LOG(LogRapidJson, Error, TEXT("Expected a real, got %s."), *TypeToString(value));
			return 0;
		}

		return value.GetDouble();
	}
};

template <> struct ValueExtractor<double>
{
	template <typename EncodingType, typename AllocatorType>
	static double ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
	{
		if (!value.IsDouble())
		{
			UE_LOG(LogRapidJson, Error, TEXT("Expected a real, got %s."), *TypeToString(value));
			return 0;
		}

		return value.GetDouble();
	}
};

template <> struct ValueExtractor<long double>
{
	template <typename EncodingType, typename AllocatorType>
	static long double ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
	{
		if (!value.IsDouble())
		{
			UE_LOG(LogRapidJson, Error, TEXT("Expected a real, got %s."), *TypeToString(value));
			return 0;
		}

		return value.GetDouble();
	}
};

template <> struct ValueExtractor<FString>
{
	template <typename EncodingType, typename AllocatorType>
	static auto ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
		-> typename TEnableIf<
		TIsSame<typename EncodingType::Ch, ANSICHAR>::Value, FString>::Type
	{
		if (!value.IsString())
		{
			UE_LOG(LogRapidJson, Error, TEXT("Expected a string, got %s."), *TypeToString(value));
			return FString::Empty;
		}

		return TCHAR_TO_UTF8(value.GetString());
	}

	template <typename EncodingType, typename AllocatorType>
	static auto ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
		-> typename TEnableIf<
		TIsSame<typename EncodingType::Ch, TCHAR>::Value, FString>::Type
	{
		if (!value.IsString())
		{
			UE_LOG(LogRapidJson, Error, TEXT("Expected a string, got %s."), *TypeToString(value));
			return "";
		}

		return TransCode<FString, rapidjson::UTF16<>, rapidjson::UTF8<>>(value);
	}
};

template <> struct ValueExtractor<FName>
{
    template <typename EncodingType, typename AllocatorType>
    static auto ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
        -> typename TEnableIf<
        TIsSame<typename EncodingType::Ch, ANSICHAR>::Value, FName>::Type
    {
        if (!value.IsString())
        {
            UE_LOG(LogRapidJson, Error, TEXT("Expected a string, got %s."), *TypeToString(value));
            return FString::Empty;
        }

        return TCHAR_TO_UTF8(value.GetString());
    }

    template <typename EncodingType, typename AllocatorType>
    static auto ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
        -> typename TEnableIf<
        TIsSame<typename EncodingType::Ch, TCHAR>::Value, FName>::Type
    {
        if (!value.IsString())
        {
            UE_LOG(LogRapidJson, Error, TEXT("Expected a string, got %s."), *TypeToString(value));
            return "";
        }

        return TransCode<FName, rapidjson::UTF16<>, rapidjson::UTF8<>>(value);
    }
};

template <> struct ValueExtractor<FText>
{
    template <typename EncodingType, typename AllocatorType>
    static auto ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
        -> typename TEnableIf<
        TIsSame<typename EncodingType::Ch, ANSICHAR>::Value, FText>::Type
    {
        if (!value.IsString())
        {
            UE_LOG(LogRapidJson, Error, TEXT("Expected a string, got %s."), *TypeToString(value));
            return FText::GetEmpty();
        }

        return FText::FromString(TCHAR_TO_UTF8(value.GetString()));
    }

    template <typename EncodingType, typename AllocatorType>
    static auto ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
        -> typename TEnableIf<
        TIsSame<typename EncodingType::Ch, TCHAR>::Value, FText>::Type
    {
        if (!value.IsString())
        {
            UE_LOG(LogRapidJson, Error, TEXT("Expected a string, got %s."), *TypeToString(value));
            return FText::GetEmpty();
        }

        return FText::FromString(TransCode<FString, rapidjson::UTF16<>, rapidjson::UTF8<>>(value));
    }
};

template <typename DataType> struct ValueExtractor<TUniquePtr<DataType>>
{
	template <typename EncodingType, typename AllocatorType>
	static TUniquePtr<DataType>
		ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
	{
		if (value.IsNull())
		{
			return nullptr;
		}

		return TUniquePtr<DataType>(ValueExtractor<DataType>::ExtractOrThrow(value));
	}
};

template <typename DataType> struct ValueExtractor<TSharedPtr<DataType>>
{
	template <typename EncodingType, typename AllocatorType>
	static TSharedPtr<DataType>
		ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
	{
		if (value.IsNull())
		{
			return nullptr;
		}

		return MakeShared<DataType>(ValueExtractor<DataType>::ExtractOrThrow(value));
	}
};

template <typename DataType> struct ValueExtractor<TOptional<DataType>>
{
	template <typename EncodingType, typename AllocatorType>
	static TOptional<DataType>
		ExtractOrThrow(const rapidjson::GenericValue<EncodingType, AllocatorType>& value)
	{
		if (value.IsNull())
		{
			return TOptional<DataType>();
		}

		return ValueExtractor<DataType>::ExtractOrThrow(value);
	}
};

template <typename InsertionPolicy, typename DataType, typename ContainerType>
void Insert(DataType&& value, ContainerType& container)
{
	static_assert(
		TIsConstructible<DataType, typename ContainerType::ElementType>::Value,
		"The type being inserted is not the same as, or cannot be converted to, the "
		"container's value type.");

	InsertionPolicy::Insert(MoveTemp(value), container);
}

template <typename PairType, typename EncodingType, typename AllocatorType>
PairType ConstructNestedPair(const rapidjson::GenericMember<EncodingType, AllocatorType>& member)
{
	using KeyType = typename TDecay<typename PairType::KeyType>::Type;
	using NestedType = typename PairType::ValueType;

	static_assert(
		TIsConstructible<NestedType>::Value,
		"Nested container must be default constructible.");

	NestedType container;
	Deserializer::FromJson(member.value, container);

	return MakeTuple(ValueExtractor<KeyType>::ExtractOrThrow(member.name), MoveTemp(container));
}

template <typename PairType, typename EncodingType, typename AllocatorType>
auto ToKeyValuePair(const rapidjson::GenericMember<EncodingType, AllocatorType>& member) ->
typename TEnableIf<
	Traits::TreatAsValue<typename PairType::ValueType>::Value, PairType>::Type
{
	using KeyType = typename TDecay<typename PairType::KeyType>::Type;
	using ValueType = typename PairType::ValueType;

	return MakeTuple(ValueExtractor<KeyType>::ExtractOrThrow(member.name),
		ValueExtractor<ValueType>::ExtractOrThrow(member.value));
}


template <typename PairType, typename EncodingType, typename AllocatorType>
auto ToKeyValuePair(const rapidjson::GenericMember<EncodingType, AllocatorType>& member) ->
typename TEnableIf<
	Traits::TreatAsObject<typename PairType::ValueType>::Value, PairType>::Type
{
	if (!member.value.IsObject())
	{
		UE_LOG(LogRapidJson, Error, TEXT("Expected an object, got %s."), *TypeToString(member.value));
		return;
	}

	return ConstructNestedPair<PairType>(member);
}

template <typename PairType, typename EncodingType, typename AllocatorType>
auto ToKeyValuePair(const rapidjson::GenericMember<EncodingType, AllocatorType>& member) ->
typename TEnableIf<
	Traits::TreatAsArray<typename PairType::ValueType>::Value, PairType>::Type
{
	if (!member.value.IsArray())
	{
		UE_LOG(LogRapidJson, Error, TEXT("Expected an array, got %s."), *TypeToString(member.value));
		return;
	}

	return ConstructNestedPair<PairType>(member);
}

template <
	typename InsertionPolicy, typename EncodingType, typename AllocatorType, typename ContainerType>
	void DispatchInsertion(
		const rapidjson::GenericMember<EncodingType, AllocatorType>& member, ContainerType& container)
{
	auto pair = ToKeyValuePair<typename ContainerType::ElementType>(member);
	Insert<InsertionPolicy>(MoveTemp(pair), container);
}

template <
	typename InsertionPolicy, typename ContainerType, typename EncodingType, typename AllocatorType>
	auto DispatchInsertion(
		const rapidjson::GenericValue<EncodingType, AllocatorType>& json_value, ContainerType& container) ->
	typename TEnableIf<Traits::TreatAsValue<typename ContainerType::ElementType>::Value>::Type
{
	using DesiredType = typename ContainerType::ElementType;
	DesiredType desired_type;
	Deserializer::FromJson(json_value, desired_type);

	Insert<InsertionPolicy>(MoveTemp(desired_type), container);
}

template <
	typename InsertionPolicy, typename ContainerType, typename EncodingType, typename AllocatorType>
	auto DispatchInsertion(
		const rapidjson::GenericValue<EncodingType, AllocatorType>& json_value, ContainerType& container) ->
	typename TEnableIf<
	Traits::TreatAsArray<typename ContainerType::ElementType>::Value ||
	Traits::TreatAsObject<typename ContainerType::ElementType>::Value>::Type
{
	static_assert(
		TIsConstructible<typename ContainerType::ElementType>::Value,
		"Nested container must be default constructible.");

	using NestedContainerType = typename ContainerType::ElementType;

	NestedContainerType nested_container;
	Deserializer::FromJson(json_value, nested_container);

	Insert<InsertionPolicy>(MoveTemp(nested_container), container);
}

template <
	typename InsertionPolicy, typename ContainerType, typename EncodingType, typename AllocatorType>
	void DeserializeJsonObject(
		const rapidjson::GenericValue<EncodingType, AllocatorType>& json_value, ContainerType& container)
{
	if (!json_value.IsObject())
	{
		UE_LOG(LogRapidJson, Error, TEXT("Expected an object, got %s."), *TypeToString(json_value));
		return;
	}

	const auto& json_object = json_value.GetObject();
	for (const auto& nested_json_value : json_object)
	{
		DispatchInsertion<InsertionPolicy>(nested_json_value, container);
	}
}

template <
	typename InsertionPolicy, typename ContainerType, typename EncodingType, typename AllocatorType>
	void DeserializeJsonArray(
		const rapidjson::GenericValue<EncodingType, AllocatorType>& json_value, ContainerType& container)
{
	if (!json_value.IsArray())
	{
		UE_LOG(LogRapidJson, Error, TEXT("Expected an array, got %s."), *TypeToString(json_value));
		return;
	}

	const auto& json_array = json_value.GetArray();
	for (const auto& nested_json_value : json_array)
	{
		DispatchInsertion<InsertionPolicy>(nested_json_value, container);
	}
}


template <typename ContainerType, typename EncodingType, typename AllocatorType>
auto FromJson(
	const rapidjson::GenericValue<EncodingType, AllocatorType>& json_value, ContainerType& container) ->
	typename TEnableIf<
	Traits::TreatAsArray<ContainerType>::Value>::Type
{
	DeserializeJsonArray<DefaultInsertionPolicy>(json_value, container);
}

template <typename ContainerType, typename EncodingType, typename AllocatorType>
auto FromJson(
	const rapidjson::GenericValue<EncodingType, AllocatorType>& json_value, ContainerType& container) ->
	typename TEnableIf<
	Traits::TreatAsObject<ContainerType>::Value>::Type
{
	DeserializeJsonObject<BackInsertionPolicy>(json_value, container);
}

template <typename DataType, typename EncodingType, typename AllocatorType>
auto FromJson(
	const rapidjson::GenericValue<EncodingType, AllocatorType>& json_value, DataType& data) ->
	typename TEnableIf<
	Traits::HasFromJson<DataType, EncodingType, AllocatorType>::Value>::Type
{
	if (!json_value.IsObject())
	{
		UE_LOG(LogRapidJson, Error, TEXT("Expected an object, got %s."), *TypeToString(json_value));
		return;
	}

	data.FromJson(json_value);
}

template <typename DataType, typename EncodingType, typename AllocatorType>
auto FromJson(
	const rapidjson::GenericMember<EncodingType, AllocatorType>& member, DataType& data) ->
	typename TEnableIf<
	Traits::HasFromJson<DataType, EncodingType, AllocatorType>::Value>::Type
{
	if (!member.value.IsObject())
	{
		UE_LOG(LogRapidJson, Error, TEXT("Expected an object, got %s."), *TypeToString(member.value));
		return;
	}

	data.FromJson(member.value);
}

template <typename DataType, typename EncodingType, typename AllocatorType>
auto FromJson(
	const rapidjson::GenericValue<EncodingType, AllocatorType>& json_value, DataType& data) ->
	typename TEnableIf<
	Traits::TreatAsValue<typename DataType>::Value &&
	!Traits::HasFromJson<DataType, EncodingType, AllocatorType>::Value>::Type
{
	data = ValueExtractor<DataType>::ExtractOrThrow(json_value);
}

template <typename DataType, typename EncodingType, typename AllocatorType>
auto FromJson(
	const rapidjson::GenericMember<EncodingType, AllocatorType>& member, DataType& data) ->
	typename TEnableIf<
	Traits::TreatAsValue<typename DataType>::Value &&
	!Traits::HasFromJson<DataType, EncodingType, AllocatorType>::Value>::Type
{
	data = ValueExtractor<DataType>::ExtractOrThrow(member.value);
}

} // namespace Detail
} // namespace Deserializer
} // namespace Json
