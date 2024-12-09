// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#pragma once

#if WITH_EDITOR

/// <summary>
/// IElasticQueryClient is an interface class defining methods to
/// access ElasticSearch or to derive specific Mock implementations to
/// test ElasticSearch queries in automated tests without requiring a
/// network connection or particular data be available in ElasticSearch
/// while tests execute.
/// </summary>
class ELASTICTELEMETRYEDITOR_API IElasticQueryClient
{
  public:
	IElasticQueryClient(){};
	virtual ~IElasticQueryClient(){};

	/// <summary>
	/// QueryIndex
	///
	/// This will query a specific index defined in ElasticSearch
	/// (for example Game-locationevents).
	///
	/// Any valid ElasticSearch DSL json query can be used.
	/// See: https://www.elastic.co/guide/en/elasticsearch/reference/current/query-dsl.html
	///
	/// For a *real* http connection, this will likely be an asynchronous
	/// call, whereas a test mock will likely trigger the
	/// OnRequestComplete callback during the QueryIndex invocation.
	/// </summary>
	/// <param name="IndexName">
	///		This should be the name of the ElasticSearch index to search.
	/// </param>
	/// <param name="JsonQuery">
	///		Query DSL json string to pass to ElasticSearch
	/// </param>
	/// <param name="OnRequestComplete">
	///		Callback to trigger when procesing the request completes.
	/// </param>
	virtual void QueryIndex(
	    const FString & JsonQuery, TFunction<void(bool, const FJsonObject &)> OnRequestComplete) const = 0;
};
#endif // WITH_EDITOR
