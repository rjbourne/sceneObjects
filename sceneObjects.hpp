#ifndef SCENEOBJECTS_H_   /* Include guard */
#define SCENEOBJECTS_H_

#define SO_VERSION_MAJOR 1
#define SO_VERSION_MINOR 2
#define SO_VERSION_REVISION 2

#include "sceneObjects.hpp"
#include <vector>
#include <string>
#include <stdio.h>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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
        GLint specularPowerLoc;
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
        void setSpecularPower(unsigned int specPower);
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

struct SO_AssimpVertex {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec2 texCoords = glm::vec2(0.0f, 0.0f);
    glm::vec3 tangent = glm::vec3(0.0f, 0.0f, 0.0f);
};

struct SO_AssimpTexture {
    GLuint textureId;
    std::string path;
};

class SO_AssimpShader : public SO_Shader {
    protected:
        int numberLights;
        GLint normalMatrixLoc;
        GLint viewPositionLoc;
        GLint specularPowerLoc;
    public:
        GLuint generate(int numberLightsIn, int diffuseTextures, int specularTextures, int normalTextures);
        void setModelMatrix(glm::mat4 modelMatrix) override;
        void setViewPosition(glm::vec3 viewPosition) override;
        void setLightPosition(int index, glm::vec3 lightPosition);
        void setLightConstant(int index, float lightConstant);
        void setLightLinear(int index, float lightLinear);
        void setLightQuadratic(int index, float lightQuadratic);
        void setLightAmbient(int index, glm::vec3 lightAmbient);
        void setLightDiffuse(int index, glm::vec3 lightDiffuse);
        void setLightSpecular(int index, glm::vec3 lightSpecular);
        void setSpecularPower(unsigned int specPower);
};

class SO_AssimpMesh {
        GLuint vbo;
        GLuint vao;
        GLuint ebo;
    public:
        SO_AssimpShader shader;
        std::vector<SO_AssimpVertex> vertices;
        std::vector<unsigned int> elements;
        std::vector<SO_AssimpTexture> diffuseMaps;
        glm::vec3 diffuseColor = glm::vec3(0.0f, 0.0f, 0.0f);
        std::vector<SO_AssimpTexture> specularMaps;
        glm::vec3 specularColor = glm::vec3(0.0f, 0.0f, 0.0f);
        std::vector<SO_AssimpTexture> normalMaps;
        SO_AssimpShader* createShader(int numberLights);
        void draw();
        ~SO_AssimpMesh();
};

class SO_AssimpModel {
    public:
        std::vector<SO_AssimpMesh> meshes;
        std::vector<SO_AssimpTexture> globalTextures;
        std::string directory;
        SO_AssimpModel(std::string path, int aiOptions);
        void loadModel(std::string path, int aiOptions);
        void processNode(aiNode* node, const aiScene* scene);
        SO_AssimpMesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<SO_AssimpTexture> loadMaterialTextures(aiMaterial* material, aiTextureType type);
        GLuint loadTextureFromFile(std::string path);
        void draw();
        void createShaders(int numberLights);
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

#ifdef _WIN32
//A class capable of rendering to a ffmpeg stream
class SO_FfmpegStream {
    std::string filepath;
    bool streaming = false;
    FILE*  ffmpeg;
    int width;
    int height;
    std::unique_ptr<int[]> buffer;
    public:
        SO_FfmpegStream(std::string filepathIn);
        void setFilepath(std::string filepathIn);
        void openStream(int widthIn, int heightIn);
        void renderFrame(void);
        void closeStream(void);

};
#else
#pragma message("WARNING: class SO_Ffmpeg is not supported on this OS, and is not available")
#endif


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

template <typename T> T lerp(T a, T b, float x) {
    return a + (T)(x * (b - a));
}

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