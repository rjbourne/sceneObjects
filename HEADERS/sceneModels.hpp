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


/// A class which can load in object files using the assimp library
/**
 * This class can load in object files from many formats using the assimp library. 
 * It currently supports loading diffuse, normal and specular textures as well as the mesh.
 * It supports multiple-mesh files, storing each as an individual SO_ModelMesh
**/
class SO_AssimpModel {
    public:
        std::vector<SO_ModelMesh> meshes; ///< The meshes in the file - for simple objects usually only 1 mesh is created
        /// A vector of the textures in all involved meshes - prevents loading multiple instances of the same texture for different meshes
        std::vector<SO_ModelTexture> globalTextures;
        std::string directory; ///< The filepath and filname of the model file
        ///constructor for assimp model (currently only allows 1 pair of texture coords per mesh)
        /**
         * `aiOptions` should be members of the `aiPostProccessSteps` enum e.g.
         * -aiProcess_FlipUVs
         * -aiProcess_GenNormals
         * -aiProcess_OptimizeMeshes
         * -aiProcess_OptimizeGraph
         * Note that aiTriangulate is always called - the SO_ModelMesh only deals with triangular faces.
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
         * Note that aiTriangulate is always called - the SO_ModelMesh only deals with triangular faces.
        **/
        void loadModel(std::string path, int aiOptions);
        void processNode(aiNode* node, const aiScene* scene); ///< Process each node of an assimp model, may contain multiple meshes and have its own relative coordinates
        SO_ModelMesh processMesh(aiMesh* mesh, const aiScene* scene); ///< Convert each mesh of an assimp node into an SO_ModelMesh
        std::vector<SO_ModelTexture> loadMaterialTextures(aiMaterial* material, aiTextureType type); ///< Load all the required textures of an assimp mesh
        void render(); ///< renders the model by calling the render() method of each child SO_ModelMesh
        void createShaders(int numberLights); ///< Calls each SO_ModelMesh to create its own shaders using the number of pointlight sources given
};


}

#endif