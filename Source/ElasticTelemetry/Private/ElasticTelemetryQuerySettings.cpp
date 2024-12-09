// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "ElasticTelemetryQuerySettings.h"

FElasticTelemetryQuerySettings::FElasticTelemetryQuerySettings()
    : Username(TEXT("InvalidUser"))
    , Password(TEXT("ChangeMe"))
{
}

UElasticTelemetryQueryConfig::UElasticTelemetryQueryConfig()
    : Listener(nullptr)
{
}

void UElasticTelemetryQueryConfig::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (Listener)
	{
		Listener->OnPropertyUpdated();
	}
}
