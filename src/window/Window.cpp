#include"window/Window.h"

#include<glad/glad.h>

#include<cstdint>
#include<iostream>

const static uint32_t DEFAULT_WINDOW_WIDTH = 800;
const static uint32_t DEFAULT_WINDOW_HEIGHT = 600;

Window::Window(const std::string &title):
    m_window(nullptr)
{
    if (!glfwInit())
    {
        std::cerr << "Error initializing GLFW" << std::endl;
        return;
    }

    uint32_t width, height;
    GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *vid_mode;

    if (primary_monitor && (vid_mode = glfwGetVideoMode(primary_monitor)))
    {
        width = static_cast<uint32_t>(vid_mode->width);
        height = static_cast<uint32_t>(vid_mode->height);
    }
    else
    {
        width = DEFAULT_WINDOW_WIDTH;
        height = DEFAULT_WINDOW_HEIGHT;
    }
    
    if (!(m_window = glfwCreateWindow(width, height, title.c_str(), primary_monitor, nullptr)))
    {
        std::cerr << "Error creating GLFW window" << std::endl;
        return;
    }

    m_aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

    m_keyboard = new Keyboard(m_window);
    m_mouse = new Mouse(m_window);
    m_cursor = new Cursor(m_window);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);

        Window *this_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        this_window->m_aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    });

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(&glfwGetProcAddress)))
    {
        std::cerr << "Error loading GLAD OpenGL loader" << std::endl;
        return;
    }
}

Window::~Window()
{
    delete m_keyboard;
    delete m_mouse;
    delete m_cursor;
    
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::Show()
{
    glfwShowWindow(m_window);
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(m_window);
}

void Window::Update()
{
    for (int32_t key = 0; key <= GLFW_KEY_LAST; ++key)
        m_keyboard->m_keys[key] = m_keyboard->KeyDown(key);

    for (int32_t mouse_button = 0; mouse_button <= GLFW_MOUSE_BUTTON_LAST; ++mouse_button)
        m_mouse->m_mouse_buttons[mouse_button] = m_mouse->MouseButtonDown(mouse_button);
    
    double c_x, c_y;
    glfwGetCursorPos(m_window, &c_x, &c_y);
    m_cursor->m_ox = m_cursor->m_nx;
    m_cursor->m_oy = m_cursor->m_ny;
    m_cursor->m_nx = static_cast<float>(c_x);
    m_cursor->m_ny = static_cast<float>(c_y);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

Keyboard *Window::GetKeyboard() { return m_keyboard; }
Mouse *Window::GetMouse() { return m_mouse; }
Cursor *Window::GetCursor() { return m_cursor; }

float Window::GetAspectRatio() { return m_aspect_ratio; }

Keyboard::Keyboard(GLFWwindow *window):
    m_window(window),
    m_keys()
{}

Keyboard::~Keyboard()
{}

bool Keyboard::KeyDown(int32_t key) { return glfwGetKey(m_window, key) == GLFW_PRESS; }
bool Keyboard::KeyPressed(int32_t key) { return KeyDown(key) && !m_keys[key]; }

Mouse::Mouse(GLFWwindow *window):
    m_window(window),
    m_mouse_buttons()
{}

Mouse::~Mouse()
{}

bool Mouse::MouseButtonDown(int32_t mouse_button) { return glfwGetMouseButton(m_window, mouse_button) == GLFW_PRESS; }
bool Mouse::MouseButtonPressed(int32_t mouse_button) { return MouseButtonDown(mouse_button) && !m_mouse_buttons[mouse_button]; }

Cursor::Cursor(GLFWwindow *window):
    m_window(window),
    m_nx(),
    m_ny(),
    m_ox(),
    m_oy()
{}

Cursor::~Cursor()
{}

float Cursor::GetCursorX() { return m_nx; }
float Cursor::GetCursorY() { return m_ny; }
float Cursor::GetCursorDX() { return m_nx - m_ox; }
float Cursor::GetCursorDY() { return m_ny - m_oy; }

void Cursor::Enable()
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Cursor::Disable()
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Cursor::Toggle()
{
    if (glfwGetInputMode(m_window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
        Disable();
    else
        Enable();
}