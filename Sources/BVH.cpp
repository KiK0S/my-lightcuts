#include "BVH.hpp"


std::ostream& operator << (std::ostream& out, const BoundingBox3d& box) {
    out << "([" << box.x_min << ", " << box.x_max << "], [" << box.y_min << ", " << box.y_max << "], [" << box.z_min << ", " << box.z_max << "])";
    return out;
}
