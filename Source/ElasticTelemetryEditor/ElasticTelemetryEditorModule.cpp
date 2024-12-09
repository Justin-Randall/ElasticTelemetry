// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "ElasticTelemetryEditorModule.h"
#include "ElasticTelemetry.h"
#include "ElasticTelemetryEnvironmentSettings.h"
#include "ElasticTelemetryEnvironmentSettingsCustomization.h"
#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "HttpModule.h"

#define LOCTEXT_NAMESPACE "FElasticTelemetryEditorModule"

IMPLEMENT_MODULE(FElasticTelemetryEditorModule, ElasticTelemetryEditor)

DEFINE_LOG_CATEGORY(TelemetryEditorLog);

void FElasticTelemetryEditorModule::StartupModule()
{
	UpdateConfig();

	// ensure the http module is setup from this thread before attempting to use it elsewhere
	// otherwise, the http module may not be ready when the http logger tries to use it
	FHttpModule::Get();

	if (GIsEditor)
	{
		FPropertyEditorModule & PropertyModule =
		    FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomClassLayout("ElasticTelemetryEnvironmentSettings",
		    FOnGetDetailCustomizationInstance::CreateStatic(
		        &FElasticTelemetryEnvironmentSettingsCustomization::MakeInstance));
	}

	if (ISettingsModule * SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Elastic Telemetry",
		    LOCTEXT("ElasticTelemetrySettingsName", "Elastic Telemetry Settings"),
		    LOCTEXT("ElasticTelemetrySettingsDescription", "Configure Elastic Telemetry"),
		    GetMutableDefault<UElasticTelemetryEnvironmentSettings>());
	}

	QueryConfig->SetListener(this);
}

void FElasticTelemetryEditorModule::ShutdownModule()
{
	QueryConfig->SetListener(nullptr);
	if (ISettingsModule * SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Elastic Telemetry");
	}
}

void FElasticTelemetryEditorModule::UpdateConfig()
{
	FScopeLock Lock(&SettingsLock);
	QueryConfig = GetMutableDefault<UElasticTelemetryQueryConfig>();
	if (nullptr == QueryConfig)
	{
		UE_LOG(TelemetryEditorLog, Error, TEXT("Failed to get settings."));
		return;
	}

	// Get the FElasticTelemetryModule
	const auto ElasticTelemetryModule = FModuleManager::GetModulePtr<FElasticTelemetryModule>("ElasticTelemetry");
	if (nullptr == ElasticTelemetryModule)
	{
		UE_LOG(TelemetryEditorLog, Error, TEXT("Failed to get ElasticTelemetry module."));
		return;
	}
	ElasticTelemetryModule->UpdateConfig();

	QueryConfig->SetListener(this);
}

#undef LOCTEXT_NAMESPACE
