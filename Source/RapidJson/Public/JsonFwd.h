#pragma once

#include <rapidjson/document.h>
#include "CoreMinimal.h"
#include "JsonTraits.h"

namespace Json
{
namespace Serializer
{
namespace Detail
{
template <typename WriterType> void ToJson(WriterType& writer, bool data);

template <typename WriterType> void ToJson(WriterType& writer, int32  data);

template <typename WriterType> void ToJson(WriterType& writer, uint32 data);

template <typename WriterType> void ToJson(WriterType& writer, int64 data);

template <typename WriterType> void ToJson(WriterType& writer, uint64 data);

template <typename WriterType, typename DataType>
auto ToJson(WriterType& writer, DataType data) ->
    typename TEnableIf<TIsFloatingPoint<DataType>::Value>::Type;

template <typename WriterType, typename CharacterType>
void ToJson(
    WriterType& writer, const FString& data);

template <typename WriterType> auto ToJson(WriterType& writer, const ANSICHAR* data);
	
template <typename WriterType> auto ToJson(WriterType& writer, const TCHAR* data);

template <typename WriterType, typename DataType>
void ToJson(WriterType& writer, const TSharedPtr<DataType>& pointer);

template <typename WriterType, typename DataType>
void ToJson(WriterType& writer, const TUniquePtr<DataType>& pointer);

template <typename WriterType, typename DataType>
void ToJson(WriterType& writer, const TWeakPtr<DataType>& weakPointer);

template <typename WriterType, typename ContainerType>
auto ToJson(WriterType& writer, const ContainerType& container) ->
    typename TEnableIf<Traits::TreatAsArray<ContainerType>::Value>::Type;

template <typename WriterType, typename ContainerType>
auto ToJson(WriterType& writer, const ContainerType& container) ->
    typename TEnableIf<Traits::TreatAsObject<ContainerType>::Value>::Type;

template <typename WriterType, typename FirstType, typename SecondType>
void ToJson(WriterType& writer, const TPair<FirstType, SecondType>& pair);

template <typename WriterType, typename DataType>
void ToJson(WriterType& writer, const TOptional<DataType>& data);

template <typename WriterType, typename DataType>
auto ToJson(WriterType& writer, const DataType& data) ->
	typename TEnableIf<Traits::HasToJson<WriterType, DataType>::Value>::Type;

} // namespace Detail
} // namespace Serializer

namespace Deserializer
{
namespace Detail
{
template <typename ContainerType, typename EncodingType, typename AllocatorType>
auto FromJson(
	const rapidjson::GenericValue<EncodingType, AllocatorType>& json_value,
	ContainerType& container) ->
	typename TEnableIf<
		Traits::TreatAsArray<ContainerType>::Value>::Type;
	
template <typename ContainerType, typename EncodingType, typename AllocatorType>
auto FromJson(
	const rapidjson::GenericValue<EncodingType, AllocatorType>& json_value,
	ContainerType& container) ->
	typename TEnableIf<
		Traits::TreatAsObject<ContainerType>::Value>::Type;

template <typename DataType, typename EncodingType, typename AllocatorType>
auto FromJson(
	const rapidjson::GenericValue<EncodingType, AllocatorType>& json_value, DataType& data) ->
	typename TEnableIf<
	Traits::HasFromJson<DataType, EncodingType, AllocatorType>::Value>::Type;

template <typename DataType, typename EncodingType, typename AllocatorType>
auto FromJson(
	const rapidjson::GenericMember<EncodingType, AllocatorType>& member, DataType& data) ->
	typename TEnableIf<
	Traits::HasFromJson<DataType, EncodingType, AllocatorType>::Value>::Type;
	
} // namespace Detail
} // namespace Deserializer
} // namespace Json
