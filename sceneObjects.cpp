#define STB_IMAGE_IMPLEMENTATION

#include "sceneObjects.hpp"
#include <stb_image.h>
#include <stdio.h>
#include <memory>
#include <cstdio>
#include <string>
#include <vector>
#include <stdexcept>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//compile a vertex shader from the given char*
void SO_Shader::createVertexShader(const char* vertexSource) {
    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexSource, NULL);
    glCompileShader(vertexShaderID);

    GLint status;
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) {
        printf("vertex shader %d compiled successfully\n", vertexShaderID);
    } else {
        char buffer[512];
        glGetShaderInfoLog(vertexShaderID, 512, NULL, buffer);    
        printf(vertexSource);
        printf("\nvertex shader %d failed to compile\n", vertexShaderID);
        printf("%s\n", buffer);
    }
}

//compile a fragment shader from the given char*
void SO_Shader::createFragmentShader(const char* fragmentSource) {
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShaderID);
    GLint status;
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) {
        printf("fragment shader %d compiled successfully\n", fragmentShaderID);
    } else {
        char buffer[512];
        glGetShaderInfoLog(fragmentShaderID, 512, NULL, buffer);
        printf(fragmentSource);    
        printf("\nfragment shader %d failed to compile\n", fragmentShaderID);
        printf("%s\n", buffer);
    }
}

//link together the previously compiled shaders into a program
void SO_Shader::linkProgram(void) {
    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glBindFragDataLocation(programID, 0, "outColor");
    glLinkProgram(programID);
    GLint status;
    glGetProgramiv(programID, GL_LINK_STATUS, &status);
    if (status == GL_TRUE) {
        printf("program %d linked successfully\n", programID);
    } else {
        char buffer[512];
        glGetProgramInfoLog(programID, 512, NULL, buffer);    
        printf("program %d failed to link\n", programID);
        printf("%s\n", buffer);
    }
    modelMatrixLoc = glGetUniformLocation(programID, "model");
    viewMatrixLoc = glGetUniformLocation(programID, "view");
    projectionMatrixLoc = glGetUniformLocation(programID, "proj");
    programCreated = true;
    glDeleteShader(fragmentShaderID);
    glDeleteShader(vertexShaderID);
}

//returns the ID of the program associated with this class
GLuint SO_Shader::getProgramID(void) {
    return programID;
}

//applies the model matrix to the shader program
//model matrix transforms from modelspace to worldspace
void SO_Shader::setModelMatrix(glm::mat4 modelMatrix) {
    glProgramUniformMatrix4fv(programID, modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

//applies the view matrix to the shader program
//view matrix transforms from worldspace to cameraspace
void SO_Shader::setViewMatrix(glm::mat4 viewMatrix) {
    glProgramUniformMatrix4fv(programID, viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}

//set the position of the camera in the shader program in worldspace
//method has no effect in SO::Shader base class
void SO_Shader::setViewPosition(glm::vec3 viewPosition) {
    return;
}

//applies the projection matrix to the shader program
//projection matrix transforms from cameraspace to clipspace
void SO_Shader::setProjectionMatrix(glm::mat4 projectionMatrix) {
    glProgramUniformMatrix4fv(programID, projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

//destructor deletes the opengl program
SO_Shader::~SO_Shader(void) {
    if (programCreated) {
        glDeleteProgram(programID); //destructor deletes program
    }
}


//numberLights gives number of point lights in the scene
//optionsIn : binary flags given by enums SO_ShaderOptions
// expects in: position, normal, (ambient/diffuse/specular/alpha)Attrib, color, instanceMatrix, normalInstMatrix
//uniforms: set with SO_PhongShader::set_____ methods
GLuint SO_PhongShader::generate(int numberLightsIn, unsigned int optionsIn) {
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
void SO_PhongShader::setModelMatrix(glm::mat4 modelMatrix) {
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
void SO_PhongShader::setPostModelMatrix(glm::mat4 modelMatrix) {
    if ((options & SO_INSTANCED) == SO_INSTANCED) {
        glProgramUniformMatrix4fv(this->getProgramID(), postModelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
        glProgramUniformMatrix4fv(this->getProgramID(), postNormalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
}

//set the position of the camera in the shader program in worldspace
//method has no effect in SO::Shader base class
void SO_PhongShader::setViewPosition(glm::vec3 viewPosition) {
    glProgramUniform3fv(this->getProgramID(), viewPositionLoc, 1, glm::value_ptr(viewPosition));
}

//set the position of a light in worldspace
//index is the number of the light (rom 0 to numberLights-1)
//lightPosition is the position
void SO_PhongShader::setLightPosition(int index, glm::vec3 lightPosition) {
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
void SO_PhongShader::setLightConstant(int index, float lightConstant) {
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
void SO_PhongShader::setLightLinear(int index, float lightLinear) {
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
void SO_PhongShader::setLightQuadratic(int index, float lightQuadratic) {
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
void SO_PhongShader::setLightAmbient(int index, glm::vec3 lightAmbient) {
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
void SO_PhongShader::setLightDiffuse(int index, glm::vec3 lightDiffuse) {
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
void SO_PhongShader::setLightSpecular(int index, glm::vec3 lightSpecular) {
    if (index < 0 || index >= numberLights) {
        std::string error = "index to lights array is out of range\nrecieved: " + std::to_string(index) + "\nlength: " + std::to_string(numberLights);
        throw std::invalid_argument(error.c_str());
    }
    std::string name = "lights[" + std::to_string(index) + "].specular";
    GLint lightSpecularLoc = glGetUniformLocation(this->getProgramID(), name.c_str());
    glProgramUniform3fv(this->getProgramID(), lightSpecularLoc, 1, glm::value_ptr(lightSpecular));
}

//set the ambient color of a material
void SO_PhongShader::setMaterialAmbient(glm::vec3 ambientMaterial) {
    if ((options & SO_MATERIAL) == SO_MATERIAL and (options & SO_AMBIENT_ATTRIBUTE) != SO_AMBIENT_ATTRIBUTE) {
        glProgramUniform3fv(this->getProgramID(), ambientMatLoc, 1, glm::value_ptr(ambientMaterial));
    }
}

//set the diffuse color of a material
void SO_PhongShader::setMaterialDiffuse(glm::vec3 diffuseMaterial) {
    if ((options & SO_MATERIAL) == SO_MATERIAL and (options & SO_DIFFUSE_ATTRIBUTE) != SO_DIFFUSE_ATTRIBUTE) {
        glProgramUniform3fv(this->getProgramID(), diffuseMatLoc, 1, glm::value_ptr(diffuseMaterial));
    }
}

//set the specular color of a material
void SO_PhongShader::setMaterialSpecular(glm::vec3 specularMaterial) {
    if ((options & SO_MATERIAL) == SO_MATERIAL and (options & SO_SPECULAR_ATTRIBUTE) != SO_SPECULAR_ATTRIBUTE) {
        glProgramUniform3fv(this->getProgramID(), specularMatLoc, 1, glm::value_ptr(specularMaterial));
    }
}

//set the alpha of a material
void SO_PhongShader::setMaterialAlpha(float alphaMaterial) {
    if ((options & SO_MATERIAL) == SO_MATERIAL and (options & SO_ALPHA_ATTRIBUTE) != SO_ALPHA_ATTRIBUTE and (options & SO_ALPHA) == SO_ALPHA) {
        glProgramUniform1f(this->getProgramID(), ambientMatLoc, alphaMaterial);
    }
}

void SO_PhongShader::setColor(glm::vec3 color) {
    if ((options & SO_MATERIAL) != SO_MATERIAL) {
        if ((options & SO_ALPHA) != SO_ALPHA) {
            glProgramUniform3fv(this->getProgramID(), colorLoc, 1, glm::value_ptr(color));
        } else {
            throw std::invalid_argument("cannot pass vec3 as color argument when alpha is enabled");
        }
    }
}

void SO_PhongShader::setColor(glm::vec4 color) {
    if ((options & SO_MATERIAL) != SO_MATERIAL) {
        if ((options & SO_ALPHA) == SO_ALPHA) {
            glProgramUniform4fv(this->getProgramID(), colorLoc, 1, glm::value_ptr(color));
        } else {
            throw std::invalid_argument("cannot pass vec4 as color argument when alpha is disabled");
        }
    }
}

void SO_PhongShader::setSpecularPower(unsigned int specPower) {
    glProgramUniform1ui(this->getProgramID(), specularPowerLoc, specPower);
}

float skyboxVertices[] = {
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
GLuint SO_SkyboxShader::generate(std::vector<std::string> imageFilesIn) {
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
void SO_SkyboxShader::setModelMatrix(glm::mat4 modelMatrix) {
    modelMatrix = glm::mat4(glm::mat3(modelMatrix));
    SO_Shader::setModelMatrix(modelMatrix);
}

//set the view matrix of the skybox (removes translations)
void SO_SkyboxShader::setViewMatrix(glm::mat4 viewMatrix) {
    viewMatrix = glm::mat4(glm::mat3(viewMatrix));
    SO_Shader::setViewMatrix(viewMatrix);
}

//render the skybox - should be last render done for efficiency
//this operation leaves the shader and VAO set on the skybox
void SO_SkyboxShader::render(GLenum depthFuncReset) {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(this->getProgramID());
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(depthFuncReset);
}

//destructor
SO_SkyboxShader::~SO_SkyboxShader(void) {
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
}

// generate a shader program for a assimp mesh
GLuint SO_AssimpShader::generate(int numberLightsIn, int diffuseTextures, int specularTextures, int normalTextures) {
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
void SO_AssimpShader::setModelMatrix(glm::mat4 modelMatrix) {
    SO_Shader::setModelMatrix(modelMatrix);
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    glProgramUniformMatrix4fv(this->getProgramID(), normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

//set the position of the camera in the shader program in worldspace
//method has no effect in SO::Shader base class
void SO_AssimpShader::setViewPosition(glm::vec3 viewPosition) {
    glProgramUniform3fv(this->getProgramID(), viewPositionLoc, 1, glm::value_ptr(viewPosition));
}

//set the position of a light in worldspace
//index is the number of the light (rom 0 to numberLights-1)
//lightPosition is the position
void SO_AssimpShader::setLightPosition(int index, glm::vec3 lightPosition) {
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
void SO_AssimpShader::setLightConstant(int index, float lightConstant) {
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
void SO_AssimpShader::setLightLinear(int index, float lightLinear) {
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
void SO_AssimpShader::setLightQuadratic(int index, float lightQuadratic) {
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
void SO_AssimpShader::setLightAmbient(int index, glm::vec3 lightAmbient) {
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
void SO_AssimpShader::setLightDiffuse(int index, glm::vec3 lightDiffuse) {
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
void SO_AssimpShader::setLightSpecular(int index, glm::vec3 lightSpecular) {
    if (index < 0 || index >= numberLights) {
        std::string error = "index to lights array is out of range\nrecieved: " + std::to_string(index) + "\nlength: " + std::to_string(numberLights);
        throw std::invalid_argument(error.c_str());
    }
    std::string name = "lights[" + std::to_string(index) + "].specular";
    GLint lightSpecularLoc = glGetUniformLocation(this->getProgramID(), name.c_str());
    glProgramUniform3fv(this->getProgramID(), lightSpecularLoc, 1, glm::value_ptr(lightSpecular));
}

void SO_AssimpShader::setSpecularPower(unsigned int specPower) {
    glProgramUniform1ui(this->getProgramID(), specularPowerLoc, specPower);
}

// craetes a shader for the mesh
SO_AssimpShader* SO_AssimpMesh::createShader(int numberLights) {
    shader = SO_AssimpShader();
    shader.generate(numberLights, diffuseMaps.size(), specularMaps.size(), normalMaps.size());
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SO_AssimpVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), &elements[0], GL_STATIC_DRAW);

    //vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SO_AssimpVertex), (void*)0);
    //normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SO_AssimpVertex), (void*)offsetof(SO_AssimpVertex, normal));
    //texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SO_AssimpVertex), (void*)offsetof(SO_AssimpVertex, texCoords));
    if (normalMaps.size() > 0) {
        //tangents
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SO_AssimpVertex), (void*)offsetof(SO_AssimpVertex, tangent));
    }

    glBindVertexArray(0);

    return &shader;
}


//draws the mesh - call at render time
void SO_AssimpMesh::draw() {
    glUseProgram(shader.getProgramID());
    if (diffuseMaps.size() == 0) {
        glUniform3fv(glGetUniformLocation(shader.getProgramID(), "colorDiffuse"), 1, glm::value_ptr(diffuseColor));
    } else {
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "textureDiffuse"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMaps[0].textureId);
    }
    if (specularMaps.size() == 0) {
        glUniform3fv(glGetUniformLocation(shader.getProgramID(), "colorSpecular"), 1, glm::value_ptr(specularColor));
    } else {
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "textureSpecular"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMaps[0].textureId);
    }
    if (normalMaps.size() > 0) {
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "textureNormal"), 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normalMaps[0].textureId);
    }

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);
}

SO_AssimpMesh::~SO_AssimpMesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

//constructor for assimp model (only allows 1 pair of texture coords)
//aiOptions should be members of aiPostProccessSteps enum e.g.
// -aiProcess_FlipUVs
// -aiProcess_GenNormals
// -aiProcess_OptimizeMeshes
// -aiProcess_OptimizeGraph
// Note that aiTriangulate is always called
SO_AssimpModel::SO_AssimpModel(std::string path, int aiOptions) {
    loadModel(path, aiOptions);
}

//constructor for assimp model (only allows 1 pair of texture coords)
//aiOptions should be members of aiPostProccessSteps enum e.g.
// -aiProcess_FlipUVs
// -aiProcess_GenNormals
// -aiProcess_OptimizeMeshes
// -aiProcess_OptimizeGraph
// Note that aiTriangulate is always called
void SO_AssimpModel::loadModel(std::string path, int aiOptions) {
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
void SO_AssimpModel::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

//convertes an assimp mesh into an SO mesh
SO_AssimpMesh SO_AssimpModel::processMesh(aiMesh* mesh, const aiScene* scene) {
    SO_AssimpMesh SOMesh;
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
std::vector<SO_AssimpTexture> SO_AssimpModel::loadMaterialTextures(aiMaterial* material, aiTextureType type) {
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
GLuint SO_AssimpModel::loadTextureFromFile(std::string path) {
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
void SO_AssimpModel::draw() {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].draw();
    }
}

//creates all the shaders for the meshes
void SO_AssimpModel::createShaders(int numberLights) {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].createShader(numberLights);
    }
}


SO_Camera::SO_Camera(float fovIn=45.0f, float aspectRatioIn=1.0f, float nearClipIn=0.1f, float farClipIn=100.0f, 
                        glm::vec3 positionIn=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 frontIn=glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 upIn=glm::vec3(0.0f, 1.0f, 0.0f)) {
    fov = fovIn;
    aspectRatio = aspectRatioIn;
    nearClip = nearClipIn;
    farClip = farClipIn;
    position = positionIn;
    front = frontIn;
    up = upIn;
}

//update the view matrix of all linked shaders
void SO_Camera::updateViewMatrix(void) {
    glm::mat4 viewMatrix = glm::lookAt(position, position+front, up);
    for (unsigned int i = 0; i < shaderPointers.size(); i++) {
        shaderPointers[i]->setViewMatrix(viewMatrix);
        shaderPointers[i]->setViewPosition(position);
    }
}

//update the projection matrix of all linked shaders
void SO_Camera::updateProjectionMatrix(void) {
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
    for (unsigned int i = 0; i < shaderPointers.size(); i++) {
        shaderPointers[i]->setProjectionMatrix(projectionMatrix);
    }
}

//link the camera to the shader program given by pointer
void SO_Camera::linkShader(SO_Shader *shaderRefIn) {
    for (unsigned int i = 0; i < shaderPointers.size(); i++) {
        if (shaderPointers[i] == shaderRefIn) {
            return;
        }
    }
    shaderPointers.push_back(shaderRefIn);
}

//unlink the camera to the shader program given by pointer
void SO_Camera::unlinkShader(SO_Shader *shaderRefIn) {
    for (unsigned int i = 0; i < shaderPointers.size(); i++) {
        if (shaderPointers[i] == shaderRefIn) {
            shaderPointers.erase(shaderPointers.begin() + i);
            return;
        }
    }
}

#ifdef _WIN32
SO_FfmpegStream::SO_FfmpegStream(std::string filepathIn) {
    setFilepath(filepathIn);
}

//takes an std::string to set the filepath of the next stream opened
void SO_FfmpegStream::setFilepath(std::string filepathIn) {
    filepath = filepathIn;
}


//start a stream to an ffmpeg file
void SO_FfmpegStream::openStream(int widthIn, int heightIn) {
    //http://blog.mmacklin.com/2013/06/11/real-time-video-capture-with-ffmpeg/
    width = widthIn;
    height = heightIn;

    std::string cmd = "ffmpeg -r 60 -f rawvideo -pix_fmt rgba -s " + std::to_string(width) + "x" + std::to_string(height) + " -i - "
                        "-threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip " + filepath;

    ffmpeg = _popen(cmd.c_str(), "wb");

    buffer = std::make_unique<int[]>(width*height);

    streaming = true;
}

//render a frame
void SO_FfmpegStream::renderFrame(void) {
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.get());
    fwrite(buffer.get(), sizeof(int)*width*height, 1, ffmpeg);
}

void SO_FfmpegStream::closeStream(void) {
    _pclose(ffmpeg);
}
#endif

//create vector at a ratio of division/subdivisions from vector1 to vector2
glm::vec3 createRatioVector(int subdivisions, int division, glm::vec3 vector1, glm::vec3 vector2) {
    return glm::vec3((float)(subdivisions-division)*vector1 + (float)division*vector2)/(float)(subdivisions);
}

//create a sphere mesh starting with an icosohedron base (an icosphere)
//subdivisions is the number of divisions along each edge of the starting icosahedron
SO_MeshData createIcosphere(int subdivisions) {
    // create icosahedron https://en.wikipedia.org/wiki/Regular_icosahedron, https://en.wikipedia.org/wiki/Regular_icosahedron#/media/File:Icosahedron-golden-rectangles.svg
    float phi = (1.0f + sqrt(5.0f))/2;
    std::vector<glm::vec3> vertices; //vector because more vertices will be added when the mesh is created
    vertices.push_back(glm::vec3(0.0f,  1.0f,   phi));  //0
    vertices.push_back(glm::vec3(0.0f,  1.0f,   -phi)); //1
    vertices.push_back(glm::vec3(0.0f,  -1.0f,  phi));  //2
    vertices.push_back(glm::vec3(0.0f,  -1.0f,  -phi)); //3
    vertices.push_back(glm::vec3(phi,   0.0f,   1.0f)); //4
    vertices.push_back(glm::vec3(-phi,   0.0f,   1.0f));//5
    vertices.push_back(glm::vec3(phi,  0.0f,   -1.0f)); //6
    vertices.push_back(glm::vec3(-phi,  0.0f,   -1.0f));//7
    vertices.push_back(glm::vec3(1.0f,  phi,    0.0f)); //8
    vertices.push_back(glm::vec3(1.0f,  -phi,   0.0f)); //9
    vertices.push_back(glm::vec3(-1.0f, phi,    0.0f)); //10
    vertices.push_back(glm::vec3(-1.0f, -phi,   0.0f)); //11
    int edges[30][subdivisions+1]; //https://en.wikipedia.org/wiki/Regular_icosahedron#/media/File:Icosahedron-golden-rectangles.svg
    //edges in order with smallest indexed vertex first - use indicesnnot ponters so can construct an elements array for OpenGL with them
    //edges across short edges
    edges[0][0] = 0;    edges[0][subdivisions] = 2;
    edges[1][0] = 1;    edges[1][subdivisions] = 3;
    edges[2][0] = 4;    edges[2][subdivisions] = 6;
    edges[3][0] = 5;    edges[3][subdivisions] = 7;
    edges[4][0] = 8;    edges[4][subdivisions] = 10;
    edges[5][0] = 9;    edges[5][subdivisions] = 11;
    //edges across gaps - each +-1.0 goes to the vertex with a corresponding +-phi coordinate
    edges[6][0] = 0;    edges[6][subdivisions] = 8;
    edges[7][0] = 0;    edges[7][subdivisions] = 10;
    edges[8][0] = 1;    edges[8][subdivisions] = 8;
    edges[9][0] = 1;    edges[9][subdivisions] = 10;
    edges[10][0] = 2;   edges[10][subdivisions] = 9;
    edges[11][0] = 2;   edges[11][subdivisions] = 11;
    edges[12][0] = 3;   edges[12][subdivisions] = 9;
    edges[13][0] = 3;   edges[13][subdivisions] = 11;
    edges[14][0] = 0;   edges[14][subdivisions] = 4;
    edges[15][0] = 2;   edges[15][subdivisions] = 4;
    edges[16][0] = 0;   edges[16][subdivisions] = 5;
    edges[17][0] = 2;   edges[17][subdivisions] = 5;
    edges[18][0] = 1;   edges[18][subdivisions] = 6;
    edges[19][0] = 3;   edges[19][subdivisions] = 6;
    edges[20][0] = 1;   edges[20][subdivisions] = 7;
    edges[21][0] = 3;   edges[21][subdivisions] = 7;
    edges[22][0] = 4;   edges[22][subdivisions] = 8;
    edges[23][0] = 6;   edges[23][subdivisions] = 8;
    edges[24][0] = 4;   edges[24][subdivisions] = 9;
    edges[25][0] = 6;   edges[25][subdivisions] = 9;
    edges[26][0] = 5;   edges[26][subdivisions] = 10;
    edges[27][0] = 7;   edges[27][subdivisions] = 10;
    edges[28][0] = 5;   edges[28][subdivisions] = 11;
    edges[29][0] = 7;   edges[29][subdivisions] = 11;
    int faces[20][3];
    //faces in order so edge: 0: (a, b)
    //                        1: (a, c)
    //                        2: (b, c)     with a < b < c
    //faces with a short rectangle edge, if one edge is the short one - the others must be one that connects to the same vertex
    // eg edges[0] contains 0,2 : and only vertices 4,5 connect to both via an edge eg. edges 0, 14, 15
    faces[0][0] = 0;    faces[0][1] = 14,   faces[0][2] = 15;
    faces[1][0] = 0;    faces[1][1] = 16;   faces[1][2] = 17;
    faces[2][0] = 1;    faces[2][1] = 18;   faces[2][2] = 19;
    faces[3][0] = 1;    faces[3][1] = 20;   faces[3][2] = 21;
    faces[4][0] = 2;    faces[4][1] = 22;   faces[4][2] = 23;
    faces[5][0] = 2;    faces[5][1] = 24;   faces[5][2] = 25;
    faces[6][0] = 3;    faces[6][1] = 26;   faces[6][2] = 27;
    faces[7][0] = 3;    faces[7][1] = 28;   faces[7][2] = 29;
    faces[8][0] = 6;    faces[8][1] = 7;    faces[8][2] = 4;
    faces[9][0] = 8;    faces[9][1] = 9;    faces[9][2] = 4;
    faces[10][0] = 10;  faces[10][1] = 11;  faces[10][2] = 5;
    faces[11][0] = 12;  faces[11][1] = 13;  faces[11][2] = 5;
    //final 8 faces - consider 1 rectange in image above, in direction of +-1, point 2 has +-phi in same coord and +-1 in other, 
    //final point has +-phi in same coord as 2nd's +-1 and +-1 in same coord as 1sts +-phi. All +-s must match per coord
    faces[12][0] = 14; faces[12][1] = 6; faces[12][2] = 22;
    faces[13][0] = 16; faces[13][1] = 7; faces[13][2] = 26;
    faces[14][0] = 18; faces[14][1] = 8; faces[14][2] = 23;
    faces[15][0] = 20; faces[15][1] = 9; faces[15][2] = 27;
    faces[16][0] = 15; faces[16][1] = 10; faces[16][2] = 24;
    faces[17][0] = 17; faces[17][1] = 11; faces[17][2] = 28;
    faces[18][0] = 19; faces[18][1] = 12; faces[18][2] = 25;
    faces[19][0] = 21; faces[19][1] = 13; faces[19][2] = 29;

    //subdivide each icosohedron edge
    for (int division = 1; division < subdivisions; division++) {
        for (int edge = 0; edge < 30; edge++) {
            glm::vec3 newVector = createRatioVector(subdivisions, division, vertices[edges[edge][0]], vertices[edges[edge][subdivisions]]);;
            vertices.push_back(newVector);
            edges[edge][division] = vertices.size() - 1;
        }
    }

    //create inner points on each face
    //face has vertices a,b,c a<b<c and edges (a,b), (a,c), (b,c). rows start at row[0] = vertex a and move towards row[subdivision] = edge (b, c)
    std::vector<int> faceElements;
    for (int face = 0; face < 20; face++) {
        std::vector<int> currentRow; // current row of vertices being added
        std::vector<int> nextRow; // next row of vertices - being created to provide end vertices for faces on currentrow
        currentRow.push_back(edges[faces[face][0]][0]); // start with current row being just vertex a
        for (int row = 0; row < subdivisions; row++) { // last row of faces is row[subdivision-1]
            if (row == subdivisions - 1) { // if final row then should be set equal to the edge (b,c)
                nextRow.clear();
                for (int j = 0; j <= subdivisions; j++) {
                    nextRow.push_back(edges[faces[face][2]][j]);
                }
            } else { // if not final row
                nextRow.clear();
                nextRow.push_back(edges[faces[face][0]][row+1]); // add first vector from edge (a,b)
                //add intervening vertices
                for (int slice = 1; slice < row+1; slice++) { //add subdvision vectors
                    glm::vec3 newVector = createRatioVector(row+1, slice, vertices[edges[faces[face][0]][row+1]], vertices[edges[faces[face][1]][row+1]]);
                    vertices.push_back(newVector);
                    nextRow.push_back(vertices.size() - 1);
                }
                nextRow.push_back(edges[faces[face][1]][row+1]); //add final vector from edge (a,c)
            }
            // add faces to the list
            for (int j = 0; j <= row; j++) { //add faces with point on currentRow
                faceElements.push_back(currentRow[j]);
                faceElements.push_back(nextRow[j]);
                faceElements.push_back(nextRow[j+1]);
            }
            for (int j = 0; j < row; j++) { //add faces with edge on currentRow
                faceElements.push_back(currentRow[j]);
                faceElements.push_back(currentRow[j+1]);
                faceElements.push_back(nextRow[j+1]);
            }
            currentRow = nextRow;
        }
    }

    for (unsigned int i =0; i < vertices.size(); i++) {
        vertices[i] = glm::normalize(vertices[i]);
    }

    SO_MeshData icosphereData;
    icosphereData.vertices = vertices;
    icosphereData.faceElements = faceElements;
    return icosphereData;

}

//a gradual step function between (0,0) and (1,1)
double fade(double x) {
    if (x <= 0) {
        return 0;
    } else if (x >= 1) {
        return 1;
    }
    return x*x*x*(x*(x*6 - 15) + 10); //return 6t^5-15t^4+10t^3 smooth between (0,0) and (1,1)
}

//increments around a loop
int inc(int num, int repeat) {
    num++;
    if (repeat > 0) {
        num %= repeat;
    }
    return num;
}

//get gradient of perlin corner
double grad(int hash, double x, double y, double z) {
     switch(hash & 0xF) {
        case 0x0: return  x + y;
        case 0x1: return -x + y;
        case 0x2: return  x - y;
        case 0x3: return -x - y;
        case 0x4: return  x + z;
        case 0x5: return -x + z;
        case 0x6: return  x - z;
        case 0x7: return -x - z;
        case 0x8: return  y + z;
        case 0x9: return -y + z;
        case 0xA: return  y - z;
        case 0xB: return -y - z;
        case 0xC: return  y + x;
        case 0xD: return -y + z;
        case 0xE: return  y - x;
        case 0xF: return -y - z;
        default: return 0; // never happens
    }
}

//moduus function without negatives
double modulus(double x, double y) {
    return x - y*floor(x/y);
}

int perlinPerms[512] = { 151,160,137,91,90,15,                 // Hash lookup table as defined by Ken Perlin.  This is a randomly
                        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,    // arranged array of all numbers from 0-255 inclusive.
                        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
                        88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
                        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
                        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
                        135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
                        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
                        223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
                        129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
                        251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
                        49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
                        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,

                        151,160,137,91,90,15,                 // Repeats due to inc function not being looped, more effieicent than recreating each call?
                        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
                        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
                        88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
                        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
                        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
                        135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
                        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
                        223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
                        129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
                        251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
                        49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
                        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
                    };

//get perlin noise at coords (x,y,z)
double perlin(double x, double y, double z, double repeat) { //https://adrianb.io/2014/08/09/perlinnoise.html
    if (repeat > 0) {
        x = modulus(x, repeat);
        y = modulus(y, repeat);
        z = modulus(z, repeat);
    }

    x = modulus(x, 256);
    y = modulus(y, 256);
    z = modulus(z, 256);

    int xi = (int)x & 255;                           // Calculate the "unit cube" that the point asked will be located in
    int yi = (int)y & 255;                              // The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
    int zi = (int)z & 255;                              // plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
    double xf = x-(int)x;
    double yf = y-(int)y;
    double zf = z-(int)z;

    double u = fade(xf);
    double v = fade(yf);
    double w = fade(zf);

    int aaa, aba, aab, abb, baa, bba, bab, bbb;
    aaa = perlinPerms[perlinPerms[perlinPerms[    xi ]        +    yi ]           +    zi ];
    aba = perlinPerms[perlinPerms[perlinPerms[    xi ]        +inc(yi, repeat)]   +    zi ];
    aab = perlinPerms[perlinPerms[perlinPerms[    xi ]        +    yi ]           +inc(zi, repeat)];
    abb = perlinPerms[perlinPerms[perlinPerms[    xi ]        +inc(yi, repeat)]   +inc(zi, repeat)];
    baa = perlinPerms[perlinPerms[perlinPerms[inc(xi, repeat)]+    yi ]           +    zi ];
    bba = perlinPerms[perlinPerms[perlinPerms[inc(xi, repeat)]+inc(yi, repeat)]   +    zi ];
    bab = perlinPerms[perlinPerms[perlinPerms[inc(xi, repeat)]+    yi ]           +inc(zi, repeat)];
    bbb = perlinPerms[perlinPerms[perlinPerms[inc(xi, repeat)]+inc(yi, repeat)]   +inc(zi, repeat)];

    double x1, x2, y1, y2;
    x1 = lerp(    grad (aaa, xf  , yf  , zf),           // The gradient function calculates the dot product between a pseudorandom
                grad (baa, xf-1, yf  , zf),             // gradient vector and the vector from the input coordinate to the 8
                u);                                     // surrounding points in its unit cube.
    x2 = lerp(    grad (aba, xf  , yf-1, zf),           // This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
                grad (bba, xf-1, yf-1, zf),             // values we made earlier.
                  u);
    y1 = lerp(x1, x2, v);

    x1 = lerp(    grad (aab, xf  , yf  , zf-1),
                grad (bab, xf-1, yf  , zf-1),
                u);
    x2 = lerp(    grad (abb, xf  , yf-1, zf-1),
                  grad (bbb, xf-1, yf-1, zf-1),
                  u);
    y2 = lerp (x1, x2, v);

    return (lerp (y1, y2, w)+1)/2;
}

//get the linear interpolation color from a colormap
glm::vec3 getLerpColor(SO_ColorMap &map, float min, float max, float value) {
    value = (value - min)/(max - min); //normalise values
    if (value <= map.weights[0]) {
        return map.colors[0]; // if below min value return lowest value
    }
    for (int j = 1; j < map.length; j++) {
        if (value <= map.weights[j]) {
            return lerp(map.colors[j-1], map.colors[j], (value - map.weights[j-1])/(map.weights[j] - map.weights[j-1]));
        }
    }
    return map.colors[map.length-1];
}