#version 330 core
#extension GL_ARB_gpu_shader_int64 : enable

layout (location = 0) in vec4 pos;

uniform mat4	view;
uniform mat4	proj;
uniform mat4	model;

out float       grad;

void main()
{
    grad = pos.w;
    gl_Position = proj * view * model * vec4(pos.xyz, 1.0);
}