#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 currentPos;
out vec3 normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 camMatrix;
uniform mat4 model;
uniform vec3 translation;
uniform bool staticTex;

void main()
{
   currentPos = vec3(model * vec4(aPos, 1.0f)) + translation;
   normal = abs(aNormal);
   color = aColor;
   if (staticTex) {
      int biggestIndex = normal.x > normal.y ? (normal.x > normal.z ? 0 : 2) : (normal.y > normal.z ? 1 : 2);
      if (biggestIndex == 0)
         texCoord = currentPos.zy; // Use YZ plane for X-dominant faces
      else if (biggestIndex == 1)
         texCoord = currentPos.xz; // Use XZ plane for Y-dominant faces
      else
         texCoord = currentPos.xy; // Use XY plane for Z-dominant faces
   }
   else
      texCoord = aTex;

   gl_Position = camMatrix * vec4(currentPos, 1.0f);
}
