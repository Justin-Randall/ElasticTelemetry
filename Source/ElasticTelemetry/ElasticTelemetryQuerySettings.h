// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Logging/LogVerbosity.h"
#include "ElasticTelemetryQuerySettings.generated.h"

USTRUCT(BlueprintType)
struct ELASTICTELEMETRY_API FElasticTelemetryQuerySettings
{
	GENERATED_BODY()

	FElasticTelemetryQuerySettings();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "ElasticSearch user. Ask server administrator.")
	FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "ElasticSearch password. Ask server administrator.")
	FString Password;
};

class IPropertyUpdateListener
{
  public:
	virtual void OnPropertyUpdated() = 0;
};

// TODO - move this to the editor module. Keep the settings in the telemetry module
UCLASS(config = Editor, defaultconfig, meta = (DisplayName = "Elastic Query Settings"))
class ELASTICTELEMETRY_API UElasticTelemetryQueryConfig : public UDeveloperSettings
{
	GENERATED_BODY()

	UElasticTelemetryQueryConfig();

  public:
	inline const FElasticTelemetryQuerySettings & GetQuerySettings() const { return QuerySettings; }
	inline void SetListener(IPropertyUpdateListener * InListener) { Listener = InListener; }

#if WITH_EDITOR
  protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent & PropertyChangedEvent) override;
#endif // WITH_EDITOR

  private:
	UPROPERTY(Config, EditAnywhere)
	FElasticTelemetryQuerySettings QuerySettings;
	IPropertyUpdateListener *      Listener;
};
