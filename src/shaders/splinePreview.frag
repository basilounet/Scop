#version 330 core

out vec4 FragColor;

in float   grad;

vec3    col1 = vec3(0, 1, 0);
vec3    col2 = vec3(1, 0, 0);

void main()
{
    vec3    color = mix(col1, col2, grad);

    FragColor = vec4(color, 1.);

}