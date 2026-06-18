#!/usr/bin/env bash
set -e

if ! command -v glslc &> /dev/null; then
    echo "glslc not found. Make sure you've sourced the Vulkan SDK setup script (e.g. source ~/VulkanSDK/<version>/setup-env.sh) and try again."
    exit 1
fi

for shader in shaders/*.vert shaders/*.frag; do
    [ -e "$shader" ] || continue

    filename=$(basename "$shader")      # e.g. shader.vert
    stage="${filename##*.}"             # e.g. vert
    output="shaders/${stage}.spv"       # e.g. shaders/vert.spv

    echo "Compiling $shader -> $output"
    glslc "$shader" -o "$output"
done