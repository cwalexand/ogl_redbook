///////////////////////////////////////////////////////////////////////////////
//
// File Name: drawcommands.cpp
//
// Purpose: Implements Example 3.6, 'Drawing Commands Example', in The OpenGL
//          Programming Guide, Eighth Edition.
//
///////////////////////////////////////////////////////////////////////////////
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "ShaderUtil.h"
#include "vmath.h"


// File Scope Globals
static float aspect = 1.0f;
static GLuint render_prog;
static GLuint vbo[1];
static GLuint ebo[1];
static GLint render_model_matrix_loc;
static GLint render_projection_matrix_loc;


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
    vmath::mat4 model_matrix;
    vmath::mat4 projection_matrix;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up the projection matrix
    projection_matrix = vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 20.0f);
    glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, projection_matrix);

    // Draw Arrays
    model_matrix = vmath::translate(-3.0f, 0.0f, -5.0f);
    glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // DrawElements
    model_matrix = vmath::translate(-1.0f, 0.0f, -5.0f);
    glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);

    // DrawElementsBaseVertex
    model_matrix = vmath::translate(1.0f, 0.0f, -5.0f);
    glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
    glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);

    // DrawArraysInstanced
    model_matrix = vmath::translate(3.0f, 0.0f, -5.0f);
    glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);

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
    ShaderInfo shader_info[] =
    {
        { GL_VERTEX_SHADER, "../../shaders/primitive_restart.vs.glsl", 0 },
        { GL_FRAGMENT_SHADER, "../../shaders/primitive_restart.fs.glsl", 0 },
        { GL_NONE, NULL, 0 }
    };

    render_prog = su.LoadShaders(shader_info);

    glUseProgram(render_prog);

    // Get the uniform locations from the shader
    render_model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
    render_projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");

    // A single triangle
    const GLfloat vertex_positions[] =
    {
        -1.0f, -1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 1.0f,
    };

    // Color for each vertex
    const GLfloat vertex_colors[] =
    {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f
    };

    // Indices for the triangle strips
    const GLushort vertex_indices[] =
    {
        0, 1, 2
    };

    // Set up the element array buffer
    glGenBuffers(1, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

    // Set up the vertex buffer
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions) + sizeof(vertex_colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_positions), vertex_positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vertex_colors), vertex_colors);

    // Enable position & color data
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertex_positions));
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
    glDeleteProgram(render_prog);
    glDeleteBuffers(1, vbo);
    glDeleteBuffers(1, ebo);
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

    // aspect is used to set up the projection matrix
    aspect = float(height) / float(width);
}
