REM call "$(SolutionDir)FrameWork\Scripts\CopyProjectLibsToDependencies.bat" $(SolutionDir) RapBot $(ProjectName) $(Configuration) $(TargetName)

echo off

set SolutionDir=%1
set Category=%2
set ProjectName=%3
set Configuration=%4
set TargetName=%5

set SourcePath="%SolutionDir%%Category%\%ProjectName%\Bin\%Configuration%"
set LibsPath="%SolutionDir%%Category%\%ProjectName%\Dependencies\Libs\%ProjectName%\%Configuration%"
set DllsPath="%SolutionDir%%Category%\%ProjectName%\Dependencies\DynamicLibs\%Configuration%"

@echo =====================================
@echo Copying %ProjectName% generated libs to the dependencies folder...
@echo Solution dir : %SolutionDir%
@echo Project category : %Category%
@echo Project name : %ProjectName%
@echo Configuration : %Configuration%
@echo Target name : %TargetName%
@echo .
@echo Source path : %SourcePath%
@echo Libs path : %LibsPath%
@echo Dlls path : %DllsPath%
@echo ....................................

if exist "%SourcePath%\%TargetName%.lib" (
    if not exist %LibsPath% ( 
        mkdir %LibsPath%
        @echo creating %LibsPath%
        )

    xcopy "%SourcePath%\%TargetName%.lib" %LibsPath% /y
)

if exist "%SourcePath%\%TargetName%.dll" (
    if not exist "%DllsPath%" ( 
        mkdir "%DllsPath%\"
        @echo creating %DllsPath%
        )

    xcopy "%SourcePath%\%TargetName%.dll" "%DllsPath%\" /y
)
