//
//  Model.hpp
//  Gaia
//
//  Created by Wenlin Mao on 5/31/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef Model_hpp
#define Model_hpp
#include "core.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "util.h"
#include "constant.h"


using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(GLuint shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to stream
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to stream
             else if(name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to stream

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // render data
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
};


class Model
{
public:
    // model data
    vector<Texture> textures_loaded;    // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;
    
    glm::vec3 start_loc = glm::vec3(0.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 center = glm::vec3(0.0f);;
    glm::vec3 scale_factor = glm::vec3(1.0f);
    glm::mat4 rotation = glm::mat4(1.0f);
    float size = COIN_SIZE;

    // constructor, expects a filepath to a 3D model.
    Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw(GLuint shader)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    virtual void update() {
        resetModel();
        applyScale();
        applyRotate();
        applyTranslate();
    }

    /* TRANSFORMATION FUNCTIONS HERE */
    // to maintain the object logic, moved sclaing to light class for light sources
    void scale(float s) {

        scale_factor *= s;
    }

    /* TRANSFORMATION FUNCTIONS HERE */
    // to maintain the object logic, moved sclaing to light class for light sources
    void scale(const glm::vec3& s) {

        scale_factor *= s;
    }

    void rotate(float angle, glm::vec3 rotAxis) {
        // CHANGE THIS LATER!!!!!!!!!
        // prevent undefined rot Axis
        //if (rotAxis.x < 0.0001f || rotAxis.y < 0.0001f || rotAxis.z < 0.0001f) {
        //    return;
        //}
        //model = glm::rotate(model, glm::radians(angle), rotAxis);
        rotation = glm::rotate(rotation, angle, rotAxis);
    }

    void translate(glm::vec3 tVec) {
        // Keep track of object center
        center += glm::vec3(tVec);
    }

    void translate(float tx, float ty, float tz) {
        // Keep track of object center
        center += glm::vec3(tx, ty, tz);
    }

    // reset location
    void resetPos() {
        translate(-center);
        translate(start_loc);
        center = start_loc;
    }

    // reset scale and rotation
    void resetScaleAndOrientation() {
        // Invert the current transformation matrix
        scale_factor = glm::vec3(1.0f);
        model = glm::mat4(1.0f);
        // Save the old position as we reset to the origin
        glm::vec3 pos = center;
        center = glm::vec3(0, 0, 0);

        // Move back translate
        translate(pos);
    }

    void move(const glm::vec3& pos) {
        center = pos;
    }

    void applyScale() {

        glm::vec4 x = glm::vec4(scale_factor.x, 0, 0, 0);
        glm::vec4 y = glm::vec4(0, scale_factor.y, 0, 0);
        glm::vec4 z = glm::vec4(0, 0, scale_factor.z, 0);
        glm::vec4 w = glm::vec4(0, 0, 0, 1.0f);

        glm::mat4 scaleMat = glm::mat4(x, y, z, w);

        //std::cout << "scale factor here" << scale_factor << std::endl;

        model = scaleMat * model;

    }

    void applyTranslate() {

        glm::vec4 x = glm::vec4(1.f, 0, 0, 0);
        glm::vec4 y = glm::vec4(0, 1.f, 0, 0);
        glm::vec4 z = glm::vec4(0, 0, 1.f, 0);
        glm::vec4 w = glm::vec4(center, 1.f);

        glm::mat4 transMat = glm::mat4(x, y, z, w);

        model = transMat * model;
    }

    void applyRotate() {
        model = rotation * model;
    }

    void resetModel() {
        // clear any rotation or scale, restart from the start_loc.
        // since we did canonicalization when parsing
        model = glm::mat4(1.0f);
    }

    void setSize(float s) {
        size = s;
    }
    
private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene)
    {
        // process each mesh located at the current node
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene.
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        float min_x, max_x, min_y, max_y, min_z, max_z;
        float diff_x, diff_y, diff_z;
        float diff;
        
        // itnitialize x, y, z values
        min_x = mesh->mVertices[0].x;
        max_x = mesh->mVertices[0].x;
        min_y = mesh->mVertices[0].y;
        max_y = mesh->mVertices[0].y;
        min_z = mesh->mVertices[0].z;
        max_z = mesh->mVertices[0].z;
        
        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            
            if (mesh->mVertices[i].x < min_x) {
                min_x = mesh->mVertices[i].x;
            }
            else if (mesh->mVertices[i].x > max_x) {
                max_x = mesh->mVertices[i].x;
            }
            if (mesh->mVertices[i].y < min_y) {
                min_y = mesh->mVertices[i].y;
            }
            else if (mesh->mVertices[i].y > max_y) {
                max_y = mesh->mVertices[i].y;
            }
            if (mesh->mVertices[i].z < min_z) {
                min_z = mesh->mVertices[i].z;
            }
            else if (mesh->mVertices[i].z > max_z) {
                max_z = mesh->mVertices[i].z;
            }
            
            // normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
            vertices.push_back(vertex);
        }
        
        diff_x = max_x - min_x;
        diff_y = max_y - min_y;
        diff_z = max_z - min_z;
    
        diff = diff_x > diff_y ? diff_x : diff_y;
        diff = diff_z > diff ? diff_z : diff;
        
        this->scale_factor = glm::vec3(size / diff);
        
        this->start_loc.x = min_x + (max_x - min_x) / 2.0f;
        this->start_loc.y = min_y + (max_y - min_y) / 2.0f;
        this->start_loc.z = min_z + (max_z - min_z) / 2.0f;
        
        this->center = start_loc;

        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        
        // TODO: temperory texture loading
        if (textures.size() == 0){
            // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile("300coin.png", this->directory);
            texture.type = typeName;
            texture.path = "300coin.png";
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
        
        return textures;
    }
};

#endif /* Model_hpp */
