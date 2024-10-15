oscar64 .\orbix.cpp  -n -Os -tf=crt
IF errorlevel 1 (ECHO "Error") ELSE (x64sc.exe .\orbix.crt)

