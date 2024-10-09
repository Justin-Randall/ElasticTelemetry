#pragma once

#include "CoreMinimal.h"
#include "Herald/ILogTransformer.hpp"
#include "Herald/ILogWriter.hpp"

class FElasticTelemetryModule;

class ELASTICTELEMETRY_API FElasticTelemetryOutputDevice : public FOutputDevice
{
public:
	explicit FElasticTelemetryOutputDevice(const FElasticTelemetryModule& Module);
	virtual ~FElasticTelemetryOutputDevice() override;

	inline Herald::ILogTransformerPtr GetJsonTransformer() const
	{
		return JsonTransformer;
	}

protected:
	virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const class FName& Category) override;

	mutable FCriticalSection	   ProcessingRequestLock;
	size_t						   ProcessingRequestCount;
	Herald::ILogTransformerPtr	   JsonTransformer;
	Herald::ILogWriterPtr		   ElasticWriter;
	const FElasticTelemetryModule& ElasticTelemetry;
};
