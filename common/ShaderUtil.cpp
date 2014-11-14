///////////////////////////////////////////////////////////////////////////////
//
// File Name: ShaderUtil.cpp
//
// Purpose: This file contains the definition of the ShaderUtil class. The
//          ShaderUtil class is a collection of utilities that simplify the
//          process of constructing GLSL shaders.
//
///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include "ShaderUtil.h"


///////////////////////////////////////////////////////////////////////////////
// Function Name: ShaderUtil
//
// Purpose: Initializes ShaderUtil data at instantiation.
// 
// INPUTS: None.
//
// OUTPUTS: None.
//
///////////////////////////////////////////////////////////////////////////////
ShaderUtil::ShaderUtil(void)
{
}



///////////////////////////////////////////////////////////////////////////////
// Function Name: ~ShaderUtil
//
// Purpose: Cleans up any memory allocated at run time.
// 
// INPUTS: None.
//
// OUTPUTS: None.
//
///////////////////////////////////////////////////////////////////////////////
ShaderUtil::~ShaderUtil(void)
{
}



///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////
GLuint ShaderUtil::LoadShaders(const char *vertexSource, const char *fragmentSource)
{
    if (!vertexSource || !fragmentSource) return 0;

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vShader, 1, &vertexSource, NULL);
    glCompileShader(vShader);
    GLint status;
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &status);

    if ( !status )
    {
#ifdef _DEBUG
        GLsizei len;
        glGetShaderiv( vShader, GL_INFO_LOG_LENGTH, &len );

        GLchar* log = new GLchar[len+1];
        glGetShaderInfoLog( vShader, len, &len, log );
        std::cerr << "Shader compilation failed: " << log << std::endl;
        delete [] log;
#endif /* DEBUG */
        return 0;
    }

    glShaderSource(fShader, 1, &fragmentSource, NULL);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &status);

    if ( !status )
    {
#ifdef _DEBUG
        GLsizei len;
        glGetShaderiv( fShader, GL_INFO_LOG_LENGTH, &len );

        GLchar* log = new GLchar[len+1];
        glGetShaderInfoLog( fShader, len, &len, log );
        std::cerr << "Shader compilation failed: " << log << std::endl;
        delete [] log;
#endif /* DEBUG */
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if ( !status )
    {
#ifdef _DEBUG
        GLsizei len;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );

        GLchar* log = new GLchar[len+1];
        glGetProgramInfoLog( program, len, &len, log );
        std::cerr << "Shader linking failed: " << log << std::endl;
        delete [] log;
#endif 
        return 0;
    }

    return program;
}



///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////
GLuint ShaderUtil::LoadShaders(ShaderInfo* shaders)
{
    if (!shaders) return 0;

    GLuint program = glCreateProgram();

    ShaderInfo* entry = shaders;
    while (entry->type != GL_NONE)
    {
        GLuint shader = glCreateShader( entry->type );

        entry->shader = shader;

        const GLchar* source = ReadShader( entry->filename );
        if ( source == NULL ) 
        {
            for ( entry = shaders; entry->type != GL_NONE; ++entry )
            {
                glDeleteShader( entry->shader );
                entry->shader = 0;
            }

            return 0;
        }

        glShaderSource( shader, 1, &source, NULL );
        delete [] source;

        glCompileShader( shader );

        GLint compiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled )
        {
#ifdef _DEBUG
            GLsizei len;
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );

            GLchar* log = new GLchar[len+1];
            glGetShaderInfoLog( shader, len, &len, log );
            std::cerr << "Shader compilation failed: " << log << std::endl;
            delete [] log;
#endif /* DEBUG */

            return 0;
        }

        glAttachShader( program, shader );

        ++entry;
    }

    glLinkProgram( program );

    GLint linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked )
    {
#ifdef _DEBUG
        GLsizei len;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );

        GLchar* log = new GLchar[len+1];
        glGetProgramInfoLog( program, len, &len, log );
        std::cerr << "Shader linking failed: " << log << std::endl;
        delete [] log;
#endif /* DEBUG */

        for ( entry = shaders; entry->type != GL_NONE; ++entry )
        {
            glDeleteShader( entry->shader );
            entry->shader = 0;
        }

        return 0;
    }

    return program;
}



///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////
const GLchar *ShaderUtil::ReadShader(const char* filename)
{
	std::ifstream file(filename, std::ios::binary);

    if (!file)
    {
#ifdef _DEBUG
        std::cerr << "Unable to open file '" << filename << "'" << std::endl;
#endif /* DEBUG */
        return NULL;
    }

    file.seekg (0, file.end);
    int len = (int)file.tellg();
    file.seekg (0, file.beg);
    
    GLchar* source = new GLchar[len+1];

    file.read(source, len);
    file.close();

    source[len] = 0;

    return const_cast<const GLchar*>(source);
}

