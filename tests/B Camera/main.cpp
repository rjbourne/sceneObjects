//includes
#include "..\\..\\sceneObjects.hpp"
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


int main(int argc, char *argv[]) {


    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f

    };

    int elements[] = {
        0, 1, 2,
        2, 3, 0,
        4, 5, 6,
        6, 7, 4,
        8, 9, 10,
        10, 11, 8,
        12, 13, 14,
        14, 15, 12,
        16, 17, 18,
        18, 19, 16,
        20, 21, 22,
        22, 23, 20,
    };

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



    GLuint shaderProgram;
    SO_PhongShader shaderObj;
    shaderProgram = shaderObj.generate(1, SO_COLOR_ATTRIBUTE);
    glUseProgram(shaderProgram); // use the shader program

    shaderObj.setLightPosition(0, glm::vec3(2.0f, 0.0f, 2.0f));
    shaderObj.setLightConstant(0, 1.0f);
    shaderObj.setLightLinear(0, 0.0f);
    shaderObj.setLightQuadratic(0, 0.0f);
    shaderObj.setLightAmbient(0, glm::vec3(0.1f, 0.1f, 0.1f));
    shaderObj.setLightDiffuse(0, glm::vec3(1.0f, 1.0f, 1.0f));
    shaderObj.setLightSpecular(0, glm::vec3(0.5f, 0.5f, 0.5f));

    shaderObj.setModelMatrix(glm::mat4(1.0f));
    
    SO_Camera cameraObj = SO_Camera(45.0f, 1.0f, 0.1f, 10.0f, glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(-2.0f, -2.0f, -2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    cameraObj.linkShader(&shaderObj);
    cameraObj.updateViewMatrix();
    cameraObj.updateProjectionMatrix();


    glUseProgram(shaderProgram);

    // the vertex array object for the scene
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    GLuint vbo; // apply vertices to the vbo
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint ebo; // apply triangle elements to vbo
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    //locate vertex coords within buffer
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
                        9*sizeof(float), 0);
    
    //locate vertex normal within buffer
    GLint normalAttrib = glGetAttribLocation(shaderProgram, "normal");
    glEnableVertexAttribArray(normalAttrib);
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE,
                        9*sizeof(float), (void*)(3*sizeof(float)));

    //locate vertex colors within buffer
    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
                        9*sizeof(float), (void*)(6*sizeof(float)));



    glEnable( GL_DEPTH_TEST );
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

    while(!glfwWindowShouldClose(window))
    {
        double time = glfwGetTime();
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, sizeof(elements), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
        printf("%.5f\n", glfwGetTime() - time);
    }

    //if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    //    glfwSetWindowShouldClose(window, GL_TRUE);

    //clear up

    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);

    glDeleteVertexArrays(1, &vao);

    glfwTerminate();

    return 0;
}