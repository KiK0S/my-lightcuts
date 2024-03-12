#pragma once

#include "Mesh.h"
#include "Material.hpp"
#include <memory>
#include <glad/glad.h>

class Model {
public:
    Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material): mesh(mesh), material(material) {}

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
};