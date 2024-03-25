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
    glm::vec3 error_bound{0.0f};
    glm::vec3 light{0.0f};
    int last_updated = 0;
    int last_updated_light = 0;
};

struct LightTree {

    LightTree() {}

    void build(std::vector<std::shared_ptr<PointLight>> lights);
    glm::vec3 getLight(LightCutNode node, glm::vec3 position, const BRDF& brdf, BRDFArgs& args);
    std::shared_ptr<PointLight> selectLightNode(LightCutNode node, bool map_intensity, double rnd = -1);
    std::vector<std::shared_ptr<PointLight>> getLights(glm::vec3 position, const BRDF& brdf, BRDFArgs& args, bool print = false);


    std::vector<LightCutNode> tree;
    std::vector<std::shared_ptr<PointLight>> lights;
    std::vector<int> s;
    int timer = 0;
    bool enable_sampling = false;
};