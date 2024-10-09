#include "ElasticTelemetry.h"
#if WITH_EDITOR
	#include "ISettingsModule.h"
	#include "ISettingsSection.h"
	#include "ISettingsContainer.h"
#endif
#include "ElasticTelemetryOutputDevice.h"

#define LOCTEXT_NAMESPACE "FElasticTelemetryModule"

void FElasticTelemetryModule::ShutdownModule()
{
#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Elastic Telemetry");
	}
#endif
	if (OutputDevice)
	{
		delete OutputDevice;
		OutputDevice = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
