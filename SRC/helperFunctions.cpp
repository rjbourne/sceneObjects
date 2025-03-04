/** \file helperFunctions.cpp */
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "sceneObjects.hpp"


//create vector at a ratio of division/subdivisions from vector1 to vector2
glm::vec3 sceneObjects::createRatioVector(int subdivisions, int division, glm::vec3 vector1, glm::vec3 vector2) {
    return glm::vec3((float)(subdivisions-division)*vector1 + (float)division*vector2)/(float)(subdivisions);
}

//a gradual step function between (0,0) and (1,1)
double sceneObjects::fade(double x) {
    if (x <= 0) {
        return 0;
    } else if (x >= 1) {
        return 1;
    }
    return x*x*x*(x*(x*6 - 15) + 10); //return 6t^5-15t^4+10t^3 smooth between (0,0) and (1,1)
}

//increments around a loop
int sceneObjects::inc(int num, int repeat) {
    num++;
    if (repeat > 0) {
        num %= repeat;
    }
    return num;
}

//get gradient of perlin corner
double sceneObjects::grad(int hash, double x, double y, double z) {
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

//modulus function without negatives
double sceneObjects::modulus(double x, double y) {
    return x - y*floor(x/y);
}

int sceneObjects::perlinPerms[512] = { 151,160,137,91,90,15,                 // Hash lookup table as defined by Ken Perlin.  This is a randomly
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
double sceneObjects::perlin(double x, double y, double z, double repeat) { //https://adrianb.io/2014/08/09/perlinnoise.html
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
    aaa = sceneObjects::perlinPerms[sceneObjects::perlinPerms[sceneObjects::perlinPerms[    xi ]        +    yi ]           +    zi ];
    aba = sceneObjects::perlinPerms[sceneObjects::perlinPerms[sceneObjects::perlinPerms[    xi ]        +inc(yi, repeat)]   +    zi ];
    aab = sceneObjects::perlinPerms[sceneObjects::perlinPerms[sceneObjects::perlinPerms[    xi ]        +    yi ]           +inc(zi, repeat)];
    abb = sceneObjects::perlinPerms[sceneObjects::perlinPerms[sceneObjects::perlinPerms[    xi ]        +inc(yi, repeat)]   +inc(zi, repeat)];
    baa = sceneObjects::perlinPerms[sceneObjects::perlinPerms[sceneObjects::perlinPerms[inc(xi, repeat)]+    yi ]           +    zi ];
    bba = sceneObjects::perlinPerms[sceneObjects::perlinPerms[sceneObjects::perlinPerms[inc(xi, repeat)]+inc(yi, repeat)]   +    zi ];
    bab = sceneObjects::perlinPerms[sceneObjects::perlinPerms[sceneObjects::perlinPerms[inc(xi, repeat)]+    yi ]           +inc(zi, repeat)];
    bbb = sceneObjects::perlinPerms[sceneObjects::perlinPerms[sceneObjects::perlinPerms[inc(xi, repeat)]+inc(yi, repeat)]   +inc(zi, repeat)];

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

//loads texture files into openGL
GLuint sceneObjects::loadTextureFromFile(std::string path) {
    std::string filename = path;

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
        std::string error = "Unable to load texture at path: " + filename;
        throw std::runtime_error(error.c_str());
    }
    stbi_image_free(data);

    return textureID;
}