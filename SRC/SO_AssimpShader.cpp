/** \file SO_AssimpShader.cpp */
#include "sceneObjects.hpp"
#include "sceneModels.hpp"

// generate a shader program for a assimp mesh
GLuint sceneObjects::SO_AssimpShader::generate(int numberLightsIn, int diffuseTextures, int specularTextures, int normalTextures) {
    numberLights = numberLightsIn;
    std::string vertexSourceStr;
    vertexSourceStr = R"glsl(
        #version 330 core

        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 normal;
        layout (location = 2) in vec2 texCoord;)glsl";
    if (normalTextures > 0) {
        vertexSourceStr += R"glsl(
        layout (location = 3) in vec3 tangent;
        )glsl";
    }
    vertexSourceStr += R"glsl(
        out vec3 worldPos;
        out vec2 TexCoord;)glsl";
    if (normalTextures > 0) {
        vertexSourceStr += R"glsl(
        out mat3 TBN;
        )glsl";
    } else {
        vertexSourceStr += R"glsl(
        out vec3 norm;
        )glsl";
    }
    vertexSourceStr += R"glsl(

        uniform mat4 normalMatrix;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 proj;

        void main() {
            gl_Position = proj * view *  model * vec4(position, 1.0);)glsl";
    if (normalTextures > 0) {
        vertexSourceStr += R"glsl(
            vec3 T = normalize(vec3(normalMatrix * vec4(tangent, 0.0)));
            vec3 N = normalize(vec3(normalMatrix * vec4(normal, 0.0)));
            // re-orthogonalize T with respect to N - Gram-Schmidt process
            T = normalize(T - dot(T, N) * N);
            // then retrieve perpendicular vector B with the cross product of T and N
            vec3 B = cross(N, T);
            TBN = mat3(T, B, N);
        )glsl";
    } else {
        vertexSourceStr += R"glsl(
            norm = normalize(vec3(normalMatrix * vec4(normal, 0.0)));
        )glsl";
    }
    vertexSourceStr += R"glsl(
            worldPos = vec3(model * vec4(position, 1.0));
            TexCoord = texCoord;
        }
    )glsl";

    std::string fragmentSourceStr;
    fragmentSourceStr = R"glsl(
        #version 330 core

        struct PointLight {
            vec3 lightPos;

            float constant;
            float linear;
            float quadratic;

            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
        };

        in vec3 worldPos;
        in vec2 TexCoord;)glsl";
    if (normalTextures > 0) {
        fragmentSourceStr += R"glsl(
        in mat3 TBN;
        )glsl";
    } else {
        fragmentSourceStr += R"glsl(
        in vec3 norm;
        )glsl";
    }
    fragmentSourceStr += R"glsl(

        out vec4 outColor;

        uniform vec3 viewPos;
        uniform unsigned int specPower;
        uniform PointLight lights[)glsl" + std::to_string(numberLights) + R"glsl(];

        uniform )glsl" + (std::string)((diffuseTextures == 0) ? "vec3 colorDiffuse" : "sampler2D textureDiffuse") + R"glsl(;
        uniform )glsl" + (std::string)((specularTextures == 0) ? "vec3 colorSpecular" : "sampler2D textureSpecular") + R"glsl(;
        )glsl" + (std::string)((normalTextures == 0) ? "" : "uniform sampler2D textureNormal;") + R"glsl(

        vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
            vec3 lightDir = normalize(light.lightPos - worldPos);
            // diffuse shading
            float diff = max(dot(normal, lightDir), 0.0);
            // specular shading
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), specPower);
            // attenuation
            float distance    = length(light.lightPos - worldPos);
            float attenuation = 1.0 / (light.constant + light.linear * distance + 
                        light.quadratic * (distance * distance));   
            // combine results
            vec3 ambient  = light.ambient  * )glsl" + (std::string)((diffuseTextures == 0) ? "colorDiffuse" : "texture(textureDiffuse, TexCoord).xyz") + R"glsl(;
            vec3 diffuse  = light.diffuse  * diff * )glsl" + (std::string)((diffuseTextures == 0) ? "colorDiffuse" : "texture(textureDiffuse, TexCoord).xyz") + R"glsl(;
            vec3 specular = light.specular * spec * )glsl" + (std::string)((specularTextures == 0) ? "colorSpecular" : "texture(textureSpecular, TexCoord).xyz") + R"glsl(;
            ambient  *= attenuation;
            diffuse  *= attenuation;
            specular *= attenuation;
            return (ambient + diffuse + specular);
        }

        void main()
        {)glsl";
    if (normalTextures > 0) {
        fragmentSourceStr += R"glsl(
            vec3 norm = texture(textureNormal, TexCoord).rgb;
            norm = norm * 2.0 - 1.0;   
            norm = normalize(TBN * norm); 
        )glsl";
    }
    fragmentSourceStr += R"glsl(
            vec3 viewDir = normalize(viewPos - worldPos); 
            vec3 result = vec3(0.0, 0.0, 0.0);
            for (int i = 0; i < )glsl" + std::to_string(numberLights) + R"glsl(; i++) {
                result += CalcPointLight(lights[i], norm, viewDir);
            }
            outColor = vec4(result, 1.0);
        })glsl";

    createVertexShader(vertexSourceStr.c_str());
    createFragmentShader(fragmentSourceStr.c_str());
    linkProgram();

    normalMatrixLoc = glGetUniformLocation(this->getProgramID(), "normalMatrix");
    viewPositionLoc = glGetUniformLocation(this->getProgramID(), "viewPos");
    specularPowerLoc = glGetUniformLocation(this->getProgramID(), "specPower");
    setSpecularPower(32);

    return this->getProgramID();
}

//applies the model matrix to the shader program
//model matrix transforms from modelspace to worldspace
//also creates and applies the corresponding normal matrix
void sceneObjects::SO_AssimpShader::setModelMatrix(glm::mat4 modelMatrix) {
    SO_Shader::setModelMatrix(modelMatrix);
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    glProgramUniformMatrix4fv(this->getProgramID(), normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

//set the position of the camera in the shader program in worldspace
//method has no effect in SO::Shader base class
void sceneObjects::SO_AssimpShader::setViewPosition(glm::vec3 viewPosition) {
    glProgramUniform3fv(this->getProgramID(), viewPositionLoc, 1, glm::value_ptr(viewPosition));
}

//set the position of a light in worldspace
//index is the number of the light (rom 0 to numberLights-1)
//lightPosition is the position
void sceneObjects::SO_AssimpShader::setLightPosition(int index, glm::vec3 lightPosition) {
    if (index < 0 || index >= numberLights) {
        std::string error = "index to lights array is out of range\nrecieved: " + std::to_string(index) + "\nlength: " + std::to_string(numberLights);
        throw std::invalid_argument(error.c_str());
    }
    std::string name = "lights[" + std::to_string(index) + "].lightPos";
    GLint lightPositionLoc = glGetUniformLocation(this->getProgramID(), name.c_str());
    glProgramUniform3fv(this->getProgramID(), lightPositionLoc, 1, glm::value_ptr(lightPosition));
}

//set the constant attenuation factor of a light in worldspace
//index is the number of the light (rom 0 to numberLights-1)
//lightConstant is the constant
void sceneObjects::SO_AssimpShader::setLightConstant(int index, float lightConstant) {
    if (index < 0 || index >= numberLights) {
        std::string error = "index to lights array is out of range\nrecieved: " + std::to_string(index) + "\nlength: " + std::to_string(numberLights);
        throw std::invalid_argument(error.c_str());
    }
    std::string name = "lights[" + std::to_string(index) + "].constant";
    GLint lightConstantLoc = glGetUniformLocation(this->getProgramID(), name.c_str());
    glProgramUniform1f(this->getProgramID(), lightConstantLoc, lightConstant);
}

//set the linear attenuation factor of a light in worldspace
//index is the number of the light (rom 0 to numberLights-1)
//lightLinear is the coefficient
void sceneObjects::SO_AssimpShader::setLightLinear(int index, float lightLinear) {
    if (index < 0 || index >= numberLights) {
        std::string error = "index to lights array is out of range\nrecieved: " + std::to_string(index) + "\nlength: " + std::to_string(numberLights);
        throw std::invalid_argument(error.c_str());
    }
    std::string name = "lights[" + std::to_string(index) + "].linear";
    GLint lightLinearLoc = glGetUniformLocation(this->getProgramID(), name.c_str());
    glProgramUniform1f(this->getProgramID(), lightLinearLoc, lightLinear);
}

//set the quadratic attenuation factor of a light in worldspace
//index is the number of the light (rom 0 to numberLights-1)
//lightQuadratic is the coefficient
void sceneObjects::SO_AssimpShader::setLightQuadratic(int index, float lightQuadratic) {
    if (index < 0 || index >= numberLights) {
        std::string error = "index to lights array is out of range\nrecieved: " + std::to_string(index) + "\nlength: " + std::to_string(numberLights);
        throw std::invalid_argument(error.c_str());
    }
    std::string name = "lights[" + std::to_string(index) + "].quadratic";
    GLint lightQuadraticLoc = glGetUniformLocation(this->getProgramID(), name.c_str());
    glProgramUniform1f(this->getProgramID(), lightQuadraticLoc, lightQuadratic);
}

//set the ambient color and strength of a light
//index is the number of the light (rom 0 to numberLights-1)
//lightAmbient is the color (RGB)
void sceneObjects::SO_AssimpShader::setLightAmbient(int index, glm::vec3 lightAmbient) {
    if (index < 0 || index >= numberLights) {
        std::string error = "index to lights array is out of range\nrecieved: " + std::to_string(index) + "\nlength: " + std::to_string(numberLights);
        throw std::invalid_argument(error.c_str());
    }
    std::string name = "lights[" + std::to_string(index) + "].ambient";
    GLint lightAmbientLoc = glGetUniformLocation(this->getProgramID(), name.c_str());
    glProgramUniform3fv(this->getProgramID(), lightAmbientLoc, 1, glm::value_ptr(lightAmbient));
}

//set the diffuse color and strength of a light
//index is the number of the light (rom 0 to numberLights-1)
//lightDiffuse is the color (RGB)
void sceneObjects::SO_AssimpShader::setLightDiffuse(int index, glm::vec3 lightDiffuse) {
    if (index < 0 || index >= numberLights) {
        std::string error = "index to lights array is out of range\nrecieved: " + std::to_string(index) + "\nlength: " + std::to_string(numberLights);
        throw std::invalid_argument(error.c_str());
    }
    std::string name = "lights[" + std::to_string(index) + "].diffuse";
    GLint lightDiffuseLoc = glGetUniformLocation(this->getProgramID(), name.c_str());
    glProgramUniform3fv(this->getProgramID(), lightDiffuseLoc, 1, glm::value_ptr(lightDiffuse));
}

//set the specular color and strength of a light
//index is the number of the light (rom 0 to numberLights-1)
//lightSpecular is the color (RGB)
void sceneObjects::SO_AssimpShader::setLightSpecular(int index, glm::vec3 lightSpecular) {
    if (index < 0 || index >= numberLights) {
        std::string error = "index to lights array is out of range\nrecieved: " + std::to_string(index) + "\nlength: " + std::to_string(numberLights);
        throw std::invalid_argument(error.c_str());
    }
    std::string name = "lights[" + std::to_string(index) + "].specular";
    GLint lightSpecularLoc = glGetUniformLocation(this->getProgramID(), name.c_str());
    glProgramUniform3fv(this->getProgramID(), lightSpecularLoc, 1, glm::value_ptr(lightSpecular));
}

void sceneObjects::SO_AssimpShader::setSpecularPower(unsigned int specPower) {
    glProgramUniform1ui(this->getProgramID(), specularPowerLoc, specPower);
}