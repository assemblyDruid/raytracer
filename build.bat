@echo off
@SET SCRIPT_DIR=%cd%
@SET APP_NAME="rtx"
@SET APP_ARCH=x64
@SET APP_SDK_VER=10.0.18362.0

::
:: Initialize cl.exe for correct environment.
::

@where cl >nul 2>nul
:: VS 2017 Community Edition
rem IF %ERRORLEVEL% NEQ 0 call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall" %APP_ARCH% >nul

:: VS 2017 Professional Edition
IF %ERRORLEVEL% NEQ 0 call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsall" %APP_ARCH% >nul

:: VS 2017 Enterprise Edition
rem IF %ERRORLEVEL% NEQ 0 call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvarsall" %APP_ARCH% >nul

:: VS 2019 Community Edition
rem IF %ERRORLEVEL% NEQ 0 call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64 >nul

:: VS 2019 Enterprise Edition
rem IF %ERRORLEVEL% NEQ 0 call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" %APP_ARCH% >nul


:: Fix on some development machines with complicated VS SDK setups
rem SET ERRORLEVEL=0
rem SET "APP_LIB_UM_PATH=C:\Program Files (x86)\Windows Kits\10\Lib\%APP_SDK_VER%\um\%APP_ARCH%"
rem SET "APP_LIB_UCRT_PATH=C:\Program Files (x86)\Windows Kits\10\Lib\%APP_SDK_VER%\ucrt\%APP_ARCH%"
rem SET "APP_INC_UM_PATH=C:\Program Files (x86)\Windows Kits\10\Include\%APP_SDK_VER%\um"
rem SET "APP_INC_UCRT_PATH=C:\Program Files (x86)\Windows Kits\10\Include\%APP_SDK_VER%\ucrt"

rem IF NOT EXIST "%APP_LIB_UM_PATH%" (
rem    @echo [ error ] Invalid LIB_UM path: "%APP_LIB_UM_PATH%"
rem    rem GOTO :exit
rem )

rem IF NOT EXIST "%APP_LIB_UCRT_PATH%" (
rem    @echo [ error ] Invalid LIB_UCRT path: "%APP_LIB_UCRT_PATH%"
rem    rem GOTO :exit
rem )

rem IF NOT EXIST "%APP_INC_UM_PATH%" (
rem    @echo [ error ] Invalid INC_UM path: "%APP_INC_UM_PATH%"
rem    rem GOTO :exit
rem )

rem IF NOT EXIST "%APP_INC_UCRT_PATH%" (
rem    @echo [ error ] Invalid INC_UCRT path: "%APP_INC_UM_PATH%"
rem    rem GOTO :exit
rem )

::
:: Compile & Link
::

:: /TC                  Compile as C code.
:: /TP                  Compile as C++ code.
:: /Oi                  Enable intrinsic functions.
:: /Od 	                Disables optimization.
:: /Qpar                Enable parallel code generation.
:: /Ot                  Favor fast code (over small code).
:: /Ob2                 Enable full inline expansion. [ cfarvin::NOTE ] Debugging impact.
:: /Z7	                Full symbolic debug info. No pdb. (See /Zi, /Zl).
:: /GS	                Detect buffer overruns.
:: /MD	                Multi-thread specific, DLL-specific runtime lib. (See /MDd, /MT, /MTd, /LD, /LDd).
:: /GL	                Whole program optimization.
:: /EHsc                No exception handling (Unwind semantics requrie vstudio env). (See /W1).
:: /I<arg>              Specify include directory.
:: /link                Invoke microsoft linker options.
:: /NXCOMPAT            Comply with Windows Data Execution Prevention.
:: /MACHINE:<arg>       Declare machine arch (should match vcvarsall env setting).
:: /NODEFAULTLIB:<arg>  Ignore a library.
:: /LIBPATH:<arg>       Specify library directory/directories.
IF %ERRORLEVEL% NEQ 0 GOTO :exit
mkdir msvc_landfill >nul 2>nul
pushd msvc_landfill >nul
cl %SCRIPT_DIR%\\%APP_NAME%.c /TC /Oi /W4 /WX /02 /GL /MD /EHsc /nologo ^
/I"%SCRIPT_DIR%" ^
/link /SUBSYSTEM:CONSOLE /NXCOMPAT /MACHINE:x64 /NODEFAULTLIB:MSVCRTD ^
User32.Lib shell32.lib odbccp32.lib && ^
xcopy /y %APP_NAME%.exe ..\ >null && ^
popd >null && ^
rtx && ^
start rtx.bmp

:exit
rem /I"%APP_INC_UM_PATH%" /I"%APP_INC_UCRT_PATH%" ^
rem /LIBPATH:"%APP_LIB_UM_PATH%" ^
rem /LIBPATH:"%APP_LIB_UCRT_PATH%" ^
