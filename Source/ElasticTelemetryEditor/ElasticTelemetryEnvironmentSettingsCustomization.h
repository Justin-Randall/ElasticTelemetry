// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#pragma once

#include "IDetailCustomization.h"

class FElasticTelemetryEnvironmentSettingsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	TSharedPtr<IPropertyHandle>				   ActiveEnvHandle;
	TSharedPtr<IPropertyHandle>				   EnvironmentsHandle;
	TArray<TSharedPtr<FString>>				   Options;
	TSharedPtr<FString>						   GetInitiallySelectedItem(const FString& CurrentValue) const;
	TSharedPtr<SComboBox<TSharedPtr<FString>>> OptionsComboBox;

	void RepopulateOptions();
	void OnEnvironmentSelected(TSharedPtr<FString> SelectedOption, ESelectInfo::Type SelectInfo, TSharedRef<IPropertyHandle> PropertyHandle);
	void SetupKeyChangeListeners();
};
