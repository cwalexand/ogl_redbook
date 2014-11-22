///////////////////////////////////////////////////////////////////////////////
//
// File Name: instancing.cpp
//
// Purpose: Implements Example 3.13, 'gl_InstanceID Example Vertex Shader', in 
//          The OpenGL Programming Guide, Eighth Edition.
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
static GLuint color_tbo;
static GLuint model_matrix_tbo;
static GLuint shader_prog;
static GLint view_matrix_loc;
static GLint projection_matrix_loc;
static GLuint color_tbo_loc;
static GLuint model_matrix_tbo_loc;
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

    // Set model matrices for each instance
    mat4 matrices[INSTANCE_COUNT];

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

    // Bind the model matrix VBO and change its data
    glBindBuffer(GL_TEXTURE_BUFFER, model_matrix_buffer);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(matrices), matrices, GL_DYNAMIC_DRAW);

    // Clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Activate instancing program
    glUseProgram(shader_prog);

    // Set the TBO samplers to the right texture unit indices
    glUniform1i(color_tbo_loc, 0);
    glUniform1i(model_matrix_tbo_loc, 1);

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
        { GL_VERTEX_SHADER, "../../shaders/instancing_tbo.vs.glsl", 0 },
        { GL_FRAGMENT_SHADER, "../../shaders/instancing.fs.glsl", 0 },
        { GL_NONE, NULL, 0 }
    };

    shader_prog = su.LoadShaders(shader_info);

    // Get the location of the projetion_matrix uniform
    view_matrix_loc = glGetUniformLocation(shader_prog, "view_matrix");
    projection_matrix_loc = glGetUniformLocation(shader_prog, "projection_matrix");

    // Set up the TBO samplers
    color_tbo_loc = glGetUniformLocation(shader_prog, "color_tbo");
    model_matrix_tbo_loc = glGetUniformLocation(shader_prog, "model_matrix_tbo");

    // Load the object
    object.LoadFromVBM("../../media/armadillo_low.vbm", 
        glGetAttribLocation(shader_prog, "position"), 
        glGetAttribLocation(shader_prog, "normal"), 
        -1);  // our shader doesn't use texture coordinates

    /*

        THIS IS COMMENTED OUT HERE BECAUSE THE VBM OBJECT TAKES
        CARE OF IT FOR US

    // Get the locations of the vertex attributes in 'shader_prog', which is the
    // (linked) program object that we're going to be rendering with. Note
    // that this isn't really necessary because we specified locations for
    // all the attributes in our vertex shader. This code could be made
    // more concise by assuming the vertex attributes are where we asked
    // the compiler to put them.
    int position_loc    = glGetAttribLocation(prog, "position");
    int normal_loc      = glGetAttribLocation(prog, "normal");

    // Configure the regular vertex attribute arrays - position and normal.
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glVertexAttribPointer(position_loc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(position_loc);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(normal_loc);

    */

    // Now we set up the TBOs for the instance colors and the model matrices...

    // Create the TBO to store colors
    glGenTextures(1, &color_tbo);
    glBindTexture(GL_TEXTURE_BUFFER, color_tbo);

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

    // Create the buffer, put color data in it, attach it to the TBO 
    // and initialize its format
    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_TEXTURE_BUFFER, color_buffer);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, color_buffer);

    // Now do the same thing with a TBO for the model matrices. The buffer object
    // (model_matrix_buffer) is created and sized to store one mat4 per instance.
    glGenTextures(1, &model_matrix_tbo);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, model_matrix_tbo);
    glGenBuffers(1, &model_matrix_buffer);
    glBindBuffer(GL_TEXTURE_BUFFER, model_matrix_buffer);
    glBufferData(GL_TEXTURE_BUFFER, INSTANCE_COUNT * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, model_matrix_buffer);

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
