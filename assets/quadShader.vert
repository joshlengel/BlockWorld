#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

out vec2 _uv;

uniform vec2 offset;
uniform vec2 scale;

void main() {
    gl_Position = vec4(position * scale + offset, 0.0, 1.0);
    _uv = uv;
}