//includes
#include <sceneObjects.hpp>
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
int winWidth = 800;
int winHeight = 800;

int main(int argc, char *argv[]) {


    //set up window
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, "OpenGL", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    std::vector<float> vertices;
    vertices.resize(WIDTH*HEIGHT*3);
    for (int w = 0; w < WIDTH; w++) {
        for (int h = 0; h < HEIGHT; h++) {
            int start = 3*(w+h*WIDTH);
            vertices[start + 0] = -1.0f + 2.0f*((float)w/(WIDTH-1));
            vertices[start + 1] = -1.0f + 2.0f*((float)h/(HEIGHT-1));
            vertices[start + 2] = perlin(vertices[start + 0]*20, vertices[start + 1]*20, 0.0, 0);
        }
    }

    std::vector<int> elements;
    elements.resize(6*(WIDTH - 1)*(HEIGHT - 1));
    for (int w = 0; w < WIDTH - 1; w++) {
        for (int h = 0; h < HEIGHT - 1; h++) {
            int start = 6*(w+h*(WIDTH-1));
            elements[start + 0] = w+h*WIDTH;
            elements[start + 1] = (w+1)+h*WIDTH;
            elements[start + 2] = w+(h+1)*WIDTH;

            elements[start + 3] = w+(h+1)*WIDTH;
            elements[start + 4] = (w+1)+(h+1)*WIDTH;
            elements[start + 5] = (w+1)+h*WIDTH;
        }
    }


    const char* vertexSource = R"glsl(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in float color;

        out float Color;

        void main() {
            Color = color;
            gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
        }

    )glsl";

    const char* fragmentSource = R"glsl(
        #version 330 core

        in float Color;

        out vec4 outColor;

        void main() {
            outColor = vec4(Color, Color, Color, 1.0);
        }

    )glsl";

    SO_Shader shaderObj;
    shaderObj.createVertexShader(vertexSource);
    shaderObj.createFragmentShader(fragmentSource);
    shaderObj.linkProgram();

    glUseProgram(shaderObj.getProgramID());

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(int), &elements[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(0*sizeof(float)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(2*sizeof(float)));



    //glEnable( GL_DEPTH_TEST );
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    double time = glfwGetTime(); // timing
    while(!glfwWindowShouldClose(window))
    {
        double deltaT = glfwGetTime() - time; // get frame time
        time = glfwGetTime();
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
        //printf("%.5f\n", glfwGetTime() - time);
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