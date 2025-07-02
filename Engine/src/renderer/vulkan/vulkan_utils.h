#pragma once

#include "defines.h"
#include <vulkan/vulkan.h>
#include "core/spa_assert.h"


#define VK_CHECK(res) do {SPA_ASSERT(res == VK_SUCCESS);} while(false)

inline const char* vk_result_to_string(VkResult result);
bool setup_validation_layers(VkInstanceCreateInfo& create_info);
