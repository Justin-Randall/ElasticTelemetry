// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "ElasticTelemetryEnvironmentSettings.h"
#include "Herald/LogLevels.hpp"
#include "Herald/ILogTransformer.hpp"

DECLARE_LOG_CATEGORY_EXTERN(TelemetryLog, Log, All);

class FElasticTelemetryOutputDevice;

/// <summary>
/// The ElasticTelemetry module is responsible for sending logs to an ElasticSearch server.
/// Configuration settings are stored in the ElasticTelemetrySettings struct, with 3 default
/// "Environment" configurations, each with their own settings. These are changed in the editor
/// under Project Settings -> Plugins -> Elastic Telemetry. See the included README.md for more
/// information on how to set up the ElasticSearch server and how to use the plugin.
/// </summary>
class ELASTICTELEMETRY_API FElasticTelemetryModule : public IModuleInterface
{
public:
	FElasticTelemetryModule();
	virtual ~FElasticTelemetryModule();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/// <summary>
	/// Get the active configuration settings for the module. Returns by value. Locks the settings to allow real-time changes to be made in the editor.
	/// </summary>
	/// <returns>Active configuration settings.</returns>
	FElasticTelemetrySettings GetSettings() const;

	/// <summary>
	/// The OutputDevice contains a log transformer that can be used to add headers to the log messages.
	/// </summary>
	/// <returns>ILogTransformerPtr with addHeader()/removeHeader()</returns>
	Herald::ILogTransformerPtr GetJsonTransformer() const;

protected:
	void UpdateConfig();

protected:
	// Since settings may be used by different threads, and because
	// in the editor, it would be nice to have them updated in real-time,
	// to test configurations, these are by-value and locked when accessed.
	mutable FCriticalSection  SettingsLock;
	FElasticTelemetrySettings Settings;

	// This is instantiated in StartupModule and deleted in ShutdownModule
	// It addes itself to the global log system via GLog->AddOutputDevice()
	// in its constructor. All UE_LOG() will invoke OutputDevice->Serialize()
	// which in turn will pass the log message to the JsonTransformer. Once the
	// transformer is done with the message, it will be passed to the ElasticWriter
	// which will queue it up for delivery to the ElasticSearch server.
	FElasticTelemetryOutputDevice* OutputDevice;
};
