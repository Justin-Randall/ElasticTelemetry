// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ElasticTelemetrySettings.h"
#include "ElasticTelemetryEnvironmentSettings.generated.h"

// UCLASS(config = ElasticTelemetry, defaultconfig, meta = (DisplayName = "Elastic Telemetry Settings"))
UCLASS(config = Engine, defaultconfig, meta = (DisplayName = "Elastic Telemetry Settings"))
class ELASTICTELEMETRY_API UElasticTelemetryEnvironmentSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UElasticTelemetryEnvironmentSettings();
	const FElasticTelemetrySettings	 GetActiveSettings() const;
	const FElasticTelemetrySettings* GetSettingsForEnvironment(const FString& Environment) const;
	virtual FName					 GetCategoryName() const override;
	virtual FName					 GetSectionName() const override;
	inline FString					 GetActiveEnvironment() const { return ActiveEnvironment; }
	inline void						 SetActiveEnvironment(const FString& Environment);

public:
	UPROPERTY(Config, EditAnywhere, meta = (EditCondition = false, HideEditConditionToggle))
	FString ActiveEnvironment;

	UPROPERTY(Config, EditAnywhere)
	TMap<FString, FElasticTelemetrySettings> Environments;
};