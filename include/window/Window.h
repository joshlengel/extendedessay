#pragma once

#define GLFW_INCLUDE_NONE
#include<GLFW/glfw3.h>

#include<string>

class Keyboard;
class Mouse;
class Cursor;

class Window
{
public:
    Window(const std::string &title);
    ~Window();

    void Show();
    bool ShouldClose();
    void Update();

    Keyboard *GetKeyboard();
    Mouse *GetMouse();
    Cursor *GetCursor();

    float GetAspectRatio();

private:
    GLFWwindow *m_window;

    Keyboard *m_keyboard;
    Mouse *m_mouse;
    Cursor *m_cursor;

    float m_aspect_ratio;
};

class Keyboard
{
    friend class Window;
public:
    bool KeyDown(int32_t key);
    bool KeyPressed(int32_t key);

private:
    Keyboard(GLFWwindow *window);
    ~Keyboard();

    GLFWwindow *m_window;
    bool m_keys[GLFW_KEY_LAST + 1];
};

class Mouse
{
    friend class Window;
public:
    bool MouseButtonDown(int32_t mouse_button);
    bool MouseButtonPressed(int32_t mouse_button);

private:
    Mouse(GLFWwindow *window);
    ~Mouse();

    GLFWwindow *m_window;
    bool m_mouse_buttons[GLFW_MOUSE_BUTTON_LAST + 1];
};

class Cursor
{
    friend class Window;
public:
    float GetCursorX();
    float GetCursorY();
    float GetCursorDX();
    float GetCursorDY();

    void Enable();
    void Disable();
    void Toggle();

private:
    Cursor(GLFWwindow *window);
    ~Cursor();

    GLFWwindow *m_window;
    float m_nx, m_ny;
    float m_ox, m_oy;
};