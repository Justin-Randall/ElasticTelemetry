#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Logging/LogVerbosity.h"
#include "ElasticTelemetrySettings.generated.h"

#if 0
// Needed to convert between Unreal's log verbosity and our custom log verbosity because Unreal's log verbosity will not work with a TArray
UENUM(BlueprintType)
enum class EElasticLogVerbosity : uint8
{
	NoLogging    UMETA(DisplayName = "No Logging"),
	Fatal        UMETA(DisplayName = "Fatal"),
	Error        UMETA(DisplayName = "Error"),
	Warning      UMETA(DisplayName = "Warning"),
	Display      UMETA(DisplayName = "Display"),
	Log          UMETA(DisplayName = "Log"),
	Verbose      UMETA(DisplayName = "Verbose"),
	VeryVerbose  UMETA(DisplayName = "Very Verbose")
};

inline EElasticLogVerbosity ConvertLogVerbosity(const ELogVerbosity::Type Level)
{
	switch (Level)
	{
	case ELogVerbosity::NoLogging: return EElasticLogVerbosity::NoLogging;
	case ELogVerbosity::Fatal: return EElasticLogVerbosity::Fatal;
	case ELogVerbosity::Error: return EElasticLogVerbosity::Error;
	case ELogVerbosity::Warning: return EElasticLogVerbosity::Warning;
	case ELogVerbosity::Display: return EElasticLogVerbosity::Display;
	case ELogVerbosity::Log: return EElasticLogVerbosity::Log;
	case ELogVerbosity::Verbose: return EElasticLogVerbosity::Verbose;
	case ELogVerbosity::VeryVerbose: return EElasticLogVerbosity::VeryVerbose;
	default: return EElasticLogVerbosity::NoLogging;
	}
}
#endif

USTRUCT(BlueprintType)
struct ELASTICTELEMETRY_API FElasticTelemetrySettings
{
	GENERATED_BODY()

	FElasticTelemetrySettings();

	// Conversion helper
	bool IsLogLevelEnabled(ELogVerbosity::Type LogLevel) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Enable/Disable Elastic Telemetry Globally")
	bool Enabled;

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "ElasticSearch API endpoint. Usually https://someserver.com:9200/")
	FString EndpointURL;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Index prefix for all log messages, defaults to UELog")
	FString IndexName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "ElasticSearch user. Ask server administrator.")
	FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "ElasticSearch password. Ask server administrator.")
	FString Password;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool EnableFatal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool EnableError;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool EnableWarning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool EnableDisplay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool EnableLog;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool EnableVerbose;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool EnableVeryVerbose;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IncludeCallstacksOnFatal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IncludeCallstacksOnError;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IncludeCallstacksOnWarning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IncludeCallstacksOnDisplay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IncludeCallstacksOnLog;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IncludeCallstacksOnVerbose;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IncludeCallstacksOnVeryVerbose;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, DisplayName = "List of categories to exclude. Ignored if IncludedCategories is not empty.")
	TArray<FName> ExcludedLogCategories;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, DisplayName = "List of categories to only include. Exclusions are ignored if this is not empty.")
	TArray<FName> IncludedLogCategories;
};
