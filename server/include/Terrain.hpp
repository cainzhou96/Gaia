//
//  Terrain.hpp
//  Gaia
//
//  Created by Wenlin Mao on 4/25/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef Terrain_hpp
#define Terrain_hpp

#include "core.h"
#include "TerrainMesh.hpp"
#include "TerrainBoundingBox.hpp"
#include "SDL2_gfxPrimitives.h"
#include "constant.h"
#include "time.h"
#include "util.h"

typedef struct {
   float x0, x1, y0, y1, z0, z1;
} ClipVolume;

class Terrain{
public:
    Terrain();
    Terrain(int width, int depth, float step);
    ~Terrain();
    
    void update (float deltaTime);
    void reset(int seed);

    void setHeightsFromTexture(const char *file, float offset, float scale);
    void terrainBuildMesh(std::vector<float> height);
    void computeBoundingBoxes(); // called once after building mesh for the first time
    float getHeightAt(float x, float z);
    
    std::vector<unsigned int>* getIndices();
    std::vector<glm::vec3>* getVertices();
    std::vector<glm::vec3>* getNormals();
    std::vector<TerrainBoundingBox>* getBoundingBoxes(); 
    
    std::vector<float> getHeightMap();
        
    void edit(std::vector<glm::vec2> editPoints, float h);
    void editPoint(const glm::vec2& point, float h);

    SDL_Surface* surface;
    SDL_Renderer* soft_renderer;

    TerrainMesh mesh;
    float getHeight(unsigned int w, unsigned int d);
    std::vector<float> height;
    void setHeight(unsigned int w, unsigned int d, float h);

private:
    unsigned num_indices;

    //GLuint VAO;
    //GLuint VBO_positions, VBO_normals, EBO;
    
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    std::vector<unsigned int> indices;
    std::vector<TerrainBoundingBox> boundingBoxes;
    
    std::vector<float> colorMap;
    int width;
    int depth;
    float step;

    float line_step = TERRAIN_STEP;
    float min_width = TERRAIN_MIN_WIDTH;
    float max_width = TERRAIN_MAX_WIDTH;

    glm::vec3 calculateNormal(unsigned x, unsigned z);
    void computeIndicesForClipVolume(ClipVolume *clip);
    
    void setHeightsFromSurface(float offset, float scale);

    void drawLineOnSurface(glm::vec2 start, glm::vec2 end, float color);
    void drawLineOnSDL(const glm::vec2& start, const glm::vec2& end, const int color);
    
    void putpixel(int x, int y, float color);

    glm::vec2 getClampedPoint(float max_width, const glm::vec2& point);
};

#endif /* Terrain_hpp */
