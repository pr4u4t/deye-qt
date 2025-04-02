param (
    [string]$InstanceName,
    [string]$InputFile = "input.yaml"
)

# Convert instance name to lowercase and replace spaces with underscores
$ProcessedInstance = $InstanceName.ToLower() -replace ' ', '_'

# Read the input file
$Content = Get-Content -Raw -Path $InputFile

# Replace {{instance}} with the processed instance name
$UpdatedContent = $Content -replace "{{instance}}", $ProcessedInstance

# Output to stdout
Write-Output $UpdatedContent
