#include "Material.hpp"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "BRDF.hpp"

float dothemi(glm::vec3 a, glm::vec3 b)
{
	return std::max(0.01f, glm::dot(a, b));
}


float DistributionGGX(glm::vec3 N, glm::vec3 H, float roughness) {
    float a2 = roughness * roughness;
    float NdotH = dothemi(N, H);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

glm::vec3 FresnelSchlick(float cosTheta, glm::vec3 F0) {
    return F0 + (glm::vec3(1.0) - F0) * std::pow(1.0f - cosTheta, 5.0f);
}

float GeomShlick(glm::vec3 N, glm::vec3 w0, glm::vec3 wi, float roughness) {
    float a0 = dothemi(N, w0);
    float ai = dothemi(N, wi);
    float k = roughness * std::sqrt(2.0f / PI);
    return a0 * ai / (a0 * (1.0f - k) + k) / (ai * (1.0f- k) + k);
}


