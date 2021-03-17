#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "sceneObjects.hpp"


//constructor for assimp model (only allows 1 pair of texture coords)
//aiOptions should be members of aiPostProccessSteps enum e.g.
// -aiProcess_FlipUVs
// -aiProcess_GenNormals
// -aiProcess_OptimizeMeshes
// -aiProcess_OptimizeGraph
// Note that aiTriangulate is always called
sceneObjects::SO_AssimpModel::SO_AssimpModel(std::string path, int aiOptions) {
    loadModel(path, aiOptions);
}

//constructor for assimp model (only allows 1 pair of texture coords)
//aiOptions should be members of aiPostProccessSteps enum e.g.
// -aiProcess_FlipUVs
// -aiProcess_GenNormals
// -aiProcess_OptimizeMeshes
// -aiProcess_OptimizeGraph
// Note that aiTriangulate is always called
void sceneObjects::SO_AssimpModel::loadModel(std::string path, int aiOptions) {
    Assimp::Importer importer;
    aiOptions |= aiProcess_Triangulate;
    const aiScene* scene = importer.ReadFile(path, 0);
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        if (scene->mMaterials[i]->GetTextureCount(aiTextureType_HEIGHT) > 0) {
            aiOptions |= aiProcess_CalcTangentSpace;
            break;
        }
    }
    importer.ApplyPostProcessing(aiOptions);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::invalid_argument(std::string("Assimp error imorting: ") + std::string(importer.GetErrorString()));
        return;
    }
    directory = path.substr(0, path.find_last_of("/\\"));
    processNode(scene->mRootNode, scene);
    
}

//processes each node of a scene
void sceneObjects::SO_AssimpModel::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

//convertes an assimp mesh into an SO mesh
sceneObjects::SO_AssimpMesh sceneObjects::SO_AssimpModel::processMesh(aiMesh* mesh, const aiScene* scene) {
    sceneObjects::SO_AssimpMesh SOMesh;
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    //diffuse texture
    SOMesh.diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
    aiColor3D diffuseColor (0.0f, 1.0f, 0.0f);
    if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor)) {
        throw std::invalid_argument("Failed to load diffuse color from assimp material");
    }
    SOMesh.diffuseColor.r = diffuseColor.r;
    SOMesh.diffuseColor.g = diffuseColor.g;
    SOMesh.diffuseColor.b = diffuseColor.b;
    //specular texture
    SOMesh.specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
    aiColor3D specularColor (0.0f, 1.0f, 0.0f);
    if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor)) {
        throw std::invalid_argument("Failed to load specular color from assimp material");
    }
    SOMesh.specularColor.r = specularColor.r;
    SOMesh.specularColor.g = specularColor.g;
    SOMesh.specularColor.b = specularColor.b;
    //normal texture
    SOMesh.normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT);
    bool includeTangent = SOMesh.normalMaps.size() > 0;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        SO_AssimpVertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;
        vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
        vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        if (includeTangent) {
            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;
        }
        SOMesh.vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            SOMesh.elements.push_back(face.mIndices[j]);
        }
    }
    return SOMesh;
}

//loads the texture files from a material
std::vector<sceneObjects::SO_AssimpTexture> sceneObjects::SO_AssimpModel::loadMaterialTextures(aiMaterial* material, aiTextureType type) {
    std::vector<SO_AssimpTexture> textures;
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
        aiString str;
        material->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < globalTextures.size(); j++) {
            if (std::strcmp(globalTextures[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(globalTextures[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            SO_AssimpTexture texture;
            texture.textureId = loadTextureFromFile(std::string(str.C_Str()));
            texture.path = str.C_Str();
            textures.push_back(texture);
            globalTextures.push_back(texture);

        }
    }
    return textures;
}

//loads texture files into openGL
GLuint sceneObjects::SO_AssimpModel::loadTextureFromFile(std::string path) {
    std::string filename = directory + '\\' + path;

    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(false);
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    else {
        std::string error = "Unable to load skybox texture at path: " + filename;
        throw std::runtime_error(error.c_str());
    }
    stbi_image_free(data);

    return textureID;
}

//draws the scene - call at render time
void sceneObjects::SO_AssimpModel::draw() {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].draw();
    }
}

//creates all the shaders for the meshes
void sceneObjects::SO_AssimpModel::createShaders(int numberLights) {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].createShader(numberLights);
    }
}