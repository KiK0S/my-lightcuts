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


bool rayTriangleIntersect(const Ray& ray, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, float& t);

glm::vec3 computeBarycentricCoordinates(const glm::vec3& point, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C);
