#pragma once

#include"Vec.h"
#include"Mat.h"

class Window;

class Camera
{
public:
    Vec3f position;
    Vec2f rotation;

    Mat4 GetViewMatrix() const;
    void GetRay(Vec3f &ray_start, Vec3f &ray_direction) const;
};

class CameraController
{
public:
    CameraController(const Window &window);

    void SetCamera(Camera &camera);
    void SetSpeed(float speed);
    void SetSensitivity(float sensitivity);

    void Update(float dt);

private:
    const Window &m_window;
    Camera *m_camera;

    float m_speed;
    float m_sensitivity;
};