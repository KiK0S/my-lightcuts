#include "BoundingBox.hpp"


std::ostream& operator << (std::ostream& out, const BoundingBox3d& box) {
    out << "([" << box.x_min << ", " << box.x_max << "], [" << box.y_min << ", " << box.y_max << "], [" << box.z_min << ", " << box.z_max << "])";
    return out;
}

float BoundingBox3d::min_axis(int idx) const {
    if (idx == 0) return x_min;
    if (idx == 1) return y_min;
    if (idx == 2) return z_min;
}
float BoundingBox3d::max_axis(int idx) const {
    if (idx == 0) return x_max;
    if (idx == 1) return y_max;
    if (idx == 2) return z_max;
}
glm::vec3 BoundingBox3d::p1() const {
    return glm::vec3(x_min, y_min, z_min);
}
glm::vec3 BoundingBox3d::p2() const {
    return glm::vec3(x_max, y_max, z_max);
}
int BoundingBox3d::longest_axis() const {
    float dx = x_max - x_min;
    float dy = y_max - y_min;
    float dz = z_max - z_min;
    if (dx >= dy && dx >= dz) {
        return 0;
    } else if (dy >= dz) {
        return 1;
    } else {
        return 2;
    }
    return -1;
}

std::pair<BoundingBox3d, BoundingBox3d> BoundingBox3d::partition() const {
    float dx = x_max - x_min;
    float dy = y_max - y_min;
    float dz = z_max - z_min;
    BoundingBox3d left_box = *this;
    BoundingBox3d right_box = *this;
    
    if (dx >= dy && dx >= dz) {
        left_box.x_max = x_min + dx / 2;
        right_box.x_min = x_min + dx / 2;
    } else if (dy >= dz) {
        left_box.y_max = y_min + dy / 2;
        right_box.y_min = y_min + dy / 2;
    } else {
        left_box.z_max = z_min + dz / 2;
        right_box.z_min = z_min + dz / 2;    
    }

    return std::make_pair(left_box, right_box);
}

// https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics/
bool BoundingBox3d::hasIntersection( const Ray& ray) const {
    float tx1 = (x_min - ray.origin.x) / ray.direction.x, tx2 = (x_max - ray.origin.x) / ray.direction.x;
    float tmin = std::min( tx1, tx2 ), tmax = std::max( tx1, tx2 );
    float ty1 = (y_min - ray.origin.y) / ray.direction.y, ty2 = (y_max - ray.origin.y) / ray.direction.y;
    tmin = std::max( tmin, std::min( ty1, ty2 ) ), tmax = std::min( tmax, std::max( ty1, ty2 ) );
    float tz1 = (z_min - ray.origin.z) / ray.direction.z, tz2 = (z_max - ray.origin.z) / ray.direction.z;
    tmin = std::max( tmin, std::min( tz1, tz2 ) ), tmax = std::min( tmax, std::max( tz1, tz2 ) );
    return tmax >= tmin && tmax > 0;
}

bool BoundingBox3d::contains(glm::vec3 pos) const {
    return x_min <= pos.x <= x_max &&
            y_min <= pos.y <= y_max &&
            z_min <= pos.z <= z_max;
}

bool BoundingBox3d::contains(const std::vector<glm::vec3>& positions) const {
    glm::vec3 x{0};
    for (auto p : positions) {
        x += p;
    }
    x /= positions.size();
    return contains(x);
}


void BoundingBox3d::update(glm::vec3 point) {
    x_min = std::min(x_min, point.x);
    x_max = std::max(x_max, point.x);
    y_min = std::min(y_min, point.y);
    y_max = std::max(y_max, point.y);
    z_min = std::min(z_min, point.z);
    z_max = std::max(z_max, point.z);
}

void BoundingBox3d::update(const std::vector<glm::vec3>& positions) {
    for (auto p : positions) {
        update(p);
    }
}

void BoundingBox3d::update(const BoundingBox3d& other) {
    x_min = std::min(x_min, other.x_min);
    x_max = std::max(x_max, other.x_max);
    y_min = std::min(y_min, other.y_min);
    y_max = std::max(y_max, other.y_max);
    z_min = std::min(z_min, other.z_min);
    z_max = std::max(z_max, other.z_max);
}
