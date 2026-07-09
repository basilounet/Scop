#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 camMatrix;
uniform mat4 model;
uniform vec3 translation;
// Have this uniform if using the first or second methods showed in the tutorial
uniform float outlining;

void main()
{
    vec3 currentPos = vec3(model * vec4(aPos + aNormal * outlining, 1.0f)) + translation;

    gl_Position = camMatrix * vec4(currentPos, 1.0f);
}
