@echo off
SET /A "index = 1"
SET /A "count = 20"

:while
if %index% leq %count% (
     START /MIN Client.exe
     SET /A index = %index% + 1
     @echo %index%
     goto :while
)

