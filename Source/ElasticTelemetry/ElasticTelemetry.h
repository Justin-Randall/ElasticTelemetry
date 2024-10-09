// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "ElasticTelemetryEnvironmentSettings.h"
#include "Herald/LogLevels.hpp"
#include "Herald/ILogTransformer.hpp"

DECLARE_LOG_CATEGORY_EXTERN(TelemetryLog, Log, All);

class FElasticTelemetryOutputDevice;

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
	mutable FCriticalSection	   SettingsLock;
	FElasticTelemetrySettings	   Settings;
	FElasticTelemetryOutputDevice* OutputDevice;
};
