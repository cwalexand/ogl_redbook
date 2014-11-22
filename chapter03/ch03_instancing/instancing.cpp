///////////////////////////////////////////////////////////////////////////////
//
// File Name: instancing.cpp
//
// Purpose: Implements Example 3.12, 'Instancing Example', in The OpenGL
//          Programming Guide, Eighth Edition.
//
///////////////////////////////////////////////////////////////////////////////
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "ShaderUtil.h"
#include "vmath.h"
#include "VBObject.h"
using namespace vmath;

#define BUFFER_OFFSET(x)  ((const GLvoid*) (x))

// File Scope Globals
static float aspect = 1.0;
static GLuint color_buffer;
static GLuint model_matrix_buffer;
static GLuint shader_prog;
static GLint view_matrix_loc;
static GLint projection_matrix_loc;
static VBObject object;

static const int INSTANCE_COUNT = 100;



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
    float t = float(GetTickCount() & 0x3FFF) / float(0x3FFF);

    // Clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Bind the model matrix VBO and change its data
    glBindBuffer(GL_ARRAY_BUFFER, model_matrix_buffer);

    // Set model matrices for each instance
    mat4 * matrices = (mat4 *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    for (int n = 0; n < INSTANCE_COUNT; ++n)
    {
        float a = 50.0f * float(n) / 4.0f;
        float b = 50.0f * float(n) / 5.0f;
        float c = 50.0f * float(n) / 6.0f;

        matrices[n] = rotate(a + t * 360.0f, 1.0f, 0.0f, 0.0f) *
                      rotate(b + t * 360.0f, 0.0f, 1.0f, 0.0f) *
                      rotate(c + t * 360.0f, 0.0f, 0.0f, 1.0f) *
                      translate(10.0f + a, 40.0f + b, 50.0f + c);
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Activate instancing program
    glUseProgram(shader_prog);

    // Set up the view and projection matrices
    mat4 view_matrix(translate(0.0f, 0.0f, -1500.0f) * rotate(t * 360.0f * 2.0f, 0.0f, 1.0f, 0.0f));
    mat4 projection_matrix(frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 5000.0f));

    glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, view_matrix);
    glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, projection_matrix);

    // Render INSTANCE_COUNT objects
    object.Render(0, INSTANCE_COUNT);

    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);

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
        { GL_VERTEX_SHADER, "../../shaders/instancing.vs.glsl", 0 },
        { GL_FRAGMENT_SHADER, "../../shaders/instancing.fs.glsl", 0 },
        { GL_NONE, NULL, 0 }
    };

    shader_prog = su.LoadShaders(shader_info);

    // Get the location of the projetion_matrix uniform
    view_matrix_loc = glGetUniformLocation(shader_prog, "view_matrix");
    projection_matrix_loc = glGetUniformLocation(shader_prog, "projection_matrix");

    // Get the locations of the vertex attributes in 'shader_prog', which is the
    // (linked) program object that we're going to be rendering with. Note
    // that this isn't really necessary because we specified locations for
    // all the attributes in our vertex shader. This code could be made
    // more concise by assuming the vertex attributes are where we asked
    // the compiler to put them.
    int color_loc       = glGetAttribLocation(shader_prog, "color");
    int matrix_loc      = glGetAttribLocation(shader_prog, "model_matrix");

    // Load the object
    object.LoadFromVBM("../../media/armadillo_low.vbm", 
        glGetAttribLocation(shader_prog, "position"), 
        glGetAttribLocation(shader_prog, "normal"),
        -1);  // our shader doesn't use texture coordinates

    // Bind its vertex array object so that we can append the instanced attributes
    object.BindVertexArray();

    // Configure the regular vertex attribute arrays - position and color.
    /*
    // This is commented out here because the VBM object takes care
    // of it for us.
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glVertexAttribPointer(position_loc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(position_loc);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(normal_loc);
    */

    // Generate the colors of the objects
    const int NUM_COLORS = 7;
    vec4 color_table[NUM_COLORS] = {
        vec4(1.0, 0.0, 0.0, 1.0),
        vec4(0.0, 1.0, 0.0, 1.0),
        vec4(1.0, 1.0, 0.0, 1.0),
        vec4(0.0, 0.0, 1.0, 1.0),
        vec4(1.0, 0.0, 1.0, 1.0),
        vec4(0.0, 1.0, 1.0, 1.0),
        vec4(1.0, 1.0, 1.0, 1.0)
    };

    vec4 colors[INSTANCE_COUNT];
    for (int n = 0; n < INSTANCE_COUNT; ++n)
    {
        colors[n] = color_table[n % NUM_COLORS];
    }

    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);

    // Now we set up the color array. We want each instance of our geometry
    // to assume a different color, so we'll just pack colors into a buffer
    // object and make an instanced vertex attribute out of it.
    glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(color_loc);
    // This is the important bit... set the divisor for the color array to
    // 1 to get OpenGL to give us a new value of 'color' per instance
    // rather than per vertex.
    glVertexAttribDivisor(color_loc, 1);

    // Likewise, we can do the same with the model matrix. Note that a
    // matrix input to the vertex shader consumes N consecutive input
    // locations, where N is the number of columns in the matrix. So...
    // we have four vertex attributes to set up.
    glGenBuffers(1, &model_matrix_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, model_matrix_buffer);
    glBufferData(GL_ARRAY_BUFFER, INSTANCE_COUNT * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);


    // Set up the vertex attribute
    // Loop over each column of the matrix...
    for (int i = 0; i < 4; i++)
    {
        // Set up the vertex attribute
        glVertexAttribPointer(matrix_loc + i,              // Location
                              4, GL_FLOAT, GL_FALSE,       // vec4
                              sizeof(mat4),                // Stride
                              (void *)(sizeof(vec4) * i)); // Start offset
        // Enable it
        glEnableVertexAttribArray(matrix_loc + i);
        // Make it instanced
        glVertexAttribDivisor(matrix_loc + i, 1);
    }

    // Done (unbind the object's VAO)
    glBindVertexArray(0);

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
    glDeleteBuffers(1, &color_buffer);
    glDeleteBuffers(1, &model_matrix_buffer);
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
