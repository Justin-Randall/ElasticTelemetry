// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "ElasticTelemetryEditorModule.h"
#include "Modules/ModuleManager.h"
#include "ElasticTelemetryEnvironmentSettingsCustomization.h"
#include "ElasticTelemetryEnvironmentSettings.h"
#include "PropertyEditorModule.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"

#define LOCTEXT_NAMESPACE "FElasticTelemetryEditorModule"

IMPLEMENT_MODULE(FElasticTelemetryEditorModule, ElasticTelemetryEditor)

void FElasticTelemetryEditorModule::StartupModule()
{
	if (GIsEditor)
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomClassLayout("ElasticTelemetryEnvironmentSettings",
			FOnGetDetailCustomizationInstance::CreateStatic(&FElasticTelemetryEnvironmentSettingsCustomization::MakeInstance));
	}

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Elastic Telemetry",
			LOCTEXT("ElasticTelemetrySettingsName", "Elastic Telemetry Settings"),
			LOCTEXT("ElasticTelemetrySettingsDescription", "Configure Elastic Telemetry"),
			GetMutableDefault<UElasticTelemetryEnvironmentSettings>());
	}
}

void FElasticTelemetryEditorModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Elastic Telemetry");
	}
}

#undef LOCTEXT_NAMESPACE
