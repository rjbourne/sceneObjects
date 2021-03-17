#include "sceneObjects.hpp"


//compile a vertex shader from the given char*
void sceneObjects::SO_Shader::createVertexShader(const char* vertexSource) {
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
void sceneObjects::SO_Shader::createFragmentShader(const char* fragmentSource) {
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
void sceneObjects::SO_Shader::linkProgram(void) {
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
    modelMatrixLoc = glGetUniformLocation(programID, "model");
    viewMatrixLoc = glGetUniformLocation(programID, "view");
    projectionMatrixLoc = glGetUniformLocation(programID, "proj");
    programCreated = true;
    glDeleteShader(fragmentShaderID);
    glDeleteShader(vertexShaderID);
}

//returns the ID of the program associated with this class
GLuint sceneObjects::SO_Shader::getProgramID(void) {
    return programID;
}

//applies the model matrix to the shader program
//model matrix transforms from modelspace to worldspace
void sceneObjects::SO_Shader::setModelMatrix(glm::mat4 modelMatrix) {
    glProgramUniformMatrix4fv(programID, modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

//applies the view matrix to the shader program
//view matrix transforms from worldspace to cameraspace
void sceneObjects::SO_Shader::setViewMatrix(glm::mat4 viewMatrix) {
    glProgramUniformMatrix4fv(programID, viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}

//set the position of the camera in the shader program in worldspace
//method has no effect in SO::Shader base class
void sceneObjects::SO_Shader::setViewPosition(glm::vec3 viewPosition) {
    return;
}

//applies the projection matrix to the shader program
//projection matrix transforms from cameraspace to clipspace
void sceneObjects::SO_Shader::setProjectionMatrix(glm::mat4 projectionMatrix) {
    glProgramUniformMatrix4fv(programID, projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

//destructor deletes the opengl program
sceneObjects::SO_Shader::~SO_Shader(void) {
    if (programCreated) {
        glDeleteProgram(programID); //destructor deletes program
    }
}
