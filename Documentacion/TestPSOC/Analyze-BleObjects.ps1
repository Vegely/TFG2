<#
.SYNOPSIS
    Analyzes object files (.o) and outputs data separated by semicolons (;).
#>

# ==============================================================================
# 1. CONFIGURATION
# ==============================================================================
# Your PSoC / ARM Tool Path
$ToolPath = "C:\Program Files (x86)\Cypress\PSoC Creator\4.4\PSoC Creator\import\gnu\arm\5.4.1\bin\"
$Prefix   = "arm-none-eabi-"

# Output filenames
$ReportFile = "ble_report_semicolon.txt"
$CsvFile    = "ble_data_semicolon.csv"

# ==============================================================================
# 2. ANALYSIS LOGIC
# ==============================================================================
$nm   = "${ToolPath}${Prefix}nm.exe"
$size = "${ToolPath}${Prefix}size.exe"

if (-not (Test-Path $nm)) {
    Write-Host "ERROR: Tool not found at $nm" -ForegroundColor Red
    exit
}

$AllSymbols = @()
$TotalCode = 0
$TotalData = 0
$TotalBSS  = 0

$objectFiles = Get-ChildItem -Path . -Filter *.o -Recurse
Write-Host "Analyzing $($objectFiles.Count) files..." -ForegroundColor Cyan

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
$ReportContent += "BLE MEMORY ANALYSIS REPORT"
$ReportContent += "Generated: $(Get-Date)"
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
# This file contains EVERYTHING, not just the top 50
$AllSymbols | Sort-Object Size -Descending | Export-Csv -Path $CsvFile -Delimiter ";" -NoTypeInformation

Write-Host "Done!" -ForegroundColor Green
Write-Host "1. Summary (Top 50): $ReportFile"
Write-Host "2. Full Data (Excel): $CsvFile"