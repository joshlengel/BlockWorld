#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color;

out vec2 _uv;
out vec4 _color;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    _uv = uv;
    _color = color;
}