#include "Misc/AutomationTest.h"
#include "ElasticTelemetrySettings.h"
#include "ElasticTelemetryEnvironmentSettings.h"
#include "ETLogger.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FElasticTelemetrySettingsTest, "ElasticTelemetry.Settings.ConcreteClassTests", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FElasticTelemetrySettingsTest::RunTest(const FString& Parameters)
{
	FElasticTelemetrySettings Settings;

	// Test constructor defaults. Changing those defaults will break the tests because they may also break assumptions elsewhere
	TestEqual(TEXT("Default Endpoint URL should be 'https://localhost:9200'"), Settings.EndpointURL, TEXT("https://localhost:9200"));
	TestEqual(TEXT("Default Username should be 'DefaultUser'"), Settings.Username, TEXT("DefaultUser"));
	TestEqual(TEXT("Default Password should be 'ChangeMe'"), Settings.Password, TEXT("ChangeMe"));
	TestEqual(TEXT("Default IndexName should be 'UELog'"), Settings.IndexName, TEXT("UELog"));
	TestFalse(TEXT("Elastic Telemetry should be disabled by default"), Settings.Enabled);
	TestTrue(TEXT("LogLevel Fatal should be enabled by default"), Settings.IsLogLevelEnabled(ELogVerbosity::Fatal));
	TestTrue(TEXT("LogLevel Error should be enabled by default"), Settings.IsLogLevelEnabled(ELogVerbosity::Error));
	TestTrue(TEXT("LogLevel Warning should be enabled by default"), Settings.IsLogLevelEnabled(ELogVerbosity::Warning));
	TestTrue(TEXT("LogLevel Display should be enabled by default"), Settings.IsLogLevelEnabled(ELogVerbosity::Display));
	TestTrue(TEXT("LogLevel Log should be enabled by default"), Settings.IsLogLevelEnabled(ELogVerbosity::Log));
	TestFalse(TEXT("LogLevel Verbose should be disabled by default"), Settings.IsLogLevelEnabled(ELogVerbosity::Verbose));
	TestFalse(TEXT("LogLevel VeryVerbose should be disabled by defaults"), Settings.IsLogLevelEnabled(ELogVerbosity::VeryVerbose));
	TestFalse(TEXT("Invalid LogLevel should be disabled by default"), Settings.IsLogLevelEnabled(static_cast<ELogVerbosity::Type>(-1)));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FElasticTelemetryEnvironmentSettingsTest, "ElasticTelemetry.Settings.PerEnvironment", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FElasticTelemetryEnvironmentSettingsTest::RunTest(const FString& Parameters)
{
	// There are separate settings for each environment, and there is a container class that holds all of them
	// the container class is called UElasticTelemetryEnvironmentSettings and should have a TMap of FString to
	// UElasticTelemetrySettings to get the settings for a specific environment, you can call
	// GetSettingsForEnvironment(FString EnvironmentName). The class should have some default settings for each environment
	// on the constructor, and the settings should be configurable in the editor. They will come from ini files, via UDeveloperSettings
	// and should be able to be changed at runtime in the editor. The settings should be saved to the ini files when changed in the editor.

	// Instantiate the UElasticTelemetryEnvironmentSettings class
	UElasticTelemetryEnvironmentSettings* EnvironmentSettings = NewObject<UElasticTelemetryEnvironmentSettings>();

	TestEqual(TEXT("Category name should be 'Plugins'"), EnvironmentSettings->GetCategoryName(), FName("Plugins"));
	TestEqual(TEXT("Section name should be 'Elastic Telemetry'"), EnvironmentSettings->GetSectionName(), FName("Elastic Telemetry"));

	// UElasticTelemetryEnvironmentSettings EnvironmentSettings = GetMutableDefault<UElasticTelemetryEnvironmentSettings>();

	// Get the settings for the "Development" environment
	const FElasticTelemetrySettings* DevelopmentSettings = EnvironmentSettings->GetSettingsForEnvironment("Development");
	TestNotNull(TEXT("Development settings should not be null"), DevelopmentSettings);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FElasticTelemetryHelpersTest, "ElasticTelemetry.Settings.Helpers", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FElasticTelemetryHelpersTest::RunTest(const FString& Parameters)
{
	// add key/value headers for the session
	// useful for adding metadata to each log entry for searching, collation and analysis. Like a player ID, machine ID, multiplayer session ID....
	Herald::addHeader("TestHeaderKey", "TestHeaderValue");

	Herald::log(Herald::LogLevels::Info, "Helper no variadics");

	Herald::log(Herald::LogLevels::Info, "Helper with variadics", "Hello", "World");

	return true;
}
