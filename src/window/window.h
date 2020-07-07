#ifndef _VK_TEST_WINDOW_H_
#define _VK_TEST_WINDOW_H_

#include <glfw/glfw3.h>

extern GLFWwindow *vkt_window;
extern const int vkt_WIN_WIDTH;
extern const int vkt_WIN_HEIGHT;

void vkt_InitWindow();
void vkt_CleanWindow();

#endif
