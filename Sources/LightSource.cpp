#include "LightSource.hpp"

float dothemi(glm::vec3 a, glm::vec3 b)
{
	return std::max(0.01f, glm::dot(a, b));
}

const float PI = glm::pi<float>();

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

glm::vec3 GetLight(glm::vec3 l, glm::vec3 c, glm::vec3 n, glm::vec3 v, glm::vec3 albedo, float roughness) {
	glm::vec3 h = normalize(l + v);
	// sample albedo
	glm::vec3 diffuse = albedo / PI;
	
	glm::vec3 F0{1.0f, 0.71, 0.29};
	F0 = F0 * 0.7f + 0.3f * diffuse;

	glm::vec3 F = FresnelSchlick(std::max(0.0f, dothemi(h, l)), F0);
	float D = DistributionGGX(n, h, roughness);
	float G = GeomShlick(n, l, v, roughness);
	glm::vec3 specular = F * D * G / 4.0f / dothemi(n, l) / dothemi(n, v); 
	return c * glm::dot(n, l) * (diffuse + specular); 
}