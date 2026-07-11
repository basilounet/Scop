# Scop
This project has been created as part of the 42 curriculum by bvasseur.

---
## Description
The goal of this project is to build a small program that displays a 3D object created with a modeling tool such as Blender.
It will be our responsibility to parse it and render it correctly using OpenGL in C++. 
The program will be able to display the object in a window and allow the user to manipulate it (rotate, zoom, etc.).
---
## Instrucions
Run the following command to compile the project:
```
make
```
Then run the program with the following command:
```
./scop <path_to_obj_file_1> <path_to_obj_file_2> ...
```
---
## Resources
- [OpenGL](https://www.opengl.org/)
- [GLFW](https://www.glfw.org/)
- [GLM](https://github.com/g-truc/glm) (as a reference for matrix and vector operations)
- [Wavefront OBJ file format](https://en.wikipedia.org/wiki/Wavefront_.obj_file)
- [Wavefront source 2](https://fr.wikipedia.org/wiki/Objet_3D_(format_de_fichier))
- [Video tutorials](https://www.youtube.com/watch?v=z03LXhRBLGI&list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-&index=2)
- [ImGui widget list](https://pthom.github.io/imgui_explorer/)
---
## Bonuses
- Better parsing of the OBJ file format
- Better texture mapping
- Moving Camera
- Real time and interactive advanced debug infos on screen
- Real time Textures, Materials and Object loading AND changes
- Meshes outlines with shaders
- SkyBox
- Object animations
- Multiples objects and meshing each having their own pos, orientation, outline, scale, shader, animations, etc. They also can be tweaked in real time and individually with ImGui.
- Splines
---