// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#pragma once

#include <string>
#include <memory>

#include "Mesh.h"

namespace MeshLoader {

/// Loads an OFF mesh file. See https://en.wikipedia.org/wiki/OFF_(file_format)
void loadOFF (const std::string & filename, std::shared_ptr<Mesh> meshPtr);

}
