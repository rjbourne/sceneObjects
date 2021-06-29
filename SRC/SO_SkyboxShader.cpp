/** \file SO_SkyboxShader.cpp */
#define STB_IMAGE_IMPLEMENTATION

#include "sceneObjects.hpp"
#include <stb_image.h>

float sceneObjects::skyboxVertices[108] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};

//generate a skybox using a vector of 6 images in order right (+x), left, top (+y), bottom, front (-z), back
//max 2048 size (some frames delay in render function if larger) why? max texture size in buffer?
GLuint sceneObjects::SO_SkyboxShader::generate(std::vector<std::string> imageFilesIn) {
    if (imageFilesIn.size() != 6) {
        std::string error = "expected 6 filenames to skybox generate(), got " + std::to_string(imageFilesIn.size());
        throw std::invalid_argument(error.c_str());
    }
    imageFiles = imageFilesIn;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, numChannels;
    for (int  i = 0; i < 6; i++) {
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(imageFiles[i].c_str(), &width, &height, &numChannels, STBI_rgb_alpha);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        } else {
            std::string error = "Unable to load skybox texture at path: " + imageFiles[i];
            throw std::runtime_error(error.c_str());
        }
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    const char* vertexSource = R"glsl(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        out vec3 TexCoords;

        uniform mat4 proj;
        uniform mat4 view;
        uniform mat4 model;

        void main()
        {
            TexCoords = aPos;
            vec4 pos = proj * view * model * vec4(aPos, 1.0);
            gl_Position = pos.xyww;
        })glsl";
    createVertexShader(vertexSource);

    const char* fragmentSource = R"glsl(
        #version 330 core
        out vec4 FragColor;

        in vec3 TexCoords;

        uniform samplerCube skybox;

        void main()
        {
            FragColor = texture(skybox, TexCoords);
        })glsl";
    createFragmentShader(fragmentSource);
    linkProgram();
    glProgramUniform1i(this->getProgramID(), glGetUniformLocation(this->getProgramID(), "skybox"), 0);

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    return this->getProgramID();
}

//set the model matrix of the skybox (empty function)
void sceneObjects::SO_SkyboxShader::setModelMatrix(glm::mat4 modelMatrix) {
    modelMatrix = glm::mat4(glm::mat3(modelMatrix));
    SO_Shader::setModelMatrix(modelMatrix);
}

//set the view matrix of the skybox (removes translations)
void sceneObjects::SO_SkyboxShader::setViewMatrix(glm::mat4 viewMatrix) {
    viewMatrix = glm::mat4(glm::mat3(viewMatrix));
    SO_Shader::setViewMatrix(viewMatrix);
}

//render the skybox - should be last render done for efficiency
//this operation leaves the shader and VAO set on the skybox
void sceneObjects::SO_SkyboxShader::render(GLenum depthFuncReset) {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(this->getProgramID());
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(depthFuncReset);
}

//destructor
sceneObjects::SO_SkyboxShader::~SO_SkyboxShader(void) {
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
}