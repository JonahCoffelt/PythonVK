#ifndef KATRA_INCLUDES_H
#define KATRA_INCLUDES_H

#include <vulkan/vulkan.h>

// STLIB
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <optional>
#include <set>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <fstream>
#include <chrono>

// GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// STB (declarations only — implementation lives in src/util/stb_impl.cpp)
#include <katra/util/stb_impl.h>

// Tiny Obj (declarations only — implementation lives in src/util/tinyobjloader_impl.cpp)
#include <katra/util/tinyobjloader_impl.h>

#endif