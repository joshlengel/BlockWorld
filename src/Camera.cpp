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

void Camera::GetRay(Vec3f &ray_start, Vec3f &ray_direction) const
{
    ray_start = position;

    ray_direction.x = std::sin(rotation.y) * std::cos(rotation.x);
    ray_direction.y = std::sin(rotation.x);
    ray_direction.z = -std::cos(rotation.y) * std::cos(rotation.x);
}

CameraController::CameraController(const Window &window):
    window(window),
    camera(nullptr)
{}

void CameraController::SetCamera(Camera &c) { camera = &c; }