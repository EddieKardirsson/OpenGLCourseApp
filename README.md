# OpenGLCourseApp

This project is set for Visual Studio 2022.

Using GLEW 2.2.0 and the specific 64-bit version
Using GLFW 3.3.9 and also the 64-bit version 

Both of these OpenGL libraries are included in the External Libs folder inside the Solution Directory (one level above project).

The Additional Include Directories has included both library includes
Linker also contains the corresponding Additional Library  Directories
At Linker->Input, the Additional Dependencies has opengl32.lib, glew32.lib and glfw3.lib added.

So if using Windows 10 and Visual Studio 2022 it should be working. But if using anything else, You may want to change the versions of 
GLEW and GLFW.

Otherwise, I'll provide with the links.

GLEW: https://sourceforge.net/projects/glew/
GLFW: https://www.glfw.org/download.html


This ReadMe is mainly as a note to self, but should be clear to anyone who wants to clone the repo.
Hopefully, this will work and not break. I'll do a double check on my laptop later.