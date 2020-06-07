//
//  Terrain.cpp
//  Gaia
//
//  Created by Wenlin Mao on 4/25/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "Terrain.hpp"
#define BOUNDING_BOX_STEP 20

Terrain::Terrain(){
}

Terrain::Terrain(int width, int depth, float step) : width(width), depth(depth), step(step){
    
    height.resize(width * depth, 0);
    Uint32 rmask, gmask, bmask, amask;

    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif
    surface = SDL_CreateRGBSurface(0, width, depth, 32, rmask, gmask, bmask, amask);

    if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
    }

    soft_renderer = SDL_CreateSoftwareRenderer(surface);
    SDL_SetRenderDrawColor(soft_renderer, 127, 127, 127, 255);
    SDL_RenderClear(soft_renderer);

    setHeightsFromSurface(0.0f, TERRAIN_SCALE);
    terrainBuildMesh(height);
}

Terrain::~Terrain(){
}

float Terrain::getHeight(unsigned int w, unsigned int d)
{
    auto index = (w)*depth + (d);

    if (index >= height.size()) {
        return 0;
    }

    return height[index];
}

std::vector<float> Terrain::getHeightMap()
{
    return height;
}

void Terrain::setHeight(unsigned int w, unsigned int d, float h)
{
    height[(w) * depth + (d)] = h;
}

float Terrain::getHeightAt(float x, float z)
{
   /* Terrain's Z extends towards -Z, but our vertices need positive numbers */
   z = -z;

   /* Find offsets of the coords into a terrain quad */
   float offx = fmodf(x, step);
   float offz = fmodf(z, step);

   /* Compute the plane equation for the triangle we are in */
   glm::vec3 p1, p2, p3;
   float A, B, C, D;
   if (offx + offz <= step) {
      /* First triangle in the quad */
      p1.x = trunc(x / step);
      p1.z = trunc(z / step);
      p1.y = getHeight((int)p1.x, (int)p1.z);

      p2.x = trunc(x / step) + 1;
      p2.z = trunc(z / step);
      p2.y = getHeight((int)p2.x, (int)p2.z);

      p3.x = trunc(x / step);
      p3.z = trunc(z / step) + 1;
      p3.y = getHeight((int)p3.x, (int)p3.z);
   } else {
      /* Second triangle in the quad */
      p1.x = trunc(x / step) + 1;
      p1.z = trunc(z / step);
      p1.y = getHeight((int)p1.x, (int)p1.z);

      p2.x = trunc(x / step);
      p2.z = trunc(z / step) + 1;
      p2.y = getHeight((int)p2.x, (int)p2.z);

      p3.x = trunc(x / step) + 1;
      p3.z = trunc(z / step) + 1;
      p3.y = getHeight((int)p3.x, (int)p3.z);
   }

   /* Above we compute X,Z coords as vertex indices so we could use TERRAIN()
    * to compute heights at specific vertices, but to apply the plane equation
    * we need to turn the coordinates into world units
    */
   p1.x *= step;
   p1.z *= step;
   p2.x *= step;
   p2.z *= step;
   p3.x *= step;
   p3.z *= step;

   /* FIXME: we probably want to pre-compute plane equations for each
    * triangle in the terrain rather than recomputing them all the time
    */
   A = (p2.y - p1.y) * (p3.z - p1.z) - (p3.y - p1.y) * (p2.z - p1.z);
   B = (p2.z - p1.z) * (p3.x - p1.x) - (p3.z - p1.z) * (p2.x - p1.x);
   C = (p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y);
   D = -(A * p1.x + B * p1.y + C * p1.z);

   /* Use the plane equation to find Y given (X,Z) */
   return (-D - C * z - A * x) / B;
}


glm::vec3 Terrain::calculateNormal(unsigned x, unsigned z)
{
   if (x == 0)
      x = 1;
   if (z == 0)
      z = 1;
   float hl = getHeight(x - 1, z);
   float hr = getHeight(x + 1, z);
   float hd = getHeight(x, z + 1); /* Terrain expands towards -Z */
   float hu = getHeight(x, z - 1);
   glm::vec3 n = glm::vec3(hl - hr, 2.0f, hd - hu);
   n = glm::normalize(n);
   return n;
}

void Terrain::terrainBuildMesh(std::vector<float> h)
{
    height = h;
    /* GL's +Z axis goes towards the camera, so make the terrain's Z coordinates
     * negative so that larger (negative) Z coordinates are more distant.
     */
    mesh = TerrainMesh();
    
    int vertices_w = width;
    int vertices_d = depth;
    
    /* Add each vertex in the grid. We are going to render using an index buffer
     * so we don't need to duplicate vertices, which reduces massively the
     * storage requirements for the vertex data, since terrains have high
     * vertex counts.
     */
    for (int vx = 0; vx < vertices_w; vx++) {
        for (int vz = 0; vz < vertices_d; vz++) {
            float vy = getHeight(vx, vz);
            glm::vec3 v0 = glm::vec3(vx * step, vy, -vz * step);
            glm::vec3 n0 = calculateNormal(vx, vz);
            mesh.addVertex(v0.x, v0.y, v0.z, n0.x, n0.y, n0.z);
        }
    }

    indices.clear();
    for (int i = 0; i < vertices_w - 1; i++){
        for (int j = 0; j < vertices_d - 1; j++){
            unsigned int upperLeftIdx = i * vertices_w + j;
            unsigned int upperRightIdx = i * vertices_w + (j + 1);
            unsigned int lowerLeftIdx = (i + 1) * vertices_w + j;
            unsigned int lowerRightIdx = (i + 1) * vertices_w + (j + 1);
            
            indices.insert(indices.end(), {lowerLeftIdx, upperRightIdx, upperLeftIdx, lowerLeftIdx, lowerRightIdx, upperRightIdx});
        }
    }
    
    //    /* Build the indices to render the terrain using a single triangle strip
    //    * (using degenerate triangles) since that yields much better performance
    //    * than rendering triangles.
    //    */
    //    unsigned numIdx =
    //      (vertices_w - 1) * (vertices_d * 2) + (vertices_w - 2) + (vertices_d - 2);
    //    indices.resize(numIdx, 0);
    //
    //    /* Initialize the number of rendering indices so it covers the entire
    //    * terrain.
    //    */
    //    ClipVolume clip;
    //    clip.x0 = 0.0f;
    //    clip.x1 = (width - 1) * step;
    //    clip.z1 = 0.0f;
    //    clip.z0 = -(depth - 1) * step;
    //    computeIndicesForClipVolume(&clip);
    //
    //    assert(numIdx = num_indices);
}

void Terrain::computeIndicesForClipVolume(ClipVolume *clip)
{
    int min_col = std::max(std::min((clip->x0 / step), width - 2.0f), 0.0f);
    int max_col = std::max(std::min((clip->x1 + step) / step, width - 2.0f), 0.0f);

   /* Z1 is always largest, because our terrain grows towrds -Z, it means that
    * it defines the std::min_row
    */
    int min_row = std::max(std::min(-clip->z1 / step, depth - 1.0f), 0.0f);
    int max_row = std::max(std::min(- (clip->z0 - step) / step, depth - 1.0f), 0.0f);

   /* If this happens then the terrain is not visible */
   if (min_col == max_col || min_row == max_row) {
      num_indices = 0;
      return;
   }

   unsigned index = 0;
   for (int c = min_col; c <= max_col; c++) {
      for (int r = min_row; r <= max_row; r++) {
         /* If this is not the first strip then we need to produce a degenerate
          * link with the previous strip using the first vertex from this strip
          * and the last vertex from the last before we start recording the
          * new strip. Here is the first vertex of this strip.
          */
         if (c > min_col && r == min_row)
            indices[index++] = c * depth + r;

         indices[index++] = c * depth + r;
         indices[index++] = (c + 1) * depth + r;

         /* Link the next triangle strip using degenerate triangles. For that
          * we need to duplicate the last vertex of this strip and the first
          * vertex of the next.
          */
         if (r == max_row && c < max_col)
            indices[index++] = (c + 1) * depth + r;
      }
   }

   num_indices = index;
}

void Terrain::setHeightsFromTexture(const char *file, float offset, float scale)
{
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        std::cout << "Could not initialize STL surface" << SDL_GetError() <<std::endl;
        return;
    }

    surface = IMG_Load(file);
    
    if (!surface) {
      std::cout << "Texture Manager: Failed to load file, " << file << std::endl;
      return;
    }
    
    uint8_t *pixels = (uint8_t *) surface->pixels;
    float scale_x = ((float) surface->w) / (width - 1);
    float scale_z = ((float) surface->h) / (depth - 1);

    for (int x = 0; x < width; x++) {
        for (int z = 0; z < depth; z++) {
            int img_x = (int) truncf(x * scale_x);
            int img_y = (int) truncf(z * scale_z);
            float h = pixels[img_y * surface->pitch + img_x * 4];

            /* Normalize height to [-1, 1] */
            h = h / 127.5 - 1.0f;

            /* Apply scale */
            h *= scale;

            /* Apply height offset */
            h += offset;

            setHeight(x, z, h);
        }
    }
}

std::vector<unsigned int>* Terrain::getIndices() {
    return &indices;
}

std::vector<glm::vec3>* Terrain::getVertices() {
    return &mesh.vertices;
}

std::vector<glm::vec3>* Terrain::getNormals() {
    return &mesh.normals;
}

std::vector<TerrainBoundingBox>* Terrain::getBoundingBoxes() {
    return &boundingBoxes;
}


void Terrain::computeBoundingBoxes() {
    std::vector<unsigned int>* indices = this->getIndices();
    std::vector<glm::vec3>* vertices = this->getVertices();
    
    for (int vx = 0; vx < width; vx+=BOUNDING_BOX_STEP) {
        for (int vz = 0; vz < depth; vz+=BOUNDING_BOX_STEP) {
            TerrainBoundingBox box;
            box.minPoint = glm::vec2(vx * step, -(vz+BOUNDING_BOX_STEP) * step);
            box.maxPoint = glm::vec2((vx+BOUNDING_BOX_STEP) * step, -vz * step);
            
            for (int i = 2; i < indices->size(); i+=3) {
                glm::vec3& a = (*vertices)[(*indices)[i-2]];
                glm::vec3& b = (*vertices)[(*indices)[i-1]];
                glm::vec3& c = (*vertices)[(*indices)[i]];

                if ((a.x >= box.minPoint.x && a.x < box.maxPoint.x && a.z >= box.minPoint.y && a.z < box.maxPoint.y) ||
                    (b.x >= box.minPoint.x && b.x < box.maxPoint.x && b.z >= box.minPoint.y && b.z < box.maxPoint.y) ||
                    (c.x >= box.minPoint.x && c.x < box.maxPoint.x && c.z >= box.minPoint.y && c.z < box.maxPoint.y)) {
                    box.indices2triangles.push_back(i);
                }
            }
            
            boundingBoxes.push_back(box);
        }
    }
}

void Terrain::setHeightsFromSurface(float offset, float scale)
{
    uint8_t* pixels = (uint8_t*)surface->pixels;
    float scale_x = ((float)surface->w) / (width - 1);
    float scale_z = ((float)surface->h) / (depth - 1);

    for (int x = 0; x < width; x++) {
        for (int z = 0; z < depth; z++) {
            int img_x = (int)truncf(x * scale_x);
            int img_y = (int)truncf(z * scale_z);
            float h = pixels[img_y * surface->pitch + img_x * 4];

            /* Normalize height to [-1, 1] */
            h = h / 127 - 1.0f;

            /* Apply scale */
            h *= scale;

            /* Apply height offset */
            h += offset;

            setHeight(x, z, h);
        }
    }
}

void Terrain::drawLineOnSurface(glm::vec2 start, glm::vec2 end, float color){
    int x0 = start.x;
    int y0 = start.y;
    int x1 = end.x;
    int y1 = end.y;
    
    int dx, dy, p, x, y;
    
    dx = x1 - x0;
    dy = y1 - y0;
    
    x = x0;
    y = y0;
    
    p = 2 * dy - dx;
    
    while(x < x1)
    {
        if(p >= 0)
        {
            putpixel(x,y,color);
            y = y+1;
            p = p + 2 * dy - 2 * dx;
        }
        else
        {
            putpixel(x,y,color);
            p = p + 2 * dy;
        }
        x = x + 1;
    }
}

void Terrain::putpixel(int x, int y, float color){
    uint8_t *pixels = (uint8_t *) surface->pixels;
    float scale_x = ((float) surface->w) / (width - 1);
    float scale_z = ((float) surface->h) / (depth - 1);
    int img_x = (int) truncf(x * scale_x);
    int img_y = (int) truncf(y * scale_z);
    
    //color /= 2;
    int radius = 2;
    
    
    for (int i=-radius ; i<radius ; i++) {
        for(int j=-radius; j<radius; j++) {
            if((i*i + j*j)<(radius*radius)){
                int x_coord = std::min(std::max(0, img_x + j), surface->w-1);
                int y_coord = std::min(std::max(0, img_y + i), surface->h-1);
                
                uint32_t pixel = pixels[y_coord * surface->pitch + x_coord * 4];
                uint8_t r, g, b = 127;
                
                SDL_GetRGB( pixel, surface->format ,  &r, &g, &b );
                
                r = std::min(std::max(0.0f,r + color), 255.0f);
                g = std::min(std::max(0.0f,g + color), 255.0f);
                b = std::min(std::max(0.0f,b + color), 255.0f);
                
                pixels[y_coord * surface->pitch + x_coord * 4] = SDL_MapRGB(surface->format, r, g, b);
            }
        }
    }
}


glm::vec2 Terrain::getClampedPoint(float max_width, const glm::vec2& point) {
    int point_x = std::min(std::max(5.0f, point.x), width - 5.0f - 1.0f);
    int point_y = std::min(std::max(5.0f, point.y), depth - 5.0f - 1.0f);

    float scale_x = ((float)surface->w) / (width - 1);
    float scale_z = ((float)surface->h) / (depth - 1);

    point_x = (int)truncf(point_x * scale_x);
    point_y = (int)truncf(point_y * scale_z);

    return glm::vec2(point_x, point_y);
}

void Terrain::drawLineOnSDL(const glm::vec2& start, const glm::vec2& end, const int color) {

    glm::vec2 startDraw = getClampedPoint(max_width, start);
    glm::vec2 endDraw = getClampedPoint(max_width, end);

    for (float i = line_step - 1.0f; i >= 0.0f; i--) {
        float f = (line_step - i) / line_step;
        int res_color = (float)color / line_step;
        int width = max_width * (1.0f - f) + min_width;
        if (res_color > 0)
            thickLineRGBA(soft_renderer, (int)startDraw.x, (int)startDraw.y, (int)endDraw.x, (int)endDraw.y, width, res_color, res_color, res_color, 255, SDL_BLENDMODE_ADD);
        else {
            res_color = 127.0f + (float)color * f;
            thickLineRGBA(soft_renderer, (int)startDraw.x, (int)startDraw.y, (int)endDraw.x, (int)endDraw.y, width, res_color, res_color, res_color, 255, SDL_BLENDMODE_NONE);
        }
    }
}

void Terrain::edit(std::vector<glm::vec2> editPoints, float h)
{

    std::cout << "test" << std::endl;
    int color = h / 10 * 127;
    for (int i = 0; i < editPoints.size() - 1; i++) {
        drawLineOnSDL(editPoints[i], editPoints[i + 1], color);
    }

    //IMG_SavePNG(surface, "out.png");

    setHeightsFromSurface(0.0f, TERRAIN_SCALE);
    terrainBuildMesh(height);
}


void Terrain::editPoint(const glm::vec2& point, float h) {
    int color = h / 10 * 127;

    glm::vec2 centerDraw = getClampedPoint(max_width, point);

    for (float i = line_step - 1.0f; i >= 0.0f; i--) {
        float f = (line_step - i) / line_step;
        int res_color = (float)color / line_step;
        int width = max_width * (1.0f - f) + min_width;
        if (res_color > 0)
            filledCircleRGBA(soft_renderer, (int)centerDraw.x, (int)centerDraw.y, width, res_color, res_color, res_color, 255, SDL_BLENDMODE_ADD);
        else {
            res_color = 127.0f + (float)color * f;
            filledCircleRGBA(soft_renderer, (int)centerDraw.x, (int)centerDraw.y, width, res_color, res_color, res_color, 255, SDL_BLENDMODE_NONE);
        }

    }

    //IMG_SavePNG(surface, "out.png");

    setHeightsFromSurface(0.0f, TERRAIN_SCALE);
    terrainBuildMesh(height);
}

void Terrain::update(float deltaTime) {
}

void Terrain::reset(int seed) {
    std::srand(seed);

    SDL_SetRenderDrawColor(soft_renderer, 127, 127, 127, 255);
    SDL_RenderClear(soft_renderer);

    std::vector<glm::vec2> out_wall = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 251.f),
        glm::vec2(251.f, 251.f),
        glm::vec2(251.f, 0.0f),
        glm::vec2(0.0f, 0.0f)
    };
    float h = 10;
    int color = h / 10 * 127;
    for (int i = 0; i < out_wall.size() - 1; i++) {
        drawLineOnSDL(out_wall[i], out_wall[i + 1], color);
    }

    h = PEN_HEIGHT;
    color = h / 10 * 127;
    
    for (int i = 0; i < RANDOM_GENERATE_COUNT; i++) {
        std::vector<glm::vec2> line = {
            glm::vec2(rndFloat(max_width, width - max_width),
                rndFloat(max_width, width - max_width)),
            glm::vec2(rndFloat(max_width, width - max_width),
                rndFloat(max_width, width - max_width))
        };
        drawLineOnSDL(line[0], line[1], color);
    }

    //IMG_SavePNG(surface, "out.png");

    setHeightsFromSurface(0.0f, TERRAIN_SCALE);
    terrainBuildMesh(height);
}