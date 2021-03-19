//includes
#define _USE_MATH_DEFINES
#define GLEW_STATIC

#include "sceneObjects.hpp"
#include <cstdio>
#include <cmath>
#include <chrono>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int WIDTH = 800;
int HEIGHT = 800;
float FOV = 45.0f;

std::vector<std::string> files {
    "assets\\right.jpg",
    "assets\\left.jpg",
    "assets\\top.jpg",
    "assets\\bottom.jpg",
    "assets\\front.jpg",
    "assets\\back.jpg"
};

//camera
sceneObjects::SO_Camera cameraObj = sceneObjects::SO_Camera(FOV, WIDTH/HEIGHT, 0.1f, 100.0f, glm::vec3(6.0f, 0.0f, 6.0f), glm::vec3(-3.0f, -3.0f, -3.0f), glm::vec3(0.0f, 0.0f, 1.0f));;

void setFOV(sceneObjects::SO_Camera *camera) {
    if (camera->aspectRatio > 1) {
        camera->fov = FOV;
    } else {
        camera->fov = glm::degrees(atan(tan(glm::radians(FOV)/2) * (1/camera->aspectRatio)) * 2);
    }
}

//zooming
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    FOV -= yoffset;
    if (FOV  < 10.0) {
        FOV  = 10.0;
    } else if (FOV  > 90.0) {
        FOV  = 90.0;
    }
    setFOV(&cameraObj);
    cameraObj.updateProjectionMatrix();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    cameraObj.aspectRatio = (float)width/(float)height;
    setFOV(&cameraObj);
    cameraObj.updateProjectionMatrix();
}

int main(int argc, char *argv[]) {

    //set up window
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //glfwWindowHint(GLFW_FLOATING, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    glewExperimental = GL_TRUE;
    glewInit();

    sceneObjects::SO_SkyboxShader skyboxObj;
    skyboxObj.generate(files);
    skyboxObj.setModelMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

    cameraObj.linkShader(&skyboxObj);
    cameraObj.updateViewMatrix();
    cameraObj.updateProjectionMatrix();

    double mouseSpeedX = -50; // camera controls
    double mouseSpeedY = -50;
    double anglePhi = 120.0;
    double angleTheta = 90.0;
    bool mouseDown = false;
    float dist = 4.0f;
    double xpos;
    double ypos;

    // set up camera position
    cameraObj.position = glm::vec3(dist*cos(glm::radians(anglePhi))*sin(glm::radians(angleTheta)), 
                                    dist*sin(glm::radians(anglePhi))*sin(glm::radians(angleTheta)), 
                                    dist*cos(glm::radians(angleTheta)));
    cameraObj.front = -cameraObj.position;
    cameraObj.updateViewMatrix();
    glfwSetScrollCallback(window, scroll_callback);

    // enable opengl options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    double time = glfwGetTime(); // timing
    while(!glfwWindowShouldClose(window))
    {
        double deltaT = glfwGetTime() - time; // get frame time
        time = glfwGetTime();
         //handle user input
        if (mouseDown) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
                mouseDown = false;
            } else {
                double newx;
                double newy;
                glfwGetCursorPos(window, &newx, &newy);
                anglePhi -= mouseSpeedX * (newx - xpos) * deltaT;
                angleTheta -= mouseSpeedY * (newy - ypos) * deltaT;
                if (angleTheta < 0.1) {
                    angleTheta = 0.1;
                } else if (angleTheta > 180) {
                    angleTheta = 180;
                }
                cameraObj.position = glm::vec3(dist*cos(glm::radians(anglePhi))*sin(glm::radians(angleTheta)), 
                                    dist*sin(glm::radians(anglePhi))*sin(glm::radians(angleTheta)), 
                                    dist*cos(glm::radians(angleTheta)));
                cameraObj.front = -cameraObj.position;
                cameraObj.updateViewMatrix();
                xpos = newx;
                ypos = newy;
            }
        } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            mouseDown = true;
            glfwGetCursorPos(window, &xpos, &ypos);
        }

        //render scene
        //printf("%.5f\t", glfwGetTime() - time);
        glClear(GL_DEPTH_BUFFER_BIT);
 
        skyboxObj.render();
        glfwSwapBuffers(window);
        glfwPollEvents();
        //printf("%.5f\n", glfwGetTime() - time);
    }

    //if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    //    glfwSetWindowShouldClose(window, GL_TRUE);

    //clear up

    glfwTerminate();

    return 0;
}