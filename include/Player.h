#pragma once

#include"Vec.h"
#include"Camera.h"

class World;

class FPPlayer : public CameraController
{
public:
    enum class Type
    {
        SUPER,
        NORMAL
    };

    FPPlayer(const Window &window, const Vec3f &position, const Vec2f &rotation, World &world);

    Vec3f GetPosition() const;
    bool IsSubmerged() const;

    void SetSpeed(float acceleration, float max_speed);
    void SetSensitivity(float sensitivity);
    void SetType(Type type);

    virtual void Update(float dt) override;

private:
    Type m_type;
    bool m_has_jump;
    bool m_submerged;

    Vec3f m_position;
    Vec2f m_rotation;
    Vec3f m_velocity;

    float m_acc, m_max_speed;
    float m_sensitivity;

    World &m_world;
};