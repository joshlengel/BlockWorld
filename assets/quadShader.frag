#version 330

in vec2 _uv;

out vec4 fragment;

uniform sampler2D tex;
uniform vec4 color;

void main() {
    fragment = color * texture2D(tex, _uv);
}