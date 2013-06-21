@ECHO OFF
FOR %%i IN (%*) DO (
    ECHO %%i
    gcj2wgs.exe %%i
)
pause
