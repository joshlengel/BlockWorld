#include"Window.h"

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<stdexcept>
#include<cmath>
#include<cstring>

Window::Window(int width, int height, const std::string &title):
    m_width(width),
    m_height(height),
    m_old_cx(0.0f),
    m_old_cy(0.0f),
    m_cvx(0.0f),
    m_cvy(0.0f)
{
    std::memset(m_buttons, 0, sizeof(m_buttons));

    // Initialize GLFW
    if (!glfwInit()) throw std::runtime_error("Error initializing GLFW");
    
    // Create invisible, non-resizable window
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) throw std::runtime_error("Error opening window");

    // Center window
    const GLFWvidmode *vid_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(m_window, (vid_mode->width - width) / 2, (vid_mode->height - height) / 2);
    glfwSetCursorPos(m_window, 0.0, 0.0);

    // Initialize OpenGL context
    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) throw std::runtime_error("Error initializing GLAD");
}

Window::~Window()
{
    glfwTerminate();
}

int Window::GetWidth() const { return m_width; }
int Window::GetHeight() const { return m_height; }

void Window::Show() { glfwShowWindow(m_window); }
bool Window::ShouldClose() { return glfwWindowShouldClose(m_window); }
void Window::Update(float dt)
{
    glfwSwapBuffers(m_window);

    for (int key = 0; key < GLFW_KEY_LAST; ++key) m_buttons[key] = KeyDown(key);
    glfwPollEvents();

    double new_cx, new_cy;
    glfwGetCursorPos(m_window, &new_cx, &new_cy);

    m_cvx = static_cast<float>(new_cx - m_old_cx) / dt;
    m_cvy = static_cast<float>(new_cy - m_old_cy) / dt;

    m_old_cx = new_cx;
    m_old_cy = new_cy;
}

bool Window::KeyDown(int key) const { return glfwGetKey(m_window, key) == GLFW_PRESS; }
bool Window::KeyPressed(int key) const { return KeyDown(key) && !m_buttons[key]; }
bool Window::KeyReleased(int key) const { return !KeyDown(key) && m_buttons[key]; }
void Window::GetCursorVelocity(float &vx, float &vy) const { vx = m_cvx; vy = m_cvy; }

bool Window::CursorLocked() const { return glfwGetInputMode(m_window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL; }
void Window::LockCursor() const { glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
void Window::UnlockCursor() const { glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
void Window::ToggleCursor() const
{
    if (CursorLocked()) UnlockCursor();
    else LockCursor();
    glfwSetCursorPos(m_window, 0.0, 0.0);
    m_old_cx = m_old_cy = 0.0;
}

Mat4 Window::GetProjectionMatrix() const
{
    float tan_half_fov = std::tan(M_PI * 0.5f * 0.5f);
    float ar = static_cast<float>(m_width) / static_cast<float>(m_height);
    float clip_near = 0.1f;
    float clip_far = 100.0f;

    return
    {
        1.0f / (tan_half_fov * ar), 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / tan_half_fov, 0.0f, 0.0f,
        0.0f, 0.0f, (clip_far + clip_near) / (clip_near - clip_far), 2 * clip_far * clip_near / (clip_near - clip_far),
        0.0f, 0.0f, -1.0f, 0.0f
    };
}