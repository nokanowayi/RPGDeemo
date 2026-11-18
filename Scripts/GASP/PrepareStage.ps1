param([string]$SourceProject = 'C:/Users/wayi/Documents/Unreal Projects/游戏动画示例/游戏动画示例.uproject')
$ErrorActionPreference = 'Stop'
$projectRoot = (Resolve-Path "$PSScriptRoot/../..").Path
$reportRoot = Join-Path $projectRoot 'Saved/Codex/GASPRetarget'
$stageRoot = Join-Path $reportRoot 'Stage'
if (Test-Path -LiteralPath $stageRoot) { throw "Staging directory exists: $stageRoot. Use the existing stage or choose a new report directory; never overwrite blindly." }
$sourceRoot = Split-Path $SourceProject
$inventory = Get-Content (Join-Path $reportRoot 'source_inventory.json') -Raw | ConvertFrom-Json
$targetDeps = Get-Content (Join-Path $reportRoot 'target_dependencies.json') -Raw | ConvertFrom-Json
New-Item -ItemType Directory -Path (Join-Path $stageRoot 'Content') -Force | Out-Null
$descriptor = Get-Content -LiteralPath $SourceProject -Raw | ConvertFrom-Json
$descriptor | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath (Join-Path $stageRoot 'GASPStage.uproject') -Encoding utf8
Copy-Item -LiteralPath (Join-Path $sourceRoot 'Config') -Destination (Join-Path $stageRoot 'Config') -Recurse
$copied = @()
foreach ($entry in $inventory.dependencies) {
    if (!$entry.bytes) { continue }
    $relative = $entry.package.Substring('/Game/'.Length)
    $destBase = Join-Path $stageRoot "Content/$relative"
    New-Item -ItemType Directory -Path (Split-Path $destBase) -Force | Out-Null
    foreach ($extension in @('.uasset', '.ubulk', '.uexp')) {
        $sourceFile = Join-Path $sourceRoot "Content/$relative$extension"
        if (Test-Path -LiteralPath $sourceFile) {
            Copy-Item -LiteralPath $sourceFile -Destination "$destBase$extension"
            $copied += $sourceFile
        }
    }
}
foreach ($package in $targetDeps.packages) {
    $relative = $package.Substring('/Game/'.Length)
    $destBase = Join-Path $stageRoot "Content/$relative"
    if (Test-Path -LiteralPath "$destBase.uasset") { throw "Source/target staging collision: $package" }
    New-Item -ItemType Directory -Path (Split-Path $destBase) -Force | Out-Null
    foreach ($extension in @('.uasset', '.ubulk', '.uexp')) {
        $sourceFile = Join-Path $projectRoot "Content/$relative$extension"
        if (Test-Path -LiteralPath $sourceFile) {
            Copy-Item -LiteralPath $sourceFile -Destination "$destBase$extension"
            $copied += $sourceFile
        }
    }
}
@{stage = $stageRoot; sourceProject = $SourceProject; copiedFiles = $copied.Count} |
    ConvertTo-Json | Set-Content (Join-Path $reportRoot 'stage_manifest.json') -Encoding utf8
Write-Output "Prepared $($copied.Count) isolated staging files in $stageRoot. Live projects were not changed."
