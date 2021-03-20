/** \file sceneModels.hpp
 * \brief The header file containing all objects associated specifically with the assimp library
*/

#ifndef SCENEMODELS_H_   /* Include guard */
#define SCENEMODELS_H_

#include "sceneObjects.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace sceneObjects {

/// The container for a single vertex imported with the assimp system
struct SO_AssimpVertex {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); ///< The position of the vertex in modelSpace
    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f); ///< The normal to the surface in model space
    glm::vec2 texCoords = glm::vec2(0.0f, 0.0f); ///< The coords of the texture at this vertex 2D (u,v)
    glm::vec3 tangent = glm::vec3(0.0f, 0.0f, 0.0f); ///< The tangent vector lies along the u axis of the texture - the bitangent is not stored
};

/// Stores a texture for an SO_AssimpModel
struct SO_AssimpTexture {
    GLuint textureId; ///< The OpenGL texture ID for the texture
    std::string path; ///< The filepath and filename for the texture
};

/// The shader for an SO_AssimpMesh object
/**
 * This shader allows the use of diffuse, specular and normal maps if textures are provided
 * It is intended to be used with the SO_AssimpMesh class - although this is not a requirement
**/
class SO_AssimpShader : public SO_Shader {
    protected:
        int numberLights; ///< Number of point light sources in the scene
        GLint normalMatrixLoc; ///< The OpenGL location of the mat4 uniform named "normalMatrix" in the shader
        GLint viewPositionLoc; ///< The OpenGL location of the vec3 uniform named "viewPos" in the shader
        GLint specularPowerLoc; ///< The OpenGL location of the unsigned int uniform named "specPower" in the shader
        using SO_Shader::createVertexShader;
        using SO_Shader::createFragmentShader;
        using SO_Shader::linkProgram;
    public:
        /// Generate the shader program using the number of lights/textures of each type as provided
        /**
         * Note that the current shader implementation will only use the first texture of each type provided.
         * The shader implements a Phong lighting model.
        **/
        GLuint generate(int numberLightsIn, int diffuseTextures, int specularTextures, int normalTextures);
        /// An override of the base class method which also calculates the transformation for normal vectors and passes this into the shader.
        void setModelMatrix(glm::mat4 modelMatrix) override;
        /// Set the position of the camera in world space
        void setViewPosition(glm::vec3 viewPosition) override;
        /// Set the position of light number 'index' in world space
        void setLightPosition(int index, glm::vec3 lightPosition);
        /// Set the constant term of the lights attenuation factor: 1 for a constant brightness source with distance.
        void setLightConstant(int index, float lightConstant);
        /// Set the linear term of the lights attenuation factor: 0 for a constant brightness source with distance.
        void setLightLinear(int index, float lightLinear);
        /// Set the quadratic term of the lights attenuation factor: 0 for a constant brightness source with distance.
        void setLightQuadratic(int index, float lightQuadratic);
        /// Set the colour and strength of the lights ambient factor - independent of material properties.
        void setLightAmbient(int index, glm::vec3 lightAmbient);
        /// Set the colour and strength of the lights diffuse factor - independent of material properties.
        void setLightDiffuse(int index, glm::vec3 lightDiffuse);
        /// Set the colour and strength of the lights specular factor - independent of material properties.
        void setLightSpecular(int index, glm::vec3 lightSpecular);
        /// Set the power of the specular highlights - higher integers produce 'sharper' spots
        void setSpecularPower(unsigned int specPower);
};

/// A class which stores an advanced mesh object - including textures
/**
 * This class stores a mesh object with vertices, face elements and diffuse, specular and normal textures
**/
class SO_AssimpMesh {
        GLuint vbo; ///< The vertex buffer object for the mesh
        GLuint vao; ///< The vertex array object for the mesh
        GLuint ebo; ///< The lement buffer object for the mesh
    public:
        SO_AssimpShader shader; ///< The shader program used to render this object - customised based on lighting choice and textures
        std::vector<SO_AssimpVertex> vertices; ///< The vertices of the mesh - includes texture coordinates and normals/tangents
        std::vector<unsigned int> elements; ///< The elements used to construct triangular faces from the mesh
        std::vector<SO_AssimpTexture> diffuseMaps; ///< The diffuse textures to be used for the mesh - currently only the first is used
        glm::vec3 diffuseColor = glm::vec3(0.0f, 0.0f, 0.0f); ///< The diffuse colour to be used if no diffuse texture is present
        std::vector<SO_AssimpTexture> specularMaps; ///< The specular texture to be used for the mesh - currently only the first is used
        glm::vec3 specularColor = glm::vec3(0.0f, 0.0f, 0.0f); ///< The specular colour to be used if no specular texture is present
        std::vector<SO_AssimpTexture> normalMaps; ///< The normal textures to be used for the object - currently only the first is used
        SO_AssimpShader* createShader(int numberLights); ///< The function which creates the custom SO_AssimpShader for this mesh
        ///renders the mesh
        /**
         * \warning this operation leaves the shader program and VAO set on the mesh shader after calling useProgram() and bindVertexArray() must be recalled
        **/
        void render();
        ///The custom destructor for the SO_AssimpMesh class
        /**
         * The custom destructor for this class is implemented with non-default behaviour to destory the VAO/VBO that have been created. 
        **/
        ~SO_AssimpMesh();
};

/// A class which can load in object files using the assimp library
/**
 * This class can load in object files from many formats using the assimp library. 
 * It currently supports loading diffuse, normal and specular textures as well as the mesh.
 * It supports multiple-mesh files, storing each as an individual SO_AssimpMesh
**/
class SO_AssimpModel {
    public:
        std::vector<SO_AssimpMesh> meshes; ///< The meshes in the file - for simple objects usually only 1 mesh is created
        /// A vector of the textures in all involved meshes - prevents loading multiple instances of the same texture for different meshes
        std::vector<SO_AssimpTexture> globalTextures;
        std::string directory; ///< The filepath and filname of the model file
        ///constructor for assimp model (currently only allows 1 pair of texture coords per mesh)
        /**
         * `aiOptions` should be members of the `aiPostProccessSteps` enum e.g.
         * -aiProcess_FlipUVs
         * -aiProcess_GenNormals
         * -aiProcess_OptimizeMeshes
         * -aiProcess_OptimizeGraph
         * Note that aiTriangulate is always called - the SO_AssimpMesh only deals with triangular faces.
         * This constructor duplicates the behavious of loadModel(), which is in fact called by the constructor
        **/
        SO_AssimpModel(std::string path, int aiOptions);
        ///loader for assimp model if constructor not used (currently only allows 1 pair of texture coords per mesh)
        /**
         * `aiOptions` should be members of the `aiPostProccessSteps` enum e.g.
         * -aiProcess_FlipUVs
         * -aiProcess_GenNormals
         * -aiProcess_OptimizeMeshes
         * -aiProcess_OptimizeGraph
         * Note that aiTriangulate is always called - the SO_AssimpMesh only deals with triangular faces.
        **/
        void loadModel(std::string path, int aiOptions);
        void processNode(aiNode* node, const aiScene* scene); ///< Process each node of an assimp model, may contain multiple meshes and have its own relative coordinates
        SO_AssimpMesh processMesh(aiMesh* mesh, const aiScene* scene); ///< Convert each mesh of an assimp node into an SO_AssimpMesh
        std::vector<SO_AssimpTexture> loadMaterialTextures(aiMaterial* material, aiTextureType type); ///< Load all the required textures of an assimp mesh
        GLuint loadTextureFromFile(std::string path); ///< Loads a texture from an image file
        void render(); ///< renders the model by calling the render() method of each child SO_AssimpMesh
        void createShaders(int numberLights); ///< Calls each SO_AssimpMesh to create its own shaders using the number of pointlight sources given
};


}

#endif