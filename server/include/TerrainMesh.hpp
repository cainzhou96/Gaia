//
//  TerrainMesh.hpp
//  Gaia
//
//  Created by Wenlin Mao on 4/26/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef TerrainMesh_hpp
#define TerrainMesh_hpp

#include "core.h"

class TerrainMesh{
public:
    TerrainMesh();
    ~TerrainMesh();
    
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    
    void addVertex(float x, float y, float z,
                   float nx, float ny, float nz);
};

#endif /* TerrainMesh_hpp */
