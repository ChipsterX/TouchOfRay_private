@echo off

set startdir=%cd%
cd %~dp0

echo --- ags_sample ---
cd ..\ags_sample\premake
call :createvsfiles

echo --- crossfire_sample ---
cd ..\..\crossfire_sample\premake
call :createvsfiles

echo --- eyefinity_sample ---
cd ..\..\eyefinity_sample\premake
call :createvsfiles

echo --- dxut core ---
cd ..\dxut\Core
call :createdxutvsfiles
echo --- dxut optional ---
cd ..\Optional
call :createdxutvsfiles
cd ..\
:: we don't keep solution files for dxut
call :cleandxutslnfiles

cd %startdir%

pause
goto :EOF

:: run premake for vs2013, vs2015, and vs2017
:createvsfiles
..\..\premake\premake5.exe vs2013
..\..\premake\premake5.exe vs2015
..\..\premake\premake5.exe vs2017
goto :EOF

:: run premake for dxut
:createdxutvsfiles
..\..\..\premake\premake5.exe vs2013
..\..\..\premake\premake5.exe vs2015
..\..\..\premake\premake5.exe vs2017
goto :EOF

:: delete unnecessary sln files
:cleandxutslnfiles
del /f /q Core\DXUT_2013.sln
del /f /q Core\DXUT_2015.sln
del /f /q Core\DXUT_2017.sln

del /f /q Optional\DXUTOpt_2013.sln
del /f /q Optional\DXUTOpt_2015.sln
del /f /q Optional\DXUTOpt_2017.sln
goto :EOF
