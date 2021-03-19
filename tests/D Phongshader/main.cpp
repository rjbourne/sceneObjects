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


    float vertices[] = {
    //   x      y      z      nx     ny     nz
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f

    };

    float verticesCol[] = {
    //   x      y      z      nx     ny     nz     R      G      B
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glewExperimental = GL_TRUE;
    glewInit();

///////////////////////////////////////////////////// coloured cube center

    GLuint shaderProgram1;
    SO_PhongShader shaderObj1;
    shaderProgram1 = shaderObj1.generate(1, SO_COLOR_ATTRIBUTE);
    glUseProgram(shaderProgram1); // use the shader program

    shaderObj1.setLightPosition(0, glm::vec3(3.0f, 4.0f, 2.0f));
    shaderObj1.setLightConstant(0, 1.0f);
    shaderObj1.setLightLinear(0, 0.0f);
    shaderObj1.setLightQuadratic(0, 0.0f);
    shaderObj1.setLightAmbient(0, glm::vec3(0.2f, 0.2f, 0.2f));
    shaderObj1.setLightDiffuse(0, glm::vec3(1.0f, 1.0f, 1.0f));
    shaderObj1.setLightSpecular(0, glm::vec3(0.5f, 0.5f, 0.5f));

    shaderObj1.setModelMatrix(glm::mat4(1.0f));
    
    cameraObj.linkShader(&shaderObj1);


    glUseProgram(shaderProgram1);

    // the vertex array object for the scene
    GLuint vao1;
    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);


    GLuint vbo1; // apply vertices to the vbo
    glGenBuffers(1, &vbo1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCol), verticesCol, GL_STATIC_DRAW);

    GLuint ebo1; // apply triangle elements to vbo
    glGenBuffers(1, &ebo1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    //locate vertex coords within buffer
    GLint posAttrib1 = glGetAttribLocation(shaderProgram1, "position");
    glEnableVertexAttribArray(posAttrib1);
    glVertexAttribPointer(posAttrib1, 3, GL_FLOAT, GL_FALSE,
                        9*sizeof(float), 0);
    
    //locate vertex normal within buffer
    GLint normalAttrib1 = glGetAttribLocation(shaderProgram1, "normal");
    glEnableVertexAttribArray(normalAttrib1);
    glVertexAttribPointer(normalAttrib1, 3, GL_FLOAT, GL_FALSE,
                        9*sizeof(float), (void*)(3*sizeof(float)));

    //locate vertex colors within buffer
    GLint colAttrib1 = glGetAttribLocation(shaderProgram1, "color");
    glEnableVertexAttribArray(colAttrib1);
    glVertexAttribPointer(colAttrib1, 3, GL_FLOAT, GL_FALSE,
                        9*sizeof(float), (void*)(6*sizeof(float)));

//////////////////////////////////////////////////////////////////// plain red cube

    GLuint shaderProgram2;
    SO_PhongShader shaderObj2;
    shaderProgram2 = shaderObj2.generate(1);
    glUseProgram(shaderProgram2); // use the shader program

    shaderObj2.setLightPosition(0, glm::vec3(3.0f, 4.0f, 2.0f));
    shaderObj2.setLightConstant(0, 1.0f);
    shaderObj2.setLightLinear(0, 0.0f);
    shaderObj2.setLightQuadratic(0, 0.0f);
    shaderObj2.setLightAmbient(0, glm::vec3(0.2f, 0.2f, 0.2f));
    shaderObj2.setLightDiffuse(0, glm::vec3(1.0f, 1.0f, 1.0f));
    shaderObj2.setLightSpecular(0, glm::vec3(0.5f, 0.5f, 0.5f));
    shaderObj2.setColor(glm::vec3(1.0f, 0.0f, 0.0f));

    shaderObj2.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.5f)));
    
    cameraObj.linkShader(&shaderObj2);


    glUseProgram(shaderProgram2);

    // the vertex array object for the scene
    GLuint vao2;
    glGenVertexArrays(1, &vao2);
    glBindVertexArray(vao2);


    GLuint vbo2; // apply vertices to the vbo
    glGenBuffers(1, &vbo2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint ebo2; // apply triangle elements to vbo
    glGenBuffers(1, &ebo2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    //locate vertex coords within buffer
    GLint posAttrib2 = glGetAttribLocation(shaderProgram2, "position");
    glEnableVertexAttribArray(posAttrib2);
    glVertexAttribPointer(posAttrib2, 3, GL_FLOAT, GL_FALSE,
                        6*sizeof(float), 0);
    
    //locate vertex normal within buffer
    GLint normalAttrib2 = glGetAttribLocation(shaderProgram2, "normal");
    glEnableVertexAttribArray(normalAttrib2);
    glVertexAttribPointer(normalAttrib2, 3, GL_FLOAT, GL_FALSE,
                        6*sizeof(float), (void*)(3*sizeof(float)));
    
//////////////////////////////////////////////////////////////////// transparent white cube

    GLuint shaderProgram3;
    SO_PhongShader shaderObj3;
    shaderProgram3 = shaderObj3.generate(1, SO_ALPHA);
    glUseProgram(shaderProgram3); // use the shader program

    shaderObj3.setLightPosition(0, glm::vec3(3.0f, 4.0f, 2.0f));
    shaderObj3.setLightConstant(0, 1.0f);
    shaderObj3.setLightLinear(0, 0.0f);
    shaderObj3.setLightQuadratic(0, 0.0f);
    shaderObj3.setLightAmbient(0, glm::vec3(0.2f, 0.2f, 0.2f));
    shaderObj3.setLightDiffuse(0, glm::vec3(1.0f, 1.0f, 1.0f));
    shaderObj3.setLightSpecular(0, glm::vec3(0.5f, 0.5f, 0.5f));
    shaderObj3.setColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.1f));

    shaderObj3.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.5f)));
    
    cameraObj.linkShader(&shaderObj3);


    glUseProgram(shaderProgram3);

    // the vertex array object for the scene
    GLuint vao3;
    glGenVertexArrays(1, &vao3);
    glBindVertexArray(vao3);


    GLuint vbo3; // apply vertices to the vbo
    glGenBuffers(1, &vbo3);
    glBindBuffer(GL_ARRAY_BUFFER, vbo3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint ebo3; // apply triangle elements to vbo
    glGenBuffers(1, &ebo3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    //locate vertex coords within buffer
    GLint posAttrib3 = glGetAttribLocation(shaderProgram3, "position");
    glEnableVertexAttribArray(posAttrib3);
    glVertexAttribPointer(posAttrib3, 3, GL_FLOAT, GL_FALSE,
                        6*sizeof(float), 0);
    
    //locate vertex normal within buffer
    GLint normalAttrib3 = glGetAttribLocation(shaderProgram3, "normal");
    glEnableVertexAttribArray(normalAttrib3);
    glVertexAttribPointer(normalAttrib3, 3, GL_FLOAT, GL_FALSE,
                        6*sizeof(float), (void*)(3*sizeof(float)));

///////////////////////////////////////////////////// ambient colours with blue diffuse and green specular

    GLuint shaderProgram4;
    SO_PhongShader shaderObj4;
    shaderProgram4 = shaderObj4.generate(1, SO_MATERIAL | SO_AMBIENT_ATTRIBUTE);
    glUseProgram(shaderProgram4); // use the shader program

    shaderObj4.setLightPosition(0, glm::vec3(3.0f, 4.0f, 2.0f));
    shaderObj4.setLightConstant(0, 1.0f);
    shaderObj4.setLightLinear(0, 0.0f);
    shaderObj4.setLightQuadratic(0, 0.0f);
    shaderObj4.setLightAmbient(0, glm::vec3(0.3f, 0.3f, 0.3f));
    shaderObj4.setLightDiffuse(0, glm::vec3(1.0f, 1.0f, 1.0f));
    shaderObj4.setLightSpecular(0, glm::vec3(0.5f, 0.5f, 0.5f));
    shaderObj4.setMaterialDiffuse(glm::vec3(0.0f, 0.0f, 0.4f));
    shaderObj4.setMaterialSpecular(glm::vec3(0.0f, 1.0f, 0.0f));

    shaderObj4.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0f)));
    
    cameraObj.linkShader(&shaderObj4);


    glUseProgram(shaderProgram4);

    // the vertex array object for the scene
    GLuint vao4;
    glGenVertexArrays(1, &vao4);
    glBindVertexArray(vao4);


    GLuint vbo4; // apply vertices to the vbo
    glGenBuffers(1, &vbo4);
    glBindBuffer(GL_ARRAY_BUFFER, vbo4);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCol), verticesCol, GL_STATIC_DRAW);

    GLuint ebo4; // apply triangle elements to vbo
    glGenBuffers(1, &ebo4);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo4);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    //locate vertex coords within buffer
    GLint posAttrib4 = glGetAttribLocation(shaderProgram4, "position");
    glEnableVertexAttribArray(posAttrib4);
    glVertexAttribPointer(posAttrib4, 3, GL_FLOAT, GL_FALSE,
                        9*sizeof(float), 0);
    
    //locate vertex normal within buffer
    GLint normalAttrib4 = glGetAttribLocation(shaderProgram4, "normal");
    glEnableVertexAttribArray(normalAttrib4);
    glVertexAttribPointer(normalAttrib4, 3, GL_FLOAT, GL_FALSE,
                        9*sizeof(float), (void*)(3*sizeof(float)));

    //locate vertex colors within buffer
    GLint colAttrib4 = glGetAttribLocation(shaderProgram4, "ambientAttrib");
    glEnableVertexAttribArray(colAttrib4);
    glVertexAttribPointer(colAttrib4, 3, GL_FLOAT, GL_FALSE,
                        9*sizeof(float), (void*)(6*sizeof(float)));

///////////////////////////////////////////////////// cyan icosphere

    GLuint shaderProgram5;
    SO_PhongShader shaderObj5;
    shaderProgram5 = shaderObj5.generate(1);
    glUseProgram(shaderProgram5); // use the shader program

    shaderObj5.setLightPosition(0, glm::vec3(3.0f, 4.0f, 2.0f));
    shaderObj5.setLightConstant(0, 1.0f);
    shaderObj5.setLightLinear(0, 0.0f);
    shaderObj5.setLightQuadratic(0, 0.0f);
    shaderObj5.setLightAmbient(0, glm::vec3(0.3f, 0.3f, 0.3f));
    shaderObj5.setLightDiffuse(0, glm::vec3(1.0f, 1.0f, 1.0f));
    shaderObj5.setLightSpecular(0, glm::vec3(0.5f, 0.5f, 0.5f));
    shaderObj5.setColor(glm::vec3(0.0f, 1.0f, 1.0f));

    shaderObj5.setModelMatrix(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.5f, 0.0f)), glm::vec3(0.5f, 0.5f, 0.5f)));
    
    cameraObj.linkShader(&shaderObj5);

    SO_MeshData sphereData = createIcosphere(5);

    glUseProgram(shaderProgram5);

    // the vertex array object for the scene
    GLuint vao5;
    glGenVertexArrays(1, &vao5);
    glBindVertexArray(vao5);


    GLuint vbo5; // apply vertices to the vbo
    glGenBuffers(1, &vbo5);
    glBindBuffer(GL_ARRAY_BUFFER, vbo5);
    glBufferData(GL_ARRAY_BUFFER, sphereData.vertices.size()*sizeof(float)*3, &sphereData.vertices[0].x, GL_STATIC_DRAW);

    GLuint ebo5; // apply triangle elements to vbo
    glGenBuffers(1, &ebo5);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo5);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereData.faceElements.size()*sizeof(int), &sphereData.faceElements[0], GL_STATIC_DRAW);

    //locate vertex coords within buffer
    GLint posAttrib5 = glGetAttribLocation(shaderProgram5, "position");
    glEnableVertexAttribArray(posAttrib5);
    glVertexAttribPointer(posAttrib5, 3, GL_FLOAT, GL_FALSE,
                        3*sizeof(float), 0);
    
    //locate vertex normal within buffer
    GLint normalAttrib5 = glGetAttribLocation(shaderProgram5, "normal");
    glEnableVertexAttribArray(normalAttrib5);
    glVertexAttribPointer(normalAttrib5, 3, GL_FLOAT, GL_FALSE,
                        3*sizeof(float), 0);


///////////////////////////////////////////////////// coloured cube instanced

    GLuint shaderProgram6;
    SO_PhongShader shaderObj6;
    shaderProgram6 = shaderObj6.generate(1, SO_COLOR_ATTRIBUTE | SO_INSTANCED);
    glUseProgram(shaderProgram6); // use the shader program

    shaderObj6.setLightPosition(0, glm::vec3(3.0f, 4.0f, 2.0f));
    shaderObj6.setLightConstant(0, 1.0f);
    shaderObj6.setLightLinear(0, 0.0f);
    shaderObj6.setLightQuadratic(0, 0.0f);
    shaderObj6.setLightAmbient(0, glm::vec3(0.2f, 0.2f, 0.2f));
    shaderObj6.setLightDiffuse(0, glm::vec3(1.0f, 1.0f, 1.0f));
    shaderObj6.setLightSpecular(0, glm::vec3(0.5f, 0.5f, 0.5f));

    shaderObj6.setModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(0.08f, 0.08f, 0.08f)));
    shaderObj6.setPostModelMatrix(glm::mat4(1.0f));
    
    std::vector<glm::mat4> intPos;
    intPos.reserve(1000);
    for (float i = 0; i < 1; i+=0.1) {
        for (float j = 0; j < 1; j+=0.1) {
            for (float k = 0; k < 1; k+=0.1) {
                intPos.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(1.05f + i, -0.45f + j, -0.45f + k)));
            }
        }
    }
    std::vector<glm::mat4> intNormals;
    intNormals.resize(intPos.size());
    for (int i = 0; i < intPos.size(); i++) {
        intNormals[i] = glm::transpose(glm::inverse(intPos[i]));
    }


    cameraObj.linkShader(&shaderObj6);


    glUseProgram(shaderProgram6);

    // the vertex array object for the scene
    GLuint vao6;
    glGenVertexArrays(1, &vao6);
    glBindVertexArray(vao6);


    GLuint vbo6; // apply vertices to the vbo
    glGenBuffers(1, &vbo6);
    glBindBuffer(GL_ARRAY_BUFFER, vbo6);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCol), verticesCol, GL_STATIC_DRAW);

    //locate vertex coords within buffer
    GLint posAttrib6 = glGetAttribLocation(shaderProgram6, "position");
    glEnableVertexAttribArray(posAttrib6);
    glVertexAttribPointer(posAttrib6, 3, GL_FLOAT, GL_FALSE,
                        9*sizeof(float), 0);
    
    //locate vertex normal within buffer
    GLint normalAttrib6 = glGetAttribLocation(shaderProgram6, "normal");
    glEnableVertexAttribArray(normalAttrib6);
    glVertexAttribPointer(normalAttrib6, 3, GL_FLOAT, GL_FALSE,
                        9*sizeof(float), (void*)(3*sizeof(float)));

    //locate vertex colors within buffer
    GLint colAttrib6 = glGetAttribLocation(shaderProgram6, "color");
    glEnableVertexAttribArray(colAttrib6);
    glVertexAttribPointer(colAttrib6, 3, GL_FLOAT, GL_FALSE,
                        9*sizeof(float), (void*)(6*sizeof(float)));

    GLuint vbo6InstPos;
    glGenBuffers(1, &vbo6InstPos);
    glBindBuffer(GL_ARRAY_BUFFER, vbo6InstPos);
    glBufferData(GL_ARRAY_BUFFER, intPos.size()*sizeof(glm::mat4), &intPos[0], GL_STATIC_DRAW);

    GLint instPosAttrib = glGetAttribLocation(shaderProgram6, "instanceMatrix");
    glEnableVertexAttribArray(instPosAttrib);
    glVertexAttribPointer(instPosAttrib, 4, GL_FLOAT, GL_FALSE,
                        sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(instPosAttrib+1);
    glVertexAttribPointer(instPosAttrib+1, 4, GL_FLOAT, GL_FALSE,
                        sizeof(glm::mat4), (void*)(1*sizeof(glm::vec4)));
    glEnableVertexAttribArray(instPosAttrib+2);
    glVertexAttribPointer(instPosAttrib+2, 4, GL_FLOAT, GL_FALSE,
                        sizeof(glm::mat4), (void*)(2*sizeof(glm::vec4)));
    glEnableVertexAttribArray(instPosAttrib+3);
    glVertexAttribPointer(instPosAttrib+3, 4, GL_FLOAT, GL_FALSE,
                        sizeof(glm::mat4), (void*)(3*sizeof(glm::vec4)));
    glVertexAttribDivisor(instPosAttrib, 1);
    glVertexAttribDivisor(instPosAttrib+1, 1);
    glVertexAttribDivisor(instPosAttrib+2, 1);
    glVertexAttribDivisor(instPosAttrib+3, 1);

    GLuint vbo6InstNorm;
    glGenBuffers(1, &vbo6InstNorm);
    glBindBuffer(GL_ARRAY_BUFFER, vbo6InstNorm);
    glBufferData(GL_ARRAY_BUFFER, intNormals.size()*sizeof(glm::mat4), &intNormals[0], GL_STATIC_DRAW);

    GLint instNormAttrib = glGetAttribLocation(shaderProgram6, "normalInstMatrix");
    glEnableVertexAttribArray(instNormAttrib);
    glVertexAttribPointer(instNormAttrib, 4, GL_FLOAT, GL_FALSE,
                        sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(instNormAttrib + 1);
    glVertexAttribPointer(instNormAttrib+1, 4, GL_FLOAT, GL_FALSE,
                        sizeof(glm::mat4), (void*)(1*sizeof(glm::vec4)));
    glEnableVertexAttribArray(instNormAttrib + 2);
    glVertexAttribPointer(instNormAttrib+2, 4, GL_FLOAT, GL_FALSE,
                        sizeof(glm::mat4), (void*)(2*sizeof(glm::vec4)));
    glEnableVertexAttribArray(instNormAttrib + 3);
    glVertexAttribPointer(instNormAttrib+3, 4, GL_FLOAT, GL_FALSE,
                        sizeof(glm::mat4), (void*)(3*sizeof(glm::vec4)));
    glVertexAttribDivisor(instNormAttrib, 1);
    glVertexAttribDivisor(instNormAttrib+1, 1);
    glVertexAttribDivisor(instNormAttrib+2, 1);
    glVertexAttribDivisor(instNormAttrib+3, 1);

    GLuint ebo6; // apply triangle elements to vbo
    glGenBuffers(1, &ebo6);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo6);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);



    double mouseSpeedX = 1; // camera controls
    double mouseSpeedY = 1;
    double anglePhi = 120.0;
    double angleTheta = 90.0;
    bool mouseDown = false;
    float dist = 7.0f;
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
        glUseProgram(shaderProgram1);
        glBindVertexArray(vao1);
        glDrawElements(GL_TRIANGLES, sizeof(elements), GL_UNSIGNED_INT, 0);
        glUseProgram(shaderProgram2);
        glBindVertexArray(vao2);
        glDrawElements(GL_TRIANGLES, sizeof(elements), GL_UNSIGNED_INT, 0);
        glUseProgram(shaderProgram4);
        glBindVertexArray(vao4);
        glDrawElements(GL_TRIANGLES, sizeof(elements), GL_UNSIGNED_INT, 0);
        glUseProgram(shaderProgram5);
        glBindVertexArray(vao5);
        glDrawElements(GL_TRIANGLES, sphereData.faceElements.size(), GL_UNSIGNED_INT, 0);
        glUseProgram(shaderProgram6);
        glBindVertexArray(vao6);
        glDrawElementsInstanced(GL_TRIANGLES, sizeof(elements)/sizeof(unsigned int), GL_UNSIGNED_INT, 0, intPos.size());
        glUseProgram(shaderProgram3); //transparents last
        glBindVertexArray(vao3);
        glDepthMask(GL_FALSE);
        glCullFace(GL_FRONT); // render back of cube first - do not need polygon z sorting as cube is convex
        glDrawElements(GL_TRIANGLES, sizeof(elements), GL_UNSIGNED_INT, 0);
        glCullFace(GL_BACK);
        glDrawElements(GL_TRIANGLES, sizeof(elements), GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glfwSwapBuffers(window);
        glfwPollEvents();
        //printf("%.5f\n", glfwGetTime() - time);
    }

    //if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    //    glfwSetWindowShouldClose(window, GL_TRUE);

    //clear up

    glDeleteBuffers(1, &ebo1);
    glDeleteBuffers(1, &vbo1);
    glDeleteVertexArrays(1, &vao1);

    glDeleteBuffers(1, &ebo2);
    glDeleteBuffers(1, &vbo2);
    glDeleteVertexArrays(1, &vao2);

    glDeleteBuffers(1, &ebo3);
    glDeleteBuffers(1, &vbo3);
    glDeleteVertexArrays(1, &vao3);

    glDeleteBuffers(1, &ebo4);
    glDeleteBuffers(1, &vbo4);
    glDeleteVertexArrays(1, &vao4);

    glDeleteBuffers(1, &ebo5);
    glDeleteBuffers(1, &vbo5);
    glDeleteVertexArrays(1, &vao5);

    glDeleteBuffers(1, &ebo6);
    glDeleteBuffers(1, &vbo6);
    glDeleteVertexArrays(1, &vao6);

    glfwTerminate();

    return 0;
}