#pragma once

#include "FutureStd.h"
#include "Templates/UnrealTemplate.h"

namespace Json
{
namespace Traits
{

template <bool _Val>
using TBoolConstant = TIntegralConstant<bool, _Val>;

using TrueType = TBoolConstant<true>;
using FalseType = TBoolConstant<false>;

// Traits to achieve conditional types for const/non-const iterators.
template <bool Condition, class TypeIfTrue, class TypeIfFalse>
class TConditional
{
public:
	typedef TypeIfFalse Type;
};

template <class TypeIfTrue, class TypeIfFalse>
class TConditional<true, TypeIfTrue, TypeIfFalse>
{
public:
	typedef TypeIfTrue Type;
};

template <typename> struct IsPair : FalseType
{
};

template <typename FirstType, typename SecondType>
struct IsPair<TPair<FirstType, SecondType>> : TrueType
{
};

template <typename FirstType, typename SecondType>
struct IsPair<std::pair<FirstType, SecondType>> : TrueType
{
};

template <typename, typename = void> struct TreatAsArray : FalseType
{
};


/**
 * @note Uses SFINAE to detect whether the input type has a `ValueType` definition, and if it does,
 * additional type traits will determine whether we're dealing with a container whose value type is
 * something other than a `std::pair<...>`.
 **/


template <typename DataType>
struct TreatAsArray<DataType, future_std::void_t<typename DataType::ElementType>>
	: TConditional<
	TContainerTraits<DataType>::MoveWillEmptyContainer && !IsPair<typename DataType::ElementType>::Value,
	TrueType, FalseType>::Type
{
};

template <typename... Args> struct TreatAsArray<TTuple<Args...>> : TrueType
{
};

template <typename ArrayType, size_t ArraySize>
struct TreatAsArray<ArrayType[ArraySize]> : TrueType
{
};

template <size_t ArraySize> struct TreatAsArray<ANSICHAR[ArraySize]> : FalseType
{
};

template <size_t ArraySize> struct TreatAsArray<TCHAR[ArraySize]> : FalseType
{
};


template <typename CharacterType, typename CharacterTraitsType, typename AllocatorType>
struct TreatAsArray<std::basic_string<CharacterType, CharacterTraitsType, AllocatorType>>
	: FalseType
{
};

template <>
struct TreatAsArray<FString> : FalseType
{
};

template <typename, typename = void> struct TreatAsObject : FalseType
{
};

/**
 * @note Uses SFINAE to detect whether the input type has a `ValueType` definition, and if it does,
 * additional type traits will determine whether we're dealing with a container whose value type is
 * a `std::pair<...>`. Anything that stores a `std::pair<...>` will therefore be treated as an
 * acceptable sink for a JSON object.
 **/
template <typename DataType>
struct TreatAsObject<DataType, future_std::void_t<typename DataType::ElementType>>
	: TConditional<
	TContainerTraits<DataType>::MoveWillEmptyContainer && IsPair<typename DataType::ElementType>::Value,
	TrueType, FalseType>::Type
{
};

template <typename DataType> struct TreatAsValue
{
	static constexpr bool Value =
		!(TreatAsArray<DataType>::Value || TreatAsObject<DataType>::Value);
};

template <typename, typename, typename = void> struct HasToJson : FalseType
{
};

template <typename WriterType, typename DataType>
struct HasToJson<
	WriterType, DataType, future_std::void_t<decltype(DeclVal<DataType&>()
		.ToJson(DeclVal<WriterType&>()))>> : TrueType
{
};

template <typename, typename, typename, typename = void> struct HasFromJson : FalseType
{
};

template <typename DataType, typename EncodingType, typename AllocatorType>
struct HasFromJson<
	DataType, EncodingType, AllocatorType, future_std::void_t<decltype(DeclVal<DataType&>()
		.FromJson(DeclVal<const rapidjson::GenericValue<EncodingType, AllocatorType>&>()))>> : TrueType
{
};

} // namespace Traits
} // namespace Json
