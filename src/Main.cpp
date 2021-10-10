#include"Window.h"
#include"Shader.h"
#include"Chunk.h"
#include"Camera.h"
#include"Texture.h"

#include<iostream>
#include<chrono>

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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    Camera camera{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
    CameraController controller(window);
    controller.SetCamera(camera);
    controller.SetSpeed(10.0f);

    Chunk chunk(0, 0);
    Texture tex = Texture::FromImage("assets/blocks.png");
    Shader mesh_shader("assets/meshShader.vert", "assets/meshShader.frag");
    mesh_shader.DeclareUniform("view");
    mesh_shader.DeclareUniform("projection");

    window.LockCursor();
    window.Show();

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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        tex.Bind();
        mesh_shader.Bind();
        mesh_shader.SetUniform("view", camera.GetViewMatrix());
        mesh_shader.SetUniform("projection", window.GetProjectionMatrix());
        chunk.Render();

        window.Update(dt);
    }
}