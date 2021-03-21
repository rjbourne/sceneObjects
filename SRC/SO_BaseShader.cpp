/** \file SO_BaseShader.cpp */
#include "sceneObjects.hpp"

//compile a vertex shader from the given char*
void sceneObjects::SO_BaseShader::createVertexShader(const char* vertexSource) {
    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexSource, NULL);
    glCompileShader(vertexShaderID);

    GLint status;
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) {
        printf("vertex shader %d compiled successfully\n", vertexShaderID);
    } else {
        char buffer[512];
        glGetShaderInfoLog(vertexShaderID, 512, NULL, buffer);    
        printf(vertexSource);
        printf("\nvertex shader %d failed to compile\n", vertexShaderID);
        printf("%s\n", buffer);
    }
}

//compile a fragment shader from the given char*
void sceneObjects::SO_BaseShader::createFragmentShader(const char* fragmentSource) {
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShaderID);
    GLint status;
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) {
        printf("fragment shader %d compiled successfully\n", fragmentShaderID);
    } else {
        char buffer[512];
        glGetShaderInfoLog(fragmentShaderID, 512, NULL, buffer);
        printf(fragmentSource);    
        printf("\nfragment shader %d failed to compile\n", fragmentShaderID);
        printf("%s\n", buffer);
    }
}

//link together the previously compiled shaders into a program
void sceneObjects::SO_BaseShader::linkProgram(void) {
    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glBindFragDataLocation(programID, 0, "outColor");
    glLinkProgram(programID);
    GLint status;
    glGetProgramiv(programID, GL_LINK_STATUS, &status);
    if (status == GL_TRUE) {
        printf("program %d linked successfully\n", programID);
    } else {
        char buffer[512];
        glGetProgramInfoLog(programID, 512, NULL, buffer);    
        printf("program %d failed to link\n", programID);
        printf("%s\n", buffer);
    }
    programCreated = true;
    glDeleteShader(fragmentShaderID);
    glDeleteShader(vertexShaderID);
}

//returns the ID of the program associated with this class
GLuint sceneObjects::SO_BaseShader::getProgramID(void) {
    return programID;
}

//destructor deletes the opengl program
sceneObjects::SO_BaseShader::~SO_BaseShader(void) {
    if (programCreated) {
        glDeleteProgram(programID); //destructor deletes program
    }
}
