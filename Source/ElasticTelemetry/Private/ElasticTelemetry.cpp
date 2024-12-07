// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "ElasticTelemetry.h"
#include "ElasticTelemetryEnvironmentSettings.h"
#include "HttpModule.h"
#include "Herald/LogLevels.hpp"
#include "ElasticTelemetryOutputDevice.h"
#include "ETLogger.h"

#define LOCTEXT_NAMESPACE "FElasticTelemetryModule"

DEFINE_LOG_CATEGORY(TelemetryLog);

FElasticTelemetryModule::FElasticTelemetryModule()
	: Settings(), OutputDevice(nullptr)
{
}

FElasticTelemetryModule::~FElasticTelemetryModule()
{
	if (OutputDevice)
	{
		delete OutputDevice;
	}
}

void FElasticTelemetryModule::StartupModule()
{
#if !UE_BUILD_SHIPPING
	UpdateConfig();

	// Already spawned from the editor most likely, which is
	// re-logging output.
	if (IsRunningCommandlet())
	{
		ShutdownModule();
		return;
	}

	OutputDevice = new FElasticTelemetryOutputDevice(*this);

	// ensure the http module is setup from this thread before attempting to use it elsewhere
	// otherwise, the http module may not be ready when the http logger tries to use it
	FHttpModule::Get();

	// Create a unique session ID guid for this session
	// This is used to group logs together in ElasticSearch
	// and can be used to filter logs by session
	const auto SessionID = FGuid::NewGuid().ToString();
	Herald::addHeader("SessionID", SessionID);

	const auto ComputerName = FPlatformProcess::ComputerName();
	if (nullptr != ComputerName)
	{
		Herald::addHeader("ComputerName", ComputerName);
	}

	#if UE_SERVER
	Herald::addHeader("UserName", "DedicatedServer");
	#else
	// This returns the locally logged in user, not the Steam/XBox/Sony Network user
	const auto UserName = FPlatformProcess::UserName();
	if (nullptr != UserName)
	{
		Herald::addHeader("UserName", UserName);
	}
	#endif // UE_SERVER
#endif	   //! UE_BUILD_SHIPPING
}

void FElasticTelemetryModule::UpdateConfig()
{
	const auto NewEnvSettings = GetDefault<UElasticTelemetryEnvironmentSettings>();
	if (nullptr == NewEnvSettings)
	{
		UE_LOG(TelemetryLog, Error, TEXT("Failed to get settings."));
		return;
	}
	FElasticTelemetrySettings NewSettings;
	{
		FScopeLock Lock(&SettingsLock);
		auto	   UpdatedSettings = NewEnvSettings->GetSettingsForEnvironment(NewEnvSettings->GetActiveEnvironment());
		if (nullptr == UpdatedSettings)
		{
			UE_LOG(TelemetryLog, Error, TEXT("Failed to get settings for environment %s."), *NewEnvSettings->GetActiveEnvironment());
			return;
		}

		// Make a local copy to work with as the settings are applied
		Settings = *UpdatedSettings;
	}

	// Apply the settings to the Herald log system
	if (!Settings.Enabled)
	{
		UE_LOG(TelemetryLog, Log, TEXT("Telemetry is disabled."));
		Herald::disableAllLogLevels();
	}
	else
	{
		Settings.EnableFatal ? Herald::enableLogLevel(Herald::LogLevels::Fatal) : Herald::disableLogLevel(Herald::LogLevels::Fatal);
		Settings.EnableError ? Herald::enableLogLevel(Herald::LogLevels::Error) : Herald::disableLogLevel(Herald::LogLevels::Error);
		Settings.EnableWarning ? Herald::enableLogLevel(Herald::LogLevels::Warning) : Herald::disableLogLevel(Herald::LogLevels::Warning);
		Settings.EnableDisplay ? Herald::enableLogLevel(Herald::LogLevels::Info) : Herald::disableLogLevel(Herald::LogLevels::Info);
		Settings.EnableLog ? Herald::enableLogLevel(Herald::LogLevels::Debug) : Herald::disableLogLevel(Herald::LogLevels::Debug);
		Settings.EnableVerbose ? Herald::enableLogLevel(Herald::LogLevels::Trace) : Herald::disableLogLevel(Herald::LogLevels::Trace);
		Settings.EnableVeryVerbose ? Herald::enableLogLevel(Herald::LogLevels::Analysis) : Herald::disableLogLevel(Herald::LogLevels::Analysis);

		// Hook up the Output device writer to the transformer

		UE_LOG(TelemetryLog, Log, TEXT("Telemetry is enabled."));
	}
}

FElasticTelemetrySettings FElasticTelemetryModule::GetSettings() const
{
	FScopeLock Lock(&SettingsLock);
	return Settings;
}

Herald::ILogTransformerPtr FElasticTelemetryModule::GetJsonTransformer() const
{
	// Get the Transformer from the OutputDevice
	if (!OutputDevice)
	{
		return nullptr;
	}

	return OutputDevice->GetJsonTransformer();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FElasticTelemetryModule, ElasticTelemetry)