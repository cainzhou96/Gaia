//
//  TerrainBoundingBox.hpp
//  Gaia
//
//  Created by Cain on 5/9/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef TerrainBoundingBox_hpp
#define TerrainBoundingBox_hpp

#include <stdio.h>
#include "core.h"

class TerrainBoundingBox {
public:
    std::vector<int> indices2triangles;
    glm::vec2 minPoint;
    glm::vec2 maxPoint;
}; 

#endif /* TerrainBoundingBox_hpp */
