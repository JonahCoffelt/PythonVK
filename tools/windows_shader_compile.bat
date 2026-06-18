@echo off
setlocal enabledelayedexpansion

where glslc >nul 2>nul
if errorlevel 1 (
    echo glslc not found. Make sure you've installed the Vulkan SDK and that
    echo its bin directory ^(e.g. C:\VulkanSDK\^<version^>\Bin^) is on your PATH,
    echo then restart your terminal and try again.
    exit /b 1
)

for %%f in (shaders\*.vert shaders\*.frag) do (
    echo Compiling %%f -^> %%f.spv
    glslc "%%f" -o "%%f.spv"
    if errorlevel 1 exit /b 1
)

endlocal