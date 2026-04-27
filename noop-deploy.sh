: <<'BATCH'
@echo off
setlocal

:: Define file names
set "SOURCE_FILE=noop.c"
set "EXE_NAME=noop.exe"
set "TARGET_PATH=.\%EXE_NAME%"

echo Creating source file...
echo int main(void) { return 0; } > %SOURCE_FILE%

:: Check if cl.exe is in the path
where cl.exe >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] cl.exe (MSVC Compiler) not found in PATH.
    echo Please run this from a Developer Command Prompt for Visual Studio.
    del %SOURCE_FILE%
    exit /b 1
)

echo Building with cl...
:: /O2: Optimization for speed
:: /Fe: Rename output executable
:: /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup : Prevents console window popup
cl.exe /O2 %SOURCE_FILE% /Fe:"%TARGET_PATH%" /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup >nul

if %ERRORLEVEL% equ 0 (
    echo Success! Binary deployed to: %TARGET_PATH%

    :: Clean up temporary build files
    del %SOURCE_FILE%
    del noop.obj
) else (
    echo [ERROR] Compilation failed.
    del %SOURCE_FILE%
    exit /b 1
)

echo Testing binary...
start /wait "" "%TARGET_PATH%"
echo Test complete. (Exited with code %ERRORLEVEL%)

endlocal
BATCH

#!/bin/bash

# Define the source content
SOURCE_CODE="int main(void) { return 0; }"

# Determine the correct temp directory
# Windows bash usually uses $TEMP, Linux/macOS usually use $TMPDIR
TARGET_DIR="."
FILE_NAME="noop"

# Add .exe extension if we are on Windows
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
    FILE_NAME="noop.exe"
fi

TARGET_PATH="$TARGET_DIR/$FILE_NAME"

echo "Creating source file..."
echo "$SOURCE_CODE" > noop.c

echo "Building with clang..."
if clang -O3 noop.c -o "$TARGET_PATH"; then
    echo "Success! Binary deployed to: $TARGET_PATH"

    # Clean up the source file
    rm noop.c
else
    echo "Error: Compilation failed."
    exit 1
fi

# Verify exit code
echo "Testing binary..."
"$TARGET_PATH"
if [ $? -eq 0 ]; then
    echo "Test passed: Binary exited with code 0."
else
    echo "Test failed: Binary returned non-zero exit code."
fi
