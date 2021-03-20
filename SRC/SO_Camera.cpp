#include "sceneObjects.hpp"

sceneObjects::SO_Camera::SO_Camera(float fovIn, float aspectRatioIn, float nearClipIn, float farClipIn, glm::vec3 positionIn, glm::vec3 frontIn, glm::vec3 upIn) {
    fov = fovIn;
    aspectRatio = aspectRatioIn;
    nearClip = nearClipIn;
    farClip = farClipIn;
    position = positionIn;
    front = frontIn;
    up = upIn;
}

//update the view matrix of all linked shaders
void sceneObjects::SO_Camera::updateViewMatrix(void) {
    glm::mat4 viewMatrix = glm::lookAt(position, position+front, up);
    for (unsigned int i = 0; i < shaderPointers.size(); i++) {
        shaderPointers[i]->setViewMatrix(viewMatrix);
        shaderPointers[i]->setViewPosition(position);
    }
}

//update the projection matrix of all linked shaders
void sceneObjects::SO_Camera::updateProjectionMatrix(void) {
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
    for (unsigned int i = 0; i < shaderPointers.size(); i++) {
        shaderPointers[i]->setProjectionMatrix(projectionMatrix);
    }
}

//link the camera to the shader program given by pointer
void sceneObjects::SO_Camera::linkShader(sceneObjects::SO_Shader *shaderRefIn) {
    for (unsigned int i = 0; i < shaderPointers.size(); i++) {
        if (shaderPointers[i] == shaderRefIn) {
            return;
        }
    }
    shaderPointers.push_back(shaderRefIn);
}

//unlink the camera to the shader program given by pointer
void sceneObjects::SO_Camera::unlinkShader(sceneObjects::SO_Shader *shaderRefIn) {
    for (unsigned int i = 0; i < shaderPointers.size(); i++) {
        if (shaderPointers[i] == shaderRefIn) {
            shaderPointers.erase(shaderPointers.begin() + i);
            return;
        }
    }
}
