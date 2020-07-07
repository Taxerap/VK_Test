#include "window.h"

#include "../main.h"
#include <logger/log.h>

GLFWwindow *vkt_window;
const int vkt_WIN_WIDTH = 1366;
const int vkt_WIN_HEIGHT = 768;

void vkt_InitWindow()
{
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    vkt_window = glfwCreateWindow(vkt_WIN_WIDTH, vkt_WIN_HEIGHT, vkt_PROG_NAME, NULL, NULL);
    glfwShowWindow(vkt_window);

    log_info("Created a window.");
}

void vkt_CleanWindow()
{
    glfwDestroyWindow(vkt_window);

    log_info("Destroyed a window.");
}
