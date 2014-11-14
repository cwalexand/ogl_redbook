///////////////////////////////////////////////////////////////////////////////
//
// File Name: main.cpp
//
// Purpose: This file contains the program entry point. We use GLEW and
//          GLUT to set up the application framework. 
//
///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "GL/glew.h"
#include "GL/freeglut.h"

#define ESC 0x1B

// These GLUT callback functions are implemented by the various projects...
extern void display();
extern void reshape(int width, int height);

// A couple more 'virtual' functions to help projects set up and clean up...
extern void initialize();
extern void finalize();



///////////////////////////////////////////////////////////////////////////////
// Function Name: keyboard
//
// Purpose: GLUT keyboard callback for the current window.
// 
// INPUTS: key - user generated ASCII character
//
//         x, y - the mouse location in window relative coordinates when the
//                key was pressed
//
// OUTPUTS: None.
//
///////////////////////////////////////////////////////////////////////////////
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {

    case ESC:
        finalize();
        exit(EXIT_SUCCESS);
        break;
    }
}



///////////////////////////////////////////////////////////////////////////////
// Function Name: idle
//
// Purpose: GLUT global idle callback. This function is used to make GLUT
//          execute the display callback for continuous animation when window
//          system events are not being received.
// 
// INPUTS: None.
//
// OUTPUTS: None.
//
///////////////////////////////////////////////////////////////////////////////
void idle()
{
    glutPostRedisplay();
}



///////////////////////////////////////////////////////////////////////////////
// Function Name: main
//
// Purpose: Sets up GLUT, initializes GLEW, registers GLUT callback functions,
//          and drops into the GLUT main loop.
// 
// INPUTS: argc - contains the number of arguments
//         argv - "argument vector", a one-dimensional array of strings
//
// OUTPUTS: Returns EXIT_FAILURE if there is trouble with GLEW, otherwise
//          returns EXIT_SUCCESS.
//
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(640, 480);
    //glutInitContextVersion(2, 1);
    //glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("OpenGL Redbook");

    if (glewInit())
    {
#ifdef _DEBUG
        std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
#endif /* DEBUG */

        exit(EXIT_FAILURE);
    }

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    
    initialize();

    glutMainLoop();

    return EXIT_SUCCESS;
}
