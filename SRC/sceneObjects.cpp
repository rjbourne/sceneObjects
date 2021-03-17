#define STB_IMAGE_IMPLEMENTATION

#define GLEW_STATIC

#include "sceneObjects.hpp"
#include <stb_image.h>
#include <stdio.h>
#include <memory>
#include <cstdio>
#include <string>
#include <vector>
#include <stdexcept>
#include <exception>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>





