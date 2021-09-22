# DynamicWall
Dynamic Wall Paper written with win32API and glfw

## WallWindow

WallWindow is a program to create a glfw window and load dll from path/main.dll to run render and pass `GLFWwindow*` as parameter

You can use this tool by writing a dll and expose three methods:

```c++
void render_draw();
void render_load(GLFWwindow* window);
void render_destroy();
```

You can include `render_dll.h` to include the definitions.