#version 330 core

out vec4 FragColor;

in vec3 currentPos;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

uniform sampler2D texture0;

void main()
{
    FragColor = vec4(normal, 1.0f);
//    FragColor = texture(texture0, texCoord);
}
