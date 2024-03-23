#pragma once
#include "BVH.hpp"
#include "BRDF.hpp"
#include "LightSource.hpp"
#include <vector>
#include <set>
#include <queue>

struct LightCutNode {
    int left_idx = -1;
    int right_idx = -1;
    BoundingBox3d box;
    int light_idx = -1;
    float intensity = 1.0f;
};

struct LightTree {

    LightTree() {}

    void build(std::vector<std::shared_ptr<PointLight>> lights);
    glm::vec3 getLight(std::shared_ptr<PointLight> light, glm::vec3 position, const BRDF& brdf, BRDFArgs& args);
    std::shared_ptr<PointLight> selectLightNode(LightCutNode node);
    std::vector<std::shared_ptr<PointLight>> getLights(glm::vec3 position, const BRDF& brdf, BRDFArgs& args, bool print = false);


    std::vector<LightCutNode> tree;
    std::vector<std::shared_ptr<PointLight>> lights;
};