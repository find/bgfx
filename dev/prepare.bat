@echo off

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
copy /y ..\.build\win32_vs2013\bin\bgfxDebug.lib lib\bgfx.lib
copy /y ..\.build\win32_vs2013\bin\example-commonDebug.lib lib\example-common.lib
copy /y ..\..\bx\tools\bin\windows\genie.exe tools\bin\
copy /y ..\.build\win32_vs2013\bin\shadercDebug.exe tools\bin\shaderc.exe
copy /y ..\.build\win32_vs2013\bin\texturecDebug.exe tools\bin\texturec.exe
copy /y ..\.build\win32_vs2013\bin\geometrycDebug.exe tools\bin\geometryc.exe
copy /y ..\.build\win32_vs2013\bin\makedisttexDebug.exe tools\bin\makedisttex.exe


