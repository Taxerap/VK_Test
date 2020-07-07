#ifndef _VK_TEST_VK_H_
#define _VK_TEST_VK_H_

#include <inttypes.h>

#include <vulkan/vulkan.h>

extern VkInstance                  vkt_vulkan_instance;
extern VkDebugUtilsMessengerEXT    vkt_vulkan_debug_messenger;
extern VkSurfaceKHR                vkt_vulkan_surface;
extern VkPhysicalDevice            vkt_vulkan_physical_device;
extern VkDevice                    vkt_vulkan_device;
extern VkQueue                     vkt_vulkan_graphics_queue;

extern const char *vkt_VALIDATION_LAYERS[];
extern const uint32_t vkt_VALIDATION_LAYERS_COUNT;
extern const char **vkt_REAL_EXT;

typedef struct vkt_QueueFamilyIndices_t
{
    uint32_t graphics_family;
    uint32_t present_family;
} vkt_QueueFamilyIndices;

vkt_QueueFamilyIndices vkt_FindQueueFamilies(VkPhysicalDevice);

void vkt_InitVK();
void vkt_CleanVK();

#endif
