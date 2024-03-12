#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>



class Material {
public:
    Material(glm::vec4 albedo, float shininess, float ka, float kd, float ks): albedo(albedo), shininess(shininess), ka(ka), kd(kd), ks(ks) {}

    glm::vec4 albedo;
    float shininess;
    float kd;
    float ka;
    float ks;
};
