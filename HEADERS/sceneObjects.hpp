/** \file sceneObjects.hpp
 * \brief The main header file for the sceneObjects library
*/

#ifndef SCENEOBJECTS_H_   /* Include guard */
#define SCENEOBJECTS_H_

#define SO_VERSION_MAJOR 2
#define SO_VERSION_MINOR 1
#define SO_VERSION_REVISION 0

#define GLEW_STATIC

#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/** \namespace sceneObjects
 * \brief The namespace containing all classes, members, structs involved in the sceneObjects library
*/
namespace sceneObjects {

///generic shader program class
/**
 * The SO_Shader class is the base class for all shader classes in the sceneObjects library.
 * It provides the ability to create a program out of a vertex shader and fragment shader which must be provided by the user, 
 * as well as the ability to supply three matrix uniforms - intended for use in 3D projection
 * these must be of type mat4 in the glsl with names "model", "view", and "proj" for use with the setter functions
**/
class SO_Shader {
    GLuint programID; ///< The OpenGL program ID used to reference the program when performing tasks such as useProgram()
    GLuint vertexShaderID; ///< The OpenGL shader ID associated with the vertex shader
    GLuint fragmentShaderID; ///< The OpenGL shader ID associated with the fragment shader
    GLint modelMatrixLoc; ///< The OpenGL location of the mat4 uniform named "model" in the shader
    GLint viewMatrixLoc; ///< The OpenGL location of the mat4 uniform named "view" in the shader
    GLint projectionMatrixLoc; ///< The OpenGL location of the mat4 uniform named "proj" in the shader
    bool programCreated = false; ///< Whether a program has yet been successfully linked by the class
    public:
        ///The custom destructor for the SO_Shader class
        /**
         * The custom destructor for this class is implemented with non-default behaviour to destory the program that has been created. 
         * Note that any VAOs, VBOs, etc. are not managed by the class and are therefore not deleted - this must be handled by the user.
        **/
        ~SO_Shader(void);
        /// Creates a vertex shader for the shader program using the char* argument
        void createVertexShader(const char* vertexSource);
        /// Creates a fragment shader for the shader program using the char* input
        void createFragmentShader(const char* fragmentSource);
        /// Links the vertex and fragment shaders into a program
        void linkProgram(void);
        /// returns the OpenGL program ID associated with the shader program for use in functions like useProgram()
        GLuint getProgramID(void);
        /// Sets the mat4 "model" uniform in the shader
        /**
         * Setting the "model" uniform requires the uniform to have been defined within the shader. It is a requirement that 
         * shaders utilising the SO_Shader class implement a "model" uniform even if it is unused.
         * The model matrix transforms from model space to world space
        **/
        virtual void setModelMatrix(glm::mat4 modelMatrix);
        /// Sets the mat4 "view" uniform in the shader
        /**
         * Setting the "view" uniform requires the uniform to have been defined within the shader. It is a requirement that 
         * shaders utilising the SO_Shader class implement a "view" uniform even if it is unused.
         * The view matrix transforms from world space to camera space
        **/
        virtual void setViewMatrix(glm::mat4 viewMatrix);
        /// Sets the unused vec3 "viewPos" uniform in the shader - DOES NOTHING
        /**
         * Setting the "viewPos" uniform must be done outside the shader - this function is blank as of present. It is NOT a requirement that 
         * shaders utilising the SO_Shader class implement a "viewPos" uniform.
         * The viewPos gives the position of the camera in world space
        **/
        virtual void setViewPosition(glm::vec3 viewPosition);
        /// Sets the mat4 "proj" uniform in the shader
        /**
         * Setting the "proj" uniform requires the uniform to have been defined within the shader. It is a requirement that 
         * shaders utilising the SO_Shader class implement a "proj" uniform even if it is unused.
         *  The proj matrix transforms between camera space and projection space
        **/
        virtual void setProjectionMatrix(glm::mat4 projectionMatrix);
};

/// The options which can be passed into SO_PhongShader to control the exact behaviour
enum SO_ShaderOptions : unsigned int {
    /// Enables the alpha channel on the object
    /**
     * If enabled then an alpha value can be passed into the shader using setColor(vec4) or setMaterialAlpha if alpha is a uniform, 
     * or as a vertex attribute if it is not a vertex uniform. It is expected that objects with alpha values are rendered last and with careful culling settings.
     * for example the following code may be used
     * ```cpp
     * glDepthMask(GL_FALSE);
     * glCullFace(GL_FRONT); // render back of cube first - do not need polygon z sorting as cube is convex
     * glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);
     * glCullFace(GL_BACK);
     * glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);
     * glDepthMask(GL_TRUE);
     * ```
    **/
    SO_ALPHA = 1,
    /// Enables instanced rendering of an object
    /**
     * If enabled then the shader acts on instanced objects - it expects to recieve the matrix "postModel" as a uniform, 
     * as well as to recieve the "instanceMatrix" and "normalInst Matrix" as vertex attributes set to be instanced with
     * glVertexAttribDivisor. Additionally glDrawElementsInstanced should be called in preference to glDrawElements
    **/
    SO_INSTANCED = 2,
    /// Enables the use of material shaders rather than fixed colours
    /**
     * If enabled then the alternative rendering settings of ambient/diffuse/speccular are used rather than a default colour. 
     * Each of these can be set separately as either uniforms or vertex attributes using further shaderOptions. 
     * The material choice is separate from the ambient/diffuse/specular lighting colours - it provides additional variation in 
     * material under the same lighting
    **/
    SO_MATERIAL = 4,
    /// Enables the ambient colour to be passed as a vertex attribute as opposed to a uniform using setMaterialAmbient()
    SO_AMBIENT_ATTRIBUTE = 8,
    /// Enables the diffuse colour to be passed as a vertex attribute as opposed to a uniform using setMaterialDiffuse()
    SO_DIFFUSE_ATTRIBUTE = 16,
    /// Enables the specular colour to be passed as a vertex attribute as opposed to a uniform using setMaterialSpecular()
    SO_SPECULAR_ATTRIBUTE = 32,
    /// Enables the alpha colour to be passed as a vertex attribute in material mode as opposed to a uniform using setMaterialAlpha()
    SO_ALPHA_ATTRIBUTE = 64,
    /// Enables the colour to be passed as a vertex attribute in colour mode as opposed to a uniform using setColor()
    SO_COLOR_ATTRIBUTE = 128
};

/// 3D Phong model lighting class
/**
 * A class which provides a wide range of shaders for different circumstances - utilkising a basic phong shader model. 
 * For more advanced features such as normal mapping and specular mapping models with UV mapped textures can be uesd with SO_AssimpModel.
 * The functionaility required is selected by use of the SO_ShaderOptions enums.
**/
class SO_PhongShader : public SO_Shader {
    protected:
        int numberLights; ///< number of point light sources in the scene
        unsigned int options; ///< the options to be used - passed in as enums but stored here as an unsigned int
        GLint normalMatrixLoc; ///< The OpenGL location of the mat4 uniform named "normalMatrix" in the shader
        GLint viewPositionLoc; ///< The OpenGL location of the vec3 uniform named "viewPos" in the shader
        GLint postModelMatrixLoc; ///< The OpenGL location of the mat4 uniform named "postModel" in the shader
        GLint postNormalMatrixLoc; ///< The OpenGL location of the mat4 uniform named "postNormalMatrix" in the shader
        GLint ambientMatLoc; ///< The OpenGL location of the vec3 uniform named "ambientMat" in the shader
        GLint diffuseMatLoc; ///< The OpenGL location of the vec3 uniform named "diffuseMat" in the shader
        GLint specularMatLoc; ///< The OpenGL location of the vec3 uniform named "specularMat" in the shader
        GLint alphaMatLoc; ///< The OpenGL location of the float uniform named "alphaMat" in the shader
        GLint colorLoc; ///< The OpenGL location of the vec3/4 (depending on SO_Alpha) uniform named "Color" in the shader
        GLint specularPowerLoc; ///< The OpenGL location of the unsigned int uniform named "specPower" in the shader
        using SO_Shader::createVertexShader;
        using SO_Shader::createFragmentShader;
        using SO_Shader::linkProgram;
    public:
        /// the generate function creates the vertex and fragment shaders and links them into a program
        /**
         * The args are `numberLightsIn` which gives the number of point light sources in the scene, and 
         * `optionsIn` which is the SO_ShaderOptions enums.
        **/
        GLuint generate(int numberLightsIn=1, unsigned int optionsIn=0);
        /// An override of the base class method which also calculates the transformation for normal vectors and passes this into the shader. Applied in world space before instance matrix if using instancing.
        void setModelMatrix(glm::mat4 modelMatrix) override;
        /// The PostModelMatrix is the model matrix applied in world space after the instancing matrix - available only if instancing enabled
        void setPostModelMatrix(glm::mat4 modelMatrix);
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
        /// Set the materials ambient colour - available only if material mode enabled with ambient as uniforms
        void setMaterialAmbient(glm::vec3 ambientMaterial);
        /// Set the materials diffuse colour - available only if material mode enabled with diffuse as uniforms
        void setMaterialDiffuse(glm::vec3 diffuseMaterial);
        /// Set the materials specular colour - available only if material mode enabled with specular as uniforms
        void setMaterialSpecular(glm::vec3 specularMaterial);
        /// Set the materials alpha transparency - available only if material mode and alpha enabled as uniforms
        void setMaterialAlpha(float alphaMaterial);
        /// Set the color of the object - available only is material mode not enabled and alpha not enabled with colour as uniforms
        void setColor(glm::vec3 color);
        /// Set the color of the object - available only is material mode not enabled and alpha enabled with colour as uniforms
        void setColor(glm::vec4 color);
        /// Set the power of the specular highlights - higher integers produce 'sharper' spots
        void setSpecularPower(unsigned int specPower);
};

///Generate a skybox using 6 images which will appear in the background
/**
 * Generate a skybox using a vector of 6 images in order right (+x), left (-x), top (+y), bottom (-y), front (-z), back (+z)
 * max size 2048x2048 (some frames delay in render function if larger)
**/
class SO_SkyboxShader : public SO_Shader {
    protected:
        std::vector<std::string> imageFiles; ///< Stores the file locations of the 6 asset images
        GLuint textureID; ///< The OpenGL texture ID of the cubemap texture
        GLuint skyboxVAO; ///< The vertex array object ID for the shader
        GLuint skyboxVBO; ///< The vertex buffer object ID for the shader
        using SO_Shader::createVertexShader;
        using SO_Shader::createFragmentShader;
        using SO_Shader::linkProgram;
    public:
        ///The custom destructor for the SO_SkyboxShader class
        /**
         * The custom destructor for this class is implemented with non-default behaviour to destory the VAO/VBO that have been created. 
         * The parent SO_Shader destructor is called and destroys the program
        **/
        ~SO_SkyboxShader(void);
        /// An override of the base class method which removes translations as the skybox should be centred on the camera
        void setModelMatrix(glm::mat4 modelMatrix) override;
        /// An override of the base class method which removes translations as the skybox should be centred on the camera
        void setViewMatrix(glm::mat4 viewMatrix) override;
        /// the generate function creates the vertex and fragment shaders and links them into a program
        /**
         * The arg `imageFilesIn` is a vector of 6 image filepaths (as strings) to form the skybox in the order right (+x), left (-x), top (+y), bottom (-y), front (-z), back (+z)
        **/
        GLuint generate(std::vector<std::string> imageFilesIn);
        ///renders the skybox - should be last render done for efficiency
        /**
         * \warning this operation leaves the shader program and VAO set on the skybox after calling useProgram() and bindVertexArray() must be recalled
        **/
        void render(GLenum depthFuncReset = GL_LESS);
};


/// The container for a single vertex in an SO_ModelMesh
struct SO_ModelVertex {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); ///< The position of the vertex in modelSpace
    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f); ///< The normal to the surface in model space
    glm::vec2 texCoords = glm::vec2(0.0f, 0.0f); ///< The coords of the texture at this vertex 2D (u,v)
    glm::vec3 tangent = glm::vec3(0.0f, 0.0f, 0.0f); ///< The tangent vector lies along the u axis of the texture - the bitangent is not stored
};

/// Stores a texture for an SO_AssimpModel
struct SO_ModelTexture {
    GLuint textureId; ///< The OpenGL texture ID for the texture
    std::string path; ///< The filepath and filename for the texture
};

/// The shader for an SO_ModelMesh object
/**
 * This shader allows the use of diffuse, specular and normal maps if textures are provided
 * It is intended to be used with the SO_ModelMesh class - although this is not a requirement
**/
class SO_ModelShader : public SO_Shader {
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
class SO_ModelMesh {
        GLuint vbo; ///< The vertex buffer object for the mesh
        GLuint vao; ///< The vertex array object for the mesh
        GLuint ebo; ///< The lement buffer object for the mesh
    public:
        SO_ModelShader shader; ///< The shader program used to render this object - customised based on lighting choice and textures
        std::vector<SO_ModelVertex> vertices; ///< The vertices of the mesh - includes texture coordinates and normals/tangents
        std::vector<unsigned int> elements; ///< The elements used to construct triangular faces from the mesh
        std::vector<SO_ModelTexture> diffuseMaps; ///< The diffuse textures to be used for the mesh - currently only the first is used
        glm::vec3 diffuseColor = glm::vec3(0.0f, 0.0f, 0.0f); ///< The diffuse colour to be used if no diffuse texture is present
        std::vector<SO_ModelTexture> specularMaps; ///< The specular texture to be used for the mesh - currently only the first is used
        glm::vec3 specularColor = glm::vec3(0.0f, 0.0f, 0.0f); ///< The specular colour to be used if no specular texture is present
        std::vector<SO_ModelTexture> normalMaps; ///< The normal textures to be used for the object - currently only the first is used
        SO_ModelShader* createShader(int numberLights); ///< The function which creates the custom SO_ModelShader for this mesh
        ///renders the mesh
        /**
         * \warning this operation leaves the shader program and VAO set on the mesh shader after calling useProgram() and bindVertexArray() must be recalled
        **/
        void render();
        ///The custom destructor for the SO_ModelMesh class
        /**
         * The custom destructor for this class is implemented with non-default behaviour to destory the VAO/VBO that have been created. 
        **/
        ~SO_ModelMesh();
};

/// A 3D camera capable of creating view/projection matrices to linked shader programs on call
/**
 * This camera class is designed to allow multiple objects to be kept in the same scene by automatically updating
 * the relevant view and projection matrices with the updateViewMatrix() and updateProjectionMatrix() methods. 
 * Any derived class of SO_Shader can be added to the camera.
**/
class SO_Camera {
    std::vector<SO_Shader*> shaderPointers; ///< A vector of pointers to the shaders which must be updated
    public:
        float fov; ///< the field of view in degrees
        float aspectRatio; ///< The aspect ratio of the camera
        float nearClip; ///< The distance to the near clipping plane - fragments closer than this distance will not be rendered
        float farClip; ///< The distance to the far clipping plane - fragments further than this will not be rendered
        glm::vec3 position; ///< The position of the camera in world space
        glm::vec3 front; ///< A vec3 pointing in the direction of the centre of the camera's FOV
        glm::vec3 up; ///< A vec3 pointing in the upwards direction of the camera's frame
        /// Constructor for a camera object
        /**
         * `float fov` the field of view in degrees
         * `float aspectRatio` The aspect ratio of the camera
         * `float nearClip` The distance to the near clipping plane - fragments closer than this distance will not be rendered
         * `float farClip` The distance to the far clipping plane - fragments further than this will not be rendered
         * `glm::vec3 position` The position of the camera in world space
         * `glm::vec3 front` A vec3 pointing in the direction of the centre of the camera's FOV
         * `glm::vec3 up` A vec3 pointing in the upwards direction of the camera's frame
        **/
        SO_Camera(float fovIn=45.0f, float aspectRatioIn=1.0f, float nearClipIn=0.1f, float farClipIn=100.0, glm::vec3 positionIn=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 frontIn=glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 upIn=glm::vec3(0.0f, 1.0f, 0.0f));
        /// link a shader passed in by pointer to the camera object
        void linkShader(SO_Shader *shaderRefIn);
        /// unlink a shader passed in by pointer from the camera object
        void unlinkShader(SO_Shader *shaderRefIn);
        /// update the view matrices of all linked shaders to the cameras current settings
        void updateViewMatrix(void);
        /// update the projection (proj) matrices of all linked shaders to the cameras current settings
        void updateProjectionMatrix(void);
};

#ifdef _WIN32
///A class capable of rendering to a ffmpeg stream
/**
 * This class can render frames to a ffmpeg stream and hence to video
 * \warning This class is currently only available on Windows
 * \warning This class requires ffmpef to be installed
**/
class SO_FfmpegStream {
    std::string filepath; ///< The filepath and filename to stream into
    bool streaming = false; ///< Whether the object is currently streaming
    FILE*  ffmpeg; ///< The file object that is being streamed into
    int width; ///< The width (px) of the video
    int height; ///< The height (px) of the video
    int fps = 60; ///< The target fps of the video (may not match fps of interactive program unless designed in by user)
    std::unique_ptr<int[]> buffer; ///< A buffer to store the frame data passed out by OpenGL
    public:
        /// Constructor for an SO_FfmpegStream which takes in `string filepathIn` as the filepath and filename to stream into.
        SO_FfmpegStream(std::string filepathIn);
        /// Allows the filepath to be changed without creating a new stream object
        void setFilepath(std::string filepathIn);
        /// Begin streaming into file with resolution `widthIn`x`heightIn` at `fpsIn` frames per second. Note that fps is target and not enforced unless by user
        void openStream(int widthIn, int heightIn, int fpsIn = 60);
        /// Render the current OpenGL screen into a frame of the stream
        void renderFrame(void);
        /// Finish streaming into file and save the file
        void closeStream(void);

};
#else
#pragma message("WARNING: class SO_Ffmpeg is not supported on this OS, and is not available")
#endif


///struct containing data for a simple mesh which can be used or manipulated by the user
/**
 * struct consists of a vector of vec3s called `vertices` which are the vertices of the mesh, and a vector of ints `faceElements` which give the 
 * vertices in groups of three to be rendered as triangles by OpenGL
**/
struct SO_MeshData {
    std::vector<glm::vec3> vertices;
    std::vector<int> faceElements;
};

///creates a weighted sum of two vectors
/**
 * creates the weighted sum of vectors (`subdivisions`-`division`)/`subdivisions`*`vector1` + `division`/`subdivisions`*`vector2`
 * If vector1 and vector2 are position vectors then this gives the position vector to a point `division`/`subdivitions` from vector1 to vector2
**/
glm::vec3 createRatioVector(int subdivisions, int division, glm::vec3 vector1, glm::vec3 vector2);

///creates a sphere mesh beginning from an icosahedron
/**
 * Creates a sphere mesh by taking an icosahedron and dividing each edge into `subdivisions` sections. 
 * Then the faces of the icosahedron are split up from the cuts of the edges.
 * Finally all the vertices are normalised to s distance 1 from the origin
**/
SO_MeshData createIcosphere(int subdivisions);

/// Return 6t^5-15t^4+10t^3 - a smooth function between (0,0) and (1,1) with gradient 0 at each point
double fade(double x);
/// increments a number `num` looping around to 0 at the value of `repeat` (i.e. returns (num+1)%repeat if num >0) - not mod since it doesn't loop negative values
int inc(int num, int repeat);
/// gets the gradient of perlin corner (see algorithm)
double grad(int hash, double x, double y, double z);

/// linear interpolation for any type T with mulitplication an addition defined
/**
 * i.e. returns `(1-x)*a + x*b`
**/
template <typename T> T lerp(T a, T b, float x) {
    return a + (T)(x * (b - a));
}

/// a modulus function which is 'correct' about negatives
/**
 * returns `x - y*floor(x/y)`
**/
double modulus(double x, double y);

/// stores the table of permutations required to generate perlin noise
extern int perlinPerms[512];

/// generates perlin noise at coordinates `x`,`y`,`z`, with a repeat size of `repeat`
double perlin(double x, double y, double z, double repeat);

/// A class which allows easy creation and use of colormaps
/**
 * This class contains a collection of key float positions and RGB colours. Standard positions are expected to lie between 0 and 1 
 * however this is not required - positions outside this range represent values beyond the 'max' and 'min' range of the expected data.
 * The class contains various methods for creating a spectrum of colours between the key positions.
**/
class SO_ColorMap {
    private:
        std::map<float, glm::vec3> colors; ///< The map of colours and associated positions
    public:
        void setValue(float position, glm::vec3 color); ///< Set a keyColor and position in the map
        void deleteValue(float position); ///< Delete the keyColor at `position`. If no color is present at the position, there is no effect.
        std::vector<float> getPositions(); ///< Returns an ordered vector of the key positions within the map
        std::vector<glm::vec3> getColors(); ///< Returns an ordered vector of the key colours in the map - order corresponds with the vector returned by getPositions()
        /// Returns a color from a linear interpolation of the keyColors
        /**
         * The SO_ColorMap is used to create a smooth color gradient. The `min` and `max` variables are taken as the expected extremes of the data, 
         * and normalised laong with `value` to the range [0, 1]. If `value<min` or `value>max` then positions outside the range [0,1] can be accessed.
         * This is not forbidden behaviour. If the requested `value` lies between two known key positions after normalisation then the linear interpolation
         *  of the two key colors is returned. If the `value` beyond the minimum or maximum known key position then the minimum or maximum colors are returned.
         * If the SO_ColorMap is empty then `glm::vec3(0.0f, 0.0f, 0.0f)` is returned.
        **/
        glm::vec3 getLerpColor(float min, float max, float value);
};

GLuint loadTextureFromFile(std::string path); ///< Loads a texture from an image file - returns OpenGL texture ID

}
#endif // FOO_H_