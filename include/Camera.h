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
    virtual ~CameraController() = default;
    
    void SetCamera(Camera &camera);

    virtual void Update(float dt) = 0;

protected:
    const Window &window;
    Camera *camera;
};