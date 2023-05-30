REM call "$(SolutionDir)FrameWork\Scripts\CopyProjectLibrariesToCurrentProject.bat" $(SolutionDir) Games TSOIRebirth Utilities SoIREditor $(Configuration) TSOIRebirth

echo off

set SolutionDir=%1
set SrcCategory=%2
set SrcProjectName=%3
set DstCategory=%4
set DstProjectName=%5
set Configuration=%6
set TargetName=%7
set CustomOutputDir=%8

set SourcePath="%SolutionDir%%SrcCategory%\%SrcProjectName%\Bin\%Configuration%"
set LibPath="%SolutionDir%%DstCategory%\%DstProjectName%\Dependencies\Libs\%SrcProjectName%\%Configuration%"
set DllPath="%SolutionDir%%DstCategory%\%DstProjectName%\Bin\%Configuration%"

if not "%CustomOutputDir%" == "" (
    set SourcePath="%SolutionDir%%SrcCategory%\%SrcProjectName%\Bin\%CustomOutputDir%"
)

@echo =====================================
@echo Copying %SrcProjectName% generated libs to %DstProjectName% dependencies and bin folder...
@echo Solution dir : %SolutionDir%
@echo Source project category : %SrcCategory%
@echo Source project name : %SrcProjectName%
@echo Destination project category : %DstCategory%
@echo Destination project name : %DstProjectName%
@echo Configuration : %Configuration%
@echo Target name : %TargetName%
@echo .
@echo Source path : %SourcePath%
@echo Lib path : %LibPath%
@echo Dll path : %DllPath%
@echo ....................................

if not exist "%SourcePath%" (
    @echo SOURCE PATH %SourcePath% DOESN'T EXIST!
) else (

    if exist "%SourcePath%\%TargetName%.lib" (
        if not exist %LibPath% ( 
            mkdir %LibPath%
            @echo creating %LibPath%
            )
    
        xcopy "%SourcePath%\%TargetName%.lib" %LibPath% /y
    ) else (
        @echo %SourcePath%\%TargetName%.lib NOT FOUND!
    )
    
    if exist "%SourcePath%\%TargetName%.dll" (
        if not exist "%DllPath%" ( 
            mkdir "%DllPath%\"
            @echo creating %DllPath%
            )
    
        xcopy "%SourcePath%\%TargetName%.dll" "%DllPath%\" /y
    ) else (
        @echo %SourcePath%\%TargetName%.dll NOT FOUND!
    )

)
