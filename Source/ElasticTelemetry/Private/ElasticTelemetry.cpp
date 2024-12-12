// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "ElasticTelemetry.h"
#include "ETLogger.h"
#include "ElasticTelemetryEnvironmentSettings.h"
#include "ElasticTelemetryOutputDevice.h"
#include "ElasticTelemetryWriter.h"
#include "FileNameFriendly.h"
#include "Herald/JsonLogTransformerFactory.hpp"
#include "Herald/LogLevels.hpp"
#include "HttpModule.h"

#define LOCTEXT_NAMESPACE "FElasticTelemetryModule"

DEFINE_LOG_CATEGORY(TelemetryLog);

FElasticTelemetryModule::FElasticTelemetryModule()
    : Settings()
    , EventSettings()
    , OutputDevice(nullptr)
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
	// Create the event writer chain. It will be configured during UpdateConfig()
	auto EventWriterBuilder = createElasticTelemetryWriterBuilder();
	if (nullptr == EventWriterBuilder)
	{
		UE_LOG(TelemetryLog, Error, TEXT("Failed to create log writer builder."));
		return;
	}
	EventWriter = EventWriterBuilder->build();

	auto LogFactory = Herald::createJsonLogTransformerBuilder();
	if (nullptr == LogFactory)
	{
		UE_LOG(TelemetryLog, Error, TEXT("Failed to create log transformer factory."));
		return;
	}
	EventTransformer = LogFactory->attachLogWriter(EventWriter).build();

	// Already spawned from the editor most likely, which is
	// re-logging output.
	if (IsRunningCommandlet())
	{
		ShutdownModule();
		return;
	}

	OutputDevice = new FElasticTelemetryOutputDevice(*this);
	if (nullptr == OutputDevice)
	{
		UE_LOG(TelemetryLog, Error, TEXT("Failed to create output device."));
		return;
	}

	UpdateConfig();

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
#endif //! UE_BUILD_SHIPPING
}

void FElasticTelemetryModule::UpdateConfig()
{
	if (nullptr == OutputDevice)
	{
		UE_LOG(TelemetryLog, Display, TEXT("OutputDevice is null. This is normal when cooking."));
		return;
	}

	const auto NewEnvSettings = GetDefault<UElasticTelemetryEnvironmentSettings>();
	if (nullptr == NewEnvSettings)
	{
		UE_LOG(TelemetryLog, Error, TEXT("Failed to get settings."));
		return;
	}

	const auto NewEventSettings = GetDefault<UElasticTelemetryQueryConfig>();
	if (nullptr == NewEventSettings)
	{
		UE_LOG(TelemetryLog, Error, TEXT("Failed to get event settings."));
		return;
	}

	{
		FScopeLock Lock(&SettingsLock);
		auto       UpdatedSettings = NewEnvSettings->GetSettingsForEnvironment(NewEnvSettings->GetActiveEnvironment());
		if (nullptr == UpdatedSettings)
		{
			UE_LOG(TelemetryLog, Error, TEXT("Failed to get settings for environment %s."),
			    *NewEnvSettings->GetActiveEnvironment());
			return;
		}

		// Make a local copy to work with as the settings are applied
		Settings      = *UpdatedSettings;
		EventSettings = NewEventSettings->GetQuerySettings();
	}

	// Apply the settings to the Herald log system
	// TODO: This is not dynamically updating the log writer endpoint configuration!
	//   This should be done via ElasticTelemetryWriter and thread safe!
	if (!Settings.Enabled)
	{
		UE_LOG(TelemetryLog, Log, TEXT("Telemetry is disabled."));
		Herald::disableAllLogLevels();
	}
	else
	{
		Settings.EnableFatal ? Herald::enableLogLevel(Herald::LogLevels::Fatal)
		                     : Herald::disableLogLevel(Herald::LogLevels::Fatal);
		Settings.EnableError ? Herald::enableLogLevel(Herald::LogLevels::Error)
		                     : Herald::disableLogLevel(Herald::LogLevels::Error);
		Settings.EnableWarning ? Herald::enableLogLevel(Herald::LogLevels::Warning)
		                       : Herald::disableLogLevel(Herald::LogLevels::Warning);
		Settings.EnableDisplay ? Herald::enableLogLevel(Herald::LogLevels::Info)
		                       : Herald::disableLogLevel(Herald::LogLevels::Info);
		Settings.EnableLog ? Herald::enableLogLevel(Herald::LogLevels::Debug)
		                   : Herald::disableLogLevel(Herald::LogLevels::Debug);
		Settings.EnableVerbose ? Herald::enableLogLevel(Herald::LogLevels::Trace)
		                       : Herald::disableLogLevel(Herald::LogLevels::Trace);
		Settings.EnableVeryVerbose ? Herald::enableLogLevel(Herald::LogLevels::Analysis)
		                           : Herald::disableLogLevel(Herald::LogLevels::Analysis);

		// Hook up the Output device writer to the transformer

		UE_LOG(TelemetryLog, Log, TEXT("Telemetry is enabled."));
	}

	const std::string IndexName      = TCHAR_TO_UTF8(*FileNameFriendly(Settings.IndexName));
	std::string       EventIndexName = TCHAR_TO_UTF8(*FileNameFriendly(Settings.EventIndexName));
	const std::string EndpointURL    = TCHAR_TO_UTF8(*Settings.EndpointURL);
	const std::string Username       = TCHAR_TO_UTF8(*Settings.Username);
	const std::string Password       = TCHAR_TO_UTF8(*Settings.Password);

	// Apply settings to ElasticWriter
	auto ElasticWriter = OutputDevice->GetElasticWriter();
	if (nullptr == ElasticWriter)
	{
		UE_LOG(TelemetryLog, Error, TEXT("ElasticWriter is null."));
	}
	else
	{
		ElasticWriter->addConfigPair("EndpointURL", EndpointURL);
		ElasticWriter->addConfigPair("Username", Username);
		ElasticWriter->addConfigPair("Password", Password);
		ElasticWriter->addConfigPair("IndexName", IndexName);
	}

	// Apply settings to the event writer
	if (nullptr == EventWriter)
	{
		UE_LOG(TelemetryLog, Error, TEXT("Event writer is null."));
		return;
	}

	// For events, uses the same log endpoint, user and password. Uses EventIndexName for queries
	EventWriter->addConfigPair("EndpointURL", EndpointURL);
	EventWriter->addConfigPair("Username", Username);
	EventWriter->addConfigPair("Password", Password);
	EventWriter->addConfigPair("IndexName", EventIndexName);
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