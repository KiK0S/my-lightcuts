#pragma once
#include <glm/glm.hpp>
#include "Model.hpp"
#include "BRDF.hpp"

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
	void normalize() {
		direction /= glm::length(direction);
	}
};

struct RayHit {
	float t = -1;
	Ray ray;
	glm::vec3 normal;
	BRDF brdf;

	void update(const RayHit& other) {
		if (t == -1) {
			(*this) = other;
			return;
		}
		if (other.t == -1) {
			return;
		}
		
		if (other.t < t) {
			(*this) = other;
		}
	}
};


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