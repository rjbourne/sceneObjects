//includes
#include <sceneObjects.hpp>
#include <sceneModels.hpp>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace sceneObjects;

int WIDTH = 800;
int HEIGHT = 800;
float FOV = 45.0f;

SO_Camera cameraObj = SO_Camera(45.0f, 1.0f, 0.1f, 10.0f, glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(-2.0f, -2.0f, -2.0f), glm::vec3(0.0f, 0.0f, 1.0f));

void setFOV(SO_Camera *camera) {
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

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    SO_AssimpModel cube = SO_AssimpModel(".\\assets\\cube.obj",
                                        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);
    cube.createShaders(1);
    for (int i = 0; i < cube.meshes.size(); i++) {
        cube.meshes[i].shader.setLightPosition(0, glm::vec3(2.0f, 3.0f, 4.0f));
        cube.meshes[i].shader.setLightConstant(0, 1.0f);
        cube.meshes[i].shader.setLightLinear(0, 0.0f);
        cube.meshes[i].shader.setLightQuadratic(0, 0.0f);
        cube.meshes[i].shader.setLightAmbient(0, glm::vec3(0.2f, 0.2f, 0.2f));
        cube.meshes[i].shader.setLightDiffuse(0, glm::vec3(1.0f, 1.0f, 1.0f));
        cube.meshes[i].shader.setLightSpecular(0, glm::vec3(0.5f, 0.5f, 0.5f));

        cube.meshes[i].shader.setModelMatrix(glm::mat4(1.0f));
        cameraObj.linkShader(&cube.meshes[i].shader);
    }

    double mouseSpeedX = 1; // camera controls
    double mouseSpeedY = 1;
    double anglePhi = 0.0;
    double angleTheta = 90.0;
    bool mouseDown = false;
    float dist = 8.0f;
    double xpos;
    double ypos;
    cameraObj.position = glm::vec3(dist*cos(glm::radians(anglePhi))*sin(glm::radians(angleTheta)), 
                                    dist*sin(glm::radians(anglePhi))*sin(glm::radians(angleTheta)), 
                                    dist*cos(glm::radians(angleTheta)));
    cameraObj.front = -cameraObj.position;
    cameraObj.updateViewMatrix();
    cameraObj.updateProjectionMatrix();

    glEnable( GL_DEPTH_TEST );
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    double time = glfwGetTime(); // timing
    while(!glfwWindowShouldClose(window))
    {
        double deltaT = glfwGetTime() - time; // get frame time
        time = glfwGetTime();
        if (mouseDown) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
                mouseDown = false;
            } else {
                double newx;
                double newy;
                glfwGetCursorPos(window, &newx, &newy);
                anglePhi -= mouseSpeedX * (newx - xpos);
                angleTheta -= mouseSpeedY * (newy - ypos);
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

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        cube.render();
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