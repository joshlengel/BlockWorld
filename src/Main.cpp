#include"Window.h"
#include"Shader.h"
#include"World.h"
#include"Camera.h"
#include"Texture.h"
#include"Quad.h"

#include<iostream>
#include<chrono>
#include<cmath>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

static const int WIDTH = 900;
static const int HEIGHT = 600;
static const std::string TITLE = "Block World";

static const float SKY_RED = 0.47f;
static const float SKY_GREEN = 0.93f;
static const float SKY_BLUE = 0.91f;

int main()
{
    Window window(WIDTH, HEIGHT, TITLE);

    glClearColor(SKY_RED, SKY_GREEN, SKY_BLUE, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    Camera camera{ { 0.0f, 40.0f, 0.0f }, { 0.0f, 0.0f } };
    CameraController controller(window);
    controller.SetCamera(camera);
    controller.SetSpeed(10.0f);

    World world(0, 0, 921879573298);
    Texture tex = Texture::FromImage("assets/blocks.png");
    Shader mesh_shader("assets/meshShader.vert", "assets/meshShader.frag");
    mesh_shader.DeclareUniform("view");
    mesh_shader.DeclareUniform("projection");
    mesh_shader.DeclareUniform("fog_color");
    mesh_shader.DeclareUniform("fog_density");
    mesh_shader.DeclareUniform("fog_gradient");

    mesh_shader.Bind();
    mesh_shader.SetUniform("fog_color", { SKY_RED, SKY_GREEN, SKY_BLUE });
    mesh_shader.SetUniform("fog_density", 0.01f);
    mesh_shader.SetUniform("fog_gradient", 1.5f);

    float ar = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
    QuadRenderer quad_renderer;
    Texture crosshairs = Texture::FromImage("assets/crosshairs.png");

    Quad quad;
    quad.x = 0.0f; quad.y = 0.0f;
    quad.width = 0.05f / ar;
    quad.height = 0.05f;
    quad.texture = &crosshairs;
    quad_renderer.Add(quad);

    window.Show();
    window.LockCursor();

    // Timing
    std::chrono::high_resolution_clock::time_point t1, t2;
    t1 = t2 = std::chrono::high_resolution_clock::now();

    // Main loop
    while (!window.ShouldClose())
    {
        t2 = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration_cast<std::chrono::duration<float>>(t2 - t1).count();
        t1 = t2;

        if (window.KeyPressed(GLFW_KEY_ESCAPE)) window.ToggleCursor();

        if (window.CursorLocked()) controller.Update(dt);
        world.Update(camera.position);

        if (window.MouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
        {
            world.Break(camera.position,
                {
                    std::sin(camera.rotation.y) * std::cos(camera.rotation.x),
                    std::sin(camera.rotation.x),
                    -std::cos(camera.rotation.y) * std::cos(camera.rotation.x)
                });
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        tex.Bind();
        mesh_shader.Bind();
        mesh_shader.SetUniform("view", camera.GetViewMatrix());
        mesh_shader.SetUniform("projection", window.GetProjectionMatrix());
        world.Render();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        quad_renderer.Render();

        window.Update(dt);
    }
}