#include "sceneObjects.hpp"

//numberLights gives number of point lights in the scene
//optionsIn : binary flags given by enums SO_ShaderOptions
// expects in: position, normal, (ambient/diffuse/specular/alpha)Attrib, color, instanceMatrix, normalInstMatrix
//uniforms: set with SO_PhongShader::set_____ methods
GLuint sceneObjects::SO_PhongShader::generate(int numberLightsIn, unsigned int optionsIn) {
    numberLights = numberLightsIn; //no. of point sources
    options = optionsIn;
    // create shaders and link them - delete after
    std::string vertexSourceStr;
    vertexSourceStr = R"glsl(
        #version 330 core

        in vec3 position;
        in vec3 normal;)glsl";
    if ((options & SO_MATERIAL) == SO_MATERIAL) {
        if ((options & SO_AMBIENT_ATTRIBUTE) == SO_AMBIENT_ATTRIBUTE) {
            vertexSourceStr += "\nin vec3 ambientAttrib;";
        }
        if ((options & SO_DIFFUSE_ATTRIBUTE) == SO_DIFFUSE_ATTRIBUTE) {
            vertexSourceStr += "\nin vec3 diffuseAttrib;";
        }
        if ((options & SO_SPECULAR_ATTRIBUTE) == SO_SPECULAR_ATTRIBUTE) {
            vertexSourceStr += "\nin vec3 specularAttrib;";
        }
        if ((options & SO_ALPHA_ATTRIBUTE) == SO_ALPHA_ATTRIBUTE and (options & SO_ALPHA) == SO_ALPHA) {
            vertexSourceStr += "\nin float alphaAttrib;";
        }
    } else if ((options & SO_COLOR_ATTRIBUTE) == SO_COLOR_ATTRIBUTE) {
        vertexSourceStr += "\nin vec" + (std::string)(((options & SO_ALPHA) == SO_ALPHA) ? "4" : "3") + " color;";
    }
    if ((options & SO_INSTANCED) == SO_INSTANCED) {
        vertexSourceStr += R"glsl(
        in mat4 instanceMatrix;
        in mat4 normalInstMatrix;)glsl";
    }

    if ((options & SO_MATERIAL) == SO_MATERIAL) {
        if ((options & SO_AMBIENT_ATTRIBUTE) == SO_AMBIENT_ATTRIBUTE) {
            vertexSourceStr += "\nout vec3 AmbientMat;";
        }
        if ((options & SO_DIFFUSE_ATTRIBUTE) == SO_DIFFUSE_ATTRIBUTE) {
            vertexSourceStr += "\nout vec3 DiffuseMat;";
        }
        if ((options & SO_SPECULAR_ATTRIBUTE) == SO_SPECULAR_ATTRIBUTE) {
            vertexSourceStr += "\nout vec3 SpecularMat;";
        }
        if ((options & SO_ALPHA_ATTRIBUTE) == SO_ALPHA_ATTRIBUTE and (options & SO_ALPHA) == SO_ALPHA) {
            vertexSourceStr += "\nout float AlphaMat;";
        }
    } else if ((options & SO_COLOR_ATTRIBUTE) == SO_COLOR_ATTRIBUTE) {
        vertexSourceStr += "\nout vec" + (std::string)(((options & SO_ALPHA) == SO_ALPHA) ? "4" : "3") + " Color;";
    }
    vertexSourceStr += R"glsl(
        out vec3 norm;
        out vec3 worldPos;

        uniform mat4 normalMatrix;
        uniform mat4 model;)glsl";
    if ((options & SO_INSTANCED) == SO_INSTANCED) {
        vertexSourceStr += R"glsl(
        uniform mat4 postNormalMatrix;
        uniform mat4 postModel;)glsl";
    }
    vertexSourceStr += R"glsl(
        uniform mat4 view;
        uniform mat4 proj;

        void main()
        {)glsl";
    if ((options & SO_MATERIAL) == SO_MATERIAL) {
        if ((options & SO_AMBIENT_ATTRIBUTE) == SO_AMBIENT_ATTRIBUTE) {
            vertexSourceStr += "\n\tAmbientMat = ambientAttrib;";
        }
        if ((options & SO_DIFFUSE_ATTRIBUTE) == SO_DIFFUSE_ATTRIBUTE) {
            vertexSourceStr += "\n\tDiffuseMat = diffuseAttrib;";
        }
        if ((options & SO_SPECULAR_ATTRIBUTE) == SO_SPECULAR_ATTRIBUTE) {
            vertexSourceStr += "\n\tSpecularMat = specularAttrib;";
        }
        if ((options & SO_ALPHA_ATTRIBUTE) == SO_ALPHA_ATTRIBUTE and (options & SO_ALPHA) == SO_ALPHA) {
            vertexSourceStr += "\n\tAlphaMat = alphaAttrib;";
        }
    } else if ((options & SO_COLOR_ATTRIBUTE) == SO_COLOR_ATTRIBUTE) {
        vertexSourceStr += "\n\tColor = color;";
    }
    vertexSourceStr += R"glsl(
            gl_Position = proj * view * )glsl" + (std::string)(((options & SO_INSTANCED) == SO_INSTANCED) ? "postModel * instanceMatrix *" : "") + R"glsl( model * vec4(position, 1.0);
            norm = normalize(vec3()glsl" + (std::string)(((options & SO_INSTANCED) == SO_INSTANCED) ? "postNormalMatrix * normalInstMatrix *" : "") + R"glsl( normalMatrix * vec4(normal, 0.0)));
            worldPos = vec3()glsl" + (std::string)(((options & SO_INSTANCED) == SO_INSTANCED) ? "postModel * instanceMatrix *" : "") + R"glsl(model * vec4(position, 1.0));
        }
    )glsl";
    const char* vertexSource = vertexSourceStr.c_str();
    createVertexShader(vertexSource);
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
        };)glsl";

    if ((options & SO_MATERIAL) == SO_MATERIAL) {
        if ((options & SO_AMBIENT_ATTRIBUTE) == SO_AMBIENT_ATTRIBUTE) {
            fragmentSourceStr += "\nin vec3 AmbientMat;";
        }
        if ((options & SO_DIFFUSE_ATTRIBUTE) == SO_DIFFUSE_ATTRIBUTE) {
            fragmentSourceStr += "\nin vec3 DiffuseMat;";
        }
        if ((options & SO_SPECULAR_ATTRIBUTE) == SO_SPECULAR_ATTRIBUTE) {
            fragmentSourceStr += "\nin vec3 SpecularMat;";
        }
        if ((options & SO_ALPHA_ATTRIBUTE) == SO_ALPHA_ATTRIBUTE and (options & SO_ALPHA) == SO_ALPHA) {
            fragmentSourceStr += "\nin float AlphaMat;";
        }
    } else if ((options & SO_COLOR_ATTRIBUTE) == SO_COLOR_ATTRIBUTE) {
        fragmentSourceStr += "\nin vec" + (std::string)(((options & SO_ALPHA) == SO_ALPHA) ? "4" : "3") + " Color;";
    }

    fragmentSourceStr += R"glsl(
        in vec3 norm;
        in vec3 worldPos;

        out vec4 outColor;)glsl";
    if ((options & SO_MATERIAL) == SO_MATERIAL) {
        if ((options & SO_AMBIENT_ATTRIBUTE) != SO_AMBIENT_ATTRIBUTE) {
            fragmentSourceStr += "\nuniform vec3 AmbientMat;";
        }
        if ((options & SO_DIFFUSE_ATTRIBUTE) != SO_DIFFUSE_ATTRIBUTE) {
            fragmentSourceStr += "\nuniform vec3 DiffuseMat;";
        }
        if ((options & SO_SPECULAR_ATTRIBUTE) != SO_SPECULAR_ATTRIBUTE) {
            fragmentSourceStr += "\nuniform vec3 SpecularMat;";
        }
        if ((options & SO_ALPHA_ATTRIBUTE) != SO_ALPHA_ATTRIBUTE and (options & SO_ALPHA) == SO_ALPHA) {
            fragmentSourceStr += "\nuniform float AlphaMat;";
        }
    } else if ((options & SO_COLOR_ATTRIBUTE) != SO_COLOR_ATTRIBUTE) {
        fragmentSourceStr += "\nuniform vec" + (std::string)(((options & SO_ALPHA) == SO_ALPHA) ? "4" : "3") + " Color;";
    }
    fragmentSourceStr += R"glsl(
        uniform vec3 viewPos;
        uniform unsigned int specPower;
        uniform PointLight lights[)glsl" + std::to_string(numberLights) + R"glsl(];

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
            vec3 ambient  = light.ambient  * )glsl" + (std::string)(((options & SO_MATERIAL) == SO_MATERIAL) ? "AmbientMat" : "Color.xyz") + R"glsl(;
            vec3 diffuse  = light.diffuse  * diff * )glsl" + (std::string)(((options & SO_MATERIAL) == SO_MATERIAL) ? "DiffuseMat" : "Color.xyz") + R"glsl(;
            vec3 specular = light.specular * spec * )glsl" + (std::string)(((options & SO_MATERIAL) == SO_MATERIAL) ? "SpecularMat" : "Color.xyz") + R"glsl(;
            ambient  *= attenuation;
            diffuse  *= attenuation;
            specular *= attenuation;
            return (ambient + diffuse + specular);
        }

        void main()
        {
            vec3 viewDir = normalize(viewPos - worldPos); 
            vec3 result = vec3(0.0, 0.0, 0.0);
            for (int i = 0; i < )glsl" + std::to_string(numberLights) + R"glsl(; i++) {
                result += CalcPointLight(lights[i], norm, viewDir);
            })glsl";
    if ((options & SO_ALPHA) == SO_ALPHA) {
        if ((options & SO_MATERIAL) == SO_MATERIAL) {
            fragmentSourceStr += "\n\toutColor = vec4(result, AlphaMat);";
        } else {
            fragmentSourceStr += "\n\toutColor = vec4(result, Color.a);";
        }
    } else {
        fragmentSourceStr += "outColor = vec4(result, 1.0);";
    }
    fragmentSourceStr += R"glsl(
        }
    )glsl";
    const char* fragmentSource = fragmentSourceStr.c_str();
    createFragmentShader(fragmentSource);
    linkProgram();

    // get locations of shader uniforms
    normalMatrixLoc = glGetUniformLocation(this->getProgramID(), "normalMatrix");
    viewPositionLoc = glGetUniformLocation(this->getProgramID(), "viewPos");
    specularPowerLoc = glGetUniformLocation(this->getProgramID(), "specPower");
    setSpecularPower(32);

    if ((options & SO_INSTANCED) == SO_INSTANCED) {
        postModelMatrixLoc = glGetUniformLocation(this->getProgramID(), "postModel");
        postNormalMatrixLoc = glGetUniformLocation(this->getProgramID(), "postNormalMatrix");
    }
    if ((options & SO_MATERIAL) == SO_MATERIAL) {
        if ((options & SO_AMBIENT_ATTRIBUTE) != SO_AMBIENT_ATTRIBUTE) {
            ambientMatLoc = glGetUniformLocation(this->getProgramID(), "AmbientMat");
        }
        if ((options & SO_DIFFUSE_ATTRIBUTE) != SO_DIFFUSE_ATTRIBUTE) {
            diffuseMatLoc = glGetUniformLocation(this->getProgramID(), "DiffuseMat");
        }
        if ((options & SO_SPECULAR_ATTRIBUTE) != SO_SPECULAR_ATTRIBUTE) {
            specularMatLoc = glGetUniformLocation(this->getProgramID(), "SpecularMat");
        }
        if ((options & SO_ALPHA_ATTRIBUTE) != SO_ALPHA_ATTRIBUTE and (options & SO_ALPHA) == SO_ALPHA) {
            alphaMatLoc = glGetUniformLocation(this->getProgramID(), "AlphaMat");
        }
    } else if ((options & SO_COLOR_ATTRIBUTE) != SO_COLOR_ATTRIBUTE) {
        colorLoc = glGetUniformLocation(this->getProgramID(), "Color");
    }

    return this->getProgramID();
}

//applies the model matrix to the shader program
//model matrix transforms from modelspace to worldspace
//also creates and applies the corresponding normal matrix
void sceneObjects::SO_PhongShader::setModelMatrix(glm::mat4 modelMatrix) {
    SO_Shader::setModelMatrix(modelMatrix);
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    glProgramUniformMatrix4fv(this->getProgramID(), normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

//applies the post model matrix to the shader program
//the post model matrix is applied to all instances of the model in world space
//only useable if the shader has instanced set to true
//
//model matrix transforms from worldspace to worldspace
//also creates and applies the corresponding normal matrix
void sceneObjects::SO_PhongShader::setPostModelMatrix(glm::mat4 modelMatrix) {
    if ((options & SO_INSTANCED) == SO_INSTANCED) {
        glProgramUniformMatrix4fv(this->getProgramID(), postModelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
        glProgramUniformMatrix4fv(this->getProgramID(), postNormalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
}

//set the position of the camera in the shader program in worldspace
//method has no effect in SO::Shader base class
void sceneObjects::SO_PhongShader::setViewPosition(glm::vec3 viewPosition) {
    glProgramUniform3fv(this->getProgramID(), viewPositionLoc, 1, glm::value_ptr(viewPosition));
}

//set the position of a light in worldspace
//index is the number of the light (rom 0 to numberLights-1)
//lightPosition is the position
void sceneObjects::SO_PhongShader::setLightPosition(int index, glm::vec3 lightPosition) {
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
void sceneObjects::SO_PhongShader::setLightConstant(int index, float lightConstant) {
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
void sceneObjects::SO_PhongShader::setLightLinear(int index, float lightLinear) {
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
void sceneObjects::SO_PhongShader::setLightQuadratic(int index, float lightQuadratic) {
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
void sceneObjects::SO_PhongShader::setLightAmbient(int index, glm::vec3 lightAmbient) {
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
void sceneObjects::SO_PhongShader::setLightDiffuse(int index, glm::vec3 lightDiffuse) {
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
void sceneObjects::SO_PhongShader::setLightSpecular(int index, glm::vec3 lightSpecular) {
    if (index < 0 || index >= numberLights) {
        std::string error = "index to lights array is out of range\nrecieved: " + std::to_string(index) + "\nlength: " + std::to_string(numberLights);
        throw std::invalid_argument(error.c_str());
    }
    std::string name = "lights[" + std::to_string(index) + "].specular";
    GLint lightSpecularLoc = glGetUniformLocation(this->getProgramID(), name.c_str());
    glProgramUniform3fv(this->getProgramID(), lightSpecularLoc, 1, glm::value_ptr(lightSpecular));
}

//set the ambient color of a material
void sceneObjects::SO_PhongShader::setMaterialAmbient(glm::vec3 ambientMaterial) {
    if ((options & SO_MATERIAL) == SO_MATERIAL and (options & SO_AMBIENT_ATTRIBUTE) != SO_AMBIENT_ATTRIBUTE) {
        glProgramUniform3fv(this->getProgramID(), ambientMatLoc, 1, glm::value_ptr(ambientMaterial));
    }
}

//set the diffuse color of a material
void sceneObjects::SO_PhongShader::setMaterialDiffuse(glm::vec3 diffuseMaterial) {
    if ((options & SO_MATERIAL) == SO_MATERIAL and (options & SO_DIFFUSE_ATTRIBUTE) != SO_DIFFUSE_ATTRIBUTE) {
        glProgramUniform3fv(this->getProgramID(), diffuseMatLoc, 1, glm::value_ptr(diffuseMaterial));
    }
}

//set the specular color of a material
void sceneObjects::SO_PhongShader::setMaterialSpecular(glm::vec3 specularMaterial) {
    if ((options & SO_MATERIAL) == SO_MATERIAL and (options & SO_SPECULAR_ATTRIBUTE) != SO_SPECULAR_ATTRIBUTE) {
        glProgramUniform3fv(this->getProgramID(), specularMatLoc, 1, glm::value_ptr(specularMaterial));
    }
}

//set the alpha of a material
void sceneObjects::SO_PhongShader::setMaterialAlpha(float alphaMaterial) {
    if ((options & SO_MATERIAL) == SO_MATERIAL and (options & SO_ALPHA_ATTRIBUTE) != SO_ALPHA_ATTRIBUTE and (options & SO_ALPHA) == SO_ALPHA) {
        glProgramUniform1f(this->getProgramID(), ambientMatLoc, alphaMaterial);
    }
}

void sceneObjects::SO_PhongShader::setColor(glm::vec3 color) {
    if ((options & SO_MATERIAL) != SO_MATERIAL) {
        if ((options & SO_ALPHA) != SO_ALPHA) {
            glProgramUniform3fv(this->getProgramID(), colorLoc, 1, glm::value_ptr(color));
        } else {
            throw std::invalid_argument("cannot pass vec3 as color argument when alpha is enabled");
        }
    }
}

void sceneObjects::SO_PhongShader::setColor(glm::vec4 color) {
    if ((options & SO_MATERIAL) != SO_MATERIAL) {
        if ((options & SO_ALPHA) == SO_ALPHA) {
            glProgramUniform4fv(this->getProgramID(), colorLoc, 1, glm::value_ptr(color));
        } else {
            throw std::invalid_argument("cannot pass vec4 as color argument when alpha is disabled");
        }
    }
}

void sceneObjects::SO_PhongShader::setSpecularPower(unsigned int specPower) {
    glProgramUniform1ui(this->getProgramID(), specularPowerLoc, specPower);
}