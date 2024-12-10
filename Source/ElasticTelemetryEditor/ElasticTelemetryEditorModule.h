// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#if WITH_EDITOR

#pragma once

#include "CoreMinimal.h"
#include "ElasticTelemetryQuerySettings.h"
#include "ElasticQueryClient.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(TelemetryEditorLog, Log, All);

class FElasticTelemetryEditorModule : public IModuleInterface, public IPropertyUpdateListener
{
  public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	inline const FElasticTelemetryQuerySettings & GetQuerySettings() const { return QueryConfig->GetQuerySettings(); }
	inline const FElasticQueryClient &            GetQueryClient() const { return QueryClient; }

  protected:
	void        UpdateConfig();
	inline void OnPropertyUpdated() override { UpdateConfig(); }

  private:
	mutable FCriticalSection       SettingsLock;
	UElasticTelemetryQueryConfig * QueryConfig;
	FElasticQueryClient            QueryClient;
};

#endif // WITH_EDITOR
