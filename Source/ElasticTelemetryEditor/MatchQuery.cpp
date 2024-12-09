// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#if WITH_EDITOR

#include "MatchQuery.h"

FQueryTerm::FQueryTerm() {}

FQueryTerm::FQueryTerm(const std::string & Field, const std::string & Value)
    : Field(ANSI_TO_TCHAR(Field.c_str()))
    , Value(ANSI_TO_TCHAR(Value.c_str()))
{
}

FQueryTerm::FQueryTerm(const FString & Field, const FString & Value)
    : Field(Field)
    , Value(Value)
{
}

FQueryTerm::FQueryTerm(const char * Field, const char * Value)
    : Field(Field)
    , Value(Value)
{
}

FQueryRange::FQueryRange() {}

FQueryRange::FQueryRange(
    const std::string & Field, const std::string & GreaterThanOrEqualTo, const std::string & LessThanOrEqualTo)
    : Field(ANSI_TO_TCHAR(Field.c_str()))
    , Gte(ANSI_TO_TCHAR(GreaterThanOrEqualTo.c_str()))
    , Lte(ANSI_TO_TCHAR(LessThanOrEqualTo.c_str()))
{
}

FQueryRange::FQueryRange(const FString & Field, const FString & GreaterThanOrEqualTo, const FString & LessThanOrEqualTo)
    : Field(Field)
    , Gte(GreaterThanOrEqualTo)
    , Lte(LessThanOrEqualTo)
{
}

FQueryRange::FQueryRange(const char * Field, const char * GreaterThanOrEqualTo, const char * LessThanOrEqualTo)
    : Field(Field)
    , Gte(GreaterThanOrEqualTo)
    , Lte(LessThanOrEqualTo)
{
}

FQueryBool & FQueryBool::AndHasRangeFilter(const FQueryRange & FilterRange)
{
	FilterRanges.Add(FilterRange);
	return *this;
}

FQueryBool & FQueryBool::AndHasTermFilter(const FQueryTerm & FilterTerm)
{
	FilterTerms.Add(FilterTerm);
	return *this;
}

FQueryBool & FQuery::WithBoolQuery() { return BoolQuery; }

FSearch & FSearch::SetResultSize(size_t Size)
{
	ResultSize = Size;
	return *this;
}

FQueryBool & FSearch::WithBoolQuery() { return Query.WithBoolQuery(); }

#endif // WITH_EDITOR
