//
// Created by JeremyGuo on 2021/9/21.
//

#ifndef DYNAMICWALL_RENDER_DLL_H
#define DYNAMICWALL_RENDER_DLL_H

#define GLFW_EXPOSE_NATIVE_WIN32
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#ifdef RENDER_DLL_EXPORT
#define RENDER_DLL _declspec(dllexport)
#else
#ifdef RENDER_DLL_IMPORT
#define RENDER_DLL  _declspec(dllimport)
#else
#error You should specify use RENDER_DLL_EXPORT or RENDER_DLL_IMPORT
#endif
#endif

extern "C" RENDER_DLL void render_draw();
extern "C" RENDER_DLL void render_load(GLFWwindow* window);
extern "C" RENDER_DLL void render_destroy();

#endif //DYNAMICWALL_RENDER_DLL_H
