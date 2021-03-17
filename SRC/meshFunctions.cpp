#include "sceneObjects.hpp"

//create a sphere mesh starting with an icosohedron base (an icosphere)
//subdivisions is the number of divisions along each edge of the starting icosahedron
sceneObjects::SO_MeshData sceneObjects::createIcosphere(int subdivisions) {
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
            glm::vec3 newVector = sceneObjects::createRatioVector(subdivisions, division, vertices[edges[edge][0]], vertices[edges[edge][subdivisions]]);;
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
                    glm::vec3 newVector = sceneObjects::createRatioVector(row+1, slice, vertices[edges[faces[face][0]][row+1]], vertices[edges[faces[face][1]][row+1]]);
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

    sceneObjects::SO_MeshData icosphereData;
    icosphereData.vertices = vertices;
    icosphereData.faceElements = faceElements;
    return icosphereData;

}