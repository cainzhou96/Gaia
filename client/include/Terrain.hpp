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
#include "constant.h"
#include "TerrainMesh.hpp"
#include "TerrainBoundingBox.hpp"
#include "SDL2_gfxPrimitives.h"
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
    void draw(const glm::mat4& view, const glm::mat4& projection,
            const glm::vec3& campos, GLuint shader);
    void multiTextureDraw(const glm::mat4& view, const glm::mat4& projection,
        const glm::vec3& campos, GLuint shader);
    void reset(int seed);

    void setHeightsFromTexture(const char *file, float offset, float scale);
    void terrainBuildMesh(std::vector<float> height);
    void computeBoundingBoxes(); // called once after building mesh for the first time
    float getHeightAt(float x, float z);

    std::vector<unsigned int>* getIndices();
    std::vector<glm::vec3>* getVertices();
    std::vector<glm::vec3>* getNormals();
    std::vector<TerrainBoundingBox>* getBoundingBoxes();

    void edit(std::vector<glm::vec2> editPoints, float h);
    void editPoint(const glm::vec2& point, float h);

    SDL_Surface * surface;
    SDL_Renderer* soft_renderer;

    TerrainMesh mesh;

    float getHeight(unsigned int w, unsigned int d);
    std::vector<float> height;


private:
    unsigned num_indices;

    GLuint VAO;
    GLuint VBO_positions, VBO_normals, VBO_texCoords, EBO;

    unsigned int grassTexture;
    unsigned int rockTexture;
    unsigned int cracksTexture;
    unsigned int heightMapTexture;
    unsigned int snowTexture;

    //SDL_BlendMode subtractBlendMode;

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    std::vector<unsigned int> indices;
    std::vector<TerrainBoundingBox> boundingBoxes;

    int width;
    int depth;
    float step;

    float line_step = TERRAIN_STEP;
    float min_width = TERRAIN_MIN_WIDTH;
    float max_width = TERRAIN_MAX_WIDTH;

    float SpecularPower = 300.0; //! slider[10, 300, 1000]
    glm::vec3 Ka = glm::vec3(0.7, 0.7, 0.7); //! color[0.7, 0.7, 0.7]
    glm::vec3 Kd = glm::vec3(1.0, 1.0, 1.0); //! color[(0.9, 0.8, 1.0]
    glm::vec3 Ks = glm::vec3(0.5, 0.5, 0.5); //! color[0.7, 0.7, 0.7]
    glm::vec3 Ke = glm::vec3(0.0, 0.0, 0.0); //! color[0.7, 0.7, 0.7]
    glm::vec3 baseColor = glm::vec3(0.7); //! color[0.7, 0.7, 0.7]

    //! <group name="Light Properties"/>
    glm::vec3 LightPosition = glm::vec3(125.5f, 50.0, -125.5f);
    glm::vec3 Ia = glm::vec3(0.3, 0.3, 0.3); //! color[0.3, 0.3, 0.3]
    glm::vec3 Id = glm::vec3(0.7, 0.7, 0.7); //! color[1.0, 1.0, 1.0]
    glm::vec3 Is = glm::vec3(0.01, 0.01, 0.01); //! color[0.7, 0.7, 0.7]

    //! <group name="Border Properties"/>
    glm::vec3 borderColor = glm::vec3(0.0); //! color[0.0, 0.0, 0.0]
    float borderTolerance = 0.005; //! slider[0.0001, 0.5, 0.9999]

    //! <group name="Color and Specular Steps"/>
    int colorSteps = 5; //! slider[1, 5, 30]
    int specularSteps = 10; //! slider[1, 5, 30]


    //! <group name="Usage Properties"/>
    bool manualCamPos = true;  //! checkbox[false]
    bool shading = true;  //! checkbox[true]
    bool usePhongBlinn = true; //! checkbox[true]
    bool seeOutLine = true; //! checkbox[true]
    bool seeColorSteps = true; //! checkbox[false]
    bool seeSpecularSteps = true; //! checkbox[true]


    void textureFromSurface(SDL_Surface* surface);
    void prepareDraw();
    void setHeight(unsigned int w, unsigned int d, float h);
    glm::vec3 calculateNormal(unsigned x, unsigned z);
    void computeIndicesForClipVolume(ClipVolume *clip);

    void setHeightsFromSurface(float offset, float scale);

    void drawLineOnSurface(glm::vec2 start, glm::vec2 end, float color);
    void drawLineOnSDL(const glm::vec2& start, const glm::vec2& end, const int color);

    void putpixel(int x, int y, float color);

    glm::vec2 getClampedPoint(float max_width, const glm::vec2& point);
};

#endif /* Terrain_hpp */
