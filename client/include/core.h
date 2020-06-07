//
//  core.h
//  Gaia
//
//  Created by Wenlin Mao on 4/18/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#ifndef _CORE_H_
#define _CORE_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include <assert.h>


#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#else
#include <SDL.h>
#include <SDL_image.h>
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <boost/range/iterator_range.hpp>
#include <SFML/Audio.hpp>
#include <boost/filesystem.hpp>


const float PI = glm::pi<float>();

#endif
