#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "Transform.h"
#include "Model.hpp"


class PointLight : public Transform {
public:
    PointLight(glm::vec3 position, glm::vec3 color, float intensity = 1.0f): color(color), intensity(intensity) {
        setTranslation(position);
    }
    glm::vec3 color;
    float intensity;
};

class DirectionalLight {
public:
    DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity = 1.0f): direction(direction), color(color), intensity(intensity) {}
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
};