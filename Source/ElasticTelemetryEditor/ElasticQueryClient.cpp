// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#if WITH_EDITOR

#include "ElasticQueryClient.h"
#include "Dom/JsonObject.h"
#include "ElasticTelemetry.h"
#include "ElasticTelemetryEditorModule.h"
#include "FileNameFriendly.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"

FElasticQueryClient::FElasticQueryClient() {}

FElasticQueryClient::~FElasticQueryClient() {}

void FElasticQueryClient::QueryIndex(
    const FString & JsonQuery, TFunction<void(bool, const FJsonObject &)> OnRequestComplete) const
{
	FElasticTelemetryModule & Module = FModuleManager::GetModuleChecked<FElasticTelemetryModule>("ElasticTelemetry");

	// FElasticSearchTelemetryModule* Tel =
	// static_cast<FElasticSearchTelemetryModule*>(FModuleManager::Get().GetModule(FName("Telemetry")));
	auto          UserSettings      = Module.GetQuerySettings();
	const FString TelemetryUserName = UserSettings.Username;
	const FString TelemetryPassword = UserSettings.Password;

	auto          WriterSettings   = Module.GetSettings();
	const FString TelemetryURLBase = WriterSettings.EndpointURL;

	FHttpModule & Http    = FHttpModule::Get();
	auto          Request = Http.CreateRequest();

	FString normalizedIndex = TelemetryURLBase;
	// ensure normalizedIndex ends with a slash
	if (normalizedIndex.Len() > 0 && normalizedIndex[normalizedIndex.Len() - 1] != '/')
	{
		normalizedIndex += '/';
	}
	const FString TelemetryURL = normalizedIndex + WriterSettings.EventIndexName + "/_search";

	const FString Auth     = FBase64::Encode(TelemetryUserName + ":" + TelemetryPassword);
	const FString AuthLine = FString("Basic ") + Auth;

	Request->SetURL(TelemetryURL);
	Request->SetVerb("POST");
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Authorization", AuthLine);

	Request->SetContentAsString(JsonQuery);
	Request->OnProcessRequestComplete().BindLambda(
	    [=](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
		    if (nullptr == HttpResponse)
		    {
			    if (nullptr != HttpRequest)
			    {
				    HttpRequest->CancelRequest();
			    }
			    return;
		    }

		    const auto ResponseCode = HttpResponse->GetResponseCode();
		    if (ResponseCode > 399 || !bSucceeded) // error
		    {
			    // for debugging - do not submit a change with this active.
			    // Because this lambda capture may trigger with HttpResponse
			    // in an unknown state late in the application's lifecycle!
			    //
			    // auto Content = HttpResponse->GetContentAsString();

			    // the log server is having problems, or the client cannot reach it, so
			    // stop doing all of this work for nothing.
			    HttpRequest->CancelRequest();
		    }
		    else
		    {
			    auto Content = HttpResponse->GetContentAsString();

			    TSharedRef<TJsonReader<>> Reader  = TJsonReaderFactory<>::Create(*Content);
			    bool                      Success = false;
			    TSharedPtr<FJsonObject>   Result  = MakeShareable(new FJsonObject());
			    if (FJsonSerializer::Deserialize(Reader, Result))
			    {
				    Success = true;
			    }
			    OnRequestComplete(Success, *Result);
		    }
	    });
	Request->ProcessRequest();
}

#endif // WITH_EDITOR
