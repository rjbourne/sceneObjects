/** \file SO_Shader.cpp */
#include "sceneObjects.hpp"

//link together the previously compiled shaders into a program
void sceneObjects::SO_Shader::linkProgram(void) {
    SO_BaseShader::linkProgram();
    modelMatrixLoc = glGetUniformLocation(this->getProgramID(), "model");
    viewMatrixLoc = glGetUniformLocation(this->getProgramID(), "view");
    projectionMatrixLoc = glGetUniformLocation(this->getProgramID(), "proj");
    viewPositionLoc = glGetUniformLocation(this->getProgramID(), "viewPos");
}

//applies the model matrix to the shader program
//model matrix transforms from modelspace to worldspace
void sceneObjects::SO_Shader::setModelMatrix(glm::mat4 modelMatrix) {
    glProgramUniformMatrix4fv(this->getProgramID(), modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

//applies the view matrix to the shader program
//view matrix transforms from worldspace to cameraspace
void sceneObjects::SO_Shader::setViewMatrix(glm::mat4 viewMatrix) {
    glProgramUniformMatrix4fv(this->getProgramID(), viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}

//set the position of the camera in the shader program in worldspace
//position of camera in worldspace
void sceneObjects::SO_Shader::setViewPosition(glm::vec3 viewPosition) {
    glProgramUniform3fv(this->getProgramID(), viewPositionLoc, 1, glm::value_ptr(viewPosition));
}


//applies the projection matrix to the shader program
//projection matrix transforms from cameraspace to clipspace
void sceneObjects::SO_Shader::setProjectionMatrix(glm::mat4 projectionMatrix) {
    glProgramUniformMatrix4fv(this->getProgramID(), projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}
