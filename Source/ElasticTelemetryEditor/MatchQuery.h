// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#pragma once
#if WITH_EDITOR

#include "CoreMinimal.h"
#include <map>
#include <string>
#include "StringConversions.h"
#include "rapidjsoncpp/to_json.hpp"

#include "MatchQuery.generated.h"

/// <summary>
/// MatchQuery.h - A simple query builder for ElasticSearch queries
/// </summary>
/// <remarks>
/// Use of the C++ standard library is intentional to ensure queries also match
/// the expected types in the Herald logging library, which also sends events to
/// ElasticSearch.
///
/// Conversions from Unreal types to standard library types are provided to help
/// keep UnrealEngine code consistent and not force standard library types to bleed
/// into game code.
/// </remarks>

USTRUCT(BlueprintType)
struct ELASTICTELEMETRYEDITOR_API FQueryTerm
{
	GENERATED_BODY()

	FQueryTerm();
	FQueryTerm(const std::string & Field, const std::string & Value);
	FQueryTerm(const FString & Field, const FString & Value);
	FQueryTerm(const char * Field, const char * Value); // disambiguate conversions when using string literals

	UPROPERTY(
	    EditAnywhere, BlueprintReadWrite, Category = "ElasticQuery", DisplayName = "Name of any field in an event")
	FString Field;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ElasticQuery", DisplayName = "Value to match in the term")
	FString Value;
};

USTRUCT(BlueprintType)
struct ELASTICTELEMETRYEDITOR_API FQueryRange
{
	GENERATED_BODY()

	FQueryRange();
	FQueryRange(
	    const std::string & Field, const std::string & GreaterThanOrEqualTo, const std::string & LessThanOrEqualTo);
	FQueryRange(const FString & Field, const FString & GreaterThanOrEqualTo, const FString & LessThanOrEqualTo);
	FQueryRange(const char * Field, const char * GreaterThanOrEqualTo,
	    const char * LessThanOrEqualTo); // disambiguate conversions when using string literals

	UPROPERTY(
	    EditAnywhere, BlueprintReadWrite, Category = "ElasticQuery", DisplayName = "Name of any field in an event")
	FString Field;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ElasticQuery",
	    DisplayName = "Greater than or equal to (optional)")
	FString Gte; // Greater than or equal to

	UPROPERTY(
	    EditAnywhere, BlueprintReadWrite, Category = "ElasticQuery", DisplayName = "Less than or equal to (optional)")
	FString Lte; // Less than or equal to
};

USTRUCT(BlueprintType)
struct ELASTICTELEMETRYEDITOR_API FQueryBool
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ElasticQuery", DisplayName = "Filter terms")
	TArray<FQueryTerm> FilterTerms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ElasticQuery", DisplayName = "Filter ranges")
	TArray<FQueryRange> FilterRanges;

	FQueryBool & AndHasTermFilter(const FQueryTerm & FilterTerm);
	FQueryBool & AndHasRangeFilter(const FQueryRange & FilterRange);
};

USTRUCT(BlueprintType)
struct ELASTICTELEMETRYEDITOR_API FQuery
{
	GENERATED_BODY()

	FQueryBool BoolQuery;

	FQueryBool & WithBoolQuery();
};

USTRUCT(BlueprintType)
struct ELASTICTELEMETRYEDITOR_API FSearch
{
	GENERATED_BODY()

	/// <summary>
	/// Maximuim number of results to return in this query
	/// </summary>
	size_t ResultSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ElasticQuery", DisplayName = "Query")
	FQuery Query;

	FSearch &    SetResultSize(size_t Size);
	FQueryBool & WithBoolQuery();
};

namespace rapidjson
{
	template <typename WriterType>
	void write(WriterType & w, const FQueryTerm & value)
	{
		w.StartObject();
		{
			w.Key("term");
			w.StartObject();
			{
				write(w, std::to_string(value.Field), std::to_string(value.Value));
			}
			w.EndObject();
		}
		w.EndObject();
	}

	template <typename WriterType>
	void write(WriterType & w, const FQueryRange & value)
	{
		w.StartObject();
		{
			w.Key("range");
			w.StartObject();
			{
				std::string field = std::to_string(value.Field);
				w.Key(field.c_str());
				w.StartObject();
				{
					if (!value.Gte.IsEmpty())
					{
						w.Key("gte");
						write(w, std::to_string(value.Gte));
					}
					if (!value.Lte.IsEmpty())
					{
						w.Key("lte");
						write(w, std::to_string(value.Lte));
					}
				}
				w.EndObject();
			}
			w.EndObject();
		}
		w.EndObject();
	}

	template <typename WriterType>
	void write(WriterType & w, const FQueryBool & value)
	{
		w.StartObject();
		{
			w.Key("filter");
			w.StartArray();
			{
				if (!value.FilterTerms.IsEmpty())
				{
					for (const auto & term : value.FilterTerms)
					{
						write(w, term);
					}
				}

				if (!value.FilterRanges.IsEmpty())
				{
					for (const auto & range : value.FilterRanges)
					{
						write(w, range);
					}
				}
			}
			w.EndArray();
		}
		w.EndObject();
	}

	template <typename WriterType>
	void write(WriterType & w, const FQuery & value)
	{
		w.StartObject();
		{
			w.Key("bool");
			write(w, value.BoolQuery);
		}
		w.EndObject();
	}

	template <typename WriterType>
	void write(WriterType & w, const FSearch & value)
	{
		w.StartObject();
		{
			write(w, "size", value.ResultSize);
			w.Key("query");
			write(w, value.Query);
		}
		w.EndObject();
	}
} // namespace rapidjson

#endif // WITH_EDITOR
