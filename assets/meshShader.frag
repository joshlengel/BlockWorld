#version 330

in vec2 _uv;
in float visibility;
in float _lighting;

out vec4 fragment;

uniform sampler2D tex;

uniform vec3 fog_color;

void main() {
    vec4 color = texture2D(tex, _uv);
    if (color.a < 0.5) discard;

    color = vec4(color.xyz * _lighting, color.a);
    fragment = mix(vec4(fog_color, 1.0), color, visibility);
}