<#
.SYNOPSIS
    Run Unreal Engine automated tests from the command line using UnrealEditor-Cmd.exe.
.DESCRIPTION
    This script runs automation tests in an Unreal Engine project via the command line without launching the full editor.
    If OpenCppCoverage is installed, it generates a Cobertura XML code coverage report.
    It also checks the coverage percentage against a threshold and fails the build if the threshold is not met.
.PARAMETER ProjectPath
    The full path to the Unreal Engine project (.uproject file). Defaults to the inferred project path.
.PARAMETER TestPrefix
    The prefix of the test name you want to run (e.g., 'ElasticTelemetry'). Defaults to 'ElasticTelemetry'.
.PARAMETER AdditionalArgs
    Any additional arguments you want to pass to UnrealEditor-Cmd.exe. Defaults to an empty string.
.PARAMETER UnrealEngineDir
    The path to the Unreal Engine directory. Defaults to "C:\EpicGames\UE_5.4".
.PARAMETER Threshold
    The minimum code coverage percentage threshold required for the tests to pass.
.EXAMPLE
    ./RunTests.ps1 -ProjectPath "C:\MyProject\MyGame.uproject" -TestPrefix "MyGame.FastTests" -Threshold 80
.EXAMPLE
    ./RunUnrealTests.ps1
    This will run with default parameters and generate code coverage if OpenCppCoverage is installed.
.NOTES
    This script will fail if the code coverage percentage is below the specified threshold.
#>

param (
    [Parameter(Mandatory=$false)]
    [string]$ProjectPath = $null,
    # [string]$ProjectPath = "$PSScriptRoot\..\..\PluginWorkspace.uproject",

    [Parameter(Mandatory=$false)]
    [string]$TestPrefix = "ElasticTelemetry",

    [Parameter(Mandatory=$false)]
    [string]$UnrealEngineDir = "C:\EpicGames\UE_5.4",

    [Parameter(Mandatory=$false)]
    [string]$AdditionalArgs = "",

    [Parameter(Mandatory=$false)]
    [double]$Threshold = 80  # Default threshold to 80%
)

if(-not $ProjectPath) {
    # Define the directory to search
    $SearchDirectory = Join-Path $PSScriptRoot '..\..'

    # Search for .uproject files in the directory
    $UProjectFiles = Get-ChildItem -Path $SearchDirectory -Filter '*.uproject' -File -Recurse

    if ($UProjectFiles.Count -eq 1) {
        # Only one .uproject file found
        $ProjectPath = $UProjectFiles[0].FullName
    }
    elseif ($UProjectFiles.Count -gt 1) {
        # Multiple .uproject files found, select the first one or handle as needed
        Write-Warning "Multiple .uproject files found. Using the first one."
        $ProjectPath = $UProjectFiles[0].FullName
    }
    else {
        # No .uproject files found
        Write-Error "No .uproject files found in the directory '$SearchDirectory'. Please specify the -ProjectPath parameter."
        exit 1
    }
}

Write-Host "[info] Starting Unreal Automation Tests"
Write-Host ""

$ProjectRoot = (Resolve-Path -Path "$PSScriptRoot\..\..").Path

# Need the full path to the project file. The default above uses $PSScriptRoot to help infer the location.
$ResolvedProjectPath = (Resolve-Path -Path $ProjectPath).Path

# Unreal's command-line tools typically require absolute paths to locate project files correctly.
$UnrealEditorCmdPath = "$UnrealEngineDir\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"

function Test-RequiredPath {
    param (
        [string]$Path,
        [string]$ErrorMessage
    )
    if (-Not (Test-Path $Path)) {
        Write-Host $ErrorMessage
        exit 1
    }
}

Test-RequiredPath $ResolvedProjectPath "[Error] Project file '$ResolvedProjectPath' not found."
Test-RequiredPath $UnrealEditorCmdPath "[Error] UnrealEditor-Cmd.exe is missing and not found at '$UnrealEditorCmdPath'."

# If CppCoverage is available, use it to also generate coverage reports
# otherwise just the tests run and the pass/fail relies solely on that.
$OpenCppCoverageCommand = Get-Command "OpenCppCoverage.exe" -ErrorAction SilentlyContinue
$UseCoverage = $false
if ($OpenCppCoverageCommand) {
    $OpenCppCoveragePath = $OpenCppCoverageCommand.Path
    Write-Host "[info] OpenCppCoverage detected at $OpenCppCoveragePath."
    $UseCoverage = $true
} else {
    Write-Host "[info] OpenCppCoverage not found."
}

# On the fence about this one. These are *technically* artifacts of the build process
# but they are also specific to a build in the context of the build for use by
# build tools and automation, not necessarily coders. 
$CoverageOutputDir = "$PSScriptRoot\Intermediate\CodeCoverage"

# Create directory if it doesn't exist
if (-Not (Test-Path $CoverageOutputDir)) {
    New-Item -Path $CoverageOutputDir -ItemType Directory -Force | Out-Null
}

# Convert CoverageOutputDir to a full path
$CoverageOutputDir = (Resolve-Path -Path $CoverageOutputDir).Path
$CoberturaFile = "$CoverageOutputDir\coverage.xml"

# This is going to be in "Saved/Logs" in the project directory.
$TestsLogFile = "AutomatedTests.log"

# Don't risk reporting old coverage data. Clear it out before running tests.
function Clear-CoverageOutput {
    if (Test-Path $CoverageOutputDir) {
        Remove-Item -Path "$CoverageOutputDir\*" -Recurse -Force | Out-Null
    }
    if (-Not (Test-Path $CoverageOutputDir)) {
        New-Item -Path $CoverageOutputDir -ItemType Directory -Force | Out-Null
    }
}

# Two different Start-Process paths in this script. This function helps keep the code DRY.
function Invoke-Process {
    param (
        [string]$FilePath,
        [string]$Arguments
    )
    
    # Starts the process and waits for it to complete
    $process = Start-Process -FilePath $FilePath -ArgumentList $Arguments -NoNewWindow -Wait -PassThru
    
    # Capture the exit code from the process
    return $process.ExitCode
}

# UnrealEditor-Cmd command line, crafted via an array just
# to make changes/edits easier as well as keeping diffs sane.
$ExecCmd = "Automation RunTests $TestPrefix; Quit"
$Arguments = @(
    $ResolvedProjectPath,
    "-NoLoadMap",
    "-nullrhi", 
    "-ExecCmds=`"$ExecCmd`"",
    "-NoSplash",
    "-Unattended",
    "-NoCompile",
    "-NoLogTimes",
    "-Log=$TestsLogFile",
     $AdditionalArgs
) -join " "

# Unreal startup will only get worse over time. This may be handy
# if an effort is made to try to pair down the startup times just to get to the fast tests.
$testExecutionTime = Measure-Command {
    if ($UseCoverage) {
        Write-Host "OpenCppCoverage generating coverage report."
        Clear-CoverageOutput
        
        # Execute tests wrapped with OpenCppCoverage
        # OpenCppCoverage does not natively support LCOV exclusions, so we use the --excluded_line_regex option
        $OpenCppCoverageArgs = @(
            "--export_type=cobertura:$CoberturaFile",
            "--export_type=html:$CoverageOutputDir",
            "--sources", "Plugins",
            "--modules", "$ProjectRoot\Plugins\ElasticTelemetry"
            "--excluded_line_regex", ".*LCOV_EXCL_LINE.*"
            "--excluded_sources", "UnrealEngine",
            "--excluded_sources", "Intermediate",
            "--excluded_sources", "$ProjectRoot\Plugins\ElasticTelemetry\Source\ElasticTelemetry\ElasticTelemetry_ShutdownModule.cpp",
            "--excluded_sources", "$ProjectRoot\Plugins\ElasticTelemetry\Source\ElasticTelemetryTests\ElasticTelemetryTests_ShutdownModule.cpp",
            "--", $UnrealEditorCmdPath, $Arguments
        ) -join " "
        $exitCode = Invoke-Process -FilePath $OpenCppCoveragePath -Arguments $OpenCppCoverageArgs
        Write-Host "[info] OpenCppCoverage has finished generating the code coverage report, exit code: $exitCode"
    } else {
        Write-Host "[info] Running tests without coverage."
        $exitCode = Invoke-Process -FilePath $UnrealEditorCmdPath -Arguments $Arguments
        Write-Host "[info] Unreal Automation Tests have finished running, exit code: $exitCode"
    }
}
Write-Host ""
Write-Host "[info] The test run took $($testExecutionTime.TotalSeconds) seconds (including engine initialization and setup)."

$AutomatedTestsLogPath = "$ProjectRoot\Saved\Logs\$TestsLogFile"

function Show-Log {
    Write-Host ""
    Write-Host "=== Errors, Warnings and LogAutomation Output from $AutomatedTestsLogPath ==="
    Get-Content $AutomatedTestsLogPath | Select-String -Pattern "Warning|Error|LogAutomationC" | ForEach-Object { Write-Host $_.Line }
    Write-Host "=== End of Errors, Warnings and LogAutomation Output ==="
    Write-Host ""
}

# Give CI/CD an opportunity to fail the build if tests fail
if ($exitCode -ne 0) {
    # Find all lines with Warning, Error or LogAutomationC in the log file and print the output
    Show-Log
    Write-Host "[error] Tests failed."
    
    exit $exitCode
}

Write-Host ""
Write-Host "[info] All tests passed."

# If coverage was generated, check coverage threshold. Check the default at the top of this file.
# Adjust it upward as code coverage improves. It should NOT be going down, that means things are
# getting worse, not better.
if (Test-Path $CoberturaFile) {
    [xml]$coverageXml = Get-Content $CoberturaFile
    $lineRate = $coverageXml.coverage.'line-rate'
    $coverage = [math]::Round([double]$lineRate * 100, 2)

    if ($coverage -lt $Threshold) {
        Show-Log
        Write-Output "[error] Coverage ($coverage%) is below the threshold ($Threshold%)."
        exit 1
    } else {
        Write-Output "[info] Coverage ($coverage%) meets the threshold ($Threshold%)."
        exit 0
    }
} else {
    Write-Host "[info] No coverage file found!"
    exit 0
}
