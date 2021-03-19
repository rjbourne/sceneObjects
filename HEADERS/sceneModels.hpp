#ifndef SCENEMODELS_H_   /* Include guard */
#define SCENEMODELS_H_

#include "sceneObjects.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace sceneObjects {


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


}

#endif