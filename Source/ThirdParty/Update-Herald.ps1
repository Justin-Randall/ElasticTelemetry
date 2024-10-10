# Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
# MIT License, see LICENSE file for full details.

$heraldUrl = "https://github.com/Justin-Randall/herald/releases/download/latest/Herald.zip"
$thirdPartyPath = "$PSScriptRoot"  # This is the path to the ThirdParty directory where the script resides
$heraldDir = Join-Path $thirdPartyPath "Herald"
$heraldZip = Join-Path $thirdPartyPath "Herald.zip"

# Download the Herald zip file
Write-Host "Downloading Herald.zip from $heraldUrl ..."
try {
    Invoke-WebRequest -Uri $heraldUrl -OutFile $heraldZip -UseBasicParsing
    Write-Host "Download successful."
} catch {
    Write-Host "Failed to download Herald.zip."
    exit 1
}

# Remove existing Herald directory if download is successful
if (Test-Path $heraldDir) {
    Write-Host "Removing existing Herald directory..."
    Remove-Item -Recurse -Force $heraldDir
}

# Unzip the Herald archive
Write-Host "Unzipping Herald.zip to Herald directory..."
try {
    Expand-Archive -Path $heraldZip -DestinationPath $heraldDir
    Write-Host "Herald unzipped successfully."
} catch {
    Write-Host "Failed to unzip Herald.zip."
    exit 1
}

# Clean up zip file after extraction
Write-Host "Removing the downloaded Herald.zip..."
Remove-Item -Force $heraldZip

Write-Host "Herald update completed."
