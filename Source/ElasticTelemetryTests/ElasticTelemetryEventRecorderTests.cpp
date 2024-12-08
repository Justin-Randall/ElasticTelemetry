// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "StringConversions.h"
#include "Misc/AutomationTest.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "ElasticTelemetry.h"
#include "ElasticTelemetryEditorModule.h"
#include "Herald/Logger.hpp"
#include "MatchQuery.h"
#include "ETLogger.h"
#include "Herald/ILogWriter.hpp"
#include "Herald/WriterBuilder.hpp"
#include "Herald/JsonLogTransformerFactory.hpp"
#include "Herald/LogLevels.hpp"
#include "Herald/BaseLogTransformer.hpp"
#include "Herald/GetTimeStamp.hpp"
#include "Herald/TransformerBuilder.hpp"
#include "rapidjsoncpp/to_json.hpp"
#include "rapidjsoncpp/to_json_map.hpp"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FElasticTelemetryEventRecorderTest, "ElasticTelemetry.EventRecorder.SendEvents",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

class MockWriter : public Herald::ILogWriter
{
  public:
	virtual ILogWriter & addConfigPair(const std::string & key, const std::string & value) override { return *this; }
	virtual void         write(const std::string & msg) override { LastMessage = msg; }
	std::string          LastMessage;
};

namespace rapidjson
{
	// add a writer for LogEntry
	template <typename WriterType>
	void write(WriterType & w, const Herald::LogEntry & value)
	{
		w.StartObject();
		// w.Key("level");
		// w.String(Herald::logTypeNames.at(value.logLevel).c_str());
		w.Key("type");
		w.String(value.message.c_str());
		for (const auto & [k, v] : value.metadata)
		{
			w.Key(k.c_str());
			w.String(v.c_str());
		}
		w.EndObject();
	}
} // namespace rapidjson

// TODO : testing here, move this to the main module once it works
class JsonEventTransformer : public Herald::BaseLogTransformer
{
  public:
	JsonEventTransformer()          = default;
	virtual ~JsonEventTransformer() = default; // LCOV_EXCL_LINE

	virtual void log(const Herald::LogEntry & entry) override
	{
		// use json to log the message
		const std::string timeStamp = Herald::getTimeStamp();

		std::string json = rapidjson::to_json("event", entry, "headers", headers, "timestamp", timeStamp);

		// ship it to the callbacks
		for (const auto & callback : callbacks)
		{
			callback(json);
		}

		// ship it to the writers
		for (const auto & writer : writers)
		{
			if (auto w = writer.lock())
			{
				if (w)
					w->write(json);
			}
		}
	}

  private:
};

bool FElasticTelemetryEventRecorderTest::RunTest(const FString & Parameters)
{
	// build a writer, then feed it to a JsonTransformer to then inject into the module for testing
	auto writer      = Herald::createWriterBuilder<MockWriter>()->build();
	auto transformer = Herald::createJsonLogTransformerBuilder()->attachLogWriter(writer).build();

	transformer->addHeader("map", "neko_uplink_v2");
	transformer->addHeader("game_mode", "uplink_v2");
	transformer->addHeader("user", "neko");
	transformer->addHeader("machine_name", "schroedinger");
	FVector vec(1, 2, 3);

	// Dependency injection: use this transformer for testing
	Herald::event(*transformer, "PlayerDeath", "Location", vec);

	// verify the last message in the writer matches the expected message
	// no need to verify the time stamp, but the headers and location do need verifying.
	// Look for containing string matches for map, event name, location key, game mode, etc...
	// the actual values are not important, just that they are present in the message
	const std::string & lastMessage = static_cast<MockWriter *>(writer.get())->LastMessage;

	// Use  test macros to check for pass or fail in the last message string
	TestTrue("map key exists", lastMessage.find("map") != std::string::npos);
	TestTrue("map value exists", lastMessage.find("neko_uplink_v2") != std::string::npos);
	TestTrue("game_mode key exists`", lastMessage.find("game_mode") != std::string::npos);
	TestTrue("game_mode value", lastMessage.find("uplink_v2") != std::string::npos);
	TestTrue("user key exists", lastMessage.find("user") != std::string::npos);
	TestTrue("user value exists", lastMessage.find("neko") != std::string::npos);
	TestTrue("machine_name key exists", lastMessage.find("machine_name") != std::string::npos);
	TestTrue("machine_name value exists", lastMessage.find("schroedinger") != std::string::npos);
	TestTrue("Location key exists", lastMessage.find("Location") != std::string::npos);

	// Verify FString api as well
	const FString eventName(TEXT("PlayerDeath"));
	const FString locationKey(TEXT("Location"));
	Herald::event(eventName, locationKey, vec);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FElasticTelemetryEventIntegratedTest,
    "ElasticTelemetry.EventRecorder.SendEventsToElasticSearch",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FElasticTelemetryEventIntegratedTest::RunTest(const FString & Parameters)
{
	// get the machine FGuid generated by Unreal
	const auto CPUVendor = FGenericPlatformMisc::GetCPUVendor();
	const auto CPUBrand  = FGenericPlatformMisc::GetCPUBrand();
	const auto CPUInfo   = FGenericPlatformMisc::GetCPUInfo();
	const auto CPUCount  = FGenericPlatformMisc::NumberOfCores();
	const auto CPUName   = FGenericPlatformMisc::GetCPUChipset();
	const auto DeviceId  = FGenericPlatformMisc::GetDeviceId();
	const auto LoginId   = FGenericPlatformMisc::GetLoginId();
	const auto OSVersion = FGenericPlatformMisc::GetOSVersion();
	const auto OSName    = FGenericPlatformMisc::GetOperatingSystemId();

	Herald::addHeader("LoginId", LoginId);
	Herald::event("MachineInfoTestEvent", "CPUVendor", CPUVendor, "CPUBrand", CPUBrand, "CPUInfo", CPUInfo, "CPUCount",
	    CPUCount, "CPUName", CPUName, "DeviceID", DeviceId, "OSVersion", OSVersion, "OSName", OSName);

	FSearch Query;
	Query.SetResultSize(10000)
	    .WithBoolQuery()
	    .AndHasTermFilter(FQueryTerm("log.event.keyword", "MachineInfoTestEvent"))
	    .AndHasRangeFilter(FQueryRange("timestamp", "2024-12-04T06:00:00Z", "2024-12-04T08:00:00Z"));

	std::string json = rapidjson::to_json(Query);

	// craft queries
	const auto & EditorModule =
	    FModuleManager::GetModuleChecked<FElasticTelemetryEditorModule>("ElasticTelemetryEditor");
	const auto & QueryClient = EditorModule.GetQueryClient();

	QueryClient.QueryIndex(FString(json.c_str()), [](bool success, const FJsonObject & response) {
		if (success)
		{
			UE_LOG(LogTemp, Warning, TEXT("Query success"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Query failed"));
		}
	});
	return false; // fail until the event posts and is verified in a query
}
