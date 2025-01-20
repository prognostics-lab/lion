@ECHO off

IF /I "%1"=="all" GOTO all
IF /I "%1"=="help" GOTO help
IF /I "%1"=="clean" GOTO clean
IF /I "%1"=="configure" GOTO configure
IF /I "%1"=="build" GOTO build
IF /I "%1"=="install" GOTO install
IF /I "%1"=="" GOTO all
GOTO error

:all
    CALL make.bat build
    GOTO :EOF

:help
    ECHO Usage
    ECHO -----
    ECHO     make [command]
    ECHO[
    ECHO Commands
    ECHO --------
    ECHO     build   : Default, generates minimal configuration and builds the library
    ECHO     clean   : Removes all build files and targets
    ECHO     install : Requires sudo, installs the library files and headers
    GOTO :EOF

:clean
    ECHO Cleaning working directory
    if exist bin (
        RMDIR /s /q bin
    )
    if exist build (
        RMDIR /s /q build
    )
    if exist lib (
        RMDIR /s /q lib
    )
    if exist pdb (
        RMDIR /s /q pdb
    )
    if exist Testing (
        RMDIR /s /q Testing
    )
    if exist logs (
        RMDIR /s /q logs
    )
    if exist pysrc/lion/_lion.* (
        RMDIR /s /q pysrc/lion/_lion.*
    )
    GOTO :EOF

:configure
    ECHO Configuring
    if defined VCPKG_ROOT (
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake -A x64
    ) else (
        ECHO [ERROR] VCPKG_ROOT not set
    )
    GOTO :EOF

:build
    CALL make.bat configure
    ECHO Building
    cmake --build build --config Release
    GOTO :EOF

:install
    ECHO Installing

    ECHO Set UAC = CreateObject^("Shell.Application"^) > "%temp%getadmin.vbs"
    ECHO UAC.ShellExecute "cmake.exe", "--install build", "", "runas", 1 >> "%temp%getadmin.vbs"
    "%temp%getadmin.vbs"
    DEL "%temp%getadmin.vbs"

    GOTO :EOF

:error
    IF "%1"=="" (
        ECHO make: *** No targets specified and no makefile found.  Stop.
    ) ELSE (
        ECHO make: *** No rule to make target '%1%'. Stop.
    )
    GOTO :EOF
