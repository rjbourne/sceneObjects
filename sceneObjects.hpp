#ifndef SCENEOBJECTS_H_   /* Include guard */
#define SCENEOBJECTS_H_

#define SO_VERSION_MAJOR 1
#define SO_VERSION_MINOR 1
#define SO_VERSION_REVISION 2

#include "sceneObjects.hpp"
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//generic shader program class
class SO_Shader {
    GLuint programID;
    GLuint vertexShaderID;
    GLuint fragmentShaderID;
    GLint modelMatrixLoc; //location of shader uniforms
    GLint viewMatrixLoc;
    GLint projectionMatrixLoc;
    bool programCreated = false;
    public:
        ~SO_Shader(void);
        void createVertexShader(const char* vertexSource);
        void createFragmentShader(const char* fragmentSource);
        void linkProgram(void);
        GLuint getProgramID(void);
        virtual void setModelMatrix(glm::mat4 modelMatrix);
        virtual void setViewMatrix(glm::mat4 viewMatrix);
        virtual void setViewPosition(glm::vec3 viewPosition);
        virtual void setProjectionMatrix(glm::mat4 projectionMatrix);
};

enum SO_ShaderOptions : unsigned int {
    SO_ALPHA = 1,
    SO_INSTANCED = 2,
    SO_MATERIAL = 4,
    SO_AMBIENT_ATTRIBUTE = 8,
    SO_DIFFUSE_ATTRIBUTE = 16,
    SO_SPECULAR_ATTRIBUTE = 32,
    SO_ALPHA_ATTRIBUTE = 64,
    SO_COLOR_ATTRIBUTE = 128
};


// 3D Phong model lighting class
class SO_PhongShader : public SO_Shader {
    protected:
        int numberLights; //number of point light sources
        unsigned int options;
        GLint normalMatrixLoc;
        GLint viewPositionLoc;
        GLint postModelMatrixLoc;
        GLint postNormalMatrixLoc;
        GLint ambientMatLoc;
        GLint diffuseMatLoc;
        GLint specularMatLoc;
        GLint alphaMatLoc;
        GLint colorLoc;
        using SO_Shader::createVertexShader;
        using SO_Shader::createFragmentShader;
        using SO_Shader::linkProgram;
    public:
        GLuint generate(int numberLightsIn=1, unsigned int optionsIn=0);
        void setModelMatrix(glm::mat4 modelMatrix) override;
        void setPostModelMatrix(glm::mat4 modelMatrix);
        void setViewPosition(glm::vec3 viewPosition) override;
        void setLightPosition(int index, glm::vec3 lightPosition);
        void setLightConstant(int index, float lightConstant);
        void setLightLinear(int index, float lightLinear);
        void setLightQuadratic(int index, float lightQuadratic);
        void setLightAmbient(int index, glm::vec3 lightAmbient);
        void setLightDiffuse(int index, glm::vec3 lightDiffuse);
        void setLightSpecular(int index, glm::vec3 lightSpecular);
        void setMaterialAmbient(glm::vec3 ambientMaterial);
        void setMaterialDiffuse(glm::vec3 diffuseMaterial);
        void setMaterialSpecular(glm::vec3 specularMaterial);
        void setMaterialAlpha(float alphaMaterial);
        void setColor(glm::vec3 color);
        void setColor(glm::vec4 color);
};

class SO_SkyboxShader : public SO_Shader {
    protected:
        std::vector<std::string> imageFiles;
        GLuint textureID;
        GLuint skyboxVAO;
        GLuint skyboxVBO;
    public:
        ~SO_SkyboxShader(void);
        void setModelMatrix(glm::mat4 modelMatrix) override;
        void setViewMatrix(glm::mat4 viewMatrix) override;
        GLuint generate(std::vector<std::string> imageFilesIn);
        void render(GLenum depthFuncReset = GL_LESS);
};

//A 3D camera capable of creating view/projection matricces to linked shader programs on call
class SO_Camera {
    std::vector<SO_Shader*> shaderPointers;
    public:
        float fov;
        float aspectRatio;
        float nearClip;
        float farClip;
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        SO_Camera(float fovIn, float aspectRatioIn, float nearClipIn, float farClipIn, glm::vec3 positionIn, glm::vec3 frontIn, glm::vec3 upIn);
        void linkShader(SO_Shader *shaderRefIn);
        void unlinkShader(SO_Shader *shaderRefIn);
        void updateViewMatrix(void);
        void updateProjectionMatrix(void);
};

//struct containing return data for a generic mesh
struct SO_MeshData {
    std::vector<glm::vec3> vertices;
    std::vector<int> faceElements;
};

glm::vec3 createRatioVector(int subdivisions, int division, glm::vec3 vector1, glm::vec3 vector2);
SO_MeshData createIcosphere(int subdivisions);

double fade(double x);
int inc(int num, int repeat);
double grad(int hash, double x, double y, double z);
double lerp(double a, double b, double x);
glm::vec3 lerp(glm::vec3 a, glm::vec3 b, double x);
double modulus(double x, double y);
double perlin(double x, double y, double z, double repeat);

//contains all the information required to create a colormap gradient
struct SO_ColorMap {
    int length;
    glm::vec3* colors;
    float* weights;
};

glm::vec3 getLerpColor(SO_ColorMap &map, float min, float max, float value);

#endif // FOO_H_