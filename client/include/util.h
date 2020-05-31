//
//  util.h
//  Gaia
//
//  Created by Wenlin Mao on 5/31/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef util_h
#define util_h

#include "core.h"

static unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    GLenum texture_format;
    GLint  nOfColors;
    
    // TODO: only works for PNG textures, use JPG, need RGB
    SDL_Surface *surface;

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        std::cout << "Could not initialize STL Image" << SDL_GetError() <<std::endl;
        return textureID;
    }
    
    surface = IMG_Load(filename.c_str());
    
    if (surface)
    {
        // get the number of channels in the SDL surface
        nOfColors = surface->format->BytesPerPixel;
        if( nOfColors == 4 )     // contains an alpha channel
        {
            if(surface->format->Rmask == 0x000000ff)
                texture_format = GL_RGBA;
            else
                texture_format = GL_BGRA;
        }
        else if( nOfColors == 3 )     // no alpha channel
        {
            if(surface->format->Rmask == 0x000000ff)
                texture_format = GL_RGB;
            else
                texture_format = GL_BGR;
        }
        else
        {
            printf("warning: the image is not truecolor..  this will probably break\n");
            // this error should not go unhandled
        }
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, texture_format, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
    } else {
        printf("SDL could not load bag.png: %s\n", SDL_GetError());
        SDL_Quit();
    }


    return textureID;
}


static unsigned int loadCubemapJPG(const std::vector<std::string>& faces)
{
    GLenum texture_format;
    GLint  nOfColors;
    
    SDL_Surface *surface;

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)){
        std::cout << "Could not initialize STL Image" << SDL_GetError() <<std::endl;
        return textureID;
    }
    
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        surface = IMG_Load(faces[i].c_str());
        if (surface) {
            
            // get the number of channels in the SDL surface
            nOfColors = surface->format->BytesPerPixel;
            if( nOfColors == 4 )     // contains an alpha channel
            {
                if(surface->format->Rmask == 0x000000ff)
                    texture_format = GL_RGBA;
                else
                    texture_format = GL_BGRA;
            }
            else if( nOfColors == 3 )     // no alpha channel
            {
                if(surface->format->Rmask == 0x000000ff)
                    texture_format = GL_RGB;
                else
                    texture_format = GL_BGR;
            }
            else
            {
                printf("warning: the image is not truecolor..  this will probably break\n");
                // this error should not go unhandled
            }
            
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, texture_format, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
            
            
        } else {
            printf("SDL could not load bag.png: %s\n", SDL_GetError());
            SDL_Quit();
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    if ( surface ) {
        SDL_FreeSurface( surface );
    }

    return textureID;
}


static unsigned int loadCubemapPNG(const std::vector<std::string>& faces)
{
    GLenum texture_format;
    GLint  nOfColors;
    
    SDL_Surface *surface;
    
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        std::cout << "Could not initialize STL Image" << SDL_GetError() <<std::endl;
        return textureID;
    }
    
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        surface = IMG_Load(faces[i].c_str());
        if (surface) {
            // get the number of channels in the SDL surface
            nOfColors = surface->format->BytesPerPixel;
            if( nOfColors == 4 )     // contains an alpha channel
            {
                if(surface->format->Rmask == 0x000000ff)
                    texture_format = GL_RGBA;
                else
                    texture_format = GL_BGRA;
            }
            else if( nOfColors == 3 )     // no alpha channel
            {
                if(surface->format->Rmask == 0x000000ff)
                    texture_format = GL_RGB;
                else
                    texture_format = GL_BGR;
            }
            else
            {
                printf("warning: the image is not truecolor..  this will probably break\n");
                // this error should not go unhandled
            }
            
            
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, texture_format, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
            
            
        } else {
            printf("SDL could not load bag.png: %s\n", SDL_GetError());
            SDL_Quit();
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    if ( surface ) {
        SDL_FreeSurface( surface );
    }

    return textureID;
}

#endif /* util_h */
