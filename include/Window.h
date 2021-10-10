#pragma once

#include"Mat.h"

#include<string>

struct GLFWwindow;

class Window
{
public:
    Window(int width, int height, const std::string &title);
    ~Window();

    int GetWidth() const;
    int GetHeight() const;

    void Show();
    bool ShouldClose();
    void Update(float dt);

    bool KeyDown(int key) const;
    bool KeyPressed(int key) const;
    bool KeyReleased(int key) const;
    void GetCursorVelocity(float &vx, float &vy) const;

    bool CursorLocked() const;
    void LockCursor() const;
    void UnlockCursor() const;
    void ToggleCursor() const;

    Mat4 GetProjectionMatrix() const;

private:
    GLFWwindow *m_window;

    int m_width, m_height;
    mutable double m_old_cx, m_old_cy;
    float m_cvx, m_cvy;

    bool m_buttons[400];
};