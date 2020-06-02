//
//  TerrainMesh.cpp
//  Gaia
//
//  Created by Wenlin Mao on 4/26/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "TerrainMesh.hpp"

TerrainMesh::TerrainMesh(){
    vertices = std::vector<glm::vec3>();
    normals = std::vector<glm::vec3>();
}

TerrainMesh::~TerrainMesh(){
}

void TerrainMesh::addVertex(float x, float y, float z,
                    float nx, float ny, float nz, float tx,  float tz)
{
   glm::vec3 v = glm::vec3(x, y, z);
   glm::vec3 n = glm::vec3(nx, ny, nz);
   glm::vec2 t = glm::vec2(tx, tz);
   vertices.push_back(v);
   normals.push_back(n);
   texCoord.push_back(t);
}

//void TerrainMesh::convertLowPoly(unsigned int ul, unsigned int ur,
//                                 unsigned int ll, unsigned int lr){
//    glm::vec3 upperEdge = vertices[ul] - vertices[ur];
//    glm::vec3 diagonal
//}
