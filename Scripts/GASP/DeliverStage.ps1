$ErrorActionPreference = 'Stop'
$projectRoot = (Resolve-Path "$PSScriptRoot/../..").Path
$reportRoot = Join-Path $projectRoot 'Saved/Codex/GASPRetarget'
$manifest = Get-Content (Join-Path $reportRoot 'stage_export_manifest.json') -Raw | ConvertFrom-Json
if (!$manifest.reload_passed) { throw 'Fresh-process asset reload validation did not pass.' }
$existing = @((Get-Content (Join-Path $reportRoot 'target_dependencies.json') -Raw | ConvertFrom-Json).packages)
$copyFiles = @()
foreach ($entry in $manifest.files) {
    $relative = $entry.package.Substring('/Game/'.Length)
    $destFile = Join-Path $projectRoot "Content/$relative.uasset"
    if ($existing -contains $entry.package) {
        if (!(Test-Path -LiteralPath $destFile)) { throw "Expected existing target package is missing: $destFile" }
        continue
    }
    if (Test-Path -LiteralPath $destFile) { throw "Refusing to overwrite existing project package: $destFile" }
    if ((Get-FileHash -LiteralPath $entry.file -Algorithm SHA256).Hash.ToLowerInvariant() -ne $entry.sha256) {
        throw "Staging file hash mismatch: $($entry.file)"
    }
    $copyFiles += [pscustomobject]@{source=$entry.file;destination=$destFile;package=$entry.package;sha256=$entry.sha256;bytes=$entry.bytes}
}
# All collisions and checksums are validated before the first project write.
foreach ($entry in $copyFiles) {
    New-Item -ItemType Directory -Path (Split-Path $entry.destination) -Force | Out-Null
    Copy-Item -LiteralPath $entry.source -Destination $entry.destination
}
@{createdFiles=$copyFiles;createdPackageCount=$copyFiles.Count;existingPackagesPreserved=$existing.Count;sequences=330;montages=34} |
    ConvertTo-Json -Depth 10 | Set-Content (Join-Path $reportRoot 'delivery_manifest.json') -Encoding utf8
"Delivered $($copyFiles.Count) new packages; preserved $($existing.Count) existing target packages."
