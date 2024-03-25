#pragma once
#include <utility>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include "Ray.hpp"


struct BoundingBox3d {
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float z_min;
    float z_max;

    float min_axis(int idx) const;
    float max_axis(int idx) const;
    glm::vec3 p1() const;
    glm::vec3 p2() const;
    BoundingBox3d afterRotation(glm::mat3 rotation) const;
    int longest_axis() const;
    std::pair<BoundingBox3d, BoundingBox3d> partition() const;
    bool hasIntersection( const Ray& ray) const;

    bool contains(glm::vec3 pos) const;
    bool contains(const std::vector<glm::vec3>& positions) const;

    void update(glm::vec3 point);
    void update(const std::vector<glm::vec3>& positions);
    void update(const BoundingBox3d& other);
};

std::ostream& operator << (std::ostream& out, const BoundingBox3d& box);
