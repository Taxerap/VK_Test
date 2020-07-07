#include "vk.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <logger/log.h>
#include <glfw/glfw3.h>

#include "../main.h"
#include "../window/window.h"

VkInstance                  vkt_vulkan_instance;
VkDebugUtilsMessengerEXT    vkt_vulkan_debug_messenger;
VkSurfaceKHR                vkt_vulkan_surface;
VkPhysicalDevice            vkt_vulkan_physical_device;
VkDevice                    vkt_vulkan_device;
VkQueue                     vkt_vulkan_graphics_queue;

const char *                vkt_VALIDATION_LAYERS[] = 
{
    "VK_LAYER_KHRONOS_validation"
};
const uint32_t              vkt_VALIDATION_LAYERS_COUNT = 1;

const char **vkt_REAL_EXT;

static bool vkt_CheckValidationLayerSupport()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    VkLayerProperties *available_layers = malloc(layer_count * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

    for (int i = 0; i < vkt_VALIDATION_LAYERS_COUNT; i++)
    {
        bool layer_found = false;

        for (int j = 0; j < layer_count; j++)
            if (strcmp(vkt_VALIDATION_LAYERS[i], available_layers[j].layerName) == 0)
            {
                layer_found = true;
                break;
            }

        if (!layer_found)
        {
            free(available_layers);
            return false;
        }
    }

    free(available_layers);
    return true;
}

static void vkt_CreateVKInstance()
{
    if (!vkt_CheckValidationLayerSupport())
    {
        log_error("Failed to request validation layers.");
        abort();
    }

    VkApplicationInfo app_inf =
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = vkt_PROG_NAME,
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "none",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_1
    };

    uint32_t glfw_extension_count = 0;
    const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    const char **real_extensions = malloc((1 + glfw_extension_count) * sizeof(char *));
    memcpy(real_extensions, glfw_extensions, glfw_extension_count * sizeof(char *));
    real_extensions[glfw_extension_count] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    vkt_REAL_EXT = real_extensions;

    VkInstanceCreateInfo cre_inf =
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_inf,
        .enabledExtensionCount = glfw_extension_count + 1,
        .ppEnabledExtensionNames = real_extensions,
        .enabledLayerCount = vkt_VALIDATION_LAYERS_COUNT,
        .ppEnabledLayerNames = vkt_VALIDATION_LAYERS
    };

    VkResult result = vkCreateInstance(&cre_inf, NULL, &vkt_vulkan_instance);
    if (result != VK_SUCCESS)
    {
        log_error("Failed to initialize Vulkan.");
        abort();
    }

    log_info("Created a vulkan instance.");

    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
    log_info("%" PRIu32 " vulkan extensions supported:\n", extension_count);

    VkExtensionProperties *extensions = malloc(extension_count * sizeof(VkExtensionProperties));
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions);

    for (int i = 0; i < extension_count; i++)
        log_info("%s%s", extensions[i].extensionName, (i == extension_count - 1) ? "\n" : "");

    free(extensions);
}

static VkResult vkt_CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *create_info,
    VkAllocationCallbacks *allocator,
    VkDebugUtilsMessengerEXT *debug_mes)
{
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func)
        return func(instance, create_info, allocator, debug_mes);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void vkt_DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debug_mes,
    const VkAllocationCallbacks *allocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func)
        func(instance, debug_mes, allocator);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL vkt_DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *user_data)
{
    log_debug("Validation layer: %s\n", callback_data->pMessage);

    return VK_FALSE;
}

static void vkt_SetupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT create_inf =
    {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = &vkt_DebugCallback
    };

    if (vkt_CreateDebugUtilsMessengerEXT(vkt_vulkan_instance, &create_inf, NULL, &vkt_vulkan_debug_messenger) != VK_SUCCESS)
    {
        log_error("Failed to set up debug messenger!");
        abort();
    }

    log_info("Created a debug messenger.");
}

static void vkt_CreateSurface()
{
    if (glfwCreateWindowSurface(vkt_vulkan_instance, vkt_window, NULL, &vkt_vulkan_surface) != VK_SUCCESS)
    {
        log_error("Failed to create a vulkan surface!");
        abort();
    }

    log_info("Created a vulkan surface.");
}

static bool vkt_IsDeviceSuitable(VkPhysicalDevice device)
{
    // Just assume it's suitable...
    return true;
}

static void vkt_PickPhysicalDevice()
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(vkt_vulkan_instance, &device_count, NULL);

    if (!device_count)
    {
        log_error("Failed to find GPUs with Vulkan support!");
        abort();
    }

    VkPhysicalDevice *devices = calloc(device_count, sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(vkt_vulkan_instance, &device_count, devices);
    log_info("There are total of %" PRIu32 " physical devices.", device_count);

    for (int i = 0; i < device_count; i++)
        if (vkt_IsDeviceSuitable(devices[i]))
        {
            vkt_vulkan_physical_device = devices[i];
            break;
        }

    if (vkt_vulkan_physical_device == VK_NULL_HANDLE)
    {
        log_error("Failed to find a suitable GPU!");
        free(devices);
        abort();
    }

    free(devices);
    log_info("Found a suitable GPU.");
}

static void vkt_CreateLogicalDevice()
{
    vkt_QueueFamilyIndices indices = vkt_FindQueueFamilies(vkt_vulkan_physical_device);

    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info =
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = indices.graphics_family,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority
    };

    VkPhysicalDeviceFeatures device_features = {};

    VkDeviceCreateInfo device_create_info =
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = &queue_create_info,
        .queueCreateInfoCount = 1,
        .pEnabledFeatures = &device_features
    };

    if(vkCreateDevice(vkt_vulkan_physical_device, &device_create_info, NULL, &vkt_vulkan_device) != VK_SUCCESS)
    {
        log_error("Failed to create logical device!");
        abort();
    }

    vkGetDeviceQueue(vkt_vulkan_device, indices.graphics_family, 0, &vkt_vulkan_graphics_queue);
    log_info("Created a logical device.");
}

vkt_QueueFamilyIndices vkt_FindQueueFamilies(VkPhysicalDevice device)
{
    vkt_QueueFamilyIndices indices = {};

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkt_vulkan_physical_device, &queue_family_count, NULL);

    VkQueueFamilyProperties *queue_families = malloc(queue_family_count * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    int j = 0;
    for (int i = 0; i < queue_family_count; i++)
    {
        if(queue_families[i].queueCount > 0 && queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphics_family = j;

        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, j, vkt_vulkan_surface, &present_support);

        if (queue_families[i].queueCount > 0 && present_support)
            indices.present_family = j;

        j++;
    }

    free(queue_families);
    return indices;
}

void vkt_InitVK()
{
    vkt_CreateVKInstance();
    vkt_CreateSurface();
    vkt_SetupDebugMessenger();
    vkt_PickPhysicalDevice();
    vkt_CreateLogicalDevice();
}

void vkt_CleanVK()
{
    vkDestroyDevice(vkt_vulkan_device, NULL);
    log_info("Destryoed a logical device.");

    vkt_DestroyDebugUtilsMessengerEXT(vkt_vulkan_instance, vkt_vulkan_debug_messenger, NULL);
    log_info("Destroyed a debug messenger.");

    vkDestroySurfaceKHR(vkt_vulkan_instance, vkt_vulkan_surface, NULL);
    log_info("Destroyed a vulkan surface.");

    vkDestroyInstance(vkt_vulkan_instance, NULL);
    free(vkt_REAL_EXT);
    log_info("Destroyed a vulkan instance.");
}
