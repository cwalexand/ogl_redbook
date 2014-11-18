///////////////////////////////////////////////////////////////////////////////
//
// File Name: primitive_restart.cpp
//
// Purpose: Implements Example 3.8, 'Drawing a Cube Made of Two Triangle Strips
//          Using Primitive Reset', in The OpenGL Programming Guide, Eighth
//          Edition.
//
///////////////////////////////////////////////////////////////////////////////
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "ShaderUtil.h"
#include "vmath.h"

#define USE_PRIMITIVE_RESTART
#define BUFFER_OFFSET(x)  ((const GLvoid*) (x))

// File Scope Globals
static float aspect = 1.0;
static GLuint shader_prog;
static GLuint vao;
static GLuint vbo;
static GLuint ebo;
static GLint model_matrix_loc;
static GLint projection_matrix_loc;



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
    float t = float(GetTickCount() & 0x1FFF) / float(0x1FFF);
    const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
    const vmath::vec3 Z(0.0f, 0.0f, 1.0f);

    // Setup
    glEnable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate simple shading program
    glUseProgram(shader_prog);

    // Set up the model and projection matrix
    vmath::mat4 model_matrix(vmath::translate(0.0f, 0.0f, -5.0f) * 
                             vmath::rotate(t * 360.0f, Y) *
                             vmath::rotate(t * 720.0f, Z));

    vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, 
                                                 aspect, 1.0f, 20.0f));

    glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, model_matrix);
    glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, projection_matrix);

    // Set up for a glDrawElements call
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

#ifdef USE_PRIMITIVE_RESTART
    // When primitive restart is on, we can call one draw command
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(0xFFFF);
    glDrawElements(GL_TRIANGLE_STRIP, 17, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
#else
    // Without primitive restart, we need to call two draw commands
    glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
    glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT,
                   BUFFER_OFFSET(9 * sizeof(GLushort)));
#endif

    glDisable(GL_CULL_FACE);
    glUseProgram(0);

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

    static ShaderInfo shader_info[] =
    {
        { GL_VERTEX_SHADER, "../../shaders/primitive_restart.vs.glsl", 0 },
        { GL_FRAGMENT_SHADER, "../../shaders/primitive_restart.fs.glsl", 0 },
        { GL_NONE, NULL, 0 }
    };

    shader_prog = su.LoadShaders(shader_info);

    model_matrix_loc = glGetUniformLocation(shader_prog, "model_matrix");
    projection_matrix_loc = glGetUniformLocation(shader_prog, "projection_matrix");

    // 8 corners of a cube, side length 2, centered on the origin
    static const GLfloat cube_positions[] =
    {
        -1.0f, -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 1.0f,
        1.0f, -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 1.0f,
        1.0f,  1.0f, -1.0f, 1.0f,
        1.0f,  1.0f,  1.0f, 1.0f
    };

    // Color for each vertex
    static const GLfloat cube_colors[] =
    {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f
    };

    // Indices for the triangle strips
    static const GLushort cube_indices[] =
    {
        0, 1, 2, 3, 6, 7, 4, 5,   // First strip
        0xFFFF,                   // <-- This is the restart index
        2, 6, 0, 4, 1, 5, 3, 7    // Second strip
    };

    // Set up the element array buffer
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), 
                 cube_indices, GL_STATIC_DRAW);

    //// Set up the vertex attributes
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_positions) + sizeof(cube_colors),
                 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_positions), cube_positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_positions), 
                    sizeof(cube_colors), cube_colors);    

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 
                          BUFFER_OFFSET(sizeof(cube_positions)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

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
    glDeleteProgram(shader_prog);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
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

    aspect = float(height) / float(width);
}

