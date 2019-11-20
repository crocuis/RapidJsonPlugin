# RapidJsonPlugin

## RapidJson Plugin for UnrealEngine 4

[RapidJson](https://github.com/Tencent/rapidjson) is A fast JSON parser for C++. The purpose of this plug-in is to make RapidJson easy and simple to use, inspired by [msgpack-c](https://github.com/msgpack/msgpack-c). This was implemented using template and MACRO, using [better-enums](https://github.com/aantron/better-enums) to help stringify enum.

## Example

```c++

JSON_MAKE_ENUM(Word, int, Hello, World);
JSON_EXTERNAL_DEFINE(FVector, X, Y, Z);

struct Sample_member_struct
{
    TArray<int32> Arr;
	TArray<FVector> Arr2;
	Word Word;

	JSON_DEFINE(Arr, Arr2, Word);
};

struct Sample_struct
{
	int32 TestInt;
	FString TestStr;
	sample_member_struct My_struct;

	JSON_DEFINE(TestInt, TestStr, My_struct)
};

Sample_member_struct member;
member.Arr = { 1,2,3,4,5 };
member.Arr2 = { FVector(1,2,3), FVector(2,3,4) };
member.Word = Word::World;
Sample_struct sample = { 1, TEXT("HELLO"), member };

auto json = Json::SerializeToJson(sample);
auto sample2 = Json::DeserializeFromJson<Sample_struct>(json);

sample == sample2

```
