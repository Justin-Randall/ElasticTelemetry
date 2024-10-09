// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "ElasticTelemetryEnvironmentSettings.h"
#include "Herald/LogLevels.hpp"

DECLARE_LOG_CATEGORY_EXTERN(TelemetryLog, Log, All);

class FElasticTelemetryOutputDevice;
namespace Herald
{
	void log(LogLevels LogLevel, const FString& Message);
} // namespace Herald

class FElasticTelemetryModule : public IModuleInterface
{
public:
	FElasticTelemetryModule();
	virtual ~FElasticTelemetryModule();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// Return by value after locking and making a copy. In editor environments, settings may be updated real-time.
	FElasticTelemetrySettings GetSettings() const;

	FElasticTelemetryOutputDevice* GetOutputDevice() const
	{
		return OutputDevice;
	}

protected:
	void UpdateConfig();

protected:
	friend void					   Herald::log(LogLevels LogLevel, const FString& Message);
	mutable FCriticalSection	   SettingsLock;
	FElasticTelemetrySettings	   Settings;
	FElasticTelemetryOutputDevice* OutputDevice;
};
