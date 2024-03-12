// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#include "ShaderProgram.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <exception>
#include <ios>

#include "Error.h"

using namespace std;

// Create a GPU program i.e., a graphics pipeline
ShaderProgram::ShaderProgram (const std::string & name) : m_id (glCreateProgram ()), m_name (name) {}


ShaderProgram::~ShaderProgram () {
	glDeleteProgram (m_id); 
}

void ShaderProgram::loadShader (GLenum type, const std::string & shaderFilename) {
	GLuint shader = glCreateShader (type); // Create the shader, e.g., a vertex shader to be applied to every single vertex of a mesh
	std::string shaderSourceString = file2String (shaderFilename); // Loads the shader source from a file to a C++ string
	const GLchar * shaderSource = (const GLchar *)shaderSourceString.c_str (); // Interface the C++ string through a C pointer
	glShaderSource (shader, 1, &shaderSource, NULL); // Load the shader source code
	glCompileShader (shader);  // The GPU driver compile the shader
	printOpenGLError ("Compiling Shader " + shaderFilename);  // Check for OpenGL errors
    GLint shaderCompiled;
    glGetShaderiv (shader, GL_COMPILE_STATUS, &shaderCompiled);
    printOpenGLError ("Compiling Shader " + shaderFilename);  // Check for OpenGL errors
    if (!shaderCompiled)
    	exitOnCriticalError ("Error: shader not compiled. Info. Log.:\n" + shaderInfoLog (shaderFilename, shader) + "\nSource:\n" + shaderSource);
	glAttachShader (m_id, shader); // Set the vertex shader as the one ot be used with the program/pipeline
	glDeleteShader (shader);
}

void ShaderProgram::link () { 
	glLinkProgram (m_id); 
	printOpenGLError ("Linking Program " + name ());
    GLint linked;
    glGetProgramiv (m_id, GL_LINK_STATUS, &linked);
    if (!linked)
        exitOnCriticalError ("Shader program not linked: " + infoLog ());
}


std::shared_ptr<ShaderProgram> ShaderProgram::genBasicShaderProgram (const std::string & vertexShaderFilename,
															 	 	 const std::string & fragmentShaderFilename) {
	std::string shaderProgramName =  "Shader Program <" + vertexShaderFilename + " - " + fragmentShaderFilename + ">";
	std::shared_ptr<ShaderProgram> shaderProgramPtr = std::make_shared<ShaderProgram> (shaderProgramName);
	shaderProgramPtr->loadShader (GL_VERTEX_SHADER, vertexShaderFilename);
	shaderProgramPtr->loadShader (GL_FRAGMENT_SHADER, fragmentShaderFilename);
	shaderProgramPtr->link ();
	return shaderProgramPtr;
}

std::string ShaderProgram::file2String (const std::string & filename) {
	std::ifstream input (filename.c_str ());
	if (!input)
		throw std::ios_base::failure ("[Shader Program][file2String] Error: cannot open " + filename);
	std::stringstream buffer;
	buffer << input.rdbuf ();
    return buffer.str ();
}

std::string ShaderProgram::shaderInfoLog (const std::string & shaderName, GLuint shaderId) {
    std::string infoLogStr = "";
    int infologLength = 0;
    glGetShaderiv (shaderId, GL_INFO_LOG_LENGTH, &infologLength);
    printOpenGLError ("Gathering Shader InfoLog Length for " + shaderName);
    if (infologLength > 0) {
        GLchar *str = new GLchar[infologLength];
        int charsWritten  = 0;
        glGetShaderInfoLog (shaderId, infologLength, &charsWritten, str);
        printOpenGLError ("Gathering Shader InfoLog for " + shaderName);
        infoLogStr  = std::string (str);
        delete [] str;
    }
    return infoLogStr;
}

std::string ShaderProgram::infoLog () {
    std::string infoLogStr = "";
    int infologLength = 0;
    glGetProgramiv (m_id, GL_INFO_LOG_LENGTH, &infologLength);
    printOpenGLError ("Gathering InfoLog for Program " + name ());
    if (infologLength > 0) {
        GLchar *str = new GLchar[infologLength];
        int charsWritten  = 0;
        glGetProgramInfoLog (m_id, infologLength, &charsWritten, str);
        printOpenGLError ("Gathering InfoLog for Program " + name ());
        infoLogStr  = std::string (str);
        delete [] str;
    }
    return infoLogStr;
}