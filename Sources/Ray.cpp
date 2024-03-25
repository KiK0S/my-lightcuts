#include "Ray.hpp"

bool rayTriangleIntersect(const Ray& ray, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, float& t) {
    const float EPSILON = 0.000001;
    glm::vec3 edge1, edge2, h, s, q;
    float a, f, u, v;

    edge1 = p1 - p0;
    edge2 = p2 - p0;
    h = glm::cross(ray.direction, edge2);
    a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON)
        return false;

    f = 1.0f / a;
    s = ray.origin - p0;
    u = f * glm::dot(s, h);

    if (u < 0.0f || u > 1.0f)
        return false;

    q = glm::cross(s, edge1);
    v = f * glm::dot(ray.direction, q);

    if (v < 0.0f || u + v > 1.0f)
        return false;

    t = f * glm::dot(edge2, q);
    if (t > EPSILON) {
		return true;
	}

    return false;
}

glm::vec3 computeBarycentricCoordinates(const glm::vec3& point, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) {
    glm::vec3 v0 = B - A;
    glm::vec3 v1 = C - A;
    glm::vec3 v2 = point - A;

    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);

    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float v = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float w = (dot00 * dot12 - dot01 * dot02) * invDenom;
    float u = 1.0f - v - w;

    return glm::vec3(u, v, w);
}
