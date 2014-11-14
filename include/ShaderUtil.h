///////////////////////////////////////////////////////////////////////////////
//
// File Name: ShaderUtil.h
//
// Purpose: This file contains the declaration of the ShaderUtil class. The
//          ShaderUtil class is a collection of utilities that simplify the
//          process of constructing GLSL shaders.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __SHADERUTIL_H
#define __SHADERUTIL_H

#include "GL/glew.h"


// Use ShaderInfo something like this:
// ShaderInfo shader_info[] =
// {
//     { GL_VERTEX_SHADER, "vertexShader.glsl", 0 },
//     { GL_FRAGMENT_SHADER, "fragmentShader.glsl", 0 },
//     { GL_NONE, NULL, 0 }
// };
struct ShaderInfo
{
    GLenum       type;
    const char*  filename;
    GLuint       shader;
};


class ShaderUtil
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Function Name: ShaderUtil
    //
    // Purpose: Initializes ShaderUtil data at instantiation.
    // 
    // INPUTS: None.
    //
    // OUTPUTS: None.
    //
    ///////////////////////////////////////////////////////////////////////////
    ShaderUtil(void);

    ///////////////////////////////////////////////////////////////////////////
    // Function Name: ~ShaderUtil
    //
    // Purpose: Cleans up any memory allocated at run time.
    // 
    // INPUTS: None.
    //
    // OUTPUTS: None.
    //
    ///////////////////////////////////////////////////////////////////////////
    ~ShaderUtil(void);

    ///////////////////////////////////////////////////////////////////////////
    // Function Name: LoadShaders
    //
    // Purpose: Compiles and links a shader program that consists of a vertex
    //          shader and a fragment shader.
    // 
    // INPUTS: vertexSource   - pointer to the vertex shader code string
    //         fragmentSource - pointer to the fragment shader code string
    //
    // OUTPUTS: Returns 0 if something went wrong, otherwise returns the handle
    //          of the shader program.
    //
    ///////////////////////////////////////////////////////////////////////////
    GLuint LoadShaders(const char*vertexSource, const char *fragmentSource);

    ///////////////////////////////////////////////////////////////////////////
    // Function Name: LoadShaders
    //
    // Purpose: Compiles and links a shader program that consists of a collection
    //          of shaders to load from external files.
    // 
    // INPUTS: shaders - A list of shaders to load from external files, compile,
    //                   and link. For example, ShaderInfo could look something
    //                   like this:
    //
    //                   ShaderInfo shader_info[] =
    //                   {
    //                       { GL_VERTEX_SHADER, "vertexShader.glsl", 0 },
    //                       { GL_FRAGMENT_SHADER, "fragmentShader.glsl", 0 },
    //                       { GL_NONE, NULL, 0 }
    //                   };
    //
    // OUTPUTS: Returns 0 if something went wrong, otherwise returns the handle
    //          of the shader program.
    //
    ///////////////////////////////////////////////////////////////////////////
    GLuint LoadShaders(ShaderInfo* shaders);

private:
    ///////////////////////////////////////////////////////////////////////////
    // Function Name: ReadShader
    //
    // Purpose: Reads shader source from an external file.
    // 
    // INPUTS: filename - the name of the file that contains the shader code.
    //
    // OUTPUTS: Returns NULL if something went wrong, otherwise returns a pointer
    //          to a string that contains the shader source. 
    //
    // NOTES: The calling function is responsible for deleting the shader source
    //        string.
    //
    ///////////////////////////////////////////////////////////////////////////
    const GLchar *ReadShader(const char* filename);

};

#endif // __SHADERUTIL_H
