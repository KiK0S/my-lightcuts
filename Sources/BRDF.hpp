#pragma once
#include "Material.hpp"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/ext.hpp>
#include <memory>

const float PI = glm::pi<float>();

float dothemi(glm::vec3 a, glm::vec3 b);

float DistributionGGX(glm::vec3 N, glm::vec3 H, float roughness);

glm::vec3 FresnelSchlick(float cosTheta, glm::vec3 F0);

float GeomShlick(glm::vec3 N, glm::vec3 w0, glm::vec3 wi, float roughness);



struct BRDFArgs {
    glm::vec3 normal;
    glm::vec3 cameraDir;
    glm::vec3 lightDir;
};

struct BRDF {
    BRDF(std::shared_ptr<Material> material): material(material) {}
    BRDF(): material() {}

    glm::vec3 operator ()(const BRDFArgs& args) const {
        return material->kd * diffuse(args) + material->ks * specular(args);
    }

    glm::vec3 diffuse(const BRDFArgs& args) const {
        return glm::vec3(material->albedo) / PI;
    }

    glm::vec3 specular(const BRDFArgs& args) const {
        glm::vec3 r = glm::dot(args.lightDir, args.normal) * 2 * args.normal - args.lightDir;
        return glm::vec3(1.0) * glm::dot(r, args.cameraDir);
    }

    glm::vec3 advanced_specular(const BRDFArgs& args) const {
        glm::vec3 h = normalize(args.lightDir + args.cameraDir);
        // sample albedo
        glm::vec3 diffuse = glm::vec3(material->albedo) / PI;
        
        glm::vec3 F0{1.0f, 0.71, 0.29};
        F0 = F0 * 0.7f + 0.3f * diffuse;

        glm::vec3 F = FresnelSchlick(std::max(0.0f, dothemi(h, args.lightDir)), F0);
        float D = DistributionGGX(args.normal, h, material->roughness);
        float G = GeomShlick(args.normal, args.lightDir, args.cameraDir, material->roughness);
        return F * D * G / 4.0f / dothemi(args.normal, args.lightDir) / dothemi(args.normal, args.cameraDir);
    }


    std::shared_ptr<Material> material;
};


