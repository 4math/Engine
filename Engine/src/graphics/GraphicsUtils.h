#pragma once

#include "vulkan/vulkan.h"
#include <string>
#include <vector>
#include <iostream>

std::vector<VkExtensionProperties> ListInstanceExtensions(bool print_);
std::vector<VkPhysicalDevice> ListPhysicalDevices(VkInstance instance_);