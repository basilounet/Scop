#version 330 core

out vec4 FragColor;

in vec3 currentPos;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

uniform sampler2D texture0;
uniform float useTexturePercentage;

void main()
{
    FragColor = vec4(normal, 1.0f) * (1.0 - useTexturePercentage) + texture(texture0, texCoord) * useTexturePercentage;
}
