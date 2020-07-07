#include "trim.h"

#include <stdio.h>

#include <logger/log.h>
#include <glfw/glfw3.h>

#include "main.h"

void vkt_InitLogger()
{
    vkt_log_file = fopen("vk.log", "w");
    log_set_fp(vkt_log_file);
    log_set_level(LOG_TRACE);
}

void vkt_InitGLFW()
{
    glfwInit();
}

void vkt_CleanLogger()
{
    fclose(vkt_log_file);
}

void vkt_CleanGLFW()
{
    glfwTerminate();
}
