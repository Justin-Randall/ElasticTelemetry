#include "ElasticTelemetryOutputDevice.h"
#include "ElasticTelemetry.h"
#include "ElasticTelemetrySettings.h"
#include "ElasticTelemetryEnvironmentSettings.h"
#include "Herald/LogLevels.hpp"
#include "Herald/Logger.hpp"
#include "Herald/JsonLogTransformerFactory.hpp"
#include "ElasticTelemetryWriter.h"
// #include "Herald/Logger.hpp"
#include "Herald/LogEntry.hpp"
#include <string>
#include "StringConversions.h"

namespace
{
	std::string LogVerbosityToString(ELogVerbosity::Type Verbosity)
	{
		switch (Verbosity)
		{
			case ELogVerbosity::NoLogging:
				return "NoLogging";
			case ELogVerbosity::Fatal:
				return "Fatal";
			case ELogVerbosity::Error:
				return "Error";
			case ELogVerbosity::Warning:
				return "Warning";
			case ELogVerbosity::Display:
				return "Display";
			case ELogVerbosity::Log:
				return "Log";
			case ELogVerbosity::Verbose:
				return "Verbose";
			case ELogVerbosity::VeryVerbose:
				return "VeryVerbose";
			default:
				return "Unknown";
		}
	}
} // namespace

/// <summary>
/// FileNameFriendly
///
/// The index name is used to programmatically instance new indices
/// in ElasticSearch. Internally, these names are used as filenames,
/// so only characters that are valid for filenames should be used.
///
/// </summary>
/// <param name="IndexName"></param>
/// <returns>
/// A normalized, lower-case FString suitable for sending to
/// ElasticSearch for use as an index.
/// </returns>
FString FileNameFriendly(const FString& IndexName)
{
	FString Result = IndexName.ToLower();
	for (TCHAR& i : Result)
	{
		if ((i >= 'a' && i <= 'z') || (i >= '0' && i <= '9') || (i == '-') || (i == '_') || (i == '.'))
			continue;
		i = '_';
	}
	return Result;
}

FElasticTelemetryOutputDevice::FElasticTelemetryOutputDevice(const FElasticTelemetryModule& Module)
	: ProcessingRequestLock()
	, ProcessingRequestCount(0)
	, JsonTransformer(nullptr)
	, ElasticWriter(nullptr)
	, ElasticTelemetry(Module)
{
	if (!GLog)
		return;

	// auto shared = std::make_shared<FElasticTelemetryOutputDevice>(*this);
	// Herald::ILogWriterPtr wptr(this);
	// JsonTransformer = LogFactory->attachLogWriter(wptr).build();

	// The writer is responsible for queueing up messages from the transformer and delivering them to the ElasticSearch server
	// Give it the config pairs from the settings
	auto WriterBuilder = createElasticTelemetryWriterBuilder();
	if (nullptr == WriterBuilder)
	{
		UE_LOG(TelemetryLog, Error, TEXT("Failed to create log writer builder."));
		return;
	}

	// get the active settings to feed to the builder
	// auto Settings = GetDefault<UElasticTelemetryEnvironmentSettings>();
	auto ActiveSettings = ElasticTelemetry.GetSettings();

	// add the config pairs to the writer
	std::string IndexName = TCHAR_TO_UTF8(*FileNameFriendly(ActiveSettings.IndexName));
	std::string EndpointURL = TCHAR_TO_UTF8(*ActiveSettings.EndpointURL);
	std::string Username = TCHAR_TO_UTF8(*ActiveSettings.Username);
	std::string Password = TCHAR_TO_UTF8(*ActiveSettings.Password);

	ElasticWriter = WriterBuilder->addConfigPair(
									 "IndexName", IndexName)
						.addConfigPair("EndpointURL", EndpointURL)
						.addConfigPair("Username", Username)
						.addConfigPair("Password", Password)
						.build();

	// Create the log transformer
	auto LogFactory = Herald::createJsonLogTransformerBuilder();
	if (nullptr == LogFactory)
	{
		UE_LOG(TelemetryLog, Error, TEXT("Failed to create log transformer factory."));
		return;
	}

	JsonTransformer = LogFactory->attachLogWriter(ElasticWriter).build();
	GLog->AddOutputDevice(this); // do this last, don't want log events arriving before the transformer/writer chain is in place
}

FElasticTelemetryOutputDevice::~FElasticTelemetryOutputDevice()
{
	if (GLog)
		GLog->RemoveOutputDevice(this);
}

void FElasticTelemetryOutputDevice::Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category)
{
	const std::string CategoryName = TCHAR_TO_UTF8(*(Category.GetPlainNameString()));

	// Don't log about self logging about self logging about self logging ...
	if (CategoryName == "LogOutputDevice")
		return;

	// early out opportunities
	auto ActiveSettings = ElasticTelemetry.GetSettings();

	const std::string Msg(TCHAR_TO_UTF8(Message));
	bool			  PrintCallStack = false;

	for (const auto excludedCategory : ActiveSettings.ExcludedLogCategories)
	{
		if (excludedCategory == Category)
			return;
	}

	// map Unreal log verbosity type to Herald::LogTypes because it is not level based but a bitmask
	auto LType = Herald::LogLevels::Analysis;

	switch (Verbosity)
	{
		case ELogVerbosity::Fatal:
			LType = Herald::LogLevels::Fatal;
			PrintCallStack = ActiveSettings.IncludeCallstacksOnFatal;
			break;
		case ELogVerbosity::Error:
			LType = Herald::LogLevels::Error;
			PrintCallStack = ActiveSettings.IncludeCallstacksOnError;
			break;
		case ELogVerbosity::Warning:
			LType = Herald::LogLevels::Warning;
			PrintCallStack = ActiveSettings.IncludeCallstacksOnWarning;
			break;
		case ELogVerbosity::Display:
			LType = Herald::LogLevels::Info;
			PrintCallStack = ActiveSettings.IncludeCallstacksOnDisplay;
			break;
		case ELogVerbosity::Log:
			LType = Herald::LogLevels::Debug;
			PrintCallStack = ActiveSettings.IncludeCallstacksOnLog;
			break;
		case ELogVerbosity::Verbose:
			LType = Herald::LogLevels::Trace;
			PrintCallStack = ActiveSettings.IncludeCallstacksOnVerbose;
			break;
		case ELogVerbosity::VeryVerbose:
			LType = Herald::LogLevels::Analysis;
			PrintCallStack = ActiveSettings.IncludeCallstacksOnVeryVerbose;
			break;
		default:
			break;
	}
	std::string VerbosityString = LogVerbosityToString(Verbosity);

	static const std::string UELogKey("UE_LOG");
	static const std::string CategoryKey("Category");
	static const std::string MessageKey("MESSAGE");
	static const std::string VerbosityKey("Verbosity");
	// IF the editor is in use AND IF an ensure is being triggered AND IF a debugger is present AND IF telemetry is configured to
	// include callstacks for the requested error type THEN an unfornate string of events is triggered in UE4 internal macro
	// processing and call stack generation that creates a deadlock. So, if you are running the editor under a debugger and have
	// call stacks configured for this error level, do not expect them to show up.
#if WITH_EDITOR
	if (PrintCallStack && FPlatformMisc::IsDebuggerPresent())
		PrintCallStack = false; // dirty hack to get around bad behavior with internals in UE4 ensure macros
#endif

	if (PrintCallStack)
	{
		constexpr SIZE_T HumanReadableStringSize = 32792;
		ANSICHAR		 HumanReadableString[HumanReadableStringSize];
		const int32		 IgnoreCount = 2;

		FGenericPlatformStackWalk::StackWalkAndDump(HumanReadableString, HumanReadableStringSize, IgnoreCount, nullptr);

		static const std::string CallStackKey("CallStack");
		const std::string		 CallStack(HumanReadableString);
		Herald::log(*JsonTransformer, LType, Msg, CategoryKey, CategoryName, VerbosityKey, VerbosityString, CallStackKey, CallStack);
	}
	else
	{
		Herald::log(*JsonTransformer, LType, Msg, CategoryKey, CategoryName, VerbosityKey, VerbosityString);
	}
}