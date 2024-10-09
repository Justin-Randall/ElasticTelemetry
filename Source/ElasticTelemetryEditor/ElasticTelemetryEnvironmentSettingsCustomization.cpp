// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "ElasticTelemetryEnvironmentSettingsCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "ElasticTelemetryEnvironmentSettings.h"
#include "Widgets/Input/SComboBox.h"
#include "PropertyHandle.h"

TSharedRef<IDetailCustomization> FElasticTelemetryEnvironmentSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FElasticTelemetryEnvironmentSettingsCustomization);
}

TSharedPtr<FString> FElasticTelemetryEnvironmentSettingsCustomization::GetInitiallySelectedItem(const FString& CurrentValue) const
{
	for (const auto& Option : Options)
	{
		if (Option.IsValid() && *Option == CurrentValue)
		{
			return Option;
		}
	}

	// Return a valid item even if no match is found (prevents crashes)
	return Options.Num() > 0 ? Options[0] : nullptr;
}

void FElasticTelemetryEnvironmentSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	ActiveEnvHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UElasticTelemetryEnvironmentSettings, ActiveEnvironment));
	EnvironmentsHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UElasticTelemetryEnvironmentSettings, Environments));
	DetailBuilder.HideProperty(ActiveEnvHandle);

	EnvironmentsHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FElasticTelemetryEnvironmentSettingsCustomization::RepopulateOptions));

	RepopulateOptions();

	FString CurrentValue;
	ActiveEnvHandle->GetValue(CurrentValue);

	DetailBuilder.EditCategory("ElasticTelemetryEnvironmentSettings")
		.AddCustomRow(FText::FromString("Active Environment"))
		.NameContent()
			[ActiveEnvHandle->CreatePropertyNameWidget()]
		.ValueContent()
			[SAssignNew(OptionsComboBox, SComboBox<TSharedPtr<FString>>)
					.OptionsSource(&Options)
					.OnSelectionChanged(this, &FElasticTelemetryEnvironmentSettingsCustomization::OnEnvironmentSelected, ActiveEnvHandle.ToSharedRef())
					.InitiallySelectedItem(GetInitiallySelectedItem(CurrentValue))
					.OnGenerateWidget_Lambda([](TSharedPtr<FString> InOption) -> TSharedRef<SWidget> {
						return SNew(STextBlock).Text(FText::FromString(*InOption));
					})
					.Content()
						[SNew(STextBlock)
								.Text_Lambda([this]() {
									FString Value;
									ActiveEnvHandle->GetValue(Value);
									return FText::FromString(Value);
								})]];
}

void FElasticTelemetryEnvironmentSettingsCustomization::SetupKeyChangeListeners()
{
	if (!EnvironmentsHandle.IsValid())
	{
		return;
	}

	uint32 NumElements = 0;
	EnvironmentsHandle->GetNumChildren(NumElements);

	for (uint32 i = 0; i < NumElements; ++i)
	{
		TSharedPtr<IPropertyHandle> ElementHandle = EnvironmentsHandle->GetChildHandle(i);
		TSharedPtr<IPropertyHandle> KeyHandle = ElementHandle->GetKeyHandle();

		if (KeyHandle.IsValid())
		{
			KeyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([this]() {
				RepopulateOptions();
			}));
		}
	}
}

void FElasticTelemetryEnvironmentSettingsCustomization::RepopulateOptions()
{
	Options.Empty();

	if (const UElasticTelemetryEnvironmentSettings* Settings = GetDefault<UElasticTelemetryEnvironmentSettings>())
	{
		for (const auto& Elem : Settings->Environments)
		{
			Options.Add(MakeShared<FString>(Elem.Key));
		}

		FString ActiveEnvironmentValue;
		ActiveEnvHandle->GetValue(ActiveEnvironmentValue);
		if (!Options.ContainsByPredicate([&](const TSharedPtr<FString>& Option) { return *Option == ActiveEnvironmentValue; }))
		{
			ActiveEnvHandle->SetValue(*Options[0]);
		}

		if (GEditor)
		{
			GEditor->GetTimerManager()->SetTimerForNextTick([this]() {
				SetupKeyChangeListeners();
			});
		}

		// Refresh the combo box to show the updated options
		if (OptionsComboBox.IsValid())
		{
			OptionsComboBox->RefreshOptions();
		}
	}
}

void FElasticTelemetryEnvironmentSettingsCustomization::OnEnvironmentSelected(TSharedPtr<FString> SelectedOption, ESelectInfo::Type SelectInfo, TSharedRef<IPropertyHandle> PropertyHandle)
{
	if (SelectedOption.IsValid())
	{
		PropertyHandle->SetValue(*SelectedOption); // Update the ActiveEnvironment property when a new option is selected
	}
}
