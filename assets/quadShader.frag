#version 330

in vec2 _uv;
in vec4 _color;

out vec4 fragment;

void main() {
    fragment = _color;
}