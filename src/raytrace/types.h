// raytrace/types.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// basic types

#pragma once

#include <cstdint>
#include <limits>
#include <memory>

#include "glm.h"

namespace rtiow {

using color_t = glm::vec3;
using point_t = glm::vec3;
using vector_t = glm::vec3;

using material_id_t = size_t;

} // namespace rtiow
