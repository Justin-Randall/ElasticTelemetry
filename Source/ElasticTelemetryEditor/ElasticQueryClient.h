// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#if WITH_EDITOR

#pragma once

#include "IElasticQueryClient.h"

class FElasticTelemetryModule;

/// <summary>
/// An implementation of the TelemetryQueryHttpClient interface that
/// uses Unreal's own FHttpModule and FHttpRequest types to send an
/// asynchronous request to ElasticSearch, returning results through
/// the supplied callback as an FJsonObject.
/// </summary>
class ELASTICTELEMETRYEDITOR_API FElasticQueryClient : public IElasticQueryClient
{
  public:
	FElasticQueryClient();
	~FElasticQueryClient();

	/// <summary>
	/// QueryIndex
	///
	/// This will query a specific index defined in ElasticSearch
	/// (for example Game-locationevents).
	///
	/// Any valid ElasticSearch DSL json query can be used.
	/// See: https://www.elastic.co/guide/en/elasticsearch/reference/current/query-dsl.html
	///
	/// This is an asynchronous call. One or more frames will pass
	/// before the callback is triggered. Please be aware of object
	/// lifetimes in context of the callback.
	///
	/// *IMPORTANT*
	///		The editor user MUST set their ElasticSearch user name and
	///		password. In the editor, navigate to: Edit -> Project Settings.
	///		Within the Project Settings UI, find
	///		"Editor Telemetry Query User Settings" the set
	///		Elastic Stack Query User Name and Elastic Stack Query Password.
	/// </summary>
	/// <param name="IndexName">
	///		This should be the name of the ElasticSearch index to search.
	/// </param>
	/// <param name="JsonQuery">
	///		Query DSL json test to pass to ElasticSearch
	///		See: https://www.elastic.co/guide/en/elasticsearch/reference/current/query-dsl.html
	/// </param>
	/// <param name="OnRequestComplete">
	///		Callback to trigger when procesing the request completes.
	/// </param>
	virtual void QueryIndex(
	    const FString & JsonQuery, TFunction<void(bool, const FJsonObject &)> OnRequestComplete) const;
};

#endif // WITH_EDITOR
