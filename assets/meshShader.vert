#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in float lighting;

out vec2 _uv;
out float visibility;
out float _lighting;

uniform mat4 view;
uniform mat4 projection;

uniform float fog_density;
uniform float fog_gradient;

void main() {
    vec4 viewPos = view * vec4(position, 1.0);
    gl_Position = projection * viewPos;
    _uv = uv;
    _lighting = lighting;

    float dist = length(viewPos);
    visibility = exp(-pow(fog_density * dist, fog_gradient));
}