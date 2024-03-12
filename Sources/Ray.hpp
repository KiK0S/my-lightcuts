#pragma once
#include <glm/glm.hpp>
#include "Model.hpp"

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
	std::shared_ptr<Material> material;

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