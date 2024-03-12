// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#include "Error.h"

#include <iostream>
#include <cstdlib>
#include <string>

#include "Console.h"

/*
	IMPORTANT MESSAGE

	The following code is mostly commented so that MyRenderer can run 
	under OpenGL 4.1 and be compatible with MacOS devices.
	If you want more accurate debug trace, you shall uncomment the
	following code and change 
	 	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
	by
	 	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 6);
	in Main.cpp. This will work if under Linux or Windows. 
*/


void debugMessageCallback (GLenum source,
    					   GLenum type,
    					   GLuint id,
    					   GLenum severity,
    					   GLsizei length,
    					   const GLchar* message,
    					   const void* userParam) {
	/*
	std::string sourceString ("Unknown");
	if (source == GL_DEBUG_SOURCE_API) 
		sourceString = "API";
	else if (source  == GL_DEBUG_SOURCE_WINDOW_SYSTEM)
		sourceString = "Window system API";
	else if (source  == GL_DEBUG_SOURCE_SHADER_COMPILER)
		sourceString = "Shading laguage compiler";
	else if (source  == GL_DEBUG_SOURCE_THIRD_PARTY)
		sourceString = "Application associated with OpenGL";
	else if (source  == GL_DEBUG_SOURCE_APPLICATION)
		sourceString = "User generated";
	else if (source  == GL_DEBUG_SOURCE_OTHER)
		sourceString = "Other";	
	
	std::string severityString ("Unknown");
	
	if (severity == GL_DEBUG_SEVERITY_HIGH)
		severityString = "High";
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
		severityString = "Medium";
	else if (severity == GL_DEBUG_SEVERITY_LOW)
		severityString = "Low";
	else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		severityString = "Nofitication";
	
	
	std::string typeString ("Unknown");
	if (type == GL_DEBUG_TYPE_ERROR)
		typeString = "Error";
	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
		typeString = "Deprecated behavior";
	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)	
		typeString = "Undefined behavior";
	else if (type == GL_DEBUG_TYPE_PORTABILITY)	
		typeString = "Portability issue";
	else if (type == GL_DEBUG_TYPE_PERFORMANCE)	
		typeString = "Performance issue";
	else if (type == GL_DEBUG_TYPE_MARKER)	
		typeString = "Command stream annotation";
	else if (type == GL_DEBUG_TYPE_PUSH_GROUP)
		typeString = "Group pushing";
	else if (type == GL_DEBUG_TYPE_POP_GROUP)
		typeString = "Group popping";
	else if (type == GL_DEBUG_TYPE_OTHER)
		typeString = "Other";
	
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION && severity != GL_DEBUG_SEVERITY_LOW) {
	
		Console::print (std::string("\n\t----------------") + "\n"
			  		+ "\t[OpenGL Callback Message]: " + ( type == GL_DEBUG_TYPE_ERROR ? std::string ("** CRITICAL **") : std::string ("** NON CRITICAL **") ) + "\n"
			  		+ "\t    source = " + sourceString + "\n"
			  		+ "\t    type = " + typeString + "\n"
			  		+ "\t    severity = " + severityString + "\n"
			  		+ "\t    message = " + message + "\n" + "\n"
					+ std::string("\t----------------") + "\n"); 
	
	} 
	
	if (type == GL_DEBUG_TYPE_ERROR) 
		std::exit (EXIT_FAILURE);
	*/
}

void exitOnCriticalError (const std::string & message) {
	Console::print (std::string("[Critical error]") + message +"\n");
	//std::cerr << "> [Clearing resources]" << std::endl;
	Console::print (std::string("[Quit]\n"));
	std::exit (EXIT_FAILURE);
}

void checkGLExceptions () {
    GLenum glerr = glGetError ();
    switch (glerr) {
        case GL_INVALID_ENUM:
            exitOnCriticalError ("Invalid Enum");
            break;
        case GL_INVALID_VALUE:
            exitOnCriticalError ("Invalid Value");
            break;
        case GL_INVALID_OPERATION:
            exitOnCriticalError ("Invalid Operation");
            break;
/*        case GL_STACK_OVERFLOW:
            exitOnCriticalError ("Stack Overflow");
            break;
        case GL_STACK_UNDERFLOW:
            exitOnCriticalError ("Stack Underflow");
            break;
 */
        case GL_OUT_OF_MEMORY:
            exitOnCriticalError ("Out of Memory");
            break;
        /*case GL_TABLE_TOO_LARGE:
            exitOnCriticalError ("Table Too Large");
            break;*/
        case GL_NO_ERROR:
            break;
        default:
            exitOnCriticalError ("Other Error");
            break;
    }
}

int printOglError (const std::string & msg, const char * file, int line) {
    GLenum glErr;
    int    retCode = 0;
    glErr = glGetError ();
    while (glErr != GL_NO_ERROR) {
        printf ("glError in file %s @ line %d: %s - %s\n", file, line, "gluErrorString(glErr)", msg.c_str ());
        retCode = 1;
        glErr = glGetError ();
    }
    return retCode;
}