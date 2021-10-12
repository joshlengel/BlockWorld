#include"Player.h"
#include"Window.h"
#include"World.h"

#include<cmath>
#include<vector>
#include<iostream>

#include<GLFW/glfw3.h>

static const float PLAYER_WIDTH = 0.5f;
static const float PLAYER_HEIGHT = 1.9f;
static const float CAM_HEIGHT = 1.7f;

static const float JUMP_SPEED = 8.0f;
static const float GRAVITY = -20.0f;
static const float DRAG_COEFFICIENT = 0.8f;

static const float EPSILON = 0.001f;

static int32_t sgn(float val)
{
    return val < 0.0f? -1 : 1;
}

FPPlayer::FPPlayer(const Window &window, const Vec3f &position, const Vec2f &rotation, World &world):
    CameraController(window),
    m_type(Type::NORMAL),
    m_position(position),
    m_rotation(rotation),
    m_velocity(),
    m_world(world)
{}

Vec3f FPPlayer::GetPosition() const { return m_position; }

void FPPlayer::SetSpeed(float acceleration, float max_speed) { m_acc = acceleration; m_max_speed = max_speed; }
void FPPlayer::SetSensitivity(float sensitivity) { m_sensitivity = sensitivity; }
void FPPlayer::SetType(Type type) { m_type = type; }

void FPPlayer::Update(float dt)
{
    if (!camera) return;

    // Update velocity (keyboard input, drag, gravity, etc.)
    float s = std::sin(m_rotation.y);
    float c = std::cos(m_rotation.y);

    Vec3f acc;
    
    if (window.KeyDown(GLFW_KEY_W))
    {
        acc.z -= c;
        acc.x += s;
    }

    if (window.KeyDown(GLFW_KEY_S))
    {
        acc.z += c;
        acc.x -= s;
    }

    if (window.KeyDown(GLFW_KEY_D))
    {
        acc.z += s;
        acc.x += c;
    }

    if (window.KeyDown(GLFW_KEY_A))
    {
        acc.z -= s;
        acc.x -= c;
    }

    acc = Vec3f::Dot(acc, acc) > 0.0f? Vec3f::Normalized(acc) : Vec3f();

    switch (m_type)
    {
        case Type::SUPER:
            if (window.KeyDown(GLFW_KEY_SPACE))
            {
                acc.y += 1.0f;
            }

            if (window.KeyDown(GLFW_KEY_LEFT_SHIFT) || window.KeyDown(GLFW_KEY_RIGHT_SHIFT))
            {
                acc.y -= 1.0f;
            }
            break;
        
        case Type::NORMAL:
            if (window.KeyDown(GLFW_KEY_SPACE) && m_has_jump)
            {
                m_velocity.y = JUMP_SPEED;
                m_has_jump = false;
            }
            break;
    }
    
    acc *= m_acc;
    if (m_type == Type::NORMAL)
    {
        acc.y += GRAVITY;
    }

    m_velocity += acc * dt;
    Vec2f planar_velocity = { m_velocity.x, m_velocity.z };

    if (Vec2f::Dot(planar_velocity, planar_velocity) > m_max_speed * m_max_speed) planar_velocity = Vec2f::Normalized(planar_velocity) * m_max_speed;
    m_velocity.x = planar_velocity.x;
    m_velocity.z = planar_velocity.y;

    if (m_has_jump)
    {
        float v = Vec3f::Length(m_velocity);

        if (v > 0.01f)
        {
            m_velocity.x += DRAG_COEFFICIENT * GRAVITY * m_velocity.x / v * dt;
            m_velocity.z += DRAG_COEFFICIENT * GRAVITY * m_velocity.z / v * dt;
        }
        else
        {
            m_velocity = Vec3f();
        }
    }

    float cvx, cvy;
    window.GetCursorVelocity(cvx, cvy);

    m_rotation.x += m_sensitivity * cvy * dt;
    m_rotation.y += m_sensitivity * cvx * dt;

    m_rotation.x = std::max(std::min(m_rotation.x, static_cast<float>(M_PI_2)), static_cast<float>(-M_PI_2));

    // Do physics
    float time = dt;
    while (time > 0.0f)
    {
        float left = m_position.x - PLAYER_WIDTH * 0.5f;
        float right = m_position.x + PLAYER_WIDTH * 0.5f;
        float bottom = m_position.y - PLAYER_HEIGHT * 0.5f;
        float top = m_position.y + PLAYER_HEIGHT * 0.5f;
        float back = m_position.z - PLAYER_WIDTH * 0.5f;
        float front = m_position.z + PLAYER_WIDTH * 0.5f;

        int32_t sx = sgn(m_velocity.x);
        int32_t sy = sgn(m_velocity.y);
        int32_t sz = sgn(m_velocity.z);

        int32_t min_x = static_cast<int32_t>(std::floor(m_position.x - sx * PLAYER_WIDTH * 0.5f));
        int32_t max_x = static_cast<int32_t>(std::floor(m_position.x + m_velocity.x * time + sx * (PLAYER_WIDTH * 0.5f + 1.0f)));
        int32_t min_y = static_cast<int32_t>(std::floor(m_position.y - sy * PLAYER_HEIGHT * 0.5f));
        int32_t max_y = static_cast<int32_t>(std::floor(m_position.y + m_velocity.y * time + sy * (PLAYER_HEIGHT * 0.5f + 1.0f)));
        int32_t min_z = static_cast<int32_t>(std::floor(m_position.z - sz * PLAYER_WIDTH * 0.5f));
        int32_t max_z = static_cast<int32_t>(std::floor(m_position.z + m_velocity.z * time + sz * (PLAYER_WIDTH * 0.5f + 1.0f)));

        float min_time = time;
        Vec3f min_position = m_position + m_velocity * time;
        Vec3f min_velocity = m_velocity;
        bool min_has_jump = m_has_jump;

        for (int32_t x = min_x; x != max_x; x += sx)
        for (int32_t y = min_y; y != max_y; y += sy)
        for (int32_t z = min_z; z != max_z; z += sz)
        {
            Voxel *v = m_world.GetBlock({ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) });

            if (!v || v->type == Voxel::Type::AIR) continue;

            float v_left = x;
            float v_right = x + 1;
            float v_bottom = y;
            float v_top = y + 1;
            float v_back = z;
            float v_front = z + 1;

            // x' = x + v * t => t = (x' - x) / v
            float t_left = (v_right - left) / m_velocity.x;
            float t_right = (v_left - right) / m_velocity.x;
            float t_bottom = (v_top - bottom) / m_velocity.y;
            float t_top = (v_bottom - top) / m_velocity.y;
            float t_back = (v_front - back) / m_velocity.z;
            float t_front = (v_back - front) / m_velocity.z;

            if (m_velocity.x < 0.0f && left > v_right && t_left < min_time)
            {
                float f_bottom = bottom + m_velocity.y * t_left;
                float f_top = top + m_velocity.y * t_left;
                float f_back = back + m_velocity.z * t_left;
                float f_front = front + m_velocity.z * t_left;

                if (f_bottom < v_top && f_top > v_bottom && f_back < v_front && f_front > v_back)
                {
                    min_time = t_left;
                    min_position = m_position + m_velocity * t_left;
                    min_position.x += EPSILON;
                    min_velocity = { 0.0f, m_velocity.y, m_velocity.z };
                    min_has_jump = m_has_jump;
                }
            }

            if (m_velocity.x > 0.0f && right < v_left && t_right < min_time)
            {
                float f_bottom = bottom + m_velocity.y * t_right;
                float f_top = top + m_velocity.y * t_right;
                float f_back = back + m_velocity.z * t_right;
                float f_front = front + m_velocity.z * t_right;

                if (f_bottom < v_top && f_top > v_bottom && f_back < v_front && f_front > v_back)
                {
                    min_time = t_right;
                    min_position = m_position + m_velocity * t_right;
                    min_position.x -= EPSILON;
                    min_velocity = { 0.0f, m_velocity.y, m_velocity.z };
                    min_has_jump = m_has_jump;
                }
            }

            if (m_velocity.y < 0.0f && bottom > v_top && t_bottom < min_time)
            {
                float f_left = left + m_velocity.x * t_bottom;
                float f_right = right + m_velocity.x * t_bottom;
                float f_back = back + m_velocity.z * t_bottom;
                float f_front = front + m_velocity.z * t_bottom;

                if (f_left < v_right && f_right > v_left && f_back < v_front && f_front > v_back)
                {
                    min_time = t_bottom;
                    min_position = m_position + m_velocity * t_bottom;
                    min_position.y += EPSILON;
                    min_velocity = { m_velocity.x, 0.0f, m_velocity.z };
                    min_has_jump = true;
                }
            }

            if (m_velocity.y > 0.0f && top < v_bottom && t_top < min_time)
            {
                float f_left = left + m_velocity.x * t_top;
                float f_right = right + m_velocity.x * t_top;
                float f_back = back + m_velocity.z * t_top;
                float f_front = front + m_velocity.z * t_top;

                if (f_left < v_right && f_right > v_left && f_back < v_front && f_front > v_back)
                {
                    min_time = t_top;
                    min_position = m_position + m_velocity * t_top;
                    min_position.y -= EPSILON;
                    min_velocity = { m_velocity.x, 0.0f, m_velocity.z };
                    min_has_jump = m_has_jump;
                }
            }

            if (m_velocity.z < 0.0f && back > v_front && t_back < min_time)
            {
                float f_left = left + m_velocity.x * t_back;
                float f_right = right + m_velocity.x * t_back;
                float f_bottom = bottom + m_velocity.y * t_back;
                float f_top = top + m_velocity.y * t_back;

                if (f_left < v_right && f_right > v_left && f_bottom < v_top && f_top > v_bottom)
                {
                    min_time = t_back;
                    min_position = m_position + m_velocity * t_back;
                    min_position.z += EPSILON;
                    min_velocity = { m_velocity.x, m_velocity.y, 0.0f };
                    min_has_jump = m_has_jump;
                }
            }

            if (m_velocity.z > 0.0f && front < v_back && t_front < min_time)
            {
                float f_left = left + m_velocity.x * t_front;
                float f_right = right + m_velocity.x * t_front;
                float f_bottom = bottom + m_velocity.y * t_front;
                float f_top = top + m_velocity.y * t_front;

                if (f_left < v_right && f_right > v_left && f_bottom < v_top && f_top > v_bottom)
                {
                    min_time = t_front;
                    min_position = m_position + m_velocity * t_front;
                    min_position.z -= EPSILON;
                    min_velocity = { m_velocity.x, m_velocity.y, 0.0f };
                    min_has_jump = m_has_jump;
                }
            }
        }

        time -= min_time;
        m_position = min_position;
        m_velocity = min_velocity;
        m_has_jump = min_has_jump;
    }

    // Update camera
    camera->position = m_position + Vec3f(0.0f, CAM_HEIGHT - PLAYER_HEIGHT * 0.5f, 0.0f);
    camera->rotation = m_rotation;

    // Check for interaction
    if (window.MouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        Vec3f ray_start, ray_direction;
        camera->GetRay(ray_start, ray_direction);
        m_world.Break(ray_start, ray_direction);
    }

    if (window.MouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
    {
        Vec3f ray_start, ray_direction;
        camera->GetRay(ray_start, ray_direction);
        m_world.Place(ray_start, ray_direction, Voxel::Type::STONE, [=](const Vec3f &pos)
        {
            return pos.x < std::floor(m_position.x - PLAYER_WIDTH * 0.5f)
                || pos.x > std::floor(m_position.x + PLAYER_WIDTH * 0.5f)
                || pos.y < std::floor(m_position.y - PLAYER_HEIGHT * 0.5f)
                || pos.y > std::floor(m_position.y + PLAYER_HEIGHT * 0.5f)
                || pos.z < std::floor(m_position.z - PLAYER_WIDTH * 0.5f)
                || pos.z > std::floor(m_position.z + PLAYER_WIDTH * 0.5f);
        });
    }
}