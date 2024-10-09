// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "ElasticTelemetryWriter.h"
#include "Herald/ILogWriter.hpp"
#include "Herald/ILogWriterBuilder.hpp"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"

// This is all hidden away from the Engine so, use C++ standard library types expected by Herald, no conversions needed
#include <map>
#include <memory>
#include <string>
#include <vector>

class ElasticTelemetryWriter : public Herald::ILogWriter, public FRunnable
{
public:
	ElasticTelemetryWriter()
		: EndpointURL("")
		, Username("")
		, Password("")
		, IndexName("")
		, ConfigPairs()
		, CurrentPendingRequests(0)
		, WorkerThread(nullptr)
		, bStopWorkerThread(false)
		, QueueEvent(nullptr)
		, MaximumPendingRequests(4)
	{
		QueueEvent = FPlatformProcess::GetSynchEventFromPool(false);

		// Start the worker thread
		WorkerThread = FRunnableThread::Create(this, TEXT("ElasticTelemetryWriter"));
	}

	virtual ~ElasticTelemetryWriter() override
	{
		Stop();
		if (WorkerThread)
		{
			WorkerThread->WaitForCompletion();
			delete WorkerThread;
		}

		FPlatformProcess::ReturnSynchEventToPool(QueueEvent);
		QueueEvent = nullptr;
	}

	virtual ILogWriter& addConfigPair(const std::string& key, const std::string& value) override
	{
		// Config pairs should include ElasticSearch paramaters like
		// the endpoint URL, the writer username and password, the index name, etc.
		ConfigPairs[key] = value;

		if (key == "EndpointURL")
			EndpointURL = value.c_str();
		else if (key == "Username")
			Username = value.c_str();
		else if (key == "Password")
			Password = value.c_str();
		else if (key == "IndexName")
			IndexName = value.c_str();

		// ensure endpoint URL ends with a trailing slash
		if (EndpointURL.Len() > 0 && EndpointURL[EndpointURL.Len() - 1] != '/')
			EndpointURL += '/';

		return *this;
	}

	// This is going to happen in the same thread as the engine's GLog call context
	// so the write() method will merely queue up the message for the worker thread
	// to dispatch to the ElasticSearch server without blocking the game on I/O
	virtual void write(const std::string& Msg) override
	{
		if (bStopWorkerThread)
			return;

		{
			FScopeLock Lock(&QueueMutex);
			OutboundMessages.Enqueue(Msg);
		}
		QueueEvent->Trigger();
	}

	virtual uint32 Run() override
	{
		std::vector<FString> BatchMessages;

		while (!bStopWorkerThread)
		{
			QueueEvent->Wait();

			if (bStopWorkerThread)
				break;

			bool bHasMessage = false;

			std::string Msg;
			{
				FScopeLock Lock(&QueueMutex);
				while (OutboundMessages.IsEmpty() == false)
				{
					Msg.clear();
					bHasMessage = OutboundMessages.Dequeue(Msg);
					if (bHasMessage)
						BatchMessages.push_back(FString(Msg.c_str()));
				}
			}

			for (const auto i : BatchMessages)
			{
				// Send the message to the ElasticSearch server
				// using the HTTP module
				// LogUsingHttpModule(i, IndexName, nullptr);
				SendHttpRequest(*i);
			}
			BatchMessages.clear();
		}
		return 0;
	}

	virtual void Stop() override
	{
		bStopWorkerThread = true;
		if (QueueEvent)
			QueueEvent->Trigger();
	}

	FString							   EndpointURL;
	FString							   Username;
	FString							   Password;
	FString							   IndexName;
	std::map<std::string, std::string> ConfigPairs;
	uint32_t						   CurrentPendingRequests;
	uint32_t						   MaximumPendingRequests;

	// queue for outbound messages for the worker thread to pick up
	FRunnableThread*	WorkerThread;
	TQueue<std::string> OutboundMessages;
	FThreadSafeBool		bStopWorkerThread;
	FCriticalSection	QueueMutex;
	FEvent*				QueueEvent;

	void SendHttpRequest(const FString& Message)
	{
		static FString InCall;
		if (Message == InCall)
		{
			return; // avoid recursion in case something in FHttpModule or elsewhere logs
		}
		InCall = Message;

		FHttpModule& Http = FHttpModule::Get();
		auto		 Request = Http.CreateRequest();
		// const int32	 MaximumPendingRequests = Settings.MaximumPendingRequests;

		const FString FullURL = EndpointURL + IndexName + "/_doc";
		const FString Auth = FBase64::Encode(Username + ":" + Password);
		const FString AuthLine = FString("Basic ") + Auth;

		Request->SetURL(FullURL);
		Request->SetVerb("POST");
		Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
		Request->SetHeader("Content-Type", "application/json");
		Request->SetHeader("Authorization", AuthLine);

		Request->SetContentAsString(Message);
		// Prevent flooding libcurl. If it runs out of connections, it will spam like mad and drop the frame rate to 2FPS
		while (!bStopWorkerThread && CurrentPendingRequests >= MaximumPendingRequests)
		{
			FPlatformProcess::Sleep(0.1f);
		}

		// Increment the pending request count
		// This is the only thread accessing it, so no need to lock
		CurrentPendingRequests++;
		Request->ProcessRequest();
		Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
			// Decrement the pending request count
			// This is the only thread accessing it, so no need to lock
			CurrentPendingRequests--;
			InCall.Reset();

			int32 ResponseCode = 0;
			if (Response)
				ResponseCode = Response->GetResponseCode();

			if (ResponseCode > 399 || !bWasSuccessful) // error
			{
				// for debugging - do not submit a change with this active.
				// Because this lambda capture may trigger with HttpResponse
				// in an unknown state late in the application's lifecycle!
				// auto Content = Response->GetContentAsString();

				// the log server is having problems, or the client cannot reach it, so
				// stop doing all of this work for nothing.
				Request->CancelRequest();
				Stop();
			}
		});
	}
};

class ElasticTelemetryWriterBuilder : public Herald::ILogWriterBuilder
{
public:
	ElasticTelemetryWriterBuilder() = default;
	virtual ILogWriterBuilder& addConfigPair(const std::string& key, const std::string& value) override
	{
		ConfigPairs.push_back({ key, value });
		return *this;
	}

	virtual Herald::ILogWriterPtr build() override
	{
		auto result = std::make_unique<ElasticTelemetryWriter>();
		for (auto [k, v] : ConfigPairs)
		{
			result->addConfigPair(k, v);
			UE_LOG(LogTemp, Warning, TEXT("ConfigPair: %s = %s"), *FString(k.c_str()), *FString(v.c_str()));
		}
		return result;
	}
	virtual ~ElasticTelemetryWriterBuilder() override {}

	std::vector<std::pair<std::string, std::string>> ConfigPairs;
};

Herald::ILogWriterBuilderPtr createElasticTelemetryWriterBuilder()
{
	return std::make_unique<ElasticTelemetryWriterBuilder>();
}
