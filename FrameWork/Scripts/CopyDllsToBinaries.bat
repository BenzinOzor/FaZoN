REM call "$(SolutionDir)FrameWork\Scripts\CopyDllsToBinaries.bat" $(SolutionDir) FrameWork FrameWork Samples $(ProjectName) $(Configuration)

echo off

set SolutionDir=%1
set SrcCategory=%2
set SrcProjectName=%3
set DstCategory=%4
set DstProjectName=%5
set Configuration=%6

set SrcPath="%SolutionDir%%SrcCategory%\%SrcProjectName%\Dependencies\DynamicLibs\%Configuration%"
set DstPath="%SolutionDir%%DstCategory%\%DstProjectName%\Bin\%Configuration%"

@echo =====================================
@echo Copying dynamic libs from %SrcProjectName% dependencies to %DstProjectName% binaries...
@echo Solution dir : %SolutionDir%
@echo Source project category : %SrcCategory%
@echo Source project name : %SrcProjectName%
@echo Destination project category : %DstCategory%
@echo Destination project name : %DstProjectName%
@echo Configuration : %Configuration%
@echo .
@echo Source path : %SrcPath%
@echo Destination path : %DstPath%
@echo ....................................

if not exist "%DstPath%" (
    mkdir "%DstPath%"
    @echo Creating %DstPath% ...
)

xcopy "%SrcPath%\*.*" "%DstPath%" /y
