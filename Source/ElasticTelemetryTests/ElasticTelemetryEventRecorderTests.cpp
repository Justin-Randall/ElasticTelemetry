// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#if WITH_EDITOR

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

class FWaitForTelemetryQueryLatentCommand : public IAutomationLatentCommand
{
  public:
	FWaitForTelemetryQueryLatentCommand(FElasticTelemetryEventIntegratedTest * InTest, const FString & InJsonQuery)
	    : Test(InTest)
	    , JsonQuery(InJsonQuery)
	    , bQueryIssued(false)
	    , bRetrieved(false)
	    , Retries(0)
	{
	}

	virtual bool Update() override
	{
		// If we haven't issued the query yet, do it now
		if (!bQueryIssued)
		{
			bQueryIssued = true;
			IssueQuery();
		}

		// If we've retrieved a successful response, we're done
		if (bRetrieved)
		{
			Test->TestTrue(TEXT("Successfully retrieved telemetry event within time range"), true);
			return true; // Command finished successfully
		}

		// If not retrieved yet, check if we need to retry
		if (Retries >= MaxRetries)
		{
			Test->AddError(TEXT("Failed to retrieve telemetry event within the given retries"));
			return true; // Command finished with failure
		}

		// Not done yet, try again next frame after sleeping 1 second
		FPlatformProcess::Sleep(1.0f);
		Retries++;
		IssueQuery();
		return false;
	}

  private:
	void IssueQuery()
	{
		const auto & EditorModule =
		    FModuleManager::GetModuleChecked<FElasticTelemetryEditorModule>("ElasticTelemetryEditor");
		const auto & QueryClient = EditorModule.GetQueryClient();

		QueryClient.QueryIndex(JsonQuery, [this](bool success, const FJsonObject & response) {
			if (success)
			{
				// Validate that we got some hits
				if (!response.HasField(TEXT("hits")))
				{
					Test->AddError(TEXT("Response missing 'hits' field"));
					return;
				}

				TSharedPtr<FJsonObject> hitsObj = response.GetObjectField(TEXT("hits"));
				if (!hitsObj.IsValid())
				{
					Test->AddError(TEXT("'hits' is not a valid JSON object"));
					return;
				}

				if (!hitsObj->HasField(TEXT("total")))
				{
					Test->AddError(TEXT("No 'total' field in 'hits' object"));
					return;
				}

				TSharedPtr<FJsonObject> totalObj = hitsObj->GetObjectField(TEXT("total"));
				if (!totalObj.IsValid() || !totalObj->HasField(TEXT("value")))
				{
					Test->AddError(TEXT("No 'value' field in 'total' object"));
					return;
				}

				double totalValue = totalObj->GetNumberField(TEXT("value"));
				Test->TestTrue(TEXT("At least one hit returned"), totalValue > 0);

				// Now check the hits array
				const TArray<TSharedPtr<FJsonValue>> * hitsArray = nullptr;
				if (!hitsObj->TryGetArrayField(TEXT("hits"), hitsArray) || !hitsArray || hitsArray->Num() == 0)
				{
					Test->AddError(TEXT("No hits array or empty hits array"));
					return;
				}

				bool foundMachineInfoTestEvent = false;

				// Look through each hit for the expected event and properties
				for (const TSharedPtr<FJsonValue> & hitVal : *hitsArray)
				{
					if (!hitVal.IsValid())
						continue;

					TSharedPtr<FJsonObject> hitObj = hitVal->AsObject();
					if (!hitObj.IsValid())
						continue;

					if (!hitObj->HasField(TEXT("_source")))
						continue;

					TSharedPtr<FJsonObject> sourceObj = hitObj->GetObjectField(TEXT("_source"));
					if (!sourceObj.IsValid())
						continue;

					if (!sourceObj->HasField(TEXT("log")))
						continue;

					TSharedPtr<FJsonObject> logObj = sourceObj->GetObjectField(TEXT("log"));
					if (!logObj.IsValid())
						continue;

					// Check the event name
					if (logObj->HasField(TEXT("event")))
					{
						FString eventName = logObj->GetStringField(TEXT("event"));
						if (eventName == "MachineInfoTestEvent")
						{
							foundMachineInfoTestEvent = true;

							// Validate some fields we posted
							if (logObj->HasField(TEXT("CPUBrand")))
							{
								FString CPUBrand = logObj->GetStringField(TEXT("CPUBrand"));
								// We know what we sent was some known brand, e.g. "GenericCPUBrand"
								Test->TestEqual(TEXT("CPUBrand matches expected"), CPUBrand, TEXT("GenericCPUBrand"));
							}
							else
							{
								Test->AddError(TEXT("Missing 'CPUBrand' in log data"));
							}

							// You can add more checks here for CPUCount, CPUInfo, etc. as needed

							// If all we needed was to confirm at least one correct event hit, we can break here.
							break;
						}
					}
				}

				Test->TestTrue(TEXT("Found at least one MachineInfoTestEvent in the hits"), foundMachineInfoTestEvent);

				// Mark retrieval as successful
				bRetrieved = true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Query failed"));
			}
		});
	}

  private:
	static constexpr int                   MaxRetries = 10;
	FElasticTelemetryEventIntegratedTest * Test;
	FString                                JsonQuery;
	bool                                   bQueryIssued;
	bool                                   bRetrieved;
	int                                    Retries;
};

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

	std::chrono::system_clock::time_point now              = std::chrono::system_clock::now();
	const auto                            fiveMinutesAgo   = std::to_string(now - std::chrono::minutes(5));
	const auto                            fiveMinutesAhead = std::to_string(now + std::chrono::minutes(5));

	FSearch Query;
	Query.SetResultSize(10000)
	    .WithBoolQuery()
	    .AndHasTermFilter(FQueryTerm("log.event.keyword", "MachineInfoTestEvent"))
	    .AndHasRangeFilter(FQueryRange("timestamp", fiveMinutesAgo, fiveMinutesAhead));

	std::string json = rapidjson::to_json(Query);
	FString     JsonQuery(json.c_str());
	ADD_LATENT_AUTOMATION_COMMAND(FWaitForTelemetryQueryLatentCommand(this, JsonQuery));
#if 0
	// craft queries
	const auto & EditorModule =
	    FModuleManager::GetModuleChecked<FElasticTelemetryEditorModule>("ElasticTelemetryEditor");
	const auto & QueryClient = EditorModule.GetQueryClient();

	bool retrieved = false;
	int  retries   = 0;
	while (retries < 10 && !retrieved)
	{
		FPlatformProcess::Sleep(1.0f);

		QueryClient.QueryIndex(FString(json.c_str()), [&retrieved](bool success, const FJsonObject & response) {
			if (success)
			{
				UE_LOG(LogTemp, Warning, TEXT("Query success"));

				retrieved = true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Query failed"));

				// sleep for a second before retrying
			}
		});
		++retries;
	}

	return retrieved;
#endif // 0
	return true;
}
#endif // WITH_EDITOR
