///////////////////////////////////////////////////////////////////////////////
//
// File Name: triangles.cpp
//
// Purpose: Implements Example 1.1, 'Our First OpenGL Program', in The OpenGL
//          Programming Guide, Eighth Edition.
//
///////////////////////////////////////////////////////////////////////////////
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "ShaderUtil.h"

#define BUFFER_OFFSET(x)  ((const void*) (x))

// File Scope Globals
static enum Buffer_IDs {ArrayBuffer, NumBuffers};
static enum Attrib_IDs {vPosition = 0};
static GLuint Buffers[NumBuffers];
static const GLuint NumVertices = 6;
static GLuint program;


///////////////////////////////////////////////////////////////////////////////
// Function Name: display
//
// Purpose: GLUT display callback.
// 
// INPUTS: None.
//
// OUTPUTS: None.
//
///////////////////////////////////////////////////////////////////////////////
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    glutSwapBuffers();
}



///////////////////////////////////////////////////////////////////////////////
// Function Name: initialize
//
// Purpose: Sets up shaders and buffers used in the display function.
// 
// INPUTS: None.
//
// OUTPUTS: None.
//
///////////////////////////////////////////////////////////////////////////////
void initialize()
{
    ShaderUtil su;

    GLfloat vertices[NumVertices][2] = {
        {-0.90f, -0.90f}, // Triangle 1
        { 0.85f, -0.90f},
        {-0.90f,  0.85f},
        { 0.90f, -0.85f}, // Triangle 2
        { 0.90f,  0.90f},
        {-0.85f,  0.90f}
    };

    glGenBuffers(NumBuffers, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, "../../shaders/triangles.vert", 0},
        {GL_FRAGMENT_SHADER, "../../shaders/triangles.frag", 0},
        {GL_NONE, NULL, 0}
    };

    program = su.LoadShaders(shaders);
    glUseProgram(program);
    
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

///////////////////////////////////////////////////////////////////////////////
// Function Name: finalize
//
// Purpose: Cleans up on exit if the ESC key is used to exit the program.
// 
// INPUTS: None.
//
// OUTPUTS: None.
//
///////////////////////////////////////////////////////////////////////////////
void finalize()
{
    glUseProgram(0);
    glDeleteProgram(program);
    glDeleteBuffers(NumBuffers, Buffers);
}



///////////////////////////////////////////////////////////////////////////////
// Function Name: reshape
//
// Purpose: GLUT reshape callback.
// 
// INPUTS: None.
//
// OUTPUTS: None.
//
///////////////////////////////////////////////////////////////////////////////
void reshape(int width, int height)
{
    glViewport(0, 0 , width, height);
}
