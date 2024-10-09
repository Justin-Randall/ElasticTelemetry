// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "ElasticTelemetry.h"
#include "ElasticTelemetryOutputDevice.h"

#define LOCTEXT_NAMESPACE "FElasticTelemetryModule"

void FElasticTelemetryModule::ShutdownModule()
{
	if (OutputDevice)
	{
		delete OutputDevice;
		OutputDevice = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
