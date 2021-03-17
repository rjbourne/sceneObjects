#include "sceneObjects.hpp"

// craetes a shader for the mesh
sceneObjects::SO_AssimpShader* sceneObjects::SO_AssimpMesh::createShader(int numberLights) {
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
void sceneObjects::SO_AssimpMesh::draw() {
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

sceneObjects::SO_AssimpMesh::~SO_AssimpMesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}
