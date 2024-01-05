@echo off

:: First check if docker is running
docker ps >NUL 2>NUL
if %errorlevel% neq 0 (
  echo.
  echo Docker engine is not running! Run Docker Desktop application
  echo.
  exit /b
)

:: Check if os_build exists and whether one should be created or started
docker container inspect os_build >NUL 2>NUL
if %errorlevel% neq 0 (
  echo Haven't found container named os_build
  echo Creating a new container from image yevhenii0/os_build:0.0.1. Dir %CD% will be mounted to /src
  docker run -i -d --name os_build -v %CD%:/src yevhenii0/os_build:0.0.1
) else (
  docker start os_build >NUL
)

:: Building kernel
echo Building kernel in the os_build container
docker exec os_build bash -l -c "cd /src && make"
if %errorlevel% neq 0 (
  echo.
  echo Compilation failed
  echo.
  exit /b
)

:: Check if qemu is available in PATH
where qemu-system-i386 >NUL 2>NUL
if %errorlevel% == 0 (
    :: Running kernel is qemu emulator
    qemu-system-i386 -kernel build/kernel.bin
    exit /b 0
)

:: Try to look into program files to see if it's in its default location
SET qemu_path_candidate=C:\Program Files\qemu\qemu-system-i386.exe

:: qemu-system-i386 -kernel build/kernel.bin
if exist "%qemu_path_candidate%" (
    :: Running kernel is qemu emulator
    "%qemu_path_candidate%" -kernel build/kernel.bin
) else (
    echo .
    echo "Cannot find qemu. Have you installed it?"
    echo .
    exit /b
)
