#include "ElasticTelemetry.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"
#include "ElasticTelemetryOutputDevice.h"

#define LOCTEXT_NAMESPACE "FElasticTelemetryModule"

void FElasticTelemetryModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Elastic Telemetry");
	}

	if (OutputDevice)
	{
		delete OutputDevice;
		OutputDevice = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
