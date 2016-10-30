#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <memory>

using namespace glm;
using std::shared_ptr;

namespace Game_View
{
    /**
     * Struct that packs the data needed by
     * the rendering together into an easily
     * accessible format
     */
    struct VertexData
    {
        vec3 Position;
        vec3 Normal;
        vec2 UV;
        float MaterialIndex;
    };
}