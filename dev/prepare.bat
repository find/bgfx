@echo off
if "%1"=="" (
    echo usage: %0 compiler config
    echo    eg: %0 win32_vs2013 Debug
    goto badend
)

if "%2"=="" (
    set config=Debug
) else (
    set config=%2
)

if not exist inc md inc
if not exist assets md assets
if not exist assets\shaders md assets\shaders
if not exist assets\font md assets\font
if not exist helpers md helpers
if not exist lib md lib
if not exist tools md tools
if not exist tools\bin md tools\bin
if not exist make md make
:: if not exists src md src

copy /y ..\include\* inc\
copy /y ..\src\*.sh assets\shaders\
copy /y ..\examples\common\*.sh assets\shaders\
copy /y ..\examples\runtime\font\droidsans.ttf assets\font\
copy /y ..\examples\runtime\font\roboto* assets\font\
copy /y ..\examples\runtime\font\visitor1.ttf assets\font\
copy /y ..\scripts\*.mk make\
xcopy /y /e /q ..\examples\common\* helpers
copy /y ..\.build\%1\bin\bgfx%config%.lib lib\bgfx.lib
copy /y ..\.build\%1\bin\example-common%config%.lib lib\example-common.lib
copy /y ..\..\bx\tools\bin\windows\genie.exe tools\bin\
copy /y ..\.build\%1\bin\shaderc%config%.exe tools\bin\shaderc.exe
copy /y ..\.build\%1\bin\texturec%config%.exe tools\bin\texturec.exe
copy /y ..\.build\%1\bin\geometryc%config%.exe tools\bin\geometryc.exe
copy /y ..\.build\%1\bin\makedisttex%config%.exe tools\bin\makedisttex.exe

goto goodend

:badend

echo oops

:goodend
