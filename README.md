ogl_redbook
===========

Refactor, debug, and complete code examples for the OpenGL Programming Guide, Eighth Edition.

The baseline for this work was taken from the code examples found here:

http://www.opengl-redbook.com/Code/oglpg-8th-edition.zip

GLEW and freeglut are required for this project. You can find them here:

http://glew.sourceforge.net/

http://freeglut.sourceforge.net/

THe projects in this repository were created with Microsoft Visual Studio Express 2012 for Windows Desktop. To use these projects, you will need to update the project paths to your GLEW and freeglut directories in the following Property Pages areas:

All Configurations : C/C++ : General : Additional Include Directories

All Configurations : Linker : General : Additional Library Directories

Even though the OpenGL Programming Guide, Eighth Edition, is targeted for OpenGL 4.3, my display adapter only supports OpenGL up to version 2.1. I'll let you know if I run into any issues...

**Example** | **Code**
------------|-------------
1.1 Our First OpenGL Program | chapter01/ch01_triangles
3.6 Drawing Commands | chapter03/ch03_drawcommands
3.8 Primitive Restart | chapter03/ch03_primitive_restart
3.12 Instancing | chapter03/ch03_instancing
3.13 gl_InstanceID Example Vertex Shader | chapter03/ch03_instancing_tbo
