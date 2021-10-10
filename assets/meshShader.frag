#version 330

in vec2 _uv;

out vec4 fragment;

uniform sampler2D tex;

void main() {
    fragment = texture2D(tex, _uv);
}