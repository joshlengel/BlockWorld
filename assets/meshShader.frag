#version 330

in vec2 _uv;
in float visibility;
in float _lighting;
in float height;

out vec4 fragment;

uniform sampler2D tex;

uniform vec3 fog_color;

uniform vec3 water_color;
uniform float water_height;
uniform int water_enabled;

void main() {
    vec4 color = texture2D(tex, _uv);
    if (color.a < 0.5) discard;

    color = vec4(color.xyz * _lighting, color.a);
    color = color * (float(bool(water_enabled) && height < water_height) * (vec4(water_color, 1.0) - vec4(1.0)) + vec4(1.0));
    fragment = mix(vec4(fog_color, 1.0), color, visibility);
}