#version 330 core

layout (location = 0) in vec4 pos;

uniform mat4 camMatrix;
uniform mat4 model;
uniform vec3 translation;

out float       grad;

void main()
{
    grad = pos.w;
    gl_Position = camMatrix * vec4(vec3(model * vec4(pos.xyz, 1.)) + translation, 1.0f);
}