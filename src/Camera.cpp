#include"Camera.h"
#include"Window.h"

#include<cmath>

#include<GLFW/glfw3.h>

Mat4 Camera::GetViewMatrix() const
{
    Mat4 translate;
    translate.m30 = -position.x;
    translate.m31 = -position.y;
    translate.m32 = -position.z;

    Mat4 rotate_x;
    rotate_x.m11 =  std::cos(rotation.x);
    rotate_x.m21 =  std::sin(rotation.x);
    rotate_x.m12 = -std::sin(rotation.x);
    rotate_x.m22 =  std::cos(rotation.x);

    Mat4 rotate_y;
    rotate_y.m00 =  std::cos(rotation.y);
    rotate_y.m20 =  std::sin(rotation.y);
    rotate_y.m02 = -std::sin(rotation.y);
    rotate_y.m22 =  std::cos(rotation.y);

    return rotate_x * rotate_y * translate;
}

void Camera::GetRay(Vec3 &ray_start, Vec3 &ray_direction) const
{
    ray_start = position;

    ray_direction.x = std::sin(rotation.y) * std::cos(rotation.x);
    ray_direction.y = std::sin(rotation.x);
    ray_direction.z = -std::cos(rotation.y) * std::cos(rotation.x);
}

CameraController::CameraController(const Window &window):
    m_window(window),
    m_camera(nullptr),
    m_speed(1.0f),
    m_sensitivity(0.001f)
{}

void CameraController::SetCamera(Camera &camera) { m_camera = &camera; }
void CameraController::SetSpeed(float speed) { m_speed = speed; }
void CameraController::SetSensitivity(float sensitivity) { m_sensitivity = sensitivity; }

void CameraController::Update(float dt)
{
    if (!m_camera) return;

    float dx = m_speed * dt;
    float s_dx = dx * std::sin(m_camera->rotation.y);
    float c_dx = dx * std::cos(m_camera->rotation.y);
    
    if (m_window.KeyDown(GLFW_KEY_W))
    {
        m_camera->position.z -= c_dx;
        m_camera->position.x += s_dx;
    }

    if (m_window.KeyDown(GLFW_KEY_S))
    {
        m_camera->position.z += c_dx;
        m_camera->position.x -= s_dx;
    }

    if (m_window.KeyDown(GLFW_KEY_D))
    {
        m_camera->position.z += s_dx;
        m_camera->position.x += c_dx;
    }

    if (m_window.KeyDown(GLFW_KEY_A))
    {
        m_camera->position.z -= s_dx;
        m_camera->position.x -= c_dx;
    }

    if (m_window.KeyDown(GLFW_KEY_SPACE))
    {
        m_camera->position.y += dx;
    }

    if (m_window.KeyDown(GLFW_KEY_LEFT_SHIFT) || m_window.KeyDown(GLFW_KEY_RIGHT_SHIFT))
    {
        m_camera->position.y -= dx;
    }

    float cvx, cvy;
    m_window.GetCursorVelocity(cvx, cvy);

    m_camera->rotation.x += m_sensitivity * cvy * dt;
    m_camera->rotation.y += m_sensitivity * cvx * dt;

    m_camera->rotation.x = std::max(std::min(m_camera->rotation.x, static_cast<float>(M_PI_2)), static_cast<float>(-M_PI_2));
}