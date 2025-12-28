<#
.SYNOPSIS
    Analyzes object files (.o) and outputs data separated by semicolons (;).
#>

# ==============================================================================
# 1. CONFIGURATION
# ==============================================================================

# --- TARGET DIRECTORY (Updated) ---
# We explicitly set the path you requested here:
$TargetDirectory = "C:\Users\bogurad\Desktop\ProyectosgitHub\TFG2\Codigo\Algoritmospostcuanticos\Psoc_modus\HQC\build\Release\local\src"

# --- TOOL PATHS ---
# CHECK THIS: If you use ModusToolbox, this path might be different (e.g., C:\ModusToolbox\tools_3.1\gcc\bin)
$ToolPath = "C:\Program Files (x86)\Cypress\PSoC Creator\4.4\PSoC Creator\import\gnu\arm\5.4.1\bin\"
$Prefix   = "arm-none-eabi-"

# Output filenames (These will be created where you save this script)
$ReportFile = "ble_report_semicolon.txt"
$CsvFile    = "ble_data_semicolon.csv"

# ==============================================================================
# 2. ANALYSIS LOGIC
# ==============================================================================
$nm   = "${ToolPath}${Prefix}nm.exe"
$size = "${ToolPath}${Prefix}size.exe"

# Check if tools exist
if (-not (Test-Path $nm)) {
    Write-Host "ERROR: Tool not found at $nm" -ForegroundColor Red
    Write-Host "Please check the `$ToolPath variable in the script." -ForegroundColor Yellow
    exit
}

# Check if target directory exists
if (-not (Test-Path $TargetDirectory)) {
    Write-Host "ERROR: Target directory not found: $TargetDirectory" -ForegroundColor Red
    exit
}

$AllSymbols = @()
$TotalCode = 0
$TotalData = 0
$TotalBSS  = 0

# Updated to look in $TargetDirectory instead of "."
$objectFiles = Get-ChildItem -Path $TargetDirectory -Filter *.o -Recurse
Write-Host "Analyzing $($objectFiles.Count) files in target directory..." -ForegroundColor Cyan

foreach ($file in $objectFiles) {
    # 1. Get Section Sizes
    try {
        $sizeOutput = & $size -A $file.FullName
        $sizeOutput | ForEach-Object {
            if ($_ -match "\.text\s+(\d+)") { $TotalCode += [int]$Matches[1] }
            if ($_ -match "\.data\s+(\d+)") { $TotalData += [int]$Matches[1] }
            if ($_ -match "\.bss\s+(\d+)")  { $TotalBSS  += [int]$Matches[1] }
        }
    } catch {}

    # 2. Get Symbols
    try {
        $rawSymbols = & $nm -C --print-size --size-sort --radix=d -r $file.FullName
        foreach ($line in $rawSymbols) {
            if ($line -match "^\s*(\d+)\s+(\d+)\s+([a-zA-Z])\s+(.+)$") {
                $TypeChar = $Matches[3].ToUpper()
                switch ($TypeChar) {
                    "T" { $TypeName = "Code" }
                    "D" { $TypeName = "Data" }
                    "B" { $TypeName = "Zero" }
                    "R" { $TypeName = "ReadOnly" }
                    Default { $TypeName = $TypeChar }
                }
                 
                $AllSymbols += [PSCustomObject]@{
                    Size    = [int]$Matches[2]
                    Type    = $TypeName
                    Name    = $Matches[4]
                    File    = $file.Name
                    Address = $Matches[1]
                }
            }
        }
    } catch {}
}

# ==============================================================================
# 3. GENERATE SEMICOLON REPORT
# ==============================================================================
Write-Host "Generating reports..." -ForegroundColor Yellow

# --- Report Content ---
$ReportContent = @()
$ReportContent += "MEMORY ANALYSIS REPORT"
$ReportContent += "Generated: $(Get-Date)"
$ReportContent += "Target: $TargetDirectory"
$ReportContent += "--------------------------------------------------"
$ReportContent += "TOTAL SUMMARY"
$ReportContent += "Category;Size(Bytes)"
$ReportContent += "Code (Flash);$TotalCode"
$ReportContent += "Data (RAM);$TotalData"
$ReportContent += "BSS (RAM);$TotalBSS"
$ReportContent += ""
$ReportContent += "--------------------------------------------------"
$ReportContent += "TOP 50 LARGEST SYMBOLS (Separated by ;)"
$ReportContent += "--------------------------------------------------"
$ReportContent += "Size;Type;Name;File"  # The Header

# Add top 50 rows formatted with semicolons
$AllSymbols | Sort-Object Size -Descending | Select-Object -First 50 | ForEach-Object {
    $ReportContent += "$($_.Size);$($_.Type);$($_.Name);$($_.File)"
}

$ReportContent | Out-File -FilePath $ReportFile -Encoding UTF8

# --- Generate Full CSV (Semicolon Delimited) ---
$AllSymbols | Sort-Object Size -Descending | Export-Csv -Path $CsvFile -Delimiter ";" -NoTypeInformation

Write-Host "Done!" -ForegroundColor Green
Write-Host "1. Summary (Top 50): $ReportFile"
Write-Host "2. Full Data (Excel): $CsvFile"