# Define the input and output file paths
$inputFilePath = "..\depeg\resources\title_logo_data.bin"
$outputFilePath = "..\depeg\resources\logo_data.bin"

# Define the start and end offsets
$startOffset = 0x500
$endOffset = 0xb40

# Read the input file as a byte array
$fileBytes = [System.IO.File]::ReadAllBytes($inputFilePath)

# Extract the specified range of bytes
$byteRange = $fileBytes[$startOffset..$endOffset]

# Write the extracted bytes to the output file
[System.IO.File]::WriteAllBytes($outputFilePath, $byteRange)

Write-Output "Bytes from 0x500 to 0xb40 have been written to $outputFilePath"