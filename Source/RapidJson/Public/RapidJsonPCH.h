#pragma once

#ifndef THIRD_PARTY_INCLUDES_START
#	define THIRD_PARTY_INCLUDES_START
#	define THIRD_PARTY_INCLUDES_END
#endif

#define BETTER_ENUMS_CONSTEXPR_TO_STRING
#ifndef BETTER_ENUMS_DEFAULT_CONSTRUCTOR
#define BETTER_ENUMS_DEFAULT_CONSTRUCTOR(Enum)                                 \
public:                                                                        \
    Enum() = default;
#endif

THIRD_PARTY_INCLUDES_START
#include "rapidjson/encodings.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "better_enums/enum.h"
THIRD_PARTY_INCLUDES_END

#define EXPAND(x) x
#define FOR_EACH_1(what, x, ...) what(x)
#define FOR_EACH_2(what, x, ...)\
  what(x);\
  EXPAND(FOR_EACH_1(what,  __VA_ARGS__))
#define FOR_EACH_3(what, x, ...)\
  what(x);\
  EXPAND(FOR_EACH_2(what, __VA_ARGS__))
#define FOR_EACH_4(what, x, ...)\
  what(x);\
  EXPAND(FOR_EACH_3(what,  __VA_ARGS__))
#define FOR_EACH_5(what, x, ...)\
  what(x);\
  EXPAND(FOR_EACH_4(what,  __VA_ARGS__))
#define FOR_EACH_6(what, x, ...)\
  what(x);\
  EXPAND(FOR_EACH_5(what,  __VA_ARGS__))
#define FOR_EACH_7(what, x, ...)\
  what(x);\
  EXPAND(FOR_EACH_6(what,  __VA_ARGS__))
#define FOR_EACH_8(what, x, ...)\
  what(x);\
  EXPAND(FOR_EACH_7(what,  __VA_ARGS__))
#define FOR_EACH_NARG(...) FOR_EACH_NARG_(__VA_ARGS__, FOR_EACH_RSEQ_N())
#define FOR_EACH_NARG_(...) EXPAND(FOR_EACH_ARG_N(__VA_ARGS__))
#define FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define FOR_EACH_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0
#define CONCATENATE(x,y) x##y
#define FOR_EACH_(N, what, ...) EXPAND(CONCATENATE(FOR_EACH_, N)(what, __VA_ARGS__))
#define FOR_EACH(what, ...) FOR_EACH_(FOR_EACH_NARG(__VA_ARGS__), what, __VA_ARGS__)
#define STRINGIFY_(X) #X
#define STRINGIFY(X) STRINGIFY_(X)

#define JSON_INNER_WRITE(X) writer.Key(STRINGIFY(X)); Json::Serializer::ToJson(writer, X);
#define JSON_INNER_READ(X) \
member_iterator = document.FindMember(TEXT(STRINGIFY(X))); \
if (member_iterator == document.MemberEnd()) \
{ \
	UE_LOG(LogRapidJson, Error, TEXT("not found member %s"), #X); \
	return; \
} \
Json::Deserializer::FromJson(member_iterator->value, X);

#define JSON_DEFINE(...) \
template <typename WriterType> \
void ToJson(WriterType& writer) const \
{ \
	writer.StartObject(); \
	FOR_EACH(JSON_INNER_WRITE, __VA_ARGS__) \
	writer.EndObject(); \
} \
template <typename EncodingType, typename AllocatorType> \
void FromJson(const rapidjson::GenericValue<EncodingType, AllocatorType>& document) \
{ \
	rapidjson::GenericValue<EncodingType, AllocatorType>::ConstMemberIterator member_iterator; \
	FOR_EACH(JSON_INNER_READ, __VA_ARGS__) \
} \

#define JSON_EXTERNAL_WRITE(X) writer.Key(STRINGIFY(X)); Json::Serializer::ToJson(writer, data.X);
#define JSON_EXTERNAL_READ(X) \
member_iterator = document.FindMember(TEXT(STRINGIFY(X))); \
if (member_iterator == document.MemberEnd()) \
{ \
	UE_LOG(LogRapidJson, Error, TEXT("not found member %s"), #X); \
	return; \
} \
Json::Deserializer::FromJson(member_iterator->value, data.X);

#define JSON_EXTERNAL_DEFINE(DataType, ...) \
template <typename Writer> \
void ToJson(Writer & writer, const DataType& data) \
{ \
	writer.StartObject(); \
	FOR_EACH(JSON_EXTERNAL_WRITE, __VA_ARGS__) \
	writer.EndObject(); \
} \
template <typename EncodingType, typename AllocatorType> \
void FromJson(const rapidjson::GenericValue<EncodingType, AllocatorType>& document, DataType& data) \
{ \
	rapidjson::GenericValue<EncodingType, AllocatorType>::ConstMemberIterator member_iterator; \
	FOR_EACH(JSON_EXTERNAL_READ, __VA_ARGS__) \
}

#define JSON_ADD_INT_ENUM(ENUM) \
template <typename Writer> \
void ToJson(Writer & writer, const ENUM& o) \
{ \
	Json::Serializer::ToJson(writer, static_cast<std::underlying_type_t<ENUM>>(o)); \
} \
template <typename EncodingType, typename AllocatorType> \
void FromJson(const rapidjson::GenericValue<EncodingType, AllocatorType>& document, ENUM& o) \
{ \
	o = static_cast<ENUM>(document.GetInt()); \
}

#define JSON_MAKE_ENUM(ENUM, Underlying, ...) \
BETTER_ENUM(ENUM, Underlying, __VA_ARGS__) \
template <typename Writer> \
void ToJson(Writer & writer, const ENUM& o) \
{ \
	Json::Serializer::ToJson(writer, o._to_string()); \
} \
template <typename EncodingType, typename AllocatorType> \
void FromJson(const rapidjson::GenericValue<EncodingType, AllocatorType>& document, ENUM& o) \
{ \
	o = ENUM::_from_string(TCHAR_TO_UTF8(document.GetString())); \
}
