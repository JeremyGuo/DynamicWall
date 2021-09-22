//
// Created by JeremyGuo on 2021/9/21.
//

#include "WallWindow.h"
#include "cmdline.h"
#include <csignal>

HWND background = nullptr;

static
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    HWND p = FindWindowExA(hwnd, nullptr, "SHELLDLL_DefView", nullptr);
    HWND* ret = (HWND*)lParam;

    if (p) {
        // Gets the WorkerW Window after the current one.
        *ret = FindWindowExA(nullptr, hwnd, "WorkerW", nullptr);
    }
    return true;
}

WallWindow::~WallWindow() {
    sub_render_destroy();
    FreeLibrary(handle);
    glfwDestroyWindow(window);
}

void WallWindow::draw() {
    sub_render_draw();
}

WallWindow::WallWindow(GLFWmonitor* monitor, const std::string& render_dll_path, int least_refresh_rate, char level) {
    // Get background handle
    if (level == 'D') {
        HWND hwnd = ::FindWindowA("progman", "Program Manager");
        SendMessageTimeout(hwnd, 0x052c, 0, 0, SMTO_NORMAL, 0x3e8, nullptr);
        EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&background));

        if (!background) {
            throw std::runtime_error("Background Window Handle Not found.");
        }
    }

    // GLFW initialize
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }

    // Create GLFW window and mount to child
    {
        const GLFWvidmode *vid_mode = glfwGetVideoMode(monitor);
        int width = vid_mode->width, height = vid_mode->height;
        window = glfwCreateWindow(width, height, "DYN_WALL", nullptr, nullptr);
        if (window == nullptr) {
            throw std::runtime_error("Window creation failed.");
        }
        int monitor_x_pos, monitor_y_pos;
        HWND window_handle = glfwGetWin32Window(window);
        switch (level) {
            case 'D':
                SetParent(window_handle, background);
                break;
            case 'N':
                break;
            case 'T':
                SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        glfwGetMonitorPos(monitor, &monitor_x_pos, &monitor_y_pos);
        glfwSetWindowPos(window, monitor_x_pos, monitor_y_pos);

        if (vid_mode->refreshRate < least_refresh_rate) {
            throw std::runtime_error("Refresh rate too high");
        }

        int interval = 1;
        while (vid_mode->refreshRate / (interval + 1) >= least_refresh_rate)
            interval ++;
        glfwSwapInterval(interval);
    }

    // Find and load dll
    {
        std::string dll_path = render_dll_path + "/main.dll";
        handle = LoadLibrary(render_dll_path.c_str());

        if (handle == nullptr)
            throw std::runtime_error("Failed to load dll");

        this->sub_render_draw = (RenderDraw)GetProcAddress(handle, "render_draw");
        this->sub_render_init = (RenderInit)GetProcAddress(handle, "render_load");
        this->sub_render_destroy = (RenderDestroy)GetProcAddress(handle, "render_destroy");

        if (!this->sub_render_destroy)
            throw std::runtime_error("Failed to get correct destroy function in dll.");
        if (!this->sub_render_init)
            throw std::runtime_error("Failed to get correct load function in dll.");
        if (!this->sub_render_draw)
            throw std::runtime_error("Failed to get correct load function in dll.");

        this->sub_render_init(window);
    }
}

volatile int stop;
void sigint_handler ( int signum ) {
    if (signum == SIGINT) {
        stop = 1;
    }
}

int main(int argc, char** argv) {
    HANDLE init_sem = OpenSemaphore(NULL, false, "DynamicWindowInit");
    glfwInit();

    WallWindow *main_window = nullptr;
    cmdline::parser cmd_parser;

    stop = 0;

    signal(SIGINT, sigint_handler);

    cmd_parser.add<std::string>("path", 'p', "Path to the direction of renderer", true);
    cmd_parser.add<int>("monitor", 'm', "The index of the monitor to render to", false, 0);
    cmd_parser.add<int>("refresh", 'r', "The minimum refresh rate when render.");
    cmd_parser.add<std::string>("level", 'l', "The level of the window be in. ('T'op(May cause serious problem for cover all interactable window), 'D'esktop, 'N'ormal)");

    if (!cmd_parser.parse(argc, argv)) {
        glfwTerminate();
        cmd_parser.parse_check(argc, argv);
    }

    try {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        int monitor_index = cmd_parser.get<int>("monitor");
        if (monitor_index != 0) {
            int monitor_num;
            GLFWmonitor** monitors = glfwGetMonitors(&monitor_num);
            if (monitor_index >= monitor_num)
                throw std::runtime_error("Monitor index out of range.");
            monitor = monitors[monitor_index];
        }

        char level = cmd_parser.get<std::string>("level").c_str()[0];
        if (level != 'T' && level != 'D' && level != 'N')
            throw std::runtime_error("Not a valid option of level");

        std::string dll_path = cmd_parser.get<std::string>("path");
        int least_refresh_rate = cmd_parser.get<int>("refresh");

        main_window = new WallWindow(monitor, dll_path, least_refresh_rate, level);
    } catch (...) {
        // DO NOT RELEASE SEM TO TELL MAIN PROCESS IT CRASHED
        delete main_window;
        glfwTerminate();
        return 0;
    }
    ReleaseSemaphore(init_sem, 1, nullptr);

    while (!stop) {
        main_window->draw();
    }

    delete main_window;
    glfwTerminate();
    return 0;
}
