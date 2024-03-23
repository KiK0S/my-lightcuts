#include "LightCut.hpp"

#include <vector>
#include <set>
#include <memory>
#include <random>
#include <queue>

void LightTree::build(std::vector<std::shared_ptr<PointLight>> lights_) {
    this->lights = lights_;
    tree.resize(0);
    for (int i = 0; i < lights.size(); i++) {
        LightCutNode node;
        node.light_idx = i;
        node.intensity = lights[i]->intensity;
        node.box = BoundingBox3d {
            100000,
            -100000,
            100000,
            -100000,
            100000,
            -100000
        };
        node.box.update(lights[i]->getTranslation());
        tree.push_back(node);
    }
    std::vector<int> active_clusters(lights.size());
    std::iota(active_clusters.begin(), active_clusters.end(), 0);
    auto node = [&](int i) {
        return tree[active_clusters[i]];
    };
    auto score = [&](int i, int j) {
        BoundingBox3d bb = node(i).box;
        bb.update(node(j).box);
        auto dx = bb.x_max - bb.x_min;
        auto dy = bb.y_max - bb.y_min;
        auto dz = bb.z_max - bb.z_min;
        return (dx * dx + dy * dy + dz * dz)
                    * (lights[node(i).light_idx]->intensity + lights[node(j).light_idx]->intensity);
    };
    while (active_clusters.size() > 1) {
        int cur_i = 0;
        int cur_j = 1;
        double cur_tmp = 1e18;
        for (int i = 0; i < active_clusters.size(); i++) {
            for (int j = i + 1; j < active_clusters.size(); j++) {
                if (score(i, j) < cur_tmp) {
                    cur_i = i;
                    cur_j = j;
                    cur_tmp = score(i, j);
                }
            }
        }
        LightCutNode united;
        united.left_idx = active_clusters[cur_i];
        united.right_idx = active_clusters[cur_j];
        united.box = node(cur_i).box;
        united.box.update(node(cur_j).box);
        std::random_device rd;  // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<float> dist(0, node(cur_i).intensity + node(cur_j).intensity);
        if (dist(gen) < node(cur_i).intensity) {
            united.light_idx = node(cur_i).light_idx;
        } else {
            united.light_idx = node(cur_j).light_idx;
        }
        united.intensity = node(cur_i).intensity + node(cur_j).intensity;
        active_clusters.push_back(tree.size());
        tree.push_back(united);
        active_clusters.erase(active_clusters.begin() + cur_j);
        active_clusters.erase(active_clusters.begin() + cur_i);
    }
    // std::queue<int> q;
    // q.push(tree.size() - 1);
    // while (!q.empty()) {
    //     int v = q.front();
    //     q.pop();
    //     // std::cout << v << ' ';
    //     if (tree[v].left_idx == -1) {
    //         // std::cout << '\n';
    //         continue;
    //     }
    //     // std::cout << tree[v].left_idx << ' ' << tree[v].right_idx << '\n';
    //     q.push(tree[v].left_idx);
    //     q.push(tree[v].right_idx);
    // }
}

glm::vec3 LightTree::getLight(std::shared_ptr<PointLight> light, glm::vec3 position, const BRDF& brdf, BRDFArgs& args) {
    args.lightDir = light->getTranslation() - position;
    return brdf(args);    
}

std::shared_ptr<PointLight> LightTree::selectLightNode(LightCutNode node) {
    if (node.left_idx == -1) {
        return lights[node.light_idx];
    }
    // auto res = std::make_shared<PointLight>(*lights[node.light_idx]);
    // res->intensity = node.intensity;
    // return res;
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<float> dist(0, lights[tree[node.left_idx].light_idx]->intensity + lights[tree[node.right_idx].light_idx]->intensity);
    if (dist(gen) < lights[tree[node.left_idx].light_idx]->intensity) {
        auto res = selectLightNode(tree[node.left_idx]);
        res->intensity = node.intensity;
        return res;
    } else {
        auto res =  selectLightNode(tree[node.right_idx]);
        res->intensity = node.intensity;
        return res;
    }
}

std::vector<std::shared_ptr<PointLight>> LightTree::getLights(glm::vec3 position, const BRDF& brdf, BRDFArgs& args, bool print) {
    float PI = glm::pi<float>();
    int root = tree.size() - 1;
    auto estimate_error = [&](LightCutNode node) { 
        if (node.left_idx == -1) {
            return glm::vec3(-1.0f);
        }

        auto light = selectLightNode(node); 
        auto length = glm::length(light->getTranslation() - position);
        float g = 0;
        for (int i = 0; i < 3; i++)  {
            if (position[i] < node.box.min_axis(i)) {
                g += (node.box.min_axis(i) - position[i]) * (node.box.min_axis(i) - position[i]);
            } else if (position[i] > node.box.max_axis(i)) {
                g += (position[i] - node.box.max_axis(i)) * (position[i] - node.box.max_axis(i));
            }
        }
        if (g < 0.01f) {
            return glm::vec3(1e18f);
        }
        g = 1.0 / g;
        glm::vec3 diffuse = brdf.diffuse(args);

        BoundingBox3d bb = node.box;
        bb.x_min -= position[0];
        bb.x_max -= position[0];
        bb.y_min -= position[1];
        bb.y_max -= position[1];
        bb.z_min -= position[2];
        bb.z_max -= position[2];
        auto a = args.normal;
        auto b = glm::vec3(0.0, 0.0, 1.0);
        glm::vec3 axis = glm::cross(a, b);
        float angle = std::acos(glm::dot(glm::normalize(a), glm::normalize(b)));

        // Construct rotation matrix
        glm::mat3 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);

        if ((rotationMatrix * a).z < 0) {
            rotationMatrix = glm::rotate(glm::mat4(1.0f), -angle, axis);
        }

        auto min_tr = (rotationMatrix[0] * bb.p1());
        auto max_tr = (rotationMatrix[0] * bb.p2());
        bb.x_min = min_tr[0];
        bb.x_max = max_tr[0];
        bb.y_min = min_tr[1];
        bb.y_max = max_tr[1];
        bb.z_min = min_tr[2];
        bb.z_max = max_tr[2];

        auto z_max = std::max(bb.z_max, bb.z_min);
        if (z_max <= 0) return glm::vec3(0.0f);
        auto x_min = std::min(std::abs(bb.x_min), std::abs(bb.x_max));
        if (bb.x_max >= 0 && bb.x_min <= 0) x_min = 0.0;
        auto y_min = std::min(std::abs(bb.y_min), std::abs(bb.y_max));
        if (bb.y_max >= 0 && bb.y_min <= 0) y_min = 0.0;
        float dot_bound = z_max / std::sqrt(x_min * x_min + y_min * y_min + z_max * z_max);
        glm::vec3 brdf_diffuse = glm::vec3(1.0) / PI;
        // glm::vec3 m_specular =  = glm::vec3(1.0); * glm::dot(
        float v = 1.0f;
        glm::vec3 m = /*dot_bound  *  dot_bound  * */ brdf_diffuse;
        if (print)
            std::cout << length << ' ' << g << '\n';
        auto res = node.intensity * g * v * m;
        for (int i = 0; i < 3; i++) {
            res[i] = std::abs(res[i]);
        }
        return res;// + glm::vec3(1.0) * glm::dot(r, direction));  

        // float m = ;
        // glm::vec3 cur_estimate = getLight(lights[node.block_start], position, direction, normal, albedo);
        // glm::vec3 child_estimates = getLight(lights[tree[node.left_idx].block_start], position, direction, normal, albedo) + 
        //                         getLight(lights[tree[node.right_idx].block_start], position, direction, normal, albedo);
        // return 1.0 - glm::length(cur_estimate) / glm::length(child_estimates);
    };
    glm::vec3 illumination = getLight(selectLightNode(tree[root]), position, brdf, args);
    float coeff = 0.02;
    auto max_comp = [](glm::vec3 v) {
        return std::max(v[0], std::max(v[1], v[2]));
    };
    auto cmp = [&](int i, int j) {
        LightCutNode a = tree[i];
        LightCutNode b = tree[j];
        
        if (a.left_idx == -1) {
            if (b.left_idx == -1) {
                return i < j;
            }
            return false;
        }
        if (b.left_idx == -1) {
            return true;
        }
        auto ei = max_comp(estimate_error(a));
        auto ej = max_comp(estimate_error(b));
        if (ei == ej) {
            // node with less number is higher
            return i < j;
        }
        return ei > ej;
    };
    std::set<int, decltype(cmp)> s(cmp);
    s.insert(root);

    while(s.size() < 20) {
        
        int node = *s.begin();
        if (print) {
            std::cout << node << '\n';
        }
        if (tree[node].left_idx == -1) {
            // std::cout << "found leaf\n";
            break;
        }
        s.erase(node);
        auto err_est = estimate_error(tree[node]);
        if (err_est[0] <= coeff * illumination[0] && err_est[1] <= coeff * illumination[1] && err_est[2] <= coeff * illumination[2]) {
            break;
        }
        illumination -= getLight(selectLightNode(tree[node]), position, brdf, args);
        s.insert(tree[node].left_idx);
        s.insert(tree[node].right_idx);
        illumination += getLight(selectLightNode(tree[tree[node].left_idx]), position, brdf, args);
        illumination += getLight(selectLightNode(tree[tree[node].right_idx]), position, brdf, args);
    }
    std::vector<std::shared_ptr<PointLight>> res;
    if (print)
        std::cout << "===\n";
    for (int idx : s) {
        if (print)
            std::cout << tree[idx].light_idx << ' ';
        res.push_back(selectLightNode(tree[idx]));
    }
    if (print)
        std::cout << '\n';
    return res;
}