#version 330 core

out vec4 FragColor;

in vec3 currentPos;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

uniform sampler2D   texture0;
uniform float       useTexturePercentage;
uniform float       useColorPercentage;

void main()
{
    FragColor = vec4(normal, 1.0f) * (1.0 - useTexturePercentage) + texture(texture0, texCoord) * useTexturePercentage;
//    FragColor = FragColor * (1.0 - useColorPercentage) + vec4(color, 1.0f) * useColorPercentage;
    FragColor = vec4(vec3(dot(FragColor.rgb, vec3(0.299, 0.587, 0.114))), 1.0f) * (1.0 - useColorPercentage) + FragColor * useColorPercentage;
}
