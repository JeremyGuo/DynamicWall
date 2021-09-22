//
// Created by JeremyGuo on 2021/9/21.
//

#ifndef DYNAMICWALL_WALLWINDOW_H
#define DYNAMICWALL_WALLWINDOW_H

#include <stb_image.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <windows.h>
#include <winuser.h>
#include <synchapi.h>
#include <thread>

#define RENDER_DLL_IMPORT
#include "render_dll.h"

typedef void(*RenderDraw)();
typedef void(*RenderInit)(GLFWwindow* window);
typedef void(*RenderDestroy)();

class WallWindow {
private:
    GLFWwindow* window;
    HINSTANCE handle;

    RenderDraw sub_render_draw;
    RenderInit sub_render_init;
    RenderDestroy sub_render_destroy;
public:
    WallWindow(GLFWmonitor* monitor, const std::string& render_dll_path, int least_refresh_rate, char level);
    ~WallWindow();
    void draw();
};



#endif //DYNAMICWALL_WALLWINDOW_H
