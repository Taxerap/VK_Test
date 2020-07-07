#include "main.h"

#include <stdlib.h>
#include <stdio.h>

#include "trim.h"
#include "window/window.h"
#include "vk/vk.h"

const char *vkt_PROG_NAME = "vk";

FILE *vkt_log_file;

int main(int argc, char *argv[])
{
    vkt_InitLogger();
    vkt_InitGLFW();
    vkt_InitWindow();
    vkt_InitVK();

    while (!glfwWindowShouldClose(vkt_window))
    {
        glfwPollEvents();
    }

    vkt_CleanVK();
    vkt_CleanWindow();
    vkt_CleanGLFW();
    vkt_CleanLogger();

    return EXIT_SUCCESS;
}
